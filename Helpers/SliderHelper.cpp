//----------------------------------------------------------------------------------------------------------------------
//	SliderHelper.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "SliderHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"

using IInspectable = winrt::Windows::Foundation::IInspectable;
using RangeBaseValueChangedEventArgs = winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	Slider	sSliderInUpdate(nullptr);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - SliderHelper

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
SliderHelper& SliderHelper::setValue(double value)
//----------------------------------------------------------------------------------------------------------------------
{
	// Set value
	sSliderInUpdate = getSlider();
	getSlider().Value(value);
	sSliderInUpdate = nullptr;

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
SliderHelper& SliderHelper::setNormalizedValue(double value)
//----------------------------------------------------------------------------------------------------------------------
{
	// Set value
	sSliderInUpdate = getSlider();
	getSlider().Value(value * getSlider().Maximum());
	sSliderInUpdate = nullptr;

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
SliderHelper& SliderHelper::setValueChangedProc(std::function<void(double value)> valueChangedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Set handler
	getSlider().ValueChanged(
			[valueChangedProc](const IInspectable& sender,
					const RangeBaseValueChangedEventArgs& rangeBaseValueChangedEventArgs) {
				// Check if handling events
				if (sender == sSliderInUpdate)
					return;

				// Call proc
				valueChangedProc(rangeBaseValueChangedEventArgs.NewValue());
			});

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
SliderHelper& SliderHelper::setNormalizedValueChangedProc(std::function<void(double value)> valueChangedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	auto	maximum = getSlider().Maximum();

	// Set handler
	getSlider().ValueChanged(
			[valueChangedProc, maximum](const IInspectable& sender,
					const RangeBaseValueChangedEventArgs& rangeBaseValueChangedEventArgs) {
				// Check if handling events
				if (sender == sSliderInUpdate)
					return;

				// Call proc
				valueChangedProc(rangeBaseValueChangedEventArgs.NewValue() / maximum);
			});

	return *this;
}
