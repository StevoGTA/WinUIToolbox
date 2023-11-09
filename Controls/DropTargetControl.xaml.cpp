//----------------------------------------------------------------------------------------------------------------------
//	DropTargetControl.xaml.cpp			©2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "DropTargetControl.xaml.h"

#include "WinRTToolbox.DropTargetControl.g.cpp"

#include "IStorageItem+Helpers.h"

#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

using namespace winrt;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::WinRTToolbox::implementation;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::ApplicationModel::DataTransfer;

//----------------------------------------------------------------------------------------------------------------------
// MARK: DropTargetControl::Internals

class DropTargetControl::Internals {
	public:
		Internals() {}

		OV<Info>	mInfo;
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
	// Import
	mInternals->mInfo->receive(IStorageItemGetFoldersFiles(co_await dragEventArgs.DataView().GetStorageItemsAsync()));
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
void DropTargetControl::setInfo(const Info& info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mInfo.setValue(info);
}
