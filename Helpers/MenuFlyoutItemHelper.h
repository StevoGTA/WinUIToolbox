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
 
								// Instance methods
		//MenuFlyoutHelper&	addSectionTitle(const hstring& title)
		//						{
		//							// Create item
		//							MenuFlyoutItem item;
		//							item.Text(title);
		//							item.IsEnabled(false);
		//							getMenuFlyout().Items().Append(item);

		//							return *this;
		//						}

		//MenuFlyout			getMenuFlyout() const
		//						{ return getDependencyObject(); }

								// Class methods
		static	MenuFlyoutItem	forGroupTitle(const hstring& title)
									{
										// Create MenuFlyoutItem
										MenuFlyoutItem menuFlyoutItem;
										menuFlyoutItem.Text(title);
										menuFlyoutItem.IsEnabled(false);

										return menuFlyoutItem;
									}
		static	MenuFlyoutItem	forTitle(const hstring& title, std::function<bool()> validationProc,
										std::function<void()> actionProc);
									//{
									//	// Create MenuFlyoutItem
									//	MenuFlyoutItem menuFlyoutItem;
									//	menuFlyoutItem.Text(title);
									//	menuFlyoutItem.Click(
									//			[actionProc](const IInspectable& sender,
									//					const RoutedEventArgs& routedEventArgs){
									//				// Call actionProc
									//				actionProc();
									//			});

									//	return menuFlyoutItem;
									//}
		static	void			validate(MenuFlyoutItem menuFlyoutItem);
};
