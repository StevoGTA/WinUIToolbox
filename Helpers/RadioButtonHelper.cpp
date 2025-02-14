//----------------------------------------------------------------------------------------------------------------------
//	RadioButtonHelper.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "RadioButtonHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt\Windows.Foundation.h"

using IInspectable = winrt::Windows::Foundation::IInspectable;
using RoutedEventArgs = winrt::Microsoft::UI::Xaml::RoutedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	RadioButton	sRadioButtonInUpdate(nullptr);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - RadioButtonHelper

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
RadioButtonHelper& RadioButtonHelper::setChecked(bool isChecked)
//----------------------------------------------------------------------------------------------------------------------
{
	// Set value
	sRadioButtonInUpdate = getRadioButton();
	getRadioButton().IsChecked(isChecked);
	sRadioButtonInUpdate = nullptr;

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
RadioButtonHelper& RadioButtonHelper::setClickProc(std::function<void()> clickProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	getRadioButton().Click([clickProc](const IInspectable& sender, const RoutedEventArgs& routedEventArgs) {
		// Check if handling events
		if (sender == sRadioButtonInUpdate)
			return;

		// Call proc
		clickProc();
	});

	return *this;
}
