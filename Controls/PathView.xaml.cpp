//----------------------------------------------------------------------------------------------------------------------
//	PathView.xaml.cpp			�2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "PathView.xaml.h"

#include "ButtonHelper.h"
#include "FontIconHelper.h"
#include "StackPanelHelper.h"
#include "TextBlockHelper.h"

#include "winrt\Windows.UI.Xaml.Interop.h"

#include "WinUIToolbox.PathView.g.cpp"

#include <windows.h>
#include <shellapi.h>

#include <cmath>
#include <limits>
#include <vector>

#pragma comment(lib, "shell32.lib")

using namespace winrt::WinUIToolbox::implementation;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;

using Rect = winrt::Windows::Foundation::Rect;

//----------------------------------------------------------------------------------------------------------------------
// MARK: PathView::Internals

class PathView::Internals {
	// SegmentInfo
	public:
		struct SegmentInfo {
			// Properties
			std::wstring	mCumulativePath;

			Button			mButton = nullptr;
			TextBlock		mLabel = nullptr;
			TextBlock		mEllipsis = nullptr;

			double			mFullWidth = 0.0;
			double			mCollapsedWidth = 0.0;
		};

	// Methods
	public:
						Internals(PathView& pathView) : mPathView(pathView), mHoveredIndex(-1) {}

				void	rebuildSegments()
							{
								// Reset
								mPathView.Children().Clear();
								mSegments.clear();
								mHoveredIndex = -1;

								// Setup
								std::wstring	path(mPathView.Path().c_str());
								size_t			pathLength = path.size();

								// Iterate path, extracting each segment (with cumulative path from the absolute offset)
								size_t	i = 0;
								while (i < pathLength) {
									// Skip separators
									while ((i < pathLength) && ((path[i] == L'\\') || (path[i] == L'/')))
										// Skipping path separator
										i++;
									if (i >= pathLength)
										break;

									// Collect segment name
									size_t	segmentStart = i;
									while ((i < pathLength) && (path[i] != L'\\') && (path[i] != L'/'))
										// One more character
										i++;

									// Setup segment strings
									std::wstring	name = path.substr(segmentStart, i - segmentStart);
									std::wstring	cumulativePath = path.substr(0, i);
									if (!cumulativePath.empty() && (cumulativePath.back() == L':'))
										// Drive root (e.g. "C:" => "C:\")
										cumulativePath += L'\\';

									// Setup segment content
									StackPanelHelper	stackPanelHelper;
									stackPanelHelper
											.setOrientation(Orientation::Horizontal)
											.setVerticalAlignment(VerticalAlignment::Center)
											.setSpacing(2.0);

									// Separator (chevron) for all but the first segment
									int	segmentsCount = (int) mSegments.size();
									if (segmentsCount > 0) {
										// ChevronRight glyph
										FontIcon	fontIcon =
															FontIconHelper(L"\uE76C", 10.0)
																		.setVerticalAlignment(VerticalAlignment::Center)
																		.getFontIcon();
										stackPanelHelper.add(fontIcon);
									}

									// Label
									TextBlock	label =
														TextBlockHelper()
																.setText(name)
																.setVerticalAlignment(VerticalAlignment::Center)
																.getTextBlock();
									stackPanelHelper.add(label);

									// Ellipsis (shown only when collapsed)
									TextBlock	ellipsis =
														TextBlockHelper()
																.setText(L"\u2026")
																.setVerticalAlignment(VerticalAlignment::Center)
																.setVisibility(Visibility::Collapsed)
																.getTextBlock();
									stackPanelHelper.add(ellipsis);

									// Button
									Button	button =
													ButtonHelper()
															.setBackground(winrt::Windows::UI::Colors::Transparent())
															.setBorderThickness(0.0)
															.setPadding(4.0, 2.0, 4.0, 2.0)
															.setMinWidth(0.0)
															.setMinHeight(0.0)
															.setVerticalAlignment(VerticalAlignment::Center)
															.setContent(stackPanelHelper.getStackPanel())
															.setClickedProc([cumulativePath]() {
																// Reveal in File Explorer
																::ShellExecuteW(nullptr, L"explore",
																		cumulativePath.c_str(), nullptr, nullptr,
																		SW_SHOW);
															})
															.setPointerEnteredProc([this, segmentsCount]() {
																// Update hovered index
																mHoveredIndex = segmentsCount;
																mPathView.InvalidateMeasure();
															})
															.setPointerExitedProc([this, segmentsCount]() {
																// Clear hovered index
																if (mHoveredIndex == segmentsCount)
																	mHoveredIndex = -1;
																mPathView.InvalidateMeasure();
															})
															.getButton();
									mPathView.Children().Append(button);

									// Store segment info
									SegmentInfo	segmentInfo;
									segmentInfo.mCumulativePath = cumulativePath;
									segmentInfo.mButton = button;
									segmentInfo.mLabel = label;
									segmentInfo.mEllipsis = ellipsis;
									mSegments.push_back(segmentInfo);
								}

								// Request layout
								mPathView.InvalidateMeasure();
							}

		static	void	setSegmentCollapsed(const SegmentInfo& segmentInfo, bool collapsed)
							{
								// We show either label or ellipsis
								segmentInfo.mLabel.Visibility(
										collapsed ? Visibility::Collapsed : Visibility::Visible);
								segmentInfo.mEllipsis.Visibility(
										collapsed ? Visibility::Visible : Visibility::Collapsed);
							}

