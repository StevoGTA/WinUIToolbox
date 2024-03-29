//----------------------------------------------------------------------------------------------------------------------
//	GroupViewBuilder.h			�2024 Stevo Brock		All rights reserved.
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
									GroupViewBuilder(const hstring& title, double itemLeadingInset,
											double itemTrailingInset);
									GroupViewBuilder(const hstring& title, double itemLeadingInset);
									GroupViewBuilder(const hstring& title);

									// Instance methods
				GroupViewBuilder&	Add(FrameworkElement frameworkElement);
				GroupViewBuilder&	Add(Button& button)
										{ return Add(button.as<FrameworkElement>()); }
				GroupViewBuilder&	Add(Page& page)
										{ return Add(page.as<FrameworkElement>()); }
				GroupViewBuilder&	Add(TextBlock& textBlock)
										{ return Add(textBlock.as<FrameworkElement>()); }
				GroupViewBuilder&	Add(UIElement& uiElement)
										{ return Add(uiElement.as<FrameworkElement>()); }

				GroupViewBuilder&	Add(FrameworkElement frameworkElement, double leadingInset, double trailingInset);
				GroupViewBuilder&	Add(Page& page, double leadingInset, double trailingInset)
										{ return Add(page.as<FrameworkElement>(), leadingInset, trailingInset); }
				GroupViewBuilder&	Add(TextBlock& textBlock, double leadingInset, double trailingInset)
										{ return Add(textBlock.as<FrameworkElement>(), leadingInset, trailingInset); }
				GroupViewBuilder&	Add(UIElement& uiElement, double leadingInset, double trailingInset)
										{ return Add(uiElement.as<FrameworkElement>(), leadingInset, trailingInset); }

				GroupViewBuilder&	Add(const hstring& title, FrameworkElement frameworkElement, double leadingInset,
											double trailingInset);
				GroupViewBuilder&	Add(const hstring& title, Page& page, double leadingInset, double trailingInset)
										{ return Add(title, page.as<FrameworkElement>(), leadingInset, trailingInset); }
				GroupViewBuilder&	Add(const hstring& title, TextBlock& textBlock, double leadingInset,
											double trailingInset)
										{ return Add(title, textBlock.as<FrameworkElement>(), leadingInset,
												trailingInset); }
				GroupViewBuilder&	Add(const hstring& title, UIElement& uiElement, double leadingInset,
											double trailingInset)
										{ return Add(title, uiElement.as<FrameworkElement>(), leadingInset,
												trailingInset); }

				UIElement			getUIElement() const;

									// Class methods
		static	UIElement			composeUIElement(const hstring& title, double itemLeadingInset,
											FrameworkElement frameworkElement);
		static	UIElement			composeUIElement(const hstring& title, double itemLeadingInset, Page& page)
										{ return composeUIElement(title, itemLeadingInset,
												page.as<FrameworkElement>()); }

	// Properties
	private:
		Internals*	mInternals;
};
