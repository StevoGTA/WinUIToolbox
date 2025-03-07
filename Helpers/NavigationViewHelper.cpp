//----------------------------------------------------------------------------------------------------------------------
//	NavigationViewHelper.cpp			�2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "NavigationViewHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt\Windows.Foundation.h"
#include "winrt\Windows.Foundation.Collections.h"

using IInspectable = winrt::Windows::Foundation::IInspectable;
using NavigationViewItem = winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem;
using NavigationViewSelectionChangedEventArgs =
		winrt::Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs;
using RoutedEventArgs = winrt::Microsoft::UI::Xaml::RoutedEventArgs;

using namespace winrt;

//----------------------------------------------------------------------------------------------------------------------
// MARK: NavigationViewHelper

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
NavigationViewHelper& NavigationViewHelper::setNavigationViewItemIsEnabled(int index, bool isEnabled)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	NavigationViewItem	navigationViewItem = getNavigationView().MenuItems().GetAt(index).as<NavigationViewItem>();
	navigationViewItem.IsEnabled(isEnabled);
	navigationViewItem.Opacity(isEnabled ? 1.0 : 0.5);

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
NavigationViewHelper& NavigationViewHelper::setSelectedTagChangedProc(
		std::function<void(const winrt::hstring& tag)> selectedTagChangedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	getNavigationView().SelectionChanged(
			[selectedTagChangedProc](const NavigationView& sender,
					const NavigationViewSelectionChangedEventArgs& navigationViewSelectionChangedArgs) {
				// Call proc
				selectedTagChangedProc(
						unbox_value<winrt::hstring>(navigationViewSelectionChangedArgs.SelectedItemContainer().Tag()));
			});

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
NavigationViewHelper& NavigationViewHelper::setLoadedProc(std::function<void()> loadedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	getNavigationView().Loaded([loadedProc](const IInspectable& sender, const RoutedEventArgs& routedEventArgs) {
		// Call proc
		loadedProc();
	});

	return *this;
}
