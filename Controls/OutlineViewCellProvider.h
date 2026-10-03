//----------------------------------------------------------------------------------------------------------------------
//	OutlineViewCellProvider.h			©2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "winrt\Windows.Foundation.h"
#include "winrt\WinUIToolbox.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: OutlineViewCellProvider

class OutlineViewCellProvider {
	// Methods
	public:
														// Lifecycle methods
		virtual											~OutlineViewCellProvider() {}

														// Instance methods
		virtual	winrt::WinUIToolbox::OutlineViewCell	getCell(const winrt::hstring& columnIdentifier,
																const winrt::hstring& identifier,
																const winrt::WinUIToolbox::OutlineView&
																		outlineView) = 0;
		virtual	winrt::WinUIToolbox::OutlineViewCell	getRowCell(const winrt::hstring& identifier,
																const winrt::WinUIToolbox::OutlineView& outlineView)
															{ return nullptr; }
		virtual	winrt::hstring							getTextSearchString(const winrt::hstring& identifier) = 0;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - OutlineViewCellProviderRInspectable

struct OutlineViewCellProviderRInspectable :
		public winrt::implements<OutlineViewCellProviderRInspectable, winrt::Windows::Foundation::IInspectable> {
	// Methods
	public:
									// Lifecycle methods
									OutlineViewCellProviderRInspectable(
											OutlineViewCellProvider& outlineViewCellProvider) :
										mOutlineViewCellProvider(outlineViewCellProvider)
										{}

									// Instance methods
		OutlineViewCellProvider&	GetReference() const
										{ return mOutlineViewCellProvider; }

	// Properties
	private:
		OutlineViewCellProvider&	mOutlineViewCellProvider;
};
