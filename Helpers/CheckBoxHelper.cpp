//----------------------------------------------------------------------------------------------------------------------
//	CheckBoxHelper.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CheckBoxHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt\Windows.Foundation.h"

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
void CheckBoxHelper::setStateChangedProc(std::function<void(bool isChecked)> stateChangedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals->mCheckBox.Checked(
			[stateChangedProc](const IInspectable& sender, const RoutedEventArgs& routedEventArgs) {
				// Call proc
				stateChangedProc(sender.as<CheckBox>().IsChecked().GetBoolean());
			});
	mInternals->mCheckBox.Unchecked(
			[stateChangedProc](const IInspectable& sender, const RoutedEventArgs& routedEventArgs) {
				// Call proc
				stateChangedProc(sender.as<CheckBox>().IsChecked().GetBoolean());
			});
	mInternals->mCheckBox.Indeterminate(
			[stateChangedProc](const IInspectable& sender, const RoutedEventArgs& routedEventArgs) {
				// Call proc
				stateChangedProc(sender.as<CheckBox>().IsChecked().GetBoolean());
			});
}
