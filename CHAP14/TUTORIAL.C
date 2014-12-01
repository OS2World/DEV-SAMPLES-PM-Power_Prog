// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      TUTORIAL.C                                       mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Source File for the TUTORIAL control library, part of         บ
// บ                 the sample code for Chapter 14.                               บ
// บ                                                                               บ
// บ  NOTES:         This code forms the dynamic link library which implements     บ
// บ                 the tutorial control window class.                            บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
#define INCL_PM
#include <os2.h>
#include "tutorial.h"

// --------------------------------------------------------------------------
// Definitions for the window instance data
// --------------------------------------------------------------------------
#define     WINWORD_WIN1DATA     0
#define     WINWORD_WIN2DATA     4
#define     WINWORD_TIMERMS      8
#define     WINWORD_TIMING      12

// --------------------------------------------------------------------------
// Definition of the automation timer id
// --------------------------------------------------------------------------
#define     ID_TIMER             (TID_USERMAX - 1)

// --------------------------------------------------------------------------
// Prototype for the main window procedure
// --------------------------------------------------------------------------
MRESULT APIENTRY TutorWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

// --------------------------------------------------------------------------
// Prototypes for the message processing worker functions
// --------------------------------------------------------------------------
MRESULT wmButton1Click( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wmButton1DblClk( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wmChar( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wmDestroy( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wmMouseMove( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wmPaint( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wmSetFocus( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wmSize( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wmTimer( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT tmSetData( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT tmAutomate( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT tmQueryData( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT tmQueryState( HWND hwnd, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------
// Prototypes for miscellaneous utility functions
// --------------------------------------------------------------------------
static void DrawPanel( HPS hps, PVOID pData, BOOL bBitmap, PRECTL pRectl );

// *******************************************************************************
// FUNCTION:     RegisterTutorialControl
//
// FUNCTION USE: Registration of the Tutorial Control window class
//
// DESCRIPTION:  This function is the only function exported from the dynamic
//               link library.  This function must be called by applications
//               using the control to register the class before attempting to
//               create any tutorial controls
//
// PARAMETERS:   HAB       Thread anchor block handle
//
// RETURNS:      BOOL      Result of the WinRegisterClass API call.
//
// *******************************************************************************
BOOL EXPENTRY RegisterTutorialControl(HAB hab)
{
// --------------------------------------------------------------------------
// Register the class.  TutorWinProc handles messages send to tutorial
// control windows, and 16 bytes are reserved in the window instance data.
// --------------------------------------------------------------------------
   return WinRegisterClass( hab, "TUTORIAL", TutorWinProc,
                            CS_SIZEREDRAW, 16 );
}

// *******************************************************************************
// FUNCTION:     TutorWinProc
//
// FUNCTION USE: Handle messages sent to the TUTORIAL control.
//
// DESCRIPTION:  This procedure processes all messages arriving for the windows
//               of the TUTORIAL class.  In this implementation the window
//               procedure's purpose is to resolve the proper method to be
//               called for the message.
//
// PARAMETERS:   HWND     control window handle
//               ULONG    window message
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  WinDefWindowProc for all unprocessed messages;
//                        otherwise, message dependent
//
// *******************************************************************************
MRESULT APIENTRY TutorWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg) {
   case TM_SETDATA:        return tmSetData( hwnd, mp1, mp2 );
   case TM_AUTOMATE:       return tmAutomate( hwnd, mp1, mp2 );
   case TM_QUERYDATA:      return tmQueryData( hwnd, mp1, mp2 );
   case TM_QUERYSTATE:     return tmQueryState( hwnd, mp1, mp2 );
   case WM_BUTTON1CLICK:   return wmButton1Click( hwnd, mp1, mp2 );
   case WM_BUTTON1DBLCLK:  return wmButton1DblClk( hwnd, mp1, mp2 );
   case WM_CHAR:           return wmChar( hwnd, mp1, mp2 );
   case WM_CREATE:         return wmCreate( hwnd, mp1, mp2 );
   case WM_DESTROY:        return wmDestroy( hwnd, mp1, mp2 );
   case WM_MOUSEMOVE:      return wmMouseMove( hwnd, mp1, mp2 );
   case WM_PAINT:          return wmPaint( hwnd, mp1, mp2 );
   case WM_SETFOCUS:       return wmSetFocus( hwnd, mp1, mp2 );
   case WM_SIZE:       return wmSize( hwnd, mp1, mp2 );
   case WM_TIMER:          return wmTimer( hwnd, mp1, mp2 );
   default:                return WinDefWindowProc( hwnd, msg, mp1, mp2 );
   } /* endswitch */
}

// *******************************************************************************
// FUNCTION:     tmSetData
//
// FUNCTION USE: Handles the TM_SETDATA message.
//
// DESCRIPTION:  This message allows the application to modify the control data
//               for the control.  This function retrieves the elements of the
//               control data structure sent by the application and stores them
//               in the window instance data of the control.
//
// PARAMETERS:   HWND     control window handle
//               MPARAM   pointer to a TUTORCDATA structure
//               MPARAM   flag indicating that redraw should occur after the
//                        data is modified.
//
// RETURNS:      MRESULT  TRUE if successful, FALSE if an error occurs.
//
// *******************************************************************************
MRESULT tmSetData( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PTUTORCDATA pCdata = (PTUTORCDATA)mp1;
   MRESULT     mr = MRFROMLONG(FALSE);

// --------------------------------------------------------------------------
// Ensure that a pointer was passed and that the indicated size of the
// data is correct.
// --------------------------------------------------------------------------
   if (pCdata != NULL) {
      if (pCdata->cb == sizeof( TUTORCDATA)) {

// --------------------------------------------------------------------------
// Extract the elements from the structure and store in instance data
// --------------------------------------------------------------------------
         WinSetWindowPtr( hwnd, WINWORD_WIN1DATA, pCdata->hWin1Data );
         WinSetWindowPtr( hwnd, WINWORD_WIN2DATA, pCdata->hWin2Data );
         WinSetWindowULong( hwnd, WINWORD_TIMERMS, pCdata->ulTimerMS );

// --------------------------------------------------------------------------
// set the result code
// --------------------------------------------------------------------------
         mr = MRFROMLONG(TRUE);

// --------------------------------------------------------------------------
// invalidate control window if requested.
// --------------------------------------------------------------------------
         if( (BOOL)LONGFROMMP(mp2) ) WinInvalidateRect( hwnd, NULL, TRUE );
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// return the result code
// --------------------------------------------------------------------------
   return mr;
}

// *******************************************************************************
// FUNCTION:     tmQueryData
//
// FUNCTION USE: Handles the TM_QUERYDATA message.
//
// DESCRIPTION:  This message allows the application to retrieve the control data
//               for the control.  This function retrieves the elements of the
//               control data structure from the window instance data of the
//               control and stores them in the structure provided by the
//               application.
//
// PARAMETERS:   HWND     control window handle
//               MPARAM   pointer to a TUTORCDATA structure
//               MPARAM   reserved
//
// RETURNS:      MRESULT  TRUE if successful, FALSE if an error occurs.
//
// *******************************************************************************
MRESULT tmQueryData( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PTUTORCDATA pCdata = (PTUTORCDATA)mp1;
   MRESULT     mr = MRFROMLONG(FALSE);

// --------------------------------------------------------------------------
// Ensure that a pointer was passed and that the indicated size of the
// data is correct.
// --------------------------------------------------------------------------
   if (pCdata != NULL) {
      if (pCdata->cb == sizeof( TUTORCDATA)) {

// --------------------------------------------------------------------------
// Extract the elements from the instance data and store in structure
// provided by the application.
// --------------------------------------------------------------------------
         pCdata->hWin1Data = WinQueryWindowPtr( hwnd, WINWORD_WIN1DATA );
         pCdata->hWin2Data = WinQueryWindowPtr( hwnd, WINWORD_WIN2DATA );
         pCdata->ulTimerMS = WinQueryWindowULong( hwnd, WINWORD_TIMERMS );

// --------------------------------------------------------------------------
// set the result code
// --------------------------------------------------------------------------
         mr = MRFROMLONG(TRUE);
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// return the result code
// --------------------------------------------------------------------------
   return mr;
}

// *******************************************************************************
// FUNCTION:     tmAutomate
//
// FUNCTION USE: Handles the TM_AUTOMATE message.
//
// DESCRIPTION:  This message allows the application to start and stop the
//               automation feature of the control.  When this feature is
//               enabled, a TN_CLICKED notification is sent to the control's
//               owner at specified intervals.
//
// PARAMETERS:   HWND     control window handle
//               MPARAM   BOOLEAN value indicating that automation should
//                        be started when TRUE or stopped when FALSE.
//               MPARAM   reserved
//
// RETURNS:      MRESULT  TRUE if successful, FALSE if an error occurs.
//
// *******************************************************************************
MRESULT tmAutomate( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   BOOL     bAutomate = (BOOL)LONGFROMMP(mp1);
   ULONG    flStyle = WinQueryWindowULong( hwnd, QWL_STYLE );
   ULONG    timerID;
   ULONG    ulTimerMS = WinQueryWindowULong( hwnd, WINWORD_TIMERMS );
   MRESULT  mr = MRFROMLONG(FALSE);

// --------------------------------------------------------------------------
// If automation start requested and automation as been enabled.
// --------------------------------------------------------------------------
   if (bAutomate && flStyle & TS_AUTO) {

// --------------------------------------------------------------------------
// Start a timer running which expires at the interval specified in the
// control data.
// --------------------------------------------------------------------------
      timerID = WinStartTimer( WinQueryAnchorBlock( hwnd ),
                               hwnd,
                               ID_TIMER,
                               ulTimerMS );

// --------------------------------------------------------------------------
// if successful, set successful return code and set flag in the window
// window instance data to indicate that automation is active.
// --------------------------------------------------------------------------
      if (timerID) {
         mr = MRFROMLONG(TRUE);
         WinSetWindowULong( hwnd, WINWORD_TIMING, 1L );
      } /* endif */

// --------------------------------------------------------------------------
// If automation stop was requested.
// --------------------------------------------------------------------------
   } else if (!bAutomate) {

// --------------------------------------------------------------------------
// ensure that automation is in progress.
// --------------------------------------------------------------------------
      if( WinQueryWindowULong( hwnd, WINWORD_TIMING )) {

// --------------------------------------------------------------------------
// then stop the timer, setting return code to success of operation.
// --------------------------------------------------------------------------
         mr = MRFROMLONG(WinStopTimer( WinQueryAnchorBlock( hwnd ),
                                       hwnd, ID_TIMER ));

// --------------------------------------------------------------------------
// set flag to indicate that automation is not active
// --------------------------------------------------------------------------
         WinSetWindowULong( hwnd, WINWORD_TIMING, 0L );
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// return the result code
// --------------------------------------------------------------------------
   return mr;
}

// *******************************************************************************
// FUNCTION:     tmQueryState
//
// FUNCTION USE: Handles the TM_QUERYSTATE message.
//
// DESCRIPTION:  This function returns the state of the automation in progress
//               flag to the application
//
// PARAMETERS:   HWND     control window handle
//               MPARAM   reserved
//               MPARAM   reserved
//
// RETURNS:      MRESULT  TRUE if automation is running, FALSE if automation
//                        is not running.
//
// *******************************************************************************
MRESULT tmQueryState( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
// --------------------------------------------------------------------------
// return the setting of the automation flag in the window instance data
// --------------------------------------------------------------------------
   return MRFROMLONG(WinQueryWindowULong( hwnd, WINWORD_TIMING));
}

// *******************************************************************************
// FUNCTION:     wmButton1Click
//
// FUNCTION USE: Handles the WM_BUTTON1CLICK message.
//
// DESCRIPTION:  Sends a WM_CONTROL message to the control's owner window,
//               notifying it that a button click has occurred, and that
//               the click indicates a backward move if in the left half
//               of the control window or a forward move if in the right
//               half of the window
//
// PARAMETERS:   HWND     control window handle
//               MPARAM   POINTS structure indicating the pointer location
//                        at the time of the click.
//               MPARAM   not used by this function
//
// RETURNS:      MRESULT  BOOLEAN value indicating if the message was processed
//
// *******************************************************************************
MRESULT wmButton1Click( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   POINTS      pts = *((POINTS*)&mp1);
   RECTL       rectl;
   ULONG       direction;
   HWND        hwndOwner;
   USHORT      usId;

// --------------------------------------------------------------------------
// get the control window rectangle
// --------------------------------------------------------------------------
   WinQueryWindowRect( hwnd, &rectl );

// --------------------------------------------------------------------------
// Set flag based on the position of the pointer
// --------------------------------------------------------------------------
   if ((LONG)pts.x > rectl.xRight/2) {
      direction = TA_FORWARD;
   } else {
      direction = TA_BACKWARD;
   }

// --------------------------------------------------------------------------
// Get the owner window handle and our ID
// --------------------------------------------------------------------------
   hwndOwner = WinQueryWindow( hwnd, QW_OWNER );
   usId = WinQueryWindowUShort( hwnd, QWS_ID );

// --------------------------------------------------------------------------
// If there is an owner window, send the notification
// --------------------------------------------------------------------------
   if (hwndOwner != NULLHANDLE) {
      WinSendMsg( hwndOwner, WM_CONTROL,
                  MPFROM2SHORT( usId, TN_CLICKED ),
                  MPFROMLONG(direction));
   }

// --------------------------------------------------------------------------
// indicate that the message was processed.
// --------------------------------------------------------------------------
   return MRFROMLONG(TRUE);
}

// *******************************************************************************
// FUNCTION:     wmButton1DblClk
//
// FUNCTION USE: Handles the WM_BUTTON1DBLCLK message.
//
// DESCRIPTION:  Sends a TN_ENTER notification to the control's owner when
//               a Button 1 double click occurs.
//
// PARAMETERS:   HWND     control window handle
//               MPARAM   not used by this function
//               MPARAM   not used by this function
//
// RETURNS:      MRESULT  BOOLEAN value indicating if the message was processed
//
// *******************************************************************************
MRESULT wmButton1DblClk( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND        hwndOwner;
   USHORT      usId;

// --------------------------------------------------------------------------
// Get the owner window handle and our ID
// --------------------------------------------------------------------------
   hwndOwner = WinQueryWindow( hwnd, QW_OWNER );
   usId = WinQueryWindowUShort( hwnd, QWS_ID );

// --------------------------------------------------------------------------
// If there is an owner window, send the notification
// --------------------------------------------------------------------------
   if (hwndOwner != NULLHANDLE) {
      WinSendMsg( hwndOwner, WM_CONTROL,
                  MPFROM2SHORT( usId, TN_ENTER ),
                  MPFROMHWND(hwnd));
   }

// --------------------------------------------------------------------------
// indicate that the message was processed.
// --------------------------------------------------------------------------
   return MRFROMLONG(TRUE);
}

// *******************************************************************************
// FUNCTION:     wmChar
//
// FUNCTION USE: Handles the WM_CHAR message.
//
// DESCRIPTION:  Sends a TN_ENTER notification to the control's owner window
//               if either the ENTER or NEWLINE keys is pressed.
//
// PARAMETERS:   HWND     control window handle
//               MPARAM   keystroke flags in low-order 16 bits.
//               MPARAM   virtual key code in high-order 16 bits
//
// RETURNS:      MRESULT  BOOLEAN value indicating if the message was processed
//
// *******************************************************************************
MRESULT wmChar( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND        hwndOwner;
   USHORT      usId;
   USHORT      fsFlags;
   UCHAR       ucChar;

// --------------------------------------------------------------------------
// Extract the flags and virtual key code from the message parameters
// --------------------------------------------------------------------------
   fsFlags = SHORT1FROMMP(mp1);
   ucChar = CHAR3FROMMP(mp2);

// --------------------------------------------------------------------------
// Only process DOWN strokes which represent the ENTER or NEWLINE keys
// without the SHIFT, CONTROL, or ALT key
// --------------------------------------------------------------------------
   if( fsFlags & KC_VIRTUALKEY &&
       !(fsFlags & (KC_CTRL | KC_ALT | KC_SHIFT | KC_KEYUP )) &&
       ( ucChar == VK_ENTER || ucChar == VK_NEWLINE )) {

// --------------------------------------------------------------------------
// Get the owner window handle and our ID
// --------------------------------------------------------------------------
      hwndOwner = WinQueryWindow( hwnd, QW_OWNER );
      usId = WinQueryWindowUShort( hwnd, QWS_ID );

// --------------------------------------------------------------------------
// If there is an owner window, send the notification
// --------------------------------------------------------------------------
      if (hwndOwner != NULLHANDLE) {
         WinSendMsg( hwndOwner, WM_CONTROL,
                     MPFROM2SHORT( usId, TN_ENTER ),
                     MPFROMHWND(hwnd));
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// indicate that the message was processed.
// --------------------------------------------------------------------------
   return MRFROMLONG(TRUE);
}

// *******************************************************************************
// FUNCTION:     wmCreate
//
// FUNCTION USE: Handles the WM_CREATE message.
//
// DESCRIPTION:  Extracts the individual elements of the control data passed
//               to WinCreateWindow and stores them in the window instance
//               data for the control
//
// PARAMETERS:   HWND     control window handle
//               MPARAM   pointer to a TUTORCDATA structure
//               MPARAM   not used by this function
//
// RETURNS:      MRESULT  BOOLEAN value if an error occurred.
//
// *******************************************************************************
MRESULT wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PTUTORCDATA pTCD = (PTUTORCDATA)mp1;
   MRESULT     mr = MRFROMLONG(FALSE);

// --------------------------------------------------------------------------
// Was a control data structure passed?
// --------------------------------------------------------------------------
   if (pTCD != NULL) {

// --------------------------------------------------------------------------
// Ensure that the structure is the proper size.
// --------------------------------------------------------------------------
      if (pTCD->cb == sizeof( TUTORCDATA)) {

// --------------------------------------------------------------------------
// Move the structure elements to the window instance data
// --------------------------------------------------------------------------
         WinSetWindowPtr( hwnd, WINWORD_WIN1DATA, pTCD->hWin1Data );
         WinSetWindowPtr( hwnd, WINWORD_WIN2DATA, pTCD->hWin2Data );
         WinSetWindowULong( hwnd, WINWORD_TIMERMS, pTCD->ulTimerMS );
      } else {

// --------------------------------------------------------------------------
// Invalid structure length, abort the window creation
// --------------------------------------------------------------------------
         mr = MRFROMLONG(TRUE);
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// return error indicator
// --------------------------------------------------------------------------
   return mr;
}

// *******************************************************************************
// FUNCTION:     wmDestroy
//
// FUNCTION USE: Handles the WM_DESTROY message.
//
// DESCRIPTION:  Stops the timer, if running.
//
// PARAMETERS:   HWND     control window handle
//               MPARAM   not used by this function
//               MPARAM   not used by this function
//
// RETURNS:      MRESULT  reserved value
//
// *******************************************************************************
MRESULT wmDestroy( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{

// --------------------------------------------------------------------------
// If automation is in progress
// --------------------------------------------------------------------------
   if (WinQueryWindowULong( hwnd, WINWORD_TIMING)) {
// --------------------------------------------------------------------------
// Stop the automation timer
// --------------------------------------------------------------------------
      WinStopTimer( WinQueryAnchorBlock( hwnd ), hwnd, ID_TIMER );
   } /* endif */

// --------------------------------------------------------------------------
// return the reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wmMouseMove
//
// FUNCTION USE: Handles the WM_MOUSEMOVE message.
//
// DESCRIPTION:  Retrieves the arrow pointer and passes the pointer handle to
//               the owner window in WM_CONTROLPOINTER message, allowing the
//               owner to specify a different pointer if desired.  Sets the
//               pointer to the handle specified by the owner window.
//
// PARAMETERS:   HWND     control window handle
//               MPARAM   not used by this function
//               MPARAM   not used by this function
//
// RETURNS:      MRESULT  reserved, set to zero
//
// *******************************************************************************
MRESULT wmMouseMove( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HPOINTER     hptr;

// --------------------------------------------------------------------------
// Get the system arrow pointer
// --------------------------------------------------------------------------
   hptr = WinQuerySysPointer( HWND_DESKTOP, SPTR_ARROW, FALSE );

// --------------------------------------------------------------------------
// Pass this on to the owner as the default pointer
// --------------------------------------------------------------------------
   hptr = (HPOINTER)WinSendMsg( WinQueryWindow( hwnd, QW_OWNER ),
                      WM_CONTROLPOINTER,
                      MPFROMSHORT( WinQueryWindowUShort( hwnd, QWS_ID )),
                      MPFROMLONG( hptr ));

// --------------------------------------------------------------------------
// Set pointer to the value specified by the owner
// --------------------------------------------------------------------------
   WinSetPointer( HWND_DESKTOP, hptr );

// --------------------------------------------------------------------------
// return reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wmPaint
//
// FUNCTION USE: Handles the WM_PAINT message.
//
// DESCRIPTION:  Draws the contents of the control window
//
// PARAMETERS:   HWND     control window handle
//               MPARAM   not used by this function
//               MPARAM   not used by this function
//
// RETURNS:      MRESULT  reserved, set to zero
//
// *******************************************************************************
MRESULT wmPaint( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HPS      hps;
   RECTL    rectl;
   RECTL    rectl1;
   ULONG    flStyle = WinQueryWindowULong( hwnd, QWL_STYLE );

// --------------------------------------------------------------------------
// Start the paint operation - obtaining a presentation space for drawing
// --------------------------------------------------------------------------
   if ((hps=WinBeginPaint(hwnd, NULLHANDLE, &rectl)) != NULLHANDLE ) {

// --------------------------------------------------------------------------
// fill in the background of the control window
// --------------------------------------------------------------------------
      WinFillRect( hps, &rectl, SYSCLR_DIALOGBACKGROUND );

// --------------------------------------------------------------------------
// Get the window rectangle
// --------------------------------------------------------------------------
      WinQueryWindowRect( hwnd, &rectl );

// --------------------------------------------------------------------------
// draw a border around the inside of the window
// --------------------------------------------------------------------------
      WinDrawBorder( hps, &rectl, 1, 1, CLR_DARKGRAY, CLR_WHITE, 0 );
      WinInflateRect( WinQueryAnchorBlock(hwnd),&rectl, -1, -1 );
      WinDrawBorder( hps, &rectl, 1, 1, CLR_PALEGRAY, CLR_WHITE, 0 );
      WinInflateRect( WinQueryAnchorBlock(hwnd),&rectl, -1, -1 );
      WinDrawBorder( hps, &rectl, 1, 1, CLR_WHITE, CLR_WHITE, 0 );
      WinInflateRect( WinQueryAnchorBlock(hwnd),&rectl, -1, -1 );
      WinDrawBorder( hps, &rectl, 1, 1, CLR_PALEGRAY, CLR_WHITE, 0 );
      WinInflateRect( WinQueryAnchorBlock(hwnd),&rectl, -1, -1 );
      WinDrawBorder( hps, &rectl, 1, 1, CLR_DARKGRAY, CLR_WHITE, 0 );

      if (flStyle & TS_SPLITHORZ) {
// --------------------------------------------------------------------------
// do drawing for a horizontally split window - start with the top half
// --------------------------------------------------------------------------
         rectl1 = rectl;
         rectl1.yBottom = rectl1.yTop/2;
         WinInflateRect( WinQueryAnchorBlock(hwnd),&rectl1, -5, -5 );
         DrawPanel( hps, WinQueryWindowPtr( hwnd, WINWORD_WIN1DATA ), flStyle & TS_PANL1BITMAP,
                    &rectl1 );

// --------------------------------------------------------------------------
// now draw the bottom half
// --------------------------------------------------------------------------
         rectl1 = rectl;
         rectl1.yTop /= 2;
         WinInflateRect( WinQueryAnchorBlock(hwnd),&rectl1, -5, -5 );
         DrawPanel( hps, WinQueryWindowPtr( hwnd, WINWORD_WIN2DATA), flStyle & TS_PANL2BITMAP,
                    &rectl1 );
      } else if (flStyle & TS_SPLITVERT) {
// --------------------------------------------------------------------------
// do drawing for a vertically split window - start with the right half
// --------------------------------------------------------------------------
         rectl1 = rectl;
         rectl1.xLeft = rectl1.xRight/2;
         WinInflateRect( WinQueryAnchorBlock(hwnd),&rectl1, -5, -5 );
         DrawPanel( hps, WinQueryWindowPtr( hwnd, WINWORD_WIN2DATA ), flStyle & TS_PANL2BITMAP,
                    &rectl1 );

// --------------------------------------------------------------------------
// now draw the left half
// --------------------------------------------------------------------------
         rectl1 = rectl;
         rectl1.xRight /= 2;
         WinInflateRect( WinQueryAnchorBlock(hwnd),&rectl1, -5, -5 );
         DrawPanel( hps, WinQueryWindowPtr( hwnd, WINWORD_WIN1DATA), flStyle & TS_PANL1BITMAP,
                    &rectl1 );
      } else {
// --------------------------------------------------------------------------
// do drawing for a non-split window
// --------------------------------------------------------------------------
         rectl1 = rectl;
         WinInflateRect( WinQueryAnchorBlock(hwnd),&rectl1, -5, -5 );
         DrawPanel( hps, WinQueryWindowPtr( hwnd, WINWORD_WIN1DATA), flStyle & TS_PANL1BITMAP,
                    &rectl1 );
      } /* endif */

// --------------------------------------------------------------------------
// terminate the paint operation and release the presentation space
// --------------------------------------------------------------------------
      WinEndPaint( hps );
   } /* endif */

// --------------------------------------------------------------------------
// return the reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wmSetFocus
//
// FUNCTION USE: Handles the WM_SETFOCUS message.
//
// DESCRIPTION:  Notifies the owner window when the control is either
//               receiving or losing the focus
//
// PARAMETERS:   HWND     control window handle
//               MPARAM   not used by this function
//               MPARAM   BOOLEAN indicating that focus is being received
//                        in the low order 16 bits.
//
// RETURNS:      MRESULT  reserved, set to zero
//
// *******************************************************************************
MRESULT wmSetFocus( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND     hwndOwner = WinQueryWindow( hwnd, QW_OWNER );
   USHORT   usId = WinQueryWindowUShort( hwnd, QWS_ID );
   RECTL    rectl;

   if (SHORT1FROMMP(mp2)) {
// --------------------------------------------------------------------------
// The window is receiving the focus, send a TN_SETFOCUS notification to
// the owner window if there is one.
// --------------------------------------------------------------------------
      if (hwndOwner != NULLHANDLE) {
         WinSendMsg( hwndOwner, WM_CONTROL,
                     MPFROM2SHORT( usId, TN_SETFOCUS ),
                     MPFROMHWND( hwnd ));
      } /* endif */

// --------------------------------------------------------------------------
// Obtain the control window area and draw a cursor inside the border of the
// window.
// --------------------------------------------------------------------------
      WinQueryWindowRect( hwnd, &rectl );
      WinInflateRect( WinQueryAnchorBlock(hwnd), &rectl, -5, -5 );
      WinCreateCursor( hwnd, rectl.xLeft, rectl.yBottom,
                       rectl.xRight - rectl.xLeft,
                       rectl.yTop - rectl.yBottom,
                       CURSOR_FRAME, NULL );
      WinShowCursor( hwnd, TRUE );

   } else {
// --------------------------------------------------------------------------
// The window is losing the focus, send a TN_KILLFOCUS notification to the
// owner window if there is one and destory the cursor
// --------------------------------------------------------------------------
      if (hwndOwner != NULLHANDLE) {
         WinSendMsg( hwndOwner, WM_CONTROL,
                     MPFROM2SHORT( usId, TN_KILLFOCUS ),
                     MPFROMHWND( hwnd ));
      } /* endif */
      WinDestroyCursor( hwnd );
   } /* endif */

// --------------------------------------------------------------------------
// return the reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wmSize
//
// FUNCTION USE: Handles the WM_SIZE message.
//
// DESCRIPTION:  Updates the size of the input cursor if the control currently
//               has the focus.
//
// PARAMETERS:   HWND     control window handle
//               MPARAM   not used by this function
//               MPARAM   not used by this function
//
// RETURNS:      MRESULT  reserved
//
// *******************************************************************************
MRESULT wmSize( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   RECTL       rectl;

// --------------------------------------------------------------------------
// Only process if we currently have focus
// --------------------------------------------------------------------------
   if (WinQueryFocus( HWND_DESKTOP ) == hwnd) {

// --------------------------------------------------------------------------
// Get size of the window and shrink to area inside border
// --------------------------------------------------------------------------
      WinQueryWindowRect( hwnd, &rectl );
      WinInflateRect( WinQueryAnchorBlock(hwnd), &rectl, -5, -5 );

// --------------------------------------------------------------------------
// Destroy the existing cursor and recreate
// --------------------------------------------------------------------------
      WinDestroyCursor( hwnd );
      WinCreateCursor( hwnd, rectl.xLeft, rectl.yBottom,
                       rectl.xRight - rectl.xLeft,
                       rectl.yTop - rectl.yBottom,
                       CURSOR_FRAME, NULL );
      WinShowCursor( hwnd, TRUE );
   } /* endif */

// --------------------------------------------------------------------------
// return the reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wmTimer
//
// FUNCTION USE: Handles the WM_TIMER message.
//
// DESCRIPTION:  Sends a TN_CLICKED notification to the owner window when
//               the automation timer expires.
//
// PARAMETERS:   HWND     control window handle
//               MPARAM   timer id in the lower order 16 bits
//               MPARAM   not used by this function
//
// RETURNS:      MRESULT  BOOLEAN indicating that the message was processed.
//
// *******************************************************************************
MRESULT wmTimer( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND     hwndOwner = WinQueryWindow( hwnd, QW_OWNER );
   USHORT   usId;

// --------------------------------------------------------------------------
// Make sure this is our timer and that there is an owner window specified.
// --------------------------------------------------------------------------
   if (SHORT1FROMMP(mp1) == ID_TIMER && hwndOwner != NULLHANDLE ) {

// --------------------------------------------------------------------------
// get our window id and send the notification message, indicating a move
// forward.
// --------------------------------------------------------------------------
      usId = WinQueryWindowUShort( hwnd, QWS_ID );
      WinSendMsg( hwndOwner, WM_CONTROL,
                  MPFROM2SHORT( usId, TN_CLICKED ),
                  MPFROMLONG(TA_FORWARD));
   } /* endif */

// --------------------------------------------------------------------------
// forward messages for other timers to the default window procedure
// --------------------------------------------------------------------------
   if (SHORT1FROMMP(mp1) != ID_TIMER ) {
      WinDefWindowProc( hwnd, WM_TIMER, mp1, mp2 );
   } /* endif */

// --------------------------------------------------------------------------
// indicate that the message was processed.
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     DrawPanel
//
// FUNCTION USE: Draws the application supplied data into the window during
//               painting.
//
// DESCRIPTION:  Draws one or more lines of text into a given area of the window
//               automatically handling wrapping of words when the line extends
//               beyond the width of the window.
//
// PARAMETERS:   HPS      handle for presentation space in which to draw.
//               PVOID    data to draw.  Either a pointer to a text string or
//                        handle of a bitmap.
//               BOOL     if TRUE, PVOID is a bitmap handle.
//               PRECTL   pointer to rectangle specifying area to be drawn
//
// RETURNS:      void
//
// *******************************************************************************
static void DrawPanel( HPS hps, PVOID pData, BOOL bBitmap, PRECTL pRectl )
{
   PSZ      pszText;
   PSZ      pszText1;
   ULONG    cchText;
   ULONG    cchDrawn;
   FONTMETRICS fm;
   RECTL    rectl;

// --------------------------------------------------------------------------
// make a local copy of the drawing area
// --------------------------------------------------------------------------
   rectl = *pRectl;

   if( !bBitmap) {
// --------------------------------------------------------------------------
// drawing text, be sure we have a non-NULL pointer
// --------------------------------------------------------------------------
      pszText = (PSZ)pData;
      if( pszText != NULL ) {

// --------------------------------------------------------------------------
// Get the data for the current font - we use the height
// --------------------------------------------------------------------------
         GpiQueryFontMetrics( hps, sizeof(FONTMETRICS), &fm );
         pszText1 = pszText;

// --------------------------------------------------------------------------
// Loop while there is text or drawing area
// --------------------------------------------------------------------------
         while (*pszText1 && rectl.yTop > 0 ) {
            cchText = cchDrawn = 0;

// --------------------------------------------------------------------------
// find the end of a line of text
// --------------------------------------------------------------------------
            while (*pszText1 && *pszText1++ != '\n' ) cchText++;

// --------------------------------------------------------------------------
// draw the text - breaking into multiple lines as necessary
// --------------------------------------------------------------------------
            while( cchText && rectl.yTop > rectl.yBottom) {
               pszText = &pszText[cchDrawn];
               cchDrawn = WinDrawText( hps, cchText, pszText, &rectl, CLR_BLACK,
                                       CLR_WHITE, DT_TEXTATTRS | DT_WORDBREAK );
               cchText -= cchDrawn;
               if (cchText) {
                  rectl.yTop -= fm.lMaxBaselineExt;
               } /* endif */
            } /* endwhile */

// --------------------------------------------------------------------------
// adjust drawing position
// --------------------------------------------------------------------------
            rectl.yTop -= fm.lMaxBaselineExt;
            pszText = pszText1;
         } /* endwhile */
      } /* endif */
   } else {

// --------------------------------------------------------------------------
// draw bitmap stretched to the size of the drawing area
// --------------------------------------------------------------------------
      WinDrawBitmap( hps, (HBITMAP)pData, NULL, (PPOINTL)&rectl, 0, 0, DBM_STRETCH );
   } /* endif */
}
