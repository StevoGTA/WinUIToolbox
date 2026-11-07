//----------------------------------------------------------------------------------------------------------------------
//	WindowHelper.cpp			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "WindowHelper.h"

#include "ButtonHelper.h"

#include "winrt\Microsoft.UI.Windowing.h"
#include "winrt\Microsoft.UI.Xaml.Controls.h"
#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt\Microsoft.UI.Xaml.Input.h"
#include "winrt\Microsoft.UI.Xaml.Media.h"
#include "winrt\Windows.Foundation.h"
#include "winrt\Windows.Foundation.Collections.h"
#include "winrt\Windows.System.h"
#include "winrt\Windows.UI.h"
#include "microsoft.ui.xaml.window.h"

#include <cmath>

using AppWindow = winrt::Microsoft::UI::Windowing::AppWindow;
using AppWindowChangedEventArgs = winrt::Microsoft::UI::Windowing::AppWindowChangedEventArgs;
using Button = winrt::Microsoft::UI::Xaml::Controls::Button;
using Color = winrt::Windows::UI::Color;
using FrameworkElement = winrt::Microsoft::UI::Xaml::FrameworkElement;
using Grid = winrt::Microsoft::UI::Xaml::Controls::Grid;
using IInspectable = winrt::Windows::Foundation::IInspectable;
using KeyboardAccelerator = winrt::Microsoft::UI::Xaml::Input::KeyboardAccelerator;
using KeyboardAcceleratorPlacementMode = winrt::Microsoft::UI::Xaml::Input::KeyboardAcceleratorPlacementMode;
using OverlappedPresenter = winrt::Microsoft::UI::Windowing::OverlappedPresenter;
using PointerRoutedEventArgs = winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs;
using Popup = winrt::Microsoft::UI::Xaml::Controls::Primitives::Popup;
using RoutedEventArgs = winrt::Microsoft::UI::Xaml::RoutedEventArgs;
using SizeChangedEventArgs = winrt::Microsoft::UI::Xaml::SizeChangedEventArgs;
using SizeInt32 = winrt::Windows::Graphics::SizeInt32;
using SolidColorBrush = winrt::Microsoft::UI::Xaml::Media::SolidColorBrush;
using VirtualKey = winrt::Windows::System::VirtualKey;
using WindowEventArgs = winrt::Microsoft::UI::Xaml::WindowEventArgs;
using XamlRoot = winrt::Microsoft::UI::Xaml::XamlRoot;
using XamlRootChangedEventArgs = winrt::Microsoft::UI::Xaml::XamlRootChangedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: OwnedWindowController

class OwnedWindowController {
	public:
				OwnedWindowController(Window owner, Window child, bool modal, double scrimOpacity,
						WindowHelper::Placement placement, std::function<void()> commitProc,
						std::function<void()> cancelProc) :
					mOwner(owner), mChild(child), mModal(modal), mScrimOpacity(scrimOpacity),
							mPlacement(placement), mCommitProc(commitProc), mCancelProc(cancelProc)
					{}

		void	present();

	private:
		void	reposition() const;
		void	installScrim();
		void	updateScrimSize();
		void	removeScrim();
		void	wireButtons();
		void	teardown();

	private:
				Window					mOwner;
				Window					mChild;
				bool					mModal;
				double					mScrimOpacity;
				WindowHelper::Placement	mPlacement;
				std::function<void()>	mCommitProc;
				std::function<void()>	mCancelProc;

				HWND					mOwnerHWND = nullptr;
				HWND					mChildHWND = nullptr;

				XamlRoot				mOwnerXamlRoot{nullptr};
				Popup					mScrimPopup{nullptr};
				Grid					mScrimGrid{nullptr};

				winrt::event_token		mOwnerChangedToken{};
				winrt::event_token		mOwnerXamlRootChangedToken{};
				FrameworkElement		mChildContent{nullptr};
				winrt::event_token		mChildSizeChangedToken{};
};

