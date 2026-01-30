//----------------------------------------------------------------------------------------------------------------------
//	CheckBoxHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ControlHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.h"

using CheckBox = winrt::Microsoft::UI::Xaml::Controls::CheckBox;

//----------------------------------------------------------------------------------------------------------------------
// MARK: CheckBoxHelper

class CheckBoxHelper : public ControlHelper<CheckBox, CheckBoxHelper> {
	// Methods
	public:
						// Lifecycle methods
						CheckBoxHelper(CheckBox checkbox) : ControlHelper(checkbox) {}
						CheckBoxHelper() : ControlHelper(CheckBox()) {}

						// Instance methods
		CheckBoxHelper&	setChecked(bool isChecked);
		CheckBoxHelper&	setCheckedChangedProc(std::function<void(bool isChecked)> checkedChangedProc);

		CheckBox		getCheckBox() const
							{ return getControl(); }
};
