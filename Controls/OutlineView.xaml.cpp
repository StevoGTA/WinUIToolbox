//----------------------------------------------------------------------------------------------------------------------
//	OutlineView.xaml.cpp			©2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "OutlineView.xaml.h"

#include "OutlineViewDataSource.h"
#include "PointerHelper.h"

#include "winrt\Microsoft.UI.h"
#include "winrt\Microsoft.UI.Input.h"
#include "winrt\Microsoft.UI.Text.h"
#include "winrt\Microsoft.UI.Xaml.Controls.Primitives.h"
#include "winrt\Microsoft.UI.Xaml.Input.h"
#include "winrt\Microsoft.UI.Xaml.Media.h"
#include "winrt\Microsoft.UI.Xaml.Shapes.h"
#include "winrt\Windows.Foundation.h"
#include "winrt\Windows.Foundation.Collections.h"
#include "winrt\Windows.System.h"
#include "winrt\Windows.UI.Core.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cwctype>
#include <limits>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "WinUIToolbox.OutlineView.g.cpp"
#include "WinUIToolbox.OutlineViewCell.g.cpp"

using namespace winrt::WinUIToolbox::implementation;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;

using Application = winrt::Microsoft::UI::Xaml::Application;
using Brush = winrt::Microsoft::UI::Xaml::Media::Brush;
using CharacterReceivedRoutedEventArgs = winrt::Microsoft::UI::Xaml::Input::CharacterReceivedRoutedEventArgs;
using ContextRequestedEventArgs = winrt::Microsoft::UI::Xaml::Input::ContextRequestedEventArgs;
using CoreVirtualKeyStates = winrt::Windows::UI::Core::CoreVirtualKeyStates;
using DataSource = OutlineViewDataSource;
using FontWeights = winrt::Microsoft::UI::Text::FontWeights;
using InputCursor = winrt::Microsoft::UI::Input::InputCursor;
using InputKeyboardSource = winrt::Microsoft::UI::Input::InputKeyboardSource;
using InputSystemCursor = winrt::Microsoft::UI::Input::InputSystemCursor;
using InputSystemCursorShape = winrt::Microsoft::UI::Input::InputSystemCursorShape;
using KeyRoutedEventArgs = winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs;
using OutlineViewEditEndReason = winrt::WinUIToolbox::OutlineViewEditEndReason;
using RangeBaseValueChangedEventArgs = winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs;
using Rect = winrt::Windows::Foundation::Rect;
using RectangleGeometry = winrt::Microsoft::UI::Xaml::Media::RectangleGeometry;
using ScrollBar = winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollBar;
using ScrollingIndicatorMode = winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollingIndicatorMode;
using Size = winrt::Windows::Foundation::Size;
using SolidColorBrush = winrt::Microsoft::UI::Xaml::Media::SolidColorBrush;
using SortDescriptor = OutlineViewDataSource::SortDescriptor;
using TranslateTransform = winrt::Microsoft::UI::Xaml::Media::TranslateTransform;
using VirtualKey = winrt::Windows::System::VirtualKey;

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	const	double		kGutterWidth = 24.0;				// Chevron column for OutlineViewStyle::Gutter
static	const	double		kIndentPerLevel = 16.0;
static	const	double		kChevronWidth = 16.0;
static	const	double		kCellInset = 6.0;					// Horizontal breathing room inside a cell
static	const	double		kResizeHitZone = 4.0;				// Either side of a header divider
static	const	double		kDragThreshold = 6.0;				// Header movement before a press becomes a column drag
static	const	double		kSelectionIndicatorWidth = 3.0;
static	const	double		kSelectionIndicatorHeight = 16.0;
static	const	double		kWheelRowsPerNotch = 3.0;
static	const	double		kWheelPixelsPerNotch = 48.0;
static	const	int			kTextSearchTimeoutMilliseconds = 1000;

static	const	wchar_t*	kChevronRightGlyph = L"\uE76C";		// Segoe Fluent Icons ChevronRight
static	const	wchar_t*	kChevronDownGlyph = L"\uE70D";		// ChevronDown
static	const	wchar_t*	kSortAscendingGlyph = L"\uE70E";	// ChevronUp
static	const	wchar_t*	kSortDescendingGlyph = L"\uE70D";	// ChevronDown

static	const	wchar_t*	kTextCellRecycleKey = L"text";

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local procs

//----------------------------------------------------------------------------------------------------------------------
static winrt::hstring sToHString(const std::optional<std::wstring>& string)
//----------------------------------------------------------------------------------------------------------------------
{
	return string.has_value() ? winrt::hstring(*string) : winrt::hstring();
}

//----------------------------------------------------------------------------------------------------------------------
static IVectorView<winrt::hstring> sToHStrings(const std::vector<std::wstring>& strings)
//----------------------------------------------------------------------------------------------------------------------
{
	// Convert
	std::vector<winrt::hstring>	values;
	values.reserve(strings.size());
	for (const std::wstring& string : strings)
		// Add
		values.push_back(winrt::hstring(string));

	return winrt::single_threaded_vector<winrt::hstring>(std::move(values)).GetView();
}

//----------------------------------------------------------------------------------------------------------------------
static std::vector<std::wstring> sToWStrings(const IVectorView<winrt::hstring>& strings)
//----------------------------------------------------------------------------------------------------------------------
{
	// Convert
	std::vector<std::wstring>	result;
	if (strings != nullptr) {
		// Iterate
		result.reserve(strings.Size());
		for (const winrt::hstring& string : strings)
			// Add
			result.push_back(std::wstring(string));
	}

	return result;
}

//----------------------------------------------------------------------------------------------------------------------
static bool sDoesContain(const std::vector<std::wstring>& strings, const std::wstring& string)
//----------------------------------------------------------------------------------------------------------------------
{
	return std::find(strings.begin(), strings.end(), string) != strings.end();
}

//----------------------------------------------------------------------------------------------------------------------
static std::wstring sToLowercased(const std::wstring& string)
//----------------------------------------------------------------------------------------------------------------------
{
	// Lowercase
	std::wstring	lowercased(string);
	std::transform(lowercased.begin(), lowercased.end(), lowercased.begin(),
			[](wchar_t character){ return (wchar_t) std::towlower(character); });

	return lowercased;
}

//----------------------------------------------------------------------------------------------------------------------
static IReference<uint32_t> sToUInt32Reference(const std::optional<uint32_t>& value)
//----------------------------------------------------------------------------------------------------------------------
{
	return value.has_value() ? winrt::box_value(*value).as<IReference<uint32_t> >() : nullptr;
}

//----------------------------------------------------------------------------------------------------------------------
static Brush sBrushFor(const wchar_t* key, const wchar_t* fallbackKey = nullptr)
//----------------------------------------------------------------------------------------------------------------------
{
	// Look up the theme brush
	auto			resources = Application::Current().Resources();
	IInspectable	object = resources.TryLookup(winrt::box_value(winrt::hstring(key)));
	Brush			brush = (object != nullptr) ? object.try_as<Brush>() : nullptr;
	if ((brush == nullptr) && (fallbackKey != nullptr)) {
		// Try fallback
		object = resources.TryLookup(winrt::box_value(winrt::hstring(fallbackKey)));
		brush = (object != nullptr) ? object.try_as<Brush>() : nullptr;
	}

	if (brush == nullptr)
		// Nothing found
		brush = SolidColorBrush(winrt::Microsoft::UI::Colors::Transparent());

	return brush;
}

//----------------------------------------------------------------------------------------------------------------------
static bool sIsVirtualKeyDown(VirtualKey virtualKey)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check state
	CoreVirtualKeyStates	state = InputKeyboardSource::GetKeyStateForCurrentThread(virtualKey);

	return (static_cast<uint32_t>(state) & static_cast<uint32_t>(CoreVirtualKeyStates::Down)) != 0;
}

//----------------------------------------------------------------------------------------------------------------------
static bool sDoesVirtualKeyModifiersContain(const VirtualKeyModifiers& virtualKeyModifiers,
		VirtualKeyModifiers virtualKeyModifier)
//----------------------------------------------------------------------------------------------------------------------
{
	return (static_cast<uint32_t>(virtualKeyModifiers) & static_cast<uint32_t>(virtualKeyModifier)) != 0;
}

//----------------------------------------------------------------------------------------------------------------------
static void sEnsureCanvasDoesContainUIElement(const Canvas& canvas, const UIElement& element)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if already a child
	uint32_t	index;
	if (!canvas.Children().IndexOf(element, index))
		// Add
		canvas.Children().Append(element);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - OutlineView::Internals

class OutlineView::Internals {
	// MARK: Column
	public:
		struct Column {
			// Lifecycle methods
			Column(const OutlineViewColumn& info) :
				mInfo(info), mX(0.0), mWidth(0.0),
						mHeaderCell(nullptr), mTitleTextBlock(nullptr), mSortFontIcon(nullptr)
				{}

			// Properties
			OutlineViewColumn	mInfo;
			double				mX;
			double				mWidth;

			Border				mHeaderCell;
			TextBlock			mTitleTextBlock;
			FontIcon			mSortFontIcon;
		};

	// MARK: RowEntry
	public:
		struct RowEntry {
			// Lifecycle methods
			RowEntry() : mLevel(0), mHasChildren(false), mIsExpanded(false) {}

			// Properties
			std::wstring				mIdentifier;
			std::optional<std::wstring>	mParentIdentifier;
			uint32_t					mLevel;
			bool						mHasChildren;
			bool						mIsExpanded;
		};

	// MARK: CellSlot
	public:
		struct CellSlot {
			// Lifecycle methods
			CellSlot(const std::wstring& columnIdentifier) : mColumnIdentifier(columnIdentifier), mCell(nullptr) {}

			// Properties
			std::wstring								mColumnIdentifier;
			winrt::WinUIToolbox::OutlineViewCell	mCell;
		};

	// MARK: RealizedRow
	public:
		struct RealizedRow {
			// Lifecycle methods
			RealizedRow() :
				mRow(0), mLevel(0), mHasChildren(false), mIsExpanded(false), mIsFullWidth(false),
						mBackground(nullptr), mSelectionIndicator(nullptr), mChevron(nullptr)
				{}

			// Properties
			uint32_t				mRow;
			std::wstring			mID;
			uint32_t				mLevel;
			bool					mHasChildren;
			bool					mIsExpanded;
			bool					mIsFullWidth;

			Shapes::Rectangle		mBackground;
			Shapes::Rectangle		mSelectionIndicator;
			FontIcon				mChevron;
			std::vector<CellSlot>	mCellSlots;
		};

	// Methods
	public:
														// Lifecycle methods
														Internals(OutlineView& outlineView);
														~Internals();

														// Instance methods - projected view
				winrt::WinUIToolbox::OutlineView		getProjectedOutlineView();

														// Instance methods - setup
				void									setDataSource(const IInspectable& dataSource);

														// Instance methods - view chrome
				void									buildChrome();

														// Instance methods - columns
				std::vector<OutlineViewColumn>			getOutlineViewColumns() const;
				void									buildHeaderCell(Column& column);
				void									setOutlineViewColumns(
																const std::vector<OutlineViewColumn>&
																		outlineViewColumns);
				void									moveColumn(uint32_t fromIndex, uint32_t toIndex);
				void									updateColumn(const OutlineViewColumn& outlineViewColumn);
				void									reloadColumn(const std::wstring& identifier);

														// Instance methods - layout
				double									getBodyHeight() const;
				double									getColumnHeaderHeight() const;
				double									getViewportRows() const;
				uint32_t								getRowCount() const;
				uint32_t								getFirstRow() const;
				double									getSubRowOffset() const;
				double									getRowY(uint32_t row) const;

				void									viewportSizeChanged(double width, double height);
				void									layout();
				void									setHOffset(double offset);
				void									updateVScrollBar();
				void									scrollTo(double scrollRow);
				void									scrollRowIntoView(uint32_t row);

														// Instance methods - tree
				std::optional<std::wstring>				identifierForRow(const std::optional<uint32_t>& row) const;
				std::optional<uint32_t>					rowForIdentifier(const std::optional<std::wstring>& identifier)
																const;
				std::optional<uint32_t>					getParentRow(uint32_t row) const;
				void									appendChildRows(const std::optional<std::wstring>& identifier,
																uint32_t level);
				void									buildRows();
				void									reloadTree();
				void									setExpanded(const std::wstring& identifier, bool expanded,
																bool includeChildren);
				void									setItemExpanded(const std::wstring& identifier, bool expanded,
																bool includeChildren);
				void									toggleExpanded(uint32_t row, bool includeChildren);

														// Instance methods - rows
				void									realize();
				RealizedRow*							findRealizedRow(uint32_t row);
				RealizedRow								realizeRow(uint32_t row);
				void									populateCells(RealizedRow& realizedRow);
				void									recycleCell(
																const winrt::WinUIToolbox::OutlineViewCell&
																		outlineViewCell);
				void									recycleCells(RealizedRow& realizedRow);
				void									recycleRow(RealizedRow& realizedRow);
				void									recycleAllRows();
				void									reloadRow(RealizedRow& realizedRow,
																const std::optional<std::vector<std::wstring> >&
																		columnIdentifiers);
				double									getChevronX(const RealizedRow& realizedRow) const;
				double									getFirstColumnIndent(const RealizedRow& realizedRow) const;
				void									layoutRow(RealizedRow& realizedRow);
				void									placeCell(const UIElement& element, double x, double width,
																double y);
				void									updateRowVisuals(RealizedRow& realizedRow);
				void									updateAllRowVisuals();
				void									updateFocusRect();
				void									updateSortIndicators();
				Shapes::Rectangle						dequeueRectangle(std::vector<Shapes::Rectangle>& pool);
				UIElement								tryGetRecycledElement(const std::wstring& recycleKey);

														// Instance methods - state
				void									setHoveredRow(const std::optional<uint32_t>& row);
				void									setFocusedRow(const std::optional<uint32_t>& row);

														// Instance methods - hit testing
				bool									isInHeader(const Point& point) const;
				bool									isInBody(const Point& point) const;
				bool									isInEditor(const Point& point) const;
				std::optional<uint32_t>					getRowAt(const Point& point) const;
				int										getColumnIndexAt(double contentX) const;
				int										getColumnIndex(const std::wstring& identifier) const;
				int										getDividerIndexAt(double contentX) const;
				bool									isInChevron(const RealizedRow& realizedRow, double contentX)
																const;
				std::optional<std::wstring>				getColumnIdentifierAt(double contentX) const;

														// Instance methods - pointer
				void									pointerPressed(const Point& point,
																PointerHelper::Buttons buttons, int pressedCount,
																const VirtualKeyModifiers& virtualKeyModifiers);
				void									pointerDragged(const Point& point,
																const VirtualKeyModifiers& virtualKeyModifiers);
				void									pointerReleased(const Point& point);
				void									pointerMoved(const Point& point);
				void									updateResizeCursor(bool show);
				int										getFirstMovableColumnIndex() const;
				bool									canMoveColumn(int columnIndex) const;

														// Instance methods - sorting
				std::vector<SortDescriptor>				getSortDescriptorsForHeaderClick(const std::wstring& identifier)
																const;
				IVectorView<OutlineViewSortDescription>	getSortDescriptionsView() const;
				void									applySortDescriptors(
																const std::vector<SortDescriptor>& sortDescriptors);

