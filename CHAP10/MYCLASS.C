// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      MYCLASS.C                                        mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Subclass procedures for SUBCLASS/Chapter 10 Sample Program    บ
// บ                                                                               บ
// บ  NOTES:         Routines to subclass the entry field and static text field    บ
// บ                 to produce an edittable static text field.                    บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
#define  INCL_PM
#include <os2.h>
#include "myclass.h"

// --------------------------------------------------------------------------
// Define window instance data constants
// --------------------------------------------------------------------------
#define  WD_HWNDSTATIC     (ciEntry.cbWindowData)
#define  WD_HWNDPARENT     (ciEntry.cbWindowData + 4)
#define  SIZE_WINDOW_DATA  8

// --------------------------------------------------------------------------
// Define global variables
// --------------------------------------------------------------------------
static   BOOL        false = FALSE;
static   CLASSINFO   ciStatic;
static   CLASSINFO   ciEntry;

// --------------------------------------------------------------------------
// Prototype window subclass procedures
// --------------------------------------------------------------------------
MRESULT APIENTRY MyClassWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT APIENTRY MyClassStaticWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

// *******************************************************************************
// FUNCTION:     RegisterMyClass
//
// FUNCTION USE: Public function to register the new control class
//
// DESCRIPTION:  Obtains the information necessary to subclass the entry field
//               and static text controls, then registers new classes which
//               utilizes these two to assist in performing it's functions.
//
// PARAMETERS:   HAB          application anchor block handle
//
// RETURNS:      void
//
// *******************************************************************************
void RegisterMyClass( HAB hab ) {

   do {
// --------------------------------------------------------------------------
// Get the static text control class information
// --------------------------------------------------------------------------
      WinQueryClassInfo( hab, WC_STATIC, &ciStatic );

// --------------------------------------------------------------------------
// Get the entry field control class information
// --------------------------------------------------------------------------
      WinQueryClassInfo( hab, WC_ENTRYFIELD, &ciEntry );

// --------------------------------------------------------------------------
// Register the class for the new application control, which is a subclassed
// version of the entry field.
// --------------------------------------------------------------------------
      WinRegisterClass( hab, "MYCLASS",
               MyClassWndProc, (ULONG)(ciEntry.flClassStyle) & ~CS_PUBLIC,
               ciEntry.cbWindowData + SIZE_WINDOW_DATA);

// --------------------------------------------------------------------------
// Register the class for the new internal control, which is a subclassed
// version of the static text control
// --------------------------------------------------------------------------
      WinRegisterClass( hab, "MYCLASSSTATIC",
               MyClassStaticWndProc, (ULONG)(ciStatic.flClassStyle) & ~CS_PUBLIC,
               ciStatic.cbWindowData );
   } while ( false ); /* enddo */
}

