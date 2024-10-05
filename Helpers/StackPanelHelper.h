//----------------------------------------------------------------------------------------------------------------------
//	StackPanelHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "FrameworkElementHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.h"
#include "winrt\Windows.Foundation.Collections.h"

using Orientation = winrt::Microsoft::UI::Xaml::Controls::Orientation;
using StackPanel = winrt::Microsoft::UI::Xaml::Controls::StackPanel;

//----------------------------------------------------------------------------------------------------------------------
// MARK: StackPanelHelper

class StackPanelHelper : public FrameworkElementHelper<StackPanel, StackPanelHelper> {
	// Methods
	public:
							// Lifecycle methods
							StackPanelHelper(StackPanel stackPanel) : FrameworkElementHelper(stackPanel) {}
							StackPanelHelper() : FrameworkElementHelper(StackPanel()) {}

							// Instance methods
		StackPanelHelper&	setOrientation(Orientation orientation)
								{ getStackPanel().Orientation(orientation); return *this; }
		StackPanelHelper&	setEnabled(bool enabled);
		StackPanelHelper&	add(UIElement uiElement)
								{ getStackPanel().Children().Append(uiElement); return *this; }

		StackPanel			getStackPanel() const
								{ return getFrameworkElement(); }
};
