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
using XamlRoot = winrt::Microsoft::UI::Xaml::XamlRoot;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ContentDialogHelper

class ContentDialogHelper {
	// Types
	public:
		typedef	std::function<void()>	ButtonProc;

	// Methods
	public:
								// Class methods
		static	ContentDialog	makeClosable(const CString& title, const CString& content,
										const CString& closeButtonText, const ButtonProc& closeButtonProc = [](){})
									{ return make(title, content, OV<CString>(), [](){}, OV<CString>(), [](){},
											OV<CString>(closeButtonText), closeButtonProc); }
		static	ContentDialog	make(const CString& title, const CString& content,
										const CString& buttonText, const ButtonProc& buttonProc = [](){},
										const OV<CString>& closeButtonText = OV<CString>(),
										const ButtonProc& closeButtonProc = [](){})
									{ return make(title, content, buttonText, buttonProc, OV<CString>(), [](){},
											closeButtonText, closeButtonProc); }
		static	ContentDialog	make(const CString& title, const CString& content,
										const OV<CString>& primaryButtonText = OV<CString>(),
										const ButtonProc& primaryButtonProc = [](){},
										const OV<CString>& secondaryButtonText = OV<CString>(),
										const ButtonProc& secondaryButtonProc = [](){},
										const OV<CString>& closeButtonText = OV<CString>(),
										const ButtonProc& closeButtonProc = [](){});

	// Properties
	public:
		static	const	CString	mApplicationName;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - ContentDialogStack

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