	// Properties
	PathView&					mPathView;
	std::vector<SegmentInfo>	mSegments;
	int							mHoveredIndex;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - PathView

// MARK: Properties

DependencyProperty	PathView::mPathProperty =
							DependencyProperty::Register(L"Path", winrt::xaml_typename<winrt::hstring>(),
									winrt::xaml_typename<winrt::WinUIToolbox::PathView>(),
									PropertyMetadata{winrt::box_value(winrt::hstring(L""))});

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
PathView::PathView() : PathViewT<PathView>(), mInternals(new Internals(*this))
//----------------------------------------------------------------------------------------------------------------------
{}

//----------------------------------------------------------------------------------------------------------------------
PathView::~PathView()
//----------------------------------------------------------------------------------------------------------------------
{
	delete mInternals;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
winrt::hstring PathView::Path() const
//----------------------------------------------------------------------------------------------------------------------
{
	return winrt::unbox_value<winrt::hstring>(GetValue(PathProperty()));
}

//----------------------------------------------------------------------------------------------------------------------
void PathView::Path(const winrt::hstring& path)
//----------------------------------------------------------------------------------------------------------------------
{
	// Store
	SetValue(PathProperty(), winrt::box_value(path));

	// Rebuild segments
	mInternals->rebuildSegments();
}

// MARK: Control methods

//----------------------------------------------------------------------------------------------------------------------
Size PathView::MeasureOverride(const Size& availableSize)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	size_t	segmentsCount = mInternals->mSegments.size();
	if (segmentsCount == 0)
		return Size{0.0f, 0.0f};

	float	infinity = std::numeric_limits<float>::infinity();
	Size	measureSize{infinity, availableSize.Height};

	// Measure each segment in both states to gather widths
	double	sumFullWidth = 0.0;
	for (Internals::SegmentInfo& segmentInfo : mInternals->mSegments) {
		// Full
		Internals::setSegmentCollapsed(segmentInfo, false);
		segmentInfo.mButton.Measure(measureSize);
		segmentInfo.mFullWidth = segmentInfo.mButton.DesiredSize().Width;

		// Collapsed
		Internals::setSegmentCollapsed(segmentInfo, true);
		segmentInfo.mButton.Measure(measureSize);
		segmentInfo.mCollapsedWidth = segmentInfo.mButton.DesiredSize().Width;

		// Update sum
		sumFullWidth += segmentInfo.mFullWidth;
	}

	// Decide which segments are shown full
	std::vector<bool>	isFull(segmentsCount, false);
	if (std::isinf(availableSize.Width) || (sumFullWidth <= (double) availableSize.Width))
		// Everything fits
		for (size_t i = 0; i < segmentsCount; isFull[i++] = true) ;
	else {
		// Not everything fits.  Last segment is always full
		isFull[segmentsCount - 1] = true;

		// Any hovered segment is also full
		if ((mInternals->mHoveredIndex >= 0) && (mInternals->mHoveredIndex < (int) segmentsCount))
			// Note is full
			isFull[mInternals->mHoveredIndex] = true;

		// Compute width used by the forced-full segments and everything else collapsed
		double	usedWidth = 0.0;
		for (size_t i = 0; i < segmentsCount; i++)
			// Add either full or collapsed width based on current state
			usedWidth += isFull[i] ? mInternals->mSegments[i].mFullWidth : mInternals->mSegments[i].mCollapsedWidth;

		// Promote segments to full, from the tail toward the head, while they fit
		for (int i = (int) segmentsCount - 2; i >= 0; i--) {
			// Check if already full
			if (isFull[i])
				continue;

			// Check if promoting fits
			double	deltaWidth = mInternals->mSegments[i].mFullWidth - mInternals->mSegments[i].mCollapsedWidth;
			if ((usedWidth + deltaWidth) <= (double) availableSize.Width) {
				// Promote
				isFull[i] = true;
				usedWidth += deltaWidth;
			}
		}
	}

	// Apply final states, measure, and compute desired size
	double	totalWidth = 0.0;
	float	maxHeight = 0.0f;
	for (size_t i = 0; i < segmentsCount; i++) {
		// Apply state
		Internals::SegmentInfo&	segmentInfo = mInternals->mSegments[i];
		Internals::setSegmentCollapsed(segmentInfo, !isFull[i]);
		segmentInfo.mButton.Measure(measureSize);

		// Update totals
		Size	desiredSize = segmentInfo.mButton.DesiredSize();
		totalWidth += desiredSize.Width;
		maxHeight = std::max(maxHeight, desiredSize.Height);
	}

	// Compose desired width
	double	desiredWidth =
					std::isinf(availableSize.Width) ?
							totalWidth : std::min(totalWidth, (double) availableSize.Width);

	return Size{(float) desiredWidth, maxHeight};
}

//----------------------------------------------------------------------------------------------------------------------
Size PathView::ArrangeOverride(const Size& finalSize)
//----------------------------------------------------------------------------------------------------------------------
{
	// Lay out segments left to right at their measured widths
	double	x = 0.0;
	for (Internals::SegmentInfo& segmentInfo : mInternals->mSegments) {
		// Setup
		double	width = segmentInfo.mButton.DesiredSize().Width;

		// Arrange
		segmentInfo.mButton.Arrange(Rect{(float) x, 0.0f, (float) width, finalSize.Height});

		// Advance
		x += width;
	}

	return finalSize;
}

// MARK: Class methods

//----------------------------------------------------------------------------------------------------------------------
DependencyProperty PathView::PathProperty() noexcept
//----------------------------------------------------------------------------------------------------------------------
{
	return mPathProperty;
}
