//----------------------------------------------------------------------------------------------------------------------
//	ContentDialogStack.cpp			©2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "ContentDialogStack.h"

#include "winrt\Windows.Foundation.h"

using ContentDialogClosedEventArgs = winrt::Microsoft::UI::Xaml::Controls::ContentDialogClosedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ContentDialogStack::Internals

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
	delete mInternals;
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
	contentDialog.Closed([this](const ContentDialog& contentDialog, const ContentDialogClosedEventArgs& eventArgs){
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
