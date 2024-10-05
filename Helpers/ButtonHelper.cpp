//----------------------------------------------------------------------------------------------------------------------
//	ButtonHelper.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "ButtonHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt\Windows.Foundation.h"

using IInspectable = winrt::Windows::Foundation::IInspectable;
using RoutedEventArgs = winrt::Microsoft::UI::Xaml::RoutedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ButtonHelper

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
ButtonHelper& ButtonHelper::setClickedProc(std::function<void()> clickedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	getButton().Click(
			[clickedProc](const IInspectable& sender, const RoutedEventArgs& routedEventArgs) { clickedProc(); });

	return *this;
}
