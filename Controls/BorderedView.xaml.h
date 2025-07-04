//----------------------------------------------------------------------------------------------------------------------
//	BorderedView.xaml.h			©204 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "WinUIToolbox.BorderedView.g.h"

using UIElement = winrt::Microsoft::UI::Xaml::UIElement;

//----------------------------------------------------------------------------------------------------------------------
// MARK: - WinUIToolbox::implementation

namespace winrt::WinUIToolbox::implementation {

	// MARK: BorderedView
	struct BorderedView : BorderedViewT<BorderedView> {
		// Classes
		private:
			class	Internals;

		// Methods
		public:
					// Lifecycle methods
					BorderedView();
					~BorderedView();

					// Instance Methods
			void	SetBorderThickness(double left, double top, double right, double bottom);
			void	SetContent(UIElement uiElement);

		// Properties
		private:
			Internals*	mInternals;
	};
}

//----------------------------------------------------------------------------------------------------------------------
// MARK: - WinUIToolbox::factory_implementation

namespace winrt::WinUIToolbox::factory_implementation {

	// MARK: BorderedView
	struct BorderedView : BorderedViewT<BorderedView, implementation::BorderedView> {};
}
