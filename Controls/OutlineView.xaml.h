//----------------------------------------------------------------------------------------------------------------------
//	OutlineView.xaml.h			©2026 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "WinUIToolbox.OutlineView.g.h"
#include "WinUIToolbox.OutlineViewBeginningEditEventArgs.g.h"
#include "WinUIToolbox.OutlineViewCell.g.h"
#include "WinUIToolbox.OutlineViewColumnEventArgs.g.h"
#include "WinUIToolbox.OutlineViewColumnReorderedEventArgs.g.h"
#include "WinUIToolbox.OutlineViewContextMenuOpeningEventArgs.g.h"
#include "WinUIToolbox.OutlineViewEditEndedEventArgs.g.h"
#include "WinUIToolbox.OutlineViewItemEventArgs.g.h"
#include "WinUIToolbox.OutlineViewItemInvokedEventArgs.g.h"
#include "WinUIToolbox.OutlineViewSelectionChangedEventArgs.g.h"
#include "WinUIToolbox.OutlineViewSortDescriptionsChangedEventArgs.g.h"

#include "winrt\Microsoft.UI.Xaml.h"
#include "winrt\Microsoft.UI.Xaml.Controls.h"
#include "winrt\Windows.Foundation.h"
#include "winrt\Windows.Foundation.Collections.h"
#include "winrt\Windows.UI.h"

using Color = winrt::Windows::UI::Color;
using IInspectable = winrt::Windows::Foundation::IInspectable;
using MenuFlyout = winrt::Microsoft::UI::Xaml::Controls::MenuFlyout;
using UIElement = winrt::Microsoft::UI::Xaml::UIElement;

using OutlineViewStyle = winrt::WinUIToolbox::OutlineViewStyle;
using OutlineViewSelectionMode = winrt::WinUIToolbox::OutlineViewSelectionMode;
using OutlineViewEditEndReason = winrt::WinUIToolbox::OutlineViewEditEndReason;
using OutlineViewColumn = winrt::WinUIToolbox::OutlineViewColumn;
using OutlineViewSortDescription = winrt::WinUIToolbox::OutlineViewSortDescription;
using IOutlineViewCellFactory = winrt::WinUIToolbox::IOutlineViewCellFactory;

template <typename T> using IReference = winrt::Windows::Foundation::IReference<T>;
template <typename T> using IVectorView = winrt::Windows::Foundation::Collections::IVectorView<T>;
template <typename TSender, typename TArgs> using TypedEventHandler =
		winrt::Windows::Foundation::TypedEventHandler<TSender, TArgs>;

using OutlineViewItemEventHandler =
		TypedEventHandler<winrt::WinUIToolbox::OutlineView, winrt::WinUIToolbox::OutlineViewItemEventArgs>;
using OutlineViewItemInvokedEventHandler =
		TypedEventHandler<winrt::WinUIToolbox::OutlineView, winrt::WinUIToolbox::OutlineViewItemInvokedEventArgs>;
using OutlineViewSelectionChangedEventHandler =
		TypedEventHandler<winrt::WinUIToolbox::OutlineView, winrt::WinUIToolbox::OutlineViewSelectionChangedEventArgs>;
using OutlineViewSortDescriptionsChangedEventHandler =
		TypedEventHandler<winrt::WinUIToolbox::OutlineView,
				winrt::WinUIToolbox::OutlineViewSortDescriptionsChangedEventArgs>;
using OutlineViewColumnEventHandler =
		TypedEventHandler<winrt::WinUIToolbox::OutlineView, winrt::WinUIToolbox::OutlineViewColumnEventArgs>;
using OutlineViewColumnReorderedEventHandler =
		TypedEventHandler<winrt::WinUIToolbox::OutlineView, winrt::WinUIToolbox::OutlineViewColumnReorderedEventArgs>;
using OutlineViewBeginningEditEventHandler =
		TypedEventHandler<winrt::WinUIToolbox::OutlineView, winrt::WinUIToolbox::OutlineViewBeginningEditEventArgs>;
