//----------------------------------------------------------------------------------------------------------------------
//	ControlHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "FrameworkElementHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.h"
#include "winrt\Microsoft.UI.Xaml.Media.h"
#include "winrt\Windows.UI.h"

#include <functional>

using Brush = winrt::Microsoft::UI::Xaml::Media::Brush;
using Color = winrt::Windows::UI::Color;
using DependencyPropertyChangedEventArgs = winrt::Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs;
using FontIcon = winrt::Microsoft::UI::Xaml::Controls::FontIcon;
using SolidColorBrush = winrt::Microsoft::UI::Xaml::Media::SolidColorBrush;
using Thickness = winrt::Microsoft::UI::Xaml::Thickness;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ControlHelper

template <typename C, typename H> class ControlHelper : public FrameworkElementHelper<C, H> {
	// Methods
	public:
			// Lifecycle methods
			ControlHelper(C c) : FrameworkElementHelper(c) {}
 
			// Instance methods
		H&	setBackground(const Brush& brush)
				{ getControl().Background(brush); return (H&) *this; }
		H&	setBackground(const Color& color)
				{ getControl().Background(SolidColorBrush(color)); return (H&) *this; }

		H&	setBorderBrush(const Brush& brush)
				{ getControl().BorderBrush(brush); return (H&) *this; }
		H&	setBorderBrush(const Color& color)
				{ getControl().BorderBrush(SolidColorBrush(color)); return (H&) *this; }

		H&	setBorderThickness(double uniformLength)
				{ getControl().BorderThickness(ThicknessHelper::FromUniformLength(uniformLength)); return (H&) *this; }
		H&	setBorderThickness(double left, double top, double right, double bottom)
				{ getControl().BorderThickness(ThicknessHelper::FromLengths(left, top, right, bottom));
						return (H&) *this; }

		H&	setContentAsFontIcon(const winrt::param::hstring& glyph, double fontSize) const
				{
					// Create FontIcon
					auto	fontIcon = FontIcon();
					fontIcon.Glyph(glyph);
					fontIcon.FontSize(fontSize);

					// Set Content
					getControl().Content(fontIcon);

					return (H&) *this;
				}

		H&	setEnabled(bool enabled)
				{ getControl().IsEnabled(enabled); return (H&) *this; }
		H&	setEnabledChanged(std::function<void(bool isEnabled)> enabledChangedProc)
				{
					// Set
					getControl().IsEnabledChanged(
							[this](const IInspectable& sender, const DependencyPropertyChangedEventArgs& args){
								// Call proc
								enabledChangedProc(sender.as<C>().IsEnabled());
							});

					return (H&) *this;
				}

		C	getControl() const
				{ return getFrameworkElement(); }
};