//----------------------------------------------------------------------------------------------------------------------
void OwnedWindowController::present()
{
	// Window handles
	mOwnerHWND = WindowHelper(mOwner).getWindowHandle();
	mChildHWND = WindowHelper(mChild).getWindowHandle();

	// Make the child an OWNED window of the owner (governs z-order / taskbar / minimize only - no size or position
	// limit).  GWLP_HWNDPARENT (-8) sets the OWNER despite the "PARENT" name.
	::SetWindowLongPtrW(mChildHWND, GWLP_HWNDPARENT, (LONG_PTR) mOwnerHWND);

	// Remove the child's title bar - it is moved only with the owner.  Keep a border so it reads as a distinct window,
	//	and keep the content size (removing the title bar would otherwise hand its height to the content).
	AppWindow			childAppWindow = mChild.AppWindow();
	OverlappedPresenter	overlappedPresenter = childAppWindow.Presenter().try_as<OverlappedPresenter>();
	if (overlappedPresenter != nullptr) {
		// Remove title bar
		SizeInt32	clientSize = childAppWindow.ClientSize();
		overlappedPresenter.SetBorderAndTitleBar(true, false);
		childAppWindow.ResizeClient(clientSize);
	}

	// Wire optional Commit / Cancel buttons if present in the child content.
	wireButtons();

	// Install the modal scrim over the owner content.
	if (mModal)
		installScrim();

	// Follow the owner as it is moved or resized.
	mOwnerChangedToken =
			mOwner.AppWindow().Changed([this](const AppWindow&, const AppWindowChangedEventArgs& args){
				// Reposition on move or resize
				if (args.DidPositionChange() || args.DidSizeChange()) {
					reposition();
					updateScrimSize();
				}
			});

	// Re-center when the child content changes size (e.g. once the minimum content size takes effect).
	mChildContent = mChild.Content().try_as<FrameworkElement>();
	if (mChildContent != nullptr)
		mChildSizeChangedToken =
				mChildContent.SizeChanged([this](const IInspectable&, const SizeChangedEventArgs&){ reposition(); });

	// Tear down when the child closes - reverts the owner UI and self-deletes.
	mChild.Closed([this](const IInspectable&, const WindowEventArgs&){ teardown(); });

	// Retain the child past this scope (registry) and show it.
	WindowHelper::note(mChild);
	mChild.Activate();

	// Initial placement.
	reposition();
}

