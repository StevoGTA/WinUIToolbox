//----------------------------------------------------------------------------------------------------------------------
//	NavigationViewHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ControlHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.h"

#include <functional>

using hstring = winrt::hstring;
using NavigationView = winrt::Microsoft::UI::Xaml::Controls::NavigationView;

//----------------------------------------------------------------------------------------------------------------------
// MARK: NavigationViewHelper

class NavigationViewHelper : public ControlHelper<NavigationView, NavigationViewHelper> {
	// Methods
	public:
								// Lifecycle methods
								NavigationViewHelper(NavigationView navigationView) : ControlHelper(navigationView) {}

								// Instance methods
		NavigationViewHelper&	setNavigationViewItemIsEnabled(int index, bool isEnabled);

		NavigationViewHelper&	setLoadedProc(std::function<void()> loadedProc);
		NavigationViewHelper&	setSelectedTagChangedProc(
										std::function<void(const hstring& tag)> selectedTagChangedProc);

		NavigationView			getNavigationView() const
									{ return getControl(); }
};
