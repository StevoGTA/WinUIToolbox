//----------------------------------------------------------------------------------------------------------------------
//	SectionView.xaml.h			©2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "WinUIToolbox.SectionView.g.h"

#include "winrt\Microsoft.UI.Xaml.h"

using UIElement = winrt::Microsoft::UI::Xaml::UIElement;

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
			void	SetContent(const array_view<const UIElement>& uiElements);
			void	Insert(UIElement uiElement, int index);

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
