//----------------------------------------------------------------------------------------------------------------------
//	TextBoxHelper.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "TextBoxHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt\Windows.Foundation.h"

using IInspectable = winrt::Windows::Foundation::IInspectable;
using TextChangedEventArgs = winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	TextBox	sTextBoxInUpdate(nullptr);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - TextBoxHelper

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
TextBoxHelper& TextBoxHelper::setText(const winrt::hstring& string)
//----------------------------------------------------------------------------------------------------------------------
{
	// Update
	sTextBoxInUpdate = getTextBox();
	getTextBox().Text(string);
	sTextBoxInUpdate = nullptr;

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
TextBoxHelper& TextBoxHelper::setFocusState(FocusState focusState)
//----------------------------------------------------------------------------------------------------------------------
{
	// Update
	getTextBox().Focus(focusState);

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
TextBoxHelper& TextBoxHelper::setTextChangedProc(std::function<void(const winrt::hstring& string)> textChangedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	getTextBox().TextChanged(
			[textChangedProc](const IInspectable& sender, const TextChangedEventArgs& textChangedEventArgs) {
				// Check if handling events
				if (sender == sTextBoxInUpdate)
					return;

				// Call proc
				textChangedProc(sender.as<TextBox>().Text());
			});

	return *this;
}
