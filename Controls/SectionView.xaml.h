//----------------------------------------------------------------------------------------------------------------------
//	SectionView.xaml.h			©2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "WinUIToolbox.SectionView.g.h"

#include "winrt\Microsoft.UI.Xaml.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: winrt::WinUIToolbox::implementation

namespace winrt::WinUIToolbox::implementation {

	// MARK: SectionView
	struct SectionView : SectionViewT<SectionView> {
		// Methods
		public:
					// Lifecycle methods
					SectionView() : SectionViewT<SectionView>() {}

					// Instance methods
			void	SetContent(const array_view<const winrt::Microsoft::UI::Xaml::UIElement>& uiElements);
			void	SetContent(const hstring& string);
	};
}
//----------------------------------------------------------------------------------------------------------------------
// MARK: - winrt::WinUIToolbox::factory_implementation

namespace winrt::WinUIToolbox::factory_implementation
{
	// MARK: SectionView
	struct SectionView : SectionViewT<SectionView, implementation::SectionView> {};
}
