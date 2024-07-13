//----------------------------------------------------------------------------------------------------------------------
//	TextBoxHelper.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "TextBoxHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt\Windows.Foundation.h"

using IInspectable = winrt::Windows::Foundation::IInspectable;
using TextChangedEventArgs = winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: TextBoxHelper::Internals

class TextBoxHelper::Internals {
	public:
		Internals(TextBox textbox) : mTextBox(textbox) {}

		TextBox	mTextBox;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: TextBoxHelper

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
TextBoxHelper::TextBoxHelper(TextBox textbox)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(textbox);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
TextBoxHelper& TextBoxHelper::setText(const CString& string)
//----------------------------------------------------------------------------------------------------------------------
{
	// Update
	mInternals->mTextBox.Text(string.getOSString());

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
TextBoxHelper& TextBoxHelper::setFocusState(FocusState focusState)
//----------------------------------------------------------------------------------------------------------------------
{
	// Update
	mInternals->mTextBox.Focus(focusState);

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
TextBoxHelper& TextBoxHelper::setTextChangedProc(std::function<void()> textChangedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals->mTextBox.TextChanged(
			[textChangedProc](const IInspectable& sender, const TextChangedEventArgs& textChangedEventArgs) {
				// Call proc
				textChangedProc();
			});

	return *this;
}
