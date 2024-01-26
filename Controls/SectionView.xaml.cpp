//----------------------------------------------------------------------------------------------------------------------
//	SectionView.xaml.cpp			©2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "SectionView.xaml.h"

#include "winrt\Microsoft.UI.Xaml.Controls.h"
#include "winrt\Windows.Foundation.Collections.h"
#include "winrt\Windows.Foundation.h"

#include "WinUIToolbox.SectionView.g.cpp"

using namespace winrt;
using namespace winrt::WinUIToolbox::implementation;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;

//----------------------------------------------------------------------------------------------------------------------
// MARK: SectionView

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
void SectionView::SetContent(const array_view<const UIElement>& uiElements)
//----------------------------------------------------------------------------------------------------------------------
{
	// Start with ScrollView
	ScrollView	scrollView = ScrollView();
	Content(scrollView);

	// Add StackPanel
	StackPanel	stackPanel;
	scrollView.Content(stackPanel);

	// Add GroupViews
	stackPanel.Children().ReplaceAll(uiElements);
}

//----------------------------------------------------------------------------------------------------------------------
void SectionView::SetContent(const hstring& string)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TextBlock	textBlock;
	textBlock.Text(string);
	textBlock.HorizontalAlignment(HorizontalAlignment::Center);
	textBlock.VerticalAlignment(VerticalAlignment::Center);

	// Set content
	Content(textBlock);
}
