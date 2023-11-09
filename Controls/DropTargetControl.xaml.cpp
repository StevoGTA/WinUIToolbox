//----------------------------------------------------------------------------------------------------------------------
//	DropTargetControl.xaml.cpp			©2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "DropTargetControl.xaml.h"

#include "WinUIToolbox.DropTargetControl.g.cpp"

#include "winrt\Windows.ApplicationModel.DataTransfer.h"

using namespace winrt;
using namespace winrt::Windows::ApplicationModel::DataTransfer;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Storage;
using namespace winrt::WinUIToolbox::implementation;

//----------------------------------------------------------------------------------------------------------------------
// MARK: DropTargetControl::Internals

class DropTargetControl::Internals {
	public:
		Internals() {}

		event<EventHandler<IVectorView<IStorageItem>>>	mStorageItemsEvent;
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
event_token DropTargetControl::StorageItemsEvent(const EventHandler<IVectorView<IStorageItem>>& handler)
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
