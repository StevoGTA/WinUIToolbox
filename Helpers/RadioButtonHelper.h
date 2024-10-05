//----------------------------------------------------------------------------------------------------------------------
//	RadioButtonHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ControlHelper.h"

#include <functional>

using RadioButton = winrt::Microsoft::UI::Xaml::Controls::RadioButton;

//----------------------------------------------------------------------------------------------------------------------
// MARK: RadioButtonHelper

class RadioButtonHelper : public ControlHelper<RadioButton, RadioButtonHelper> {
	// Methods
	public:
							// Lifecycle methods
							RadioButtonHelper(RadioButton radioButton) : ControlHelper(radioButton) {}
							RadioButtonHelper() : ControlHelper(RadioButton()) {}

							// Instance methods
		RadioButtonHelper&	setChecked(bool isChecked);
		RadioButtonHelper&	setClickProc(std::function<void()> clickProc);

		RadioButton			getRadioButton() const
								{ return getControl(); }
};
