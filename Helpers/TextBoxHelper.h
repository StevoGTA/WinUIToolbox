//----------------------------------------------------------------------------------------------------------------------
//	TextBoxHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ControlHelper.h"

#include "winrt\Windows.Foundation.h"

#include <functional>

using Application = winrt::Microsoft::UI::Xaml::Application;
using FocusState = winrt::Microsoft::UI::Xaml::FocusState;
using hstring = winrt::hstring;
using KeyRoutedEventArgs = winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs;
using TextBox = winrt::Microsoft::UI::Xaml::Controls::TextBox;

//----------------------------------------------------------------------------------------------------------------------
// MARK: TextBoxHelper

class TextBoxHelper : public ControlHelper<TextBox, TextBoxHelper> {
	// Classes
	private:
		class	Internals;

	// Methods
	public:
						// Lifecycle methods
						TextBoxHelper(TextBox textBox);
						~TextBoxHelper();

						// Instance methods
		TextBoxHelper&	setBorderBrush(const Brush& brush)
							{ getTextBox().BorderBrush(brush); return *this; }
		TextBoxHelper&	setBorderToDefault()
							{
								// Set to default
								setBorderBrush(
										Application::Current().Resources()
												.TryLookup(winrt::box_value(L"TextControlBorderBrush")).as<Brush>());

								return *this;
							}
		TextBoxHelper&	setBorderToError()
							{
								// Set to error
								setBorderBrush(
										Application::Current().Resources()
												.TryLookup(winrt::box_value(L"TextControlErrorBorderBrush"))
												.as<Brush>());

								return *this;
							}

		TextBoxHelper&	setFocusState(FocusState focusState)
							{ getTextBox().Focus(focusState); return *this; }
		TextBoxHelper&	setKeyDownProc(std::function<void(const KeyRoutedEventArgs& keyRoutedEventArgs)> keyDownProc);
		TextBoxHelper&	setText(const hstring& string);
		TextBoxHelper&	setTextChangedProc(std::function<void(const winrt::hstring& string)> textChangedProc);

		TextBox			getTextBox() const
							{ return getControl(); }

	// Properties
	private:
		Internals*	mInternals;
};
