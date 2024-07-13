//----------------------------------------------------------------------------------------------------------------------
//	TextBoxHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CString.h"

#undef Delete

#include "winrt\Microsoft.UI.Xaml.Controls.h"

#define Delete(x)	{ delete x; x = nil; }

#include <functional>

using FocusState = winrt::Microsoft::UI::Xaml::FocusState;
using TextBox = winrt::Microsoft::UI::Xaml::Controls::TextBox;

//----------------------------------------------------------------------------------------------------------------------
// MARK: TextBoxHelper

class TextBoxHelper {
	// Classes
	private:
		class Internals;

	// Methods
	public:
						// Lifecycle methods
						TextBoxHelper(TextBox textBox);

						// Instance methods
		TextBoxHelper&	setText(const CString& string);
		TextBoxHelper&	setFocusState(FocusState focusState);
		TextBoxHelper&	setTextChangedProc(std::function<void()> textChangedProc);

	// Properties
	private:
		Internals*	mInternals;
};
