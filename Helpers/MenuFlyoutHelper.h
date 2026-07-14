//----------------------------------------------------------------------------------------------------------------------
//	MenuFlyoutHelper.h			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "MenuFlyoutItemHelper.h"

#include "winrt\Windows.Foundation.Collections.h"

using MenuFlyout = winrt::Microsoft::UI::Xaml::Controls::MenuFlyout;
using MenuFlyoutSeparator = winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutSeparator;
using MenuFlyoutSubItem = winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutSubItem;

//----------------------------------------------------------------------------------------------------------------------
// MARK: MenuFlyoutHelper

class MenuFlyoutHelper : public DependencyObjectHelper<MenuFlyout, MenuFlyoutHelper> {
	// Methods
	public:
							// Lifecycle methods
							MenuFlyoutHelper(MenuFlyout menuFlyout);
 
							// Instance methods
		MenuFlyoutHelper&	clear()
								{ getMenuFlyout().Items().Clear(); return *this; }

		MenuFlyoutHelper&	addSectionTitle(const winrt::param::hstring& title)
								{ getMenuFlyout().Items().Append(MenuFlyoutItemHelper::forGroupTitle(title));
										return *this; }
		MenuFlyoutHelper&	addSeparator()
								{ getMenuFlyout().Items().Append(MenuFlyoutSeparator()); return *this; }
		MenuFlyoutHelper&	addItem(const winrt::param::hstring& title, std::function<bool()> validationProc,
									std::function<void()> actionProc)
								{ getMenuFlyout().Items().Append(
											MenuFlyoutItemHelper::forTitle(title, validationProc, actionProc));
										return *this; }
		MenuFlyoutHelper&	addSubItem(const MenuFlyoutSubItem& menuFlyoutSubItem)
								{ getMenuFlyout().Items().Append(menuFlyoutSubItem); return *this; }

		MenuFlyout			getMenuFlyout() const
								{ return getDependencyObject(); }
};
