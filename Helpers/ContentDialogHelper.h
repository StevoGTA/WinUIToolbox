//----------------------------------------------------------------------------------------------------------------------
//	ContentDialogHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CString.h"

#include <functional>

#undef Delete

#include "winrt\Microsoft.UI.Xaml.Controls.h"

#define Delete(x)	{ delete x; x = nil; }

using ContentDialog = winrt::Microsoft::UI::Xaml::Controls::ContentDialog;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ContentDialogHelper

class ContentDialogHelper {
	// Types
	typedef	std::function<void()>	ButtonProc;

	// Methods
	public:
								// Class methods
		static	ContentDialog	make(const CString& title, const CString& message,
										const CString& buttonLabel, const ButtonProc& buttonProc = [](){});
		static	ContentDialog	make(const CString& title, const CString& message,
										const CString& primaryButtonLabel, const ButtonProc& primaryButtonProc,
										const CString& secondaryButtonLabel,
										const ButtonProc& secondaryButtonProc);

	// Properties
	public:
		static	const	CString	mApplicationName;
};
