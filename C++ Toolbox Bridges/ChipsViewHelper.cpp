//----------------------------------------------------------------------------------------------------------------------
//	ChipsViewHelper.cpp			©2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "ChipsViewHelper.h"

#include "CColor+Extensions.h"

#include "winrt\Windows.Foundation.Collections.h"

using ChipInfo = winrt::WinUIToolbox::ChipInfo;
using ChipInfoStyle = winrt::WinUIToolbox::ChipInfoStyle;
using ChipInfoSymbol = winrt::WinUIToolbox::ChipInfoSymbol;
using ColorReference = winrt::Windows::Foundation::IReference<winrt::Windows::UI::Color>;

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local procs

//----------------------------------------------------------------------------------------------------------------------
static ChipInfo sChipInfoFor(const SChipInfo& chipInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	return ChipInfo(chipInfo.getText().getOSString(),
			(chipInfo.getStyle() == SChipInfo::kStyleAccented) ?
					ChipInfoStyle::Accented :
					((chipInfo.getStyle() == SChipInfo::kStyleFilled) ? ChipInfoStyle::Filled : ChipInfoStyle::Outlined),
			(chipInfo.getSymbol() == SChipInfo::kSymbolLocked) ? ChipInfoSymbol::Locked : ChipInfoSymbol::None,
			chipInfo.getColor().hasValue() ? ColorReference(CColorEx::toColor(*chipInfo.getColor())) : nullptr);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - ChipsViewHelper

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
ChipsViewHelper& ChipsViewHelper::setChipInfos(const TArray<SChipInfo>& chipInfos)
//----------------------------------------------------------------------------------------------------------------------
{
	// Compose Infos
	auto	infos = winrt::single_threaded_vector<ChipInfo>();
	for (TArray<SChipInfo>::Iterator iterator = chipInfos.getIterator(); iterator; iterator++)
		// Add Info
		infos.Append(sChipInfoFor(*iterator));

	// Set
	getChipsView().Infos(infos.GetView());

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ChipsViewHelper& ChipsViewHelper::setChipInfo(const SChipInfo& chipInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Set
	getChipsView().Infos(winrt::single_threaded_vector<ChipInfo>({ sChipInfoFor(chipInfo) }).GetView());

	return *this;
}
