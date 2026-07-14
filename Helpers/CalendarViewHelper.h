//----------------------------------------------------------------------------------------------------------------------
//	CalendarViewHelper.h			©2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ControlHelper.h"

#include "winrt\Windows.Foundation.Collections.h"

using CalendarView = winrt::Microsoft::UI::Xaml::Controls::CalendarView;
using DateTime = winrt::Windows::Foundation::DateTime;
using DateTimes = winrt::Windows::Foundation::Collections::IVectorView<DateTime>;

//----------------------------------------------------------------------------------------------------------------------
// MARK: CalendarViewHelper

class CalendarViewHelper : public ControlHelper<CalendarView, CalendarViewHelper> {
	// Methods
	public:
							// Lifecycle methods
							CalendarViewHelper(CalendarView calendarView) : ControlHelper(calendarView) {}
							CalendarViewHelper() : ControlHelper(CalendarView()) {}

							// Instance methods
		CalendarViewHelper&	setSelectedDateTime(const DateTime& dateTime);
		CalendarViewHelper&	setChangedProc(
									std::function<void(const DateTimes& addedDates, const DateTimes& removedDates)>
											changedProc);

		CalendarView		getCalendarView() const
								{ return getControl(); }
};
