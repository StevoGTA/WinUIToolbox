//----------------------------------------------------------------------------------------------------------------------
//	WindowHelper.h			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "winrt\Microsoft.UI.Xaml.h"

#include <functional>
#include <windows.h>
#undef GetCurrentTime

using UIElement = winrt::Microsoft::UI::Xaml::UIElement;
using Window = winrt::Microsoft::UI::Xaml::Window;
using XamlRoot = winrt::Microsoft::UI::Xaml::XamlRoot;

//----------------------------------------------------------------------------------------------------------------------
// MARK: WindowHelper
/*
	Window size comes from the XAML of the window's content (its root element):
		- MinWidth / MinHeight on the root - the window never gets smaller than this (applied by note, so for any
			noted window), and an owned window opens at this size and can be resized.
		- Neither on the root - an owned window sizes itself to its content: its natural width and height (give the
			root a Width when its content stretches), no larger than the screen's work area, beyond which the content
			scrolls.  It follows the content as that changes, and cannot be resized.
*/

class WindowHelper {
	// Types
	public:
		enum class Placement {
			kCenteredBelowOwnerTitleBar,	// centered horizontally, top just below the owner's title bar (default)
			kCenteredInOwner,				// centered both ways within the owner
		};

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

				WindowHelper&	setModal(bool modal, double scrimOpacity = 0.6);
				WindowHelper&	setPlacement(Placement placement);
				WindowHelper&	setClosedProc(std::function<void()> closedProc);
				WindowHelper&	setCancelProc(std::function<void()> cancelProc);
				WindowHelper&	setCommitProc(std::function<void()> commitProc);

				void			presentAsOwnedFrom(Window owner);
				void			presentAsOwnedFrom(XamlRoot ownerXamlRoot);

								// Class methods
		static	void			note(Window window);
		static	Window			getFor(UIElement uiElement);
		static	Window			getFor(XamlRoot xamlRoot);

	// Properties
	private:
		Internals*	mInternals;
};
