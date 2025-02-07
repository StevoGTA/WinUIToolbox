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
// MARK: - SComboBoxItemTag

struct SComboBoxItemTag : public implements<SComboBoxItemTag, IInspectable> {
	public:
								SComboBoxItemTag(const IInspectable& value) : mValue(value), mProc(nullptr) {}
								SComboBoxItemTag(std::function<void()> proc) : mValue(nullptr), mProc(proc) {}
		
				bool			hasValue() const
									{ return mValue != nullptr; }
		const	IInspectable&	getValue() const
									{ return mValue; }

				bool			hasProc() const
									{ return mProc != nullptr; }
				void			callProc() const
									{ mProc(); }

	private:
		IInspectable			mValue;
		std::function<void()>	mProc;
};

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
ComboBoxHelper& ComboBoxHelper::addItem(const hstring& displayName, const IInspectable& value, bool isSelected)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	ComboBoxItem	comboBoxItem;
	comboBoxItem.Content(box_value(displayName));
	comboBoxItem.Tag(make<SComboBoxItemTag>(value));

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
ComboBoxHelper& ComboBoxHelper::addItem(const hstring& displayName, std::function<void()> proc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	ComboBoxItem	comboBoxItem;
	comboBoxItem.Content(box_value(displayName));
	comboBoxItem.Tag(make<SComboBoxItemTag>(proc));

	// Add
	getComboBox().Items().Append(comboBoxItem);

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
IInspectable ComboBoxHelper::getSelectedValue() const
//----------------------------------------------------------------------------------------------------------------------
{
	return getComboBox().Items().GetAt(getComboBox().SelectedIndex()).as<ComboBoxItem>().Tag().as<SComboBoxItemTag>()->
			getValue();
}

//----------------------------------------------------------------------------------------------------------------------
int ComboBoxHelper::getSelectedIntValue() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	auto	value = getSelectedValue();
	auto	intValue = value.try_as<int>();

	return intValue ? *intValue : std::stoi(std::basic_string<TCHAR>(value.as<winrt::hstring>()));
}

//----------------------------------------------------------------------------------------------------------------------
bool ComboBoxHelper::selectValue(std::function<bool(const IInspectable& value)> valueCompareProc) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate items
	auto	items = getComboBox().Items();
	for (uint32_t i = 0; i < items.Size(); i++) {
		// Get item
		auto	item = items.GetAt(i).try_as<ComboBoxItem>();
		if (!item)
			continue;

		// Get tag
		auto	tag = item.Tag().try_as<SComboBoxItemTag>();
		if (!tag)
			// No tag
			continue;

		// Get value
		if (!tag->hasValue())
			// No value
			continue;

		// Check tag
		if (valueCompareProc(tag->getValue())) {
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
bool ComboBoxHelper::selectIntValue(int value) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate items
	auto	items = getComboBox().Items();
	for (uint32_t i = 0; i < items.Size(); i++) {
		// Get item
		auto	item = items.GetAt(i).try_as<ComboBoxItem>();
		if (!item)
			continue;

		// Get tag
		auto	tag = item.Tag().try_as<SComboBoxItemTag>();
		if (!tag)
			// No tag
			continue;

		// Get value
		if (!tag->hasValue())
			// No value
			continue;

		// Check value
		auto	intValue = tag->getValue().try_as<int>();
		auto	value_ = intValue ? *intValue : std::stoi(std::basic_string<TCHAR>(tag.as<winrt::hstring>()));
		if (value_ == value) {
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
ComboBoxHelper& ComboBoxHelper::setSelectedValueChangedProc(std::function<void(const IInspectable& value)> proc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup SelectionChanged
	getComboBox().SelectionChanged(
			[this, proc](const IInspectable& sender, const SelectionChangedEventArgs& selectionChangedEventArgs) {
				// Check if handling events
				if (sender == sComboBoxInUpdate)
					return;

				// Get tag
				auto	tag =
								selectionChangedEventArgs.AddedItems().GetAt(0).as<ComboBoxItem>().Tag()
										.as<SComboBoxItemTag>();

				// Check tag
				if (tag->hasValue())
					// Value
					proc(tag->getValue());
				else
					// Proc
					tag->callProc();
			});

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ComboBoxHelper& ComboBoxHelper::setSelectedIntValueChangedProc(std::function<void(int value)> proc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	setSelectedValueChangedProc([proc](const IInspectable& value) {
		// Get info
		auto	intValue = value.try_as<int>();
		auto	value_ = intValue ? *intValue : std::stoi(std::basic_string<TCHAR>(value.as<winrt::hstring>()));

		// Call proc
		proc(value_);
	});

	return *this;
}
