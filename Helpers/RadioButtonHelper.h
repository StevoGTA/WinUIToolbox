//----------------------------------------------------------------------------------------------------------------------
//	RadioButtonHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "winrt\Microsoft.UI.Xaml.Controls.h"

#include <functional>

using RadioButton = winrt::Microsoft::UI::Xaml::Controls::RadioButton;

//----------------------------------------------------------------------------------------------------------------------
// MARK: RadioButtonHelper

class RadioButtonHelper {
	// Classes
	private:
		class Internals;

	// Methods
	public:
							// Lifecycle methods
							RadioButtonHelper(RadioButton radioButton);

							// Instance methods
		RadioButtonHelper&	setChecked(bool isChecked);
		RadioButtonHelper&	setClickProc(std::function<void()> clickProc);

	// Properties
	private:
		Internals*	mInternals;
};
