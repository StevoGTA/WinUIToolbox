//----------------------------------------------------------------------------------------------------------------------
//	DispatcherQueueHelper.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "DispatcherQueueHelper.h"

using namespace winrt;

//----------------------------------------------------------------------------------------------------------------------
// MARK: DispatcherQueueHelper::Internals

class DispatcherQueueHelper::Internals {
	public:
		Internals(DispatcherQueue dispatcherQueue) : mDispatcherQueue(dispatcherQueue) {}

		DispatcherQueue	mDispatcherQueue;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: DispatcherQueueHelper

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
DispatcherQueueHelper::DispatcherQueueHelper(DispatcherQueue dispatcherQueue)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(dispatcherQueue);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
DispatcherQueueHelper& DispatcherQueueHelper::tryEnqueueAfterMilliseconds(std::function<void()> proc,
		long long milliseconds)
//----------------------------------------------------------------------------------------------------------------------
{
	// Queue on detached thread
	DispatcherQueue	dispatcherQueue = mInternals->mDispatcherQueue;
	std::thread([dispatcherQueue, proc, milliseconds]() {
		// Delay
		std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));

		// Call proc on DispatcherQueue
		dispatcherQueue.TryEnqueue([proc](){ proc(); });
	}).detach();

	return *this;
}
