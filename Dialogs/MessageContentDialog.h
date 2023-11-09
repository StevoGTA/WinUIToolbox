//----------------------------------------------------------------------------------------------------------------------
//	MessageContentDialog.h			©2022 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CString.h"

#include <functional>	// May only be needed in VS2019

#undef Delete

#include "winrt/Microsoft.UI.Xaml.Controls.h"

#define Delete(x)	{ delete x; x = nil; }

using namespace winrt::Microsoft::UI::Xaml::Controls;

//----------------------------------------------------------------------------------------------------------------------
// MARK: MessageContentDialog

class MessageContentDialog : public ContentDialog {
	// Types
	typedef	std::function<void()>	ButtonProc;

	// Methods
	public:
						// Lifecycle methods
						MessageContentDialog(const CString& title, const CString& message,
								const CString& buttonLabel, const ButtonProc& buttonProc = [](){});
						MessageContentDialog(const CString& title, const CString& message,
								const CString& primaryButtonLabel, const ButtonProc& primaryButtonProc,
								const CString& secondaryButtonLabel,
								const ButtonProc& secondaryButtonProc);
						~MessageContentDialog() {}

						// Class methods
		static	void	showAsync(const CString& title, const CString& message,
								const CString& buttonLabel, const ButtonProc& buttonProc);
		static	void	showAsync(const CString& title, const CString& message,
								const CString& buttonLabel)
							{ showAsync(title, message, buttonLabel, [](){}); }

		static	void	showAsync(const CString& title, const CString& message,
								const CString& primaryButtonLabel, const ButtonProc& primaryButtonProc,
								const CString& secondaryButtonLabel, const ButtonProc& secondaryButtonProc);

	// Properties
	public:
		static	const	CString	mApplicationName;
};
