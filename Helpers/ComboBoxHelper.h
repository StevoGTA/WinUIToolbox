//----------------------------------------------------------------------------------------------------------------------
//	ComboBoxHelper.h			©2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ControlHelper.h"

#include <WinError.h>

#include "winrt\Microsoft.UI.Xaml.h"
#include "winrt\Microsoft.UI.Xaml.Controls.h"
#include "winrt\Windows.Foundation.h"

#include <functional>

using ComboBox = winrt::Microsoft::UI::Xaml::Controls::ComboBox;
using DateTime = winrt::Windows::Foundation::DateTime;
using IInspectable = winrt::Windows::Foundation::IInspectable;
using IPropertyValue = winrt::Windows::Foundation::IPropertyValue;
using IStringable = winrt::Windows::Foundation::IStringable;
using Point = winrt::Windows::Foundation::Point;
using PropertyType = winrt::Windows::Foundation::PropertyType;
using Rect = winrt::Windows::Foundation::Rect;
using Size = winrt::Windows::Foundation::Size;
using TimeSpan = winrt::Windows::Foundation::TimeSpan;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ComboBoxHelper

class ComboBoxHelper : public ControlHelper<ComboBox, ComboBoxHelper> {
	// Options
	public:
		enum Options {
			kOptionsNone		= 0,
			kOptionsClearItems	= 1 << 0,
		};

	// Methods
	public:
						// Lifecycle methods
						ComboBoxHelper(ComboBox comboBox, Options options = kOptionsNone);
	
						// Instance methods
		ComboBoxHelper&	addItem(const winrt::hstring& displayName, const IInspectable& value, bool isSelected = false);
		ComboBoxHelper& addItem(const winrt::hstring& displayName, int value, bool isSelected = false)
							{ return addItem(displayName, winrt::box_value<int>(value), isSelected); }
		ComboBoxHelper& addItem(const winrt::hstring& displayName, std::function<void()> proc, bool isSelected = false);
		ComboBoxHelper& addSectionTitle(const winrt::hstring& title);
		ComboBoxHelper& addSeparatorItem();

		IInspectable	getSelectedValue() const;
		int				getSelectedIntValue() const;

		bool			selectValue(std::function<bool(const IInspectable& value)> valueCompareProc) const;
		bool			selectIntValue(int value) const;

		ComboBoxHelper& setSelectedValueChangedProc(std::function<void(const IInspectable& value)> proc);
		ComboBoxHelper& setSelectedIntValueChangedProc(std::function<void(int value)> proc);

		ComboBoxHelper& setDropDownOpenedProc(std::function<void()> proc);

		ComboBox		getComboBox() const
							{ return getControl(); }

						// Class methods
static	bool			getIntFromTag(const IInspectable& tag, int& outInt);
};
