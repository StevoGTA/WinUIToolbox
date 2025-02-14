//----------------------------------------------------------------------------------------------------------------------
//	ContentDialogHelper.cpp			©2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "ContentDialogHelper.h"

#include "winrt\Windows.Foundation.h"

using namespace winrt;

using ContentDialogButtonClickEventArgs = winrt::Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ContentDialogHelper

// MARK: Class methods

//----------------------------------------------------------------------------------------------------------------------
ContentDialog ContentDialogHelper::make(const std::basic_string<TCHAR>& title, const std::basic_string<TCHAR>& content,
		const std::optional<std::basic_string<TCHAR> >& primaryButtonText, const ButtonProc& primaryButtonProc,
		const std::optional<std::basic_string<TCHAR> >& secondaryButtonText, const ButtonProc& secondaryButtonProc,
		const std::optional<std::basic_string<TCHAR> >& closeButtonText, const ButtonProc& closeButtonProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	ContentDialog	contentDialog;

	contentDialog.Title(box_value(title.c_str()));
	contentDialog.Content(box_value(content.c_str()));

	if (primaryButtonText.has_value()) {
		// Setup Primary Button
		contentDialog.PrimaryButtonText(primaryButtonText->c_str());
		contentDialog.PrimaryButtonClick(
				[primaryButtonProc](const ContentDialog& contentDialog,
								const ContentDialogButtonClickEventArgs& eventArgs){
					// Call proc
					primaryButtonProc();
				});
	}

	if (secondaryButtonText.has_value()) {
		// Setup Secondary Button
		contentDialog.SecondaryButtonText(secondaryButtonText->c_str());
		contentDialog.SecondaryButtonClick(
				[secondaryButtonProc](const ContentDialog& contentDialog,
								const ContentDialogButtonClickEventArgs& eventArgs){
					// Call proc
					secondaryButtonProc();
				});
	}

	if (closeButtonText.has_value()) {
		// Setup Close Button
		contentDialog.CloseButtonText(closeButtonText->c_str());
		contentDialog.CloseButtonClick(
				[closeButtonProc](const ContentDialog& contentDialog,
						const ContentDialogButtonClickEventArgs& eventArgs){
					// Call proc
					closeButtonProc();
				});
	}

	return contentDialog;
}
