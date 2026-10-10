//----------------------------------------------------------------------------------------------------------------------
//	ChipsView.xaml.h			©2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "WinUIToolbox.ChipInfo.g.h"
#include "WinUIToolbox.ChipsView.g.h"

#include "winrt\Microsoft.UI.Xaml.h"
#include "winrt\Windows.Foundation.Collections.h"
#include "winrt\Windows.UI.h"

using ChipInfoStyle = winrt::WinUIToolbox::ChipInfoStyle;
using ChipInfoSymbol = winrt::WinUIToolbox::ChipInfoSymbol;
using ChipInfoVectorView = winrt::Windows::Foundation::Collections::IVectorView<winrt::WinUIToolbox::ChipInfo>;
using ColorReference = winrt::Windows::Foundation::IReference<winrt::Windows::UI::Color>;

//----------------------------------------------------------------------------------------------------------------------
// MARK: winrt::WinUIToolbox::implementation

namespace winrt::WinUIToolbox::implementation {

	// MARK: ChipInfo
	struct ChipInfo : ChipInfoT<ChipInfo> {
		// Methods
		public:
							// Lifecycle methods
							ChipInfo(const hstring& text, ChipInfoStyle style, ChipInfoSymbol symbol,
									const ColorReference& tintColor) :
								mText(text), mStyle(style), mSymbol(symbol), mTintColor(tintColor)
								{}

							// Instance methods
			hstring			Text() const
								{ return mText; }
			ChipInfoStyle	Style() const
								{ return mStyle; }
			ChipInfoSymbol	Symbol() const
								{ return mSymbol; }
			ColorReference	TintColor() const
								{ return mTintColor; }

		// Properties
		private:
			hstring			mText;
			ChipInfoStyle	mStyle;
			ChipInfoSymbol	mSymbol;
			ColorReference	mTintColor;
	};

	// MARK: ChipsView
	struct ChipsView : ChipsViewT<ChipsView> {
		// Classes
		private:
			class Internals;

		// Methods
		public:
								// Lifecycle methods
								ChipsView();
								~ChipsView();

								// Instance methods
			ChipInfoVectorView	Infos() const;
			void				Infos(const ChipInfoVectorView& infos);

		// Properties
		private:
			Internals*	mInternals;
	};
}

//----------------------------------------------------------------------------------------------------------------------
// MARK: - winrt::WinUIToolbox::factory_implementation

namespace winrt::WinUIToolbox::factory_implementation {

	// MARK: ChipInfo
	struct ChipInfo : ChipInfoT<ChipInfo, implementation::ChipInfo> {};

	// MARK: ChipsView
	struct ChipsView : ChipsViewT<ChipsView, implementation::ChipsView> {};
}
