//----------------------------------------------------------------------------------------------------------------------
//	CheckBoxHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "winrt\Microsoft.UI.Xaml.Controls.h"

#include <functional>

using CheckBox = winrt::Microsoft::UI::Xaml::Controls::CheckBox;

//----------------------------------------------------------------------------------------------------------------------
// MARK: CheckBoxHelper

class CheckBoxHelper {
	// Classes
	private:
		class Internals;

	// Methods
	public:
						// Lifecycle methods
						CheckBoxHelper(CheckBox checkbox);

						// Instance methods
		CheckBoxHelper&	setChecked(bool isChecked);
		CheckBoxHelper&	setCheckedChangedProc(std::function<void(bool isChecked)> checkedChangedProc);

	// Properties
	private:
		Internals*	mInternals;
};
