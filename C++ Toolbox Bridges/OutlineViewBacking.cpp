//----------------------------------------------------------------------------------------------------------------------
//	OutlineViewBacking.cpp			Â©2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "OutlineViewBacking.h"

#include "CDictionary.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: OutlineViewBacking::Internals

class OutlineViewBacking::Internals {
	public:
		class Item {
			public:
						Item(const I<COutlineViewItem>& outlineViewItem, const OV<CString>& parentID) :
							mOutlineViewItem(outlineViewItem), mParentID(parentID),
									mNeedsReload(true)
							{}

				bool	hasChildren() const
							{ return mNeedsReload ? (mOutlineViewItem->getChildCount() > 0) : !mChildItems.isEmpty(); }

				I<COutlineViewItem>	mOutlineViewItem;
				OV<CString>			mParentID;

				TNArray<I<Item> >	mChildItems;
				TNArray<I<Item> >	mChildItemsSorted;
				bool				mNeedsReload;
		};

	// SortInfo
	public:
		struct SortInfo {
			SortInfo(const CompareOutlineViewItemsProc& compareOutlineViewItemsProc,
					const TArray<SSortDescriptor>& sortDescriptors) :
				mCompareOutlineViewItemsProc(compareOutlineViewItemsProc), mSortDescriptors(sortDescriptors)
				{}

			const	CompareOutlineViewItemsProc&	mCompareOutlineViewItemsProc;
			const	TArray<SSortDescriptor>&		mSortDescriptors;
		};

	// Methods
	public:
												Internals(const TArray<SSortDescriptor>& sortDescriptors,
														ReloadChildOutlineViewItemsProc reloadChildOutlineViewItemsProc,
														CompareOutlineViewItemsProc compareOutlineViewItemsProc) :
													mSortDescriptors(sortDescriptors),
															mReloadChildOutlineViewItemsProc(
																	reloadChildOutlineViewItemsProc),
															mCompareOutlineViewItemsProc(compareOutlineViewItemsProc)
													{}

												// Instance methods - items
				OR<I<Item> >					getItem(const CString& outlineViewItemID) const
													{ return mItemByItemID.get(outlineViewItemID); }
				void							addItems(const TArray<I<COutlineViewItem> >& outlineViewItems,
														const OV<CString>& parentID)
													{
														// Setup
														OR<I<Item> >	parentItem =
																				parentID.hasValue() ?
																						getItem(*parentID) :
																						OR<I<Item> >();
														if (parentID.hasValue() && !parentItem.hasReference())
															// No such parent
															return;

														TNArray<I<Item> >&	items =
																					parentItem.hasReference() ?
																							(*parentItem)->mChildItems :
																							mTopLevelItems;
														TNArray<I<Item> >&	itemsSorted =
																					parentItem.hasReference() ?
																							(*parentItem)->
																									mChildItemsSorted :
																							mTopLevelItemsSorted;

														// Iterate outline view items
														for (TArray<I<COutlineViewItem> >::Iterator iterator =
																		outlineViewItems.getIterator();
																iterator; iterator++) {
															// Check if already have
															if (mItemByItemID.contains((*iterator)->getID()))
																// Already have
																continue;

															// Add
															I<Item>	item(new Item(*iterator, parentID));
															mItemByItemID.set((*iterator)->getID(), item);
															items += item;
															itemsSorted += item;
														}

														// Handed children, so nothing to ask for
														if (parentItem.hasReference())
															// Loaded
															(*parentItem)->mNeedsReload = false;

														// Sort
														sortItems(itemsSorted);
													}
				void							removeItem(const I<Item>& item)
													{
														// Remove children first
														removeChildItems(*item);

														// Remove from parent
														TNArray<I<Item> >&	items =
																					item->mParentID.hasValue() ?
																							(*getItem(*item->mParentID))->
																									mChildItems :
																							mTopLevelItems;
														TNArray<I<Item> >&	itemsSorted =
																					item->mParentID.hasValue() ?
																							(*getItem(*item->mParentID))->
																									mChildItemsSorted :
																							mTopLevelItemsSorted;
														items -= item;
														itemsSorted -= item;

														// Remove
														mItemByItemID.remove(item->mOutlineViewItem->getID());
													}
				void							removeChildItems(Item& item)
													{
														// Iterate child items
														for (TArray<I<Item> >::Iterator iterator =
																		item.mChildItems.getIterator();
																iterator; iterator++) {
															// Remove its children
															removeChildItems(**iterator);

															// Remove
															mItemByItemID.remove(
																	(*iterator)->mOutlineViewItem->getID());
														}

														// Reset
														item.mChildItems.removeAll();
														item.mChildItemsSorted.removeAll();
														item.mNeedsReload = true;
													}
				void							loadChildItemsIfNeeded(Item& item)
													{
														// Check if needs reload
														if (!item.mNeedsReload)
															// Loaded
															return;

														// Ask for children
														addItems(
																mReloadChildOutlineViewItemsProc ?
																		mReloadChildOutlineViewItemsProc(
																				item.mOutlineViewItem) :
																		(TArray<I<COutlineViewItem> >)
																				TNArray<I<COutlineViewItem> >(),
																OV<CString>(item.mOutlineViewItem->getID()));
														item.mNeedsReload = false;
													}
				TArray<I<COutlineViewItem> >	getOutlineViewItems(const TArray<I<Item> >& items) const
													{
														// Collect
														TNArray<I<COutlineViewItem> >	outlineViewItems;
														for (TArray<I<Item> >::Iterator iterator = items.getIterator();
																iterator; iterator++)
															// Add
															outlineViewItems += (*iterator)->mOutlineViewItem;

														return outlineViewItems;
													}

