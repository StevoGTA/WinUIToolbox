//	ComboBoxBuilder.cpp			©2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "ComboBoxBuilder.h"

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>

using namespace winrt::Microsoft::UI::Xaml;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ComboBoxBuilder::Internals

class ComboBoxBuilder::Internals {
	public:
		Internals(ComboBox comboBox, Options options) :
			mComboBox(comboBox), mOptions(options), mMinimumWidth(0.0)
			{}

		ComboBox	mComboBox;
		Options		mOptions;
		float		mMinimumWidth;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: ComboBoxBuilder

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
ComboBoxBuilder::ComboBoxBuilder(ComboBox comboBox, Options options)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(comboBox, options);

	// Setup UI
	mInternals->mComboBox.Items().Clear();
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
void ComboBoxBuilder::add(const hstring& content, const IInspectable& tag, bool isSelected)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	ComboBoxItem	comboBoxItem;
	comboBoxItem.Content(box_value(content));
	comboBoxItem.Tag(tag);

	// Add
	mInternals->mComboBox.Items().Append(comboBoxItem);

	// Check selected
	if (isSelected)
		// Select
		mInternals->mComboBox.SelectedIndex(mInternals->mComboBox.Items().Size() - 1);
}

//----------------------------------------------------------------------------------------------------------------------
void ComboBoxBuilder::add(const IPropertyValue& comboBoxItem, bool isSelected)
//----------------------------------------------------------------------------------------------------------------------
{
	// Add
	mInternals->mComboBox.Items().Append(comboBoxItem);

	// Check selected
	if (isSelected)
		// Select
		mInternals->mComboBox.SelectedIndex(mInternals->mComboBox.Items().Size() - 1);
}

//----------------------------------------------------------------------------------------------------------------------
void ComboBoxBuilder::addSeparator()
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	ComboBoxItem	comboBoxItem;
	comboBoxItem.Content(NavigationViewItemSeparator());
	comboBoxItem.IsEnabled(false);

	// Add
	mInternals->mComboBox.Items().Append(comboBoxItem);
}
