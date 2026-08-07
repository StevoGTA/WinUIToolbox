//----------------------------------------------------------------------------------------------------------------------
//	PathView.xaml.cpp			©2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "PathView.xaml.h"

#include "ButtonHelper.h"
#include "FontIconHelper.h"
#include "StackPanelHelper.h"
#include "TextBlockHelper.h"

#include "winrt\Windows.UI.Xaml.Interop.h"

#include <cmath>
#include <limits>
#include <shellapi.h>
#include <vector>
#include <windows.h>

#include "WinUIToolbox.PathView.g.cpp"

#pragma comment(lib, "shell32.lib")

using namespace winrt::WinUIToolbox::implementation;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;

using Application = winrt::Microsoft::UI::Xaml::Application;
using Brush = winrt::Microsoft::UI::Xaml::Media::Brush;
using Rect = winrt::Windows::Foundation::Rect;

//----------------------------------------------------------------------------------------------------------------------
// MARK: PathView::Internals

class PathView::Internals {
	// ComponentInfo
	public:
		struct ComponentInfo {
			// Properties
			std::wstring	mName;
			std::wstring	mCumulativePath;
		};

		using ComponentInfos = std::vector<ComponentInfo>;

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
								Internals(PathView& pathView) :
									mPathView(pathView), mPathStyle(PathViewStyle::Standard),
											mShowsTrailingSeparator(false),
											mHoveredIndex(-1), mTrailingFontIcon(nullptr)
									{}

				void			rebuildSegments()
									{
										// Reset
										mPathView.Children().Clear();
										mSegments.clear();
										mHoveredIndex = -1;
										mTrailingFontIcon = nullptr;

										// Break the path and the root into their components
										ComponentInfos	componentInfos =
																componentInfosFor(
																		std::wstring(mPathView.Path().c_str()));
										ComponentInfos	rootComponentInfos = componentInfosFor(mRootPath);

										// See how much of the head is the root being shown from, comparing names
										//	without regard to case, as the filesystem does
										size_t	matchCount = 0;
										while ((matchCount < rootComponentInfos.size()) &&
												(matchCount < componentInfos.size()) &&
												(::CompareStringOrdinal(componentInfos[matchCount].mName.c_str(), -1,
														rootComponentInfos[matchCount].mName.c_str(), -1, TRUE) ==
																CSTR_EQUAL))
											// One more component of the root matched
											matchCount++;

										// Anything short of the whole root is not this path's root, so show all of it
										size_t	skipCount = (matchCount == rootComponentInfos.size()) ? matchCount : 0;

										// Compose a segment for each component being shown
										for (size_t i = skipCount; i < componentInfos.size(); i++)
											// Add segment
											addSegment(componentInfos[i], i > skipCount);

										// Check if ending with a separator
										if (mShowsTrailingSeparator && !mSegments.empty()) {
											// Add trailing chevron
											mTrailingFontIcon = chevronFontIcon();
											mPathView.Children().Append(mTrailingFontIcon);
										}

										// Request layout
										mPathView.InvalidateMeasure();
									}

				void			addSegment(const ComponentInfo& componentInfo, bool showsChevron)
									{
										// Setup
										std::wstring		cumulativePath = componentInfo.mCumulativePath;
										int					segmentIndex = (int) mSegments.size();

										StackPanelHelper	stackPanelHelper;
										stackPanelHelper
												.setOrientation(Orientation::Horizontal)
												.setVerticalAlignment(VerticalAlignment::Center)
												.setSpacing(2.0);

										// Separator, for all but the first shown segment
										if (showsChevron)
											// Add chevron
											stackPanelHelper.add(chevronFontIcon());

										// Label
										TextBlock	label =
															TextBlockHelper()
																	.setText(componentInfo.mName)
																	.setForeground(segmentBrush())
																	.setVerticalAlignment(VerticalAlignment::Center)
																	.getTextBlock();
										stackPanelHelper.add(label);

										// Ellipsis (shown only when collapsed)
										TextBlock	ellipsis =
															TextBlockHelper()
																	.setText(L"\u2026")
																	.setForeground(segmentBrush())
																	.setVerticalAlignment(VerticalAlignment::Center)
																	.setVisibility(Visibility::Collapsed)
																	.getTextBlock();
										stackPanelHelper.add(ellipsis);

										// Button
										Button	button =
														ButtonHelper()
																.setBackground(
																		winrt::Windows::UI::Colors::Transparent())
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
																.setPointerEnteredProc([this, segmentIndex]() {
																	// Update hovered index
																	mHoveredIndex = segmentIndex;
																	updateSegmentForegrounds();
																	mPathView.InvalidateMeasure();
																})
																.setPointerExitedProc([this, segmentIndex]() {
																	// Clear hovered index
																	if (mHoveredIndex == segmentIndex)
																		// No longer hovered
																		mHoveredIndex = -1;
																	updateSegmentForegrounds();
																	mPathView.InvalidateMeasure();
																})
																.getButton();

										// A collapsed segment cannot say what it is, and no segment says where a click
										//	will land, so the tool tip says both
										ToolTipService::SetToolTip(button,
												winrt::box_value(winrt::hstring(cumulativePath.c_str())));

										mPathView.Children().Append(button);

										// Store segment info
										SegmentInfo	segmentInfo;
										segmentInfo.mCumulativePath = cumulativePath;
										segmentInfo.mButton = button;
										segmentInfo.mLabel = label;
										segmentInfo.mEllipsis = ellipsis;
										mSegments.push_back(segmentInfo);
									}

