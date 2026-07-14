//----------------------------------------------------------------------------------------------------------------------
//	ContentDialogStack.cpp			©2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "ContentDialogStack.h"

#include "winrt\Windows.Foundation.h"

using ContentDialogClosedEventArgs = winrt::Microsoft::UI::Xaml::Controls::ContentDialogClosedEventArgs;
using event_token = winrt::event_token;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ContentDialogStack::Internals

class ContentDialogStack::Internals {
	public:
		Internals(const XamlRoot& xamlRoot) : mXamlRoot(xamlRoot) {}

		XamlRoot					mXamlRoot;
		std::vector<ContentDialog>	mContentDialogs;
		std::vector<event_token>	mClosedTokens;
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
	// Detach Closed handlers so any dialogs still alive can't fire into freed memory
	for (size_t i = 0; i < mInternals->mContentDialogs.size(); i++)
		// Detach Closed handler
		mInternals->mContentDialogs[i].Closed(mInternals->mClosedTokens[i]);

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
	mInternals->mClosedTokens.push_back(
			contentDialog.Closed(
					[this](const ContentDialog& contentDialog, const ContentDialogClosedEventArgs& eventArgs){
						// Setup
						size_t	count = mInternals->mContentDialogs.size();

						// Check if closed Content Dialog is the most recent one
						if ((count > 0) && (contentDialog == mInternals->mContentDialogs[count - 1])) {
							// Yes, remove from stack
							mInternals->mContentDialogs.pop_back();
							mInternals->mClosedTokens.pop_back();

							// Check if there are more
							if (count > 1) {
								// Re-display the next one in the stack
								mInternals->mContentDialogs[count - 2].XamlRoot(mInternals->mXamlRoot);
								mInternals->mContentDialogs[count - 2].ShowAsync();
							}
						}
					}));
	contentDialog.ShowAsync();
}

//----------------------------------------------------------------------------------------------------------------------
void ContentDialogStack::clear()
//----------------------------------------------------------------------------------------------------------------------
{
	// Detach Closed handlers and clear
	for (size_t i = 0; i < mInternals->mContentDialogs.size(); i++)
		// Detach Closed handler
		mInternals->mContentDialogs[i].Closed(mInternals->mClosedTokens[i]);
	mInternals->mContentDialogs.clear();
	mInternals->mClosedTokens.clear();
}
