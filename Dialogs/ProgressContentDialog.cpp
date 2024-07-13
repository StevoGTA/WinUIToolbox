//----------------------------------------------------------------------------------------------------------------------
//	ProgressContentDialog.cpp			©2021 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "ProgressContentDialog.h"

#include "CReferenceCountable.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt\Windows.Foundation.h"
#include "winrt\Windows.Foundation.Collections.h"
#include "winrt\Windows.System.Threading.h"

using ContentDialogButtonClickEventArgs = winrt::Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs;
using IAsyncAction = winrt::Windows::Foundation::IAsyncAction;
using Orientation = winrt::Microsoft::UI::Xaml::Controls::Orientation;
using ProgressBar = winrt::Microsoft::UI::Xaml::Controls::ProgressBar;
using StackPanel = winrt::Microsoft::UI::Xaml::Controls::StackPanel;
using TextAlignment = winrt::Microsoft::UI::Xaml::TextAlignment;
using TextBlock = winrt::Microsoft::UI::Xaml::Controls::TextBlock;
using ThicknessHelper = winrt::Microsoft::UI::Xaml::ThicknessHelper;
using ThreadPool = winrt::Windows::System::Threading::ThreadPool;

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: ProgressContentDialog::Internals

class ProgressContentDialog::Internals : public TReferenceCountableAutoDelete<Internals> {
	public:
		Internals(ProgressContentDialog& progressContentDialog, const TextBlock& messageTextBlock,
				const ProgressBar& progressBar, const Dispatching::DispatcherQueue& dispatcherQueue) :
			TReferenceCountableAutoDelete(),
					mContentDialog(progressContentDialog),
							mIsCancelled(false), mDispatcherQueue(dispatcherQueue),
							mMessageTextBlock(messageTextBlock), mProgressBar(progressBar)
			{}

		ContentDialog					mContentDialog;

		bool							mIsCancelled;
		Dispatching::DispatcherQueue	mDispatcherQueue;

		TextBlock						mMessageTextBlock;
		ProgressBar						mProgressBar;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - ProgressContentDialog

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
ProgressContentDialog::ProgressContentDialog(const Dispatching::DispatcherQueue& dispatcherQueue) : ContentDialog()
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

	// Setup internals
	mInternals = new Internals(*this, messageTextBlock, progressBar, dispatcherQueue);
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
				CString		message(progress.getMessage());
				OV<Float32>	value(progress.getValue());
		const	Internals&	internals = *((Internals*) userData);

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
}
