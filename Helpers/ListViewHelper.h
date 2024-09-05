//----------------------------------------------------------------------------------------------------------------------
//	ListViewHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "winrt\Microsoft.UI.Xaml.Controls.h"

#include <functional>

using ListView = winrt::Microsoft::UI::Xaml::Controls::ListView;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ListViewHelper

class ListViewHelper {
	// Classes
	private:
		class Internals;

	// Methods
	public:
						// Lifecycle methods
						ListViewHelper(ListView listView);

						// Instance methods
		ListViewHelper&	setSelectionChangedProc(std::function<void()> selectionChangedProc);

	// Properties
	private:
		Internals*	mInternals;
};
