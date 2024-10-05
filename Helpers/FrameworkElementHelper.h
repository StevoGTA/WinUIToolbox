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
		H&	setHorizontalAlignment(HorizontalAlignment horizontalAlignment)
				{ getFrameworkElement().HorizontalAlignment(horizontalAlignment); return (H&) *this; }

		H&	setMargin(double left, double top, double right, double bottom)
				{ getFrameworkElement().Margin(ThicknessHelper::FromLengths(left, top, right, bottom));
						return (H&) *this; }
		H&	setPadding(double left, double top, double right, double bottom)
				{ getFrameworkElement().Padding(ThicknessHelper::FromLengths(left, top, right, bottom));
						return (H&) *this; }
		H&	setMinWidth(double minWidth)
				{ getFrameworkElement().MinWidth(minWidth); return (H&) *this; }
		H&	setVerticalAlignment(VerticalAlignment verticalAlignment)
				{ getFrameworkElement().VerticalAlignment(verticalAlignment); return (H&) *this; }
		H&	setWidth(double width)
				{ getFrameworkElement().Width(width); return (H&) *this; }

		C	getFrameworkElement() const
				{ return getUIElement(); }
};