												// Instance methods - sorting
				void							sortItems(TNArray<I<Item> >& items)
													{
														// Check if sorting
														if (!mCompareOutlineViewItemsProc ||
																mSortDescriptors.isEmpty() || (items.getCount() < 2))
															// Not sorting
															return;

														// Sort
														SortInfo	sortInfo(mCompareOutlineViewItemsProc,
																			mSortDescriptors);
														items.sort((TNArray<I<Item> >::CompareProc) compareItems,
																&sortInfo);
													}
				void							resortAll()
													{
														// Top level
														mTopLevelItemsSorted = TNArray<I<Item> >(mTopLevelItems);
														sortItems(mTopLevelItemsSorted);

														// Every item with loaded children
														for (TNDictionary<I<Item> >::ValueIterator iterator =
																		mItemByItemID.getValueIterator();
																iterator; iterator++) {
															// Check for children
															Item&	item = **iterator;
															if (!item.mChildItems.isEmpty()) {
																// Sort
																item.mChildItemsSorted =
																		TNArray<I<Item> >(item.mChildItems);
																sortItems(item.mChildItemsSorted);
															}
														}
													}

												// Class methods
		static	bool							compareItems(const I<Item>& item1, const I<Item>& item2,
														SortInfo* sortInfo)
													{ return sortInfo->mCompareOutlineViewItemsProc(
															item1->mOutlineViewItem, item2->mOutlineViewItem,
															sortInfo->mSortDescriptors); }

	// Properties
	public:
		TNArray<SSortDescriptor>		mSortDescriptors;

		ReloadChildOutlineViewItemsProc	mReloadChildOutlineViewItemsProc;
		CompareOutlineViewItemsProc		mCompareOutlineViewItemsProc;
		SortDescriptorsDidChangeProc	mSortDescriptorsDidChangeProc;

		TNDictionary<I<Item> >			mItemByItemID;
		TNArray<I<Item> >				mTopLevelItems;
		TNArray<I<Item> >				mTopLevelItemsSorted;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - OutlineViewBacking

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
OutlineViewBacking::OutlineViewBacking(ReloadChildOutlineViewItemsProc reloadChildOutlineViewItemsProc,
		CompareOutlineViewItemsProc compareOutlineViewItemsProc, const TArray<SSortDescriptor>& sortDescriptors) :
		OutlineViewDataSource()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals(sortDescriptors, reloadChildOutlineViewItemsProc, compareOutlineViewItemsProc);
}