using OutlineViewEditEndedEventHandler =
		TypedEventHandler<winrt::WinUIToolbox::OutlineView, winrt::WinUIToolbox::OutlineViewEditEndedEventArgs>;
using OutlineViewContextMenuOpeningEventHandler =
		TypedEventHandler<winrt::WinUIToolbox::OutlineView,
				winrt::WinUIToolbox::OutlineViewContextMenuOpeningEventArgs>;

//----------------------------------------------------------------------------------------------------------------------
// MARK: winrt::WinUIToolbox::implementation

namespace winrt::WinUIToolbox::implementation {

	// MARK: OutlineViewCell
	struct OutlineViewCell : OutlineViewCellT<OutlineViewCell> {
		// Methods
		public:
						// Lifecycle methods
						OutlineViewCell(const UIElement& element, const hstring& recycleKey) :
							mElement(element), mRecycleKey(recycleKey)
							{}

						// Property methods
			UIElement	Element() const
							{ return mElement; }
			hstring		RecycleKey() const
							{ return mRecycleKey; }

		// Properties
		private:
			UIElement	mElement;
			hstring		mRecycleKey;
	};

	// MARK: - OutlineViewItemEventArgs
	struct OutlineViewItemEventArgs : OutlineViewItemEventArgsT<OutlineViewItemEventArgs> {
		// Methods
		public:
					// Lifecycle methods
					OutlineViewItemEventArgs(const hstring& identifier) : mIdentifier(identifier) {}

					// Property methods
			hstring	Identifier() const
						{ return mIdentifier; }

		// Properties
		private:
			hstring	mIdentifier;
	};

	// MARK: - OutlineViewItemInvokedEventArgs
	struct OutlineViewItemInvokedEventArgs : OutlineViewItemInvokedEventArgsT<OutlineViewItemInvokedEventArgs> {
		// Methods
		public:
					// Lifecycle methods
					OutlineViewItemInvokedEventArgs(const hstring& identifier, const hstring& columnIdentifier) :
						mIdentifier(identifier), mColumnIdentifier(columnIdentifier)
						{}

					// Property methods
			hstring	Identifier() const
						{ return mIdentifier; }
			hstring	ColumnIdentifier() const
						{ return mColumnIdentifier; }

		// Properties
		private:
			hstring	mIdentifier;
			hstring	mColumnIdentifier;
	};

	// MARK: - OutlineViewSelectionChangedEventArgs
	struct OutlineViewSelectionChangedEventArgs :
			OutlineViewSelectionChangedEventArgsT<OutlineViewSelectionChangedEventArgs> {
		// Methods
		public:
									// Lifecycle methods
									OutlineViewSelectionChangedEventArgs(
											const IVectorView<hstring>& selectedIdentifiers) :
										mSelectedIdentifiers(selectedIdentifiers)
										{}

									// Property methods
			IVectorView<hstring>	SelectedIdentifiers() const
										{ return mSelectedIdentifiers; }

		// Properties
		private:
			IVectorView<hstring>	mSelectedIdentifiers;
	};

	// MARK: - OutlineViewSortDescriptionsChangedEventArgs
	struct OutlineViewSortDescriptionsChangedEventArgs :
			OutlineViewSortDescriptionsChangedEventArgsT<OutlineViewSortDescriptionsChangedEventArgs> {
		// Methods
		public:
													// Lifecycle methods
													OutlineViewSortDescriptionsChangedEventArgs(
															const IVectorView<OutlineViewSortDescription>&
																	sortDescriptions) :
														mSortDescriptions(sortDescriptions)
														{}

													// Property methods
			IVectorView<OutlineViewSortDescription>	SortDescriptions() const
														{ return mSortDescriptions; }

		// Properties
		private:
			IVectorView<OutlineViewSortDescription>	mSortDescriptions;
	};

