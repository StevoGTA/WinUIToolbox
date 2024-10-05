//----------------------------------------------------------------------------------------------------------------------
//	LineHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ShapeHelper.h"

#include "winrt\Microsoft.UI.Xaml.Shapes.h"

using Binding = winrt::Microsoft::UI::Xaml::Data::Binding;
using BindingOperations = winrt::Microsoft::UI::Xaml::Data::BindingOperations;
using Line = winrt::Microsoft::UI::Xaml::Shapes::Line;
using PropertyPath = winrt::Microsoft::UI::Xaml::PropertyPath;

//----------------------------------------------------------------------------------------------------------------------
// MARK: LineHelper

class LineHelper : public ShapeHelper<Line, LineHelper> {
	// Methods
	public:
					// Lifecycle methods
					LineHelper(Line line) : ShapeHelper(line) {}
					LineHelper() : ShapeHelper(Line()) {}

					// Instance methods
		LineHelper&	setX(double value)
						{ getLine().X1(value); getLine().X2(value); return *this; }
		LineHelper&	setX1(double value)
						{ getLine().X1(value); return *this; }
		LineHelper&	setX2(double value)
						{ getLine().X2(value); return *this; }
		LineHelper&	matchWidth(double x1, FrameworkElement frameworkElement)
						{
							// Setup
							Binding	binding;
							binding.Path(PropertyPath(L"ActualWidth"));
							binding.Source(frameworkElement);

							// Set
							getLine().X1(x1);
							BindingOperations::SetBinding(getLine(), Line::X2Property(), binding);

							return *this;
						}

		LineHelper&	setY(double value)
						{ getLine().Y1(value); getLine().Y2(value); return *this; }
		LineHelper&	setY1(double value)
						{ getLine().Y1(value); return *this; }
		LineHelper&	setY2(double value)
						{ getLine().Y2(value); return *this; }
		LineHelper&	matchHeight(double y1, FrameworkElement frameworkElement)
						{
							// Setup
							Binding	binding;
							binding.Path(PropertyPath(L"ActualHeight"));
							binding.Source(frameworkElement);

							// Set
							getLine().Y1(y1);
							BindingOperations::SetBinding(getLine(), Line::Y2Property(), binding);

							return *this;
						}

					// Instance methods
		Line		getLine() const
						{ return getShape(); }
};
