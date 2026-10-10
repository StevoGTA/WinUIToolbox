//----------------------------------------------------------------------------------------------------------------------
//	ChipsView.xaml.cpp			©2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "ChipsView.xaml.h"

#include "winrt\Microsoft.UI.Xaml.Controls.h"
#include "winrt\Microsoft.UI.Xaml.Media.h"

#include <cmath>

#include "WinUIToolbox.ChipInfo.g.cpp"
#include "WinUIToolbox.ChipsView.g.cpp"

using namespace winrt::WinUIToolbox::implementation;

using Application = winrt::Microsoft::UI::Xaml::Application;
using Border = winrt::Microsoft::UI::Xaml::Controls::Border;
using Color = winrt::Windows::UI::Color;
using CornerRadiusHelper = winrt::Microsoft::UI::Xaml::CornerRadiusHelper;
using FontIcon = winrt::Microsoft::UI::Xaml::Controls::FontIcon;
using FrameworkElement = winrt::Microsoft::UI::Xaml::FrameworkElement;
using IInspectable = winrt::Windows::Foundation::IInspectable;
using Orientation = winrt::Microsoft::UI::Xaml::Controls::Orientation;
using SizeChangedEventArgs = winrt::Microsoft::UI::Xaml::SizeChangedEventArgs;
using SolidColorBrush = winrt::Microsoft::UI::Xaml::Media::SolidColorBrush;
using StackPanel = winrt::Microsoft::UI::Xaml::Controls::StackPanel;
using TextBlock = winrt::Microsoft::UI::Xaml::Controls::TextBlock;
using ThicknessHelper = winrt::Microsoft::UI::Xaml::ThicknessHelper;
using VerticalAlignment = winrt::Microsoft::UI::Xaml::VerticalAlignment;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ChipsView::Internals

class ChipsView::Internals {
	public:
						Internals() :
							mInfos(winrt::single_threaded_vector<winrt::WinUIToolbox::ChipInfo>().GetView())
							{}

				void	composeChips()
							{
								// Remove existing chips
								mStackPanel.Children().Clear();

								// Iterate infos
								for (const auto& info : mInfos) {
									// Setup
									ChipInfoStyle	style = info.Style();
									Color			tintColor =
															info.TintColor() ?
																	info.TintColor().Value() :
																	colorFor(
																			(style == ChipInfoStyle::Accented) ?
																					L"AccentTextFillColorPrimaryBrush" :
																					L"TextFillColorPrimaryBrush");

									// Compose content
									TextBlock	textBlock;
									textBlock.Text(info.Text());
									textBlock.FontSize(12.0);
									textBlock.Foreground(SolidColorBrush(tintColor));
									textBlock.VerticalAlignment(VerticalAlignment::Center);

									StackPanel	stackPanel;
									stackPanel.Orientation(Orientation::Horizontal);
									stackPanel.Spacing(3.0);
									stackPanel.Children().Append(textBlock);

									if (info.Symbol() == ChipInfoSymbol::Locked) {
										// Add lock
										FontIcon	fontIcon;
										fontIcon.Glyph(L"\xE72E");
										fontIcon.FontSize(10.0);
										fontIcon.Foreground(SolidColorBrush(colorFor(L"TextFillColorSecondaryBrush")));
										fontIcon.VerticalAlignment(VerticalAlignment::Center);
										stackPanel.Children().Append(fontIcon);
									}

									// Compose chip
									Border	border;
									border.Padding(ThicknessHelper::FromLengths(6.0, 2.0, 6.0, 2.0));
									border.BorderThickness(ThicknessHelper::FromUniformLength(1.0));
									border.BorderBrush(
											SolidColorBrush(
													withAlpha(tintColor,
															(style == ChipInfoStyle::Accented) ? 1.0 : 0.25)));
									border.Background(
											SolidColorBrush(
													(style == ChipInfoStyle::Outlined) ?
															Color{} :
															withAlpha(tintColor,
																	(style == ChipInfoStyle::Accented) ? 0.15 : 0.1)));
									border.Child(stackPanel);
									border.SizeChanged(
											[](const IInspectable& sender, const SizeChangedEventArgs& args){
												// Round the ends
												sender.as<Border>().CornerRadius(
														CornerRadiusHelper::FromUniformRadius(
																args.NewSize().Height / 2.0));
											});

									mStackPanel.Children().Append(border);
								}
							}

		static	Color	colorFor(const wchar_t* key)
							{
								// Look up the theme brush
								IInspectable	object =
														Application::Current().Resources().TryLookup(
																winrt::box_value(winrt::hstring(key)));
								SolidColorBrush	solidColorBrush =
														(object != nullptr) ?
																object.try_as<SolidColorBrush>() : nullptr;

								return (solidColorBrush != nullptr) ?
										solidColorBrush.Color() : Color{ 0xFF, 0x80, 0x80, 0x80 };
							}
		static	Color	withAlpha(Color color, double alpha)
							{
								// Scale alpha
								color.A = (uint8_t) std::lround(color.A * alpha);

								return color;
							}

		ChipInfoVectorView	mInfos;
		StackPanel			mStackPanel;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - ChipsView

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
ChipsView::ChipsView() : ChipsViewT<ChipsView>()
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals();

	mInternals->mStackPanel.Orientation(Orientation::Horizontal);
	mInternals->mStackPanel.Spacing(4.0);

	IsTabStop(false);
	Content(mInternals->mStackPanel);

	// Track theme changes
	ActualThemeChanged([this](const FrameworkElement& sender, const IInspectable& args){
		// Compose chips
		mInternals->composeChips();
	});
}

//----------------------------------------------------------------------------------------------------------------------
ChipsView::~ChipsView()
//----------------------------------------------------------------------------------------------------------------------
{
	delete mInternals;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
ChipInfoVectorView ChipsView::Infos() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mInfos;
}

//----------------------------------------------------------------------------------------------------------------------
void ChipsView::Infos(const ChipInfoVectorView& infos)
//----------------------------------------------------------------------------------------------------------------------
{
	// Store
	mInternals->mInfos = infos;

	// Compose chips
	mInternals->composeChips();
}
