//----------------------------------------------------------------------------------------------------------------------
//	MenuFlyoutItemHelper.h			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ControlHelper.h"

using hstring = winrt::hstring;
using MenuFlyoutItem = winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem;
using RoutedEventArgs = winrt::Microsoft::UI::Xaml::RoutedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: MenuFlyoutItemHelper

class MenuFlyoutItemHelper : public ControlHelper<MenuFlyoutItem, MenuFlyoutItemHelper> {
	// Methods
	public:
								// Lifecycle methods
								MenuFlyoutItemHelper(MenuFlyoutItem menuFlyoutItem) : ControlHelper(menuFlyoutItem) {}
 
								// Class methods
		static	MenuFlyoutItem	forGroupTitle(const winrt::param::hstring& title)
									{
										// Create MenuFlyoutItem
										MenuFlyoutItem menuFlyoutItem;
										menuFlyoutItem.Text(title);
										menuFlyoutItem.IsEnabled(false);

										return menuFlyoutItem;
									}
		static	MenuFlyoutItem	forTitle(const winrt::param::hstring& title, std::function<bool()> validationProc,
										std::function<void()> actionProc);
		static	void			validate(MenuFlyoutItem menuFlyoutItem);
};
