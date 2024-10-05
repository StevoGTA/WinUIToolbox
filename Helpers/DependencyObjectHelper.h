//----------------------------------------------------------------------------------------------------------------------
//	DependencyObjectHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "winrt\Microsoft.UI.Xaml.h"

using DependencyObject = winrt::Microsoft::UI::Xaml::DependencyObject;

//----------------------------------------------------------------------------------------------------------------------
// MARK: DependencyObjectHelper

template <typename C, typename H> class DependencyObjectHelper {
	// Methods
	public:
					// Lifecycle methods
					DependencyObjectHelper(C c) : mC(c) {}
		virtual		~DependencyObjectHelper() {}

					// Instance methods
				H&	setGridPosition(int columnIndex, int rowIndex) const
						{
							// Set
							mC.SetValue(winrt::Windows::UI::Xaml::Controls::Grid::ColumnProperty(), columnIndex);
							mC.SetValue(winrt::Windows::UI::Xaml::Controls::Grid::RowProperty(), rowIndex);

							return (H&) *this;
						}

				C	getDependencyObject() const
						{ return mC; }

	// Properties
	private:
		C	mC;
};
