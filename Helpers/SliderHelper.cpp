//----------------------------------------------------------------------------------------------------------------------
//	SliderHelper.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "SliderHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"

using IInspectable = winrt::Windows::Foundation::IInspectable;
using RangeBaseValueChangedEventArgs = winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: SliderHelper::Internals

class SliderHelper::Internals {
	public:
		Internals(Slider slider) : mSlider(slider) {}

		Slider	mSlider;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: StackPanelHelper

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
SliderHelper::SliderHelper(Slider slider)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(slider);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
SliderHelper& SliderHelper::setValue(double value)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mSlider.Value(value);

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
SliderHelper& SliderHelper::setNormalizedValue(double value)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mSlider.Value(value * mInternals->mSlider.Maximum());

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
SliderHelper& SliderHelper::setValueChangedProc(std::function<void(double value)> valueChangedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Set handler
	mInternals->mSlider.ValueChanged(
			[valueChangedProc](const IInspectable& sender,
					const RangeBaseValueChangedEventArgs& rangeBaseValueChangedEventArgs) {
				// Call proc
				valueChangedProc(rangeBaseValueChangedEventArgs.NewValue());
			});

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
SliderHelper& SliderHelper::setNormalizedValueChangedProcs(std::function<void(double value)> valueChangedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	auto	maximum = mInternals->mSlider.Maximum();

	// Set handler
	mInternals->mSlider.ValueChanged(
			[valueChangedProc, maximum](const IInspectable& sender,
					const RangeBaseValueChangedEventArgs& rangeBaseValueChangedEventArgs) {
				// Call proc
				valueChangedProc(rangeBaseValueChangedEventArgs.NewValue() / maximum);
			});

	return *this;
}
