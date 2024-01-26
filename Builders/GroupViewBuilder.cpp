//----------------------------------------------------------------------------------------------------------------------
//	GroupViewBuilder.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "GroupViewBuilder.h"

#include "winrt\Windows.Foundation.Collections.h"

#include "TWrappers.h"

using namespace winrt::Microsoft::UI;

#include "winrt\Microsoft.UI.Xaml.Media.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: GroupViewBuilder::Internals

class GroupViewBuilder::Internals {
	public:
		Internals(const hstring& title, double itemLeadingInset, OV<double> itemTrailingInset) :
			mMarginLeading(itemLeadingInset), mMarginTrailing(itemTrailingInset)
			{
				// Setup UI
				mExpander.Header(box_value(title));
				mExpander.Content(mStackPanel);
				mExpander.HorizontalAlignment(HorizontalAlignment::Stretch);
				mExpander.HorizontalContentAlignment(HorizontalAlignment::Stretch);
				mExpander.Padding(ThicknessHelper::FromLengths(0.0, 8.0, 0.0, 0.0));
			}

		Expander	mExpander;
		StackPanel	mStackPanel;

		double		mMarginLeading;
		OV<double>	mMarginTrailing;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: GroupViewBuilder

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
GroupViewBuilder::GroupViewBuilder(const hstring& title, double itemLeadingInset, double itemTrailingInset)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(title, itemLeadingInset, OV<double>(itemTrailingInset));
}

//----------------------------------------------------------------------------------------------------------------------
GroupViewBuilder::GroupViewBuilder(const hstring& title, double itemLeadingInset)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(title, itemLeadingInset, OV<double>());
}

//----------------------------------------------------------------------------------------------------------------------
GroupViewBuilder::GroupViewBuilder(const hstring& title)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(title, 0.0, OV<double>());
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
GroupViewBuilder& GroupViewBuilder::Add(FrameworkElement frameworkElement)
//----------------------------------------------------------------------------------------------------------------------
{
	// Add View
	frameworkElement.Margin(ThicknessHelper::FromLengths(mInternals->mMarginLeading, 0.0, 0.0, 8.0));
	mInternals->mStackPanel.Children().Append(frameworkElement);

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
GroupViewBuilder& GroupViewBuilder::Add(FrameworkElement frameworkElement, double leadingInset, double trailingInset)
//----------------------------------------------------------------------------------------------------------------------
{
	// Add View
	frameworkElement.Margin(ThicknessHelper::FromLengths(mInternals->mMarginLeading + leadingInset, 0.0, 0.0, 8.0));
	mInternals->mStackPanel.Children().Append(frameworkElement);

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
GroupViewBuilder&GroupViewBuilder:: Add(const hstring& title, FrameworkElement frameworkElement, double leadingInset,
		double trailingInset)
//----------------------------------------------------------------------------------------------------------------------
{
	// Add title
	TextBlock	titleTextBlock;
	titleTextBlock.Text(title);
	titleTextBlock.Margin(ThicknessHelper::FromLengths(mInternals->mMarginLeading, 0.0, 0.0, 0.0));
	mInternals->mStackPanel.Children().Append(titleTextBlock);

	// Add View
	frameworkElement.Margin(ThicknessHelper::FromLengths(mInternals->mMarginLeading + leadingInset, 0.0, 0.0, 8.0));
	mInternals->mStackPanel.Children().Append(frameworkElement);

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
UIElement GroupViewBuilder::UIElement() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mExpander;
}
