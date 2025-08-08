//----------------------------------------------------------------------------------------------------------------------
//	MenuFlyoutHelper.cpp			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "MenuFlyoutHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"

using MenuFlyoutSubItem = winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutSubItem;

//----------------------------------------------------------------------------------------------------------------------
// MARK: MenuFlyoutHelper

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
MenuFlyoutHelper::MenuFlyoutHelper(MenuFlyout menuFlyout) : DependencyObjectHelper(menuFlyout)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	menuFlyout.Opening(
			[this, menuFlyout](const IInspectable& sender, const IInspectable& args){
				// Check if should enable items
				for (auto menuFlyoutItem : menuFlyout.Items()) {
					// Check if MenuFlyoutItem
					if (menuFlyoutItem.try_as<MenuFlyoutItem>())
						// Validate
						MenuFlyoutItemHelper::validate(menuFlyoutItem.as<MenuFlyoutItem>());
					else if (menuFlyoutItem.try_as<MenuFlyoutSubItem>()) {
						// Validate sub items
						for (auto menuFlyoutSubItem : menuFlyoutItem.as<MenuFlyoutSubItem>().Items())
							// Validate
							MenuFlyoutItemHelper::validate(menuFlyoutSubItem.as<MenuFlyoutItem>());
					}
				}
			});
}
