//----------------------------------------------------------------------------------------------------------------------
//	PathView.xaml.h			�2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "WinUIToolbox.PathView.g.h"

#include "winrt\Microsoft.UI.Xaml.h"

using DependencyProperty = winrt::Microsoft::UI::Xaml::DependencyProperty;
using Size = winrt::Windows::Foundation::Size;

//----------------------------------------------------------------------------------------------------------------------
// MARK: winrt::WinUIToolbox::implementation

namespace winrt::WinUIToolbox::implementation {

	// MARK: PathView
	struct PathView : PathViewT<PathView> {
		// Classes
		private:
			class Internals;

		// Methods
		public:
										// Lifecycle methods
										PathView();
										~PathView();

										// Control methods
					Size				MeasureOverride(const Size& availableSize);
					Size				ArrangeOverride(const Size& finalSize);

										// Instance methods
					hstring				Path() const;
					void				Path(const hstring& path);

										// Class methods
			static	DependencyProperty	PathProperty() noexcept;

		// Properties
		private:
			static	DependencyProperty	mPathProperty;

					Internals*			mInternals;
	};
}
//----------------------------------------------------------------------------------------------------------------------
// MARK: - winrt::WinUIToolbox::factory_implementation

namespace winrt::WinUIToolbox::factory_implementation {
	// MARK: PathView
	struct PathView : PathViewT<PathView, implementation::PathView> {};
}
