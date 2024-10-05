//----------------------------------------------------------------------------------------------------------------------
//	ComboBoxHelper.cpp			©2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "ComboBoxHelper.h"

#include "winrt\Windows.Foundation.Collections.h"
#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"

using namespace winrt::Microsoft::UI::Xaml;

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	ComboBox	sComboBoxInUpdate(nullptr);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - ComboBoxHelper

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
ComboBoxHelper::ComboBoxHelper(ComboBox comboBox, Options options) : ControlHelper(comboBox)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup UI
	if (options & kOptionsClearItems) {
		// Clear items
		sComboBoxInUpdate = getComboBox();
		getComboBox().Items().Clear();
		sComboBoxInUpdate = nullptr;
	}
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
ComboBoxHelper& ComboBoxHelper::addItem(const hstring& content, const IInspectable& tag, bool isSelected)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	ComboBoxItem	comboBoxItem;
	comboBoxItem.Content(box_value(content));
	comboBoxItem.Tag(tag);

	// Add
	getComboBox().Items().Append(comboBoxItem);

	// Check selected
	if (isSelected) {
		// Select
		sComboBoxInUpdate = getComboBox();
		getComboBox().SelectedIndex(getComboBox().Items().Size() - 1);
		sComboBoxInUpdate = nullptr;
	}

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ComboBoxHelper& ComboBoxHelper::addItem(const IPropertyValue& comboBoxItem, bool isSelected)
//----------------------------------------------------------------------------------------------------------------------
{
	// Add
	getComboBox().Items().Append(comboBoxItem);

	// Check selected
	if (isSelected) {
		// Select
		sComboBoxInUpdate = getComboBox();
		getComboBox().SelectedIndex(getComboBox().Items().Size() - 1);
		sComboBoxInUpdate = nullptr;
	}

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ComboBoxHelper& ComboBoxHelper::addSectionTitle(const hstring& title)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	ComboBoxItem	comboBoxItem;
	comboBoxItem.Content(box_value(title));
	comboBoxItem.IsEnabled(false);

	// Add
	getComboBox().Items().Append(comboBoxItem);

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ComboBoxHelper& ComboBoxHelper::addSeparatorItem()
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	ComboBoxItem	comboBoxItem;
	comboBoxItem.Content(NavigationViewItemSeparator());
	comboBoxItem.IsEnabled(false);

	// Add
	getComboBox().Items().Append(comboBoxItem);

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
IInspectable ComboBoxHelper::getSelectedTag() const
//----------------------------------------------------------------------------------------------------------------------
{
	return getComboBox().Items().GetAt(getComboBox().SelectedIndex()).as<ComboBoxItem>().Tag();
}

//----------------------------------------------------------------------------------------------------------------------
int ComboBoxHelper::getSelectedIntTag() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	auto	tag = getSelectedTag();
	auto	intValue = tag.try_as<int>();

	return intValue ? *intValue : std::stoi(std::basic_string<TCHAR>(tag.as<winrt::hstring>()));
}

//----------------------------------------------------------------------------------------------------------------------
bool ComboBoxHelper::select(std::function<bool(const IInspectable& item)> itemCompareProc) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate items
	auto	items = getComboBox().Items();
	for (uint32_t i = 0; i < items.Size(); i++) {
		// Check ComboBoxItem
		if (itemCompareProc(items.GetAt(i))) {
			// Found item
			sComboBoxInUpdate = getComboBox();
			getComboBox().SelectedIndex(i);
			sComboBoxInUpdate = nullptr;

			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool ComboBoxHelper::selectTag(std::function<bool(const IInspectable& tag)> tagCompareProc) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate items
	auto	items = getComboBox().Items();
	for (uint32_t i = 0; i < items.Size(); i++) {
		// Check tag
		if (tagCompareProc(items.GetAt(i).as<ComboBoxItem>().Tag().as<IInspectable>())) {
			// Found item
			sComboBoxInUpdate = getComboBox();
			getComboBox().SelectedIndex(i);
			sComboBoxInUpdate = nullptr;

			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool ComboBoxHelper::selectIntTag(int tag) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate items
	auto	items = getComboBox().Items();
	for (uint32_t i = 0; i < items.Size(); i++) {
		// Check tag
		auto	item = items.GetAt(i).try_as<ComboBoxItem>();
		if (!item)
			continue;

		auto	tag_ = item.Tag();
		auto	intValue = tag_.try_as<int>();
		auto	value = intValue ? *intValue : std::stoi(std::basic_string<TCHAR>(tag_.as<winrt::hstring>()));
		if (value == tag) {
			// Found item
			sComboBoxInUpdate = getComboBox();
			getComboBox().SelectedIndex(i);
			sComboBoxInUpdate = nullptr;

			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------------------------------------------------
ComboBoxHelper& ComboBoxHelper::setSelectedItemChangedProc(std::function<void(const IInspectable& item)> proc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup SelectionChanged
	getComboBox().SelectionChanged(
			[this, proc](const IInspectable& sender, const SelectionChangedEventArgs& selectionChangedEventArgs) {
				// Check if handling events
				if (sender == sComboBoxInUpdate)
					return;

				// Setup
				auto	addedItems = selectionChangedEventArgs.AddedItems();

				// Call proc
				proc(addedItems.GetAt(0));
			});

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ComboBoxHelper& ComboBoxHelper::setSelectedTagChangedProc(std::function<void(const IInspectable& tag)> proc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	setSelectedItemChangedProc([proc](const IInspectable& item){ proc(item.as<ComboBoxItem>().Tag()); });

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ComboBoxHelper& ComboBoxHelper::setSelectedIntTagChangedProc(std::function<void(int tag)> proc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	setSelectedTagChangedProc([proc](const IInspectable& tag) {
		// Get info
		auto	intValue = tag.try_as<int>();
		auto	value = intValue ? *intValue : std::stoi(std::basic_string<TCHAR>(tag.as<winrt::hstring>()));

		// Call proc
		proc(value);
	});

	return *this;
}
