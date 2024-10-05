//----------------------------------------------------------------------------------------------------------------------
//	GridHelper.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "GridHelper.h"

#include "StackPanelHelper.h"

#include "winrt\Windows.Foundation.Collections.h"

using Control = winrt::Microsoft::UI::Xaml::Controls::Control;
using StackPanel = winrt::Microsoft::UI::Xaml::Controls::StackPanel;

//----------------------------------------------------------------------------------------------------------------------
// MARK: GridHelper

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
GridHelper& GridHelper::add(UIElement uiElement, int columnIndex, int rowIndex)
//----------------------------------------------------------------------------------------------------------------------
{
	// Add
	getGrid().Children().Append(uiElement);

	// Setup
	uiElement.SetValue(Grid::ColumnProperty(), winrt::box_value(columnIndex));
	uiElement.SetValue(Grid::RowProperty(), winrt::box_value(rowIndex));

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
GridHelper& GridHelper::setEnabled(bool enabled)
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all children
	auto	children = getGrid().Children();
	for (uint32_t i = 0; i < children.Size(); i++) {
		// Get child
		auto	child = children.GetAt(i);

		// Check for Control
		auto	control = child.try_as<Control>();
		if (control) {
			// Have control
			control.IsEnabled(enabled);
			continue;
		}

		// Check for StackPanel
		auto	stackPanel = child.try_as<StackPanel>();
		if (stackPanel) {
			// Have StackPanel
			StackPanelHelper	stackPanelHelper(stackPanel);
			stackPanelHelper.setEnabled(enabled);
			continue;
		}
	}

	return *this;
}
