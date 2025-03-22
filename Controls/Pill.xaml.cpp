//----------------------------------------------------------------------------------------------------------------------
//	Pill.xaml.cpp			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "Pill.xaml.h"

#include "winrt\Windows.UI.Xaml.Interop.h"
#include "winrt\Microsoft.UI.Xaml.Media.h"
#include "winrt\Microsoft.UI.Xaml.Shapes.h"
#include "winrt\Windows.Foundation.Collections.h"

#include "WinUIToolbox.Pill.g.cpp"

using namespace winrt::WinUIToolbox::implementation;
using namespace winrt::Microsoft::UI::Xaml;

using SolidColorBrush = winrt::Microsoft::UI::Xaml::Media::SolidColorBrush;

//----------------------------------------------------------------------------------------------------------------------
// MARK: Pill

// MARK: Properties

DependencyProperty	Pill::mStrokeProperty =
							DependencyProperty::Register(L"Stroke", winrt::xaml_typename<Brush>(),
									winrt::xaml_typename<winrt::WinUIToolbox::Pill>(), nullptr);
DependencyProperty	Pill::mStrokeThicknessProperty =
							DependencyProperty::Register(L"StrokeThickness", winrt::xaml_typename<double>(),
									winrt::xaml_typename<winrt::WinUIToolbox::Pill>(),
									PropertyMetadata{ winrt::box_value(1.0) });

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
Pill::Pill() : PillT<Pill>()
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup Loaded event handler
	SizeChanged([this](const IInspectable& sender, const RoutedEventArgs& routedEventArgs)
			{
				// Setup
				auto	width = ActualWidth();
				auto	height = ActualHeight();
				Brush	stroke = Stroke();

				// Create rectangle
				Shapes::Rectangle	rectangle = Shapes::Rectangle();
				rectangle.Width(width);
				rectangle.Height(height);
				rectangle.RadiusX(height / 2.0);
				rectangle.RadiusY(height / 2.0);
				rectangle.Stroke(stroke ? stroke : SolidColorBrush(winrt::Windows::UI::Colors::White()));
				rectangle.StrokeThickness(StrokeThickness());

				// Update children
				Children().Append(rectangle);
			});
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
Brush Pill::Stroke() const
//----------------------------------------------------------------------------------------------------------------------
{
	return GetValue(StrokeProperty()).as<Brush>();
}

//----------------------------------------------------------------------------------------------------------------------
void Pill::Stroke(const Brush& brush)
//----------------------------------------------------------------------------------------------------------------------
{
	SetValue(StrokeProperty(), brush);
}

//----------------------------------------------------------------------------------------------------------------------
double Pill::StrokeThickness() const
//----------------------------------------------------------------------------------------------------------------------
{
	return winrt::unbox_value<double>(GetValue(StrokeThicknessProperty()));
}

//----------------------------------------------------------------------------------------------------------------------
void Pill::StrokeThickness(double strokeThickness)
//----------------------------------------------------------------------------------------------------------------------
{
	SetValue(StrokeThicknessProperty(), winrt::box_value(strokeThickness));
}

// MARK: Class methods

//----------------------------------------------------------------------------------------------------------------------
DependencyProperty Pill::StrokeProperty() noexcept
//----------------------------------------------------------------------------------------------------------------------
{
	return mStrokeProperty;
}

//----------------------------------------------------------------------------------------------------------------------
DependencyProperty Pill::StrokeThicknessProperty() noexcept
//----------------------------------------------------------------------------------------------------------------------
{
	return mStrokeThicknessProperty;
}
