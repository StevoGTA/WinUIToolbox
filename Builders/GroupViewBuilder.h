//----------------------------------------------------------------------------------------------------------------------
//	GroupViewBuilder.h			©2024 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "winrt\Microsoft.UI.Xaml.h"
#include "winrt\Microsoft.UI.Xaml.Controls.h"

using namespace winrt;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;

//----------------------------------------------------------------------------------------------------------------------
// MARK: GroupViewBuilder

class GroupViewBuilder {
	// Classes
	private:
		class Internals;

	// Methods
	public:
									// Lifecycle methods
									GroupViewBuilder(double itemLeadingInset, double itemTrailingInset);
									GroupViewBuilder(double itemLeadingInset);
									GroupViewBuilder();
		virtual						~GroupViewBuilder();

									// Instance methods
				GroupViewBuilder&	add(FrameworkElement frameworkElement);
				GroupViewBuilder&	add(Button& button)
										{ return add(button.as<FrameworkElement>()); }
				GroupViewBuilder&	add(Page& page)
										{ return add(page.as<FrameworkElement>()); }
				GroupViewBuilder&	add(TextBlock& textBlock)
										{ return add(textBlock.as<FrameworkElement>()); }
				GroupViewBuilder&	add(UIElement& uiElement)
										{ return add(uiElement.as<FrameworkElement>()); }

				GroupViewBuilder&	add(FrameworkElement frameworkElement, double leadingInset, double trailingInset);
				GroupViewBuilder&	add(Page& page, double leadingInset, double trailingInset)
										{ return add(page.as<FrameworkElement>(), leadingInset, trailingInset); }
				GroupViewBuilder&	add(TextBlock& textBlock, double leadingInset, double trailingInset)
										{ return add(textBlock.as<FrameworkElement>(), leadingInset, trailingInset); }
				GroupViewBuilder&	add(UIElement& uiElement, double leadingInset, double trailingInset)
										{ return add(uiElement.as<FrameworkElement>(), leadingInset, trailingInset); }

				GroupViewBuilder&	add(const hstring& title, FrameworkElement frameworkElement, double leadingInset,
											double trailingInset);
				GroupViewBuilder&	add(const hstring& title, Page& page, double leadingInset, double trailingInset)
										{ return add(title, page.as<FrameworkElement>(), leadingInset, trailingInset); }
				GroupViewBuilder&	add(const hstring& title, TextBlock& textBlock, double leadingInset,
											double trailingInset)
										{ return add(title, textBlock.as<FrameworkElement>(), leadingInset,
												trailingInset); }
				GroupViewBuilder&	add(const hstring& title, UIElement& uiElement, double leadingInset,
											double trailingInset)
										{ return add(title, uiElement.as<FrameworkElement>(), leadingInset,
												trailingInset); }

				uint32_t			getCount() const;
		virtual	UIElement			getUIElement() const;

									// Class methods
		static	UIElement			composeUIElement(double itemLeadingInset, FrameworkElement frameworkElement)
										{ return GroupViewBuilder(itemLeadingInset).add(frameworkElement)
												.getUIElement(); }
		static	UIElement			composeUIElement(double itemLeadingInset, Page& page)
										{ return composeUIElement(itemLeadingInset, page.as<FrameworkElement>()); }

	// Properties
	private:
		Internals*	mInternals;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: ExpanderGroupViewBuilder

class ExpanderGroupViewBuilder : public GroupViewBuilder {
	// Classes
	private:
		class Internals;

	// Methods
	public:
											// Lifecycle methods
											ExpanderGroupViewBuilder(const hstring& title, double itemLeadingInset,
													double itemTrailingInset);
											ExpanderGroupViewBuilder(const hstring& title, double itemLeadingInset);
											ExpanderGroupViewBuilder(const hstring& title);
											~ExpanderGroupViewBuilder();

											// GroupViewBuilder methods
				UIElement					getUIElement() const;

											// Instance methods
				ExpanderGroupViewBuilder&	setExpanded(bool isExpanded);

											// Class methods
		static	UIElement					composeUIElement(const hstring& title, double itemLeadingInset,
													FrameworkElement frameworkElement)
												{ return ExpanderGroupViewBuilder(title, itemLeadingInset)
														.add(frameworkElement)
														.getUIElement(); }
		static	UIElement					composeUIElement(const hstring& title, double itemLeadingInset, Page& page)
												{ return composeUIElement(title, itemLeadingInset,
														page.as<FrameworkElement>()); }

	// Properties
	private:
		Internals*	mInternals;
};
