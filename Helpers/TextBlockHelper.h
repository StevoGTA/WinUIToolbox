//----------------------------------------------------------------------------------------------------------------------
//	TextBlockHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "FrameworkElementHelper.h"

#include "winrt\Microsoft.UI.Xaml.h"
#include "winrt\Microsoft.UI.Xaml.Controls.h"

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
		TextBlockHelper&	setText(const winrt::hstring& text)
								{ getTextBlock().Text(text); return *this; }
		TextBlockHelper&	setTextWrapping(TextWrapping textWrapping)
								{ getTextBlock().TextWrapping(textWrapping); return *this; }

		TextBlock			getTextBlock() const
								{ return getFrameworkElement(); }
};
