// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      SETFORM.C                                        mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Source for the Page Margins Dialog Processing for             บ
// บ                 PRINTIT, the Chapter 16 Sample program                        บ
// บ                                                                               บ
// บ  NOTES:         This program demonstrates printing and font management.       บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#define  INCL_PM
#include "printit.h"
#include <stdlib.h>
#include <string.h>
#include "shcommon.h"

// --------------------------------------------------------------------------
// Define meaningful constants for the value set items
// --------------------------------------------------------------------------
#define  VSI_MM      (MAKEULONG(1,1))
#define  VSI_INCHES  (MAKEULONG(2,1))
#define  VSI_POINTS  (MAKEULONG(3,1))

// --------------------------------------------------------------------------
// Define conversions between millimeters and LO_METRIC units
// --------------------------------------------------------------------------
#define MMTOLM(x)    (x*10L)
#define LMTOMM(x)    ((x+5L)/10L)

// --------------------------------------------------------------------------
// Define conversions between .01 inch and LO_METRIC units
// 2.54 LM units per .01 inch - conversion multiplies by
// by 100 to preserve accuracy
// --------------------------------------------------------------------------
#define HINTOLM(x)   (((x*254L)+50L)/100L)
#define LMTOHIN(x)   (((x*100L)+127L)/254L)

// --------------------------------------------------------------------------
// Conversion between points and LO_METRIC units
// 3.5 LM units per point - conversion multiples by
// by 10 to preserve accuracy
// --------------------------------------------------------------------------
#define PTSTOLM(x)   (((x*35L)+5L)/10L)
#define LMTOPTS(x)   (((x*10L)+17L)/35L)

// --------------------------------------------------------------------------
// Define the structure used for internal tracking during dialog processing
// --------------------------------------------------------------------------
typedef struct _SETFORMDLGDATA_ {
   USHORT   cb;
   ULONG    ulUnits;
   RECTL    rectlMargins;
} SETFORM, *PSETFORM;

// --------------------------------------------------------------------------
// Define variable to keep track of the user's last unit of measurement.
// --------------------------------------------------------------------------
static ULONG   ulUnits = VSI_MM;