														// Instance methods - selection
				bool									isRowSelected(uint32_t row) const;
				std::vector<std::wstring>				getSelectedIdentifiersInRowOrder() const;
				void									selectionChanged();
				void									setSelectedRows(const std::vector<uint32_t>& rows);
				void									selectRows(const std::vector<uint32_t>& rows);
				void									deselectRows(const std::vector<uint32_t>& rows);
				void									setSelectedIdentifiers(
																const std::vector<std::wstring>& identifiers);
				void									selectIdentifiers(const std::vector<std::wstring>& identifiers,
																bool byExtending);
				void									deselectIdentifiers(
																const std::vector<std::wstring>& identifiers);
				void									selectAllInternal();
				void									deselectAllInternal();
				void									selectRowFromPointer(uint32_t row, bool toggle, bool extend);
				void									selectRange(uint32_t row1, uint32_t row2);
				void									moveFocus(uint32_t row, bool extend, bool focusOnly);

														// Instance methods - keyboard
				bool									keyDown(VirtualKey virtualKey);
				void									characterReceived(char16_t character);

														// Instance methods - editing
				bool									beginEdit(uint32_t row,
																const std::wstring& columnIdentifier);
				void									beginEditFirstEditableColumn(uint32_t row);
				void									endEdit(bool commit, OutlineViewEditEndReason reason);
				void									continueEdit(uint32_t row,
																const std::wstring& columnIdentifier, bool forward);

														// Instance methods - menus
				void									contextRequested(const std::optional<Point>& point);

	// Properties
	public:
		OutlineView&													mOutlineView;

		IInspectable													mDataSourceObject;
		DataSource*														mDataSource;
		IOutlineViewCellFactory											mCellFactory;

		winrt::event<OutlineViewItemEventHandler>						mItemExpandedEvent;
		winrt::event<OutlineViewItemEventHandler>						mItemCollapsedEvent;
		winrt::event<OutlineViewItemInvokedEventHandler>				mItemInvokedEvent;
		winrt::event<OutlineViewSelectionChangedEventHandler>			mSelectionChangedEvent;
		winrt::event<OutlineViewSortDescriptionsChangedEventHandler>	mSortDescriptionsChangedEvent;
		winrt::event<OutlineViewColumnReorderedEventHandler>			mColumnReorderedEvent;
		winrt::event<OutlineViewColumnEventHandler>						mColumnResizedEvent;
		winrt::event<OutlineViewBeginningEditEventHandler>				mBeginningEditEvent;
		winrt::event<OutlineViewEditEndedEventHandler>					mEditEndedEvent;
		winrt::event<OutlineViewContextMenuOpeningEventHandler>			mContextMenuOpeningEvent;

		OutlineViewStyle												mOutlineStyle;
		OutlineViewSelectionMode										mSelectionMode;
		bool															mCanUserReorderColumns;
		bool															mCanUserResizeColumns;
		double															mRowHeight;
		double															mColumnHeaderHeight;

		Border															mClipBorder;
		Grid															mContentGrid;
		TranslateTransform												mTranslateTransform;
		Canvas															mRowBackgroundsCanvas;
		Canvas															mRowContentCanvas;
		Canvas															mHeaderCanvas;
		Canvas															mOverlayCanvas;
		ScrollBar														mVScrollBar;
		ScrollBar														mHScrollBar;
		Shapes::Rectangle												mHeaderBackground;
		Shapes::Rectangle												mHeaderBottomLine;
		Shapes::Rectangle												mFocusRect;
		Shapes::Rectangle												mDragIndicator;
		Shapes::Rectangle												mResizeLine;

		Brush															mHeaderBackgroundBrush;
		Brush															mHeaderTextBrush;
		Brush															mDividerBrush;
		Brush															mTextBrush;
		Brush															mSelectedBrush;
		Brush															mHoverBrush;
		Brush															mSelectedHoverBrush;
		Brush															mAccentBrush;
		Brush															mTransparentBrush;

		double															mViewportWidth;
		double															mViewportHeight;
		double															mGutterWidth;
		double															mTotalWidth;
		double															mContentWidth;
		double															mHOffset;
		double															mScrollRow;
		bool															mIsUpdatingScrollBars;
		bool															mHasFocus;

		std::vector<Column>												mColumns;
		std::vector<RowEntry>											mRowEntries;
		std::unordered_map<std::wstring, uint32_t>						mRowByIdentifier;
		std::set<std::wstring>											mExpandedIdentifiers;
		std::unordered_set<std::wstring>								mSelectedIdentifiers;
		std::vector<SortDescriptor>										mSortDescriptors;

		std::vector<RealizedRow>										mRealizedRows;
		std::vector<Shapes::Rectangle>									mBackgroundPool;
		std::vector<Shapes::Rectangle>									mIndicatorPool;
		std::vector<FontIcon>											mChevronPool;
		std::map<std::wstring, std::vector<UIElement> >					mRecycledElementsByKey;

		std::optional<uint32_t>											mFocusedRow;
		std::optional<uint32_t>											mAnchorRow;
		std::optional<uint32_t>											mHoveredRow;
		std::optional<uint32_t>											mClickedRow;
		std::optional<std::wstring>										mClickedColumnIdentifier;

		PointerHelper*													mPointerHelper;
		int																mPressedHeaderColumnIndex;
		double															mPressedX;
		bool															mIsDraggingColumn;
		int																mDropColumnIndex;
		int																mResizingColumnIndex;
		double															mResizingStartWidth;
		bool															mIsShowingResizeCursor;
		InputCursor														mResizeCursor;

		TextBox															mEditTextBox;
		std::optional<uint32_t>											mEditingRow;
		std::optional<std::wstring>										mEditingColumnIdentifier;
		bool															mIsEndingEditing;

		std::wstring													mTextSearchString;
		std::chrono::steady_clock::time_point							mTextSearchTime;
};

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
OutlineView::Internals::Internals(OutlineView& outlineView) :
		mOutlineView(outlineView),
				mDataSourceObject(nullptr), mDataSource(nullptr), mCellFactory(nullptr),
				mOutlineStyle(OutlineViewStyle::FirstColumn), mSelectionMode(OutlineViewSelectionMode::Extended),
				mCanUserReorderColumns(true), mCanUserResizeColumns(true), mRowHeight(26.0), mColumnHeaderHeight(32.0),
				mClipBorder(nullptr), mContentGrid(nullptr), mTranslateTransform(nullptr),
				mRowBackgroundsCanvas(nullptr), mRowContentCanvas(nullptr), mHeaderCanvas(nullptr),
				mOverlayCanvas(nullptr), mVScrollBar(nullptr), mHScrollBar(nullptr), mHeaderBackground(nullptr),
				mHeaderBottomLine(nullptr), mFocusRect(nullptr), mDragIndicator(nullptr), mResizeLine(nullptr),
				mHeaderBackgroundBrush(nullptr), mHeaderTextBrush(nullptr), mDividerBrush(nullptr),
				mTextBrush(nullptr), mSelectedBrush(nullptr), mHoverBrush(nullptr), mSelectedHoverBrush(nullptr),
				mAccentBrush(nullptr), mTransparentBrush(nullptr),
				mViewportWidth(0.0), mViewportHeight(0.0), mGutterWidth(0.0), mTotalWidth(0.0), mContentWidth(0.0),
				mHOffset(0.0), mScrollRow(0.0), mIsUpdatingScrollBars(false), mHasFocus(false),
				mPointerHelper(nullptr), mPressedHeaderColumnIndex(-1), mPressedX(0.0), mIsDraggingColumn(false),
				mDropColumnIndex(-1), mResizingColumnIndex(-1), mResizingStartWidth(0.0), mIsShowingResizeCursor(false),
				mResizeCursor(nullptr),
				mEditTextBox(nullptr), mIsEndingEditing(false)
//----------------------------------------------------------------------------------------------------------------------
{}

//----------------------------------------------------------------------------------------------------------------------
OutlineView::Internals::~Internals()
//----------------------------------------------------------------------------------------------------------------------
{
	// Cleanup
	delete mPointerHelper;
}

// MARK: Instance methods - projected view

//----------------------------------------------------------------------------------------------------------------------
winrt::WinUIToolbox::OutlineView OutlineView::Internals::getProjectedOutlineView()
//----------------------------------------------------------------------------------------------------------------------
{
	return mOutlineView.get_strong().as<winrt::WinUIToolbox::OutlineView>();
}

// MARK: Instance methods - setup

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::setDataSource(const IInspectable& dataSource)
//----------------------------------------------------------------------------------------------------------------------
{
	// Cleanup
	endEdit(false, OutlineViewEditEndReason::Other);
	recycleAllRows();
	mRowContentCanvas.Children().Clear();
	mRecycledElementsByKey.clear();
	mChevronPool.clear();

	// Set data source
	mDataSourceObject = dataSource;
	if (dataSource != nullptr) {
		// Carried across as Object, boxed by OutlineViewDataSourceRInspectable
		auto	wrapper = dataSource.try_as<OutlineViewDataSourceRInspectable>();
		mDataSource = (wrapper != nullptr) ? &wrapper->GetReference() : nullptr;
	} else
		// Cleared
		mDataSource = nullptr;

	// Reset
	mExpandedIdentifiers.clear();
	mSelectedIdentifiers.clear();
	mFocusedRow = std::optional<uint32_t>();
	mAnchorRow = std::optional<uint32_t>();
	mHoveredRow = std::optional<uint32_t>();
	mClickedRow = std::optional<uint32_t>();
	mClickedColumnIdentifier = std::optional<std::wstring>();
	mScrollRow = 0.0;
	mRowEntries.clear();
	mRowByIdentifier.clear();

	// Setup UI
	updateVScrollBar();
	realize();
}

// MARK: Instance methods - view chrome

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::buildChrome()
//----------------------------------------------------------------------------------------------------------------------
{
	// Brushes
	mHeaderBackgroundBrush = sBrushFor(L"LayerFillColorDefaultBrush", L"SolidBackgroundFillColorSecondaryBrush");
	mHeaderTextBrush = sBrushFor(L"TextFillColorSecondaryBrush");
	mDividerBrush = sBrushFor(L"DividerStrokeColorDefaultBrush");
	mTextBrush = sBrushFor(L"TextFillColorPrimaryBrush");
	mSelectedBrush = sBrushFor(L"ListViewItemBackgroundSelected", L"SubtleFillColorSecondaryBrush");
	mHoverBrush = sBrushFor(L"ListViewItemBackgroundPointerOver", L"SubtleFillColorSecondaryBrush");
	mSelectedHoverBrush = sBrushFor(L"ListViewItemBackgroundSelectedPointerOver", L"SubtleFillColorTertiaryBrush");
	mAccentBrush = sBrushFor(L"AccentFillColorDefaultBrush");
	mTransparentBrush = SolidColorBrush(winrt::Microsoft::UI::Colors::Transparent());

	// Grid: content | vertical scrollbar
	//       horizontal scrollbar | corner
	RowDefinition	rowDefinition0;
	rowDefinition0.Height(GridLengthHelper::FromValueAndType(1.0, GridUnitType::Star));
	mOutlineView.RowDefinitions().Append(rowDefinition0);

	RowDefinition	rowDefinition1;
	rowDefinition1.Height(GridLengthHelper::Auto());
	mOutlineView.RowDefinitions().Append(rowDefinition1);

	ColumnDefinition	columnDefinition0;
	columnDefinition0.Width(GridLengthHelper::FromValueAndType(1.0, GridUnitType::Star));
	mOutlineView.ColumnDefinitions().Append(columnDefinition0);

	ColumnDefinition	columnDefinition1;
	columnDefinition1.Width(GridLengthHelper::Auto());
	mOutlineView.ColumnDefinitions().Append(columnDefinition1);

	// Content - a clipped border holding a grid the size of the content,
	//	slid left and right by a transform for horizontal scrolling
	mTranslateTransform = TranslateTransform();

	mContentGrid = Grid();
	mContentGrid.HorizontalAlignment(HorizontalAlignment::Left);
	mContentGrid.VerticalAlignment(VerticalAlignment::Top);
	mContentGrid.RenderTransform(mTranslateTransform);

	mRowBackgroundsCanvas = Canvas();
	mRowContentCanvas = Canvas();
	mHeaderCanvas = Canvas();
	mOverlayCanvas = Canvas();
	mContentGrid.Children().Append(mRowBackgroundsCanvas);
	mContentGrid.Children().Append(mRowContentCanvas);
	mContentGrid.Children().Append(mHeaderCanvas);
	mContentGrid.Children().Append(mOverlayCanvas);

	mClipBorder = Border();
	mClipBorder.Child(mContentGrid);
	mClipBorder.Background(mTransparentBrush);	// Takes pointer events
	Grid::SetRow(mClipBorder, 0);
	Grid::SetColumn(mClipBorder, 0);
	mOutlineView.Children().Append(mClipBorder);

	// Header chrome
	mHeaderBackground = Shapes::Rectangle();
	mHeaderBackground.Fill(mHeaderBackgroundBrush);
	mHeaderBackground.IsHitTestVisible(false);
	mHeaderCanvas.Children().Append(mHeaderBackground);

	mHeaderBottomLine = Shapes::Rectangle();
	mHeaderBottomLine.Fill(mDividerBrush);
	mHeaderBottomLine.Height(1.0);
	mHeaderBottomLine.IsHitTestVisible(false);
	mHeaderCanvas.Children().Append(mHeaderBottomLine);

	// Overlays
	mFocusRect = Shapes::Rectangle();
	mFocusRect.Stroke(mAccentBrush);
	mFocusRect.StrokeThickness(1.0);
	mFocusRect.IsHitTestVisible(false);
	mFocusRect.Visibility(Visibility::Collapsed);
	mOverlayCanvas.Children().Append(mFocusRect);

	mDragIndicator = Shapes::Rectangle();
	mDragIndicator.Fill(mAccentBrush);
	mDragIndicator.Width(2.0);
	mDragIndicator.IsHitTestVisible(false);
	mDragIndicator.Visibility(Visibility::Collapsed);
	mOverlayCanvas.Children().Append(mDragIndicator);

	mResizeLine = Shapes::Rectangle();
	mResizeLine.Fill(mAccentBrush);
	mResizeLine.Width(1.0);
	mResizeLine.IsHitTestVisible(false);
	mResizeLine.Visibility(Visibility::Collapsed);
	mOverlayCanvas.Children().Append(mResizeLine);

	// Scrollbars.  The vertical one works in rows, so its range is never large.
	mVScrollBar = ScrollBar();
	mVScrollBar.Orientation(Orientation::Vertical);
	mVScrollBar.IndicatorMode(ScrollingIndicatorMode::MouseIndicator);
	mVScrollBar.Minimum(0.0);
	mVScrollBar.SmallChange(1.0);
	mOutlineView.Children().Append(mVScrollBar);

	Grid::SetRow(mVScrollBar, 0);
	Grid::SetColumn(mVScrollBar, 1);

	mHScrollBar = ScrollBar();
	mHScrollBar.Orientation(Orientation::Horizontal);
	mHScrollBar.IndicatorMode(ScrollingIndicatorMode::MouseIndicator);
	mHScrollBar.Minimum(0.0);
	mHScrollBar.SmallChange(kWheelPixelsPerNotch);
	mHScrollBar.Visibility(Visibility::Collapsed);
	mOutlineView.Children().Append(mHScrollBar);

	Grid::SetRow(mHScrollBar, 1);
	Grid::SetColumn(mHScrollBar, 0);

	// Events
	mClipBorder.SizeChanged([this](const IInspectable& sender, const SizeChangedEventArgs& sizeChangedEventArgs){
		// Update
		viewportSizeChanged(mClipBorder.ActualWidth(), mClipBorder.ActualHeight());
	});
	mVScrollBar.ValueChanged([this](const IInspectable& sender, const RangeBaseValueChangedEventArgs& args){
		// Check if us
		if (mIsUpdatingScrollBars)
			return;

		// Scroll
		mScrollRow = args.NewValue();
		realize();
	});
	mHScrollBar.ValueChanged([this](const IInspectable& sender, const RangeBaseValueChangedEventArgs& args){
		// Check if us
		if (mIsUpdatingScrollBars)
			return;

		// Scroll
		setHOffset(args.NewValue());
	});

	// Pointer
	mPointerHelper = new PointerHelper(mOutlineView);
	mPointerHelper->setPressedProc(
			[this](const Point& point, PointerHelper::Buttons buttons, int pressedCount,
					const VirtualKeyModifiers& virtualKeyModifiers){
				// Handle
				pointerPressed(point, buttons, pressedCount, virtualKeyModifiers);
			});
	mPointerHelper->setDraggedProc(
			[this](const Point& point, bool isInitialDragEvent, PointerHelper::Buttons buttons, int pressedCount,
					const VirtualKeyModifiers& virtualKeyModifiers){
				// Handle
				pointerDragged(point, virtualKeyModifiers);
			});
	mPointerHelper->setReleasedProc(
			[this](const Point& point, PointerHelper::Buttons buttons, int pressedCount,
					PointerHelper::ReleasedReason releasedReason, const VirtualKeyModifiers& virtualKeyModifiers){
				// Handle
				pointerReleased(point);
			});
	mPointerHelper->setMovedProc([this](const Point& point, const VirtualKeyModifiers& virtualKeyModifiers){
		// Handle
		pointerMoved(point);
	});
	mPointerHelper->setExitedProc([this](const Point& point, const VirtualKeyModifiers& virtualKeyModifiers){
		// Handle
		setHoveredRow(std::optional<uint32_t>());
		updateResizeCursor(false);
	});
}

