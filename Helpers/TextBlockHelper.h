//----------------------------------------------------------------------------------------------------------------------
//	TextBlockHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "FrameworkElementHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.h"

using TextBlock = winrt::Microsoft::UI::Xaml::Controls::TextBlock;

//----------------------------------------------------------------------------------------------------------------------
// MARK: TextBlockHelper

class TextBlockHelper : public FrameworkElementHelper<TextBlock, TextBlockHelper> {
	// Methods
	public:
							// Lifecycle methods
							TextBlockHelper(TextBlock textBlock) : FrameworkElementHelper(textBlock) {}
							TextBlockHelper() : FrameworkElementHelper(TextBlock()) {}

							// Instance methods
		TextBlockHelper&	setText(const hstring& text)
								{ getTextBlock().Text(text); return *this; }


		TextBlock			getTextBlock() const
								{ return getFrameworkElement(); }
};
