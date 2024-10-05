//----------------------------------------------------------------------------------------------------------------------
//	SliderHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ControlHelper.h"

#include <functional>

using Slider = winrt::Microsoft::UI::Xaml::Controls::Slider;

//----------------------------------------------------------------------------------------------------------------------
// MARK: SliderHelper

class SliderHelper : public ControlHelper<Slider, SliderHelper> {
	// Methods
	public:
						// Lifecycle methods
						SliderHelper(Slider slider) : ControlHelper(slider) {}

						// Instance methods
		SliderHelper&	setValue(double value);
		SliderHelper&	setNormalizedValue(double value);

		SliderHelper&	setValueChangedProc(std::function<void(double value)> valueChangedProc);
		SliderHelper&	setNormalizedValueChangedProc(std::function<void(double value)> valueChangedProc);

		Slider			getSlider() const
							{ return getControl(); }
};
