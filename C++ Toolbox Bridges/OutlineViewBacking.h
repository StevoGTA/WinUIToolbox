//----------------------------------------------------------------------------------------------------------------------
//	OutlineViewBacking.h			©2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "COutlineViewItem.h"
#include "SSortDescriptor.h"

#include "OutlineViewDataSource.h"

#include <functional>

//----------------------------------------------------------------------------------------------------------------------
// MARK: OutlineViewBacking
/*
	The OutlineView data source for a C++ Toolbox codebase: the tree of Outline View Items behind the view, in C++
		Toolbox terms.  Items are held by ID; a parent's children are asked for the first time the view wants them and
		kept in sorted order from then on.  The sort descriptors and the comparison live here because sorting is the one
		thing the view leaves to its data source.

	Everything else - the columns, which items are expanded, the flat list of rows, the selection - belongs to the
		view.  Ask the view about those; ask the backing for items.  Every change made here is followed by a reload on
		the view (ReloadAllItems, or ReloadItemChildren for a parent).  Nothing here is WinRT.
*/

class OutlineViewBacking : public OutlineViewDataSource {
	// Procs
	public:
		typedef	std::function<TArray<I<COutlineViewItem> >(const I<COutlineViewItem>& outlineViewItem)>
						ReloadChildOutlineViewItemsProc;
		typedef	std::function<bool(const I<COutlineViewItem>& outlineViewItem1,
								const I<COutlineViewItem>& outlineViewItem2,
								const TArray<SSortDescriptor>& sortDescriptors)>
						CompareOutlineViewItemsProc;
		typedef	std::function<void(const TArray<SSortDescriptor>& sortDescriptors)>	SortDescriptorsDidChangeProc;

	// Classes
	private:
		class Internals;

	// Methods
	public:
												// Lifecycle methods
												OutlineViewBacking(
														ReloadChildOutlineViewItemsProc reloadChildOutlineViewItemsProc,
														CompareOutlineViewItemsProc compareOutlineViewItemsProc,
														const TArray<SSortDescriptor>& sortDescriptors =
																TNArray<SSortDescriptor>());
												~OutlineViewBacking();

												// OutlineViewDataSource methods
				std::vector<std::wstring>		getChildIdentifiers(const std::optional<std::wstring>& parentIdentifier)
														const;
				bool							isExpandable(const std::wstring& identifier) const;
				void							noteSortDescriptorsDidChange(
														const std::vector<SortDescriptor>& sortDescriptors);

												// Instance methods - sorting
		const	TArray<SSortDescriptor>&		getSortDescriptors() const;
				void							setSortDescriptors(const TArray<SSortDescriptor>& sortDescriptors);

												// Instance methods - content
				void							setOutlineViewItems(
														const TArray<I<COutlineViewItem> >& outlineViewItems,
														const OV<CString>& parentID = OV<CString>());
				void							addOutlineViewItems(
														const TArray<I<COutlineViewItem> >& outlineViewItems,
														const OV<CString>& parentID = OV<CString>());
				void							removeOutlineViewItems(
														const TArray<I<COutlineViewItem> >& outlineViewItems);
				void							removeOutlineViewItemIDs(const TArray<CString>& outlineViewItemIDs);
				void							reloadChildOutlineViewItems(const CString& outlineViewItemID);

				TArray<I<COutlineViewItem> >	getTopLevelOutlineViewItems() const;
				TArray<I<COutlineViewItem> >	getChildOutlineViewItems(const CString& outlineViewItemID) const;
				OR<I<COutlineViewItem> >		getOutlineViewItem(const CString& outlineViewItemID) const;
				TArray<I<COutlineViewItem> >	getOutlineViewItems(const TArray<CString>& outlineViewItemIDs) const;
				OV<CString>						getParentOutlineViewItemID(const CString& outlineViewItemID) const;
				bool							isExpandable(const CString& outlineViewItemID) const;

												// Instance methods - procs
				void							setSortDescriptorsDidChangeProc(SortDescriptorsDidChangeProc proc);

	// Properties
	private:
		Internals*	mInternals;
};