	// MARK: - OutlineViewColumnEventArgs
	struct OutlineViewColumnEventArgs : OutlineViewColumnEventArgsT<OutlineViewColumnEventArgs> {
		// Methods
		public:
					// Lifecycle methods
					OutlineViewColumnEventArgs(const hstring& columnIdentifier) :
						mColumnIdentifier(columnIdentifier)
						{}

					// Property methods
			hstring	ColumnIdentifier() const
						{ return mColumnIdentifier; }

		// Properties
		private:
			hstring	mColumnIdentifier;
	};

	// MARK: - OutlineViewColumnReorderedEventArgs
	struct OutlineViewColumnReorderedEventArgs :
			OutlineViewColumnReorderedEventArgsT<OutlineViewColumnReorderedEventArgs> {
		// Methods
		public:
						// Lifecycle methods
						OutlineViewColumnReorderedEventArgs(const hstring& columnIdentifier, uint32_t oldIndex,
								uint32_t newIndex) :
							mColumnIdentifier(columnIdentifier), mOldIndex(oldIndex), mNewIndex(newIndex)
							{}

						// Property methods
			hstring		ColumnIdentifier() const
							{ return mColumnIdentifier; }
			uint32_t	OldIndex() const
							{ return mOldIndex; }
			uint32_t	NewIndex() const
							{ return mNewIndex; }

		// Properties
		private:
			hstring		mColumnIdentifier;
			uint32_t	mOldIndex;
			uint32_t	mNewIndex;
	};

	// MARK: - OutlineViewBeginningEditEventArgs
	struct OutlineViewBeginningEditEventArgs : OutlineViewBeginningEditEventArgsT<OutlineViewBeginningEditEventArgs> {
		// Methods
		public:
					// Lifecycle methods
					OutlineViewBeginningEditEventArgs(const hstring& identifier, const hstring& columnIdentifier) :
						mIdentifier(identifier), mColumnIdentifier(columnIdentifier), mCancel(false)
						{}

					// Property methods
			hstring	Identifier() const
						{ return mIdentifier; }
			hstring	ColumnIdentifier() const
						{ return mColumnIdentifier; }
			bool	Cancel() const
						{ return mCancel; }
			void	Cancel(bool cancel)
						{ mCancel = cancel; }
			hstring	Text() const
						{ return mText; }
			void	Text(const hstring& text)
						{ mText = text; }

		// Properties
		private:
			hstring	mIdentifier;
			hstring	mColumnIdentifier;
			bool	mCancel;
			hstring	mText;
	};

	// MARK: - OutlineViewEditEndedEventArgs
	struct OutlineViewEditEndedEventArgs : OutlineViewEditEndedEventArgsT<OutlineViewEditEndedEventArgs> {
		// Methods
		public:
										// Lifecycle methods
										OutlineViewEditEndedEventArgs(const hstring& identifier,
												const hstring& columnIdentifier, const hstring& text,
												OutlineViewEditEndReason reason) :
											mIdentifier(identifier), mColumnIdentifier(columnIdentifier), mText(text),
													mReason(reason)
											{}

										// Property methods
			hstring						Identifier() const
											{ return mIdentifier; }
			hstring						ColumnIdentifier() const
											{ return mColumnIdentifier; }
			hstring						Text() const
											{ return mText; }
			OutlineViewEditEndReason	Reason() const
											{ return mReason; }

		// Properties
		private:
			hstring						mIdentifier;
			hstring						mColumnIdentifier;
			hstring						mText;
			OutlineViewEditEndReason	mReason;
	};