// MARK: Instance methods - columns

//----------------------------------------------------------------------------------------------------------------------
std::vector<OutlineViewColumn> OutlineView::Internals::getOutlineViewColumns() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Collect
	std::vector<OutlineViewColumn>	outlineViewColumns;
	outlineViewColumns.reserve(mColumns.size());
	for (const Column& column : mColumns)
		// Add
		outlineViewColumns.push_back(column.mInfo);

	return outlineViewColumns;
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::buildHeaderCell(Column& column)
//----------------------------------------------------------------------------------------------------------------------
{
	// Title
	column.mTitleTextBlock = TextBlock();
	column.mTitleTextBlock.Text(column.mInfo.Title);
	column.mTitleTextBlock.Foreground(mHeaderTextBrush);
	column.mTitleTextBlock.FontSize(12.0);
	column.mTitleTextBlock.TextTrimming(TextTrimming::CharacterEllipsis);
	column.mTitleTextBlock.TextWrapping(TextWrapping::NoWrap);
	column.mTitleTextBlock.VerticalAlignment(VerticalAlignment::Center);
	column.mTitleTextBlock.Margin(ThicknessHelper::FromLengths(8.0, 0.0, 18.0, 0.0));
	column.mTitleTextBlock.IsHitTestVisible(false);

	column.mSortFontIcon = FontIcon();
	column.mSortFontIcon.FontSize(9.0);
	column.mSortFontIcon.Foreground(mHeaderTextBrush);
	column.mSortFontIcon.HorizontalAlignment(HorizontalAlignment::Right);
	column.mSortFontIcon.VerticalAlignment(VerticalAlignment::Center);
	column.mSortFontIcon.Margin(ThicknessHelper::FromLengths(0.0, 0.0, 6.0, 0.0));
	column.mSortFontIcon.IsHitTestVisible(false);

	Grid	grid;
	grid.Children().Append(column.mTitleTextBlock);
	grid.Children().Append(column.mSortFontIcon);

	column.mHeaderCell = Border();
	column.mHeaderCell.Background(mTransparentBrush);
	column.mHeaderCell.BorderBrush(mDividerBrush);
	column.mHeaderCell.BorderThickness(ThicknessHelper::FromLengths(0.0, 0.0, 1.0, 0.0));
	column.mHeaderCell.Child(grid);
	column.mHeaderCell.IsHitTestVisible(false);
	mHeaderCanvas.Children().Append(column.mHeaderCell);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::setOutlineViewColumns(const std::vector<OutlineViewColumn>& outlineViewColumns)
//----------------------------------------------------------------------------------------------------------------------
{
	// Old header cells go
	for (Column& column : mColumns) {
		// Remove
		uint32_t	index;
		if (mHeaderCanvas.Children().IndexOf(column.mHeaderCell, index))
			// Remove
			mHeaderCanvas.Children().RemoveAt(index);
	}
	mColumns.clear();

	// Compose columns
	for (const OutlineViewColumn& outlineViewColumn : outlineViewColumns) {
		// Setup
		Column	column(outlineViewColumn);
		buildHeaderCell(column);
		mColumns.push_back(column);
	}

	// Realized rows are laid out for the old columns
	recycleAllRows();
	updateSortIndicators();
	layout();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::moveColumn(uint32_t fromIndex, uint32_t toIndex)
//----------------------------------------------------------------------------------------------------------------------
{
	// Preflight
	if ((fromIndex >= mColumns.size()) || (toIndex >= mColumns.size()) || (fromIndex == toIndex))
		return;

	// Reorder
	winrt::hstring					identifier = mColumns[fromIndex].mInfo.Identifier;
	std::vector<OutlineViewColumn>	outlineViewColumns = getOutlineViewColumns();
	OutlineViewColumn				moved = outlineViewColumns[fromIndex];
	outlineViewColumns.erase(outlineViewColumns.begin() + fromIndex);
	outlineViewColumns.insert(outlineViewColumns.begin() + toIndex, moved);
	setOutlineViewColumns(outlineViewColumns);

	// Raise event
	if (mColumnReorderedEvent)
		mColumnReorderedEvent(getProjectedOutlineView(),
				winrt::make<OutlineViewColumnReorderedEventArgs>(identifier, fromIndex, toIndex));
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::updateColumn(const OutlineViewColumn& outlineViewColumn)
//----------------------------------------------------------------------------------------------------------------------
{
	// Find
	int	columnIndex = getColumnIndex(std::wstring(outlineViewColumn.Identifier));
	if (columnIndex < 0)
		return;

	// Update in place
	Column&	column = mColumns[columnIndex];
	if (column.mInfo.Title != outlineViewColumn.Title)
		// Title
		column.mTitleTextBlock.Text(outlineViewColumn.Title);
	column.mInfo = outlineViewColumn;

	// Layout
	layout();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::reloadColumn(const std::wstring& identifier)
//----------------------------------------------------------------------------------------------------------------------
{
	// Reload this column in every realized row
	std::optional<std::vector<std::wstring> >	columnIdentifiers(std::vector<std::wstring>(1, identifier));
	for (RealizedRow& realizedRow : mRealizedRows)
		// Reload
		reloadRow(realizedRow, columnIdentifiers);
}

// MARK: Instance methods - layout

//----------------------------------------------------------------------------------------------------------------------
double OutlineView::Internals::getBodyHeight() const
//----------------------------------------------------------------------------------------------------------------------
{
	return std::max(mViewportHeight - getColumnHeaderHeight(), 0.0);
}

//----------------------------------------------------------------------------------------------------------------------
double OutlineView::Internals::getColumnHeaderHeight() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mColumnHeaderHeight;
}

//----------------------------------------------------------------------------------------------------------------------
double OutlineView::Internals::getViewportRows() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (mRowHeight > 0.0) ? (getBodyHeight() / mRowHeight) : 0.0;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t OutlineView::Internals::getRowCount() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (uint32_t) mRowEntries.size();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t OutlineView::Internals::getFirstRow() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (uint32_t) std::floor(mScrollRow);
}

//----------------------------------------------------------------------------------------------------------------------
double OutlineView::Internals::getSubRowOffset() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (mScrollRow - std::floor(mScrollRow)) * mRowHeight;
}

//----------------------------------------------------------------------------------------------------------------------
double OutlineView::Internals::getRowY(uint32_t row) const
//----------------------------------------------------------------------------------------------------------------------
{
	return getColumnHeaderHeight() + ((double) row - (double) getFirstRow()) * mRowHeight - getSubRowOffset();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::viewportSizeChanged(double width, double height)
//----------------------------------------------------------------------------------------------------------------------
{
	// Store
	mViewportWidth = width;
	mViewportHeight = height;

	// Clip
	RectangleGeometry	rectangleGeometry;
	rectangleGeometry.Rect(Rect{0.0f, 0.0f, (float) width, (float) height});
	mClipBorder.Clip(rectangleGeometry);

	// Layout
	layout();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::layout()
//----------------------------------------------------------------------------------------------------------------------
{
	// Columns
	mGutterWidth = (mOutlineStyle == OutlineViewStyle::Gutter) ? kGutterWidth : 0.0;

	double	x = mGutterWidth;
	for (Column& column : mColumns) {
		// Place
		column.mX = x;
		column.mWidth = std::max(column.mInfo.Width, 0.0);
		x += column.mWidth;
	}
	mTotalWidth = x;
	mContentWidth = std::max(mTotalWidth, mViewportWidth);

	// Content grid
	mContentGrid.Width(mContentWidth);
	mContentGrid.Height(std::max(mViewportHeight, 0.0));

	// Header
	mHeaderCanvas.Visibility((getColumnHeaderHeight() > 0.0) ? Visibility::Visible : Visibility::Collapsed);
	mHeaderBackground.Width(mContentWidth);
	mHeaderBackground.Height(getColumnHeaderHeight());
	mHeaderBottomLine.Width(mContentWidth);
	Canvas::SetTop(mHeaderBottomLine, std::max(getColumnHeaderHeight() - 1.0, 0.0));
	for (Column& column : mColumns) {
		// Place header cell
		Canvas::SetLeft(column.mHeaderCell, column.mX);
		Canvas::SetTop(column.mHeaderCell, 0.0);
		column.mHeaderCell.Width(column.mWidth);
		column.mHeaderCell.Height(getColumnHeaderHeight());
	}

	// Horizontal scrollbar
	double	hMaximum = std::max(mTotalWidth - mViewportWidth, 0.0);
	mIsUpdatingScrollBars = true;
	mHScrollBar.Maximum(hMaximum);
	mHScrollBar.ViewportSize(mViewportWidth);
	mHScrollBar.LargeChange(mViewportWidth);
	mHScrollBar.Visibility((hMaximum > 0.0) ? Visibility::Visible : Visibility::Collapsed);
	mIsUpdatingScrollBars = false;
	setHOffset(mHOffset);

	// Rows
	updateVScrollBar();
	realize();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::setHOffset(double offset)
//----------------------------------------------------------------------------------------------------------------------
{
	// Clamp
	double	hMaximum = std::max(mTotalWidth - mViewportWidth, 0.0);
	mHOffset = std::min(std::max(offset, 0.0), hMaximum);

	// Update
	mIsUpdatingScrollBars = true;
	mHScrollBar.Value(mHOffset);
	mIsUpdatingScrollBars = false;

	mTranslateTransform.X(-mHOffset);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::updateVScrollBar()
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	double	viewportRows = getViewportRows();
	double	maximum = std::max((double) getRowCount() - viewportRows, 0.0);
	mScrollRow = std::min(std::max(mScrollRow, 0.0), maximum);

	// Update
	mIsUpdatingScrollBars = true;
	mVScrollBar.Maximum(maximum);
	mVScrollBar.ViewportSize(viewportRows);
	mVScrollBar.LargeChange(std::max(viewportRows, 1.0));
	mVScrollBar.Value(mScrollRow);
	mVScrollBar.Visibility((maximum > 0.0) ? Visibility::Visible : Visibility::Collapsed);
	mIsUpdatingScrollBars = false;
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::scrollTo(double scrollRow)
//----------------------------------------------------------------------------------------------------------------------
{
	// Update
	mScrollRow = scrollRow;

	// Update UI
	updateVScrollBar();
	realize();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::scrollRowIntoView(uint32_t row)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	double	viewportRows = getViewportRows();
	if (viewportRows <= 0.0)
		return;

	// Check where it is
	if ((double) row < mScrollRow)
		// Above
		scrollTo((double) row);
	else if (((double) row + 1.0) > (mScrollRow + viewportRows))
		// Below
		scrollTo((double) row + 1.0 - viewportRows);
}

// MARK: Instance methods - tree

//----------------------------------------------------------------------------------------------------------------------
std::optional<std::wstring> OutlineView::Internals::identifierForRow(const std::optional<uint32_t>& row) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check
	return (row.has_value() && (*row < mRowEntries.size())) ?
			std::optional<std::wstring>(mRowEntries[*row].mIdentifier) : std::optional<std::wstring>();
}

//----------------------------------------------------------------------------------------------------------------------
std::optional<uint32_t> OutlineView::Internals::rowForIdentifier(const std::optional<std::wstring>& identifier) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check
	if (!identifier.has_value())
		return std::optional<uint32_t>();

	// Look up
	auto	iterator = mRowByIdentifier.find(*identifier);

	return (iterator != mRowByIdentifier.end()) ? std::optional<uint32_t>(iterator->second) : std::optional<uint32_t>();
}

//----------------------------------------------------------------------------------------------------------------------
std::optional<uint32_t> OutlineView::Internals::getParentRow(uint32_t row) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check
	if (row >= mRowEntries.size())
		return std::optional<uint32_t>();

	return rowForIdentifier(mRowEntries[row].mParentIdentifier);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::appendChildRows(const std::optional<std::wstring>& identifier, uint32_t level)
//----------------------------------------------------------------------------------------------------------------------
{
	// Nothing without a data source
	if (mDataSource == nullptr)
		return;

	// Ask the data source for the children
	std::vector<std::wstring>	childIdentifiers = mDataSource->getChildIdentifiers(identifier);
	for (const std::wstring& childIdentifier : childIdentifiers) {
		// Setup
		RowEntry	rowEntry;
		rowEntry.mIdentifier = childIdentifier;
		rowEntry.mParentIdentifier = identifier;
		rowEntry.mLevel = level;
		rowEntry.mHasChildren = mDataSource->isExpandable(childIdentifier);
		rowEntry.mIsExpanded = rowEntry.mHasChildren && (mExpandedIdentifiers.count(childIdentifier) > 0);

		// Add
		mRowEntries.push_back(rowEntry);
		mRowByIdentifier[childIdentifier] = (uint32_t) (mRowEntries.size() - 1);

		// Descend
		if (rowEntry.mIsExpanded)
			// Recurse
			appendChildRows(std::optional<std::wstring>(childIdentifier), level + 1);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::buildRows()
//----------------------------------------------------------------------------------------------------------------------
{
	// Rebuild the flattened tree
	mRowEntries.clear();
	mRowByIdentifier.clear();
	appendChildRows(std::optional<std::wstring>(), 0);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::reloadTree()
//----------------------------------------------------------------------------------------------------------------------
{
	// Where the keyboard is, by identifier, so it can be found again
	std::optional<std::wstring>	focusedIdentifier = identifierForRow(mFocusedRow);
	std::optional<std::wstring>	anchorIdentifier = identifierForRow(mAnchorRow);
	std::optional<std::wstring>	clickedIdentifier = identifierForRow(mClickedRow);

	// Everything realized is stale
	recycleAllRows();

	// Rebuild
	buildRows();

	// Follow the rows to their new places
	mFocusedRow = rowForIdentifier(focusedIdentifier);
	mAnchorRow = rowForIdentifier(anchorIdentifier);
	mClickedRow = rowForIdentifier(clickedIdentifier);
	mHoveredRow = std::optional<uint32_t>();

	// Update
	updateVScrollBar();
	realize();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::setExpanded(const std::wstring& identifier, bool expanded, bool includeChildren)
//----------------------------------------------------------------------------------------------------------------------
{
	// Nothing without a data source
	if (mDataSource == nullptr)
		return;

	// Update the expanded set
	if (expanded) {
		// Only expandable items expand
		if (!mDataSource->isExpandable(identifier))
			return;
		mExpandedIdentifiers.insert(identifier);
	} else
		// Collapse
		mExpandedIdentifiers.erase(identifier);

	// Descend
	if (includeChildren) {
		// Iterate children
		std::vector<std::wstring>	childIdentifiers =
											mDataSource->getChildIdentifiers(std::optional<std::wstring>(identifier));
		for (const std::wstring& childIdentifier : childIdentifiers)
			// Check
			if (mDataSource->isExpandable(childIdentifier))
				// Recurse
				setExpanded(childIdentifier, expanded, true);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::setItemExpanded(const std::wstring& identifier, bool expanded, bool includeChildren)
//----------------------------------------------------------------------------------------------------------------------
{
	// Update the tree
	setExpanded(identifier, expanded, includeChildren);
	reloadTree();

	// Raise event
	if (expanded) {
		// Expanded
		if (mItemExpandedEvent)
			mItemExpandedEvent(getProjectedOutlineView(),
					winrt::make<OutlineViewItemEventArgs>(winrt::hstring(identifier)));
	} else if (mItemCollapsedEvent)
		// Collapsed
		mItemCollapsedEvent(getProjectedOutlineView(),
				winrt::make<OutlineViewItemEventArgs>(winrt::hstring(identifier)));
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::toggleExpanded(uint32_t row, bool includeChildren)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	if (row >= mRowEntries.size())
		return;

	RowEntry&	rowEntry = mRowEntries[row];
	if (!rowEntry.mHasChildren)
		return;

	// Toggle
	setItemExpanded(rowEntry.mIdentifier, !rowEntry.mIsExpanded, includeChildren);
}

// MARK: Instance methods - rows

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::realize()
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	uint32_t	rowCount = getRowCount();
	double		bodyHeight = getBodyHeight();
	uint32_t	firstRow = getFirstRow();
	uint32_t	realizeCount =
						(mRowHeight > 0.0) ? (uint32_t) std::ceil((bodyHeight + getSubRowOffset()) / mRowHeight) : 0;
	uint32_t	endRow = std::min(firstRow + realizeCount, rowCount);

	// Keep what is still showing, recycle what is not
	std::vector<RealizedRow>	realizedRows;
	for (RealizedRow& realizedRow : mRealizedRows) {
		// Check if still showing
		if ((realizedRow.mRow >= firstRow) && (realizedRow.mRow < endRow))
			// Keep
			realizedRows.push_back(realizedRow);
		else
			// Recycle
			recycleRow(realizedRow);
	}
	mRealizedRows.swap(realizedRows);

	// Realize what is missing
	for (uint32_t row = firstRow; row < endRow; row++) {
		// Check if have
		if (findRealizedRow(row) == nullptr)
			// Realize
			mRealizedRows.push_back(realizeRow(row));
	}

	// Place
	for (RealizedRow& realizedRow : mRealizedRows) {
		// Layout
		layoutRow(realizedRow);
		updateRowVisuals(realizedRow);
	}
	updateFocusRect();
}

//----------------------------------------------------------------------------------------------------------------------
OutlineView::Internals::RealizedRow* OutlineView::Internals::findRealizedRow(uint32_t row)
//----------------------------------------------------------------------------------------------------------------------
{
	// Look
	for (RealizedRow& realizedRow : mRealizedRows) {
		// Check row
		if (realizedRow.mRow == row)
			// Found
			return &realizedRow;
	}

	return nullptr;
}

//----------------------------------------------------------------------------------------------------------------------
OutlineView::Internals::RealizedRow OutlineView::Internals::realizeRow(uint32_t row)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	const	RowEntry&	rowEntry = mRowEntries[row];

	RealizedRow	realizedRow;
	realizedRow.mRow = row;
	realizedRow.mID = rowEntry.mIdentifier;
	realizedRow.mLevel = rowEntry.mLevel;
	realizedRow.mHasChildren = rowEntry.mHasChildren;
	realizedRow.mIsExpanded = rowEntry.mIsExpanded;

	// Background
	realizedRow.mBackground = dequeueRectangle(mBackgroundPool);
	realizedRow.mSelectionIndicator = dequeueRectangle(mIndicatorPool);
	realizedRow.mSelectionIndicator.Fill(mAccentBrush);
	realizedRow.mSelectionIndicator.Width(kSelectionIndicatorWidth);
	realizedRow.mSelectionIndicator.Height(kSelectionIndicatorHeight);
	realizedRow.mSelectionIndicator.RadiusX(kSelectionIndicatorWidth / 2.0);
	realizedRow.mSelectionIndicator.RadiusY(kSelectionIndicatorWidth / 2.0);

	Canvas::SetZIndex(realizedRow.mSelectionIndicator, 1);	// Above any background

	// Chevron
	if (realizedRow.mHasChildren && (mOutlineStyle != OutlineViewStyle::None)) {
		// Setup
		if (!mChevronPool.empty()) {
			// Reuse
			realizedRow.mChevron = mChevronPool.back();
			mChevronPool.pop_back();
		} else {
			// Create
			realizedRow.mChevron = FontIcon();
			realizedRow.mChevron.FontSize(12.0);
			realizedRow.mChevron.HorizontalAlignment(HorizontalAlignment::Center);
			realizedRow.mChevron.VerticalAlignment(VerticalAlignment::Center);
			realizedRow.mChevron.IsHitTestVisible(false);
			mRowContentCanvas.Children().Append(realizedRow.mChevron);
		}
		realizedRow.mChevron.Foreground(mTextBrush);
		realizedRow.mChevron.Visibility(Visibility::Visible);
	}

	// Cells
	populateCells(realizedRow);

	return realizedRow;
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::populateCells(RealizedRow& realizedRow)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	winrt::WinUIToolbox::OutlineView	projectedOutlineView = getProjectedOutlineView();

	// Full width first
	realizedRow.mIsFullWidth = false;
	if (mCellFactory != nullptr) {
		// Ask
		winrt::WinUIToolbox::OutlineViewCell	cellView =
															mCellFactory.GetRowCell(winrt::hstring(realizedRow.mID),
																	projectedOutlineView);
		if (cellView != nullptr) {
			// Full width
			realizedRow.mIsFullWidth = true;

			CellSlot	cellSlot{std::wstring()};
			cellSlot.mCell = cellView;
			cellView.Element().Visibility(Visibility::Visible);

			sEnsureCanvasDoesContainUIElement(mRowContentCanvas, cellView.Element());

			realizedRow.mCellSlots.push_back(cellSlot);

			return;
		}
	}

	// One per column
	for (const Column& column : mColumns) {
		// Setup
		CellSlot	cellSlot{std::wstring(column.mInfo.Identifier)};
		if (mCellFactory != nullptr) {
			// Ask
			winrt::WinUIToolbox::OutlineViewCell	cellView =
																mCellFactory.GetCell(column.mInfo.Identifier,
																		winrt::hstring(realizedRow.mID),
																		projectedOutlineView);
			if (cellView != nullptr) {
				// Have cell
				cellSlot.mCell = cellView;
				cellView.Element().Visibility(Visibility::Visible);

				sEnsureCanvasDoesContainUIElement(mRowContentCanvas, cellView.Element());
			}
		}
		realizedRow.mCellSlots.push_back(cellSlot);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::recycleCell(const winrt::WinUIToolbox::OutlineViewCell& outlineViewCell)
//----------------------------------------------------------------------------------------------------------------------
{
	mRecycledElementsByKey[std::wstring(outlineViewCell.RecycleKey())].push_back(outlineViewCell.Element());
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::recycleCells(RealizedRow& realizedRow)
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate cell slots
	for (CellSlot& cellSlot : realizedRow.mCellSlots) {
		// Check for cell
		if (cellSlot.mCell != nullptr) {
			// Hide and pool
			cellSlot.mCell.Element().Visibility(Visibility::Collapsed);
			recycleCell(cellSlot.mCell);
		}
	}
	realizedRow.mCellSlots.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::recycleRow(RealizedRow& realizedRow)
//----------------------------------------------------------------------------------------------------------------------
{
	// Cells
	recycleCells(realizedRow);

	// Chrome
	realizedRow.mBackground.Visibility(Visibility::Collapsed);
	realizedRow.mSelectionIndicator.Visibility(Visibility::Collapsed);

	mBackgroundPool.push_back(realizedRow.mBackground);
	mIndicatorPool.push_back(realizedRow.mSelectionIndicator);

	if (realizedRow.mChevron != nullptr) {
		// Pool
		realizedRow.mChevron.Visibility(Visibility::Collapsed);

		mChevronPool.push_back(realizedRow.mChevron);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::recycleAllRows()
//----------------------------------------------------------------------------------------------------------------------
{
	// Recycle
	for (RealizedRow& realizedRow : mRealizedRows)
		// Recycle
		recycleRow(realizedRow);
	mRealizedRows.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::reloadRow(RealizedRow& realizedRow,
		const std::optional<std::vector<std::wstring> >& columnIdentifiers)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if everything
	if (!columnIdentifiers.has_value() || realizedRow.mIsFullWidth) {
		// Everything
		recycleCells(realizedRow);
		populateCells(realizedRow);
	} else {
		// Setup
		winrt::WinUIToolbox::OutlineView	projectedOutlineView = getProjectedOutlineView();

		// Just these columns
		for (CellSlot& cellSlot : realizedRow.mCellSlots) {
			// Check column
			if (!sDoesContain(*columnIdentifiers, cellSlot.mColumnIdentifier))
				continue;

			// Reload
			if (cellSlot.mCell != nullptr) {
				// Pool the old
				cellSlot.mCell.Element().Visibility(Visibility::Collapsed);
				recycleCell(cellSlot.mCell);
				cellSlot.mCell = nullptr;
			}
			if (mCellFactory != nullptr) {
				// Ask
				winrt::WinUIToolbox::OutlineViewCell	cellView =
																	mCellFactory.GetCell(
																			winrt::hstring(cellSlot.mColumnIdentifier),
																			winrt::hstring(realizedRow.mID),
																			projectedOutlineView);
				if (cellView != nullptr) {
					// Have cell
					cellSlot.mCell = cellView;
					sEnsureCanvasDoesContainUIElement(mRowContentCanvas, cellView.Element());
					cellView.Element().Visibility(Visibility::Visible);
				}
			}
		}
	}

	// Place
	layoutRow(realizedRow);
}

//----------------------------------------------------------------------------------------------------------------------
double OutlineView::Internals::getChevronX(const RealizedRow& realizedRow) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	double	x = (mOutlineStyle == OutlineViewStyle::Gutter) ? 0.0 : (mColumns.empty() ? 0.0 : mColumns[0].mX);

	return x + (double) realizedRow.mLevel * kIndentPerLevel;
}

//----------------------------------------------------------------------------------------------------------------------
double OutlineView::Internals::getFirstColumnIndent(const RealizedRow& realizedRow) const
//----------------------------------------------------------------------------------------------------------------------
{
	return (mOutlineStyle == OutlineViewStyle::FirstColumn) ?
			((double) realizedRow.mLevel * kIndentPerLevel + kChevronWidth) : 0.0;
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::layoutRow(RealizedRow& realizedRow)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	double	y = getRowY(realizedRow.mRow);

	// Chrome
	Canvas::SetLeft(realizedRow.mBackground, 0.0);
	Canvas::SetTop(realizedRow.mBackground, y);
	realizedRow.mBackground.Width(mContentWidth);
	realizedRow.mBackground.Height(mRowHeight);

	Canvas::SetLeft(realizedRow.mSelectionIndicator, 2.0);
	Canvas::SetTop(realizedRow.mSelectionIndicator, y + (mRowHeight - kSelectionIndicatorHeight) / 2.0);

	if (realizedRow.mChevron != nullptr) {
		// Place
		Canvas::SetLeft(realizedRow.mChevron, getChevronX(realizedRow));
		Canvas::SetTop(realizedRow.mChevron, y);
		realizedRow.mChevron.Width(kChevronWidth);
		realizedRow.mChevron.Height(mRowHeight);
	}

	// Cells
	double	indent = getFirstColumnIndent(realizedRow);
	if (realizedRow.mIsFullWidth) {
		// One cell across everything
		if (!realizedRow.mCellSlots.empty() && (realizedRow.mCellSlots[0].mCell != nullptr)) {
			// Place
			double	x = (mColumns.empty() ? mGutterWidth : mColumns[0].mX) + indent + kCellInset;
			placeCell(realizedRow.mCellSlots[0].mCell.Element(), x, mContentWidth - x - kCellInset, y);
		}
	} else {
		// One per column
		size_t	count = std::min(realizedRow.mCellSlots.size(), mColumns.size());
		for (size_t i = 0; i < count; i++) {
			// Check for cell
			CellSlot&	cellSlot = realizedRow.mCellSlots[i];
			if (cellSlot.mCell == nullptr)
				continue;

			// Place
			const	Column&	column = mColumns[i];
					double	cellIndent = (i == 0) ? indent : 0.0;
					double	x = column.mX + cellIndent + kCellInset;
					double	width = column.mWidth - cellIndent - 2.0 * kCellInset;
			placeCell(cellSlot.mCell.Element(), x, width, y);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::placeCell(const UIElement& element, double x, double width, double y)
//----------------------------------------------------------------------------------------------------------------------
{
	// Size, then see how tall it wants to be so it can sit centered in the row
	FrameworkElement	frameworkElement = element.try_as<FrameworkElement>();
	if (frameworkElement != nullptr)
		// Set width
		frameworkElement.Width(std::max(width, 0.0));
	element.Measure(Size{(float) std::max(width, 0.0), (float) mRowHeight});

	double	height = std::min((double) element.DesiredSize().Height, mRowHeight);
	Canvas::SetLeft(element, x);
	Canvas::SetTop(element, y + (mRowHeight - height) / 2.0);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::updateRowVisuals(RealizedRow& realizedRow)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	bool	isSelected = isRowSelected(realizedRow.mRow);
	bool	isHovered = mHoveredRow.has_value() && (*mHoveredRow == realizedRow.mRow);

	// Update
	realizedRow.mBackground.Fill(
			isSelected ?
					(isHovered ? mSelectedHoverBrush : mSelectedBrush) : (isHovered ? mHoverBrush : mTransparentBrush));
	realizedRow.mBackground.Visibility(Visibility::Visible);
	realizedRow.mSelectionIndicator.Visibility(isSelected ? Visibility::Visible : Visibility::Collapsed);
	if (realizedRow.mChevron != nullptr)
		// Update glyph
		realizedRow.mChevron.Glyph(realizedRow.mIsExpanded ? kChevronDownGlyph : kChevronRightGlyph);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::updateAllRowVisuals()
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate realized rows
	for (RealizedRow& realizedRow : mRealizedRows)
		// Update
		updateRowVisuals(realizedRow);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::updateFocusRect()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if showing
	RealizedRow*	realizedRow = (mHasFocus && mFocusedRow.has_value()) ? findRealizedRow(*mFocusedRow) : nullptr;
	if (realizedRow == nullptr) {
		// Hide
		mFocusRect.Visibility(Visibility::Collapsed);

		return;
	}

	// Show
	Canvas::SetLeft(mFocusRect, 0.5);
	Canvas::SetTop(mFocusRect, getRowY(realizedRow->mRow) + 0.5);
	mFocusRect.Width(std::max(mContentWidth - 1.0, 0.0));
	mFocusRect.Height(std::max(mRowHeight - 1.0, 0.0));
	mFocusRect.Visibility(Visibility::Visible);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::updateSortIndicators()
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate columns
	for (Column& column : mColumns) {
		// Check if primary
		bool	isPrimary =
						!mSortDescriptors.empty() &&
								(mSortDescriptors[0].getColumnIdentifier() == column.mInfo.Identifier);
		if (isPrimary) {
			// Show
			column.mSortFontIcon.Glyph(
					mSortDescriptors[0].getIsAscending() ? kSortAscendingGlyph : kSortDescendingGlyph);
			column.mSortFontIcon.Visibility(Visibility::Visible);
		} else
			// Hide
			column.mSortFontIcon.Visibility(Visibility::Collapsed);
	}
}

//----------------------------------------------------------------------------------------------------------------------
Shapes::Rectangle OutlineView::Internals::dequeueRectangle(std::vector<Shapes::Rectangle>& pool)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check pool
	if (!pool.empty()) {
		// Reuse
		Shapes::Rectangle	rectangle = pool.back();
		pool.pop_back();

		return rectangle;
	}

	// Create
	Shapes::Rectangle	rectangle;
	rectangle.IsHitTestVisible(false);
	mRowBackgroundsCanvas.Children().Append(rectangle);

	return rectangle;
}

//----------------------------------------------------------------------------------------------------------------------
UIElement OutlineView::Internals::tryGetRecycledElement(const std::wstring& recycleKey)
//----------------------------------------------------------------------------------------------------------------------
{
	// Look for one waiting
	auto	iterator = mRecycledElementsByKey.find(recycleKey);
	if ((iterator == mRecycledElementsByKey.end()) || iterator->second.empty())
		// None
		return nullptr;

	// Take the last one
	UIElement	element = iterator->second.back();
	iterator->second.pop_back();

	return element;
}

// MARK: Instance methods - state

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::setHoveredRow(const std::optional<uint32_t>& row)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (row == mHoveredRow)
		return;

	// Update
	std::optional<uint32_t>	previousRow = mHoveredRow;
	mHoveredRow = row;

	RealizedRow*	realizedRow;
	if (previousRow.has_value() && ((realizedRow = findRealizedRow(*previousRow)) != nullptr))
		updateRowVisuals(*realizedRow);
	if (mHoveredRow.has_value() && ((realizedRow = findRealizedRow(*mHoveredRow)) != nullptr))
		updateRowVisuals(*realizedRow);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::setFocusedRow(const std::optional<uint32_t>& row)
//----------------------------------------------------------------------------------------------------------------------
{
	// Update
	mFocusedRow = row;
	if (mFocusedRow.has_value())
		// Keep it in view
		scrollRowIntoView(*mFocusedRow);
	updateFocusRect();
}

// MARK: Instance methods - hit testing

//----------------------------------------------------------------------------------------------------------------------
bool OutlineView::Internals::isInHeader(const Point& point) const
//----------------------------------------------------------------------------------------------------------------------
{
	return (getColumnHeaderHeight() > 0.0) && (point.Y >= 0.0) && (point.Y < getColumnHeaderHeight()) && (point.X >= 0.0) &&
			(point.X < mViewportWidth);
}

//----------------------------------------------------------------------------------------------------------------------
bool OutlineView::Internals::isInBody(const Point& point) const
//----------------------------------------------------------------------------------------------------------------------
{
	return (point.Y >= getColumnHeaderHeight()) && (point.Y < mViewportHeight) &&
			(point.X >= 0.0) && (point.X < mViewportWidth);
}

//----------------------------------------------------------------------------------------------------------------------
bool OutlineView::Internals::isInEditor(const Point& point) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if editing
	if (mEditTextBox == nullptr)
		return false;

	// Check bounds, in viewport coordinates
	double	x = Canvas::GetLeft(mEditTextBox) - mHOffset;
	double	y = Canvas::GetTop(mEditTextBox);

	return (point.X >= x) && (point.X < (x + mEditTextBox.Width())) &&
			(point.Y >= y) && (point.Y < (y + mEditTextBox.Height()));
}

//----------------------------------------------------------------------------------------------------------------------
std::optional<uint32_t> OutlineView::Internals::getRowAt(const Point& point) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check body
	if ((point.Y < getColumnHeaderHeight()) || (point.Y >= mViewportHeight) || (point.X < 0.0) ||
			(point.X >= mViewportWidth) || (mRowHeight <= 0.0))
		return std::optional<uint32_t>();

	// Compute
	double		y = point.Y - getColumnHeaderHeight() + getSubRowOffset();
	uint32_t	row = getFirstRow() + (uint32_t) std::floor(y / mRowHeight);

	return (row < getRowCount()) ? std::optional<uint32_t>(row) : std::optional<uint32_t>();
}

//----------------------------------------------------------------------------------------------------------------------
int OutlineView::Internals::getColumnIndexAt(double contentX) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate columns
	for (size_t i = 0; i < mColumns.size(); i++) {
		// Check
		if ((contentX >= mColumns[i].mX) && (contentX < (mColumns[i].mX + mColumns[i].mWidth)))
			// Found
			return (int) i;
	}

	return -1;
}

//----------------------------------------------------------------------------------------------------------------------
int OutlineView::Internals::getColumnIndex(const std::wstring& identifier) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate columns
	for (size_t i = 0; i < mColumns.size(); i++) {
		// Check
		if (mColumns[i].mInfo.Identifier == identifier)
			// Found
			return (int) i;
	}

	return -1;
}

//----------------------------------------------------------------------------------------------------------------------
int OutlineView::Internals::getDividerIndexAt(double contentX) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate columns
	for (size_t i = 0; i < mColumns.size(); i++) {
		// Check trailing edge
		if (std::abs(contentX - (mColumns[i].mX + mColumns[i].mWidth)) <= kResizeHitZone)
			// Found
			return (int) i;
	}

	return -1;
}

//----------------------------------------------------------------------------------------------------------------------
bool OutlineView::Internals::isInChevron(const RealizedRow& realizedRow, double contentX) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check
	if ((realizedRow.mChevron == nullptr) || (mOutlineStyle == OutlineViewStyle::None))
		return false;

	double	x = getChevronX(realizedRow);

	return (contentX >= x) && (contentX < (x + kChevronWidth));
}

//----------------------------------------------------------------------------------------------------------------------
std::optional<std::wstring> OutlineView::Internals::getColumnIdentifierAt(double contentX) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Get column
	int	columnIndex = getColumnIndexAt(contentX);

	return (columnIndex >= 0) ?
			std::optional<std::wstring>(std::wstring(mColumns[columnIndex].mInfo.Identifier)) :
			std::optional<std::wstring>();
}

// MARK: Instance methods - pointer

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::pointerPressed(const Point& point, PointerHelper::Buttons buttons, int pressedCount,
		const VirtualKeyModifiers& virtualKeyModifiers)
//----------------------------------------------------------------------------------------------------------------------
{
	// Only the header and the body are ours - the scrollbars and an edit under
	//	way look after themselves
	if ((!isInHeader(point) && !isInBody(point)) || isInEditor(point))
		return;

	// Take focus
	mOutlineView.Focus(FocusState::Pointer);

	// Nothing without a data source
	if (mDataSource == nullptr)
		return;

	// Setup
	bool	isLeft = (buttons & PointerHelper::kButtonsLeft) != 0;
	bool	isRight = (buttons & PointerHelper::kButtonsRight) != 0;
	double	contentX = point.X + mHOffset;

	// Header
	if (isInHeader(point)) {
		// Header press
		mClickedRow = std::optional<uint32_t>();
		if (!isLeft)
			return;

		// Divider first
		int	dividerIndex = getDividerIndexAt(contentX);
		if (mCanUserResizeColumns && (dividerIndex >= 0)) {
			// Resize
			mResizingColumnIndex = dividerIndex;
			mResizingStartWidth = mColumns[dividerIndex].mWidth;
			mPressedX = point.X;
		} else {
			// Column
			mPressedHeaderColumnIndex = getColumnIndexAt(contentX);
			mPressedX = point.X;
			mIsDraggingColumn = false;
			mDropColumnIndex = -1;
		}

		return;
	}

	// Body
	std::optional<uint32_t>	row = getRowAt(point);
	mClickedRow = row;
	mClickedColumnIdentifier = getColumnIdentifierAt(contentX);

	if (isLeft) {
		// Chevron
		RealizedRow*	realizedRow = row.has_value() ? findRealizedRow(*row) : nullptr;
		if ((realizedRow != nullptr) && isInChevron(*realizedRow, contentX)) {
			// Toggle
			toggleExpanded(*row, sDoesVirtualKeyModifiersContain(virtualKeyModifiers, VirtualKeyModifiers::Shift));

			return;
		}

		// Double tap
		if ((pressedCount == 2) && row.has_value()) {
			// Raise event
			if (mItemInvokedEvent)
				mItemInvokedEvent(getProjectedOutlineView(),
						winrt::make<OutlineViewItemInvokedEventArgs>(winrt::hstring(mRowEntries[*row].mIdentifier),
								sToHString(mClickedColumnIdentifier)));

			return;
		}

		// Selection
		if (row.has_value())
			// Row
			selectRowFromPointer(*row,
					sDoesVirtualKeyModifiersContain(virtualKeyModifiers, VirtualKeyModifiers::Control),
					sDoesVirtualKeyModifiersContain(virtualKeyModifiers, VirtualKeyModifiers::Shift));
		else if (!mSelectedIdentifiers.empty() && (mSelectionMode != OutlineViewSelectionMode::None))
			// Empty area
			deselectAllInternal();
	} else if (isRight && row.has_value()) {
		// A right press on something outside the selection selects it
		if (!isRowSelected(*row) && (mSelectionMode != OutlineViewSelectionMode::None)) {
			// Select
			setSelectedRows(std::vector<uint32_t>(1, *row));
			mAnchorRow = row;
			setFocusedRow(row);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::pointerDragged(const Point& point, const VirtualKeyModifiers& virtualKeyModifiers)
//----------------------------------------------------------------------------------------------------------------------
{
	// Resizing
	if (mResizingColumnIndex >= 0) {
		// Update width - the view owns columns, so set it directly and re-lay out
		Column&	column = mColumns[mResizingColumnIndex];
		double	maxWidth = (column.mInfo.MaxWidth > 0.0) ? column.mInfo.MaxWidth : std::numeric_limits<double>::max();
		double	width =
						std::min(std::max(mResizingStartWidth + (point.X - mPressedX), column.mInfo.MinWidth),
								maxWidth);
		column.mInfo.Width = width;
		layout();

		return;
	}

	// Column drag
	if ((mPressedHeaderColumnIndex >= 0) && mCanUserReorderColumns && canMoveColumn(mPressedHeaderColumnIndex)) {
		// Check threshold
		if (!mIsDraggingColumn && (std::abs(point.X - mPressedX) > kDragThreshold))
			// Now dragging
			mIsDraggingColumn = true;
		if (!mIsDraggingColumn)
			return;

		// Where would it land
		double	contentX = point.X + mHOffset;
		int		dropColumnIndex = (int) mColumns.size();
		for (size_t i = 0; i < mColumns.size(); i++) {
			// Check midpoint
			if (contentX < (mColumns[i].mX + mColumns[i].mWidth / 2.0)) {
				// Before this one
				dropColumnIndex = (int) i;
				break;
			}
		}
		mDropColumnIndex = std::max(dropColumnIndex, getFirstMovableColumnIndex());

		// Show
		double	x = (mDropColumnIndex < (int) mColumns.size()) ? mColumns[mDropColumnIndex].mX : mTotalWidth;
		Canvas::SetLeft(mDragIndicator, x - 1.0);
		Canvas::SetTop(mDragIndicator, 0.0);
		mDragIndicator.Height(mViewportHeight);
		mDragIndicator.Visibility(Visibility::Visible);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::pointerReleased(const Point& point)
//----------------------------------------------------------------------------------------------------------------------
{
	// Resizing
	if (mResizingColumnIndex >= 0) {
		// Done - raise event
		winrt::hstring	identifier = mColumns[mResizingColumnIndex].mInfo.Identifier;
		mResizingColumnIndex = -1;
		if (mColumnResizedEvent)
			mColumnResizedEvent(getProjectedOutlineView(), winrt::make<OutlineViewColumnEventArgs>(identifier));

		return;
	}

	// Header
	if (mPressedHeaderColumnIndex >= 0) {
		// Setup
		int	columnIndex = mPressedHeaderColumnIndex;
		mPressedHeaderColumnIndex = -1;
		mDragIndicator.Visibility(Visibility::Collapsed);

		if (mIsDraggingColumn) {
			// Drop
			mIsDraggingColumn = false;

			int	toIndex = mDropColumnIndex;
			if (toIndex > columnIndex)
				// Removing first shifts everything after down one
				toIndex--;
			if ((toIndex >= 0) && (toIndex != columnIndex) && (toIndex < (int) mColumns.size()))
				// Move
				moveColumn((uint32_t) columnIndex, (uint32_t) toIndex);
		} else if (columnIndex < (int) mColumns.size())
			// Click - sort
			applySortDescriptors(
					getSortDescriptorsForHeaderClick(std::wstring(mColumns[columnIndex].mInfo.Identifier)));
	}
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::pointerMoved(const Point& point)
//----------------------------------------------------------------------------------------------------------------------
{
	// Header
	if (isInHeader(point)) {
		// Cursor
		updateResizeCursor(mCanUserResizeColumns && (getDividerIndexAt(point.X + mHOffset) >= 0));
		setHoveredRow(std::optional<uint32_t>());
	} else {
		// Body
		updateResizeCursor(false);
		setHoveredRow(getRowAt(point));
	}
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::updateResizeCursor(bool show)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (show == mIsShowingResizeCursor)
		return;

	// Update
	mIsShowingResizeCursor = show;
	if (show && (mResizeCursor == nullptr))
		// Setup
		mResizeCursor = InputSystemCursor::Create(InputSystemCursorShape::SizeWestEast);
	mOutlineView.ProtectedCursor(show ? mResizeCursor : InputCursor(nullptr));
}

//----------------------------------------------------------------------------------------------------------------------
int OutlineView::Internals::getFirstMovableColumnIndex() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (mOutlineStyle == OutlineViewStyle::FirstColumn) ? 1 : 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool OutlineView::Internals::canMoveColumn(int columnIndex) const
//----------------------------------------------------------------------------------------------------------------------
{
	return columnIndex >= getFirstMovableColumnIndex();
}

// MARK: Instance methods - sorting

//----------------------------------------------------------------------------------------------------------------------
std::vector<SortDescriptor> OutlineView::Internals::getSortDescriptorsForHeaderClick(
		const std::wstring& identifier) const
//----------------------------------------------------------------------------------------------------------------------
{
	// A click on the primary column reverses it.  A click on any other column
	//	makes it the primary, ascending, with the previous sort descriptors
	//	falling in behind it.
	std::vector<SortDescriptor>	updatedSortDescriptors;
	if (!mSortDescriptors.empty() && (mSortDescriptors[0].getColumnIdentifier() == identifier))
		// Reverse primary
		updatedSortDescriptors.push_back(SortDescriptor(identifier, !mSortDescriptors[0].getIsAscending()));
	else
		// New primary
		updatedSortDescriptors.push_back(SortDescriptor(identifier, true));

	// Add the rest
	for (const SortDescriptor& sortDescriptor : mSortDescriptors) {
		// Check identifier
		if (sortDescriptor.getColumnIdentifier() != identifier)
			// Add
			updatedSortDescriptors.push_back(sortDescriptor);
	}

	return updatedSortDescriptors;
}

//----------------------------------------------------------------------------------------------------------------------
IVectorView<OutlineViewSortDescription> OutlineView::Internals::getSortDescriptionsView() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Convert
	std::vector<OutlineViewSortDescription>	values;
	values.reserve(mSortDescriptors.size());
	for (const SortDescriptor& sortDescriptor : mSortDescriptors) {
		// Add
		OutlineViewSortDescription	value;
		value.ColumnIdentifier = winrt::hstring(sortDescriptor.getColumnIdentifier());
		value.IsAscending = sortDescriptor.getIsAscending();
		values.push_back(value);
	}

	return winrt::single_threaded_vector<OutlineViewSortDescription>(std::move(values)).GetView();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::applySortDescriptors(const std::vector<SortDescriptor>& sortDescriptors)
//----------------------------------------------------------------------------------------------------------------------
{
	// Store
	mSortDescriptors = sortDescriptors;
	updateSortIndicators();

	// Tell the data source, which re-sorts and reloads, then raise event
	if (mDataSource != nullptr)
		mDataSource->noteSortDescriptorsDidChange(mSortDescriptors);
	if (mSortDescriptionsChangedEvent)
		mSortDescriptionsChangedEvent(getProjectedOutlineView(),
				winrt::make<OutlineViewSortDescriptionsChangedEventArgs>(getSortDescriptionsView()));
}

// MARK: Instance methods - selection

//----------------------------------------------------------------------------------------------------------------------
bool OutlineView::Internals::isRowSelected(uint32_t row) const
//----------------------------------------------------------------------------------------------------------------------
{
	return (row < mRowEntries.size()) && (mSelectedIdentifiers.count(mRowEntries[row].mIdentifier) > 0);
}

//----------------------------------------------------------------------------------------------------------------------
std::vector<std::wstring> OutlineView::Internals::getSelectedIdentifiersInRowOrder() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Collect in row order
	std::vector<std::wstring>	identifiers;
	for (const RowEntry& rowEntry : mRowEntries)
		// Check
		if (mSelectedIdentifiers.count(rowEntry.mIdentifier) > 0)
			// Add
			identifiers.push_back(rowEntry.mIdentifier);

	return identifiers;
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::selectionChanged()
//----------------------------------------------------------------------------------------------------------------------
{
	// Update visuals, then raise event
	updateAllRowVisuals();
	if (mSelectionChangedEvent)
		mSelectionChangedEvent(getProjectedOutlineView(),
				winrt::make<OutlineViewSelectionChangedEventArgs>(sToHStrings(getSelectedIdentifiersInRowOrder())));
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::setSelectedRows(const std::vector<uint32_t>& rows)
//----------------------------------------------------------------------------------------------------------------------
{
	// Replace
	mSelectedIdentifiers.clear();
	for (uint32_t row : rows)
		// Add
		if (row < mRowEntries.size())
			mSelectedIdentifiers.insert(mRowEntries[row].mIdentifier);

	// Selection did change
	selectionChanged();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::selectRows(const std::vector<uint32_t>& rows)
//----------------------------------------------------------------------------------------------------------------------
{
	// Add
	for (uint32_t row : rows)
		// Add
		if (row < mRowEntries.size())
			mSelectedIdentifiers.insert(mRowEntries[row].mIdentifier);

	// Selection did change
	selectionChanged();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::deselectRows(const std::vector<uint32_t>& rows)
//----------------------------------------------------------------------------------------------------------------------
{
	// Remove
	for (uint32_t row : rows)
		// Remove
		if (row < mRowEntries.size())
			mSelectedIdentifiers.erase(mRowEntries[row].mIdentifier);

	// Selection did change
	selectionChanged();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::setSelectedIdentifiers(const std::vector<std::wstring>& identifiers)
//----------------------------------------------------------------------------------------------------------------------
{
	// Replace
	mSelectedIdentifiers.clear();
	for (const std::wstring& identifier : identifiers)
		// Add
		mSelectedIdentifiers.insert(identifier);

	// Selection did change
	selectionChanged();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::selectIdentifiers(const std::vector<std::wstring>& identifiers, bool byExtending)
//----------------------------------------------------------------------------------------------------------------------
{
	// Replace or extend
	if (!byExtending)
		mSelectedIdentifiers.clear();
	for (const std::wstring& identifier : identifiers)
		// Add
		mSelectedIdentifiers.insert(identifier);

	// Selection did change
	selectionChanged();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::deselectIdentifiers(const std::vector<std::wstring>& identifiers)
//----------------------------------------------------------------------------------------------------------------------
{
	// Remove
	for (const std::wstring& identifier : identifiers)
		// Remove
		mSelectedIdentifiers.erase(identifier);

	// Selection did change
	selectionChanged();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::selectAllInternal()
//----------------------------------------------------------------------------------------------------------------------
{
	// Every row
	for (const RowEntry& rowEntry : mRowEntries)
		// Add
		mSelectedIdentifiers.insert(rowEntry.mIdentifier);

	// Selection did change
	selectionChanged();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::deselectAllInternal()
//----------------------------------------------------------------------------------------------------------------------
{
	// None
	mSelectedIdentifiers.clear();

	// Selection did change
	selectionChanged();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::selectRowFromPointer(uint32_t row, bool toggle, bool extend)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check selection mode
	if (mSelectionMode == OutlineViewSelectionMode::None) {
		// Only focus moves
		setFocusedRow(std::optional<uint32_t>(row));

		return;
	}

	// In Multiple every press toggles; in Extended the modifiers decide; in Single neither applies
	bool	canToggle = (mSelectionMode == OutlineViewSelectionMode::Multiple) ||
					(toggle && (mSelectionMode == OutlineViewSelectionMode::Extended));
	bool	canExtend = extend && (mSelectionMode == OutlineViewSelectionMode::Extended);
	if (canToggle) {
		// Toggle this row
		std::vector<uint32_t>	rows(1, row);
		if (isRowSelected(row))
			// Deselect rows
			deselectRows(rows);
		else
			// Select rows
			selectRows(rows);

		// Update anchor row
		mAnchorRow = std::optional<uint32_t>(row);
	} else if (canExtend && mAnchorRow.has_value())
		// Range from anchor
		selectRange(*mAnchorRow, row);
	else {
		// Just this row
		setSelectedRows(std::vector<uint32_t>(1, row));

		// Update anchor row
		mAnchorRow = std::optional<uint32_t>(row);
	}

	// Update
	setFocusedRow(std::optional<uint32_t>(row));
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::selectRange(uint32_t row1, uint32_t row2)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	uint32_t	startRow = std::min(row1, row2);
	uint32_t	endRow = std::max(row1, row2);

	// Collect
	std::vector<uint32_t>	rows;
	rows.reserve(endRow - startRow + 1);
	for (uint32_t row = startRow; row <= endRow; row++)
		// Add
		rows.push_back(row);

	// Select
	setSelectedRows(rows);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::moveFocus(uint32_t row, bool extend, bool focusOnly)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	uint32_t	rowCount = getRowCount();
	if (rowCount == 0)
		return;
	row = std::min(row, rowCount - 1);

	// Update selection.  In None nothing is selected, and in Multiple the keyboard moves focus without touching the
	//	selection - Space toggles, as the pointer does.
	if (!focusOnly && (mSelectionMode != OutlineViewSelectionMode::None) &&
			(mSelectionMode != OutlineViewSelectionMode::Multiple)) {
		// Selecting
		if (extend && (mSelectionMode == OutlineViewSelectionMode::Extended)) {
			// Range from anchor
			if (!mAnchorRow.has_value())
				// Set anchor row
				mAnchorRow = mFocusedRow;
			selectRange(mAnchorRow.has_value() ? *mAnchorRow : row, row);
		} else {
			// Just this row
			setSelectedRows(std::vector<uint32_t>(1, row));

			// Set anchor row
			mAnchorRow = std::optional<uint32_t>(row);
		}
	}

	// Update focus
	setFocusedRow(std::optional<uint32_t>(row));
}

// MARK: Instance methods - keyboard

//----------------------------------------------------------------------------------------------------------------------
bool OutlineView::Internals::keyDown(VirtualKey virtualKey)
//----------------------------------------------------------------------------------------------------------------------
{
	// Nothing without a data source, and nothing while editing
	if ((mDataSource == nullptr) || (mEditTextBox != nullptr))
		return false;

	// Setup
	bool		isShift = sIsVirtualKeyDown(VirtualKey::Shift);
	bool		isControl = sIsVirtualKeyDown(VirtualKey::Control);
	uint32_t	rowCount = getRowCount();
	uint32_t	focusedRow = mFocusedRow.has_value() ? *mFocusedRow : 0;
	uint32_t	pageRows = std::max((uint32_t) std::floor(getViewportRows()), (uint32_t) 1);

	// Check key
	switch (virtualKey) {
		case VirtualKey::Up:
			// Up
			if (rowCount == 0)	return true;

			moveFocus(mFocusedRow.has_value() ? ((focusedRow > 0) ? focusedRow - 1 : 0) : 0, isShift,
					isControl);

			return true;

		case VirtualKey::Down:
			// Down
			if (rowCount == 0)	return true;

			moveFocus(mFocusedRow.has_value() ? focusedRow + 1 : 0, isShift, isControl);

			return true;

		case VirtualKey::Home:
			// Home
			if (rowCount == 0)	return true;

			moveFocus(0, isShift, isControl);

			return true;

		case VirtualKey::End:
			// End
			if (rowCount == 0)	return true;

			moveFocus(rowCount - 1, isShift, isControl);

			return true;

		case VirtualKey::PageUp:
			// Page up
			if (rowCount == 0)	return true;

			moveFocus((focusedRow > pageRows) ? focusedRow - pageRows : 0, isShift, isControl);

			return true;

		case VirtualKey::PageDown:
			// Page down
			if (rowCount == 0)	return true;

			moveFocus(focusedRow + pageRows, isShift, isControl);

			return true;

		case VirtualKey::Left: {
			// Left
			if (!mFocusedRow.has_value())	return false;

			const	RowEntry&	rowEntry = mRowEntries[focusedRow];
			if (rowEntry.mHasChildren && rowEntry.mIsExpanded)
				// Collapse
				toggleExpanded(focusedRow, isShift);
			else {
				// Get parent row
				std::optional<uint32_t>	parentRow = getParentRow(focusedRow);
				if (parentRow.has_value())
					// Move focus
					moveFocus(*parentRow, false, isControl);
			}

			return true;
		}

		case VirtualKey::Right: {
			// Right
			if (!mFocusedRow.has_value())	return false;

			const	RowEntry&	rowEntry = mRowEntries[focusedRow];
			if (rowEntry.mHasChildren && !rowEntry.mIsExpanded)
				// Expand
				toggleExpanded(focusedRow, isShift);
			else if (rowEntry.mHasChildren && ((focusedRow + 1) < rowCount))
				// Move focus
				moveFocus(focusedRow + 1, false, isControl);

			return true;
		}

		case VirtualKey::Add:
			// Add
			if (mFocusedRow.has_value() && !mRowEntries[focusedRow].mIsExpanded)
				toggleExpanded(focusedRow, false);

			return true;

		case VirtualKey::Subtract:
			// Subtract
			if (mFocusedRow.has_value() && mRowEntries[focusedRow].mIsExpanded)
				toggleExpanded(focusedRow, false);

			return true;

		case VirtualKey::Multiply:
			// Multiply
			if (mFocusedRow.has_value() && !mRowEntries[focusedRow].mIsExpanded)
				toggleExpanded(focusedRow, true);

			return true;

		case VirtualKey::Space:
			// Space
			if (mFocusedRow.has_value())
				selectRowFromPointer(focusedRow, isControl, isShift);

			return true;

		case VirtualKey::Enter:
			// Enter
			if (mFocusedRow.has_value() && mItemInvokedEvent)
				mItemInvokedEvent(getProjectedOutlineView(),
						winrt::make<OutlineViewItemInvokedEventArgs>(
								winrt::hstring(mRowEntries[focusedRow].mIdentifier), winrt::hstring()));

			return true;

		case VirtualKey::F2:
			// F2
			if (mFocusedRow.has_value())
				beginEditFirstEditableColumn(focusedRow);

			return true;

		case VirtualKey::A:
			// Select all
			if (isControl &&
					((mSelectionMode == OutlineViewSelectionMode::Multiple) ||
							(mSelectionMode == OutlineViewSelectionMode::Extended))) {
				// Select all
				selectAllInternal();

				return true;
			}

			return false;

		default:
			return false;
	}
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::characterReceived(char16_t character)
//----------------------------------------------------------------------------------------------------------------------
{
	// Nothing without a way to match, and nothing while editing
	if ((mDataSource == nullptr) || (mCellFactory == nullptr) || (mEditTextBox != nullptr) || (character < 0x20))
		return;

	// Extend or restart the search string
	auto	now = std::chrono::steady_clock::now();
	if (std::chrono::duration_cast<std::chrono::milliseconds>(now - mTextSearchTime).count() >
			kTextSearchTimeoutMilliseconds)
		// Restart
		mTextSearchString.clear();
	mTextSearchTime = now;
	mTextSearchString += (wchar_t) character;

	// Search from the focused row, wrapping
	uint32_t	rowCount = getRowCount();
	if (rowCount == 0)
		return;

	std::wstring	searchString = sToLowercased(mTextSearchString);
	uint32_t		startRow =
							(mFocusedRow.has_value() && (mTextSearchString.size() > 1)) ?
									*mFocusedRow :
									(mFocusedRow.has_value() ? (*mFocusedRow + 1) % rowCount : 0);
	for (uint32_t i = 0; i < rowCount; i++) {
		// Check row
		uint32_t	row = (startRow + i) % rowCount;
		winrt::hstring	string = mCellFactory.GetTextSearchString(winrt::hstring(mRowEntries[row].mIdentifier));
		if (!string.empty() &&
				(sToLowercased(std::wstring(string)).compare(0, searchString.size(), searchString) == 0)) {
			// Found
			moveFocus(row, false, false);

			return;
		}
	}
}

// MARK: Instance methods - editing

//----------------------------------------------------------------------------------------------------------------------
bool OutlineView::Internals::beginEdit(uint32_t row, const std::wstring& columnIdentifier)
//----------------------------------------------------------------------------------------------------------------------
{
	// Preflight
	if ((mDataSource == nullptr) || (row >= getRowCount()))
		return false;

	int	columnIndex = getColumnIndex(columnIdentifier);
	if ((columnIndex < 0) || !mColumns[columnIndex].mInfo.IsEditable)
		return false;

	std::wstring	identifier = mRowEntries[row].mIdentifier;

	// Raise event - the handler may refuse the edit, or supply the text to edit
	std::wstring	text;
	bool			gotText = false;
	if (mBeginningEditEvent) {
		// Raise
		winrt::WinUIToolbox::OutlineViewBeginningEditEventArgs	args =
																		winrt::make<OutlineViewBeginningEditEventArgs>(
																				winrt::hstring(identifier),
																				winrt::hstring(columnIdentifier));
		mBeginningEditEvent(getProjectedOutlineView(), args);
		if (args.Cancel())
			return false;
		if (!args.Text().empty()) {
			// Have text
			text = std::wstring(args.Text());
			gotText = true;
		}
	}

	// Any edit under way ends first
	endEdit(true, OutlineViewEditEndReason::Other);

	// Bring it into view
	scrollRowIntoView(row);
	RealizedRow*	realizedRow = findRealizedRow(row);
	if (realizedRow == nullptr)
		return false;

	// What to edit - the cell's text, unless the handler supplied some
	if (!gotText && !realizedRow->mIsFullWidth && ((size_t) columnIndex < realizedRow->mCellSlots.size()) &&
			(realizedRow->mCellSlots[columnIndex].mCell != nullptr)) {
		// Read the text cell, if it is one
		TextBlock	textBlock = realizedRow->mCellSlots[columnIndex].mCell.Element().try_as<TextBlock>();
		if (textBlock != nullptr)
			text = std::wstring(textBlock.Text());
	}

	// Place the editor over the cell
	const	Column&	column = mColumns[columnIndex];
			double	indent = (columnIndex == 0) ? getFirstColumnIndent(*realizedRow) : 0.0;
			double	x = column.mX + indent;
			double	width = std::max(column.mWidth - indent, 0.0);
			double	y = getRowY(row);

	mEditTextBox = TextBox();
	mEditTextBox.Text(winrt::hstring(text));
	mEditTextBox.MinHeight(0.0);
	mEditTextBox.Width(width);
	mEditTextBox.Height(mRowHeight);
	mEditTextBox.Padding(ThicknessHelper::FromLengths(kCellInset - 2.0, 0.0, 4.0, 0.0));
	mEditTextBox.VerticalContentAlignment(VerticalAlignment::Center);
	mOverlayCanvas.Children().Append(mEditTextBox);

	Canvas::SetLeft(mEditTextBox, x);
	Canvas::SetTop(mEditTextBox, y);

	mEditingRow = std::optional<uint32_t>(row);
	mEditingColumnIdentifier = std::optional<std::wstring>(columnIdentifier);

	mEditTextBox.KeyDown([this](const IInspectable& sender, const KeyRoutedEventArgs& keyRoutedEventArgs){
		// Check key
		switch (keyRoutedEventArgs.Key()) {
			case VirtualKey::Enter:
				// Enter
				keyRoutedEventArgs.Handled(true);
				endEdit(true, OutlineViewEditEndReason::Enter);
				break;

			case VirtualKey::Escape:
				// Escape
				keyRoutedEventArgs.Handled(true);
				endEdit(false, OutlineViewEditEndReason::Other);
				break;

			case VirtualKey::Tab:
				// Tab
				keyRoutedEventArgs.Handled(true);
				endEdit(true,
						sIsVirtualKeyDown(VirtualKey::Shift) ?
								OutlineViewEditEndReason::ShiftTab : OutlineViewEditEndReason::Tab);
				break;

			default:
				break;
		}
	});
	mEditTextBox.LostFocus([this](const IInspectable& sender, const RoutedEventArgs& routedEventArgs){
		// Focus went elsewhere - the edit stands
		endEdit(true, OutlineViewEditEndReason::Other);
	});

	mEditTextBox.Focus(FocusState::Programmatic);
	mEditTextBox.SelectAll();

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::beginEditFirstEditableColumn(uint32_t row)
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate columns
	for (const Column& column : mColumns) {
		// Try
		if (column.mInfo.IsEditable && beginEdit(row, std::wstring(column.mInfo.Identifier)))
			return;
	}
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::endEdit(bool commit, OutlineViewEditEndReason reason)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if editing
	if ((mEditTextBox == nullptr) || mIsEndingEditing)
		return;

	// Take everything down before anyone hears about it - the delegate may
	//	reload the row, and focus moving back may fire LostFocus
	mIsEndingEditing = true;

	TextBox			editTextBox = mEditTextBox;
	std::wstring	text(editTextBox.Text());
	uint32_t		row = *mEditingRow;
	std::wstring	columnIdentifier = *mEditingColumnIdentifier;

	mEditTextBox = nullptr;
	mEditingRow = std::optional<uint32_t>();
	mEditingColumnIdentifier = std::optional<std::wstring>();

	uint32_t	index;
	if (mOverlayCanvas.Children().IndexOf(editTextBox, index))
		mOverlayCanvas.Children().RemoveAt(index);

	if (reason != OutlineViewEditEndReason::Other)
		// Focus comes back to us
		mOutlineView.Focus(FocusState::Programmatic);

	mIsEndingEditing = false;

	// Raise event
	if (commit && mEditEndedEvent && (row < getRowCount()))
		mEditEndedEvent(getProjectedOutlineView(),
				winrt::make<OutlineViewEditEndedEventArgs>(winrt::hstring(mRowEntries[row].mIdentifier),
						winrt::hstring(columnIdentifier), winrt::hstring(text), reason));

	// Carry on to the next cell
	if (commit && ((reason == OutlineViewEditEndReason::Tab) ||
			(reason == OutlineViewEditEndReason::ShiftTab)))
		// Continue
		continueEdit(row, columnIdentifier, reason == OutlineViewEditEndReason::Tab);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::continueEdit(uint32_t row, const std::wstring& columnIdentifier, bool forward)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	int	columnIndex = getColumnIndex(columnIdentifier);
	if (columnIndex < 0)
		return;

	uint32_t	rowCount = getRowCount();
	int			columnCount = (int) mColumns.size();
	int			i = columnIndex;
	uint32_t	currentRow = row;

	// Walk cells until one takes the edit, at most one full lap of rows
	for (uint32_t visited = 0; visited <= rowCount; ) {
		// Next cell
		i += forward ? 1 : -1;
		if (i >= columnCount) {
			// Next row
			i = 0;
			if ((currentRow + 1) >= rowCount)	return;
			currentRow++;
			visited++;
		} else if (i < 0) {
			// Previous row
			i = columnCount - 1;
			if (currentRow == 0)	return;
			currentRow--;
			visited++;
		}

		// Try
		const	Column&	column = mColumns[i];
		if (column.mInfo.IsEditable && beginEdit(currentRow, std::wstring(column.mInfo.Identifier))) {
			// Editing - the row follows
			moveFocus(currentRow, false, false);

			return;
		}
	}
}

// MARK: Instance methods - menus

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Internals::contextRequested(const std::optional<Point>& point)
//----------------------------------------------------------------------------------------------------------------------
{
	// Nothing without a data source
	if (mDataSource == nullptr)
		return;

	// Setup
	Point						position =
										point.has_value() ?
												*point :
												Point{(float) (mViewportWidth / 2.0), (float) (mViewportHeight / 2.0)};
	bool						isColumnHeader = point.has_value() && isInHeader(*point);
	std::optional<std::wstring>	identifier;
	std::optional<std::wstring>	columnIdentifier;
	if (isColumnHeader)
		// Header
		columnIdentifier = getColumnIdentifierAt(point->X + mHOffset);
	else {
		// Content
		if (point.has_value()) {
			// Where was clicked
			mClickedRow = getRowAt(*point);
			mClickedColumnIdentifier = getColumnIdentifierAt(point->X + mHOffset);
		} else {
			// Keyboard - the focused row
			mClickedRow = mFocusedRow;
			mClickedColumnIdentifier = std::optional<std::wstring>();
			if (mFocusedRow.has_value() && (findRealizedRow(*mFocusedRow) != nullptr))
				position = Point{(float) (mViewportWidth / 2.0), (float) (getRowY(*mFocusedRow) + mRowHeight / 2.0)};
		}
		identifier = identifierForRow(mClickedRow);
		columnIdentifier = mClickedColumnIdentifier;
	}

	// Raise event - the handler supplies the menu
	if (!mContextMenuOpeningEvent)
		return;

	winrt::WinUIToolbox::OutlineViewContextMenuOpeningEventArgs	args =
																		winrt::make<
																				OutlineViewContextMenuOpeningEventArgs>(
																				isColumnHeader, sToHString(identifier),
																				sToHString(columnIdentifier));
	mContextMenuOpeningEvent(getProjectedOutlineView(), args);

	// Show
	MenuFlyout	menuFlyout = args.Flyout();
	if (menuFlyout != nullptr)
		// Show
		menuFlyout.ShowAt(mOutlineView, position);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - OutlineView

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
OutlineView::OutlineView() : OutlineViewT<OutlineView>()
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(*this);

	IsTabStop(true);
	UseSystemFocusVisuals(false);
	mInternals->buildChrome();

	// Keyboard
	KeyDown([this](const IInspectable& sender, const KeyRoutedEventArgs& keyRoutedEventArgs){
		// Handle
		if (mInternals->keyDown(keyRoutedEventArgs.Key()))
			// Handled
			keyRoutedEventArgs.Handled(true);
	});
	CharacterReceived(
			[this](const UIElement& sender, const CharacterReceivedRoutedEventArgs& characterReceivedRoutedEventArgs){
				// Handle
				mInternals->characterReceived(characterReceivedRoutedEventArgs.Character());
			});
	GotFocus([this](const IInspectable& sender, const RoutedEventArgs& routedEventArgs){
		// Update
		mInternals->mHasFocus = true;
		mInternals->updateFocusRect();
	});
	LostFocus([this](const IInspectable& sender, const RoutedEventArgs& routedEventArgs){
		// Update
		mInternals->mHasFocus = false;
		mInternals->updateFocusRect();
	});

	// Wheel
	PointerWheelChanged([this](const IInspectable& sender, const PointerRoutedEventArgs& pointerRoutedEventArgs){
		// Setup
		auto	pointerPoint = pointerRoutedEventArgs.GetCurrentPoint(*this);
		auto	properties = pointerPoint.Properties();
		double	notches = (double) properties.MouseWheelDelta() / 120.0;
		bool	isShift =
						sDoesVirtualKeyModifiersContain(pointerRoutedEventArgs.KeyModifiers(),
								VirtualKeyModifiers::Shift);

		// Check direction
		if (properties.IsHorizontalMouseWheel())
			// Sideways
			mInternals->setHOffset(mInternals->mHOffset + notches * kWheelPixelsPerNotch);
		else if (isShift)
			// Sideways, by way of shift
			mInternals->setHOffset(mInternals->mHOffset - notches * kWheelPixelsPerNotch);
		else
			// Up and down
			mInternals->scrollTo(mInternals->mScrollRow - notches * kWheelRowsPerNotch);

		pointerRoutedEventArgs.Handled(true);
	});

	// Menus
	ContextRequested([this](const UIElement& sender, const ContextRequestedEventArgs& contextRequestedEventArgs){
		// Where
		Point	point;
		bool	hasPoint = contextRequestedEventArgs.TryGetPosition(*this, point);

		// Handle
		mInternals->contextRequested(hasPoint ? std::optional<Point>(point) : std::optional<Point>());
		contextRequestedEventArgs.Handled(true);
	});
}

//----------------------------------------------------------------------------------------------------------------------
OutlineView::~OutlineView()
//----------------------------------------------------------------------------------------------------------------------
{
	// Cleanup
	delete mInternals;
}

// MARK: Property methods

//----------------------------------------------------------------------------------------------------------------------
double OutlineView::RowHeight() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mRowHeight;
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::RowHeight(double rowHeight)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (rowHeight == mInternals->mRowHeight)
		return;

	// Update
	mInternals->mRowHeight = std::max(rowHeight, 1.0);
	mInternals->recycleAllRows();
	mInternals->layout();
}

//----------------------------------------------------------------------------------------------------------------------
double OutlineView::ColumnHeaderHeight() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mColumnHeaderHeight;
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::ColumnHeaderHeight(double columnHeaderHeight)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (columnHeaderHeight == mInternals->mColumnHeaderHeight)
		return;

	// Update
	mInternals->mColumnHeaderHeight = std::max(columnHeaderHeight, 0.0);
	mInternals->recycleAllRows();
	mInternals->layout();
}

//----------------------------------------------------------------------------------------------------------------------
OutlineViewStyle OutlineView::Style() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mOutlineStyle;
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::Style(OutlineViewStyle style)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (style == mInternals->mOutlineStyle)
		return;

	// Update
	mInternals->mOutlineStyle = style;
	mInternals->recycleAllRows();
	mInternals->layout();
}

//----------------------------------------------------------------------------------------------------------------------
OutlineViewSelectionMode OutlineView::SelectionMode() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mSelectionMode;
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::SelectionMode(OutlineViewSelectionMode selectionMode)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (selectionMode == mInternals->mSelectionMode)
		return;

	// Update - a narrower mode may leave too much selected
	mInternals->mSelectionMode = selectionMode;
	if (selectionMode == OutlineViewSelectionMode::None)
		// Nothing
		mInternals->deselectAllInternal();
	else if ((selectionMode == OutlineViewSelectionMode::Single) && (mInternals->mSelectedIdentifiers.size() > 1)) {
		// Keep the focused row if it is selected, else the first selected row
		std::optional<std::wstring>	keep = mInternals->identifierForRow(mInternals->mFocusedRow);
		if (!keep.has_value() || (mInternals->mSelectedIdentifiers.count(*keep) == 0))
			keep = mInternals->getSelectedIdentifiersInRowOrder().front();
		mInternals->setSelectedIdentifiers(std::vector<std::wstring>(1, *keep));
	}
}

//----------------------------------------------------------------------------------------------------------------------
bool OutlineView::CanUserReorderColumns() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mCanUserReorderColumns;
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::CanUserReorderColumns(bool canUserReorderColumns)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mCanUserReorderColumns = canUserReorderColumns;
}

//----------------------------------------------------------------------------------------------------------------------
bool OutlineView::CanUserResizeColumns() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mCanUserResizeColumns;
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::CanUserResizeColumns(bool canUserResizeColumns)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mCanUserResizeColumns = canUserResizeColumns;
}

// MARK: Property methods - data source and delegate

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::SetDataSource(const IInspectable& dataSource)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->setDataSource(dataSource);
}

//----------------------------------------------------------------------------------------------------------------------
IInspectable OutlineView::GetDataSource() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mDataSourceObject;
}

//----------------------------------------------------------------------------------------------------------------------
IOutlineViewCellFactory OutlineView::CellFactory() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mCellFactory;
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::CellFactory(const IOutlineViewCellFactory& cellFactory)
//----------------------------------------------------------------------------------------------------------------------
{
	// Update
	mInternals->mCellFactory = cellFactory;

	// Every cell in play came from the old factory
	mInternals->recycleAllRows();
	mInternals->mRowContentCanvas.Children().Clear();
	mInternals->mRecycledElementsByKey.clear();
	mInternals->mChevronPool.clear();
	mInternals->realize();
}

// MARK: Event methods

//----------------------------------------------------------------------------------------------------------------------
winrt::event_token OutlineView::ItemExpanded(const OutlineViewItemEventHandler& handler)
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mItemExpandedEvent.add(handler);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::ItemExpanded(const winrt::event_token& token) noexcept
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mItemExpandedEvent.remove(token);
}

//----------------------------------------------------------------------------------------------------------------------
winrt::event_token OutlineView::ItemCollapsed(const OutlineViewItemEventHandler& handler)
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mItemCollapsedEvent.add(handler);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::ItemCollapsed(const winrt::event_token& token) noexcept
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mItemCollapsedEvent.remove(token);
}

//----------------------------------------------------------------------------------------------------------------------
winrt::event_token OutlineView::ItemInvoked(const OutlineViewItemInvokedEventHandler& handler)
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mItemInvokedEvent.add(handler);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::ItemInvoked(const winrt::event_token& token) noexcept
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mItemInvokedEvent.remove(token);
}

//----------------------------------------------------------------------------------------------------------------------
winrt::event_token OutlineView::SelectionChanged(const OutlineViewSelectionChangedEventHandler& handler)
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mSelectionChangedEvent.add(handler);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::SelectionChanged(const winrt::event_token& token) noexcept
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mSelectionChangedEvent.remove(token);
}

//----------------------------------------------------------------------------------------------------------------------
winrt::event_token OutlineView::SortDescriptionsChanged(const OutlineViewSortDescriptionsChangedEventHandler& handler)
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mSortDescriptionsChangedEvent.add(handler);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::SortDescriptionsChanged(const winrt::event_token& token) noexcept
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mSortDescriptionsChangedEvent.remove(token);
}

//----------------------------------------------------------------------------------------------------------------------
winrt::event_token OutlineView::ColumnReordered(const OutlineViewColumnReorderedEventHandler& handler)
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mColumnReorderedEvent.add(handler);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::ColumnReordered(const winrt::event_token& token) noexcept
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mColumnReorderedEvent.remove(token);
}

//----------------------------------------------------------------------------------------------------------------------
winrt::event_token OutlineView::ColumnResized(const OutlineViewColumnEventHandler& handler)
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mColumnResizedEvent.add(handler);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::ColumnResized(const winrt::event_token& token) noexcept
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mColumnResizedEvent.remove(token);
}

//----------------------------------------------------------------------------------------------------------------------
winrt::event_token OutlineView::BeginningEdit(const OutlineViewBeginningEditEventHandler& handler)
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mBeginningEditEvent.add(handler);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::BeginningEdit(const winrt::event_token& token) noexcept
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mBeginningEditEvent.remove(token);
}

//----------------------------------------------------------------------------------------------------------------------
winrt::event_token OutlineView::EditEnded(const OutlineViewEditEndedEventHandler& handler)
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mEditEndedEvent.add(handler);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::EditEnded(const winrt::event_token& token) noexcept
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mEditEndedEvent.remove(token);
}

//----------------------------------------------------------------------------------------------------------------------
winrt::event_token OutlineView::ContextMenuOpening(const OutlineViewContextMenuOpeningEventHandler& handler)
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mContextMenuOpeningEvent.add(handler);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::ContextMenuOpening(const winrt::event_token& token) noexcept
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mContextMenuOpeningEvent.remove(token);
}

// MARK: Instance methods - columns

//----------------------------------------------------------------------------------------------------------------------
IVectorView<OutlineViewColumn> OutlineView::GetColumns() const
//----------------------------------------------------------------------------------------------------------------------
{
	return winrt::single_threaded_vector<OutlineViewColumn>(mInternals->getOutlineViewColumns()).GetView();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::SetColumns(const IVectorView<OutlineViewColumn>& columns)
//----------------------------------------------------------------------------------------------------------------------
{
	// Collect
	std::vector<OutlineViewColumn>	outlineViewColumns;
	if (columns != nullptr) {
		// Iterate
		outlineViewColumns.reserve(columns.Size());
		for (const OutlineViewColumn& column : columns)
			// Add
			outlineViewColumns.push_back(column);
	}

	// Update
	mInternals->setOutlineViewColumns(outlineViewColumns);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::AddColumn(const OutlineViewColumn& column)
//----------------------------------------------------------------------------------------------------------------------
{
	// Append
	std::vector<OutlineViewColumn>	outlineViewColumns = mInternals->getOutlineViewColumns();
	outlineViewColumns.push_back(column);
	mInternals->setOutlineViewColumns(outlineViewColumns);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::InsertColumn(const OutlineViewColumn& column, uint32_t index)
//----------------------------------------------------------------------------------------------------------------------
{
	// Insert
	std::vector<OutlineViewColumn>	outlineViewColumns = mInternals->getOutlineViewColumns();
	outlineViewColumns.insert(outlineViewColumns.begin() + std::min((size_t) index, outlineViewColumns.size()), column);
	mInternals->setOutlineViewColumns(outlineViewColumns);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::RemoveColumn(const winrt::hstring& columnIdentifier)
//----------------------------------------------------------------------------------------------------------------------
{
	// Remove
	std::vector<OutlineViewColumn>	outlineViewColumns = mInternals->getOutlineViewColumns();
	for (size_t i = 0; i < outlineViewColumns.size(); i++) {
		// Check
		if (outlineViewColumns[i].Identifier == columnIdentifier) {
			// Remove
			outlineViewColumns.erase(outlineViewColumns.begin() + i);
			mInternals->setOutlineViewColumns(outlineViewColumns);

			return;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::MoveColumn(uint32_t fromIndex, uint32_t toIndex)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->moveColumn(fromIndex, toIndex);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::UpdateColumn(const OutlineViewColumn& column)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->updateColumn(column);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::ReloadColumn(const winrt::hstring& columnIdentifier)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->reloadColumn(std::wstring(columnIdentifier));
}

// MARK: Instance methods - sorting

//----------------------------------------------------------------------------------------------------------------------
IVectorView<OutlineViewSortDescription> OutlineView::GetSortDescriptions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->getSortDescriptionsView();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::SetSortDescriptions(const IVectorView<OutlineViewSortDescription>& sortDescriptions)
//----------------------------------------------------------------------------------------------------------------------
{
	// Convert
	std::vector<SortDescriptor>	descriptors;
	if (sortDescriptions != nullptr) {
		// Iterate
		descriptors.reserve(sortDescriptions.Size());
		for (const OutlineViewSortDescription& sortDescriptor : sortDescriptions)
			// Add
			descriptors.push_back(
					SortDescriptor(std::wstring(sortDescriptor.ColumnIdentifier), sortDescriptor.IsAscending));
	}

	// Update
	mInternals->applySortDescriptors(descriptors);
}

// MARK: Instance methods - content

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::ReloadAllItems()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->reloadTree();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::ReloadItemChildren(const winrt::hstring& identifier)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->reloadTree();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::ReloadItem(const winrt::hstring& identifier, const IVectorView<winrt::hstring>& columnIdentifiers)
//----------------------------------------------------------------------------------------------------------------------
{
	// Only what is showing needs anything done
	std::optional<uint32_t>	rowIndex =
									mInternals->rowForIdentifier(std::optional<std::wstring>(std::wstring(identifier)));
	if (!rowIndex.has_value())
		return;

	Internals::RealizedRow*	realizedRow = mInternals->findRealizedRow(*rowIndex);
	if (realizedRow == nullptr)
		return;

	// Reload - an empty column list means every column
	std::vector<std::wstring>	identifiers = sToWStrings(columnIdentifiers);
	mInternals->reloadRow(*realizedRow,
			identifiers.empty() ?
					std::optional<std::vector<std::wstring> >() :
					std::optional<std::vector<std::wstring> >(identifiers));
}

// MARK: Instance methods - rows and tree

//----------------------------------------------------------------------------------------------------------------------
uint32_t OutlineView::GetRowCount() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->getRowCount();
}

//----------------------------------------------------------------------------------------------------------------------
winrt::hstring OutlineView::GetIdentifierAtRow(uint32_t rowIndex) const
//----------------------------------------------------------------------------------------------------------------------
{
	return sToHString(mInternals->identifierForRow(std::optional<uint32_t>(rowIndex)));
}

//----------------------------------------------------------------------------------------------------------------------
IReference<uint32_t> OutlineView::GetRowForIdentifier(const winrt::hstring& identifier) const
//----------------------------------------------------------------------------------------------------------------------
{
	return sToUInt32Reference(mInternals->rowForIdentifier(std::optional<std::wstring>(std::wstring(identifier))));
}

//----------------------------------------------------------------------------------------------------------------------
winrt::hstring OutlineView::GetParentIdentifier(const winrt::hstring& identifier) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Find
	std::optional<uint32_t>	rowIndex =
									mInternals->rowForIdentifier(std::optional<std::wstring>(std::wstring(identifier)));
	if (!rowIndex.has_value())
		return winrt::hstring();

	return sToHString(mInternals->mRowEntries[*rowIndex].mParentIdentifier);
}

//----------------------------------------------------------------------------------------------------------------------
bool OutlineView::IsExpandable(const winrt::hstring& identifier) const
//----------------------------------------------------------------------------------------------------------------------
{
	return (mInternals->mDataSource != nullptr) && mInternals->mDataSource->isExpandable(std::wstring(identifier));
}

//----------------------------------------------------------------------------------------------------------------------
bool OutlineView::IsExpanded(const winrt::hstring& identifier) const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mExpandedIdentifiers.count(std::wstring(identifier)) > 0;
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::ExpandItem(const winrt::hstring& identifier, bool expandChildren)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->setItemExpanded(std::wstring(identifier), true, expandChildren);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::CollapseItem(const winrt::hstring& identifier, bool collapseChildren)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->setItemExpanded(std::wstring(identifier), false, collapseChildren);
}

//----------------------------------------------------------------------------------------------------------------------
IVectorView<winrt::hstring> OutlineView::GetExpandedIdentifiers() const
//----------------------------------------------------------------------------------------------------------------------
{
	return sToHStrings(
			std::vector<std::wstring>(mInternals->mExpandedIdentifiers.begin(),
					mInternals->mExpandedIdentifiers.end()));
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::ScrollRowIntoView(uint32_t rowIndex)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check row
	if (rowIndex < mInternals->getRowCount())
		// Scroll
		mInternals->scrollRowIntoView(rowIndex);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::ScrollItemIntoView(const winrt::hstring& identifier)
//----------------------------------------------------------------------------------------------------------------------
{
	// Find
	std::optional<uint32_t>	rowIndex =
									mInternals->rowForIdentifier(std::optional<std::wstring>(std::wstring(identifier)));
	if (rowIndex.has_value())
		// Scroll
		mInternals->scrollRowIntoView(*rowIndex);
}

// MARK: Instance methods - selection

//----------------------------------------------------------------------------------------------------------------------
uint32_t OutlineView::GetSelectedCount() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (uint32_t) mInternals->mSelectedIdentifiers.size();
}

//----------------------------------------------------------------------------------------------------------------------
IVectorView<winrt::hstring> OutlineView::GetSelectedIdentifiers() const
//----------------------------------------------------------------------------------------------------------------------
{
	return sToHStrings(mInternals->getSelectedIdentifiersInRowOrder());
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::SetSelectedIdentifiers(const IVectorView<winrt::hstring>& identifiers)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->setSelectedIdentifiers(sToWStrings(identifiers));
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::SelectItems(const IVectorView<winrt::hstring>& identifiers, bool byExtendingSelection)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->selectIdentifiers(sToWStrings(identifiers), byExtendingSelection);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::DeselectItems(const IVectorView<winrt::hstring>& identifiers)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->deselectIdentifiers(sToWStrings(identifiers));
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::SelectAll()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->selectAllInternal();
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::DeselectAll()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->deselectAllInternal();
}

//----------------------------------------------------------------------------------------------------------------------
winrt::hstring OutlineView::GetFocusedIdentifier() const
//----------------------------------------------------------------------------------------------------------------------
{
	return sToHString(mInternals->identifierForRow(mInternals->mFocusedRow));
}

//----------------------------------------------------------------------------------------------------------------------
IReference<uint32_t> OutlineView::GetClickedRowIndex() const
//----------------------------------------------------------------------------------------------------------------------
{
	return sToUInt32Reference(mInternals->mClickedRow);
}

//----------------------------------------------------------------------------------------------------------------------
winrt::hstring OutlineView::GetClickedIdentifier() const
//----------------------------------------------------------------------------------------------------------------------
{
	return sToHString(mInternals->identifierForRow(mInternals->mClickedRow));
}

//----------------------------------------------------------------------------------------------------------------------
winrt::hstring OutlineView::GetClickedColumnIdentifier() const
//----------------------------------------------------------------------------------------------------------------------
{
	return sToHString(mInternals->mClickedColumnIdentifier);
}

//----------------------------------------------------------------------------------------------------------------------
IVectorView<winrt::hstring> OutlineView::GetContextMenuIdentifiers() const
//----------------------------------------------------------------------------------------------------------------------
{
	// The menu applies to the selection if there is one, otherwise to the clicked row.  A right-click has already
	//	moved the selection onto the clicked row when it lay outside the selection.
	std::vector<std::wstring>	identifiers;
	if (!mInternals->mSelectedIdentifiers.empty())
		// Selection
		identifiers = mInternals->getSelectedIdentifiersInRowOrder();
	else {
		// Clicked row
		std::optional<std::wstring>	clickedIdentifier = mInternals->identifierForRow(mInternals->mClickedRow);
		if (clickedIdentifier.has_value())
			identifiers.push_back(*clickedIdentifier);
	}

	return sToHStrings(identifiers);
}

// MARK: Instance methods - editing

//----------------------------------------------------------------------------------------------------------------------
bool OutlineView::BeginEdit(const winrt::hstring& identifier, const winrt::hstring& columnIdentifier)
//----------------------------------------------------------------------------------------------------------------------
{
	// Find
	std::optional<uint32_t>	rowIndex =
									mInternals->rowForIdentifier(std::optional<std::wstring>(std::wstring(identifier)));
	if (!rowIndex.has_value())
		return false;

	return mInternals->beginEdit(*rowIndex, std::wstring(columnIdentifier));
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::CommitEdit()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->endEdit(true, OutlineViewEditEndReason::Other);
}

//----------------------------------------------------------------------------------------------------------------------
void OutlineView::CancelEdit()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->endEdit(false, OutlineViewEditEndReason::Other);
}

//----------------------------------------------------------------------------------------------------------------------
bool OutlineView::IsEditing() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mEditTextBox != nullptr;
}

// MARK: Instance methods - cells

//----------------------------------------------------------------------------------------------------------------------
UIElement OutlineView::TryGetRecycledElement(const winrt::hstring& recycleKey)
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->tryGetRecycledElement(std::wstring(recycleKey));
}

//----------------------------------------------------------------------------------------------------------------------
winrt::WinUIToolbox::OutlineViewCell OutlineView::MakeTextCell(const winrt::hstring& text,
		const IReference<Color>& color, bool isBold, const winrt::hstring& toolTip)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	std::wstring	recycleKey(kTextCellRecycleKey);
	UIElement		element = mInternals->tryGetRecycledElement(recycleKey);
	TextBlock		textBlock = (element != nullptr) ? element.try_as<TextBlock>() : TextBlock(nullptr);
	if (textBlock == nullptr) {
		// Create
		textBlock = TextBlock();
		textBlock.TextTrimming(TextTrimming::CharacterEllipsis);
		textBlock.TextWrapping(TextWrapping::NoWrap);
		textBlock.IsHitTestVisible(false);
	}

	// Update
	textBlock.Text(text);
	textBlock.FontWeight(isBold ? FontWeights::SemiBold() : FontWeights::Normal());
	if (color != nullptr)
		// Color
		textBlock.Foreground(SolidColorBrush(color.Value()));
	else
		// Default
		textBlock.Foreground(sBrushFor(L"TextFillColorPrimaryBrush"));
	ToolTipService::SetToolTip(textBlock, !toolTip.empty() ? winrt::box_value(toolTip) : IInspectable(nullptr));

	return winrt::make<OutlineViewCell>(textBlock, winrt::hstring(recycleKey));
}
