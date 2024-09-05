//----------------------------------------------------------------------------------------------------------------------
//	NavigationViewHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "winrt\Microsoft.UI.Xaml.Controls.h"

#include <functional>

using hstring = winrt::hstring;
using NavigationView = winrt::Microsoft::UI::Xaml::Controls::NavigationView;

//----------------------------------------------------------------------------------------------------------------------
// MARK: NavigationViewHelper

class NavigationViewHelper {
	// Classes
	private:
		class Internals;

	// Methods
	public:
								// Lifecycle methods
								NavigationViewHelper(NavigationView navigationView);

								// Instance methods
		NavigationViewHelper&	setNavigationViewItemIsEnabled(int index, bool isEnabled);

		NavigationViewHelper&	setLoadedProc(std::function<void()> loadedProc);
		NavigationViewHelper&	setSelectedTagChangedProc(
										std::function<void(const hstring& tag)> selectedTagChangedProc);

	// Properties
	private:
		Internals*	mInternals;
};
