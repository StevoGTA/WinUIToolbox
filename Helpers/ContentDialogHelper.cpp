//----------------------------------------------------------------------------------------------------------------------
//	ContentDialogHelper.cpp			©2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "ContentDialogHelper.h"

#include "winrt\Windows.Foundation.h"

using namespace winrt;

using ContentDialogButtonClickEventArgs = winrt::Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs;
using ContentDialogClosedEventArgs = winrt::Microsoft::UI::Xaml::Controls::ContentDialogClosedEventArgs;

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
ContentDialog ContentDialogHelper::make(const CString& title, const CString& content,
		const OV<CString>& primaryButtonText, const ButtonProc& primaryButtonProc,
		const OV<CString>& secondaryButtonText, const ButtonProc& secondaryButtonProc,
		const OV<CString>& closeButtonText, const ButtonProc& closeButtonProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	ContentDialog	contentDialog;

	contentDialog.Title(box_value(sApplyReplacements(title).getOSString()));
	contentDialog.Content(box_value(sApplyReplacements(content).getOSString()));

	if (primaryButtonText.hasValue()) {
		// Setup Primary Button
		contentDialog.PrimaryButtonText(primaryButtonText->getOSString());
		contentDialog.PrimaryButtonClick(
				[primaryButtonProc](const ContentDialog& contentDialog, const ContentDialogButtonClickEventArgs& eventArgs)
						{ primaryButtonProc(); });
	}

	if (secondaryButtonText.hasValue()) {
		// Setup Secondary Button
		contentDialog.SecondaryButtonText(secondaryButtonText->getOSString());
		contentDialog.SecondaryButtonClick(
				[secondaryButtonProc](const ContentDialog& contentDialog,
						const ContentDialogButtonClickEventArgs& eventArgs)
						{ secondaryButtonProc(); });
	}

	if (closeButtonText.hasValue()) {
		// Setup Close Button
		contentDialog.CloseButtonText(closeButtonText->getOSString());
		contentDialog.CloseButtonClick(
			[closeButtonProc](const ContentDialog& contentDialog, const ContentDialogButtonClickEventArgs& eventArgs) {
				// Call proc
				closeButtonProc();
			});
	}

	return contentDialog;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - ContentDialogStack::Internals

class ContentDialogStack::Internals {
	public:
		Internals(const XamlRoot& xamlRoot) : mXamlRoot(xamlRoot) {}

				XamlRoot					mXamlRoot;
				std::vector<ContentDialog>	mContentDialogs;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - ContentDialogStack

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
ContentDialogStack::ContentDialogStack(const XamlRoot& xamlRoot)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals(xamlRoot);
}

//----------------------------------------------------------------------------------------------------------------------
ContentDialogStack::~ContentDialogStack()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mInternals);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
void ContentDialogStack::showAsync(const ContentDialog& contentDialog)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if have any current Content Dialogs
	size_t	count = mInternals->mContentDialogs.size();
	if (count > 0)
		// Hide and teardown
		mInternals->mContentDialogs[count - 1].Hide();

	// Add
	mInternals->mContentDialogs.push_back(contentDialog);

	// Setup and show
	contentDialog.XamlRoot(mInternals->mXamlRoot);
	contentDialog.Closed([this](const ContentDialog& contentDialog, const ContentDialogClosedEventArgs& eventArgs) {
		// Setup
		size_t	count = mInternals->mContentDialogs.size();

		// Check if closed Content Dialog is the most recent one
		if ((count > 0) && (contentDialog == mInternals->mContentDialogs[count - 1])) {
			// Yes, remove from stack
			mInternals->mContentDialogs.pop_back();

			// Check if there are more
			if (count > 1) {
				// Re-display the next one in the stack
				mInternals->mContentDialogs[count - 2].XamlRoot(mInternals->mXamlRoot);
				mInternals->mContentDialogs[count - 2].ShowAsync();
			}
		}
	});
	contentDialog.ShowAsync();
}

//----------------------------------------------------------------------------------------------------------------------
void ContentDialogStack::clear()
//----------------------------------------------------------------------------------------------------------------------
{
	// Clear
	mInternals->mContentDialogs.clear();
}