// *******************************************************************************
// FUNCTION:     MyClassWndProc
//
// FUNCTION USE: The window procedure for the subclassed entry field
//
// DESCRIPTION:  Processes messages sent to the control,  messages not handled
//               by this function are forwarded to the standard entry
//               field control.
//
// PARAMETERS:   HWND     client window handle
//               ULONG    window message
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  Entry field window proc for all unprocessed messages;
//                        otherwise, message dependent
//
// *******************************************************************************
MRESULT APIENTRY MyClassWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   PCREATESTRUCT     pCS;
   HWND              hwndStatic;
   ULONG             flEntryStyle = 0;
   ULONG             flStaticStyle = SS_TEXT | DT_TOP;
   CHAR  szText[ 256 ];

   switch (msg) {


// --------------------------------------------------------------------------
// MC_ALLOWEDIT: if MP1 is TRUE, allow editing in the field; otherwise
//               disable editting.
// --------------------------------------------------------------------------
   case MC_ALLOWEDIT:
// --------------------------------------------------------------------------
// Get the handle of the subclassed static text window
// --------------------------------------------------------------------------
      hwndStatic  = WinQueryWindowULong( hwnd, WD_HWNDSTATIC );
      if (hwndStatic != NULLHANDLE) {

// --------------------------------------------------------------------------
// Edit is being allowed.
// --------------------------------------------------------------------------
         if ((BOOL)mp1) {
// --------------------------------------------------------------------------
// Hide static text
// --------------------------------------------------------------------------
            WinShowWindow( hwndStatic, FALSE );

// --------------------------------------------------------------------------
// Show entry field
// --------------------------------------------------------------------------
            WinEnableWindow( hwnd, TRUE );
            WinShowWindow( hwnd, TRUE );

// --------------------------------------------------------------------------
// Set focus to the entry field
// --------------------------------------------------------------------------
            WinSetFocus( HWND_DESKTOP, hwnd );

         } else {
// --------------------------------------------------------------------------
// Stop editing - Get text from the entry field and assign to static text
// --------------------------------------------------------------------------
            WinQueryWindowText( hwnd, 256, szText );
            WinSetWindowText( hwndStatic, szText );

// --------------------------------------------------------------------------
// Hide entry field
// --------------------------------------------------------------------------
            WinShowWindow( hwnd, FALSE );
            WinEnableWindow( hwnd, FALSE );

// --------------------------------------------------------------------------
// Show static text
// --------------------------------------------------------------------------
            WinShowWindow( hwndStatic, TRUE );
         } /* endif */
      } /* endif */
      return (MRESULT)TRUE;

// --------------------------------------------------------------------------
// WM_CREATE: setup the entry field and create the static text control
// --------------------------------------------------------------------------
   case WM_CREATE:
// --------------------------------------------------------------------------
// Get the creation data
// --------------------------------------------------------------------------
      pCS = (PCREATESTRUCT)PVOIDFROMMP(mp2);

// --------------------------------------------------------------------------
// Set the entry field control style
// --------------------------------------------------------------------------
      if (!(pCS->flStyle & (MC_CENTER | MC_RIGHT))) {
         flStaticStyle |= DT_LEFT;
      } /* endif */
      if (pCS->flStyle & MC_CENTER) {
         flStaticStyle |= DT_CENTER;
      } /* endif */
      if (pCS->flStyle & MC_RIGHT) {
         flStaticStyle |= DT_RIGHT;
      } /* endif */
      if (pCS->flStyle & MC_AUTOSIZE) {
         flStaticStyle |= SS_AUTOSIZE;
      } /* endif */

// --------------------------------------------------------------------------
// Set style to outline the field and make sure readonly is off
// --------------------------------------------------------------------------
      flEntryStyle = WinQueryWindowULong( hwnd, QWL_STYLE );
      flEntryStyle |= ES_MARGIN;
      flEntryStyle &= ~ES_READONLY;
// --------------------------------------------------------------------------
// Save the new style
// --------------------------------------------------------------------------
      WinSetWindowULong( hwnd, QWL_STYLE, flEntryStyle );

// --------------------------------------------------------------------------
// Create the static text control window
// --------------------------------------------------------------------------
      hwndStatic = WinCreateWindow( pCS->hwndParent,
                                    "MYCLASSSTATIC",
                                    pCS->pszText,
                                    flStaticStyle,
                                    pCS->x,
                                    pCS->y + 2,
                                    pCS->cx,
                                    pCS->cy,
                                    hwnd,
                                    pCS->hwndInsertBehind,
                                    pCS->id | 0x8000L,
                                    NULL,
                                    pCS->pPresParams );

// --------------------------------------------------------------------------
// Save static text and parent window handles
// --------------------------------------------------------------------------
      WinSetWindowULong( hwnd, WD_HWNDSTATIC, hwndStatic );
      WinSetWindowULong( hwnd, WD_HWNDPARENT, pCS->hwndParent );

// --------------------------------------------------------------------------
// Post message to initially disable editting
// --------------------------------------------------------------------------
      WinPostMsg( hwnd, MC_ALLOWEDIT, MPFROMLONG(FALSE), 0);

// --------------------------------------------------------------------------
// Let the regular entry field procedure continue the process
// --------------------------------------------------------------------------
      return (*(ciEntry.pfnWindowProc))(hwnd, msg, mp1, mp2 );

// --------------------------------------------------------------------------
// WM_SETFOCUS: return to non-editing state when focus lost
// --------------------------------------------------------------------------
   case WM_SETFOCUS:
      if (!(BOOL)mp2) {
// --------------------------------------------------------------------------
// Post message to disallow editing
// --------------------------------------------------------------------------
         WinSendMsg( hwnd, MC_ALLOWEDIT, (MPARAM)FALSE, 0 );
      } /* endif */

// --------------------------------------------------------------------------
// Let the normal entry field processing do it's work
// --------------------------------------------------------------------------
      return( *(ciEntry.pfnWindowProc))(hwnd, msg, mp1, mp2 );

// --------------------------------------------------------------------------
// WM_CHAR: Process the ESC character to halt editing.
// --------------------------------------------------------------------------
   case WM_CHAR:
      if ((SHORT1FROMMP(mp1) & KC_VIRTUALKEY) &&
          !(SHORT1FROMMP(mp1) & KC_KEYUP)) {
         if (CHAR3FROMMP(mp2) == VK_ESC ) {
            HWND hwndParent = WinQueryWindow( hwnd, QW_PARENT );
            HWND hwndNext;

// --------------------------------------------------------------------------
// Get the parent window
// --------------------------------------------------------------------------
            hwndParent = WinQueryWindowULong( hwnd, WD_HWNDPARENT );
//            hwndParent = WinQueryWindow( hwndParent, QW_PARENT );

// --------------------------------------------------------------------------
// Try to get the normal window that would receive focus
// --------------------------------------------------------------------------
            hwndNext = (HWND)WinSendMsg( hwndParent, WM_QUERYFOCUSCHAIN,
                                (MPARAM)QFC_NEXTINCHAIN, (MPARAM)hwndParent );

// --------------------------------------------------------------------------
// No next found - just set focus back to the parent
// --------------------------------------------------------------------------
            if (hwndNext == (HWND)NULLHANDLE) {
               hwndNext = hwndParent;
            } /* endif */

// --------------------------------------------------------------------------
// Give the focus away
// --------------------------------------------------------------------------
            WinSetFocus( HWND_DESKTOP, hwndNext );
            return (MRESULT)TRUE;
         } /* endif */
      } /* endif */

// --------------------------------------------------------------------------
// let the normal edit control procedure handle all other characters
// --------------------------------------------------------------------------
      return (*(ciEntry.pfnWindowProc))(hwnd, msg, mp1, mp2 );

   default:
// --------------------------------------------------------------------------
// Let the normal edit control procedure handle all other messages
// --------------------------------------------------------------------------
      return (*(ciEntry.pfnWindowProc))(hwnd, msg, mp1, mp2 );
   } /* endswitch */
}

// *******************************************************************************
// FUNCTION:     MyClassStatocWndProc
//
// FUNCTION USE: The window procedure for the subclassed static text field
//
// DESCRIPTION:  Processes a WM_BUTTON2DOWN over the static text window to
//               allow editing.  All other messages are passed to the
//               normal procedure
//
// PARAMETERS:   HWND     client window handle
//               ULONG    window message
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  Entry field window proc for all unprocessed messages;
//                        otherwise, message dependent
//
// *******************************************************************************
MRESULT APIENTRY MyClassStaticWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   HWND        hwndEntry = WinQueryWindow( hwnd, QW_OWNER );

   switch (msg) {
   case WM_BUTTON2DOWN:
// --------------------------------------------------------------------------
// Send message to the edit control to enable editing.
// --------------------------------------------------------------------------
      WinSendMsg( hwndEntry, MC_ALLOWEDIT, (MPARAM)TRUE, 0l );
      return (MRESULT)TRUE;

   default:
// --------------------------------------------------------------------------
// Let the normal static text control procedure handle all other messages
// --------------------------------------------------------------------------
      return (*(ciStatic.pfnWindowProc))(hwnd, msg, mp1, mp2 );
   } /* endswitch */
}
