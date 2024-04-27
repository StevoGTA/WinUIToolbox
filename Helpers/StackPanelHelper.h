//----------------------------------------------------------------------------------------------------------------------
//	StackPanelHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "winrt\Microsoft.UI.Xaml.Controls.h"

using StackPanel = winrt::Microsoft::UI::Xaml::Controls::StackPanel;

//----------------------------------------------------------------------------------------------------------------------
// MARK: StackPanelHelper

class StackPanelHelper {
	// Classes
	private:
		class Internals;

	// Methods
	public:
				// Lifecycle methods
				StackPanelHelper(StackPanel stackPanel);

				// Instance methods
		void	setEnabled(bool enabled);

	// Properties
	private:
		Internals*	mInternals;
};
