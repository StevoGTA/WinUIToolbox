//----------------------------------------------------------------------------------------------------------------------
//	ButtonHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "winrt\Microsoft.UI.Xaml.Controls.h"

#include <functional>

using Button = winrt::Microsoft::UI::Xaml::Controls::Button;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ButtonHelper

class ButtonHelper {
	// Classes
	private:
		class Internals;

	// Methods
	public:
						// Lifecycle methods
						ButtonHelper(Button button);

						// Instance methods
		ButtonHelper&	setClickedProc(std::function<void()> clickedProc);

	// Properties
	private:
		Internals*	mInternals;
};
