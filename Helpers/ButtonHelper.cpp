//----------------------------------------------------------------------------------------------------------------------
//	ButtonHelper.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "ButtonHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt\Windows.Foundation.h"

using IInspectable = winrt::Windows::Foundation::IInspectable;
using RoutedEventArgs = winrt::Microsoft::UI::Xaml::RoutedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ButtonHelper::Internals

class ButtonHelper::Internals {
	public:
		Internals(Button button) : mButton(button) {}

		Button	mButton;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: ButtonHelper

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
ButtonHelper::ButtonHelper(Button button)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(button);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
ButtonHelper& ButtonHelper::setClickedProc(std::function<void()> clickedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals->mButton.Click(
			[clickedProc](const IInspectable& sender, const RoutedEventArgs& routedEventArgs) { clickedProc(); });

	return *this;
}
