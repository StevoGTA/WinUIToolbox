//----------------------------------------------------------------------------------------------------------------------
//	TextBlockHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "FrameworkElementHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.h"
#include "winrt\Microsoft.UI.Xaml.Media.h"
#include "winrt\Windows.UI.h"
#include "winrt\Windows.Foundation.h"

using Application = winrt::Microsoft::UI::Xaml::Application;
using Brush = winrt::Microsoft::UI::Xaml::Media::Brush;
using Color = winrt::Windows::UI::Color;
using FontStyle = winrt::Windows::UI::Text::FontStyle;
using SolidColorBrush = winrt::Microsoft::UI::Xaml::Media::SolidColorBrush;
using TextBlock = winrt::Microsoft::UI::Xaml::Controls::TextBlock;
using TextWrapping = winrt::Microsoft::UI::Xaml::TextWrapping;

//----------------------------------------------------------------------------------------------------------------------
// MARK: TextBlockHelper

class TextBlockHelper : public FrameworkElementHelper<TextBlock, TextBlockHelper> {
	// Methods
	public:
							// Lifecycle methods
							TextBlockHelper(TextBlock textBlock) : FrameworkElementHelper(textBlock) {}
							TextBlockHelper() : FrameworkElementHelper(TextBlock()) {}

							// Instance methods
		TextBlockHelper&	setFontStyle(FontStyle fontStyle)
								{ getTextBlock().FontStyle(fontStyle); return *this; }

		TextBlockHelper&	setForeground(const Brush& brush)
								{ getTextBlock().Foreground(brush); return *this; }
		TextBlockHelper&	setForeground(const Color& color)
								{ getTextBlock().Foreground(SolidColorBrush(color)); return *this; }
		TextBlockHelper&	setForegroundToDefault()
								{
									// Set to default
									setForeground(
											Application::Current().Resources()
													.TryLookup(winrt::box_value(L"DefaultTextForegroundThemeBrush"))
													.as<Brush>());

									return *this;
								}

		TextBlockHelper&	setText(const winrt::hstring& text)
								{ getTextBlock().Text(text); return *this; }
		TextBlockHelper&	setTextWrapping(TextWrapping textWrapping)
								{ getTextBlock().TextWrapping(textWrapping); return *this; }

		TextBlock			getTextBlock() const
								{ return getFrameworkElement(); }
};