//----------------------------------------------------------------------------------------------------------------------
OutlineViewBacking::~OutlineViewBacking()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mInternals);
}

// MARK: OutlineViewDataSource methods

//----------------------------------------------------------------------------------------------------------------------
std::vector<std::wstring> OutlineViewBacking::getChildIdentifiers(const std::optional<std::wstring>& parentIdentifier)
		const
//----------------------------------------------------------------------------------------------------------------------
{
	// Which items
	const	TNArray<I<Internals::Item> >*	items;
	if (parentIdentifier.has_value()) {
		// Child - loaded the first time the view asks
		OR<I<Internals::Item> >	parentItem = mInternals->getItem(CString(parentIdentifier->c_str()));
		if (!parentItem.hasReference())
			// No such parent
			return std::vector<std::wstring>();

		mInternals->loadChildItemsIfNeeded(**parentItem);
		items = &(*parentItem)->mChildItemsSorted;
	} else
		// Top level
		items = &mInternals->mTopLevelItemsSorted;

	// Collect
	std::vector<std::wstring>	identifiers;
	identifiers.reserve(items->getCount());
	for (TArray<I<Internals::Item> >::Iterator iterator = items->getIterator(); iterator; iterator++)
		// Add
		identifiers.push_back(std::wstring((*iterator)->mOutlineViewItem->getID().getOSString()));

	return identifiers;
}

//----------------------------------------------------------------------------------------------------------------------
bool OutlineViewBacking::isExpandable(const std::wstring& identifier) const
//----------------------------------------------------------------------------------------------------------------------
{
	return isExpandable(CString(identifier.c_str()));
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineViewBacking::noteSortDescriptorsDidChange(const std::vector<SortDescriptor>& sortDescriptors)
//----------------------------------------------------------------------------------------------------------------------
{
	// Convert
	TNArray<SSortDescriptor>	sortDescriptors_;
	for (const SortDescriptor& sortDescriptor : sortDescriptors)
		// Add
		sortDescriptors_ +=
				SSortDescriptor(CString(sortDescriptor.getColumnIdentifier().c_str()), sortDescriptor.getIsAscending());

	// Update
	setSortDescriptors(sortDescriptors_);
}

// MARK: Instance methods - sorting

//----------------------------------------------------------------------------------------------------------------------
const TArray<SSortDescriptor>& OutlineViewBacking::getSortDescriptors() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mSortDescriptors;
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineViewBacking::setSortDescriptors(const TArray<SSortDescriptor>& sortDescriptors)
//----------------------------------------------------------------------------------------------------------------------
{
	// Store and resort - the client reloads the view
	mInternals->mSortDescriptors = TNArray<SSortDescriptor>(sortDescriptors);
	mInternals->resortAll();

	// Tell the client
	if (mInternals->mSortDescriptorsDidChangeProc)
		mInternals->mSortDescriptorsDidChangeProc(mInternals->mSortDescriptors);
}

// MARK: Instance methods - content

//----------------------------------------------------------------------------------------------------------------------
void OutlineViewBacking::setOutlineViewItems(const TArray<I<COutlineViewItem> >& outlineViewItems,
		const OV<CString>& parentID)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check level
	if (!parentID.hasValue()) {
		// Top level - everything goes
		mInternals->mTopLevelItems.removeAll();
		mInternals->mTopLevelItemsSorted.removeAll();
		mInternals->mItemByItemID.removeAll();
	} else {
		// Child
		OR<I<Internals::Item> >	parentItem = mInternals->getItem(*parentID);
		if (!parentItem.hasReference())
			// No such parent
			return;

		// Current children go
		mInternals->removeChildItems(**parentItem);
	}

	// Add
	mInternals->addItems(outlineViewItems, parentID);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineViewBacking::addOutlineViewItems(const TArray<I<COutlineViewItem> >& outlineViewItems,
		const OV<CString>& parentID)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->addItems(outlineViewItems, parentID);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineViewBacking::removeOutlineViewItems(const TArray<I<COutlineViewItem> >& outlineViewItems)
//----------------------------------------------------------------------------------------------------------------------
{
	removeOutlineViewItemIDs(COutlineViewItem::getIDs(outlineViewItems));
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineViewBacking::removeOutlineViewItemIDs(const TArray<CString>& outlineViewItemIDs)
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate IDs
	for (TArray<CString>::Iterator iterator = outlineViewItemIDs.getIterator(); iterator; iterator++) {
		// Get item
		OR<I<Internals::Item> >	item = mInternals->getItem(*iterator);
		if (item.hasReference())
			// Remove - holding a reference of our own, as removing takes the map's away
			mInternals->removeItem(I<Internals::Item>(*item));
	}
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineViewBacking::reloadChildOutlineViewItems(const CString& outlineViewItemID)
//----------------------------------------------------------------------------------------------------------------------
{
	// Get item
	OR<I<Internals::Item> >	item = mInternals->getItem(outlineViewItemID);
	if (!item.hasReference())
		// No such item
		return;

	// Current children go - asked for again the next time the view wants them
	mInternals->removeChildItems(**item);
}

//----------------------------------------------------------------------------------------------------------------------
TArray<I<COutlineViewItem> > OutlineViewBacking::getTopLevelOutlineViewItems() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->getOutlineViewItems(mInternals->mTopLevelItemsSorted);
}

//----------------------------------------------------------------------------------------------------------------------
TArray<I<COutlineViewItem> > OutlineViewBacking::getChildOutlineViewItems(const CString& outlineViewItemID) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Get item
	OR<I<Internals::Item> >	item = mInternals->getItem(outlineViewItemID);
	if (!item.hasReference())
		// No such item
		return TNArray<I<COutlineViewItem> >();

	// Loaded on demand, as for the view
	mInternals->loadChildItemsIfNeeded(**item);

	return mInternals->getOutlineViewItems((*item)->mChildItemsSorted);
}