	// MARK: - OutlineViewContextMenuOpeningEventArgs
	struct OutlineViewContextMenuOpeningEventArgs :
			OutlineViewContextMenuOpeningEventArgsT<OutlineViewContextMenuOpeningEventArgs> {
		// Methods
		public:
						// Lifecycle methods
						OutlineViewContextMenuOpeningEventArgs(bool isColumnHeader, const hstring& identifier,
								const hstring& columnIdentifier) :
							mIsColumnHeader(isColumnHeader), mIdentifier(identifier),
									mColumnIdentifier(columnIdentifier), mFlyout(nullptr)
							{}

						// Property methods
			bool		IsColumnHeader() const
							{ return mIsColumnHeader; }
			hstring		Identifier() const
							{ return mIdentifier; }
			hstring		ColumnIdentifier() const
							{ return mColumnIdentifier; }
			MenuFlyout	Flyout() const
							{ return mFlyout; }
			void		Flyout(const MenuFlyout& flyout)
							{ mFlyout = flyout; }

		// Properties
		private:
			bool		mIsColumnHeader;
			hstring		mIdentifier;
			hstring		mColumnIdentifier;
			MenuFlyout	mFlyout;
	};

	// MARK: - OutlineView
	/*
		A multi-column, hierarchical, virtualized list.  It draws a column header, and
			below it only the rows that are showing: a fixed row height and a private scrollbar working in rows keep
			every coordinate the framework sees the size of the viewport, whatever the row count.

		The view owns the columns, the tree as a tree of item identifiers, which items are expanded, the flat list of
			rows that follows, and the selection.  The OutlineViewDataSource supplies the tree's shape and applies the
			sort; it is a C++ type, carried in as Object and unwrapped inside.  An IOutlineViewCellFactory supplies the
			cells, and the view raises events for what the user does.  Cells come from a recycle pool keyed by recycle
			key and are populated in code.

		The flow: set the data source, compose the columns and the cell factory, subscribe to events, then
			ReloadAllItems.  Items are named by identifier throughout, never by row index, so a reference stays good as
			rows shuffle.
	*/
	struct OutlineView : OutlineViewT<OutlineView> {
		// Classes
		private:
			class Internals;

		// Methods
		public:
														// Lifecycle methods
														OutlineView();
														~OutlineView();

														// Property methods
			double										RowHeight() const;
			void										RowHeight(double rowHeight);
			double										ColumnHeaderHeight() const;
			void										ColumnHeaderHeight(double columnHeaderHeight);
			OutlineViewStyle							Style() const;
			void										Style(OutlineViewStyle style);
			OutlineViewSelectionMode					SelectionMode() const;
			void										SelectionMode(OutlineViewSelectionMode selectionMode);
			bool										CanUserReorderColumns() const;
			void										CanUserReorderColumns(bool canUserReorderColumns);
			bool										CanUserResizeColumns() const;
			void										CanUserResizeColumns(bool canUserResizeColumns);
			IOutlineViewCellFactory						CellFactory() const;
			void										CellFactory(const IOutlineViewCellFactory& cellFactory);

														// Event methods
			winrt::event_token							ItemExpanded(const OutlineViewItemEventHandler& handler);
			void										ItemExpanded(const winrt::event_token& token) noexcept;

			winrt::event_token							ItemCollapsed(const OutlineViewItemEventHandler& handler);
			void										ItemCollapsed(const winrt::event_token& token) noexcept;

			winrt::event_token							ItemInvoked(const OutlineViewItemInvokedEventHandler& handler);
			void										ItemInvoked(const winrt::event_token& token) noexcept;

			winrt::event_token							SelectionChanged(
																const OutlineViewSelectionChangedEventHandler& handler);
			void										SelectionChanged(const winrt::event_token& token) noexcept;

			winrt::event_token							SortDescriptionsChanged(
																const OutlineViewSortDescriptionsChangedEventHandler&
																		handler);
			void										SortDescriptionsChanged(const winrt::event_token& token)
																noexcept;

			winrt::event_token							ColumnReordered(
																const OutlineViewColumnReorderedEventHandler& handler);
			void										ColumnReordered(const winrt::event_token& token) noexcept;

			winrt::event_token							ColumnResized(const OutlineViewColumnEventHandler& handler);
			void										ColumnResized(const winrt::event_token& token) noexcept;

