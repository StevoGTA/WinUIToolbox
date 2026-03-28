//----------------------------------------------------------------------------------------------------------------------
//	ToggleButtonHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ControlHelper.h"

#include <functional>

using ToggleButton = winrt::Microsoft::UI::Xaml::Controls::Primitives::ToggleButton;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ToggleButtonHelper

class ToggleButtonHelper : public ControlHelper<ToggleButton, ToggleButtonHelper> {
	// Methods
	public:
							// Lifecycle methods
							ToggleButtonHelper(ToggleButton toggleButton) : ControlHelper(toggleButton) {}
							ToggleButtonHelper() : ControlHelper(ToggleButton()) {}

							// Instance methods
		ToggleButtonHelper&	setChecked(bool isChecked);
		ToggleButtonHelper& setCheckedProc(std::function<void()> checkedProc);

		ToggleButton		getToggleButton() const
								{ return getControl(); }
};
