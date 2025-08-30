//----------------------------------------------------------------------------------------------------------------------
//	ButtonHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ControlHelper.h"

using Button = winrt::Microsoft::UI::Xaml::Controls::Button;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ButtonHelper

class ButtonHelper : public ControlHelper<Button, ButtonHelper> {
	// Methods
	public:
						// Lifecycle methods
						ButtonHelper(Button button) : ControlHelper(button) {}
						ButtonHelper() : ControlHelper(Button()) {}

						// Instance methods
		ButtonHelper&	setClickedProc(std::function<void()> clickedProc);

		Button			getButton() const
							{ return getControl(); }
};