				void			updateSegmentForegrounds()
									{
										// Setup
										Brush	brush = segmentBrush();
										Brush	hoveredBrush = brushFor(L"AccentTextFillColorPrimaryBrush");

										// Iterate segments
										for (int i = 0; i < (int) mSegments.size(); i++) {
											// Setup.  The segment under the mouse takes the accent color, which is what
											//	says it will act on a click.
											Brush	foregroundBrush =
															((i == mHoveredIndex) && (hoveredBrush != nullptr)) ?
																	hoveredBrush : brush;
											if (foregroundBrush == nullptr)
												// Leave as-is
												continue;

											// Update
											mSegments[i].mLabel.Foreground(foregroundBrush);
											mSegments[i].mEllipsis.Foreground(foregroundBrush);
										}
									}

				double			widthOf(const std::vector<bool>& isFull)
									{
										// The trailing separator is there whatever the segments are showing
										double	width =
														(mTrailingFontIcon != nullptr) ?
																mTrailingFontIcon.DesiredSize().Width : 0.0;

										// Add up what each segment is showing
										for (size_t i = 0; i < mSegments.size(); i++)
											// Add this segment
											width += isFull[i] ? mSegments[i].mFullWidth : mSegments[i].mCollapsedWidth;

										return width;
									}

				void			show(std::vector<bool>& isFull, int from, int through, double availableWidth)
									{
										// Setup
										double	usedWidth = widthOf(isFull);

										// Show as many as will fit, working from the tail toward the head
										for (int i = from; i >= through; i--) {
											// Check if already showing
											if (isFull[i])
												continue;

											// Check if showing this one fits
											double	deltaWidth = mSegments[i].mFullWidth - mSegments[i].mCollapsedWidth;
											if ((usedWidth + deltaWidth) > availableWidth)
												continue;

											// Show
											isFull[i] = true;
											usedWidth += deltaWidth;
										}
									}

				Brush			segmentBrush()
									{
										// A path standing on its own is the value being shown, so it reads as one.  A
										//	path giving context to something else steps back so that something else
										//	reads first.
										return brushFor(
												(mPathStyle == PathViewStyle::Dimmed) ?
														L"TextFillColorSecondaryBrush" : L"TextFillColorPrimaryBrush");
									}

				FontIcon		chevronFontIcon()
									{
										// Setup.  Separators are punctuation, not content, so they sit a step behind
										//	whatever the segments are showing.
										FontIconHelper	fontIconHelper(L"\uE76C", 10.0);
										fontIconHelper.setVerticalAlignment(VerticalAlignment::Center);

										Brush	brush = brushFor(L"TextFillColorTertiaryBrush");
										if (brush != nullptr)
											// Set foreground
											fontIconHelper.getFontIcon().Foreground(brush);

										return fontIconHelper.getFontIcon();
									}

		static	ComponentInfos	componentInfosFor(const std::wstring& path)
									{
										// Setup
										ComponentInfos	componentInfos;
										size_t			pathLength = path.size();

										// Iterate path, extracting each component (with cumulative path from the
										//	absolute offset)
										size_t	i = 0;
										while (i < pathLength) {
											// Skip separators
											while ((i < pathLength) && ((path[i] == L'\\') || (path[i] == L'/')))
												// Skipping path separator
												i++;
											if (i >= pathLength)
												break;

											// Collect component name
											size_t	componentStart = i;
											while ((i < pathLength) && (path[i] != L'\\') && (path[i] != L'/'))
												// One more character
												i++;

											// Setup component strings
											ComponentInfo	componentInfo;
											componentInfo.mName = path.substr(componentStart, i - componentStart);
											componentInfo.mCumulativePath = path.substr(0, i);
											if (!componentInfo.mCumulativePath.empty() &&
													(componentInfo.mCumulativePath.back() == L':'))
												// Drive root (e.g. "C:" => "C:\\")
												componentInfo.mCumulativePath += L'\\';

											componentInfos.push_back(componentInfo);
										}

										return componentInfos;
									}

		static	Brush			brushFor(const winrt::hstring& key)
									{
										// Look up the theme brush
										winrt::Windows::Foundation::IInspectable
												object =
														Application::Current().Resources()
																.TryLookup(winrt::box_value(key));

										return (object != nullptr) ? object.try_as<Brush>() : nullptr;
									}

		static	void			setSegmentCollapsed(const SegmentInfo& segmentInfo, bool collapsed)
									{
										// We show either label or ellipsis
										segmentInfo.mLabel.Visibility(
												collapsed ? Visibility::Collapsed : Visibility::Visible);
										segmentInfo.mEllipsis.Visibility(
												collapsed ? Visibility::Visible : Visibility::Collapsed);
									}

