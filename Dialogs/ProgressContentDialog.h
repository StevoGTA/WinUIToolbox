//----------------------------------------------------------------------------------------------------------------------
//	ProgressContentDialog.h			©2021 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CProgress.h"

#include <functional>	// May only be needed in VS2019

#undef Delete

#include "winrt\Microsoft.UI.Xaml.h"
#include "winrt\Microsoft.UI.Xaml.Controls.h"
#include "winrt\Microsoft.UI.Dispatching.h"

#define Delete(x)	{ delete x; x = nil; }

using namespace winrt::Microsoft::UI;

using ContentDialog = winrt::Microsoft::UI::Xaml::Controls::ContentDialog;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ProgressContentDialog

class ProgressContentDialog : public ContentDialog {
	// Procs
	typedef	std::function<void*(const I<CProgress>& progress)>	Proc;
	typedef	std::function<void()>								CancelProc;
	typedef	std::function<void(void* result)>					CompletionProc;

	// Classes
	private:
		class Internals;

	// Methods
	public:
								// Lifecycle methods
								ProgressContentDialog(const Xaml::XamlRoot& xamlRoot,
										const Dispatching::DispatcherQueue& dispatcherQueue);
								~ProgressContentDialog();

								// Instance methods
		CProgress::UpdateInfo	getProgressUpdateInfo() const;
		void					perform(const I<CProgress>& progress, Proc proc, CancelProc cancelProc,
										CompletionProc completionProc);

	// Properties
	private:
		Internals*	mInternals;
};
