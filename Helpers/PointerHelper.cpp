//----------------------------------------------------------------------------------------------------------------------
//	PointerHelper.cpp			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "PointerHelper.h"

#include "winrt\Microsoft.UI.Input.h"
#include "winrt\Microsoft.UI.Xaml.h"
#include "winrt\Microsoft.UI.Dispatching.h"
#include "winrt\Windows.Foundation.h"

using DispatcherTimer = winrt::Microsoft::UI::Xaml::DispatcherTimer;
using IInspectable = winrt::Windows::Foundation::IInspectable;
using time_point = std::chrono::steady_clock::time_point;

//----------------------------------------------------------------------------------------------------------------------
// MARK: PointerHelper::Internals

class PointerHelper::Internals {
	public:
		Internals(const UIElement& uiElement) :
			mPressedPoint(0.0f, 0.0f), mPressedVirtualKeyModifiers(VirtualKeyModifiers::None), mPressedCount(0),
					mSentOnDragged(false),
					mEnteredProc([](const Point& point, const VirtualKeyModifiers& virtualKeyModifiers){}),
					mMovedProc([](const Point& point, const VirtualKeyModifiers& virtualKeyModifiers){}),
					mExitedProc([](const Point& point, const VirtualKeyModifiers& virtualKeyModifiers){}),
					mPressedProc(
							[](const Point& point, Buttons buttons, int pressedCount,
									const VirtualKeyModifiers& virtualKeyModifiers){}),
					mDraggedProc(
							[](const Point& point, bool isInitialDragEvent, Buttons buttons, int pressedCount,
									const VirtualKeyModifiers& virtualKeyModifiers){}),
					mReleasedProc(
							[](const Point& point, Buttons buttons, int pressedCount, ReleasedReason releasedReason,
									const VirtualKeyModifiers& virtualKeyModifiers){})
			{
				// Setup event handlers
				uiElement.PointerEntered(
						[this](const IInspectable& sender, const PointerRoutedEventArgs& pointerRoutedEventArgs){
							// Setup
							auto	uiElement = sender.as<UIElement>();
							auto	position = pointerRoutedEventArgs.GetCurrentPoint(uiElement).Position();

							// Call proc
							mEnteredProc(position, pointerRoutedEventArgs.KeyModifiers());
						});
				uiElement.PointerMoved(
						[this](const IInspectable& sender, const PointerRoutedEventArgs& pointerRoutedEventArgs){
							// Setup
							auto	uiElement = sender.as<UIElement>();
							auto	position = pointerRoutedEventArgs.GetCurrentPoint(uiElement).Position();
							auto	properties = pointerRoutedEventArgs.GetCurrentPoint(uiElement).Properties();

							// Check if button is pressed
							if (properties.IsLeftButtonPressed() || properties.IsMiddleButtonPressed() ||
									properties.IsRightButtonPressed()) {
								// Button is pressed
								mPromotePressedToDragDispatchTimer.Stop();

								// Compose buttons
								Buttons	buttons = static_cast<Buttons>(0);
								if (properties.IsLeftButtonPressed())
									buttons = (Buttons) (buttons | kButtonsLeft);
								if (properties.IsMiddleButtonPressed())
									buttons = (Buttons) (buttons | kButtonsMiddle);
								if (properties.IsRightButtonPressed())
									buttons = (Buttons) (buttons | kButtonsRight);

								// Call proc
								mDraggedProc(position, !mSentOnDragged, buttons, mPressedCount,
										pointerRoutedEventArgs.KeyModifiers());

								// Update
								mSentOnDragged = true;
							} else
								// Button is not pressed
								mMovedProc(position, pointerRoutedEventArgs.KeyModifiers());
						});
				uiElement.PointerExited(
						[this](const IInspectable& sender, const PointerRoutedEventArgs& pointerRoutedEventArgs){
							// Setup
							auto	uiElement = sender.as<UIElement>();
							auto	position = pointerRoutedEventArgs.GetCurrentPoint(uiElement).Position();

							// Call proc
							mExitedProc(position, pointerRoutedEventArgs.KeyModifiers());
						});

				uiElement.PointerPressed(
						[this](const IInspectable& sender, const PointerRoutedEventArgs& pointerRoutedEventArgs){
							// Setup
							auto	uiElement = sender.as<UIElement>();
							auto	position = pointerRoutedEventArgs.GetCurrentPoint(uiElement).Position();
							auto	properties = pointerRoutedEventArgs.GetCurrentPoint(uiElement).Properties();

							Buttons	buttons = static_cast<Buttons>(0);
							if (properties.IsLeftButtonPressed())
								buttons = (Buttons) (buttons | kButtonsLeft);
							if (properties.IsMiddleButtonPressed())
								buttons = (Buttons) (buttons | kButtonsMiddle);
							if (properties.IsRightButtonPressed())
								buttons = (Buttons) (buttons | kButtonsRight);

							// Check for multiple press
							auto	now = std::chrono::steady_clock::now();
							auto	deltaTime =
											std::chrono::duration_cast<std::chrono::milliseconds>(
													now - mLastPressedTimePoint).count();
							if ((deltaTime < 500) && (std::abs(position.X - mPressedPoint.X) < 5 &&
									(std::abs(position.Y - mPressedPoint.Y) < 5)))
								// Multiple press
								mPressedCount++;
							else
								// Single press
								mPressedCount = 1;

							// Capture pointer
							uiElement.CapturePointer(pointerRoutedEventArgs.Pointer());

							// Store info
							mPressedButtons = buttons;
							mPressedPoint = position;
							mPressedVirtualKeyModifiers = pointerRoutedEventArgs.KeyModifiers();
							mLastPressedTimePoint = now;
							mSentOnDragged = false;

							// Call proc
							mPressedProc(position, buttons, mPressedCount, mPressedVirtualKeyModifiers);

							// Check if promoting pressed to drag
							if (mPromotePressedToDragDispatchTimer.Interval() != winrt::Windows::Foundation::TimeSpan{0})
								// Start timer
								mPromotePressedToDragDispatchTimer.Start();
						});
				uiElement.PointerReleased(
						[this](const IInspectable& sender, const PointerRoutedEventArgs& pointerRoutedEventArgs){
							// Setup
							auto	uiElement = sender.as<UIElement>();
							auto	properties = pointerRoutedEventArgs.GetCurrentPoint(uiElement).Properties();

							Buttons	buttons = static_cast<Buttons>(0);
							if (properties.IsLeftButtonPressed())
								buttons = (Buttons) (buttons | kButtonsLeft);
							if (properties.IsMiddleButtonPressed())
								buttons = (Buttons) (buttons | kButtonsMiddle);
							if (properties.IsRightButtonPressed())
								buttons = (Buttons) (buttons | kButtonsRight);

							// Suspend timer
							mPromotePressedToDragDispatchTimer.Stop();

							// Call proc
							mReleasedProc(pointerRoutedEventArgs.GetCurrentPoint(uiElement).Position(), buttons,
									mPressedCount, mSentOnDragged ? kReleasedReasonDragEnd : kReleasedReasonPressEnd,
									pointerRoutedEventArgs.KeyModifiers());

							// Release pointer
							uiElement.ReleasePointerCapture(pointerRoutedEventArgs.Pointer());

							// Update
							mSentOnDragged = false;
						});
				uiElement.PointerCanceled(
						[this](const IInspectable& sender, const PointerRoutedEventArgs& pointerRoutedEventArgs){
							// Setup
							auto	uiElement = sender.as<UIElement>();

							// Suspend timer
							mPromotePressedToDragDispatchTimer.Stop();

							// Check if pressed
							if (mPressedCount > 0) {
								// Setup buttons
								auto	uiElement = sender.as<UIElement>();
								auto	properties = pointerRoutedEventArgs.GetCurrentPoint(uiElement).Properties();

								Buttons	buttons = static_cast<Buttons>(0);
								if (properties.IsLeftButtonPressed())
									buttons = (Buttons) (buttons | kButtonsLeft);
								if (properties.IsMiddleButtonPressed())
									buttons = (Buttons) (buttons | kButtonsMiddle);
								if (properties.IsRightButtonPressed())
									buttons = (Buttons) (buttons | kButtonsRight);

								// Call proc
								mReleasedProc(pointerRoutedEventArgs.GetCurrentPoint(uiElement).Position(), buttons,
										mPressedCount,
										mSentOnDragged ? kReleasedReasonDragEnd : kReleasedReasonPressEnd,
										pointerRoutedEventArgs.KeyModifiers());
							}

							// Release pointer
							uiElement.ReleasePointerCapture(pointerRoutedEventArgs.Pointer());

							// Update
							mPressedCount = 0;
							mSentOnDragged = false;
						});
			}
		~Internals()
			{
				// Stop timer
				mPromotePressedToDragDispatchTimer.Stop();
			}

