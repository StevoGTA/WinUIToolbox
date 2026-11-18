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

#include <commctrl.h>

#include <algorithm>
#include <cmath>
#include <limits>

#pragma comment(lib, "comctl32.lib")

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
using ScrollBarVisibility = winrt::Microsoft::UI::Xaml::Controls::ScrollBarVisibility;
using ScrollViewer = winrt::Microsoft::UI::Xaml::Controls::ScrollViewer;
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
		void	sizeToContent() const;
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

				FrameworkElement		mSizedContent{nullptr};
				winrt::event_token		mSizedContentLayoutUpdatedToken{};

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

	// Wire optional Commit / Cancel buttons if present in the child content (before sizing may rehost the content).
	wireButtons();

	// Size - content with a MinWidth / MinHeight opens at that size and can be resized (WindowHelper::note keeps it
	//	from getting smaller), and any other content sizes the window to itself.
	AppWindow			childAppWindow = mChild.AppWindow();
	OverlappedPresenter	overlappedPresenter = childAppWindow.Presenter().try_as<OverlappedPresenter>();
	FrameworkElement	content = mChild.Content().try_as<FrameworkElement>();
	if ((content != nullptr) && ((content.MinWidth() > 0.0) || (content.MinHeight() > 0.0))) {
		// Open at the minimum size - a dimension without a minimum keeps its current size
		double		scale = (double) ::GetDpiForWindow(mChildHWND) / (double) USER_DEFAULT_SCREEN_DPI;
		SizeInt32	clientSize = childAppWindow.ClientSize();
		childAppWindow.ResizeClient(
				SizeInt32{
						(content.MinWidth() > 0.0) ? (int32_t) std::ceil(content.MinWidth() * scale) : clientSize.Width,
						(content.MinHeight() > 0.0) ?
								(int32_t) std::ceil(content.MinHeight() * scale) : clientSize.Height});
	} else if (content != nullptr) {
		// Size to the content - which decides the size
		mSizedContent = content;

		if (overlappedPresenter != nullptr)
			overlappedPresenter.IsResizable(false);

		// Measure without limits and size to it now, so the window is first shown at its size
		mSizedContent.Measure(
				winrt::Windows::Foundation::Size{std::numeric_limits<float>::infinity(),
						std::numeric_limits<float>::infinity()});
		sizeToContent();

		// Host the content in a ScrollViewer - it is measured without a height limit, so its desired height is all of
		//	it, and it scrolls once that is more than the room on screen
		ScrollViewer	scrollViewer;
		scrollViewer.HorizontalScrollBarVisibility(ScrollBarVisibility::Disabled);
		scrollViewer.VerticalScrollBarVisibility(ScrollBarVisibility::Auto);
		mChild.Content(scrollViewer);
		scrollViewer.Content(mSizedContent);

		// Follow the content's desired size after every layout pass (resizing changes only the viewport, never the
		//	desired size, so this settles).  This also corrects the first size should measuring before being shown
		//	come up short.
		mSizedContentLayoutUpdatedToken =
				mSizedContent.LayoutUpdated([this](const IInspectable&, const IInspectable&){ sizeToContent(); });
	}

	// Remove the child's title bar - it is moved only with the owner.  Keep a border so it reads as a distinct window,
	//	and keep the content size (removing the title bar would otherwise hand its height to the content).
	if (overlappedPresenter != nullptr) {
		// Remove title bar
		SizeInt32	clientSize = childAppWindow.ClientSize();
		overlappedPresenter.SetBorderAndTitleBar(true, false);
		childAppWindow.ResizeClient(clientSize);
	}

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

				// Where the owner is changes the room we have on screen
				if ((args.DidPositionChange() || args.DidSizeChange()) && (mSizedContent != nullptr))
					sizeToContent();
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
void OwnedWindowController::sizeToContent() const
{
	// Setup
	winrt::Windows::Foundation::Size	desiredSize = mSizedContent.DesiredSize();
	if ((desiredSize.Width <= 0.0f) || (desiredSize.Height <= 0.0f))
		// Not measured yet
		return;

	double	scale = (double) ::GetDpiForWindow(mChildHWND) / (double) USER_DEFAULT_SCREEN_DPI;

	// Get the room on screen - the work area's width, and from where the window's top is placed down to the bottom of
	//	the work area, each less the window's own chrome.  An owned window may extend past its owner.
	MONITORINFO	monitorInfo{sizeof(MONITORINFO)};
	::GetMonitorInfoW(::MonitorFromWindow(mOwnerHWND, MONITOR_DEFAULTTONEAREST), &monitorInfo);

	LONG	top = monitorInfo.rcWork.top;
	if (mPlacement == WindowHelper::Placement::kCenteredBelowOwnerTitleBar) {
		// Just below the owner's title bar
		POINT	ownerClientOrigin = {0, 0};
		::ClientToScreen(mOwnerHWND, &ownerClientOrigin);
		top = (std::max)(top, ownerClientOrigin.y);
	}

	RECT	childWindowRect;
	::GetWindowRect(mChildHWND, &childWindowRect);

	RECT	childClientRect;
	::GetClientRect(mChildHWND, &childClientRect);

	LONG	chromeWidth =
					(childWindowRect.right - childWindowRect.left) - (childClientRect.right - childClientRect.left);
	LONG	chromeHeight =
					(childWindowRect.bottom - childWindowRect.top) - (childClientRect.bottom - childClientRect.top);

	// Get the desired size in physical pixels, no larger than the room on screen
	int32_t	width =
					(std::min)((int32_t) std::ceil(desiredSize.Width * scale),
							(int32_t) (monitorInfo.rcWork.right - monitorInfo.rcWork.left - chromeWidth));
	int32_t	height =
					(std::min)((int32_t) std::ceil(desiredSize.Height * scale),
							(int32_t) (monitorInfo.rcWork.bottom - top - chromeHeight));

	// Resize if needed
	AppWindow	appWindow = mChild.AppWindow();
	SizeInt32	clientSize = appWindow.ClientSize();
	if ((width != clientSize.Width) || (height != clientSize.Height))
		appWindow.ResizeClient(SizeInt32{width, height});
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

	// Revoke size to content
	if ((mSizedContentLayoutUpdatedToken.value != 0) && (mSizedContent != nullptr)) {
		mSizedContent.LayoutUpdated(mSizedContentLayoutUpdatedToken);
		mSizedContentLayoutUpdatedToken = {};
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
		struct MinimumContentSize {
			double	mWidth;
			double	mHeight;
		};

	public:
									Internals(Window window) :
										mWindow(window),
												mModal(true), mScrimOpacity(0.6),
												mPlacement(WindowHelper::Placement::kCenteredBelowOwnerTitleBar)
										{}

		static	LRESULT CALLBACK	minimumContentSizeSubclassProc(HWND hWnd, UINT message, WPARAM wParam,
											LPARAM lParam, UINT_PTR subclassID, DWORD_PTR refData)
										{
											// Setup
											const	MinimumContentSize&	minimumContentSize =
																				*((MinimumContentSize*) refData);

											// Check message
											switch (message) {
												case WM_GETMINMAXINFO: {
													// Let the window have its say first - ours only ever enlarges it
													LRESULT	result = ::DefSubclassProc(hWnd, message, wParam, lParam);

													// Setup - the content size in physical pixels at the window's
													//	current scale, grown by the window's own frame at that scale.
													//	Asked at every resize, so it always matches the display.
													UINT	dpi = ::GetDpiForWindow(hWnd);
													double	scale = (double) dpi / (double) USER_DEFAULT_SCREEN_DPI;

													RECT	rect;
													rect.left = 0;
													rect.top = 0;
													rect.right = (LONG) std::ceil(minimumContentSize.mWidth * scale);
													rect.bottom = (LONG) std::ceil(minimumContentSize.mHeight * scale);
													::AdjustWindowRectExForDpi(&rect,
															(DWORD) ::GetWindowLongPtrW(hWnd, GWL_STYLE), FALSE,
															(DWORD) ::GetWindowLongPtrW(hWnd, GWL_EXSTYLE), dpi);

													// Update
													MINMAXINFO*	minMaxInfo = (MINMAXINFO*) lParam;
													minMaxInfo->ptMinTrackSize.x =
															(std::max)(minMaxInfo->ptMinTrackSize.x, rect.right -
																	rect.left);
													minMaxInfo->ptMinTrackSize.y =
															(std::max)(minMaxInfo->ptMinTrackSize.y, rect.bottom -
																	rect.top);

													return result;
												}

												case WM_NCDESTROY:
													// Cleanup
													::RemoveWindowSubclass(hWnd, minimumContentSizeSubclassProc,
															subclassID);
													delete (MinimumContentSize*) refData;
													break;

												default:
													break;
											}

											return ::DefSubclassProc(hWnd, message, wParam, lParam);
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

	// Keep the content from getting smaller than its MinWidth / MinHeight - Windows asks at every resize
	//	(WM_GETMINMAXINFO), so the minimum is always worked out at the window's current scale
	FrameworkElement	content = window.Content().try_as<FrameworkElement>();
	if ((content != nullptr) && ((content.MinWidth() > 0.0) || (content.MinHeight() > 0.0))) {
		// Install
		Internals::MinimumContentSize*	minimumContentSize =
												new Internals::MinimumContentSize{content.MinWidth(),
														content.MinHeight()};
		if (!::SetWindowSubclass(WindowHelper(window).getWindowHandle(),
				Internals::minimumContentSizeSubclassProc, 0, (DWORD_PTR) minimumContentSize))
			// Not installed
			delete minimumContentSize;
	}
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
