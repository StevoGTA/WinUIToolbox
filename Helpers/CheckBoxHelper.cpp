//----------------------------------------------------------------------------------------------------------------------
//	CheckBoxHelper.cpp			�2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CheckBoxHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt\Windows.Foundation.h"

using IInspectable = winrt::Windows::Foundation::IInspectable;
using RoutedEventArgs = winrt::Microsoft::UI::Xaml::RoutedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: CheckBoxHelper::Internals

class CheckBoxHelper::Internals {
	public:
		Internals(CheckBox checkbox) : mCheckBox(checkbox) {}

		CheckBox	mCheckBox;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: CheckBoxHelper

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CheckBoxHelper::CheckBoxHelper(CheckBox checkbox)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(checkbox);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
CheckBoxHelper& CheckBoxHelper::setChecked(bool isChecked)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mCheckBox.IsChecked(isChecked);

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
CheckBoxHelper& CheckBoxHelper::setCheckedChangedProc(std::function<void(bool isChecked)> checkedChangedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals->mCheckBox.Checked(
			[checkedChangedProc](const IInspectable& sender, const RoutedEventArgs& routedEventArgs) {
				// Call proc
				checkedChangedProc(sender.as<CheckBox>().IsChecked().GetBoolean());
			});
	mInternals->mCheckBox.Unchecked(
			[checkedChangedProc](const IInspectable& sender, const RoutedEventArgs& routedEventArgs) {
				// Call proc
				checkedChangedProc(sender.as<CheckBox>().IsChecked().GetBoolean());
			});
	mInternals->mCheckBox.Indeterminate(
			[checkedChangedProc](const IInspectable& sender, const RoutedEventArgs& routedEventArgs) {
				// Call proc
				checkedChangedProc(sender.as<CheckBox>().IsChecked().GetBoolean());
			});

	return *this;
}
