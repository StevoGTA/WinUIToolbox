//----------------------------------------------------------------------------------------------------------------------
//	DropTargetControl.xaml.h			©2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "SFoldersFiles.h"

#undef Delete

#include "Windows.h"

#include "WinRTToolbox.DropTargetControl.g.h"

#define Delete(x)	{ delete x; x = nil; }

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt/Windows.Foundation.h"

using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Windows;

//----------------------------------------------------------------------------------------------------------------------
// MARK: winrt::WinRTToolbox::implementation

namespace winrt::WinRTToolbox::implementation {

	struct DropTargetControl : DropTargetControlT<DropTargetControl> {

		// Info
		public:
			struct Info {

				// Procs
				typedef	void	(*FoldersFilesProc)(const SFoldersFiles& foldersFiles, void* userData);

				// Methods
				public:
							// Lifecycle methods
							Info(FoldersFilesProc foldersFilesProc, void* userData) :
								mFoldersFilesProc(foldersFilesProc), mUserData(userData)
								{}
							Info(const Info& other) :
								mFoldersFilesProc(other.mFoldersFilesProc), mUserData(other.mUserData)
								{}

							// Instance methods
					void	receive(const SFoldersFiles& foldersFiles) const
								{ mFoldersFilesProc(foldersFiles, mUserData); }

				// Properties
				private:
					FoldersFilesProc	mFoldersFilesProc;
					void*				mUserData;
			};

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

										// Instance methods
			void						setInfo(const Info& info);

		// Properties
		private:
			Internals*	mInternals;
	};
}

namespace winrt::WinRTToolbox::factory_implementation
{
	struct DropTargetControl : DropTargetControlT<DropTargetControl, implementation::DropTargetControl> {
	};
}
