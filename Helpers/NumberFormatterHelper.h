//----------------------------------------------------------------------------------------------------------------------
//	NumberFormatterHelper.h			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "winrt\Windows.Globalization.NumberFormatting.h"

using namespace winrt::Windows::Globalization::NumberFormatting;

//----------------------------------------------------------------------------------------------------------------------
// MARK: NumberFormatterHelper

struct NumberFormatterHelper {
	// Methods
	public:
									// Class methods
		static	INumberFormatter2	integer()
										{
											// Setup Number Rounder
											auto    numberRounder = IncrementNumberRounder();
											numberRounder.RoundingAlgorithm(RoundingAlgorithm::RoundHalfUp);

											// Setup Number Formatter
											auto    numberFormatter = DecimalFormatter();
											numberFormatter.NumberRounder(numberRounder);
											numberFormatter.FractionDigits(0);
											numberFormatter.IsDecimalPointAlwaysDisplayed(false);
											numberFormatter.IsGrouped(false);

											return numberFormatter;
									   }
};
