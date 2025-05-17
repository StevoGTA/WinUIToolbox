//----------------------------------------------------------------------------------------------------------------------
//	ControlHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "FrameworkElementHelper.h"

#undef Delete

#include "winrt\Microsoft.UI.Xaml.Controls.h"

#define Delete(x)	{ delete x; x = nil; }

#include <functional>

using DependencyPropertyChangedEventArgs = winrt::Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ControlHelper

template <typename C, typename H> class ControlHelper : public FrameworkElementHelper<C, H> {
	// Methods
	public:
			// Lifecycle methods
			ControlHelper(C c) : FrameworkElementHelper(c) {}
 
			// Instance methods
		H&	setEnabled(bool enabled)
				{ getControl().IsEnabled(enabled); return (H&) *this; }
		H&	setEnabledChanged(std::function<void(bool isEnabled)> enabledChangedProc)
				{
					// Set
					getControl().IsEnabledChanged(
							[this](const IInspectable& sender, const DependencyPropertyChangedEventArgs& args){
								// Call proc
								enabledChangedProc(sender.as<C>().IsEnabled());
							});

					return (H&) *this;
				}

		C	getControl() const
				{ return getFrameworkElement(); }
};
