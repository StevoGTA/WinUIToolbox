//----------------------------------------------------------------------------------------------------------------------
//	SliderHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "winrt\Microsoft.UI.Xaml.Controls.h"

#include <functional>

using Slider = winrt::Microsoft::UI::Xaml::Controls::Slider;

//----------------------------------------------------------------------------------------------------------------------
// MARK: SliderHelper

class SliderHelper {
	// Classes
	private:
		class Internals;

	// Methods
	public:
						// Lifecycle methods
						SliderHelper(Slider slider);

						// Instance methods
		SliderHelper&	setValue(double value);
		SliderHelper&	setNormalizedValue(double value);

		SliderHelper&	setValueChangedProc(std::function<void(double value)> valueChangedProc);
		SliderHelper&	setNormalizedValueChangedProc(std::function<void(double value)> valueChangedProc);

	// Properties
	private:
		Internals*	mInternals;
};
