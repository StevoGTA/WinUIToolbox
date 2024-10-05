//----------------------------------------------------------------------------------------------------------------------
//	SliderHelper.cpp			�2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "SliderHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"

using IInspectable = winrt::Windows::Foundation::IInspectable;
using RangeBaseValueChangedEventArgs = winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: StackPanelHelper

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
SliderHelper& SliderHelper::setValue(double value)
//----------------------------------------------------------------------------------------------------------------------
{
	getSlider().Value(value);

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
SliderHelper& SliderHelper::setNormalizedValue(double value)
//----------------------------------------------------------------------------------------------------------------------
{
	getSlider().Value(value * getSlider().Maximum());

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
				// Call proc
				valueChangedProc(rangeBaseValueChangedEventArgs.NewValue() / maximum);
			});

	return *this;
}
