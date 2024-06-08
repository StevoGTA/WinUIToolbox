//----------------------------------------------------------------------------------------------------------------------
//	GridHelper.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "GridHelper.h"

#include "StackPanelHelper.h"

#include "winrt\Windows.Foundation.Collections.h"

using Control = winrt::Microsoft::UI::Xaml::Controls::Control;
using StackPanel = winrt::Microsoft::UI::Xaml::Controls::StackPanel;

//----------------------------------------------------------------------------------------------------------------------
// MARK: GridHelper::Internals

class GridHelper::Internals {
	public:
		Internals(Grid grid) : mGrid(grid) {}

		Grid	mGrid;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: GridHelper

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
GridHelper::GridHelper(Grid grid)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(grid);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
void GridHelper::setEnabled(bool enabled)
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all children
	auto	children = mInternals->mGrid.Children();
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
			stackPanelHelper.IsEnabled(enabled);
			continue;
		}
	}
}
