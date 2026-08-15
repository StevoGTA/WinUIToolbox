//----------------------------------------------------------------------------------------------------------------------
//	WindowHelper.cpp			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "WindowHelper.h"

#include "winrt\Microsoft.UI.Windowing.h"
#include "winrt\Windows.Foundation.h"
#include "microsoft.ui.xaml.window.h"

#include <cmath>

using AppWindow = winrt::Microsoft::UI::Windowing::AppWindow;
using FrameworkElement = winrt::Microsoft::UI::Xaml::FrameworkElement;
using IInspectable = winrt::Windows::Foundation::IInspectable;
using OverlappedPresenter = winrt::Microsoft::UI::Windowing::OverlappedPresenter;
using RoutedEventArgs = winrt::Microsoft::UI::Xaml::RoutedEventArgs;
using SizeInt32 = winrt::Windows::Graphics::SizeInt32;
using WindowEventArgs = winrt::Microsoft::UI::Xaml::WindowEventArgs;
using XamlRoot = winrt::Microsoft::UI::Xaml::XamlRoot;
using XamlRootChangedEventArgs = winrt::Microsoft::UI::Xaml::XamlRootChangedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local proc declarations

static	void	sApplyMinimumContentSize(const AppWindow& appWindow, double rasterizationScale, double width,
						double height);

//----------------------------------------------------------------------------------------------------------------------
// MARK: WindowHelper::Internals

class WindowHelper::Internals {
	public:
		Internals(Window window) : mWindow(window) {}

				Window				mWindow;

		static	std::vector<Window>	mWindows;
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
	// Setup
	HWND	hWnd = nullptr;

	// Retrieve
	mInternals->mWindow.as<IWindowNative>().get()->get_WindowHandle(&hWnd);

	return hWnd;
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
		sApplyMinimumContentSize(appWindow, xamlRoot.RasterizationScale(), width, height);

		xamlRoot.Changed(
				[appWindow, width, height](const XamlRoot& xamlRoot,
						const XamlRootChangedEventArgs& xamlRootChangedEventArgs){
					// Rasterization scale may have changed (moved to a display with different scaling)
					sApplyMinimumContentSize(appWindow, xamlRoot.RasterizationScale(), width, height);
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
					sApplyMinimumContentSize(appWindow, xamlRoot.RasterizationScale(), width, height);

					xamlRoot.Changed(
							[appWindow, width, height](const XamlRoot& xamlRoot,
									const XamlRootChangedEventArgs& xamlRootChangedEventArgs){
								// Rasterization scale may have changed
								sApplyMinimumContentSize(appWindow, xamlRoot.RasterizationScale(), width, height);
							});
				});
	}

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
WindowHelper& WindowHelper::setClosedProc(std::function<void()> closedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Set proc
	mInternals->mWindow.Closed(
			[closedProc](const IInspectable& sender, const WindowEventArgs& windowEventArgs){ closedProc(); });

	return *this;
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
	// Setup
	auto	xamlRoot = uiElement.XamlRoot();
	if (xamlRoot == nullptr)
		// No XamlRoot, no Window
		return nullptr;

	// Iterate vector
	for (auto& window : Internals::mWindows) {
		// Check window
		if (window.Content().XamlRoot() == xamlRoot)
			// Found
			return window;
	}

	return nullptr;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc definitions

//----------------------------------------------------------------------------------------------------------------------
void sApplyMinimumContentSize(const AppWindow& appWindow, double rasterizationScale, double width, double height)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	OverlappedPresenter	overlappedPresenter = appWindow.Presenter().try_as<OverlappedPresenter>();
	if (overlappedPresenter == nullptr)
		// Not overlapped (full screen, compact overlay), so there is nothing to constrain
		return;

	// Get the non-client area (title bar, borders).  Both sizes are physical pixels, so the difference is the chrome.
	SizeInt32	windowSize = appWindow.Size();
	SizeInt32	clientSize = appWindow.ClientSize();
	if ((clientSize.Width <= 0) || (clientSize.Height <= 0))
		// Not laid out yet, so the chrome cannot be determined - a later call will get it
		return;

	// Convert the requested content size from DIPs to physical pixels and add the chrome
	int32_t	minimumWidth = (int32_t) std::ceil(width * rasterizationScale) + (windowSize.Width - clientSize.Width);
	int32_t	minimumHeight = (int32_t) std::ceil(height * rasterizationScale) + (windowSize.Height - clientSize.Height);

	// Set
	overlappedPresenter.PreferredMinimumWidth(minimumWidth);
	overlappedPresenter.PreferredMinimumHeight(minimumHeight);
}
