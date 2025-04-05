//----------------------------------------------------------------------------------------------------------------------
//	SelectorBarHelper.h			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ControlHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.h"
#include "winrt\Windows.Foundation.Collections.h"

using SelectorBar = winrt::Microsoft::UI::Xaml::Controls::SelectorBar;
using SelectorBarItem = winrt::Microsoft::UI::Xaml::Controls::SelectorBarItem;
using SelectorBarSelectionChangedEventArgs = winrt::Microsoft::UI::Xaml::Controls::SelectorBarSelectionChangedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: SelectorBarHelper

class SelectorBarHelper : public ControlHelper<SelectorBar, SelectorBarHelper> {
	// Methods
	public:
							// Lifecycle methods
							SelectorBarHelper(SelectorBar selectorBar) : ControlHelper(selectorBar) {}
							SelectorBarHelper() : ControlHelper(SelectorBar()) {}
 
							// Instance methods
		SelectorBarHelper&	add(const SelectorBarItem& selectorBarItem)
								{ getSelectorBar().Items().Append(selectorBarItem); return *this; }
		SelectorBarHelper&	removeAll()
								{ getSelectorBar().Items().Clear(); return *this; }

		int					getSelectedItemIndex() const
								{
									// Get index
									uint32_t	index;
									getSelectorBar().Items().IndexOf(getSelectorBar().SelectedItem(), index);

									return index;
								}

		bool				getSelectedIntTag(int& outInt) const
								{
									// Get selected item
									auto	selectedItem = getSelectorBar().SelectedItem();
									if (!selectedItem)
										// No selected item
										return false;

									return getTagAsInt(selectedItem.Tag(), outInt);
								}
		int					getSelectedIntTag(int defaultValue) const
								{
									// Get selected item
									auto	selectedItem = getSelectorBar().SelectedItem();
									if (!selectedItem)
										// No selected item
										return defaultValue;

									// Get value
									int	value;

									return getTagAsInt(selectedItem.Tag(), value) ? value : defaultValue;
								}
		SelectorBarHelper&	setSelectedIntTag(int value)
								{
									// Iterate items
									for (auto const& item : getSelectorBar().Items()) {
										// Try to get tag
										int	tag;
										if (getTagAsInt(item.Tag(), tag) && (tag == value)) {
											// Found it
											getSelectorBar().SelectedItem(item);
											break;
										}
									}

									return *this;
								}

		SelectorBarHelper&	setSelectedItemChangedProc(
									std::function<void(int index, const SelectorBarItem& selectorBarItem)>
											selectedItemChangedProc)
								{
									// Setup
									getSelectorBar().SelectionChanged(
											[selectedItemChangedProc](const SelectorBar& selectorBar,
													const SelectorBarSelectionChangedEventArgs&
															selectorBarSelectionChangedEventArgs){
										// Get info
										auto		selectorBarItem = selectorBar.SelectedItem();

										uint32_t	index;
										selectorBar.Items().IndexOf(selectorBarItem, index);

										// Call proc
										selectedItemChangedProc(index, selectorBar.SelectedItem());
									});

									return *this;
								}

		SelectorBar			getSelectorBar() const
								{ return getControl(); }
};
