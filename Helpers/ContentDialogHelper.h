//----------------------------------------------------------------------------------------------------------------------
//	ContentDialogHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include <functional>

#include "winrt\Microsoft.UI.Xaml.Controls.h"

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
		static	ContentDialog	makeClosable(const std::basic_string<TCHAR>& title,
										const std::basic_string<TCHAR>& content,
										const std::basic_string<TCHAR>& closeButtonText,
										const ButtonProc& closeButtonProc = [](){})
									{ return make(title, content, std::nullopt, [](){}, std::nullopt, [](){},
										std::optional<std::basic_string<TCHAR> >(closeButtonText), closeButtonProc); }
		static	ContentDialog	make(const std::basic_string<TCHAR>& title, const std::basic_string<TCHAR>& content,
										const std::basic_string<TCHAR>& buttonText,
										const ButtonProc& buttonProc = [](){},
										const std::optional<std::basic_string<TCHAR> >& closeButtonText = std::nullopt,
										const ButtonProc& closeButtonProc = [](){})
									{ return make(title, content, buttonText, buttonProc, std::nullopt, [](){},
											closeButtonText, closeButtonProc); }
		static	ContentDialog	make(const std::basic_string<TCHAR>& title, const std::basic_string<TCHAR>& content,
										const std::optional<std::basic_string<TCHAR> >& primaryButtonText =
												std::nullopt,
										const ButtonProc& primaryButtonProc = [](){},
										const std::optional<std::basic_string<TCHAR> >& secondaryButtonText =
												std::nullopt,
										const ButtonProc& secondaryButtonProc = [](){},
										const std::optional<std::basic_string<TCHAR> >& closeButtonText =
												std::nullopt,
										const ButtonProc& closeButtonProc = [](){});
};
