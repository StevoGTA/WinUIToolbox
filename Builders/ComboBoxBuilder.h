//----------------------------------------------------------------------------------------------------------------------
//	ComboBoxBuilder.h			©2023 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include <WinError.h>

#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Windows.Foundation.h>

using namespace winrt;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Windows::Foundation;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ComboBoxBuilder

class ComboBoxBuilder {
	// Options
	public:
		enum Options {
			kOptionsNone	= 0,
		};

	// Classes
	private:
		class Internals;

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
				ComboBoxBuilder(ComboBox comboBox, Options options = kOptionsNone);

				// Instance methods
		void	add(const hstring& content, const IInspectable& tag, bool isSelected = false);
		void	add(const IStringable& tag, bool isSelected = false)
					{ add(tag.ToString(), tag, isSelected); }
		void	add(const IPropertyValue& comboBoxItem, bool isSelected = false);
		void	addSeparator();

	// Properties
	private:
		Internals*	mInternals;
};
