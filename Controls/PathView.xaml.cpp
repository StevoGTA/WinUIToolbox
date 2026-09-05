//----------------------------------------------------------------------------------------------------------------------
//	PathView.xaml.cpp			©2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "PathView.xaml.h"

#include "ButtonHelper.h"
#include "FontIconHelper.h"
#include "StackPanelHelper.h"
#include "TextBlockHelper.h"

#include "winrt\Microsoft.UI.Composition.h"
#include "winrt\Microsoft.UI.Dispatching.h"
#include "winrt\Microsoft.UI.Input.h"
#include "winrt\Microsoft.UI.Text.h"
#include "winrt\Microsoft.UI.Xaml.Automation.h"
#include "winrt\Microsoft.UI.Xaml.Hosting.h"
#include "winrt\Microsoft.UI.Xaml.Input.h"
#include "winrt\Microsoft.UI.Xaml.Media.h"
#include "winrt\Windows.ApplicationModel.DataTransfer.h"
#include "winrt\Windows.ApplicationModel.DataTransfer.DragDrop.h"
#include "winrt\Windows.Foundation.Numerics.h"
#include "winrt\Windows.Storage.h"
#include "winrt\Windows.UI.Core.h"
#include "winrt\Windows.UI.Xaml.Interop.h"

#include <windows.h>
#include <shellapi.h>	// Needs windows.h

#include "WinUIToolbox.PathView.g.cpp"

#pragma comment(lib, "shell32.lib")

using namespace winrt::WinUIToolbox::implementation;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;

