//----------------------------------------------------------------------------------------------------------------------
//	OutlineViewDataSource.h			©2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "winrt\Windows.Foundation.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
// MARK: OutlineViewDataSource

class OutlineViewDataSource {
	public:
		struct SortDescriptor {
			// Methods
			public:
										// Lifecycle methods
										SortDescriptor(const std::wstring& columnIdentifier, bool isAscending) :
											mColumnIdentifier(columnIdentifier), mIsAscending(isAscending)
											{}

										// Instance methods
				const	std::wstring&	getColumnIdentifier() const
											{ return mColumnIdentifier; }
						bool			getIsAscending() const
											{ return mIsAscending; }

			// Properties
			private:
				std::wstring	mColumnIdentifier;
				bool			mIsAscending;
		};

	// Methods
	public:
											// Lifecycle methods
		virtual								~OutlineViewDataSource() {}

											// Instance methods
		virtual	std::vector<std::wstring>	getChildIdentifiers(const std::optional<std::wstring>& parentIdentifier)
													const = 0;
		virtual	bool						isExpandable(const std::wstring& identifier) const = 0;

		virtual	void						noteSortDescriptorsDidChange(
													const std::vector<SortDescriptor>& sortDescriptors) = 0;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - OutlineViewDataSourceRInspectable

struct OutlineViewDataSourceRInspectable :
		public winrt::implements<OutlineViewDataSourceRInspectable, winrt::Windows::Foundation::IInspectable> {
	// Methods
	public:
								// Lifecycle methods
								OutlineViewDataSourceRInspectable(OutlineViewDataSource& outlineViewDataSource) :
									mOutlineViewDataSource(outlineViewDataSource)
									{}

								// Instance methods
		OutlineViewDataSource&	GetReference() const
									{ return mOutlineViewDataSource; }

	// Properties
	private:
		OutlineViewDataSource&	mOutlineViewDataSource;
};
