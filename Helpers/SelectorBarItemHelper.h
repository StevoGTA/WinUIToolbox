//----------------------------------------------------------------------------------------------------------------------
//	SelectorBarItemHelper.h			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ControlHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.h"

using IconElement = winrt::Microsoft::UI::Xaml::Controls::IconElement;
using SelectorBarItem = winrt::Microsoft::UI::Xaml::Controls::SelectorBarItem;
using Symbol = winrt::Microsoft::UI::Xaml::Controls::Symbol;
using SymbolIcon = winrt::Microsoft::UI::Xaml::Controls::SymbolIcon;

//----------------------------------------------------------------------------------------------------------------------
// MARK: SelectorBarItemHelper

class SelectorBarItemHelper : public ControlHelper<SelectorBarItem, SelectorBarItemHelper> {
	// Methods
	public:
								// Lifecycle methods
								SelectorBarItemHelper(SelectorBarItem selectorBarItem) :
									ControlHelper(selectorBarItem)
									{}
								SelectorBarItemHelper() : ControlHelper(SelectorBarItem()) {}
 
								// Instance methods
		SelectorBarItemHelper&	setIcon(IconElement iconElement)
									{ getSelectorBarItem().Icon(iconElement); return *this; }
		SelectorBarItemHelper&	setSymbolIcon(Symbol symbol)
									{ return setIcon(SymbolIcon(symbol)); }

		SelectorBarItem			getSelectorBarItem() const
									{ return getControl(); }
};
