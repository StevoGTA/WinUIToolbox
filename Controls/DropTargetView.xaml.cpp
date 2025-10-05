//----------------------------------------------------------------------------------------------------------------------
//	DropTargetView.xaml.cpp			©2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "DropTargetView.xaml.h"

#include "DragAndDropCoordinator.h"

#include "WinUIToolbox.DropTargetView.g.cpp"

#include "winrt\Windows.ApplicationModel.DataTransfer.h"

using namespace winrt::WinUIToolbox::implementation;

using DataPackageOperation = winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation;
using StandardDataFormats = winrt::Windows::ApplicationModel::DataTransfer::StandardDataFormats;

//----------------------------------------------------------------------------------------------------------------------
// MARK: DropTargetView::Internals

class DropTargetView::Internals {
	public:
		Internals() : mDragAndDropPriority(0) {}

		event<IStorageItemVectorViewEventHandler>	mStorageItemsEvent;

		int32_t										mDragAndDropPriority;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - DropTargetView

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
DropTargetView::DropTargetView() : DropTargetViewT<DropTargetView>()
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals();

	// Setup for receiving drops
	AllowDrop(true);
}

//----------------------------------------------------------------------------------------------------------------------
DropTargetView::~DropTargetView()
//----------------------------------------------------------------------------------------------------------------------
{
	delete mInternals;
}

// MARK: Control methods

//----------------------------------------------------------------------------------------------------------------------
void DropTargetView::OnDragOver(const DragEventArgs& dragEventArgs) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Notify DragAndDropCoordinator
	DragAndDropCoordinator::shared().addHandler(*this, mInternals->mDragAndDropPriority);

	// Check if handling Drop
	if (!DragAndDropCoordinator::shared().shouldHandleDrag(*this))
		// Not handling drop
		return;

	// Set accepted operation
	dragEventArgs.AcceptedOperation(
			dragEventArgs.DataView().Contains(StandardDataFormats::StorageItems()) ?
					DataPackageOperation::Link : DataPackageOperation::None);
}

//----------------------------------------------------------------------------------------------------------------------
void DropTargetView::OnDragLeave(const DragEventArgs& dragEventArgs) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Notify DragAndDropCoordinator
	DragAndDropCoordinator::shared().removeHandler(*this);
}

//----------------------------------------------------------------------------------------------------------------------
IAsyncAction DropTargetView::OnDrop(const DragEventArgs& dragEventArgs) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if handling Drop
	if (!DragAndDropCoordinator::shared().shouldHandleDrop(*this, dragEventArgs))
		// Not handling drop
		co_return;

	// Get Storage Items
	IStorageItemVectorView	storageItems = co_await dragEventArgs.DataView().GetStorageItemsAsync();

	// Trigger event
	mInternals->mStorageItemsEvent(*this, storageItems);
}

// MARK: Event methods

//----------------------------------------------------------------------------------------------------------------------
event_token DropTargetView::StorageItemsEvent(const IStorageItemVectorViewEventHandler& handler)
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mStorageItemsEvent.add(handler);
}

//----------------------------------------------------------------------------------------------------------------------
void DropTargetView::StorageItemsEvent(const event_token& token) noexcept
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mStorageItemsEvent.remove(token);
}

// MARK: Property methods

//----------------------------------------------------------------------------------------------------------------------
int32_t DropTargetView::DragAndDropPriority()
//----------------------------------------------------------------------------------------------------------------------
{
    return mInternals->mDragAndDropPriority;
}

//----------------------------------------------------------------------------------------------------------------------
void DropTargetView::DragAndDropPriority(int32_t value)
//----------------------------------------------------------------------------------------------------------------------
{
    mInternals->mDragAndDropPriority = value;
}
