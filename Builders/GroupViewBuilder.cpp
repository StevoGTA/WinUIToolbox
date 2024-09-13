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
		Internals(double itemLeadingInset, OV<double> itemTrailingInset) :
			mMarginLeading(itemLeadingInset), mMarginTrailing(itemTrailingInset)
			{}

		StackPanel	mStackPanel;

		double		mMarginLeading;
		OV<double>	mMarginTrailing;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: GroupViewBuilder

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
GroupViewBuilder::GroupViewBuilder(double itemLeadingInset, double itemTrailingInset)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(itemLeadingInset, OV<double>(itemTrailingInset));
}

//----------------------------------------------------------------------------------------------------------------------
GroupViewBuilder::GroupViewBuilder(double itemLeadingInset)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(itemLeadingInset, OV<double>());
}

//----------------------------------------------------------------------------------------------------------------------
GroupViewBuilder::GroupViewBuilder()
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(0.0, OV<double>());
}

//----------------------------------------------------------------------------------------------------------------------
GroupViewBuilder::~GroupViewBuilder()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mInternals);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
GroupViewBuilder& GroupViewBuilder::add(FrameworkElement frameworkElement)
//----------------------------------------------------------------------------------------------------------------------
{
	// Add View
	frameworkElement.Margin(ThicknessHelper::FromLengths(mInternals->mMarginLeading, 0.0, 0.0, 8.0));
	mInternals->mStackPanel.Children().Append(frameworkElement);

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
GroupViewBuilder& GroupViewBuilder::add(FrameworkElement frameworkElement, double leadingInset, double trailingInset)
//----------------------------------------------------------------------------------------------------------------------
{
	// Add View
	frameworkElement.Margin(ThicknessHelper::FromLengths(mInternals->mMarginLeading + leadingInset, 0.0, 0.0, 8.0));
	mInternals->mStackPanel.Children().Append(frameworkElement);

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
GroupViewBuilder&GroupViewBuilder::add(const hstring& title, FrameworkElement frameworkElement, double leadingInset,
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
UIElement GroupViewBuilder::getUIElement() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mStackPanel;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: ExpanderGroupViewBuilder::Internals

class ExpanderGroupViewBuilder::Internals {
	public:
		Internals(const hstring& title)
			{
				// Setup UI
				mExpander.Header(box_value(title));
				mExpander.HorizontalAlignment(HorizontalAlignment::Stretch);
				mExpander.HorizontalContentAlignment(HorizontalAlignment::Stretch);
				mExpander.Padding(ThicknessHelper::FromLengths(0.0, 8.0, 0.0, 0.0));
			}

		Expander	mExpander;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: ExpanderGroupViewBuilder

// MARK: Lifecycle methods
//----------------------------------------------------------------------------------------------------------------------
ExpanderGroupViewBuilder::ExpanderGroupViewBuilder(const hstring& title, double itemLeadingInset,
		double itemTrailingInset) : GroupViewBuilder(itemLeadingInset, itemTrailingInset)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(title);
	mInternals->mExpander.Content(GroupViewBuilder::getUIElement());
}

//----------------------------------------------------------------------------------------------------------------------
ExpanderGroupViewBuilder::ExpanderGroupViewBuilder(const hstring& title, double itemLeadingInset) :
		GroupViewBuilder(itemLeadingInset)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(title);
	mInternals->mExpander.Content(GroupViewBuilder::getUIElement());
}

//----------------------------------------------------------------------------------------------------------------------
ExpanderGroupViewBuilder::ExpanderGroupViewBuilder(const hstring& title) : GroupViewBuilder()
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(title);
	mInternals->mExpander.Content(GroupViewBuilder::getUIElement());
}

//----------------------------------------------------------------------------------------------------------------------
ExpanderGroupViewBuilder::~ExpanderGroupViewBuilder()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mInternals);
}

// MARK: GroupViewBuilder methods

//----------------------------------------------------------------------------------------------------------------------
UIElement ExpanderGroupViewBuilder::getUIElement() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mExpander;
}
