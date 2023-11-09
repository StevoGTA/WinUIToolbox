//----------------------------------------------------------------------------------------------------------------------
//	ProgressContentDialog.cpp			©2021 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "ProgressContentDialog.h"

#include "CReferenceCountable.h"

#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.System.Threading.h>
#include <winrt/Windows.System.Threading.Core.h>

using namespace winrt::Microsoft::UI::Dispatching;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::System::Threading;

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: ProgressContentDialogInternals

class ProgressContentDialogInternals : public TReferenceCountableAutoDelete<ProgressContentDialogInternals> {
	public:
		ProgressContentDialogInternals(ProgressContentDialog& progressContentDialog, const TextBlock& messageTextBlock,
				const ProgressBar& progressBar, const DispatcherQueue& dispatcherQueue) :
			TReferenceCountableAutoDelete(),
					mContentDialog(progressContentDialog),
							mIsCancelled(false), mDispatcherQueue(dispatcherQueue),
							mMessageTextBlock(messageTextBlock), mProgressBar(progressBar)
			{}

		ContentDialog	mContentDialog;

		bool			mIsCancelled;
		DispatcherQueue	mDispatcherQueue;

		TextBlock		mMessageTextBlock;
		ProgressBar		mProgressBar;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - ProgressContentDialog

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
ProgressContentDialog::ProgressContentDialog(Xaml::XamlRoot xamlRoot,
		const Dispatching::DispatcherQueue& dispatcherQueue) : ContentDialog()
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup UI
	StackPanel	stackPanel;
	stackPanel.Orientation(Orientation::Vertical);

	TextBlock	messageTextBlock;
	messageTextBlock.Text(L"{MESSAGE}");
	messageTextBlock.TextAlignment(TextAlignment::Center);
	messageTextBlock.Margin(ThicknessHelper::FromLengths(10.0, 10.0, 10.0, 20.0));
	stackPanel.Children().Append(messageTextBlock);

	ProgressBar	progressBar;
	progressBar.Minimum(0.0);
	progressBar.Maximum(1.0);
	progressBar.Value(0.5);
	progressBar.IsIndeterminate(true);
	stackPanel.Children().Append(progressBar);

	Content(stackPanel);
	XamlRoot(xamlRoot);

	// Setup internals
	mInternals = new ProgressContentDialogInternals(*this, messageTextBlock, progressBar, dispatcherQueue);
}

//----------------------------------------------------------------------------------------------------------------------
ProgressContentDialog::~ProgressContentDialog()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->removeReference();
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
CProgress::UpdateInfo ProgressContentDialog::getProgressUpdateInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Return update info
	return CProgress::UpdateInfo([](const CProgress& progress, void* userData) {
		// Setup
				CString							message(progress.getMessage());
				OV<Float32>						value(progress.getValue());
		const	ProgressContentDialogInternals&	internals = *((ProgressContentDialogInternals*) userData);

		// Update UI
		internals.mDispatcherQueue.TryEnqueue([=, &internals]() {
			// Update UI
			internals.mMessageTextBlock.Text(message.getOSString());

			internals.mProgressBar.IsIndeterminate(!value.hasValue());
			internals.mProgressBar.Value(value.hasValue() ? *value : 0.0);
		});
	}, mInternals);
}

//----------------------------------------------------------------------------------------------------------------------
void ProgressContentDialog::perform(const I<CProgress>& progress, Proc proc, CancelProc cancelProc,
		CompletionProc completionProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	auto	internals = mInternals->addReference();

	// Setup UI
	CloseButtonText(L"Cancel");
	CloseButtonClick([=](const ContentDialog& contentDialog,
			const ContentDialogButtonClickEventArgs& eventArgs) {
		// Cancelled
		internals->mIsCancelled = true;

		// Call cancel proc
		cancelProc();
	});

	// Run
	ThreadPool::RunAsync([=](IAsyncAction const& workItem) {
		// Call proc
		void*	result = proc(progress);

		// Switch to UI
		internals->mDispatcherQueue.TryEnqueue([=]() {
			// Hide
			internals->mContentDialog.Hide();

			// Check cancelled
			if (!internals->mIsCancelled)
				// Call completion proc
				completionProc(result);

			// Cleanup
			internals->removeReference();
		});
	});

	// Show
	ShowAsync();
}
