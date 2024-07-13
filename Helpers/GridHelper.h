//----------------------------------------------------------------------------------------------------------------------
//	GridHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "winrt\Microsoft.UI.Xaml.Controls.h"

using Grid = winrt::Microsoft::UI::Xaml::Controls::Grid;

//----------------------------------------------------------------------------------------------------------------------
// MARK: GridHelper

class GridHelper {
	// Classes
	private:
		class Internals;

	// Methods
	public:
					// Lifecycle methods
					GridHelper(Grid grid);

					// Instance methods
		GridHelper&	setEnabled(bool enabled);

	// Properties
	private:
		Internals*	mInternals;
};
