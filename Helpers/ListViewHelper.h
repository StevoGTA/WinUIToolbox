//----------------------------------------------------------------------------------------------------------------------
//	ListViewHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ControlHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.h"

#include <functional>

using ListView = winrt::Microsoft::UI::Xaml::Controls::ListView;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ListViewHelper

class ListViewHelper : public ControlHelper<ListView, ListViewHelper> {
	// Methods
	public:
						// Lifecycle methods
						ListViewHelper(ListView listView) : ControlHelper(listView) {}

						// Instance methods
		ListViewHelper&	setSelectionChangedProc(std::function<void()> selectionChangedProc);

		ListView		getListView() const
							{ return getControl(); }
};
