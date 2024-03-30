//----------------------------------------------------------------------------------------------------------------------
//	ContentDialogHelper.cpp			©2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "ContentDialogHelper.h"

#include "winrt\Windows.Foundation.h"

using namespace winrt;

using ContentDialogButtonClickEventArgs = winrt::Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local proc definitions

//----------------------------------------------------------------------------------------------------------------------
static CString sApplyReplacements(const CString& string)
//----------------------------------------------------------------------------------------------------------------------
{
	return string.replacingSubStrings(CString(OSSTR("{APPLICATIONNAME}")), ContentDialogHelper::mApplicationName);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: ContentDialogHelper

// MARK: Class methods

//----------------------------------------------------------------------------------------------------------------------
ContentDialog ContentDialogHelper::make(const CString& title, const CString& message, const CString& buttonLabel,
		const ButtonProc& buttonProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	ContentDialog	contentDialog;

	contentDialog.Title(box_value(sApplyReplacements(title).getOSString()));
	contentDialog.Content(box_value(sApplyReplacements(message).getOSString()));

	contentDialog.PrimaryButtonText(buttonLabel.getOSString());
	contentDialog.PrimaryButtonClick(
		[buttonProc](const ContentDialog& contentDialog, const ContentDialogButtonClickEventArgs& eventArgs) {
			// Call proc
			buttonProc();
		});

	return contentDialog;
}

//----------------------------------------------------------------------------------------------------------------------
ContentDialog ContentDialogHelper::make(const CString& title, const CString& message,
		const CString& primaryButtonLabel, const ButtonProc& primaryButtonProc, const CString& secondaryButtonLabel,
		const ButtonProc& secondaryButtonProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	ContentDialog	contentDialog;

	contentDialog.Title(box_value(sApplyReplacements(title).getOSString()));
	contentDialog.Content(box_value(sApplyReplacements(message).getOSString()));

	contentDialog.PrimaryButtonText(primaryButtonLabel.getOSString());
	contentDialog.PrimaryButtonClick(
			[primaryButtonProc](const ContentDialog& contentDialog, const ContentDialogButtonClickEventArgs& eventArgs)
					{ primaryButtonProc(); });

	contentDialog.SecondaryButtonText(secondaryButtonLabel.getOSString());
	contentDialog.SecondaryButtonClick(
			[secondaryButtonProc](const ContentDialog& contentDialog,
					const ContentDialogButtonClickEventArgs& eventArgs)
					{ secondaryButtonProc(); });

	return contentDialog;
}
