//----------------------------------------------------------------------------------------------------------------------
//	NumberBoxHelper.cpp			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "NumberBoxHelper.h"

#include "winrt\Windows.Foundation.h"

using IInspectable = winrt::Windows::Foundation::IInspectable;
using NumberBoxValueChangedEventArgs = winrt::Microsoft::UI::Xaml::Controls::NumberBoxValueChangedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	NumberBox	sNumberBoxInUpdate(nullptr);

//----------------------------------------------------------------------------------------------------------------------
// MARK: ButtonHelper

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
NumberBoxHelper& NumberBoxHelper::setValue(double value)
//----------------------------------------------------------------------------------------------------------------------
{
	// Set value
	sNumberBoxInUpdate = getNumberBox();
	getNumberBox().Value(value);
	sNumberBoxInUpdate = nullptr;

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
NumberBoxHelper& NumberBoxHelper::setValue(int value)
//----------------------------------------------------------------------------------------------------------------------
{
	// Set value
	sNumberBoxInUpdate = getNumberBox();
	getNumberBox().Value(value);
	sNumberBoxInUpdate = nullptr;

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
NumberBoxHelper& NumberBoxHelper::setDoubleValueChangedProc(std::function<void(double value)> valueChangedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	getNumberBox().ValueChanged(
			[valueChangedProc](const IInspectable& sender,
					const NumberBoxValueChangedEventArgs& numberBoxValueChangedEventArgs){
				// Check if handling events
				if (sender == sNumberBoxInUpdate)
					return;

				// Call proc
				valueChangedProc(numberBoxValueChangedEventArgs.NewValue());
			});

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
NumberBoxHelper& NumberBoxHelper::setIntValueChangedProc(std::function<void(int value)> valueChangedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	getNumberBox().ValueChanged(
			[valueChangedProc](const IInspectable& sender,
					const NumberBoxValueChangedEventArgs& numberBoxValueChangedEventArgs){
				// Check if handling events
				if (sender == sNumberBoxInUpdate)
					return;

				// Call proc
				valueChangedProc((int) numberBoxValueChangedEventArgs.NewValue());
			});

	return *this;
}
