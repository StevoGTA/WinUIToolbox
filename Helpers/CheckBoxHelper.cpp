//----------------------------------------------------------------------------------------------------------------------
//	CheckBoxHelper.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CheckBoxHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"

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
void CheckBoxHelper::setStateChangedProc(
		std::function<void(const IInspectable& sender, const RoutedEventArgs& routedEventArgs)> stateChangedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals->mCheckBox.Checked(stateChangedProc);
	mInternals->mCheckBox.Unchecked(stateChangedProc);
	mInternals->mCheckBox.Indeterminate(stateChangedProc);
}