//----------------------------------------------------------------------------------------------------------------------
void OwnedWindowController::reposition() const
{
	// Setup
	RECT	ownerRect;
	RECT	childRect;
	::GetWindowRect(mOwnerHWND, &ownerRect);
	::GetWindowRect(mChildHWND, &childRect);

	int	childWidth = childRect.right - childRect.left;
	int	x = ownerRect.left + ((ownerRect.right - ownerRect.left) - childWidth) / 2;
	int	y;
	if (mPlacement == WindowHelper::Placement::kCenteredInOwner) {
		// Centered vertically within the owner
		int	childHeight = childRect.bottom - childRect.top;
		y = ownerRect.top + ((ownerRect.bottom - ownerRect.top) - childHeight) / 2;
	} else {
		// Top of the owner's client area (just below the title bar), in screen coordinates
		POINT	ownerClientOrigin = {0, 0};
		::ClientToScreen(mOwnerHWND, &ownerClientOrigin);
		y = ownerClientOrigin.y;
	}

	::SetWindowPos(mChildHWND, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

//----------------------------------------------------------------------------------------------------------------------
void OwnedWindowController::installScrim()
{
	// Setup
	UIElement	ownerContent = mOwner.Content();
	if (ownerContent == nullptr)
		return;

	mOwnerXamlRoot = ownerContent.XamlRoot();
	if (mOwnerXamlRoot == nullptr)
		return;

	// Scrim: a dark, hit-testable overlay that beeps when clicked (blocks input to the owner content beneath).
	Color	scrimColor;
	scrimColor.A = (uint8_t) (mScrimOpacity * 255.0);
	scrimColor.R = 0x00;
	scrimColor.G = 0x00;
	scrimColor.B = 0x00;

	mScrimGrid = Grid();
	mScrimGrid.Background(SolidColorBrush(scrimColor));
	mScrimGrid.PointerPressed([](const IInspectable&, const PointerRoutedEventArgs& pointerRoutedEventArgs){
		// Bong - the owner content is disabled while the owned window is showing.
		::MessageBeep(MB_OK);
		pointerRoutedEventArgs.Handled(true);
	});

	// Present through a Popup anchored to the owner XamlRoot - covers the whole content regardless of its root type
	// (the standard title bar is OS non-client area, so it stays draggable).
	mScrimPopup = Popup();
	mScrimPopup.XamlRoot(mOwnerXamlRoot);
	mScrimPopup.Child(mScrimGrid);
	updateScrimSize();
	mScrimPopup.IsOpen(true);

	// Keep the scrim sized to the owner content
	mOwnerXamlRootChangedToken =
			mOwnerXamlRoot.Changed([this](const XamlRoot&, const XamlRootChangedEventArgs&){ updateScrimSize(); });
}

//----------------------------------------------------------------------------------------------------------------------
void OwnedWindowController::updateScrimSize()
{
	// Size the scrim to the owner content
	if ((mScrimGrid == nullptr) || (mOwnerXamlRoot == nullptr))
		return;

	winrt::Windows::Foundation::Size	size = mOwnerXamlRoot.Size();
	mScrimGrid.Width(size.Width);
	mScrimGrid.Height(size.Height);
}

//----------------------------------------------------------------------------------------------------------------------
void OwnedWindowController::removeScrim()
{
	// Revoke the XamlRoot changed subscription
	if ((mOwnerXamlRootChangedToken.value != 0) && (mOwnerXamlRoot != nullptr)) {
		mOwnerXamlRoot.Changed(mOwnerXamlRootChangedToken);
		mOwnerXamlRootChangedToken = {};
	}

	// Close the Popup
	if (mScrimPopup != nullptr) {
		mScrimPopup.IsOpen(false);
		mScrimPopup.Child(nullptr);
		mScrimPopup = nullptr;
	}
	mScrimGrid = nullptr;
}

//----------------------------------------------------------------------------------------------------------------------
void OwnedWindowController::wireButtons()
{
	// Setup
	FrameworkElement	content = mChild.Content().try_as<FrameworkElement>();
	if (content == nullptr)
		return;

	// Commit button (optional)
	if (mCommitProc) {
		Button	commitButton = content.FindName(L"mCommitButton").try_as<Button>();
		if (commitButton != nullptr) {
			HWND					childHWND = mChildHWND;
			std::function<void()>	commitProc = mCommitProc;
			ButtonHelper(commitButton).setClickedProc([childHWND, commitProc](){
				// Commit then close (closing reverts the owner UI)
				commitProc();
				::PostMessageW(childHWND, WM_CLOSE, 0, 0);
			});

			// Enter presses it
			KeyboardAccelerator	keyboardAccelerator;
			keyboardAccelerator.Key(VirtualKey::Enter);
			commitButton.KeyboardAccelerators().Append(keyboardAccelerator);
			commitButton.KeyboardAcceleratorPlacementMode(KeyboardAcceleratorPlacementMode::Hidden);
		}
	}

	// Cancel button (optional)
	if (mCancelProc) {
		Button	cancelButton = content.FindName(L"mCancelButton").try_as<Button>();
		if (cancelButton != nullptr) {
			HWND					childHWND = mChildHWND;
			std::function<void()>	cancelProc = mCancelProc;
			ButtonHelper(cancelButton).setClickedProc([childHWND, cancelProc](){
				// Cancel then close
				cancelProc();
				::PostMessageW(childHWND, WM_CLOSE, 0, 0);
			});

			// Escape presses it
			KeyboardAccelerator	keyboardAccelerator;
			keyboardAccelerator.Key(VirtualKey::Escape);
			cancelButton.KeyboardAccelerators().Append(keyboardAccelerator);
			cancelButton.KeyboardAcceleratorPlacementMode(KeyboardAcceleratorPlacementMode::Hidden);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void OwnedWindowController::teardown()
{
	// Revoke owner follow
	if (mOwnerChangedToken.value != 0) {
		mOwner.AppWindow().Changed(mOwnerChangedToken);
		mOwnerChangedToken = {};
	}

	// Revoke child size changed
	if ((mChildSizeChangedToken.value != 0) && (mChildContent != nullptr)) {
		mChildContent.SizeChanged(mChildSizeChangedToken);
		mChildSizeChangedToken = {};
	}

	// Remove the scrim
	removeScrim();

	// Restore the owner to the foreground
	if (mOwnerHWND != nullptr)
		::SetForegroundWindow(mOwnerHWND);

	// Self-delete
	delete this;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - WindowHelper::Internals

class WindowHelper::Internals {
	public:
					Internals(Window window) :
						mWindow(window),
								mModal(true), mScrimOpacity(0.6),
								mPlacement(WindowHelper::Placement::kCenteredBelowOwnerTitleBar)
						{}

		static void	applyMinimumContentSize(const AppWindow& appWindow, double rasterizationScale, double width,
							double height)
						{
							// Setup
							OverlappedPresenter	overlappedPresenter =
														appWindow.Presenter().try_as<OverlappedPresenter>();
							if (overlappedPresenter == nullptr)
								// Not overlapped (full screen, compact overlay), so there is nothing to constrain
								return;

							// Get the non-client area (title bar, borders).  Both sizes are physical pixels, so the
							//	difference is the chrome.
							SizeInt32	windowSize = appWindow.Size();
							SizeInt32	clientSize = appWindow.ClientSize();
							if ((clientSize.Width <= 0) || (clientSize.Height <= 0))
								// Not laid out yet, so the chrome cannot be determined - a later call will get it
								return;

							// Convert the requested content size from DIPs to physical pixels and add the chrome
							int32_t	minimumWidth =
											(int32_t) std::ceil(width * rasterizationScale) +
													(windowSize.Width - clientSize.Width);
							int32_t	minimumHeight =
											(int32_t) std::ceil(height * rasterizationScale) +
													(windowSize.Height - clientSize.Height);

							// Set
							overlappedPresenter.PreferredMinimumWidth(minimumWidth);
							overlappedPresenter.PreferredMinimumHeight(minimumHeight);
						}

				Window					mWindow;

				bool					mModal;
				double					mScrimOpacity;
				WindowHelper::Placement	mPlacement;
				std::function<void()>	mCommitProc;
				std::function<void()>	mCancelProc;

		static	std::vector<Window>		mWindows;
};

std::vector<Window>	WindowHelper::Internals::mWindows;

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - WindowHelper

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
WindowHelper::WindowHelper(Window window)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals(window);
}

//----------------------------------------------------------------------------------------------------------------------
WindowHelper::~WindowHelper()
//----------------------------------------------------------------------------------------------------------------------
{
	delete mInternals;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
HWND WindowHelper::getWindowHandle() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Retrieve
	HWND	hWnd = nullptr;
	mInternals->mWindow.as<IWindowNative>().get()->get_WindowHandle(&hWnd);

	return hWnd;
}

//----------------------------------------------------------------------------------------------------------------------
WindowHelper& WindowHelper::setContentSize(double width, double height)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup - the window's DPI is known before it is shown, unlike its XamlRoot's rasterization scale
	AppWindow	appWindow = mInternals->mWindow.AppWindow();
	if (appWindow == nullptr)
		// No AppWindow
		return *this;

	double	scale = (double) ::GetDpiForWindow(getWindowHandle()) / (double) USER_DEFAULT_SCREEN_DPI;

	// Resize from DIPs to physical pixels
	appWindow.ResizeClient(SizeInt32{(int32_t) std::ceil(width * scale), (int32_t) std::ceil(height * scale)});

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
WindowHelper& WindowHelper::setMinimumContentSize(double width, double height)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	AppWindow	appWindow = mInternals->mWindow.AppWindow();
	if (appWindow == nullptr)
		// No AppWindow
		return *this;

	// Check if the XamlRoot is already available
	UIElement	content = mInternals->mWindow.Content();
	XamlRoot	xamlRoot = (content != nullptr) ? content.XamlRoot() : nullptr;
	if (xamlRoot != nullptr) {
		// Apply now and track scale changes
		Internals::applyMinimumContentSize(appWindow, xamlRoot.RasterizationScale(), width, height);

		xamlRoot.Changed(
				[appWindow, width, height](const XamlRoot& xamlRoot,
						const XamlRootChangedEventArgs& xamlRootChangedEventArgs){
					// Rasterization scale may have changed (moved to a display with different scaling)
					Internals::applyMinimumContentSize(appWindow, xamlRoot.RasterizationScale(), width, height);
				});
	} else if (content != nullptr) {
		// No XamlRoot yet (typically called from the Window constructor), so wait until the content is loaded
		FrameworkElement	frameworkElement = content.try_as<FrameworkElement>();
		if (frameworkElement == nullptr)
			// Cannot know when we are loaded
			return *this;

		frameworkElement.Loaded(
				[appWindow, width, height](const IInspectable& sender, const RoutedEventArgs& routedEventArgs){
					// Setup
					XamlRoot	xamlRoot = sender.as<FrameworkElement>().XamlRoot();
					if (xamlRoot == nullptr)
						// No XamlRoot
						return;

					// Apply now and track scale changes
					Internals::applyMinimumContentSize(appWindow, xamlRoot.RasterizationScale(), width, height);

					xamlRoot.Changed(
							[appWindow, width, height](const XamlRoot& xamlRoot,
									const XamlRootChangedEventArgs& xamlRootChangedEventArgs){
								// Rasterization scale may have changed
								Internals::applyMinimumContentSize(appWindow, xamlRoot.RasterizationScale(), width,
										height);
							});
				});
	}

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
WindowHelper& WindowHelper::setModal(bool modal, double scrimOpacity)
//----------------------------------------------------------------------------------------------------------------------
{
	// Store
	mInternals->mModal = modal;
	mInternals->mScrimOpacity = scrimOpacity;

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
WindowHelper& WindowHelper::setPlacement(Placement placement)
//----------------------------------------------------------------------------------------------------------------------
{
	// Store
	mInternals->mPlacement = placement;

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
WindowHelper& WindowHelper::setClosedProc(std::function<void()> closedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Set proc
	mInternals->mWindow.Closed(
		[closedProc](const IInspectable& sender, const WindowEventArgs& windowEventArgs) { closedProc(); });

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
WindowHelper& WindowHelper::setCancelProc(std::function<void()> cancelProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Store
	mInternals->mCancelProc = cancelProc;

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
WindowHelper& WindowHelper::setCommitProc(std::function<void()> commitProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Store
	mInternals->mCommitProc = commitProc;

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
void WindowHelper::presentAsOwnedFrom(Window owner)
//----------------------------------------------------------------------------------------------------------------------
{
	// Create the controller (self-owns until the window closes) and present.
	OwnedWindowController*	ownedWindowController =
									new OwnedWindowController(owner, mInternals->mWindow, mInternals->mModal,
											mInternals->mScrimOpacity, mInternals->mPlacement, mInternals->mCommitProc,
											mInternals->mCancelProc);
	ownedWindowController->present();
}

//----------------------------------------------------------------------------------------------------------------------
void WindowHelper::presentAsOwnedFrom(XamlRoot ownerXamlRoot)
//----------------------------------------------------------------------------------------------------------------------
{
	// Resolve the owner Window from the XamlRoot
	Window	owner = getFor(ownerXamlRoot);
	if (owner != nullptr)
		presentAsOwnedFrom(owner);
}

// MARK: Class methods

//----------------------------------------------------------------------------------------------------------------------
void WindowHelper::note(Window window)
//----------------------------------------------------------------------------------------------------------------------
{
	// Add
	Internals::mWindows.push_back(window);

	// Set closed proc
	WindowHelper(window).setClosedProc([window](){
		// Remove
		Internals::mWindows.erase(find(Internals::mWindows.begin(), Internals::mWindows.end(), window));
	});
}

//----------------------------------------------------------------------------------------------------------------------
Window WindowHelper::getFor(UIElement uiElement)
//----------------------------------------------------------------------------------------------------------------------
{
	return getFor(uiElement.XamlRoot());
}

//----------------------------------------------------------------------------------------------------------------------
Window WindowHelper::getFor(XamlRoot xamlRoot)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	if (xamlRoot == nullptr)
		// No XamlRoot, no Window
		return nullptr;

	// Iterate vector
	for (auto& window : Internals::mWindows) {
		// Check window
		if ((window.Content() != nullptr) && (window.Content().XamlRoot() == xamlRoot))
			// Found
			return window;
	}

	return nullptr;
}
