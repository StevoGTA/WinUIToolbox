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
		ButtonHelper&	setPointerEnteredProc(std::function<void()> pointerEnteredProc);
		ButtonHelper&	setPointerExitedProc(std::function<void()> pointerExitedProc);

		Button			getButton() const
							{ return getControl(); }
};
