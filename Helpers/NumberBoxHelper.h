//----------------------------------------------------------------------------------------------------------------------
//	NumberBoxHelper.h			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ControlHelper.h"

#include <functional>

#include "winrt\Windows.Foundation.h"

using Application = winrt::Microsoft::UI::Xaml::Application;
using INumberFormatter2 = winrt::Windows::Globalization::NumberFormatting::INumberFormatter2;
using NumberBox = winrt::Microsoft::UI::Xaml::Controls::NumberBox;

//----------------------------------------------------------------------------------------------------------------------
// MARK: NumberBoxHelper

class NumberBoxHelper : public ControlHelper<NumberBox, NumberBoxHelper> {
	// Methods
	public:
							// Lifecycle methods
							NumberBoxHelper(NumberBox numberBox) : ControlHelper(numberBox) {}

							// Instance methods
		NumberBoxHelper&	setNumberFormatter(INumberFormatter2 numberFormatter)
								{ getNumberBox().NumberFormatter(numberFormatter); return *this; }

		NumberBoxHelper&	setMaximum(double value)
								{ getNumberBox().Maximum(value); return *this; }
		NumberBoxHelper&	setMaximum(int value)
								{ getNumberBox().Maximum(value); return *this; }
		NumberBoxHelper&	setMinimum(double value)
								{ getNumberBox().Minimum(value); return *this; }
		NumberBoxHelper&	setMinimum(int value)
								{ getNumberBox().Minimum(value); return *this; }

		NumberBoxHelper&	setValue(double value);
		NumberBoxHelper&	setValue(int value);

		bool				isValid() const
								{ return getNumberBox().Text().size() > 0; }

		NumberBoxHelper&	setBorderToDefault()
								{
									// Set to default
									setBorderBrush(
											Application::Current().Resources()
													.TryLookup(winrt::box_value(L"TextControlBorderBrush"))
													.as<Brush>());

									return *this;
								}
		NumberBoxHelper&	setBorderToError()
								{
									// Set to error
									setBorderBrush(SolidColorBrush(winrt::Windows::UI::Colors::Red()));

									return *this;
								}

		NumberBoxHelper&	setDoubleValueChangedProc(std::function<void(double value)> valueChangedProc);
		NumberBoxHelper&	setIntValueChangedProc(std::function<void(int value)> valueChangedProc);

		NumberBox			getNumberBox() const
								{ return getControl(); }
};