			winrt::event_token							BeginningEdit(
																const OutlineViewBeginningEditEventHandler& handler);
			void										BeginningEdit(const winrt::event_token& token) noexcept;

			winrt::event_token							EditEnded(const OutlineViewEditEndedEventHandler& handler);
			void										EditEnded(const winrt::event_token& token) noexcept;

			winrt::event_token							ContextMenuOpening(
																const OutlineViewContextMenuOpeningEventHandler&
																		handler);
			void										ContextMenuOpening(const winrt::event_token& token) noexcept;

														// Instance methods
			void										SetDataSource(const IInspectable& dataSource);
			IInspectable								GetDataSource() const;

			IVectorView<OutlineViewColumn>				GetColumns() const;
			void										SetColumns(const IVectorView<OutlineViewColumn>& columns);
			void										AddColumn(const OutlineViewColumn& column);
			void										InsertColumn(const OutlineViewColumn& column, uint32_t index);
			void										RemoveColumn(const hstring& columnIdentifier);
			void										MoveColumn(uint32_t fromIndex, uint32_t toIndex);
			void										UpdateColumn(const OutlineViewColumn& column);
			void										ReloadColumn(const hstring& columnIdentifier);

			IVectorView<OutlineViewSortDescription>		GetSortDescriptions() const;
			void										SetSortDescriptions(
																const IVectorView<OutlineViewSortDescription>&
																		sortDescriptions);

			void										ReloadAllItems();
			void										ReloadItemChildren(const hstring& identifier);
			void										ReloadItem(const hstring& identifier,
																const IVectorView<hstring>& columnIdentifiers);

			uint32_t									GetRowCount() const;
			hstring										GetIdentifierAtRow(uint32_t rowIndex) const;
			IReference<uint32_t>						GetRowForIdentifier(const hstring& identifier) const;
			hstring										GetParentIdentifier(const hstring& identifier) const;
			bool										IsExpandable(const hstring& identifier) const;
			bool										IsExpanded(const hstring& identifier) const;
			void										ExpandItem(const hstring& identifier, bool expandChildren);
			void										CollapseItem(const hstring& identifier, bool collapseChildren);
			IVectorView<hstring>						GetExpandedIdentifiers() const;
			void										ScrollRowIntoView(uint32_t rowIndex);
			void										ScrollItemIntoView(const hstring& identifier);

			uint32_t									GetSelectedCount() const;
			IVectorView<hstring>						GetSelectedIdentifiers() const;
			void										SetSelectedIdentifiers(const IVectorView<hstring>& identifiers);
			void										SelectItems(const IVectorView<hstring>& identifiers,
																bool byExtendingSelection);
			void										DeselectItems(const IVectorView<hstring>& identifiers);
			void										SelectAll();
			void										DeselectAll();
			hstring										GetFocusedIdentifier() const;
			IReference<uint32_t>						GetClickedRowIndex() const;
			hstring										GetClickedIdentifier() const;
			hstring										GetClickedColumnIdentifier() const;
			IVectorView<hstring>						GetContextMenuIdentifiers() const;

			bool										BeginEdit(const hstring& identifier,
																const hstring& columnIdentifier);
			void										CommitEdit();
			void										CancelEdit();
			bool										IsEditing() const;

			UIElement									TryGetRecycledElement(const hstring& recycleKey);
			winrt::WinUIToolbox::OutlineViewCell		MakeTextCell(const hstring& text,
																const IReference<Color>& color, bool isBold,
																const hstring& toolTip);

		// Properties
		private:
			Internals*	mInternals;
	};
}

//----------------------------------------------------------------------------------------------------------------------
// MARK: - winrt::WinUIToolbox::factory_implementation

namespace winrt::WinUIToolbox::factory_implementation {
	// MARK: OutlineViewCell
	struct OutlineViewCell : OutlineViewCellT<OutlineViewCell, implementation::OutlineViewCell> {};

	// MARK: OutlineView
	struct OutlineView : OutlineViewT<OutlineView, implementation::OutlineView> {};
}
