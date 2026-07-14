//----------------------------------------------------------------------------------------------------------------------
//	ButtonHelper.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "ButtonHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt\Microsoft.UI.Xaml.Input.h"
#include "winrt\Windows.Foundation.h"

using IInspectable = winrt::Windows::Foundation::IInspectable;
using PointerRoutedEventArgs = winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs;
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
			[clickedProc](const IInspectable& sender, const RoutedEventArgs& routedEventArgs){ clickedProc(); });

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ButtonHelper& ButtonHelper::setPointerEnteredProc(std::function<void()> pointerEnteredProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	getButton().PointerEntered(
			[pointerEnteredProc](const IInspectable& sender, const PointerRoutedEventArgs& pointerRoutedEventArgs){
				// Call proc
				pointerEnteredProc();
			});

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ButtonHelper& ButtonHelper::setPointerExitedProc(std::function<void()> pointerExitedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	getButton().PointerExited(
			[pointerExitedProc](const IInspectable& sender, const PointerRoutedEventArgs& pointerRoutedEventArgs){
				// Call proc
				pointerExitedProc();
			});

	return *this;
}
