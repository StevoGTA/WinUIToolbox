//----------------------------------------------------------------------------------------------------------------------
//	StackPanelHelper.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "StackPanelHelper.h"

#include "winrt\Windows.Foundation.Collections.h"

using Control = winrt::Microsoft::UI::Xaml::Controls::Control;

//----------------------------------------------------------------------------------------------------------------------
// MARK: StackPanelHelper::Internals

class StackPanelHelper::Internals {
	public:
		Internals(StackPanel stackPanel) : mStackPanel(stackPanel) {}

		StackPanel	mStackPanel;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: StackPanelHelper

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
StackPanelHelper::StackPanelHelper(StackPanel stackPanel)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(stackPanel);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
void StackPanelHelper::IsEnabled(bool enabled)
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all children
	auto	children = mInternals->mStackPanel.Children();
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
