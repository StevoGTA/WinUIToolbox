//----------------------------------------------------------------------------------------------------------------------
//	FontIconHelper.h			©2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "FrameworkElementHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.h"

using FontIcon = winrt::Microsoft::UI::Xaml::Controls::FontIcon;

//----------------------------------------------------------------------------------------------------------------------
// MARK: FontIconHelper

class FontIconHelper : public FrameworkElementHelper<FontIcon, FontIconHelper> {
	// Methods
	public:
					// Lifecycle methods
					FontIconHelper(FontIcon fontIcon) : FrameworkElementHelper(fontIcon) {}
					FontIconHelper(const winrt::param::hstring& glyph, double fontSize) :
						FrameworkElementHelper(FontIcon())
						{
							// Setup
							getFontIcon().Glyph(glyph);
							getFontIcon().FontSize(fontSize);
						}

					// Instance methods
		FontIcon	getFontIcon() const
						{ return getFrameworkElement(); }
};
