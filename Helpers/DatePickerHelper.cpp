//----------------------------------------------------------------------------------------------------------------------
//	DatePickerHelper.cpp			©2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "DatePickerHelper.h"

#include <winrt\Windows.Foundation.h>

using DatePickerValueChangedEventArgs = winrt::Microsoft::UI::Xaml::Controls::DatePickerValueChangedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	DatePicker	sDatePickerInUpdate(nullptr);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - DatePickerHelper

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
DatePickerHelper& DatePickerHelper::setDateTime(const DateTime& dateTime)
//----------------------------------------------------------------------------------------------------------------------
{
	// Set value
	sDatePickerInUpdate = getDatePicker();
	getDatePicker().Date(dateTime);
	sDatePickerInUpdate = nullptr;

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
DatePickerHelper& DatePickerHelper::setChangedProc(
		std::function<void(const DateTime& oldDateTime, const DateTime& newDateTIme)> changedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	getDatePicker().DateChanged(
			[changedProc](const IInspectable& sender,
					const DatePickerValueChangedEventArgs& datePickerValueChangedEventArgs){
						// Check if handling events
						if (sender == sDatePickerInUpdate)
							return;

						// Call proc
						changedProc(datePickerValueChangedEventArgs.OldDate(),
								datePickerValueChangedEventArgs.NewDate());
					});

	return *this;
}
