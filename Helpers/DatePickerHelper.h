//----------------------------------------------------------------------------------------------------------------------
//	DatePickerHelper.h			©2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ControlHelper.h"

using DatePicker = winrt::Microsoft::UI::Xaml::Controls::DatePicker;
using DateTime = winrt::Windows::Foundation::DateTime;

//----------------------------------------------------------------------------------------------------------------------
// MARK: DatePickerHelper

class DatePickerHelper : public ControlHelper<DatePicker, DatePickerHelper> {
	// Methods
	public:
							// Lifecycle methods
							DatePickerHelper(DatePicker datePicker) : ControlHelper(datePicker) {}
							DatePickerHelper() : ControlHelper(DatePicker()) {}
	
							// Instance methods
		DatePickerHelper&	setDateTime(const DateTime& dateTime);
		DatePickerHelper&	setChangedProc(
									std::function<void(const DateTime& oldDateTime, const DateTime& newDateTIme)>
											changedProc);

		DatePicker			getDatePicker() const
								{ return getControl(); }
};
