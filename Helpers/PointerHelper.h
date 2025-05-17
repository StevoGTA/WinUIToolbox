//----------------------------------------------------------------------------------------------------------------------
//	PointerHelper.h			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include <functional>

#include "winrt\Microsoft.UI.Xaml.Input.h"

using Point = winrt::Windows::Foundation::Point;
using Pointer = winrt::Microsoft::UI::Xaml::Input::Pointer;
using PointerRoutedEventArgs = winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs;
using TimeSpan = winrt::Windows::Foundation::TimeSpan;
using UIElement = winrt::Microsoft::UI::Xaml::UIElement;
using VirtualKeyModifiers = winrt::Windows::System::VirtualKeyModifiers;

//----------------------------------------------------------------------------------------------------------------------
// MARK: PointerHelper

/*
	While WinUI 3 provides pretty robust foundational pointer handling, it doesn't provide higher-level functionality
		like multi-click support, or advanced concepts like promoting clicks to drags after a delay. This class aims
		to provide that functionality, while also providing a simple interface for handling pointer events.
*/

class PointerHelper {
	// Buttons
	public:
		enum Buttons {
			kButtonsLeft	= 0x01,
			kButtonsMiddle	= 0x02,
			kButtonsRight	= 0x04,
		};

	// ReleasedReason
	public:
		enum ReleasedReason {
			kReleasedReasonPressEnd,
			kReleasedReasonDragEnd,
		};

	// Procs
	public:
		typedef	std::function<void(const Point& point, const VirtualKeyModifiers& virtualKeyModifiers)> EnteredProc;
		typedef	std::function<void(const Point& point, const VirtualKeyModifiers& virtualKeyModifiers)> MovedProc;
		typedef	std::function<void(const Point& point, const VirtualKeyModifiers& virtualKeyModifiers)> ExitedProc;

		typedef	std::function<void(const Point& point, Buttons buttons, int pressedCount,
						const VirtualKeyModifiers& virtualKeyModifiers)> PressedProc;
		typedef	std::function<void(const Point& point, bool isInitialDragEvent, Buttons buttons, int pressedCount,
						const VirtualKeyModifiers& virtualKeyModifiers)> DraggedProc;
		typedef	std::function<void(const Point& point, Buttons buttons, int pressedCount, ReleasedReason releasedReason,
						const VirtualKeyModifiers& virtualKeyModifiers)> ReleasedProc;

	// Classes
	private:
		class Internals;

	// Methods
	public:
				// Lifecycle methods
				PointerHelper(const UIElement& uiElement);
				~PointerHelper();

				// Instance methods
		void	enablePromotePressedToDrag(const TimeSpan& timeSpan = std::chrono::milliseconds{150});

		void	setEnteredProc(EnteredProc enteredProc);
		void	setMovedProc(MovedProc movedProc);
		void	setExitedProc(ExitedProc exitedProc);

		void	setPressedProc(PressedProc pressedProc);
		void	setDraggedProc(DraggedProc draggedProc);
		void	setReleasedProc(ReleasedProc releasedProc);

	// Properties
	private:
		Internals*	mInternals;
};
