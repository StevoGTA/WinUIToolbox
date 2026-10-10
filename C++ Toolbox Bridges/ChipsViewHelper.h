//----------------------------------------------------------------------------------------------------------------------
//	ChipsViewHelper.h			©2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "SChipInfo.h"

#undef Delete

#include "ControlHelper.h"

#include "winrt\WinUIToolbox.h"

#define Delete(x)    { delete x; x = nil; }

using ChipsView = winrt::WinUIToolbox::ChipsView;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ChipsViewHelper

class ChipsViewHelper : public ControlHelper<ChipsView, ChipsViewHelper> {
	// Methods
	public:
							// Lifecycle methods
							ChipsViewHelper(ChipsView chipsView) : ControlHelper(chipsView) {}

							// Instance methods
		ChipsViewHelper&	setChipInfos(const TArray<SChipInfo>& chipInfos);
		ChipsViewHelper&	setChipInfo(const SChipInfo& chipInfo);

		ChipsView			getChipsView() const
								{ return getControl(); }
};
