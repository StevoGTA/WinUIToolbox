//----------------------------------------------------------------------------------------------------------------------
//	TextBoxHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CString.h"

#undef Delete

#include "ControlHelper.h"

#define Delete(x)	{ delete x; x = nil; }

#include <functional>

using FocusState = winrt::Microsoft::UI::Xaml::FocusState;
using TextBox = winrt::Microsoft::UI::Xaml::Controls::TextBox;

//----------------------------------------------------------------------------------------------------------------------
// MARK: TextBoxHelper

class TextBoxHelper : public ControlHelper<TextBox, TextBoxHelper> {
	// Methods
	public:
						// Lifecycle methods
						TextBoxHelper(TextBox textBox) : ControlHelper(textBox) {}

						// Instance methods
		TextBoxHelper&	setText(const CString& string);
		TextBoxHelper&	setFocusState(FocusState focusState);
		TextBoxHelper&	setTextChangedProc(std::function<void()> textChangedProc);

		TextBox			getTextBox() const
							{ return getControl(); }
};
