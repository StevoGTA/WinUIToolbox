//----------------------------------------------------------------------------------------------------------------------
//	DragAndDropCoordinator.cpp			©2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "DragAndDropCoordinator.h"

#include <unordered_map>

//----------------------------------------------------------------------------------------------------------------------
// MARK: ControlHash

struct ControlHash {
	public:
		std::size_t operator()(const Control& control) const noexcept
						{ return std::hash<void*>{}(winrt::get_abi(control)); }
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - ControlEqual

struct ControlEqual {
	public:
		bool	operator()(const Control& control1, const Control& control2) const noexcept
					{ return winrt::get_abi(control1) == winrt::get_abi(control2); }
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - DragAndDropCoordinator::Internals

class DragAndDropCoordinator::Internals {
	public:
		Internals() {}

		std::unordered_map<Control, int, ControlHash, ControlEqual>	mPriorityByControl;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - DragAndDropCoordinator

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
DragAndDropCoordinator::DragAndDropCoordinator()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals();
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
void DragAndDropCoordinator::addHandler(const Control& handler, int priority) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Add
	mInternals->mPriorityByControl[handler] = priority;
}

//----------------------------------------------------------------------------------------------------------------------
void DragAndDropCoordinator::removeHandler(const Control& handler) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Remove
	mInternals->mPriorityByControl.erase(handler);
}

//----------------------------------------------------------------------------------------------------------------------
bool DragAndDropCoordinator::shouldHandleDrag(const Control& handler) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if this handler has the highest priority
	auto	entry = mInternals->mPriorityByControl.find(handler);
	if (entry == mInternals->mPriorityByControl.end())
		// Not found
		return false;

	// Get priority
	auto	objectPriority = entry->second;

	// Check for higher priority
	for (const auto& entry : mInternals->mPriorityByControl) {
		// Check this entry
		if (entry.second > objectPriority)
			// Higher priority found
			return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DragAndDropCoordinator::shouldHandleDrop(const Control& handler, const DragEventArgs& dragEventArgs) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if drag has been handled
	if (!dragEventArgs.Handled()) {
		// Check if this handler has the highest priority
		auto	entry = mInternals->mPriorityByControl.find(handler);
		if (entry == mInternals->mPriorityByControl.end())
			// Not found
			return false;

		// Get priority
		auto	objectPriority = entry->second;

		// Remove drop handler
		mInternals->mPriorityByControl.erase(entry);

		// Check for higher priority
		for (const auto& entry : mInternals->mPriorityByControl) {
			// Check this entry
			if (entry.second > objectPriority)
				// Higher priority found
				return false;
		}

		// Drop is (will be) handled
		dragEventArgs.Handled(true);

		return true;
	} else {
		// Drag has already been handled
		mInternals->mPriorityByControl.erase(handler);

		return false;
	}
}

// MARK: Class methods

//----------------------------------------------------------------------------------------------------------------------
DragAndDropCoordinator& DragAndDropCoordinator::shared()
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	static	DragAndDropCoordinator	sDragAndDropCoordinator;

	return sDragAndDropCoordinator;
}