//----------------------------------------------------------------------------------------------------------------------
//	SelectorBarHelper.cpp			©2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "SelectorBarHelper.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	SelectorBar	sSelectorBar(nullptr);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - SelectorBarHelper

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
SelectorBarHelper& SelectorBarHelper::setSelectedIntTag(int value)
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate items
	for (auto const& item : getSelectorBar().Items()) {
		// Try to get tag
		int	tag;
		if (getTagAsInt(item.Tag(), tag) && (tag == value)) {
			// Found it
			sSelectorBar = getSelectorBar();
			getSelectorBar().SelectedItem(item);
			sSelectorBar = nullptr;
			break;
		}
	}

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
SelectorBarHelper& SelectorBarHelper::setSelectedItemChangedProc(
		std::function<void(int index, const SelectorBarItem& selectorBarItem)>
				selectedItemChangedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	getSelectorBar().SelectionChanged(
			[selectedItemChangedProc](const SelectorBar& selectorBar,
					const SelectorBarSelectionChangedEventArgs& selectorBarSelectionChangedEventArgs){
		// Check if handling events
		if (selectorBar == sSelectorBar)
			return;

		// Get info
		auto		selectorBarItem = selectorBar.SelectedItem();

		uint32_t	index;
		selectorBar.Items().IndexOf(selectorBarItem, index);

		// Call proc
		selectedItemChangedProc(index, selectorBar.SelectedItem());
	});

	return *this;
}
