//----------------------------------------------------------------------------------------------------------------------
//	DispatcherQueueHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "winrt\Microsoft.UI.Dispatching.h"

#include <functional>

using DispatcherQueue = winrt::Microsoft::UI::Dispatching::DispatcherQueue;

//----------------------------------------------------------------------------------------------------------------------
// MARK: DispatcherQueueHelper

class DispatcherQueueHelper {
	// Classes
	private:
		class Internals;

	// Methods
	public:
								// Lifecycle methods
								DispatcherQueueHelper(
										DispatcherQueue dispatcherQueue = DispatcherQueue::GetForCurrentThread());

								// Instance methods
		DispatcherQueueHelper&	tryEnqueueAfterMilliseconds(std::function<void()> proc, long long milliseconds);

	// Properties
	private:
		Internals*	mInternals;
};
