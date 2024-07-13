//----------------------------------------------------------------------------------------------------------------------
//	RadioButtonHelper.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "RadioButtonHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt\Windows.Foundation.h"

using IInspectable = winrt::Windows::Foundation::IInspectable;
using RoutedEventArgs = winrt::Microsoft::UI::Xaml::RoutedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: RadioButtonHelper::Internals

class RadioButtonHelper::Internals {
	public:
		Internals(RadioButton radiobutton) : mRadioButton(radiobutton) {}

		RadioButton	mRadioButton;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: RadioButtonHelper

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
RadioButtonHelper::RadioButtonHelper(RadioButton radiobutton)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(radiobutton);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
RadioButtonHelper& RadioButtonHelper::setChecked(bool isChecked)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mRadioButton.IsChecked(isChecked);

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
RadioButtonHelper& RadioButtonHelper::setClickProc(std::function<void()> clickProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals->mRadioButton.Click([clickProc](const IInspectable& sender, const RoutedEventArgs& routedEventArgs) {
		// Call proc
		clickProc();
	});

	return *this;
}
