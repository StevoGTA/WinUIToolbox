//----------------------------------------------------------------------------------------------------------------------
//	GridHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "FrameworkElementHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.h"

using Grid = winrt::Microsoft::UI::Xaml::Controls::Grid;

//----------------------------------------------------------------------------------------------------------------------
// MARK: GridHelper

class GridHelper : public FrameworkElementHelper<Grid, GridHelper> {
	// Methods
	public:
					// Lifecycle methods
					GridHelper(Grid grid) : FrameworkElementHelper(grid) {}
					GridHelper() : FrameworkElementHelper(Grid()) {}

					// Instance methods
		GridHelper&	add(UIElement uiElement, int columnIndex, int rowIndex);

		GridHelper&	setEnabled(bool enabled);

		Grid		getGrid() const
						{ return getFrameworkElement(); }
};
