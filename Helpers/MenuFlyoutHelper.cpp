//----------------------------------------------------------------------------------------------------------------------
//	MenuFlyoutHelper.cpp			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "MenuFlyoutHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"

using MenuFlyoutItemBase = winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItemBase;
using MenuFlyoutSubItem = winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutSubItem;

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local procs

//----------------------------------------------------------------------------------------------------------------------
static void sValidate(const winrt::Windows::Foundation::Collections::IVector<MenuFlyoutItemBase>& items)
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate items
	for (auto const& item : items) {
		// Check kind
		if (auto menuFlyoutItem = item.try_as<MenuFlyoutItem>())
			// Item
			MenuFlyoutItemHelper::validate(menuFlyoutItem);
		else if (auto menuFlyoutSubItem = item.try_as<MenuFlyoutSubItem>())
			// Submenu
			sValidate(menuFlyoutSubItem.Items());
	}
}

//----------------------------------------------------------------------------------------------------------------------
// MARK: MenuFlyoutHelper

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
MenuFlyoutHelper::MenuFlyoutHelper(MenuFlyout menuFlyout) : DependencyObjectHelper(menuFlyout)
//----------------------------------------------------------------------------------------------------------------------
{
	// Set opening handler
	menuFlyout.Opening([](const IInspectable& sender, const IInspectable& args){
		// Validate
		sValidate(sender.as<MenuFlyout>().Items());
	});
}
