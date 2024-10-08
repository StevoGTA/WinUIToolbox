//----------------------------------------------------------------------------------------------------------------------
//	ListViewHelper.cpp			�2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "ListViewHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt\Windows.Foundation.h"

using IInspectable = winrt::Windows::Foundation::IInspectable;
using SelectionChangedEventArgs = winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs;

using namespace winrt;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ListViewHelper

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
ListViewHelper& ListViewHelper::setSelectionChangedProc(std::function<void()> selectionChangedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	getListView().SelectionChanged(
			[selectionChangedProc](const IInspectable& sender, const SelectionChangedEventArgs& selectionChangedArgs) {
				// Call proc
				selectionChangedProc();
			});

	return *this;
}