	// Properties
	public:
		PathView&					mPathView;
		std::wstring				mRootPath;
		PathViewStyle				mPathStyle;
		bool						mShowsTrailingSeparator;

		std::vector<SegmentInfo>	mSegments;
		int							mHoveredIndex;
		FontIcon					mTrailingFontIcon;
};

//----------------------------------------------------------------------------------------------------------------------
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
	// Check if changed - a recycled list item is handed the same path far more often than a different one, and
	//	rebuilding the segments throws away every child element to build the same ones again
	if (path == Path())
		// Nothing to do
		return;

	// Store
	SetValue(PathProperty(), winrt::box_value(path));

	// Rebuild segments
	mInternals->rebuildSegments();
}

//----------------------------------------------------------------------------------------------------------------------
winrt::hstring PathView::RootPath() const
//----------------------------------------------------------------------------------------------------------------------
{
	return winrt::hstring(mInternals->mRootPath.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
void PathView::RootPath(const winrt::hstring& rootPath)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (std::wstring(rootPath.c_str()) == mInternals->mRootPath)
		// Nothing to do
		return;

	// Store
	mInternals->mRootPath = rootPath.c_str();

	// Rebuild segments
	mInternals->rebuildSegments();
}

//----------------------------------------------------------------------------------------------------------------------
bool PathView::ShowsTrailingSeparator() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mShowsTrailingSeparator;
}

//----------------------------------------------------------------------------------------------------------------------
void PathView::ShowsTrailingSeparator(bool showsTrailingSeparator)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (showsTrailingSeparator == mInternals->mShowsTrailingSeparator)
		// Nothing to do
		return;

	// Store
	mInternals->mShowsTrailingSeparator = showsTrailingSeparator;

	// Rebuild segments
	mInternals->rebuildSegments();
}

//----------------------------------------------------------------------------------------------------------------------
PathViewStyle PathView::PathStyle() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mPathStyle;
}

//----------------------------------------------------------------------------------------------------------------------
void PathView::PathStyle(PathViewStyle pathStyle)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (pathStyle == mInternals->mPathStyle)
		// Nothing to do
		return;

	// Store
	mInternals->mPathStyle = pathStyle;

	// Update - only the foreground changes, so nothing needs measuring again
	mInternals->updateSegmentForegrounds();
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

	// Measure the trailing separator, which is there whatever the segments are showing
	double	trailingWidth = 0.0;
	if (mInternals->mTrailingFontIcon != nullptr) {
		// Measure
		mInternals->mTrailingFontIcon.Measure(measureSize);
		trailingWidth = mInternals->mTrailingFontIcon.DesiredSize().Width;
	}

	// Decide which segments are shown full, taking no account of what is hovered.  The last is always full, and then
	//	segments are promoted from the tail toward the head while they still fit.
	double				availableWidth = (double) availableSize.Width;
	std::vector<bool>	isFull(segmentsCount, false);
	if (std::isinf(availableSize.Width) || ((sumFullWidth + trailingWidth) <= availableWidth))
		// Everything fits
		for (size_t i = 0; i < segmentsCount; isFull[i++] = true) ;
	else {
		// Not everything fits, so the last is shown and the rest earn their place from the tail back
		isFull[segmentsCount - 1] = true;
		mInternals->show(isFull, (int) segmentsCount - 2, 0, availableWidth);
	}

	// Expand the hovered segment.  Its left edge must not move - if it did, it would slide out from under the mouse,
	//	the mouse would land on a different segment, and the path would flip back and forth - so everything after it is
	//	taken back and then earned again, leaving the room it needs to come only from there.
	int	hoveredIndex = mInternals->mHoveredIndex;
	if ((hoveredIndex >= 0) && (hoveredIndex < (int) segmentsCount) && !isFull[hoveredIndex]) {
		// Expand
		isFull[hoveredIndex] = true;
		for (int i = hoveredIndex + 1; i < (int) segmentsCount; i++)
			// Take back
			isFull[i] = false;

		mInternals->show(isFull, (int) segmentsCount - 1, hoveredIndex + 1, availableWidth);
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

	// Add the trailing separator
	if (mInternals->mTrailingFontIcon != nullptr) {
		// Update totals
		Size	desiredSize = mInternals->mTrailingFontIcon.DesiredSize();
		totalWidth += desiredSize.Width;
		maxHeight = std::max(maxHeight, desiredSize.Height);
	}

	// Compose desired width
	double	desiredWidth = std::isinf(availableSize.Width) ? totalWidth : std::min(totalWidth, availableWidth);

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

	// Lay out the trailing separator
	if (mInternals->mTrailingFontIcon != nullptr) {
		// Setup
		double	width = mInternals->mTrailingFontIcon.DesiredSize().Width;

		// Arrange
		mInternals->mTrailingFontIcon.Arrange(Rect{(float) x, 0.0f, (float) width, finalSize.Height});
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
