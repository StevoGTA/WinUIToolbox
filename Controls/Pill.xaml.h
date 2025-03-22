//----------------------------------------------------------------------------------------------------------------------
//	Pill.xaml.h			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "WinUIToolbox.Pill.g.h"

#include "winrt\Microsoft.UI.Xaml.h"

using Brush = winrt::Microsoft::UI::Xaml::Media::Brush;
using DependencyProperty = winrt::Microsoft::UI::Xaml::DependencyProperty;

//----------------------------------------------------------------------------------------------------------------------
// MARK: winrt::WinUIToolbox::implementation

namespace winrt::WinUIToolbox::implementation {

	// MARK: Pill
	struct Pill : PillT<Pill> {
		// Methods
		public:
										// Lifecycle methods
										Pill();

										// Instance methods
					Brush				Stroke() const;;
					void				Stroke(const Brush& brush);

					double				StrokeThickness() const;
					void				StrokeThickness(double strokeThickness);

										// Class methods
			static	DependencyProperty	StrokeProperty() noexcept;
			static	DependencyProperty	StrokeThicknessProperty() noexcept;


		// Properties
		private:
			static	DependencyProperty	mStrokeProperty;
			static	DependencyProperty	mStrokeThicknessProperty;
	};
}
//----------------------------------------------------------------------------------------------------------------------
// MARK: - winrt::WinUIToolbox::factory_implementation

namespace winrt::WinUIToolbox::factory_implementation
{
	// MARK: Pill
	struct Pill : PillT<Pill, implementation::Pill> {};
}
