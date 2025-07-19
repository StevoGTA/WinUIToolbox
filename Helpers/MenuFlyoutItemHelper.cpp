//----------------------------------------------------------------------------------------------------------------------
//	MenuFlyoutItemHelper.cpp			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "MenuFlyoutItemHelper.h"

struct FunctionWrapper : winrt::implements<FunctionWrapper, winrt::Windows::Foundation::IInspectable> {
	FunctionWrapper(std::function<bool()> proc) : mProc(proc) {}

	std::function<bool()> mProc;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: MenuFlyoutItemHelper

// MARK: Class methods

//----------------------------------------------------------------------------------------------------------------------
MenuFlyoutItem MenuFlyoutItemHelper::forTitle(const hstring& title, std::function<bool()> validationProc,
		std::function<void()> actionProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Create MenuFlyoutItem
	MenuFlyoutItem menuFlyoutItem;
	menuFlyoutItem.Text(title);
	menuFlyoutItem.Tag(winrt::make<FunctionWrapper>(validationProc));
	menuFlyoutItem.Click([actionProc](const IInspectable& sender, const RoutedEventArgs& routedEventArgs){
		// Call actionProc
		actionProc();
	});

	return menuFlyoutItem;
}

//----------------------------------------------------------------------------------------------------------------------
void MenuFlyoutItemHelper::validate(MenuFlyoutItem& menuFlyoutItem)
//----------------------------------------------------------------------------------------------------------------------
{
	// Try to get validationProc
	auto	functionWrapper = menuFlyoutItem.Tag().try_as<FunctionWrapper>();
	if (functionWrapper)
		// Set IsEnabled
		menuFlyoutItem.IsEnabled(functionWrapper->mProc());
}
