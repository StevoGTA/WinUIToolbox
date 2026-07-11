//----------------------------------------------------------------------------------------------------------------------
//	TextBoxHelper.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "TextBoxHelper.h"

#include "winrt\Microsoft.UI.Dispatching.h"
#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt\Microsoft.UI.Xaml.Input.h"
#include "winrt\Microsoft.UI.Xaml.Media.h"
#include "winrt\Windows.ApplicationModel.DataTransfer.h"
#include "winrt\Windows.Foundation.h"
#include "winrt\Windows.UI.h"

using AsyncStatus = winrt::Windows::Foundation::AsyncStatus;
using Clipboard = winrt::Windows::ApplicationModel::DataTransfer::Clipboard;
using IInspectable = winrt::Windows::Foundation::IInspectable;
using SolidColorBrush = winrt::Microsoft::UI::Xaml::Media::SolidColorBrush;
using StandardDataFormats = winrt::Windows::ApplicationModel::DataTransfer::StandardDataFormats;
using TextBoxTextChangingEventArgs = winrt::Microsoft::UI::Xaml::Controls::TextBoxTextChangingEventArgs;
using TextChangedEventArgs = winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs;
using TextControlPasteEventArgs = winrt::Microsoft::UI::Xaml::Controls::TextControlPasteEventArgs;

using IAsyncOperation_hstring = winrt::Windows::Foundation::IAsyncOperation<winrt::hstring>;

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
TextBoxHelper& TextBoxHelper::setTextValidationProcs(std::function<bool(const hstring& string)> isAllowedProc,
		std::function<hstring(const hstring& string)> filterProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	getTextBox().Paste(
			[isAllowedProc](const IInspectable& sender, const TextControlPasteEventArgs& textControlPasteEventArgs){
				// Setup
				auto	dataPackageView = Clipboard::GetContent();
				if (!dataPackageView.Contains(StandardDataFormats::Text()))
					return;

				// Setup
				auto	textBox = sender.as<TextBox>();

				// Get text from clipboard
				dataPackageView.GetTextAsync()
						.Completed([textBox, isAllowedProc](const IAsyncOperation_hstring& asyncOperation,
								const AsyncStatus& asyncStatus){
									// Get pasted string
									auto	string = asyncOperation.GetResults();

									// Switch to UI thread
									textBox.DispatcherQueue().TryEnqueue([textBox, string, isAllowedProc](){
										// Check if is allowed
										if (!isAllowedProc(string))
											// Not allowed
											return;

										// Setup
										std::basic_string<TCHAR>	currentText = textBox.Text().c_str();
										auto						selectionStart = textBox.SelectionStart();
										auto						selectionLength = textBox.SelectionLength();

										// Update
										sTextBoxInUpdate = textBox;
										textBox.Text(
											(currentText.substr(0, selectionStart) +
															(std::basic_string<TCHAR>) string +
															currentText.substr(selectionStart + selectionLength))
													.substr(0, textBox.MaxLength()));
										textBox.SelectionStart(selectionStart + (int32_t) string.size());
										textBox.SelectionLength(0);
										sTextBoxInUpdate = nullptr;
									});
								});

				// Handled
				textControlPasteEventArgs.Handled(true);
			});
	getTextBox().TextChanging(
			[filterProc]
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