//----------------------------------------------------------------------------------------------------------------------
OR<I<COutlineViewItem> > OutlineViewBacking::getOutlineViewItem(const CString& outlineViewItemID) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Get item
	OR<I<Internals::Item> >	item = mInternals->getItem(outlineViewItemID);

	return item.hasReference() ? OR<I<COutlineViewItem> >((*item)->mOutlineViewItem) : OR<I<COutlineViewItem> >();
}

//----------------------------------------------------------------------------------------------------------------------
TArray<I<COutlineViewItem> > OutlineViewBacking::getOutlineViewItems(const TArray<CString>& outlineViewItemIDs) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Collect
	TNArray<I<COutlineViewItem> >	outlineViewItems;
	for (TArray<CString>::Iterator iterator = outlineViewItemIDs.getIterator(); iterator; iterator++) {
		// Get item
		OR<I<Internals::Item> >	item = mInternals->getItem(*iterator);
		if (item.hasReference())
			// Add
			outlineViewItems += (*item)->mOutlineViewItem;
	}

	return outlineViewItems;
}

//----------------------------------------------------------------------------------------------------------------------
OV<CString> OutlineViewBacking::getParentOutlineViewItemID(const CString& outlineViewItemID) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Get item
	OR<I<Internals::Item> >	item = mInternals->getItem(outlineViewItemID);

	return item.hasReference() ? (*item)->mParentID : OV<CString>();
}

//----------------------------------------------------------------------------------------------------------------------
bool OutlineViewBacking::isExpandable(const CString& outlineViewItemID) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Get item
	OR<I<Internals::Item> >	item = mInternals->getItem(outlineViewItemID);

	return item.hasReference() && (*item)->hasChildren();
}

// MARK: Instance methods - procs

//----------------------------------------------------------------------------------------------------------------------
void OutlineViewBacking::setSortDescriptorsDidChangeProc(SortDescriptorsDidChangeProc proc)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mSortDescriptorsDidChangeProc = proc;
}
