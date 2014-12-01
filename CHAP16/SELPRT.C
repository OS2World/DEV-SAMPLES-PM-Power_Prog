// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      SELPRT.C                                         mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Source for the Print Queue Selection Dialog for               บ
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
#define  INCL_SPL
#define  INCL_SPLDOSPRINT
#include "printit.h"
#include <stdlib.h>
#include <string.h>
#include "shcommon.h"

// --------------------------------------------------------------------------
// declare flag for job properties changed indication
// --------------------------------------------------------------------------
static BOOL    gbJobChanged;

// --------------------------------------------------------------------------
// Prototype the dialog procedure
// --------------------------------------------------------------------------
MRESULT EXPENTRY SelPrtDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------
// Prototype the dialog message processing procedures
// --------------------------------------------------------------------------
static MRESULT wmInitDlg( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmCommand( HWND hwnd, MPARAM mp1, MPARAM mp2 );

// *******************************************************************************
// FUNCTION:     SelectPrinter
//
// FUNCTION USE: Application interface to the print queue selection dialog
//
// DESCRIPTION:  Raises the dialog and returns dialog status to the app.
//
// PARAMETERS:   HWND     client window handle
//
// RETURNS:      BOOL     TRUE if data changed
//
// *******************************************************************************
BOOL  SelectPrinter( HWND hwnd )
{
   BOOL  bRetVal;

   bRetVal = WinDlgBox( HWND_DESKTOP, hwnd, SelPrtDlgProc,
                        NULLHANDLE, SELPRT_ID, NULL );
   return bRetVal;
}

// *******************************************************************************
// FUNCTION:     SelPrtDlgProc
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
MRESULT EXPENTRY SelPrtDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   switch (msg) {
   case WM_COMMAND:                    return wmCommand( hwnd, mp1, mp2 );
   case WM_INITDLG:                    return wmInitDlg( hwnd, mp1, mp2 );
   default:                            return WinDefDlgProc( hwnd, msg, mp1, mp2 );
   } /* endswitch */
}

// *******************************************************************************
// FUNCTION:     wm_command
//
// FUNCTION USE: Process WM_COMMAND messages for the dialog window
//
// DESCRIPTION:  If the job properties button is pressed, raises the
//               print queues job properties dialog to update the
//               application global data. If the OK, button is pressed
//               modifies the global queue selection index.
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
static MRESULT wmCommand( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PSZ         pszDeviceName;
   PSZ         pszDriverName;
   SHORT       sItem;

   switch (SHORT1FROMMP(mp1)) {
   case CID_SETUP:
// --------------------------------------------------------------------------
// get the currently selected print queue from the list box
// --------------------------------------------------------------------------
      sItem = SHORT1FROMMR( WinSendDlgItemMsg( hwnd, CID_PRINTERLB,
                            LM_QUERYSELECTION, 0l, 0l ));
      if (sItem == LIT_NONE) {
         return 0l;
      }

// --------------------------------------------------------------------------
// parse the driver and device names from the global data for the queue
// --------------------------------------------------------------------------
      pszDriverName = malloc( strlen(gpQueueBuf[sItem].pszDriverName));
      if (pszDriverName != NULL ) {
         strcpy( pszDriverName, gpQueueBuf[sItem].pszDriverName );
         pszDeviceName = (PSZ)strchr( pszDriverName, '.' );
         if (pszDeviceName != NULL) {
            *(pszDeviceName++) = '\0';
         } /* endif */

// --------------------------------------------------------------------------
// raise the job properties dialog for the queue
// --------------------------------------------------------------------------
         DevPostDeviceModes( WinQueryAnchorBlock( hwnd ),
                             gpQueueBuf[sItem].pDriverData,
                             pszDriverName,
                             pszDeviceName,
                             gpQueueBuf[sItem].pszPrinters,
                             DPDM_POSTJOBPROP );

// --------------------------------------------------------------------------
// No way of knowing if changes occurred - just assume they did
// --------------------------------------------------------------------------
         gbJobChanged = TRUE;

// --------------------------------------------------------------------------
// free the allocated memory
// --------------------------------------------------------------------------
         free( pszDriverName );
      } /* endif */
      return 0l;

   case DID_OK:
// --------------------------------------------------------------------------
// get the currently selected queue from the list box
// --------------------------------------------------------------------------
      sItem = SHORT1FROMMR( WinSendDlgItemMsg( hwnd, CID_PRINTERLB,
                            LM_QUERYSELECTION, 0l, 0l ));
      if (sItem != LIT_NONE) {
// --------------------------------------------------------------------------
// if either the queue or job properties changed, reassign the global queue
// index selection and indicate that a change occurred.
// --------------------------------------------------------------------------
         if ( sItem != gnQueue || gbJobChanged) {
            gnQueue = sItem;
            WinDismissDlg( hwnd, TRUE );
         } else {
            WinDismissDlg( hwnd, FALSE );
         } /* endif */
      } else {
         WinDismissDlg( hwnd, FALSE );
      } /* endif */
      return MRFROMLONG(0L);

   case DID_CANCEL:
// --------------------------------------------------------------------------
// normally this would return FALSE, but we don't change the properties
// so indicate that a change occurred if the job properties dialog was
// raised.
// --------------------------------------------------------------------------
      WinDismissDlg( hwnd, gbJobChanged );
      return MRFROMLONG(0L);
   } /* endswitch */

   return WinDefDlgProc( hwnd, WM_COMMAND, mp1, mp2 );
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
static MRESULT wmInitDlg( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   ULONG    nQueue;

// --------------------------------------------------------------------------               
// Remove the system menu items that are not needed by this dialog                          
// --------------------------------------------------------------------------               
   SetTheSysMenu(hwnd, NULL);                                                               

// --------------------------------------------------------------------------
// indicate that no job properties changes have yet occured.
// --------------------------------------------------------------------------
   gbJobChanged = FALSE;

// --------------------------------------------------------------------------
// add the printer queue names to the list box
// --------------------------------------------------------------------------
   for (nQueue = 0; nQueue < gcQueues; nQueue++) {
      WinSendDlgItemMsg( hwnd, CID_PRINTERLB, LM_INSERTITEM, (MPARAM)LIT_END,
                         MPFROMP(gpQueueBuf[nQueue].pszComment));

      if (nQueue == gnQueue ) {
// --------------------------------------------------------------------------
// If current printer queue is the one in use - select the list box item
// --------------------------------------------------------------------------
         WinSendDlgItemMsg( hwnd, CID_PRINTERLB, LM_SELECTITEM,
               MPFROMLONG(nQueue), MPFROMLONG(TRUE) );
      } /* endif */
   } /* endfor */

// --------------------------------------------------------------------------
// indicate no focus change occurred.
// --------------------------------------------------------------------------
   return MRFROMLONG(FALSE);
}
