//----------------------------------------------------------------------------------------------------------------------
//	CheckBoxHelper.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CheckBoxHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt\Windows.Foundation.h"

using IInspectable = winrt::Windows::Foundation::IInspectable;
using RoutedEventArgs = winrt::Microsoft::UI::Xaml::RoutedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: CheckBoxHelper

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
CheckBoxHelper& CheckBoxHelper::setChecked(bool isChecked)
//----------------------------------------------------------------------------------------------------------------------
{
	// Set value
	getCheckBox().IsChecked(isChecked);

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
CheckBoxHelper& CheckBoxHelper::setCheckedChangedProc(std::function<void(bool isChecked)> checkedChangedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	getCheckBox().Checked(
			[checkedChangedProc](const IInspectable& sender, const RoutedEventArgs& routedEventArgs) {
				// Call proc
				checkedChangedProc(sender.as<CheckBox>().IsChecked().GetBoolean());
			});
	getCheckBox().Unchecked(
			[checkedChangedProc](const IInspectable& sender, const RoutedEventArgs& routedEventArgs) {
				// Call proc
				checkedChangedProc(sender.as<CheckBox>().IsChecked().GetBoolean());
			});
	getCheckBox().Indeterminate(
			[checkedChangedProc](const IInspectable& sender, const RoutedEventArgs& routedEventArgs) {
				// Call proc
				checkedChangedProc(sender.as<CheckBox>().IsChecked().GetBoolean());
			});

	return *this;
}
