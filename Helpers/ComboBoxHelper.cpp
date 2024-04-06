//----------------------------------------------------------------------------------------------------------------------
//	ComboBoxHelper.cpp			©2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "ComboBoxHelper.h"

#include "winrt\Windows.Foundation.Collections.h"
#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"

using namespace winrt::Microsoft::UI::Xaml;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ComboBoxHelper::Internals

class ComboBoxHelper::Internals {
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
// MARK: ComboBoxHelper

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
ComboBoxHelper::ComboBoxHelper(ComboBox comboBox, Options options)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(comboBox, options);

	// Setup UI
	if (options & kOptionsClearItems)
		// Clear items
		mInternals->mComboBox.Items().Clear();
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
void ComboBoxHelper::addItem(const hstring& content, const IInspectable& tag, bool isSelected)
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
void ComboBoxHelper::addItem(const IPropertyValue& comboBoxItem, bool isSelected)
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
void ComboBoxHelper::addSeparatorItem()
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	ComboBoxItem	comboBoxItem;
	comboBoxItem.Content(NavigationViewItemSeparator());
	comboBoxItem.IsEnabled(false);

	// Add
	mInternals->mComboBox.Items().Append(comboBoxItem);
}

//----------------------------------------------------------------------------------------------------------------------
bool ComboBoxHelper::selectTag(std::function<bool(const IInspectable& tag)> proc) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate items
	auto	items = mInternals->mComboBox.Items();
	for (uint32_t i = 0; i < items.Size(); i++) {
		// Check tag
		if (proc(items.GetAt(i).as<ComboBoxItem>().Tag().as<IInspectable>())) {
			// Found item
			mInternals->mComboBox.SelectedIndex(i);

			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool ComboBoxHelper::selectIntTag(std::function<bool(int tag)> proc) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate items
	auto	items = mInternals->mComboBox.Items();
	for (uint32_t i = 0; i < items.Size(); i++) {
		// Check tag
		auto	tag = items.GetAt(i).as<ComboBoxItem>().Tag();
		auto	intValue = tag.try_as<int>();
		auto	value = intValue ? *intValue : std::stoi(std::basic_string<TCHAR>(tag.as<winrt::hstring>()));
		if (proc(value)) {
			// Found item
			mInternals->mComboBox.SelectedIndex(i);

			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------------------------------------------------
void ComboBoxHelper::selectedTagChanged(std::function<void(const IInspectable& tag)> proc) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup SelectionChanged
	mInternals->mComboBox.SelectionChanged(
			[proc](const IInspectable& sender, const SelectionChangedEventArgs& selectionChangedEventArgs) {
				// Call proc
				proc(selectionChangedEventArgs.AddedItems().GetAt(0).as<ComboBoxItem>().Tag());
			});
}

//----------------------------------------------------------------------------------------------------------------------
void ComboBoxHelper::selectedIntTagChanged(std::function<void(int tag)> proc) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup SelectionChanged
	mInternals->mComboBox.SelectionChanged(
			[proc](const IInspectable& sender, const SelectionChangedEventArgs& selectionChangedEventArgs) {
				// Setup
				auto	addedItems = selectionChangedEventArgs.AddedItems();
				if (addedItems.Size() > 0) {
					// Get info
					auto	tag = addedItems.GetAt(0).as<ComboBoxItem>().Tag();
					auto	intValue = tag.try_as<int>();
					auto	value = intValue ? *intValue : std::stoi(std::basic_string<TCHAR>(tag.as<winrt::hstring>()));

					// Call proc
					proc(value);
				}
			});
}
