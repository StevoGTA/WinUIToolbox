//----------------------------------------------------------------------------------------------------------------------
//	BorderedView.xaml.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "BorderedView.xaml.h"

#include "winrt\Microsoft.UI.Xaml.Controls.h"
#include "winrt\Windows.Foundation.h"

#include "WinUIToolbox.BorderedView.g.cpp"

using namespace winrt::WinUIToolbox::implementation;

using Border = winrt::Microsoft::UI::Xaml::Controls::Border;
using RoutedEventArgs = winrt::Microsoft::UI::Xaml::RoutedEventArgs;
using Thickness = winrt::Microsoft::UI::Xaml::Thickness;
using ThicknessHelper = winrt::Microsoft::UI::Xaml::ThicknessHelper;

//----------------------------------------------------------------------------------------------------------------------
// MARK: BorderedView::Internals

class BorderedView::Internals {
	public:
		Internals() :
			mBorderThickness(ThicknessHelper::FromUniformLength(1.0)), mContent(nullptr)
				{}

		Thickness	mBorderThickness;
		UIElement	mContent;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - BorderedView

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
BorderedView::BorderedView() : BorderedViewT<BorderedView>()
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals();

	// Wait for loaded
	Loaded([this](const IInspectable& sender, const RoutedEventArgs& routedEventArgs){
		// Setup UI
		mBorder().BorderThickness(mInternals->mBorderThickness);
		if (mInternals->mContent)
			mBorder().Child(mInternals->mContent);
	});
}

//----------------------------------------------------------------------------------------------------------------------
BorderedView::~BorderedView()
//----------------------------------------------------------------------------------------------------------------------
{
	delete mInternals;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
void BorderedView::SetBorderThickness(double left, double top, double right, double bottom)
//----------------------------------------------------------------------------------------------------------------------
{
	// Store
	mInternals->mBorderThickness = ThicknessHelper::FromLengths(left, top, right, bottom);

	// Update UI
	if (mBorder())
		// Update BorderThickness
		mBorder().BorderThickness(ThicknessHelper::FromLengths(left, top, right, bottom));
}

//----------------------------------------------------------------------------------------------------------------------
void BorderedView::SetContent(UIElement uiElement)
//----------------------------------------------------------------------------------------------------------------------
{
	// Store
	mInternals->mContent = uiElement;

	// Update UI
	if (mBorder())
		// Update Child
		mBorder().Child(uiElement);
}
