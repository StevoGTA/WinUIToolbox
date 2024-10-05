//----------------------------------------------------------------------------------------------------------------------
//	RadioButtonHelper.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "RadioButtonHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt\Windows.Foundation.h"

using IInspectable = winrt::Windows::Foundation::IInspectable;
using RoutedEventArgs = winrt::Microsoft::UI::Xaml::RoutedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: RadioButtonHelper

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
RadioButtonHelper& RadioButtonHelper::setChecked(bool isChecked)
//----------------------------------------------------------------------------------------------------------------------
{
	getRadioButton().IsChecked(isChecked);

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
RadioButtonHelper& RadioButtonHelper::setClickProc(std::function<void()> clickProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	getRadioButton().Click([clickProc](const IInspectable& sender, const RoutedEventArgs& routedEventArgs) {
		// Call proc
		clickProc();
	});

	return *this;
}
