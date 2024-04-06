//----------------------------------------------------------------------------------------------------------------------
//	CheckBoxHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "winrt\Microsoft.UI.Xaml.Controls.h"

#include <functional>

using CheckBox = winrt::Microsoft::UI::Xaml::Controls::CheckBox;
using IInspectable = winrt::Windows::Foundation::IInspectable;
using RoutedEventArgs = winrt::Microsoft::UI::Xaml::RoutedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: CheckBoxHelper

class CheckBoxHelper {
	// Classes
	private:
		class Internals;

	// Methods
	public:
				// Lifecycle methods
				CheckBoxHelper(CheckBox checkbox);

				// Instance methods
		void	setStateChangedProc(
						std::function<void(const IInspectable& sender, const RoutedEventArgs& routedEventArgs)>
								stateChangedProc);

	// Properties
	private:
		Internals*	mInternals;
};
