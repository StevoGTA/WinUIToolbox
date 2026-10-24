//----------------------------------------------------------------------------------------------------------------------
//	OutlineViewRowHeightProvider.h			©2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "winrt\Windows.Foundation.h"
#include "winrt\WinUIToolbox.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: OutlineViewRowHeightProvider

class OutlineViewRowHeightProvider {
	// Methods
	public:
						// Lifecycle methods
		virtual			~OutlineViewRowHeightProvider() {}

						// Instance methods
		virtual	double	getRowHeight(const winrt::hstring& identifier,
								const winrt::WinUIToolbox::OutlineView& outlineView) = 0;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - OutlineViewRowHeightProviderRInspectable

struct OutlineViewRowHeightProviderRInspectable :
		public winrt::implements<OutlineViewRowHeightProviderRInspectable,
				winrt::Windows::Foundation::IInspectable> {
	// Methods
	public:
										// Lifecycle methods
										OutlineViewRowHeightProviderRInspectable(
												OutlineViewRowHeightProvider& outlineViewRowHeightProvider) :
											mOutlineViewRowHeightProvider(outlineViewRowHeightProvider)
											{}

										// Instance methods
		OutlineViewRowHeightProvider&	GetReference() const
											{ return mOutlineViewRowHeightProvider; }

	// Properties
	private:
		OutlineViewRowHeightProvider&	mOutlineViewRowHeightProvider;
};
