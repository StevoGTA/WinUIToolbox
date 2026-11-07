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

				WindowHelper&	setContentSize(double width, double height);
				WindowHelper&	setMinimumContentSize(double width, double height);

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
