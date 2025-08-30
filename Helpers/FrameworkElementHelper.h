//----------------------------------------------------------------------------------------------------------------------
//	FrameworkElementHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "UIElementHelper.h"

using FrameworkElement = winrt::Microsoft::UI::Xaml::FrameworkElement;
using HorizontalAlignment = winrt::Microsoft::UI::Xaml::HorizontalAlignment;
using VerticalAlignment = winrt::Microsoft::UI::Xaml::VerticalAlignment;
using ThicknessHelper = winrt::Microsoft::UI::Xaml::ThicknessHelper;

//----------------------------------------------------------------------------------------------------------------------
// MARK: FrameworkElementHelper

template <typename C, typename H> class FrameworkElementHelper : public UIElementHelper<C, H> {
	// Methods
	public:
						// Lifecycle methods
						FrameworkElementHelper(C c) : UIElementHelper(c) {}
 
						// Instance methods
				H&		setHorizontalAlignment(HorizontalAlignment horizontalAlignment)
							{ getFrameworkElement().HorizontalAlignment(horizontalAlignment); return (H&) *this; }

				H&		setMargin(double left, double top, double right, double bottom)
							{ getFrameworkElement().Margin(ThicknessHelper::FromLengths(left, top, right, bottom));
									return (H&) *this; }

				H&		setPadding(double left, double top, double right, double bottom)
							{ getFrameworkElement().Padding(ThicknessHelper::FromLengths(left, top, right, bottom));
									return (H&) *this; }
				H&		setPadding(double uniformLength)
							{ getFrameworkElement().Padding(ThicknessHelper::FromUniformLength(uniformLength));
									return (H&) *this; }

				H&		setMinWidth(double minWidth)
							{ getFrameworkElement().MinWidth(minWidth); return (H&) *this; }
				H&		setWidth(double width)
							{ getFrameworkElement().Width(width); return (H&) *this; }

				H&		setHeight(double height)
							{ getFrameworkElement().Height(height); return (H&) *this; }

				H&		setSize(double width, double height)
							{ getFrameworkElement().Width(width); getFrameworkElement().Height(height);
									return (H&) *this; }

				bool	getTagAsInt(int& outValue)
							{ return getTagAsInt(getFrameworkElement().Tag(), outValue); }
				H&		setTag(int value)
							{ getFrameworkElement().Tag(box_value(value)); return (H&) *this; }

				H&		setVerticalAlignment(VerticalAlignment verticalAlignment)
							{ getFrameworkElement().VerticalAlignment(verticalAlignment); return (H&) *this; }

				C		getFrameworkElement() const
							{ return getUIElement(); }

						// Class methods
		static	bool	getTagAsInt(const IInspectable& tag, int& outValue)
							{
								// Try as int
								auto	tagAsInt = tag.try_as<int>();
								if (tagAsInt) {
									// Int
									outValue = *tagAsInt;

									return true;
								}

								// Try as hstring
								auto	tagAsString = tag.try_as<winrt::hstring>();
								if (tagAsString) {
									// String
									outValue = std::stoi(std::basic_string<TCHAR>(*tagAsString));

									return true;
								}

								return false;
							}
		static	bool	getTagAsInt(const IInspectable& tag, int& outValue, int defaultValue)
							{
								// Try to get int
								if (getTagAsInt(tag, outValue))
									// Success
									return true;
								else {
									// Failed.  Use default
									outValue = defaultValue;

									return false;
								}
							}
};
