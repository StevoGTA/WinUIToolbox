//----------------------------------------------------------------------------------------------------------------------
//	GridHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "FrameworkElementHelper.h"

#include "winrt\Windows.UI.h"

#include "winrt\Microsoft.UI.Xaml.Controls.h"

using Brush = winrt::Microsoft::UI::Xaml::Media::Brush;
using Color = winrt::Windows::UI::Color;
using Grid = winrt::Microsoft::UI::Xaml::Controls::Grid;
using SolidColorBrush = winrt::Microsoft::UI::Xaml::Media::SolidColorBrush;

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

		GridHelper&	setBorderBrush(const Brush& brush)
						{ getGrid().BorderBrush(brush); return *this; }
		GridHelper&	setBorderBrush(const Color& color)
						{ getGrid().BorderBrush(SolidColorBrush(color)); return *this; }

		GridHelper&	setEnabled(bool enabled);

		Grid		getGrid() const
						{ return getFrameworkElement(); }
};