// --------------------------------------------------------------------------
// Prototype the modules functions
// --------------------------------------------------------------------------
MRESULT EXPENTRY SetFormDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
static MRESULT wm_initdlg( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wm_command( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static void SetEntryField( HWND hwnd, ULONG cid, ULONG ulValue, ULONG ulUnits );
static void GetEntryField( HWND hwnd, ULONG cid, PLONG plValue, ULONG ulUnits );

// *******************************************************************************
// FUNCTION:     SetPageFormat
//
// FUNCTION USE: Application interface to the set page format dialog
//
// DESCRIPTION:  Copies the application's margin rectangle to a local area
//               and then raises the dialog.  If the user successfully
//               completes the dialog, the new data is copied back to
//               the application.
//
// PARAMETERS:   HWND     client window handle
//               PRECTL   pointer to app's margin rectangle
//
// RETURNS:      BOOL     TRUE if data changed
//
// *******************************************************************************
BOOL SetPageFormat( HWND hwnd, PRECTL prectlMargins )
{
   SETFORM  setform;
   BOOL     bRetVal;

// --------------------------------------------------------------------------
// fill in initial data
// --------------------------------------------------------------------------
   setform.cb = sizeof( SETFORM );
   setform.ulUnits = ulUnits;
   setform.rectlMargins = *prectlMargins;

// --------------------------------------------------------------------------
// Raise the dialog and get the user input */
// --------------------------------------------------------------------------
   bRetVal = WinDlgBox( HWND_DESKTOP, hwnd, SetFormDlgProc,
                        NULLHANDLE, SELFORM_ID, (PVOID)&setform );

// --------------------------------------------------------------------------
// if the user pressed OK, then modify the application's data
// --------------------------------------------------------------------------
   if ( bRetVal ) {
      *prectlMargins = setform.rectlMargins;
      ulUnits = setform.ulUnits;
   } /* endif */

// --------------------------------------------------------------------------
// return success of operation
// --------------------------------------------------------------------------
   return bRetVal;
}

// *******************************************************************************
// FUNCTION:     SetFormDlgProc
//
// FUNCTION USE: The window procedure for the dialog
//
// DESCRIPTION:  Calls the worker routine for each message handled by the
//               the application or the default procedure for messages which
//               the application does not handle.
//
// PARAMETERS:   HWND     dialog window handle
//               ULONG    window message
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  WinDefDlgProc for all unprocessed messages;
//                        otherwise, message dependent
//
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT EXPENTRY SetFormDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
 switch (msg) 
  {
   case WM_COMMAND:           
        return wm_command(hwnd, mp1, mp2); 

   case WM_INITDLG:           
        return wm_initdlg(hwnd, mp1, mp2); 

   default:                  
        return WinDefDlgProc(hwnd, msg, mp1, mp2); 
  } /* endswitch */
}

// *******************************************************************************
// FUNCTION:     wm_command
//
// FUNCTION USE: Process WM_COMMAND messages for the dialog window
//
// DESCRIPTION:  If the OK button is clicked, retrieve information from the
//               dialog to set the new margin values.
//
// PARAMETERS:   HWND     dialog window handle
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  Reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wm_command( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PSETFORM    pSetForm;

   switch (SHORT1FROMMP(mp1)) {
   case DID_OK:
// --------------------------------------------------------------------------
// User pressed the OK button - get the data pointer from instance data
// --------------------------------------------------------------------------
      pSetForm = (PSETFORM)WinQueryWindowULong( hwnd, QWL_USER );

// --------------------------------------------------------------------------
// get the selected units
// --------------------------------------------------------------------------
      pSetForm->ulUnits = (ULONG)LONGFROMMR(WinSendDlgItemMsg( hwnd,
                           CID_MEASURE, VM_QUERYSELECTEDITEM, 0L, 0L ) );

// --------------------------------------------------------------------------
// Get new margin values from the entry fields
// --------------------------------------------------------------------------
      GetEntryField( hwnd, CID_MAR_LEFT,
                     &pSetForm->rectlMargins.xLeft, pSetForm->ulUnits );
      GetEntryField( hwnd, CID_MAR_TOP,
                     &pSetForm->rectlMargins.yTop, pSetForm->ulUnits );
      GetEntryField( hwnd, CID_MAR_RIGHT,
                     &pSetForm->rectlMargins.xRight, pSetForm->ulUnits );
      GetEntryField( hwnd, CID_MAR_BOTTOM,
                     &pSetForm->rectlMargins.yBottom, pSetForm->ulUnits );

// --------------------------------------------------------------------------
// convert rectangle from current unit of measure to LO_METRIC */
// --------------------------------------------------------------------------
      if (pSetForm->ulUnits == (ULONG)VSI_INCHES) {
         pSetForm->rectlMargins.xLeft =
                                   HINTOLM( pSetForm->rectlMargins.xLeft );
         pSetForm->rectlMargins.xRight =
                                   HINTOLM( pSetForm->rectlMargins.xRight );
         pSetForm->rectlMargins.yTop =
                                   HINTOLM( pSetForm->rectlMargins.yTop );
         pSetForm->rectlMargins.yBottom =
                                   HINTOLM( pSetForm->rectlMargins.yBottom );
      } else if (pSetForm->ulUnits == (ULONG)VSI_POINTS ) {
         pSetForm->rectlMargins.xLeft =
                                   PTSTOLM( pSetForm->rectlMargins.xLeft );
         pSetForm->rectlMargins.xRight =
                                   PTSTOLM( pSetForm->rectlMargins.xRight );
         pSetForm->rectlMargins.yTop =
                                   PTSTOLM( pSetForm->rectlMargins.yTop );
         pSetForm->rectlMargins.yBottom =
                                   PTSTOLM( pSetForm->rectlMargins.yBottom );
      } else {
         pSetForm->rectlMargins.xLeft =
                                   MMTOLM( pSetForm->rectlMargins.xLeft );
         pSetForm->rectlMargins.xRight =
                                   MMTOLM( pSetForm->rectlMargins.xRight );
         pSetForm->rectlMargins.yTop =
                                   MMTOLM( pSetForm->rectlMargins.yTop );
         pSetForm->rectlMargins.yBottom =
                                   MMTOLM( pSetForm->rectlMargins.yBottom );
      } /* endif */

// --------------------------------------------------------------------------
// Indicate successful completion
// --------------------------------------------------------------------------
      WinDismissDlg( hwnd, TRUE );

// --------------------------------------------------------------------------
// return appropriate value
// --------------------------------------------------------------------------
      return (MRESULT)0L;

   case DID_CANCEL:
// --------------------------------------------------------------------------
// CANCEL pressed - indicate unsuccessful completion.
// --------------------------------------------------------------------------
      WinDismissDlg( hwnd, FALSE );

// --------------------------------------------------------------------------
// return appropriate value
// --------------------------------------------------------------------------
      return (MRESULT)0L;

   default:
      return WinDefDlgProc( hwnd, WM_COMMAND, mp1, mp2 );
   } /* endswitch */
}

// *******************************************************************************
// FUNCTION:     wm_initdlg
//
// FUNCTION USE: Process WM_INITDLG messages for the dialog window
//
// DESCRIPTION:  Initialize the dialog controls with current data
//
// PARAMETERS:   HWND     dialog window handle
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  Reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wm_initdlg( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PSETFORM    pSetForm = (PSETFORM)mp2;

// --------------------------------------------------------------------------      
// Remove the system menu items that are not needed by this dialog                 
// --------------------------------------------------------------------------      
   SetTheSysMenu(hwnd, NULL);

// --------------------------------------------------------------------------
// Establish unit of measure labels for the value set items
// --------------------------------------------------------------------------
   WinSendDlgItemMsg( hwnd, CID_MEASURE, VM_SETITEM,
                            MPFROMLONG(VSI_MM), "mm" );
   WinSendDlgItemMsg( hwnd, CID_MEASURE, VM_SETITEM,
                            MPFROMLONG(VSI_INCHES), "inches" );
   WinSendDlgItemMsg( hwnd, CID_MEASURE, VM_SETITEM,
                            MPFROMLONG(VSI_POINTS), "points" );

// --------------------------------------------------------------------------
// Select the current value set item
// --------------------------------------------------------------------------
   WinSendDlgItemMsg( hwnd, CID_MEASURE, VM_SELECTITEM,
                      MPFROMLONG( pSetForm->ulUnits ), 0L );

// --------------------------------------------------------------------------
// convert rectangle from LOMETRIC to current unit of measure
// --------------------------------------------------------------------------
   if (pSetForm->ulUnits == (ULONG)VSI_INCHES) {
      pSetForm->rectlMargins.xLeft = LMTOHIN( pSetForm->rectlMargins.xLeft );
      pSetForm->rectlMargins.xRight = LMTOHIN( pSetForm->rectlMargins.xRight );
      pSetForm->rectlMargins.yTop = LMTOHIN( pSetForm->rectlMargins.yTop );
      pSetForm->rectlMargins.yBottom =
                                     LMTOHIN( pSetForm->rectlMargins.yBottom );
   } else if (pSetForm->ulUnits == (ULONG)VSI_POINTS ) {
      pSetForm->rectlMargins.xLeft = LMTOPTS( pSetForm->rectlMargins.xLeft );
      pSetForm->rectlMargins.xRight = LMTOPTS( pSetForm->rectlMargins.xRight );
      pSetForm->rectlMargins.yTop = LMTOPTS( pSetForm->rectlMargins.yTop );
      pSetForm->rectlMargins.yBottom =
                                     LMTOPTS( pSetForm->rectlMargins.yBottom );
   } else {
      pSetForm->rectlMargins.xLeft = LMTOMM( pSetForm->rectlMargins.xLeft );
      pSetForm->rectlMargins.xRight = LMTOMM( pSetForm->rectlMargins.xRight );
      pSetForm->rectlMargins.yTop = LMTOMM( pSetForm->rectlMargins.yTop );
      pSetForm->rectlMargins.yBottom =
                                     LMTOMM( pSetForm->rectlMargins.yBottom );
   } /* endif */


// --------------------------------------------------------------------------
// set the text for the entry fields
// --------------------------------------------------------------------------
   SetEntryField( hwnd, CID_MAR_LEFT,
                       (USHORT)pSetForm->rectlMargins.xLeft, ulUnits );
   SetEntryField( hwnd, CID_MAR_TOP,
                       (USHORT)pSetForm->rectlMargins.yTop, ulUnits );
   SetEntryField( hwnd, CID_MAR_RIGHT,
                       (USHORT)pSetForm->rectlMargins.xRight, ulUnits );
   SetEntryField( hwnd, CID_MAR_BOTTOM,
                       (USHORT)pSetForm->rectlMargins.yBottom, ulUnits );

// --------------------------------------------------------------------------
// save off the internal structure pointer in the dialog instance data
// --------------------------------------------------------------------------
   WinSetWindowULong( hwnd, QWL_USER, (ULONG)pSetForm );

   return MRFROMLONG(FALSE);
}

// *******************************************************************************
// FUNCTION:     SetEntryField
//
// FUNCTION USE: Set entry field text to field value.
//
// DESCRIPTION:  Convert value to a string, moving decimal point for
//               .01 inch to inch conversion, and set the text of the
//               entry field
//
// PARAMETERS:   HWND     dialog window handle
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  Reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void SetEntryField( HWND hwnd, ULONG cid, ULONG ulValue, ULONG ulUnits )
{
   char     szText[ 34 ];
   char     *pszTemp;

// --------------------------------------------------------------------------
// Convert the numeric value to a string
// --------------------------------------------------------------------------
   _ltoa( ulValue, szText, 10 );

   if (ulUnits == (ULONG)VSI_INCHES) {

// --------------------------------------------------------------------------
// inches units are actually hundredths - so put in the decimal point
// --------------------------------------------------------------------------
      pszTemp = &szText[ strlen(szText) + 1];
      *pszTemp = *(pszTemp-1);
      pszTemp--;
      *pszTemp = *(pszTemp-1);
      pszTemp--;
      *pszTemp = *(pszTemp-1);
      pszTemp--;
      *pszTemp = '.';
   } /* endif */

// --------------------------------------------------------------------------
// Set the entry field text
// --------------------------------------------------------------------------
   WinSetDlgItemText( hwnd, cid, szText );
}

// *******************************************************************************
// FUNCTION:     GetEntryField
//
// FUNCTION USE: Get entry field text and convert to value
//
// DESCRIPTION:  Get the entry field text.  If units are in inches,
//               move the decimal place right to points to convert
//               to hundredths of an inch.  Convert the text to
//               a number.
//
// PARAMETERS:   HWND     dialog window handle
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  Reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void GetEntryField( HWND hwnd, ULONG cid, PLONG plValue, ULONG ulUnits )
{
   char     szText[ 34 ];
   char     *pszTemp;

// --------------------------------------------------------------------------
// Get the text from the entry field
// --------------------------------------------------------------------------
   WinQueryDlgItemText( hwnd, cid, 34, szText );

// --------------------------------------------------------------------------
// If units in inches - move the decimal point.
// --------------------------------------------------------------------------
   if (ulUnits == (ULONG)VSI_INCHES) {
      pszTemp = strchr(szText, '.' );
      if (pszTemp == NULL) {
         pszTemp = &szText[strlen(szText)];
      } /* endif */

// --------------------------------------------------------------------------
// If there is a decimal pt and something after it - move up the first
// two decimal places (and terminating NULL or other character).
// --------------------------------------------------------------------------
      if (*pszTemp && *(pszTemp+1)) {
         *pszTemp = *(pszTemp+1);
         *(pszTemp+1) = *(pszTemp+2);
         *(pszTemp+2) = *(pszTemp+3);
      } else {

// --------------------------------------------------------------------------
// otherwise just fill in a couple of zeros
// --------------------------------------------------------------------------
         *pszTemp = '0';
         *(pszTemp+1) = '0';
         *(pszTemp+2) = '\0';
      } /* endif */

// --------------------------------------------------------------------------
// If there's only one character after the decimal - fill a zero
// --------------------------------------------------------------------------
      pszTemp++;
      if (!*pszTemp) {
         *pszTemp = '0';
         *(pszTemp+1) = '\0';
      } /* endif */

// --------------------------------------------------------------------------
// make sure the string terminates after two digits
// --------------------------------------------------------------------------
      pszTemp++;
      if (*pszTemp) {
         *pszTemp = '\0';
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// convert to a value
// --------------------------------------------------------------------------
   *plValue = atol( szText );
}
