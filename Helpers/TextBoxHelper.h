//----------------------------------------------------------------------------------------------------------------------
//	TextBoxHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ControlHelper.h"

#include <functional>

using FocusState = winrt::Microsoft::UI::Xaml::FocusState;
using hstring = winrt::hstring;
using TextBox = winrt::Microsoft::UI::Xaml::Controls::TextBox;

//----------------------------------------------------------------------------------------------------------------------
// MARK: TextBoxHelper

class TextBoxHelper : public ControlHelper<TextBox, TextBoxHelper> {
	// Methods
	public:
						// Lifecycle methods
						TextBoxHelper(TextBox textBox) : ControlHelper(textBox) {}

						// Instance methods
		TextBoxHelper&	setText(const hstring& string);
		TextBoxHelper&	setFocusState(FocusState focusState);
		TextBoxHelper&	setTextChangedProc(std::function<void(const winrt::hstring& string)> textChangedProc);

		TextBox			getTextBox() const
							{ return getControl(); }
};
