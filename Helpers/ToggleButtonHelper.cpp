//----------------------------------------------------------------------------------------------------------------------
//	ToggleButtonHelper.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "ToggleButtonHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt\Windows.Foundation.h"

using IInspectable = winrt::Windows::Foundation::IInspectable;
using RoutedEventArgs = winrt::Microsoft::UI::Xaml::RoutedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	ToggleButton	sToggleButtonInUpdate(nullptr);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - ToggleButtonHelper

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
ToggleButtonHelper& ToggleButtonHelper::setChecked(bool isChecked)
//----------------------------------------------------------------------------------------------------------------------
{
	// Set value
	sToggleButtonInUpdate = getToggleButton();
	getToggleButton().IsChecked(isChecked);
	sToggleButtonInUpdate = nullptr;

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ToggleButtonHelper& ToggleButtonHelper::setCheckedProc(std::function<void()> checkedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	getToggleButton().Checked([checkedProc](const IInspectable& sender, const RoutedEventArgs& routedEventArgs) {
		// Check if handling events
		if (sender == sToggleButtonInUpdate)
			return;

		// Call proc
		checkedProc();
	});

	return *this;
}
