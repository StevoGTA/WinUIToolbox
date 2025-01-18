//----------------------------------------------------------------------------------------------------------------------
//	WindowHelper.h			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "winrt\Microsoft.UI.Xaml.h"

#include <functional>

using UIElement = winrt::Microsoft::UI::Xaml::UIElement;
using Window = winrt::Microsoft::UI::Xaml::Window;

//----------------------------------------------------------------------------------------------------------------------
// MARK: WindowHelper

class WindowHelper {
	// Classes
	private:
		class	Internals;

	// Methods
	public:
								// Lifecycle methods
								WindowHelper(Window window);
								~WindowHelper();

								// Instance methods
				HWND			getWindowHandle() const;

				WindowHelper&	setClosedProc(std::function<void()> closedProc);

								// Class methods
		static	void			note(Window window);
		static	Window			getFor(UIElement uiElement);

	// Properties
	private:
		Internals*	mInternals;
};
