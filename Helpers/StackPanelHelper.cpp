//----------------------------------------------------------------------------------------------------------------------
//	StackPanelHelper.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "StackPanelHelper.h"

#include "winrt\Windows.Foundation.Collections.h"

using Control = winrt::Microsoft::UI::Xaml::Controls::Control;

//----------------------------------------------------------------------------------------------------------------------
// MARK: StackPanelHelper

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
StackPanelHelper& StackPanelHelper::setEnabled(bool enabled)
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all children
	auto	children = getStackPanel().Children();
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
