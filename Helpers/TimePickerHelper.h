//----------------------------------------------------------------------------------------------------------------------
//	TimePickerHelper.h			©2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ControlHelper.h"

using TimePicker = winrt::Microsoft::UI::Xaml::Controls::TimePicker;
using TimeSpan = winrt::Windows::Foundation::TimeSpan;

//----------------------------------------------------------------------------------------------------------------------
// MARK: TimePickerHelper

class TimePickerHelper : public ControlHelper<TimePicker, TimePickerHelper> {
	// Methods
	public:
							// Lifecycle methods
							TimePickerHelper(TimePicker timePicker) : ControlHelper(timePicker) {}
							TimePickerHelper() : ControlHelper(TimePicker()) {}

							// Instance methods
		TimePickerHelper&	setTimeSpan(const TimeSpan& timeSpan);
		TimePickerHelper&	setChangedProc(
									std::function<void(const TimeSpan& oldTimeSpan, const TimeSpan& newTimeSpan)>
											changedProc);

		TimePicker			getTimePicker() const
								{ return getControl(); }
};
