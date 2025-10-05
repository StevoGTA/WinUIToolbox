//----------------------------------------------------------------------------------------------------------------------
//	DragAndDropCoordinator.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "winrt\Microsoft.UI.Xaml.h"
#include "winrt\Microsoft.UI.Xaml.Controls.h"

using Control = winrt::Microsoft::UI::Xaml::Controls::Control;
using DragEventArgs = winrt::Microsoft::UI::Xaml::DragEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: DragAndDropCoordinator

class DragAndDropCoordinator {
	// Classes
	private:
		class	Internals;

	// Methods
	public:
										// Lifecycle methods
										DragAndDropCoordinator();

										// Instance methods
				void					addHandler(const Control& handler, int priority) const;
				void					removeHandler(const Control& handler) const;
				bool					shouldHandleDrag(const Control& handler) const;
				bool					shouldHandleDrop(const Control& handler, const DragEventArgs& dragEventArgs)
												const;

										// Class methods
		static	DragAndDropCoordinator&	shared();

	// Properties
	private:
		Internals*	mInternals;
};
