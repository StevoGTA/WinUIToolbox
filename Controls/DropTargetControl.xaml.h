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

using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Windows;

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
			void						OnDragOver(const DragEventArgs& dragEventArgs) const;
			Foundation::IAsyncAction	OnDrop(const DragEventArgs& dragEventArgs) const;

										// Event methods
			event_token					StorageItemsEvent(
												const Windows::Foundation::EventHandler<
														Windows::Foundation::Collections::IVectorView<
																Windows::Storage::IStorageItem>>& handler);
			void						StorageItemsEvent(const event_token& token) noexcept;

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
