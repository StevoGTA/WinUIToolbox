//----------------------------------------------------------------------------------------------------------------------
//	CheckBoxHelper.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CheckBoxHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt\Windows.Foundation.h"

using IInspectable = winrt::Windows::Foundation::IInspectable;
using RoutedEventArgs = winrt::Microsoft::UI::Xaml::RoutedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	CheckBox	sCheckBoxInUpdate(nullptr);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CheckBoxHelper

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
CheckBoxHelper& CheckBoxHelper::setChecked(bool isChecked)
//----------------------------------------------------------------------------------------------------------------------
{
	// Set value
	sCheckBoxInUpdate = getCheckBox();
	getCheckBox().IsChecked(isChecked);
	sCheckBoxInUpdate = nullptr;

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
CheckBoxHelper& CheckBoxHelper::setCheckedChangedProc(std::function<void(bool isChecked)> checkedChangedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	getCheckBox().Checked(
			[checkedChangedProc](const IInspectable& sender, const RoutedEventArgs& routedEventArgs) {
				// Check if handling events
				if (sender == sCheckBoxInUpdate)
					return;

				// Call proc
				checkedChangedProc(sender.as<CheckBox>().IsChecked().GetBoolean());
			});
	getCheckBox().Unchecked(
			[checkedChangedProc](const IInspectable& sender, const RoutedEventArgs& routedEventArgs) {
				// Check if handling events
				if (sender == sCheckBoxInUpdate)
					return;

				// Call proc
				checkedChangedProc(sender.as<CheckBox>().IsChecked().GetBoolean());
			});
	getCheckBox().Indeterminate(
			[checkedChangedProc](const IInspectable& sender, const RoutedEventArgs& routedEventArgs) {
				// Check if handling events
				if (sender == sCheckBoxInUpdate)
					return;

				// Call proc
				checkedChangedProc(sender.as<CheckBox>().IsChecked().GetBoolean());
			});

	return *this;
}
