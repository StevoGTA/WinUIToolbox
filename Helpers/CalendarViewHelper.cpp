//----------------------------------------------------------------------------------------------------------------------
//	CalendarViewHelper.cpp			©2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CalendarViewHelper.h"

using CalendarViewSelectedDatesChangedEventArgs =
			winrt::Microsoft::UI::Xaml::Controls::CalendarViewSelectedDatesChangedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	CalendarView	sCalendarViewInUpdate(nullptr);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CalendarPickerHelper

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
CalendarViewHelper& CalendarViewHelper::setSelectedDateTime(const DateTime& dateTime)
//----------------------------------------------------------------------------------------------------------------------
{
	// Set value
	sCalendarViewInUpdate = getCalendarView();

	auto	selectedDates = getCalendarView().SelectedDates();
	selectedDates.Clear();
	selectedDates.Append(dateTime);

	sCalendarViewInUpdate = nullptr;

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
CalendarViewHelper& CalendarViewHelper::setChangedProc(
		std::function<void(const DateTimes& addedDates, const DateTimes& removedDates)> changedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	getCalendarView().SelectedDatesChanged(
			[changedProc](const IInspectable& sender,
					const CalendarViewSelectedDatesChangedEventArgs& calendarViewSelectedDatesChangedEventArgs){
						// Check if handling events
						if (sender == sCalendarViewInUpdate)
							return;

						// Call proc
						changedProc(calendarViewSelectedDatesChangedEventArgs.AddedDates(),
								calendarViewSelectedDatesChangedEventArgs.RemovedDates());
					});

	return *this;
}
