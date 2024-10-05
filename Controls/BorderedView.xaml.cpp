//----------------------------------------------------------------------------------------------------------------------
//	BorderedView.xaml.cpp			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "BorderedView.xaml.h"

#include "winrt\Microsoft.UI.Xaml.Controls.h"
#include "winrt\Windows.Foundation.h"

#include "WinUIToolbox.BorderedView.g.cpp"

using namespace winrt::WinUIToolbox::implementation;

using Border = winrt::Microsoft::UI::Xaml::Controls::Border;
using Thickness = winrt::Microsoft::UI::Xaml::Thickness;
using ThicknessHelper = winrt::Microsoft::UI::Xaml::ThicknessHelper;

//----------------------------------------------------------------------------------------------------------------------
// MARK: BorderedView::Internals

class BorderedView::Internals {
	public:
		Internals() : mBorderThickness(ThicknessHelper::FromUniformLength(1.0)), mContent(nullptr) {}

		Thickness	mBorderThickness;
		UIElement	mContent;

		Border		mBorder;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - BorderedView

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
BorderedView::BorderedView() : BorderedViewT<BorderedView>()
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup Default Style
	DefaultStyleKey(winrt::box_value(L"WinUIToolbox.BorderedView"));

	// Setup
	mInternals = new Internals();
}

//----------------------------------------------------------------------------------------------------------------------
BorderedView::~BorderedView()
//----------------------------------------------------------------------------------------------------------------------
{
	delete mInternals;
}

// MARK: FrameworkElement methods

//----------------------------------------------------------------------------------------------------------------------
void BorderedView::OnApplyTemplate() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Do super
	__super::OnApplyTemplate();

	// Finish setup
	mInternals->mBorder = GetTemplateChild(L"mBorder").as<Border>();
	mInternals->mBorder.BorderThickness(mInternals->mBorderThickness);
	if (mInternals->mContent)
		mInternals->mBorder.Child(mInternals->mContent);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
void BorderedView::SetBorderThickness(double left, double top, double right, double bottom)
//----------------------------------------------------------------------------------------------------------------------
{
	// Store
	mInternals->mBorderThickness = ThicknessHelper::FromLengths(left, top, right, bottom);

	// Update UI (only effective after OnApplyTemplate() has been called
	mInternals->mBorder.BorderThickness(mInternals->mBorderThickness);
}

//----------------------------------------------------------------------------------------------------------------------
void BorderedView::SetContent(UIElement uiElement)
//----------------------------------------------------------------------------------------------------------------------
{
	// Store
	mInternals->mContent = uiElement;

	// Update UI (only effective after OnApplyTemplate() has been called
	mInternals->mBorder.Child(mInternals->mContent);
}
