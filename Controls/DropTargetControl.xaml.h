//----------------------------------------------------------------------------------------------------------------------
//	DropTargetControl.xaml.h			©2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "SFoldersFiles.h"

#undef Delete

#include "Windows.h"

#include "WinUIToolbox.DropTargetControl.g.h"

#define Delete(x)	{ delete x; x = nil; }

#include "winrt\Microsoft.UI.Xaml.h"
#include "winrt\Microsoft.UI.Xaml.Markup.h"
#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt\Windows.Foundation.h"

using event_token = winrt::event_token;
using DragEventArgs = winrt::Microsoft::UI::Xaml::DragEventArgs;
using IAsyncAction = winrt::Windows::Foundation::IAsyncAction;
using IStorageItem = winrt::Windows::Storage::IStorageItem;
using IStorageItemVectorView = winrt::Windows::Foundation::Collections::IVectorView<IStorageItem>;
using IStorageItemVectorViewEventHandler = winrt::Windows::Foundation::EventHandler<IStorageItemVectorView>;

//----------------------------------------------------------------------------------------------------------------------
// MARK: winrt::WinUIToolbox::implementation

namespace winrt::WinUIToolbox::implementation {

	// MARK: DropTargetControl
	struct DropTargetControl : DropTargetControlT<DropTargetControl> {
		// Classes
		private:
			class Internals;

		// Methods
		public:
							// Lifecycle methods
							DropTargetControl();
							~DropTargetControl();

							// Control methods
			void			OnDragOver(const DragEventArgs& dragEventArgs) const;
			IAsyncAction	OnDrop(const DragEventArgs& dragEventArgs) const;

							// Event methods
			event_token		StorageItemsEvent(const IStorageItemVectorViewEventHandler& handler);
			void			StorageItemsEvent(const event_token& token) noexcept;

		// Properties
		private:
			Internals*	mInternals;
	};
}
//----------------------------------------------------------------------------------------------------------------------
// MARK: - winrt::WinUIToolbox::factory_implementation

namespace winrt::WinUIToolbox::factory_implementation
{
	// MARK: DropTargetControl
	struct DropTargetControl : DropTargetControlT<DropTargetControl, implementation::DropTargetControl> {
	};
}
