//----------------------------------------------------------------------------------------------------------------------
//	DropTargetView.xaml.cpp			©2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "DropTargetView.xaml.h"

#include "WinUIToolbox.DropTargetView.g.cpp"

#include "winrt\Windows.ApplicationModel.DataTransfer.h"

using namespace winrt::WinUIToolbox::implementation;

using DataPackageOperation = winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation;
using StandardDataFormats = winrt::Windows::ApplicationModel::DataTransfer::StandardDataFormats;

//----------------------------------------------------------------------------------------------------------------------
// MARK: DropTargetView::Internals

class DropTargetView::Internals {
	public:
		Internals() {}

		event<IStorageItemVectorViewEventHandler>	mStorageItemsEvent;
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
	// Set accepted operation
	dragEventArgs.AcceptedOperation(
			dragEventArgs.DataView().Contains(StandardDataFormats::StorageItems()) ?
					DataPackageOperation::Link : DataPackageOperation::None);
}

//----------------------------------------------------------------------------------------------------------------------
IAsyncAction DropTargetView::OnDrop(const DragEventArgs& dragEventArgs) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Get Storage Items
	auto	storageItems = co_await dragEventArgs.DataView().GetStorageItemsAsync();

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
