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
				for (auto item : menuFlyout.Items()) {
					// Check if MenuFlyoutItem
					if (auto menuFlyoutItem = item.try_as<MenuFlyoutItem>())
						// Validate
						MenuFlyoutItemHelper::validate(menuFlyoutItem);
					else if (auto menuFlyoutSubItem = item.try_as<MenuFlyoutSubItem>()) {
						// Validate sub items (skip separators / non-item children)
						for (auto subItem : menuFlyoutSubItem.Items())
							// Check if MenuFlyoutItem
							if (auto menuFlyoutSubMenuItem = subItem.try_as<MenuFlyoutItem>())
								// Validate
								MenuFlyoutItemHelper::validate(menuFlyoutSubMenuItem);
					}
				}
			});
}
