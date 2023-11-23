//----------------------------------------------------------------------------------------------------------------------
//	DropTargetControl.xaml.cpp			©2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "DropTargetControl.xaml.h"

#include "WinUIToolbox.DropTargetControl.g.cpp"

#include "winrt\Windows.ApplicationModel.DataTransfer.h"

using namespace winrt::WinUIToolbox::implementation;

using DataPackageOperation = winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation;
using StandardDataFormats = winrt::Windows::ApplicationModel::DataTransfer::StandardDataFormats;

//----------------------------------------------------------------------------------------------------------------------
// MARK: DropTargetControl::Internals

class DropTargetControl::Internals {
	public:
		Internals() {}

		event<IStorageItemVectorViewEventHandler>	mStorageItemsEvent;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - DropTargetControl

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
DropTargetControl::DropTargetControl()
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals();

	// Setup for receiving drops
	AllowDrop(true);
}

//----------------------------------------------------------------------------------------------------------------------
DropTargetControl::~DropTargetControl()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mInternals);
}

// MARK: Control methods

//----------------------------------------------------------------------------------------------------------------------
void DropTargetControl::OnDragOver(const DragEventArgs& dragEventArgs) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Set accepted operation
	dragEventArgs.AcceptedOperation(
			dragEventArgs.DataView().Contains(StandardDataFormats::StorageItems()) ?
					DataPackageOperation::Link : DataPackageOperation::None);
}

//----------------------------------------------------------------------------------------------------------------------
IAsyncAction DropTargetControl::OnDrop(const DragEventArgs& dragEventArgs) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Get Storage Items
	auto	storageItems = co_await dragEventArgs.DataView().GetStorageItemsAsync();

	// Trigger event
	mInternals->mStorageItemsEvent(*this, storageItems);
}

// MARK: Event methods

//----------------------------------------------------------------------------------------------------------------------
event_token DropTargetControl::StorageItemsEvent(const IStorageItemVectorViewEventHandler& handler)
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mStorageItemsEvent.add(handler);
}

//----------------------------------------------------------------------------------------------------------------------
void DropTargetControl::StorageItemsEvent(const event_token& token) noexcept
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mStorageItemsEvent.remove(token);
}
