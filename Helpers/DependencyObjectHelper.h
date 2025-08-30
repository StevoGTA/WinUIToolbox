//----------------------------------------------------------------------------------------------------------------------
//	DependencyObjectHelper.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "winrt\Microsoft.UI.Xaml.h"

using DependencyObject = winrt::Microsoft::UI::Xaml::DependencyObject;
using DependencyProperty = winrt::Microsoft::UI::Xaml::DependencyProperty;
using Grid = winrt::Microsoft::UI::Xaml::Controls::Grid;
using IInspectable = winrt::Windows::Foundation::IInspectable;

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
							mC.SetValue(Grid::ColumnProperty(), columnIndex);
							mC.SetValue(Grid::RowProperty(), rowIndex);

							return (H&) *this;
						}

				H&	setValue(const DependencyProperty& dependencyProperty, const IInspectable& value) const
						{ mC.SetValue(dependencyProperty, value); return (H&) *this; }

				C	getDependencyObject() const
						{ return mC; }

	// Properties
	private:
		C	mC;
};
