//----------------------------------------------------------------------------------------------------------------------
//	CppTBContentDialogHelper.cpp			©2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CppTBContentDialogHelper.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local proc definitions

//----------------------------------------------------------------------------------------------------------------------
static CString sApplyReplacements(const CString& string)
//----------------------------------------------------------------------------------------------------------------------
{
	return string.replacingSubStrings(CString(OSSTR("{APPLICATIONNAME}")), CppTBContentDialogHelper::mApplicationName);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: CppTBContentDialogHelper

// MARK: Class methods

//----------------------------------------------------------------------------------------------------------------------
ContentDialog CppTBContentDialogHelper::make(const CString& title, const CString& content,
		const OV<CString>& primaryButtonText, const ButtonProc& primaryButtonProc,
		const OV<CString>& secondaryButtonText, const ButtonProc& secondaryButtonProc,
		const OV<CString>& closeButtonText, const ButtonProc& closeButtonProc)
//----------------------------------------------------------------------------------------------------------------------
{
	return __super::make(sApplyReplacements(title).getOSString(), sApplyReplacements(content).getOSString(),
			primaryButtonText.hasValue() ?
					std::optional<std::basic_string<TCHAR> >(primaryButtonText->getOSString()) : std::nullopt,
			primaryButtonProc,
			secondaryButtonText.hasValue() ?
					std::optional<std::basic_string<TCHAR> >(secondaryButtonText->getOSString()) : std::nullopt,
			primaryButtonProc,
			closeButtonText.hasValue() ?
					std::optional<std::basic_string<TCHAR> >(closeButtonText->getOSString()) : std::nullopt,
			primaryButtonProc);
}