using Application = winrt::Microsoft::UI::Xaml::Application;
using AutomationProperties = winrt::Microsoft::UI::Xaml::Automation::AutomationProperties;
using Brush = winrt::Microsoft::UI::Xaml::Media::Brush;
using CompositionClip = winrt::Microsoft::UI::Composition::CompositionClip;
using Compositor = winrt::Microsoft::UI::Composition::Compositor;
using CoreCursor = winrt::Windows::UI::Core::CoreCursor;
using CoreCursorType = winrt::Windows::UI::Core::CoreCursorType;
using DataPackageOperation = winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation;
using ElementCompositionPreview = winrt::Microsoft::UI::Xaml::Hosting::ElementCompositionPreview;
using fire_and_forget = winrt::fire_and_forget;
using float2 = winrt::Windows::Foundation::Numerics::float2;
using FontWeights = winrt::Microsoft::UI::Text::FontWeights;
using IInspectable = winrt::Windows::Foundation::IInspectable;
using ImplicitAnimationCollection = winrt::Microsoft::UI::Composition::ImplicitAnimationCollection;
using InputCursor = winrt::Microsoft::UI::Input::InputCursor;
using IStorageItem = winrt::Windows::Storage::IStorageItem;
using IStorageItemVectorView = winrt::Windows::Foundation::Collections::IVectorView<IStorageItem>;
using Point = winrt::Windows::Foundation::Point;
using PointerRoutedEventArgs = winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs;
using Rect = winrt::Windows::Foundation::Rect;
using SolidColorBrush = winrt::Microsoft::UI::Xaml::Media::SolidColorBrush;
using StandardDataFormats = winrt::Windows::ApplicationModel::DataTransfer::StandardDataFormats;
using StorageItemTypes = winrt::Windows::Storage::StorageItemTypes;
using UIElements = std::vector<UIElement>;
using Visual = winrt::Microsoft::UI::Composition::Visual;

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
			std::wstring	mName;
			std::wstring	mCumulativePath;

			Button			mButton = nullptr;
			TextBlock		mLabel = nullptr;
			TextBlock		mEllipsis = nullptr;

			double			mFullWidth = 0.0;
			double			mCollapsedWidth = 0.0;
			bool			mIsCollapsed = false;

			Rect			mArrangedRect{};
		};

	// Methods
	public:
									Internals(PathView& pathView) :
										mPathView(pathView), mPathStyle(PathViewStyle::Standard),
												mShowsTrailingSeparator(false), mIsEnabled(true), mHoveredIndex(-1),
												mRootTextBlock(nullptr), mTrailingFontIcon(nullptr), mRootWidth(0.0),
												mTrailingWidth(0.0),
												mIsDropTarget(false), mDragHasFolder(false),
												mIsAnimationArmed(false), mImplicitAnimations(nullptr),
												mHandCursor(nullptr)
										{}

					void			rebuildSegments()
										{
											// Reset
											disarmAnimation();

											mPathView.Children().Clear();
											mSegments.clear();
											mHoveredIndex = -1;
											mRootTextBlock = nullptr;
											mTrailingFontIcon = nullptr;
											updateCursor();

											// Break the path and the root into their components
											std::wstring	path(mPathView.Path().c_str());
											ComponentInfos	componentInfos = componentInfosFor(path);
											ComponentInfos	rootComponentInfos = componentInfosFor(mRootPath);

											// See how much of the head is the root being shown from, comparing names
											//	without regard to case, as the filesystem does
											size_t	matchCount = 0;
											while ((matchCount < rootComponentInfos.size()) &&
													(matchCount < componentInfos.size()) &&
													(::CompareStringOrdinal(componentInfos[matchCount].mName.c_str(),
															-1, rootComponentInfos[matchCount].mName.c_str(), -1, TRUE)
																	== CSTR_EQUAL))
												// One more component of the root matched
												matchCount++;

											// Anything short of the whole root is not this path's root, so show it all
											size_t	skipCount =
															(matchCount == rootComponentInfos.size()) ? matchCount : 0;

											// A UNC path shown from its start says so, the same way writing it out
											//	as a string does - a drive letter already says it for a local path.
											//	Shown from a root, it is read relative to that root, so it says nothing.
											if (isUNC(path) && (skipCount == 0) && !componentInfos.empty()) {
												// Leading separator
												mRootTextBlock =
														TextBlockHelper()
																.setText(L"\\\\")
																.setForeground(separatorBrush())
																.setVerticalAlignment(VerticalAlignment::Center)
																.getTextBlock();
												mRootTextBlock.FontWeight(FontWeights::SemiBold());
												mPathView.Children().Append(mRootTextBlock);
											}

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

											// Measure what each element wants once, here, rather than on every pass.
											//	Measuring a segment means toggling what it shows, and doing that in a
											//	measure pass invalidates the very layout being computed.
											measureElements();

											// Say what the whole path is, for anyone who cannot see it
											std::wstring	names;
											for (size_t i = 0; i < mSegments.size(); i++)
												// Add name
												names += ((i > 0) ? L", " : L"") + mSegments[i].mName;
											AutomationProperties::SetName(mPathView, winrt::hstring(names));

											// Request layout
											mPathView.InvalidateMeasure();
										}
					void			addSegment(const ComponentInfo& componentInfo, bool showsChevron)
										{
											// Setup
											std::wstring		cumulativePath = componentInfo.mCumulativePath;

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

											// Button.  Where the pointer is over the path is answered by the path view
											//	itself, so the button says nothing about it.
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
																				cumulativePath.c_str(), nullptr,
																				nullptr, SW_SHOW);
																	})
																	.getButton();
											button.IsEnabled(mIsEnabled);

											// A collapsed segment cannot say what it is, and no segment says where a
											//	click will land, so the tool tip says both
											ToolTipService::SetToolTip(button,
													winrt::box_value(winrt::hstring(cumulativePath.c_str())));

											// The button's content is a panel, which says nothing to a screen reader
											AutomationProperties::SetName(button, winrt::hstring(componentInfo.mName));

											mPathView.Children().Append(button);

											// Store segment info
											SegmentInfo	segmentInfo;
											segmentInfo.mName = componentInfo.mName;
											segmentInfo.mCumulativePath = cumulativePath;
											segmentInfo.mButton = button;
											segmentInfo.mLabel = label;
											segmentInfo.mEllipsis = ellipsis;
											mSegments.push_back(segmentInfo);
										}

					void			measureElements()
										{
											// Setup
											float	infinity = std::numeric_limits<float>::infinity();
											Size	measureSize{infinity, infinity};

											// Measure each segment in both states
											for (SegmentInfo& segmentInfo : mSegments) {
												// Full
												setSegmentCollapsed(segmentInfo, false);
												segmentInfo.mButton.Measure(measureSize);
												segmentInfo.mFullWidth = segmentInfo.mButton.DesiredSize().Width;

												// Collapsed
												setSegmentCollapsed(segmentInfo, true);
												segmentInfo.mButton.Measure(measureSize);
												segmentInfo.mCollapsedWidth = segmentInfo.mButton.DesiredSize().Width;

												// Leave showing in full
												setSegmentCollapsed(segmentInfo, false);
											}

											// Measure the separators, which are there whatever the segments are showing
											if (mRootTextBlock != nullptr) {
												// Measure
												mRootTextBlock.Measure(measureSize);
												mRootWidth = mRootTextBlock.DesiredSize().Width;
											} else
												// None
												mRootWidth = 0.0;

											if (mTrailingFontIcon != nullptr) {
												// Measure
												mTrailingFontIcon.Measure(measureSize);
												mTrailingWidth = mTrailingFontIcon.DesiredSize().Width;
											} else
												// None
												mTrailingWidth = 0.0;
										}

					void			updateSegmentForegrounds()
										{
											// Setup
											Brush	brush = segmentBrush();
											Brush	hoveredBrush = brushFor(L"AccentTextFillColorPrimaryBrush");

											// Iterate segments
											for (int i = 0; i < (int) mSegments.size(); i++) {
												// Setup.  The segment under the pointer takes the accent color, which
												//	is what says it will act on a click.
												Brush	foregroundBrush =
																(mIsEnabled && (i == mHoveredIndex) &&
																				(hoveredBrush != nullptr)) ?
																		hoveredBrush : brush;
												if (foregroundBrush == nullptr)
													// Leave as-is
													continue;

												// Update
												mSegments[i].mLabel.Foreground(foregroundBrush);
												mSegments[i].mEllipsis.Foreground(foregroundBrush);
											}
										}

					void			updateCursor()
										{
											// Every segment reveals something, so the hand stands over whichever one
											//	is under the pointer - and over nothing when the path is not taking
											//	clicks
											bool	wantsHand = mIsEnabled && (mHoveredIndex >= 0);
											if (wantsHand && (mHandCursor == nullptr))
												// Setup
												mHandCursor =
														InputCursor::CreateFromCoreCursor(
																CoreCursor(CoreCursorType::Hand, 0));

											mPathView.ProtectedCursor(wantsHand ? mHandCursor : InputCursor(nullptr));
										}

					int				segmentIndexAt(const Point& point) const
										{
											// Every segment is placed in this view's own coordinates
											for (int i = 0; i < (int) mSegments.size(); i++) {
												// Check if inside
												const	Rect&	rect = mSegments[i].mArrangedRect;
												if ((point.X >= rect.X) && (point.X < (rect.X + rect.Width)) &&
														(point.Y >= rect.Y) && (point.Y < (rect.Y + rect.Height)))
													// Found
													return i;
											}

											return -1;
										}

					void			updateHoveredIndex(int hoveredIndex)
										{
											// Check if changed
											if (hoveredIndex == mHoveredIndex)
												// Nothing to do
												return;

											// Update
											mHoveredIndex = hoveredIndex;

											// Bring the hovered segment forward
											updateSegmentForegrounds();
											updateCursor();

											// Expand and collapse by moving, not by jumping.  What is being read has
											//	just changed shape and position, and the eye can only follow that if
											//	it can see it happen.
											armAnimation();
											mPathView.InvalidateMeasure();
										}

					void			armAnimation()
										{
											// Check if already armed
											if (mIsAnimationArmed)
												// Nothing to do
												return;

											// Everything that can move gets the same animation
											for (const UIElement& element : animatedElements()) {
												// Setup
												Visual	visual = ElementCompositionPreview::GetElementVisual(element);
												if (mImplicitAnimations == nullptr) {
													// Compose once - 0.3s, easing out, on wherever layout puts things
													Compositor	compositor = visual.Compositor();
													auto		easingFunction =
																		compositor.CreateCubicBezierEasingFunction(
																				float2(0.0f, 0.0f),
																				float2(0.58f, 1.0f));

													auto	offsetAnimation =
																	compositor.CreateVector3KeyFrameAnimation();
													offsetAnimation.InsertExpressionKeyFrame(1.0f, L"this.FinalValue",
															easingFunction);
													offsetAnimation.Duration(std::chrono::milliseconds{300});
													offsetAnimation.Target(L"Offset");

													auto	sizeAnimation = compositor.CreateVector2KeyFrameAnimation();
													sizeAnimation.InsertExpressionKeyFrame(1.0f, L"this.FinalValue",
															easingFunction);
													sizeAnimation.Duration(std::chrono::milliseconds{300});
													sizeAnimation.Target(L"Size");

													mImplicitAnimations =
															compositor.CreateImplicitAnimationCollection();
													mImplicitAnimations.Insert(L"Offset", offsetAnimation);
													mImplicitAnimations.Insert(L"Size", sizeAnimation);
												}

												// Arm.  A segment whose width is still moving would otherwise show its
												//	new title spilling past its edge.
												visual.ImplicitAnimations(mImplicitAnimations);
												visual.Clip(visual.Compositor().CreateInsetClip());
											}

											mIsAnimationArmed = true;
										}

					void			disarmAnimation()
										{
											// Check if armed
											if (!mIsAnimationArmed)
												// Nothing to do
												return;

											// Disarm.  Animations already under way carry on - only new ones stop.
											for (const UIElement& element : animatedElements()) {
												// Setup
												Visual	visual = ElementCompositionPreview::GetElementVisual(element);
												visual.ImplicitAnimations(ImplicitAnimationCollection(nullptr));
												visual.Clip(CompositionClip(nullptr));
											}

											mIsAnimationArmed = false;
										}

					UIElements		animatedElements() const
										{
											// Compose
											UIElements	elements;
											if (mRootTextBlock != nullptr)
												elements.push_back(mRootTextBlock);
											for (const SegmentInfo& segmentInfo : mSegments)
												elements.push_back(segmentInfo.mButton);
											if (mTrailingFontIcon != nullptr)
												elements.push_back(mTrailingFontIcon);

											return elements;
										}

					void			setIsDropTarget(bool isDropTarget)
										{
											// Check if changed
											if (isDropTarget == mIsDropTarget)
												// Nothing to do
												return;

											// Update
											mIsDropTarget = isDropTarget;

											// Highlight.  The accent at a quarter strength says something can land here
											//	without shouting about it.
											Brush	background = nullptr;
											if (mIsDropTarget) {
												// Compose from the accent
												Brush			brush = brushFor(L"AccentFillColorDefaultBrush");
												SolidColorBrush	accentBrush = nullptr;
												if (brush != nullptr)
													// Have brush
													accentBrush = brush.try_as<SolidColorBrush>();
												if (accentBrush != nullptr) {
													// Have accent
													auto	color = accentBrush.Color();
													color.A = 64;
													background = SolidColorBrush(color);
												}
											}

											mPathView.Background(background);
										}

					fire_and_forget	handleDragEnter(DragEventArgs dragEventArgs)
										{
											// A path view that is not taking clicks is not taking drops either, and
											//	neither is one with nothing to do with a folder
											if (!mIsEnabled || !mFolderDroppedEvent)
												// Not a destination
												co_return;

											// Ask what is being dragged before saying whether it can land - which is
											//	not known until it is read, so the answer is deferred
											auto	deferral = dragEventArgs.GetDeferral();
											auto	weakPathView = mPathView.get_weak();

											bool	hasFolder = false;
											auto	dataView = dragEventArgs.DataView();
											if (dataView.Contains(StandardDataFormats::StorageItems())) {
												// Look for a lone folder - anything else is not a destination
												IStorageItemVectorView	storageItems =
																				co_await
																						dataView.GetStorageItemsAsync();
												hasFolder = storageItems.Size() == 1;
												if (hasFolder)
													// One item - is it a folder
													hasFolder =
															storageItems.GetAt(0).IsOfType(StorageItemTypes::Folder);
											}

											// Check if still around
											if (weakPathView.get() == nullptr) {
												// Gone
												deferral.Complete();
												co_return;
											}

											// Update
											mDragHasFolder = hasFolder;
											if (mDragHasFolder) {
												// Take it, and say so - so nothing behind this view takes it as well
												dragEventArgs.AcceptedOperation(DataPackageOperation::Link);
												dragEventArgs.Handled(true);
												setIsDropTarget(true);
											}

											deferral.Complete();
										}

					void			handleDragOver(const DragEventArgs& dragEventArgs)
										{
											// Check if have a folder to take
											if (!mDragHasFolder)
												// Not taking this
												return;

											// Take it
											dragEventArgs.AcceptedOperation(DataPackageOperation::Link);
											dragEventArgs.Handled(true);
										}

					void			handleDragLeave()
										{
											// Reset
											mDragHasFolder = false;
											setIsDropTarget(false);
										}

					fire_and_forget	handleDrop(DragEventArgs dragEventArgs)
										{
											// Check if have a folder to take
											if (!mDragHasFolder)
												// Not taking this
												co_return;

											// Take it, before anything behind this view is asked
											dragEventArgs.Handled(true);

											// Unhighlight
											mDragHasFolder = false;
											setIsDropTarget(false);

											// Get the folder
											auto	deferral = dragEventArgs.GetDeferral();
											auto	weakPathView = mPathView.get_weak();

											IStorageItemVectorView	storageItems =
																			co_await dragEventArgs.DataView()
																					.GetStorageItemsAsync();
											winrt::hstring			folderPath;
											if ((storageItems.Size() == 1) &&
													storageItems.GetAt(0).IsOfType(StorageItemTypes::Folder))
												// Have folder
												folderPath = storageItems.GetAt(0).Path();

											deferral.Complete();

											// Check if still around
											if (weakPathView.get() == nullptr)
												// Gone
												co_return;

											// Trigger event
											if (!folderPath.empty())
												// Have folder
												mFolderDroppedEvent(mPathView, folderPath);
										}

					double			widthOf(const std::vector<bool>& isFull) const
										{
											// The separators are there whatever the segments are showing
											double	width = mRootWidth + mTrailingWidth;

											// Add up what each segment is showing
											for (size_t i = 0; i < mSegments.size(); i++)
												// Add this segment
												width +=
														isFull[i] ?
																mSegments[i].mFullWidth : mSegments[i].mCollapsedWidth;

											return width;
										}

					void			show(std::vector<bool>& isFull, int from, int through, double availableWidth) const
										{
											// Setup
											double	usedWidth = widthOf(isFull);

											// Show as many as will fit, working from the tail toward the head
											for (int i = from; i >= through; i--) {
												// Check if already showing
												if (isFull[i])
													continue;

												// Check if showing this one fits
												double	deltaWidth =
																mSegments[i].mFullWidth - mSegments[i].mCollapsedWidth;
												if ((usedWidth + deltaWidth) > availableWidth)
													continue;

												// Show
												isFull[i] = true;
												usedWidth += deltaWidth;
											}
										}

					Brush			segmentBrush() const
										{
											// A path standing on its own is the value being shown, so it reads as one.
											//	A path giving context to something else steps back so that something
											//	else reads first.  A path not taking clicks steps back further still.
											return brushFor(
													!mIsEnabled ?
															L"TextFillColorDisabledBrush" :
															(mPathStyle == PathViewStyle::Dimmed) ?
																	L"TextFillColorSecondaryBrush" :
																	L"TextFillColorPrimaryBrush");
										}

					Brush			separatorBrush() const
										{ return brushFor(
												mIsEnabled ?
														L"TextFillColorTertiaryBrush" :
														L"TextFillColorDisabledBrush"); }

					FontIcon		chevronFontIcon() const
										{
											// Setup
											FontIconHelper	fontIconHelper(L"\uE76C", 10.0);
											fontIconHelper.setVerticalAlignment(VerticalAlignment::Center);

											Brush	brush = separatorBrush();
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
												while ((i < pathLength) && isSeparator(path[i]))
													// Skipping path separator
													i++;
												if (i >= pathLength)
													break;

												// Collect component name
												size_t	componentStart = i;
												while ((i < pathLength) && !isSeparator(path[i]))
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

			static	bool			isSeparator(wchar_t character)
										{ return (character == L'\\') || (character == L'/'); }

			static	bool			isUNC(const std::wstring& path)
										{ return (path.size() >= 2) && isSeparator(path[0]) && isSeparator(path[1]); }

			static	Brush			brushFor(const winrt::hstring& key)
										{
											// Look up the theme brush
											auto			resources = Application::Current().Resources();
											IInspectable	object = resources.TryLookup(winrt::box_value(key));

											return (object != nullptr) ? object.try_as<Brush>() : nullptr;
										}

			static	void			setSegmentCollapsed(SegmentInfo& segmentInfo, bool collapsed)
										{
											// Check if changed - a visibility write invalidates layout up the tree, so
											//	it is only made when it changes something
											if (collapsed == segmentInfo.mIsCollapsed)
												// Nothing to do
												return;

											// We show either label or ellipsis
											segmentInfo.mIsCollapsed = collapsed;
											segmentInfo.mLabel.Visibility(
													collapsed ? Visibility::Collapsed : Visibility::Visible);
											segmentInfo.mEllipsis.Visibility(
													collapsed ? Visibility::Visible : Visibility::Collapsed);
										}

	// Properties
	public:
			PathView&							mPathView;
			std::wstring						mRootPath;
			PathViewStyle						mPathStyle;
			bool								mShowsTrailingSeparator;
			bool								mIsEnabled;

			std::vector<SegmentInfo>			mSegments;
			int									mHoveredIndex;
			TextBlock							mRootTextBlock;
			FontIcon							mTrailingFontIcon;
			double								mRootWidth;
			double								mTrailingWidth;

			bool								mIsDropTarget;
			bool								mDragHasFolder;
			event<FolderDroppedEventHandler>	mFolderDroppedEvent;

			bool								mIsAnimationArmed;
			ImplicitAnimationCollection			mImplicitAnimations;

			InputCursor							mHandCursor;
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
{
	// Setup hover.  Where the pointer is over the path is answered here, in the path's own coordinates, rather than by
	//	each segment - crossing from one segment to the next then reads as one move, not as leaving and arriving.
	PointerMoved([this](const IInspectable& sender, const PointerRoutedEventArgs& pointerRoutedEventArgs){
		// A path view that is not taking clicks has nothing to say about where the pointer is
		if (!mInternals->mIsEnabled)
			return;

		// Look for a segment under the pointer.  A separator, or the space beside it, is somewhere the pointer passes
		//	through on its way somewhere else - it is not someone deciding to stop reading - so passing over one
		//	leaves the hovered segment alone.  It gives way when the pointer reaches another segment, or leaves the
		//	path altogether.
		int	index = mInternals->segmentIndexAt(pointerRoutedEventArgs.GetCurrentPoint(*this).Position());
		if (index < 0)
			return;

		// Update hovered segment
		mInternals->updateHoveredIndex(index);
	});
	PointerExited([this](const IInspectable& sender, const PointerRoutedEventArgs& pointerRoutedEventArgs){
		// Nothing hovered
		mInternals->updateHoveredIndex(-1);
	});

	// Setup animation.  An animation is armed for one layout pass - the one a hover change asks for.  The visuals are
	//	updated after that pass, not during it, so the disarming waits a turn of the dispatcher to let the animations
	//	the pass triggered get under way.
	LayoutUpdated([weakPathView = get_weak()](const IInspectable& sender, const IInspectable& args){
		// Check if still around.  A weak reference to the implementation resolves to a com_ptr to it.
		auto	pathView = weakPathView.get();
		if ((pathView == nullptr) || !pathView->mInternals->mIsAnimationArmed)
			return;

		// Disarm after this turn
		pathView->DispatcherQueue().TryEnqueue([weakPathView](){
			// Check if still around
			auto	pathView = weakPathView.get();
			if (pathView != nullptr)
				// Disarm
				pathView->mInternals->disarmAnimation();
		});
	});

	// Setup drops.  Nothing lands here until someone says what to do with a folder (see FolderDroppedEvent).
	DragEnter([this](const IInspectable& sender, const DragEventArgs& dragEventArgs){
		// Handle
		mInternals->handleDragEnter(dragEventArgs);
	});
	DragOver([this](const IInspectable& sender, const DragEventArgs& dragEventArgs){
		// Handle
		mInternals->handleDragOver(dragEventArgs);
	});
	DragLeave([this](const IInspectable& sender, const DragEventArgs& dragEventArgs){
		// Handle
		mInternals->handleDragLeave();
	});
	Drop([this](const IInspectable& sender, const DragEventArgs& dragEventArgs){
		// Handle
		mInternals->handleDrop(dragEventArgs);
	});
}

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

//----------------------------------------------------------------------------------------------------------------------
bool PathView::IsEnabled() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mIsEnabled;
}

