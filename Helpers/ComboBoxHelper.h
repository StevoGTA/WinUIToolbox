//----------------------------------------------------------------------------------------------------------------------
//	ComboBoxHelper.h			�2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ControlHelper.h"

#include <WinError.h>

#include "winrt\Microsoft.UI.Xaml.h"
#include "winrt\Microsoft.UI.Xaml.Controls.h"
#include "winrt\Windows.Foundation.h"

#include <functional>

using namespace winrt;
using namespace winrt::Microsoft::UI::Xaml::Controls;

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

	// TComboBoxItem
	public:
		template <typename T> struct TComboBoxItem : public implements<T, IPropertyValue> {
			// Methods
			public:
										// Lifecycle methods
										TComboBoxItem<T>() : implements<T, IPropertyValue>() {}

										// IPropertyValue methods
						PropertyType	Type()
											{ return PropertyType::String; }
						bool			IsNumericScalar()
											{ return false; }
						uint8_t			GetUInt8()
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						int16_t			GetInt16()
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						uint16_t		GetUInt16()
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						int32_t			GetInt32()
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						uint32_t		GetUInt32()
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						int64_t			GetInt64()
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						uint64_t		GetUInt64()
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						float			GetSingle()
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						double			GetDouble()
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						char16_t		GetChar16()
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						bool			GetBoolean()
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						hstring			GetString()
											{ return ToString();  }
						guid			GetGuid()
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						DateTime		GetDateTime()
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						TimeSpan		GetTimeSpan()
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						Point			GetPoint()
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						Size			GetSize()
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						Rect			GetRect()
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						void			GetUInt8Array(com_array<uint8_t> &value)
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						void			GetInt16Array(com_array<int16_t> &value)
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						void			GetUInt16Array(com_array<uint16_t> &value)
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						void			GetInt32Array(com_array<int32_t> &value)
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						void			GetUInt32Array(com_array<uint32_t> &value)
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						void			GetInt64Array(com_array<int64_t> &value)
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						void			GetUInt64Array(com_array<uint64_t> &value)
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						void			GetSingleArray(com_array<float> &value)
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						void			GetDoubleArray(com_array<double> &value)
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						void			GetChar16Array(com_array<char16_t> &value)
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						void			GetBooleanArray(com_array<bool> &value)
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						void			GetStringArray(com_array<hstring> &value)
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						void			GetInspectableArray(com_array<IInspectable> &value)
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						void			GetGuidArray(com_array<guid> &value)
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						void			GetDateTimeArray(com_array<DateTime> &value)
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						void			GetTimeSpanArray(com_array<TimeSpan> &value)
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						void			GetPointArray(com_array<Point> &value)
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						void			GetSizeArray(com_array<Size> &value)
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }
						void			GetRectArray(com_array<Rect> &value)
											{ throw hresult_error(TYPE_E_TYPEMISMATCH); }

										// Subclass methods
				virtual	hstring			ToString() const = 0;
		};

	// Methods
	public:
						// Lifecycle methods
						ComboBoxHelper(ComboBox comboBox, Options options = kOptionsNone);
	
						// Instance methods
		ComboBoxHelper&	addItem(const hstring& content, const IInspectable& tag, bool isSelected = false);
		ComboBoxHelper& addItem(const hstring& content, int tag, bool isSelected = false)
							{ return addItem(content, box_value<int>(tag), isSelected); }
		ComboBoxHelper& addItem(const hstring& content, bool isSelected = false)
							{ return addItem(content, box_value<int>(0), isSelected); }
		ComboBoxHelper& addItem(const IStringable& tag, bool isSelected = false)
							{ return addItem(tag.ToString(), tag, isSelected); }
		ComboBoxHelper& addItem(const IPropertyValue& comboBoxItem, bool isSelected = false);
		ComboBoxHelper& addSectionTitle(const hstring& title);
		ComboBoxHelper& addSeparatorItem();

		IInspectable	getSelectedTag() const;
		int				getSelectedIntTag() const;

		bool			select(std::function<bool(const IInspectable& item)> itemCompareProc) const;
		bool			selectTag(std::function<bool(const IInspectable& tag)> tagCompareProc) const;
		bool			selectIntTag(int tag) const;

		ComboBoxHelper& setSelectedItemChangedProc(std::function<void(const IInspectable& item)> proc);
		ComboBoxHelper& setSelectedTagChangedProc(std::function<void(const IInspectable& tag)> proc);
		ComboBoxHelper& setSelectedIntTagChangedProc(std::function<void(int tag)> proc);

		ComboBox		getComboBox() const
							{ return getControl(); }
};
