//----------------------------------------------------------------------------------------------------------------------
//	TextBoxHelper.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "TextBoxHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt\Microsoft.UI.Xaml.Input.h"
#include "winrt\Microsoft.UI.Xaml.Media.h"
#include "winrt\Windows.Foundation.h"
#include "winrt\Windows.UI.h"

using IInspectable = winrt::Windows::Foundation::IInspectable;
using SolidColorBrush = winrt::Microsoft::UI::Xaml::Media::SolidColorBrush;
using TextBoxTextChangingEventArgs = winrt::Microsoft::UI::Xaml::Controls::TextBoxTextChangingEventArgs;
using TextChangedEventArgs = winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	TextBox	sTextBoxInUpdate(nullptr);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - TextBoxHelper::Internals

class TextBoxHelper::Internals {
	public:
		Internals() :
				mOriginalBackgroundBrush(nullptr)
			{}

		Brush		mOriginalBackgroundBrush;
		Thickness	mOriginalBorderThickness{};
		Thickness	mOriginalPadding{};
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - TextBoxHelper

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
TextBoxHelper::TextBoxHelper(TextBox textBox) : ControlHelper(textBox), mInternals(new Internals())
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
TextBoxHelper::~TextBoxHelper()
//----------------------------------------------------------------------------------------------------------------------
{
	delete mInternals;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
TextBoxHelper& TextBoxHelper::setKeyDownProc(std::function<void(
		const KeyRoutedEventArgs& keyRoutedEventArgs)> keyDownProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	getTextBox().KeyDown([keyDownProc](const IInspectable& sender, const KeyRoutedEventArgs& keyRoutedEventArgs){
		// Call proc
		keyDownProc(keyRoutedEventArgs);
	});

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
TextBoxHelper& TextBoxHelper::setText(const winrt::param::hstring& string)
//----------------------------------------------------------------------------------------------------------------------
{
	// Update
	sTextBoxInUpdate = getTextBox();
	getTextBox().Text(string);
	sTextBoxInUpdate = nullptr;

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
TextBoxHelper& TextBoxHelper::setTextFilterProc(std::function<hstring(const hstring& string)> filterProc,
		std::function<void()> wasFilteredProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	getTextBox().TextChanging(
			[filterProc, wasFilteredProc]
			(const IInspectable& sender, const TextBoxTextChangingEventArgs& textBoxTextChangingEventArgs){
				// Check if handling events
				if (sender == sTextBoxInUpdate)
					return;

				// Setup
				auto	textBox = sender.as<TextBox>();
				auto	string = textBox.Text();

				// Call proc
				auto	filteredString = filterProc(string);

				// Update text if changed
				if (filteredString != string) {
					// Update
					sTextBoxInUpdate = textBox;
					textBox.Text(filteredString);
					sTextBoxInUpdate = nullptr;

					// Caret to end (setting Text resets it to the start; macOS leaves it at the end)
					textBox.SelectionStart((int32_t) filteredString.size());
					textBox.SelectionLength(0);

					// Note was filtered
					if (wasFilteredProc)
						wasFilteredProc();
				}
		});

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
TextBoxHelper& TextBoxHelper::setTextChangedProc(std::function<void(const winrt::hstring& string)> textChangedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	getTextBox().TextChanged(
			[textChangedProc](const IInspectable& sender, const TextChangedEventArgs& textChangedEventArgs){
				// Check if handling events
				if (sender == sTextBoxInUpdate)
					return;

				// Call proc
				textChangedProc(sender.as<TextBox>().Text());
			});

	return *this;
}
