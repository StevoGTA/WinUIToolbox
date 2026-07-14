//----------------------------------------------------------------------------------------------------------------------
//	TimePickerHelper.cpp			©2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "TimePickerHelper.h"

#include <winrt\Windows.Foundation.h>

using TimePickerValueChangedEventArgs = winrt::Microsoft::UI::Xaml::Controls::TimePickerValueChangedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	TimePicker	sTimePickerInUpdate(nullptr);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - TimePickerHelper

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
TimePickerHelper& TimePickerHelper::setTimeSpan(const TimeSpan& timeSpan)
//----------------------------------------------------------------------------------------------------------------------
{
	// Set value
	sTimePickerInUpdate = getTimePicker();
	getTimePicker().Time(timeSpan);
	sTimePickerInUpdate = nullptr;

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
TimePickerHelper& TimePickerHelper::setChangedProc(
		std::function<void(const TimeSpan& oldTimeSpan, const TimeSpan& newTimeSpan)> changedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	getTimePicker().TimeChanged(
			[changedProc](const IInspectable& sender,
					const TimePickerValueChangedEventArgs& timePickerValueChangedEventArgs){
						// Check if handling events
						if (sender == sTimePickerInUpdate)
							return;

						// Call proc
						changedProc(timePickerValueChangedEventArgs.OldTime(),
								timePickerValueChangedEventArgs.NewTime());
					});

	return *this;
}
