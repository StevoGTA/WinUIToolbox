//----------------------------------------------------------------------------------------------------------------------
//	ContentDialogStack.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "winrt\Microsoft.UI.Xaml.Controls.h"

using ContentDialog = winrt::Microsoft::UI::Xaml::Controls::ContentDialog;
using XamlRoot = winrt::Microsoft::UI::Xaml::XamlRoot;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ContentDialogStack

class ContentDialogStack {
	// Classes
	private:
		class	Internals;

	// Methods
	public:
				// Lifecycle methods
				ContentDialogStack(const XamlRoot& xamlRoot);
				~ContentDialogStack();

				// Instance methods
		void	showAsync(const ContentDialog& contentDialog);
		void	clear();

	// Properties
	private:
		Internals*	mInternals;
};