//----------------------------------------------------------------------------------------------------------------------
void PathView::IsEnabled(bool isEnabled)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (isEnabled == mInternals->mIsEnabled)
		// Nothing to do
		return;

	// Store
	mInternals->mIsEnabled = isEnabled;

	// Rebuild - the separators, the segments, and what the pointer does over them all answer to this
	mInternals->rebuildSegments();
}

// MARK: Event methods

//----------------------------------------------------------------------------------------------------------------------
event_token PathView::FolderDroppedEvent(const FolderDroppedEventHandler& handler)
//----------------------------------------------------------------------------------------------------------------------
{
	// Add
	event_token	token = mInternals->mFolderDroppedEvent.add(handler);

	// Only a path view that can do something with a folder should be a drag destination
	AllowDrop(true);

	return token;
}

//----------------------------------------------------------------------------------------------------------------------
void PathView::FolderDroppedEvent(const event_token& token) noexcept
//----------------------------------------------------------------------------------------------------------------------
{
	// Remove
	mInternals->mFolderDroppedEvent.remove(token);

	// Check if still a destination
	AllowDrop((bool) mInternals->mFolderDroppedEvent);
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

	// What everything wants was measured when it was built - only what is shown in full is decided here
	double	sumFullWidth = mInternals->mRootWidth + mInternals->mTrailingWidth;
	for (const Internals::SegmentInfo& segmentInfo : mInternals->mSegments)
		// Add this segment
		sumFullWidth += segmentInfo.mFullWidth;

	// Decide which segments are shown full, taking no account of what is hovered.  The last is always full, and then
	//	segments are promoted from the tail toward the head while they still fit.
	double				availableWidth = (double) availableSize.Width;
	std::vector<bool>	isFull(segmentsCount, false);
	if (std::isinf(availableSize.Width) || (sumFullWidth <= availableWidth))
		// Everything fits
		for (size_t i = 0; i < segmentsCount; isFull[i++] = true) ;
	else {
		// Not everything fits, so the last is shown and the rest earn their place from the tail back
		isFull[segmentsCount - 1] = true;
		mInternals->show(isFull, (int) segmentsCount - 2, 0, availableWidth);
	}

	// Expand the hovered segment.  Its left edge must not move - if it did, it would slide out from under the pointer,
	//	the pointer would land on a different segment, and the path would flip back and forth - so everything after it
	//	is taken back and then earned again, leaving the room it needs to come only from there.
	int	hoveredIndex = mInternals->mHoveredIndex;
	if ((hoveredIndex >= 0) && (hoveredIndex < (int) segmentsCount) && !isFull[hoveredIndex]) {
		// Expand
		isFull[hoveredIndex] = true;
		for (int i = hoveredIndex + 1; i < (int) segmentsCount; i++)
			// Take back
			isFull[i] = false;

		mInternals->show(isFull, (int) segmentsCount - 1, hoveredIndex + 1, availableWidth);
	}

	// Apply final states, measure once each, and compute desired size
	double	totalWidth = 0.0;
	float	maxHeight = 0.0f;

	if (mInternals->mRootTextBlock != nullptr) {
		// Measure
		mInternals->mRootTextBlock.Measure(measureSize);

		Size	desiredSize = mInternals->mRootTextBlock.DesiredSize();
		totalWidth += desiredSize.Width;
		maxHeight = std::max(maxHeight, desiredSize.Height);
	}

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

	if (mInternals->mTrailingFontIcon != nullptr) {
		// Measure
		mInternals->mTrailingFontIcon.Measure(measureSize);

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
	// Setup.  Everything is placed left to right at its measured width, taking no more than is left.
	double	x = 0.0;
	auto	place = [&x, &finalSize](const UIElement& element) -> Rect {
					// Take no more width than is left
					double	desiredWidth = element.DesiredSize().Width;
					double	width = std::max(std::min(desiredWidth, (double) finalSize.Width - x), 0.0);
					Rect	rect{(float) x, 0.0f, (float) width, finalSize.Height};

					// Arrange
					element.Arrange(rect);

					// Advance
					x += desiredWidth;

					return rect;
				};

	// Leading separator
	if (mInternals->mRootTextBlock != nullptr)
		// Place
		place(mInternals->mRootTextBlock);

	// Segments, remembering where each one went so that the pointer can be answered
	for (Internals::SegmentInfo& segmentInfo : mInternals->mSegments)
		// Place
		segmentInfo.mArrangedRect = place(segmentInfo.mButton);

	// Trailing separator
	if (mInternals->mTrailingFontIcon != nullptr)
		// Place
		place(mInternals->mTrailingFontIcon);

	return finalSize;
}

// MARK: Class methods

//----------------------------------------------------------------------------------------------------------------------
DependencyProperty PathView::PathProperty() noexcept
//----------------------------------------------------------------------------------------------------------------------
{
	return mPathProperty;
}