		DispatcherTimer		mPromotePressedToDragDispatchTimer;
		Buttons				mPressedButtons;
		Point				mPressedPoint;
		VirtualKeyModifiers	mPressedVirtualKeyModifiers;
		time_point			mLastPressedTimePoint;
		int					mPressedCount;
		bool				mSentOnDragged;

		EnteredProc			mEnteredProc;
		MovedProc			mMovedProc;
		ExitedProc			mExitedProc;

		PressedProc			mPressedProc;
		DraggedProc			mDraggedProc;
		ReleasedProc		mReleasedProc;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: PointerHelper

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
PointerHelper::PointerHelper(const UIElement& uiElement)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals(uiElement);
}

//----------------------------------------------------------------------------------------------------------------------
PointerHelper::~PointerHelper()
//----------------------------------------------------------------------------------------------------------------------
{
	delete mInternals;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
void PointerHelper::enablePromotePressedToDrag(const TimeSpan& timeSpan)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mPromotePressedToDragDispatchTimer.Interval(timeSpan);
	mInternals->mPromotePressedToDragDispatchTimer.Tick([this](auto const& timer, auto const& args){
		// Stop
		mInternals->mPromotePressedToDragDispatchTimer.Stop();

		// Call proc
		mInternals->mDraggedProc(mInternals->mPressedPoint, !mInternals->mSentOnDragged, mInternals->mPressedButtons,
				mInternals->mPressedCount, mInternals->mPressedVirtualKeyModifiers);

		// Update
		mInternals->mSentOnDragged = true;
	});
}

//----------------------------------------------------------------------------------------------------------------------
void PointerHelper::setEnteredProc(EnteredProc enteredProc)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mEnteredProc = enteredProc;
}

//----------------------------------------------------------------------------------------------------------------------
void PointerHelper::setMovedProc(MovedProc movedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mMovedProc = movedProc;
}

//----------------------------------------------------------------------------------------------------------------------
void PointerHelper::setExitedProc(ExitedProc exitedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mExitedProc = exitedProc;
}

//----------------------------------------------------------------------------------------------------------------------
void PointerHelper::setPressedProc(PressedProc pressedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mPressedProc = pressedProc;
}

//----------------------------------------------------------------------------------------------------------------------
void PointerHelper::setDraggedProc(DraggedProc draggedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mDraggedProc = draggedProc;
}

//----------------------------------------------------------------------------------------------------------------------
void PointerHelper::setReleasedProc(ReleasedProc releasedProc)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mReleasedProc = releasedProc;
}
