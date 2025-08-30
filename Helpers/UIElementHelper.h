//----------------------------------------------------------------------------------------------------------------------
//	UIElementHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "DependencyObjectHelper.h"

using UIElement = winrt::Microsoft::UI::Xaml::UIElement;
using Visibility = winrt::Microsoft::UI::Xaml::Visibility;

//----------------------------------------------------------------------------------------------------------------------
// MARK: UIElementHelper

template <typename C, typename H> class UIElementHelper : public DependencyObjectHelper<C, H> {
	// Methods
	public:
					// Lifecycle methods
					UIElementHelper(C c) : DependencyObjectHelper(c) {}
		virtual		~UIElementHelper() {}
 
					// Instance methods
				H&	setVisibility(Visibility visibility)
						{ getUIElement().Visibility(visibility); return (H&) *this; }

				C	getUIElement() const
						{ return getDependencyObject(); }
};
