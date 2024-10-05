//----------------------------------------------------------------------------------------------------------------------
//	ShapeHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "FrameworkElementHelper.h"

#include "winrt\Microsoft.UI.Xaml.Shapes.h"

using Brush = winrt::Microsoft::UI::Xaml::Media::Brush;
using Shape = winrt::Microsoft::UI::Xaml::Shapes::Shape;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ShapeHelper

template <typename C, typename H> class ShapeHelper : public FrameworkElementHelper<C, H> {
	// Methods
	public:
			// Lifecycle methods
			ShapeHelper(C c) : FrameworkElementHelper(c) {}

			// Instance methods
		//H&	setStroke(const hstring& brushResourceName)
		//		{ getShape().Stroke(winrt::Application::Current().Resources().Lookup(winrt::box_value(brushResourceName).as<Brush>()));
		//				return (H&) *this; }
		H&	setStrokeThickness(double value)
				{ getShape().StrokeThickness(value); return (H&) *this; }

		C	getShape() const
				{ return getFrameworkElement(); }
};
