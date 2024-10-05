//----------------------------------------------------------------------------------------------------------------------
//	ControlHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "FrameworkElementHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: ControlHelper

template <typename C, typename H> class ControlHelper : public FrameworkElementHelper<C, H> {
	// Methods
	public:
			// Lifecycle methods
			ControlHelper(C c) : FrameworkElementHelper(c) {}
 
			// Instance methods
		H& setEnabled(bool enabled)
				{ getControl().IsEnabled(enabled); return (H&) *this; }

		C	getControl() const
				{ return getFrameworkElement(); }
};
