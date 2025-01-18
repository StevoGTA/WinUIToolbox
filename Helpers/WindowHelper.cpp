//----------------------------------------------------------------------------------------------------------------------
//	WindowHelper.cpp			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "WindowHelper.h"

#include "winrt\Windows.Foundation.h"
#include "microsoft.ui.xaml.window.h"

using IInspectable = winrt::Windows::Foundation::IInspectable;
using WindowEventArgs = winrt::Microsoft::UI::Xaml::WindowEventArgs;

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
