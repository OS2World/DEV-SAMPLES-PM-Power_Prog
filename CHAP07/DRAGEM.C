// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      DRAGEM.C                                         mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Main Source File for DRAGEM/Chapter 7 Sample Program          บ
// บ                                                                               บ
// บ  NOTES:         This program is a simple file manager which demonstrates      บ
// บ                 direct manipulation coding in PM applications.                บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#define INCL_DOSFILEMGR
#define INCL_DOSMISC
#include "dragem.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "shcommon.h"

// --------------------------------------------------------------------------
// Miscellaneous constant definitions used only in this source module.
// --------------------------------------------------------------------------
#define  SUPPORTED_RMF  ( "<DRM_OS2FILE,DRF_UNKNOWN>,"\
                          "<DRM_PRINT,DRF_UNKNOWN>,"\
                          "<DRM_DISCARD,DRF_UNKNOWN>" )

// --------------------------------------------------------------------------
// Miscellaneous global variables used only in this source module.
// --------------------------------------------------------------------------
static      BOOL  false = FALSE;
static      BOOL  fEmphasized = FALSE;
static      PFNWP pLBDefProc;
static      HWND  ghwndObject = NULLHANDLE;
ULONG       gulDragCount;
PDRAGINFO   gpSrcDinfo;

// --------------------------------------------------------------------------
// Client and Dialog window procedures
// --------------------------------------------------------------------------
MRESULT APIENTRY LBSubclassProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------
// Message processing procedures
// --------------------------------------------------------------------------
MRESULT lazy_setobjecthwnd( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT dm_discardobject( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT dm_dragleave( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT dm_dragover( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT dm_dropnotify( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT dm_printobject( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT dm_drop( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT dm_renderprepare( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_begindrag( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_button1dblclk( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_char( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_close( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_command( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_pickup( HWND hwnd, MPARAM mp1, MPARAM mp2);

// --------------------------------------------------------------------------
// Miscellaneous procedures
// --------------------------------------------------------------------------
void FillNewDir( HWND hwnd );
void StartDragOp( HWND hwnd, BOOL fLazy );
ULONG QuerySelectionCount( HWND hwndList );
void SetDragItems( PDRAGINFO *ppDinfo, HWND hwnd, BOOL fLazy );
SHORT GetItemForFile( PDRAGINFO pDinfo, PSZ pszFileName );
PSZ QueryType( PSZ pszFileName );
PSZ QueryCurrentDirectory( void );
void DrawEmphasis(HWND hwnd);
PDRAGINFO ReallocDraginfo( PDRAGINFO pDinfo );
#define static
// *******************************************************************************
// FUNCTION:     main
//
// FUNCTION USE: main entry point
//
// DESCRIPTION:  This is the main function of the application.  This function
//               obtains an anchor block handle, creates the application message
//               queue and creates the frame and client windows.  This routine
//               will subsequently enter the typical message loop which polls
//               the message queue, getting and dispatching messages until
//               WinGetMsg returns FALSE indicating a WM_QUIT message was
//               received, thereby terminating the application.
//
// PARAMETERS:   Standard 'C' language main function parameters - unused.
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
main(int argc, char *argv[], char *envp[])
{
   HAB   hab;
   HMQ   hmq;
   QMSG  qmsg;
   ULONG flStyle = FCF_STANDARD & ~FCF_ACCELTABLE;
   HWND  hwndFrame;
   HWND  hwndClient;

// --------------------------------------------------------------------------
// Single iteration loop - makes error handling easier
// --------------------------------------------------------------------------
   do {

// --------------------------------------------------------------------------
// First things first - set up the PM environment for the thread
// --------------------------------------------------------------------------
      hab = WinInitialize( 0L );
      if (hab == (HAB)NULLHANDLE) {
         // Oops - couldn't do it - bail out!
         DosBeep( 60, 100 );
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Next thing we need is a message queue
// --------------------------------------------------------------------------
      hmq = WinCreateMsgQueue( hab, 0L );
      if (hmq == (HMQ)NULLHANDLE) {
         // No message queue equals no PM - quit now
         DosBeep( 60, 100 );
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Now create the application's main frame window.  Note: no application
// class has been registered since the client area of the window is a
// list box.
// --------------------------------------------------------------------------
      hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                      0L,
                                      &flStyle,
                                      WC_LISTBOX,
                                      "Direct Manipulation Sample",
                                      LS_MULTIPLESEL | LS_EXTENDEDSEL,
                                      NULLHANDLE,
                                      RID_LAZY,
                                      &hwndClient );
      if (hwndFrame == NULLHANDLE) {
         // no window - gotta leave.
         DisplayMessages(NULLHANDLE, "Error Creating Frame Window",
                         MSG_ERROR);
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Subclass the listbox control window in order to receive direct
// manipulation messages sent to the control.
// --------------------------------------------------------------------------
      pLBDefProc = WinSubclassWindow(hwndClient, LBSubclassProc);

// --------------------------------------------------------------------------
// Restore the size and position of the frame and the listbox from the
// USER initialization file.  If this is the first time running the program
// the default sizes will be retained.
// --------------------------------------------------------------------------
      WinRestoreWindowPos( INIAPPNAME, INIFRMPOSKEY, hwndFrame );
      WinRestoreWindowPos( INIAPPNAME, INILBXPOSKEY, hwndClient );

// --------------------------------------------------------------------------
// Now display the window and make it the active frame window.
// --------------------------------------------------------------------------
      WinShowWindow( hwndFrame, TRUE );
      WinSetActiveWindow( HWND_DESKTOP, hwndFrame );

// --------------------------------------------------------------------------
// Start up a thread to run the object window which handles rendering
// --------------------------------------------------------------------------
      _beginthread( ObjectThread, NULL, 32768, (PVOID)&hwndClient);

// --------------------------------------------------------------------------
// NOTE:  This is our standard PM message loop.  The purpose of this loop
//        is to get and dispatch messages from the application message queue
//        until WinGetMsg returns FALSE, indicating a WM_QUIT message.  In
//        which case we will pop out of the loop and destroy our frame window
//        along with the application message queue then we will terminate our
//        anchor block removing our ability to call the Presentation Manager.
// --------------------------------------------------------------------------
      while (WinGetMsg( hab, &qmsg, NULLHANDLE, 0UL, 0UL))
         WinDispatchMsg( hab, &qmsg );

   } while ( false ); /* enddo */

// --------------------------------------------------------------------------
// Kill the message queue if successfully created
// --------------------------------------------------------------------------
   if (hmq != NULLHANDLE) {
      WinDestroyMsgQueue( hmq );
   } /* endif */

// --------------------------------------------------------------------------
// Disconnect from PM if necessary
// --------------------------------------------------------------------------
   if (hab != NULLHANDLE) {
      WinTerminate( hab );
   } /* endif */

   return 0;
}


// *******************************************************************************
// FUNCTION:     LBSubclassProc
//
// FUNCTION USE: Serves as both the subclass procedure for handling direct
//               manipulation messages sent to the list box and as the client
//               window procedure for the application.
//
// DESCRIPTION:  Calls the worker routine for each message handled by the
//               the application or the default procedure for messages which
//               the application does not handle.
//
// PARAMETERS:   HWND     client window handle
//               ULONG    window message
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  WinDefWindowProc for all unprocessed messages;
//                        otherwise, message dependent
//
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT APIENTRY LBSubclassProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   switch (msg) {
   case DM_DISCARDOBJECT:        return dm_discardobject( hwnd, mp1, mp2 );
   case DM_DRAGLEAVE:            return dm_dragleave( hwnd, mp1, mp2 );
   case DM_DRAGOVER:             return dm_dragover( hwnd, mp1, mp2 );
   case DM_DROPNOTIFY:           return dm_dropnotify( hwnd, mp1, mp2 );
   case DM_DROP:                 return dm_drop( hwnd, mp1, mp2 );
   case DM_PRINTOBJECT:          return dm_printobject( hwnd, mp1, mp2 );
   case DM_RENDERPREPARE:        return dm_renderprepare( hwnd, mp1, mp2 );
   case LAZY_SETOBJECTHWND:      return lazy_setobjecthwnd( hwnd, mp1, mp2 );
   case WM_BUTTON1DBLCLK:        return wm_button1dblclk( hwnd, mp1, mp2 );
   case WM_BEGINDRAG:            return wm_begindrag( hwnd, mp1, mp2 );
   case WM_CHAR:                 return wm_char( hwnd, mp1, mp2 );
   case WM_CLOSE:                return wm_close( hwnd, mp1, mp2 );
   case WM_COMMAND:              return wm_command( hwnd, mp1, mp2 );
   case WM_PICKUP:               return wm_pickup( hwnd, mp1, mp2 );
   default:                      return pLBDefProc( hwnd, msg, mp1, mp2 );
   } /* endswitch */
}

// *******************************************************************************
// General (non direct manipulation) messages.
// *******************************************************************************
// *******************************************************************************
// FUNCTION:     lazy_setobjecthwnd
//
// FUNCTION USE: Handles processing of the LAZY_SETOBJECTHWND message.
//
// DESCRIPTION:  This message is sent from the object window thread to notify
//               the client window that the object window has been created.
//               The handle to the window is stored and messages are sent
//               back to the object window to initialize the listbox.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   object window handle
//               MPARAM   not used
//
// RETURNS:      MRESULT  reserved value - zero
//
// INTERNALS:    NONE
//
// *******************************************************************************

static MRESULT lazy_setobjecthwnd( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
// --------------------------------------------------------------------------
// retrieve the object window handle from mp1 and store in global variable.
// --------------------------------------------------------------------------
   ghwndObject = HWNDFROMMP(mp1);

// --------------------------------------------------------------------------
// Tell the object window to set the current directory - from ini if exist
// --------------------------------------------------------------------------
   WinPostMsg( ghwndObject, LAZY_DIRFROMINI,
               MPFROMHWND(hwnd), MPFROMLONG(0L));

// --------------------------------------------------------------------------
// Tell the object window to fill the listbox with the file names in the
// current directory.
// --------------------------------------------------------------------------
   WinPostMsg( ghwndObject, LAZY_FILLLIST, MPFROMHWND(hwnd), NULL );

// --------------------------------------------------------------------------
// Done - return the reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wm_button1dblclk
//
// FUNCTION USE: Handles processing of the WM_BUTTON1DBLCLK message.
//
// DESCRIPTION:  Causes the listbox to be refreshed.  If the currently selected
//               listbox item is a directory, the default directory is set to
//               the selected directory before the refresh occurs.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   not used
//               MPARAM   not used
//
// RETURNS:      MRESULT  TRUE - message processed
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wm_button1dblclk( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
// --------------------------------------------------------------------------
// Call routine to refresh the directory.
// --------------------------------------------------------------------------
   FillNewDir( hwnd );

// --------------------------------------------------------------------------
// indicate that message was processed.
// --------------------------------------------------------------------------
   return (MRESULT)TRUE;
}

// *******************************************************************************
// FUNCTION:     wm_char
//
// FUNCTION USE: Handles processing of the WM_CHAR message.
//
// DESCRIPTION:  Subclasses the listbox keyboard input handling.  If a new
//               line or enter key is pressed, refreshes the listbox
//               contents after changing to a new directory if necessary.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   flags in low-order 16 bits
//               MPARAM   character code in low byte of high-order 16 bits.
//
// RETURNS:      MRESULT  TRUE if NEWLINE or ENTER; otherwise, default
//                        listbox procedure return code.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wm_char( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   if ((SHORT1FROMMP(mp1) &
       (KC_VIRTUALKEY|KC_CTRL|KC_ALT|KC_SHIFT|KC_KEYUP)) == KC_VIRTUALKEY) {
// --------------------------------------------------------------------------
// Received a virtual key with no modifiers.
// --------------------------------------------------------------------------

      if (CHAR3FROMMP(mp2) == VK_ENTER || CHAR3FROMMP(mp2) == VK_NEWLINE) {
// --------------------------------------------------------------------------
// The key was either a NEWLINE or ENTER - refresh the directory
// --------------------------------------------------------------------------
         FillNewDir( hwnd );

// --------------------------------------------------------------------------
// return indication that the message was processed.
// --------------------------------------------------------------------------
         return (MRESULT)TRUE;

      } else {
// --------------------------------------------------------------------------
// other virtual key - pass the message on to the default procedure
// --------------------------------------------------------------------------
         return pLBDefProc( hwnd, WM_CHAR, mp1, mp2 );
      } /* endif */

   } else {
// --------------------------------------------------------------------------
// Other key - pass the message on to the default procedure
// --------------------------------------------------------------------------
      return pLBDefProc( hwnd, WM_CHAR, mp1, mp2 );
   } /* endif */
}

// *******************************************************************************
// FUNCTION:     wm_close
//
// FUNCTION USE: Handles processing of the WM_CLOSE message.
//
// DESCRIPTION:  Subclasses the listbox WM_CLOSE handling.  Saves the
//               current directory and window positions in the INI files before
//               calling the default procedure.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   not used
//               MPARAM   not used
//
// RETURNS:      MRESULT  listbox procedure return code.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wm_close( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
// --------------------------------------------------------------------------
// Tell the object window to save the current directory information
// --------------------------------------------------------------------------
   WinSendMsg( ghwndObject, LAZY_INIFROMDIR, MPFROMLONG(0L), MPFROMLONG(0L));

// --------------------------------------------------------------------------
// Save the positions of the windows in the INI file
// --------------------------------------------------------------------------
   WinStoreWindowPos( INIAPPNAME, INIFRMPOSKEY, WinQueryWindow( hwnd, QW_PARENT) );
   WinStoreWindowPos( INIAPPNAME, INILBXPOSKEY, hwnd );

// --------------------------------------------------------------------------
// pass the message on to the default procedure
// --------------------------------------------------------------------------
   return pLBDefProc( hwnd, WM_CLOSE, mp1, mp2 );
}

// *******************************************************************************
// FUNCTION:     wm_command
//
// FUNCTION USE: Handles processing of the WM_COMMAND message.
//
// DESCRIPTION:  Processes menu selections for the application to implement
//               pickup and drop from the menu.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   command id in the low-order 16 bits
//               MPARAM   not used
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wm_command( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   POINTL      ptl = {0,0};
   PDRAGINFO   pdi;
   HWND        hwndFrame;
   HWND        hwndMenu;
   ULONG       ulCurDisk;
   ULONG       ulDiskMap;

// --------------------------------------------------------------------------
// First handle the menu items for selecting a disk drive
// --------------------------------------------------------------------------
   if (SHORT1FROMMP(mp1) >= MID_DRIVEA && SHORT1FROMMP(mp1) <= MID_DRIVEZ ) {

// --------------------------------------------------------------------------
// Save the current drive and then set the current drive to the menu item
// selected
// --------------------------------------------------------------------------
      DosQueryCurrentDisk( &ulCurDisk, &ulDiskMap );
      DosSetDefaultDisk( (LONG)SHORT1FROMMP(mp1) - MID_DRIVE );

// --------------------------------------------------------------------------
// Erase and then refill the listbox from the current directory of the new
// drive
// --------------------------------------------------------------------------
      WinSendMsg( hwnd, LM_DELETEALL, 0l, 0l );
      WinPostMsg( ghwndObject, LAZY_FILLLIST,
                  MPFROMHWND(hwnd), MPFROMP( NULL ) );

// --------------------------------------------------------------------------
// Get the menu window, uncheck the old current drive and check the new one.
// --------------------------------------------------------------------------
      hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
      hwndMenu = WinWindowFromID( hwndFrame, FID_MENU );
      WinCheckMenuItem( hwndMenu, ulCurDisk + MID_DRIVE, FALSE );
      WinCheckMenuItem( hwndMenu, SHORT1FROMMP(mp1), TRUE );

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
      return MRFROMLONG(0L);
   }

// --------------------------------------------------------------------------
// Now process the rest of the commands
// --------------------------------------------------------------------------
   switch (SHORT1FROMMP(mp1)) {

   case MID_PICKUP:
// --------------------------------------------------------------------------
// MID_PICKUP: Send a WM_PICKUP message to ourself to start operation.
// --------------------------------------------------------------------------
      WinSendMsg( hwnd, WM_PICKUP, MPFROMP(&ptl), MPFROMLONG(0L));
      break;

   case MID_DROPCOPY:
// --------------------------------------------------------------------------
// MID_DROPCOPY: force current objects to be dropped on ourself.
// --------------------------------------------------------------------------
      if (DrgQueryDragStatus() == DGS_LAZYDRAGINPROGRESS &&
          (pdi = DrgQueryDraginfoPtr(NULL)) != NULL ) {
// --------------------------------------------------------------------------
// If a drag is actually in progress (we got a pointer), get access to the
// DRAGINFO structure and then drop as a copy operation.
// --------------------------------------------------------------------------
         DrgAccessDraginfo( pdi );
         DrgLazyDrop( hwnd, DO_COPY, &ptl );
      } /* endif */
      break;

   case MID_DROPMOVE:
// --------------------------------------------------------------------------
// MID_DROPMOVE: force current object to be dropped on ourself
// --------------------------------------------------------------------------
      if (DrgQueryDragStatus() == DGS_LAZYDRAGINPROGRESS &&
          (pdi = DrgQueryDraginfoPtr(NULL)) != NULL ) {
// --------------------------------------------------------------------------
// If a drag is actually in progress (we got a pointer), get access to the
// DRAGINFO structure and then drop as a copy operation.
// --------------------------------------------------------------------------
         DrgAccessDraginfo( pdi );
         DrgLazyDrop( hwnd, DO_MOVE, &ptl );
      } /* endif */
      break;

   case MID_CANCEL:
// --------------------------------------------------------------------------
// MID_CANCEL: Kill any drag operation currently in progress.
// --------------------------------------------------------------------------
      DrgCancelLazyDrag( );
      break;

   case MID_PRODINFO:
// --------------------------------------------------------------------------
// MID_PRODINFO: Display the product information dialog
// --------------------------------------------------------------------------
      WinDlgBox(HWND_DESKTOP, hwnd, WinDefDlgProc, NULLHANDLE, DLGID_PRODINFO, NULL); 
      break;
   } /* endswitch */

// --------------------------------------------------------------------------
// return reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// Worker routines that process messages sent to the source of a direct
// manipulation operation.
// *******************************************************************************
// *******************************************************************************
// FUNCTION:     wm_begindrag
//
// FUNCTION USE: Handles processing of the WM_BEGINDRAG message.
//
// DESCRIPTION:  This message indicates that a drag-and-drop operation has been
//               requested.  The function calls the drag initialization routine
//               with the pickup flag clear.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   not used
//               MPARAM   not used
//
// RETURNS:      MRESULT  TRUE to indicate message was processed.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wm_begindrag( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
// --------------------------------------------------------------------------
// Initialize the direct manipulation operation
// --------------------------------------------------------------------------
   StartDragOp( hwnd, FALSE );

// --------------------------------------------------------------------------
// indicate that the message was processed
// --------------------------------------------------------------------------
   return (MRESULT)TRUE;
}

// *******************************************************************************
// FUNCTION:     wm_pickup
//
// FUNCTION USE: Handles processing of the WM_PICKUP message.
//
// DESCRIPTION:  This message indicates that a pickup-and-drop operation has been
//               requested.  The function calls the drag initialization routine
//               with the pickup flag set.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   not used
//               MPARAM   not used
//
// RETURNS:      MRESULT  TRUE to indicate message was processed.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wm_pickup( HWND hwnd, MPARAM mp1, MPARAM mp2)
{
// --------------------------------------------------------------------------
// Initialize the direct manipulation operation
// --------------------------------------------------------------------------
   StartDragOp( hwnd, TRUE );

// --------------------------------------------------------------------------
// indicate that the message was processed.
// --------------------------------------------------------------------------
   return (MRESULT)TRUE;
}

// *******************************************************************************
// FUNCTION:     dm_discardobject
//
// FUNCTION USE: Handles processing of the DM_DISCARDOBJECT message.
//
// DESCRIPTION:  This message is received when we are the source of a pickup-
//               and-drop operation and the objects have been dropped on
//               the shredder.  Though we have an opportunity to delete the
//               objects ourselves, we'll just let the shredder do it.
//
// PARAMETERS:   HWND     client window handle.
//               MPARAM   pointer to the DRAGINFO structure.
//               MPARAM   target window handle.
//
// RETURNS:      MRESULT  TRUE to indicate message was processed.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT dm_discardobject( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   WinPostMsg( ghwndObject, LAZY_DISCARDOBJ, mp1, mp2 );
   return MRFROMLONG(DRR_SOURCE);
}

// *******************************************************************************
// FUNCTION:     dm_dropnotify
//
// FUNCTION USE: Handles processing of the DM_DROPNOTIFY message.
//
// DESCRIPTION:  This message is received when we are the source of a pickup-
//               and-drop operation and the objects have been dropped or
//               the operation has been cancelled.  If the target of the
//               drop was not us, then free the DRAGINFO stucture.
//
//
// PARAMETERS:   HWND     client window handle.
//               MPARAM   pointer to the DRAGINFO structure.
//               MPARAM   target window handle.
//
// RETURNS:      MRESULT  TRUE to indicate message was processed.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT dm_dropnotify( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PDRAGINFO   pdinfo = (PDRAGINFO)mp1;
   HWND        hwndTarget = (HWND)mp2;

   if (hwnd != hwndTarget ) {

// --------------------------------------------------------------------------
// Did not drop on ourselves - release the DRAGINFO structure.  The target
// will send us back any inforamtion we need.  Except the APIs don't work
// to retrieve the DRAGINFO structure - Leave it, we'll free when all
// objects are rendered.
// --------------------------------------------------------------------------
//      DrgFreeDraginfo( pdinfo );
   } /* endif */

// --------------------------------------------------------------------------
// return the appproriate reserved value
// --------------------------------------------------------------------------
   return (MRESULT)0L;
}

// *******************************************************************************
// FUNCTION:     dm_printobject
//
// FUNCTION USE: Handles processing of the DM_DISCARDOBJECT message.
//
// DESCRIPTION:  This message is received when we are the source of a pickup-
//               and-drop operation and the objects have been dropped on
//               a printer.  Though we have an opportunity to print the
//               objects ourselves, we'll just let the shredder do it.
//
// PARAMETERS:   HWND     client window handle.
//               MPARAM   pointer to the DRAGINFO structure.
//               MPARAM   target window handle.
//
// RETURNS:      MRESULT  TRUE to indicate message was processed.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT dm_printobject( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   return MRFROMLONG(DRR_TARGET);
}


// *******************************************************************************
// FUNCTION:     dm_renderprepare
//
// FUNCTION USE: Handles processing of the DM_RENDERPREPARE message.
//
// DESCRIPTION:  This message is received when we are the source of a direct
//               manipulation operation, if we indicated that the message
//               should be sent.  We prepare for rendering by specifying that
//               the target should converse with the object window during
//               rendering.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   DRAGTRANSFER structure for the current item.
//               MPARAM   not used
//
// RETURNS:      MRESULT  TRUE to indicate message was processed and rendering
//                        can continue.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT dm_renderprepare( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PDRAGTRANSFER  pDragXfer = (PDRAGTRANSFER)mp1;

// --------------------------------------------------------------------------
// Tell the target to continue the operation by conversing with the object
// window.
// --------------------------------------------------------------------------
   pDragXfer->pditem->hwndItem = ghwndObject;

// --------------------------------------------------------------------------
// Indicate that the message has been processed and rendering may continue
// --------------------------------------------------------------------------
   return (MRESULT)TRUE;
}


/******************************************************************************
 *
 * Target Window Functions
 *
 *****************************************************************************/

// *******************************************************************************
// FUNCTION:     dm_dragleave
//
// FUNCTION USE: Handles processing of the DM_DRAGLEAVE message.
//
// DESCRIPTION:  This message is sent to a potential target window when the
//               dragged object image leaves the target window's area.
//
//
// PARAMETERS:   HWND     client window handle.
//               MPARAM   pointer to the DRAGINFO structure.
//               MPARAM   target window handle.
//
// RETURNS:      MRESULT  TRUE to indicate message was processed.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT dm_dragleave( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   if (fEmphasized) {
// --------------------------------------------------------------------------
// Emphasis was applied - remove emphasis from the window
// --------------------------------------------------------------------------
      DrawEmphasis( hwnd );
// --------------------------------------------------------------------------
// indicate that emphasis is no longer applied
// --------------------------------------------------------------------------
      fEmphasized = FALSE;
   } /* endif */

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG( 0L );
}

// *******************************************************************************
// FUNCTION:     dm_dragover
//
// FUNCTION USE: Handles processing of the DM_DRAGOVER message.
//
// DESCRIPTION:  This message is sent to a potential target window when the
//               dragged object image is moved within the window's area.  Multiple
//               messages are received to allow the target to determine the
//               ability to drop at each particular location within its window.
//
// PARAMETERS:   HWND     client window handle.
//               MPARAM   pointer to the DRAGINFO structure.
//               MPARAM   target window handle.
//
// RETURNS:      MRESULT  TRUE to indicate message was processed.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT dm_dragover( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PDRAGINFO      pdInfo = (PDRAGINFO)NULL;
   USHORT         usDrop = DOR_NEVERDROP;
   USHORT         usDefaultOp = DO_MOVE;
   PDRAGITEM      pdItem = NULL;
   ULONG          lItem = 0;

   do {
      if (!fEmphasized) {
// --------------------------------------------------------------------------
// establish target emphasis for the window
// --------------------------------------------------------------------------
         DrawEmphasis(hwnd);
         fEmphasized = TRUE;
      } /* endif */

// --------------------------------------------------------------------------
// make sure the DRAGINFO structure can be accessed
// --------------------------------------------------------------------------
      if (!(pdInfo = (PDRAGINFO)mp1) || !DrgAccessDraginfo( pdInfo )) {
         pdInfo = (PDRAGINFO)NULL;     /* prevent free at end */
         break;
      } /* endif */

// --------------------------------------------------------------------------
// assume that a drop can occur - for now
// --------------------------------------------------------------------------
      usDrop = DOR_DROP;

      switch( pdInfo->usOperation ) {
// --------------------------------------------------------------------------
// set the operation type that will be performed
// --------------------------------------------------------------------------
      case DO_COPY: usDefaultOp = DO_COPY; break;
      case DO_MOVE: usDefaultOp = DO_MOVE; break;
      case DO_DEFAULT: usDefaultOp = DO_MOVE; break;
      case DO_UNKNOWN: usDefaultOp = DO_MOVE; break;
      default: usDrop = DOR_NODROPOP; break;
      }

// --------------------------------------------------------------------------
// scan each of the dragged objects for drop capability
// --------------------------------------------------------------------------
      for( lItem = 0; lItem < pdInfo->cditem; lItem++ ) {

// --------------------------------------------------------------------------
// get the address of the dragged item
// --------------------------------------------------------------------------
         if( (pdItem = DrgQueryDragitemPtr(pdInfo, lItem)) != NULL ) {
// --------------------------------------------------------------------------
// and may sure that the OS/2 FILE mechanism is available
// --------------------------------------------------------------------------
            if( !DrgVerifyRMF( pdItem, "DRM_OS2FILE", NULL )) {
// --------------------------------------------------------------------------
// no OS/2 FILE mechanism - the object cannot be dropped
// --------------------------------------------------------------------------
               usDrop = DOR_NEVERDROP;
               break;
            }

// --------------------------------------------------------------------------
// NOTE: this application is not sensitive to the file type, so no check
//       is done.  If the type is important, insert a check of the file
//       type here.
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Make sure that the operation to be performed is supported
// --------------------------------------------------------------------------
            if( usDefaultOp == DO_COPY &&
                !(pdItem->fsSupportedOps & DO_COPYABLE )) {
               usDrop = DOR_NODROPOP;
            } else if( usDefaultOp == DO_MOVE &&
                       !(pdItem->fsSupportedOps & DO_MOVEABLE )) {
               usDrop = DOR_NODROPOP;
            }
         }
      }
   } while ( false  ); /* enddo */

// --------------------------------------------------------------------------
// release the DRAGINFO structure
// --------------------------------------------------------------------------
   if (pdInfo != (PDRAGINFO)NULL) DrgFreeDraginfo( pdInfo );

// --------------------------------------------------------------------------
// return with the drop capability and operation
// --------------------------------------------------------------------------
   return MRFROM2SHORT( usDrop, usDefaultOp );
}

// *******************************************************************************
// FUNCTION:     dm_drop
//
// FUNCTION USE: Handles processing of the DM_DROP message.
//
// DESCRIPTION:  This message is sent to the target window when the dragged
//               object's are actually dropped.
//
// PARAMETERS:   HWND     client window handle.
//               MPARAM   pointer to the DRAGINFO structure.
//               MPARAM   target window handle.
//
// RETURNS:      MRESULT  TRUE to indicate message was processed.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT dm_drop( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PDRAGINFO   pdInfo = (PDRAGINFO)mp1;

// --------------------------------------------------------------------------
// Get access to the drag information structure
// --------------------------------------------------------------------------
   if (DrgAccessDraginfo( pdInfo )) {
// --------------------------------------------------------------------------
// Send the drop off to the object window to perform the rendering in a
// separate thread.
// --------------------------------------------------------------------------
      WinPostMsg( ghwndObject, LAZY_DODROP, MPFROMHWND(hwnd), MPFROMP(pdInfo));
   } /* endif */

   if (fEmphasized) {
// --------------------------------------------------------------------------
// Clear the target emphasis
// --------------------------------------------------------------------------
      DrawEmphasis( hwnd );
// --------------------------------------------------------------------------
// and reset the emphasis applied flag
// --------------------------------------------------------------------------
      fEmphasized = FALSE;
   } /* endif */

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG( 0L );
}


/******************************************************************************
 *
 * Utility Functions
 *
 *****************************************************************************/
// *******************************************************************************
// FUNCTION:     FillNewDir
//
// FUNCTION USE: Reloads the list box with the contents of a different directory
//
// DESCRIPTION:  Obtains the directory name from the currently selected item
//               in the list box and then sends a message to the object window
//               to fill the listbox.
//
// PARAMETERS:   HWND     client window handle.
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void FillNewDir( HWND hwnd )
{
   SHORT    sItem;
   char     *pszItem = NULL;

// --------------------------------------------------------------------------
// Find the first selected item in the listbox
// --------------------------------------------------------------------------
   sItem = SHORT1FROMMR(WinSendMsg( hwnd, LM_QUERYSELECTION,
                        MPFROMLONG(LIT_FIRST), MPFROMLONG(0) ));
   if (sItem != LIT_NONE) {

// --------------------------------------------------------------------------
// Allocate enough memory for a full file path name
// --------------------------------------------------------------------------
      if (DosAllocMem( (PVOID)&pszItem, CCHMAXPATHCOMP + 1,
          PAG_COMMIT | PAG_READ | PAG_WRITE ) == NO_ERROR ) {

// --------------------------------------------------------------------------
// retrieve the item from the list box
// --------------------------------------------------------------------------
         WinSendMsg( hwnd, LM_QUERYITEMTEXT,
                     MPFROM2SHORT( sItem, CCHMAXPATHCOMP + 1 ),
                     MPFROMP( pszItem ));

// --------------------------------------------------------------------------
// clear the listbox content
// --------------------------------------------------------------------------
         WinSendMsg( hwnd, LM_DELETEALL, 0l, 0l );

// --------------------------------------------------------------------------
// tell the object window to fill the list box
// --------------------------------------------------------------------------
         WinPostMsg( ghwndObject, LAZY_FILLLIST,
                     MPFROMHWND(hwnd), MPFROMP( pszItem ) );

// --------------------------------------------------------------------------
// NOTE: memory will be freed by the object window
// --------------------------------------------------------------------------
      } /* endif */
   } /* endif */
}

// *******************************************************************************
// FUNCTION:     StartDragOp
//
// FUNCTION USE: Initiates a drag-and-drop or pickup-and-drop operation.
//
// DESCRIPTION:  Allocates and fills the DRAGINFO and DRAGITEM structures and
//               starts the drag operation
//
// PARAMETERS:   HWND     client window handle.
//               BOOL     set to indicate a pickup-and-drag operation
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void StartDragOp( HWND hwnd, BOOL fLazy )
{
   ULONG       cFiles;           /* number of files to drag */
   DRAGIMAGE   dImage;           /* an array of one DRAGIMAGE */
   PDRAGINFO   pDinfo = NULL;    /* pointer to the DRAGINFO */
   HWND        hwndDest;
   BOOL        fLazyRealloc = FALSE;

   do {
// --------------------------------------------------------------------------
// get the number of items(files) selected in the listbox, don't start the
// drag if no files are selected */
// --------------------------------------------------------------------------
      if ((cFiles = QuerySelectionCount( hwnd )) == 0) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// If doing pickup-and-drop and operation already in progress - make sure
// this window started the operation.  Then set a flag to indicate the
// reallocation may be needed.
// --------------------------------------------------------------------------
      if (fLazy && DrgQueryDragStatus() == DGS_LAZYDRAGINPROGRESS) {
         /* get DRAGINFO pointer for this window */
         pDinfo = DrgQueryDraginfoPtrFromHwnd(hwnd);
         if (pDinfo == NULL) {
            /* must be someone else's operation - exit */
            break;
         } /* endif */
         fLazyRealloc = TRUE;
      } else {       /* do drag and drop */

// --------------------------------------------------------------------------
// otherwise, this must be new operation so start allocate the DRAGINFO.
// --------------------------------------------------------------------------
         if ((pDinfo = DrgAllocDraginfo( cFiles )) == (PDRAGINFO)(NULL)) {
            break;
         } /* endif */
      } /* endif */

// --------------------------------------------------------------------------
// set up the drag image array - only one array element is used by this
// application.
// --------------------------------------------------------------------------
      dImage.cb = sizeof(DRAGIMAGE);
// --------------------------------------------------------------------------
// Set up to use a pointer and then assign the pointer to use based on the
// number of files being dragged.
// --------------------------------------------------------------------------
      dImage.fl = DRG_ICON;
      dImage.hImage = (LHANDLE)WinQuerySysPointer( HWND_DESKTOP,
                               cFiles == 1 ? SPTR_FILE : SPTR_MULTFILE,
                               FALSE );
      dImage.cxOffset = 0;
      dImage.cyOffset = 0;

// --------------------------------------------------------------------------
// not required here - but good practice to initialize anyway */
// --------------------------------------------------------------------------
      dImage.cptl = 0;
      dImage.sizlStretch.cx = 0;
      dImage.sizlStretch.cy = 0;

// --------------------------------------------------------------------------
// Fill in the DRAGITEMs for the DRAGINFO
// --------------------------------------------------------------------------
      SetDragItems( &pDinfo, hwnd, fLazyRealloc );

// --------------------------------------------------------------------------
// Save a count of the items dragged so we know when all have been rendered
// --------------------------------------------------------------------------
      gulDragCount = pDinfo->cditem;

// --------------------------------------------------------------------------
// Save a pointer to the DRAGINFO - APIs to query a pointer to the structure
// have problems.
// --------------------------------------------------------------------------
      gpSrcDinfo = pDinfo;

// --------------------------------------------------------------------------
// Start/Restart the operation depending on the type
// --------------------------------------------------------------------------
      if (fLazy) {
         DrgLazyDrag( hwnd, pDinfo, &dImage, 1, 0L );
      } else {
         hwndDest = DrgDrag( hwnd, pDinfo, &dImage, 1, VK_ENDDRAG, NULL );

// --------------------------------------------------------------------------
// For drag-and-drop, release the DRAGITEM and DRAGINFO structures if the
// operation was cancelled.
// --------------------------------------------------------------------------
         if (hwndDest == NULLHANDLE ) {
            DrgDeleteDraginfoStrHandles( pDinfo );
            DrgFreeDraginfo( pDinfo );
         } /* endif */
      } /* endif */
   } while ( false ); /* enddo */
}

// *******************************************************************************
// FUNCTION:     QuerySelectionCount
//
// FUNCTION USE: Retrieves the number of items selected in the listbox
//
// DESCRIPTION:  Scans the listbox for selected items and returns the number
//               of items found
//
// PARAMETERS:   HWND     listbox window handle.
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static ULONG QuerySelectionCount( HWND hwndList )
{
   SHORT    sItem;
   ULONG    ulSelCount = 0;

// --------------------------------------------------------------------------
// Start scanning the listbox with the first item
// --------------------------------------------------------------------------
   sItem = LIT_FIRST;

// --------------------------------------------------------------------------
// find the next selected item in the listbox
// --------------------------------------------------------------------------
   while ((sItem = SHORT1FROMMR(WinSendMsg( hwndList, LM_QUERYSELECTION,
           MPFROMSHORT(sItem), MPFROMLONG(0L)))) != LIT_NONE ) {

// --------------------------------------------------------------------------
// bump the count for each item found
// --------------------------------------------------------------------------
      ulSelCount++;
   } /* endwhile */

// --------------------------------------------------------------------------
// done - return the count
// --------------------------------------------------------------------------
   return ulSelCount;
}

// *******************************************************************************
// FUNCTION:     SetDragItems
//
// FUNCTION USE: Fills the DRAGITEM structures for a direct manipulation operation
//
// DESCRIPTION:  Scans each selected item in the listbox and fills in a DRAGITEM
//               structure.  If necessary for pickup-and-drop, reallocates the
//               DRAGINFO structure.
//
// PARAMETERS:   PDRAGINFO    pointer to DRAGINFO structure for the operation.
//               HWND         listbox window handle
//               BOOL         set if pickup-and-drop operation already in progress
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void SetDragItems( PDRAGINFO *ppDinfo, HWND hwnd, BOOL fLazy )
{
   SHORT       sItem;               /* Current listbox item */
   SHORT       cbItem;              /* Temp - selected text length */
   PSZ         pszFileName;         /* Temp - selected text */
   PSZ         pszType;             /* Temp - file type */
   PSZ         pszContainer;        /* Temp - current directory */
   DRAGITEM    dItem;               /* DRAGITEM structure */
   ULONG       litem;               /* DRAGITEM item */
   PDRAGINFO   pDinfo = *ppDinfo;

// --------------------------------------------------------------------------
// first fill in the prototype DRAGITEM with the constant fields
//   rendering mechanism/format
//   rendering conversation window handle
//   supported operations
//   control flags
// --------------------------------------------------------------------------
   dItem.hstrRMF = DrgAddStrHandle( SUPPORTED_RMF );
   dItem.hwndItem = hwnd;
   dItem.fsSupportedOps = DO_COPYABLE | DO_MOVEABLE;
   dItem.fsControl = DC_PREPARE;


// --------------------------------------------------------------------------
// start with DRAGITEM index 0 and start scan from the beginning of the list
// --------------------------------------------------------------------------
   litem = 0;
   sItem = LIT_FIRST;

// --------------------------------------------------------------------------
// Fill in a DRAGITEM for each selected item in the listbox
// --------------------------------------------------------------------------
   while ((sItem = SHORT1FROMMR( WinSendMsg( hwnd, LM_QUERYSELECTION,
                           MPFROMSHORT( sItem ), 0L ))) != LIT_NONE ) {

// --------------------------------------------------------------------------
// determine the length of the text of the item and allocate memory
// --------------------------------------------------------------------------
      cbItem = SHORT1FROMMR( WinSendMsg( hwnd, LM_QUERYITEMTEXTLENGTH,
                             MPFROMSHORT( sItem ), 0L ));
      cbItem++;
      if ((pszFileName = (PSZ)malloc( cbItem )) != (PSZ)NULL) {

// --------------------------------------------------------------------------
// Get the name of the file
// --------------------------------------------------------------------------
         WinSendMsg( hwnd, LM_QUERYITEMTEXT,
                  MPFROM2SHORT( sItem, cbItem ), pszFileName );

// --------------------------------------------------------------------------
// If a pickup-and-drop operation, check to see if a DRAGITEM is already
// allocated since we can reinitiate the operation with some of the same
// files still selected.
// --------------------------------------------------------------------------
         if (!fLazy || GetItemForFile( pDinfo, pszFileName) == -1) {

// --------------------------------------------------------------------------
// fill in the file type - we should always have at least DRT_UNKNOWN but
// use NULL handle if the string or handle could not be allocated for
// some reason.
// --------------------------------------------------------------------------
            if ((pszType = QueryType( pszFileName )) != NULL) {
               dItem.hstrType = DrgAddStrHandle( pszType );
               free( pszType );
            } else {
               dItem.hstrType = NULLHANDLE;
            } /* endif */

// --------------------------------------------------------------------------
// set the source container to the current directory.
// --------------------------------------------------------------------------
            if ((pszContainer = QueryCurrentDirectory()) != NULL) {
               dItem.hstrContainerName = DrgAddStrHandle( pszContainer );
               free( pszContainer );
            } else {
               dItem.hstrContainerName = NULLHANDLE;
            } /* endif */

// --------------------------------------------------------------------------
// set the source name to the selected item name and recommend that the
// target name be the same.
// --------------------------------------------------------------------------
            dItem.hstrSourceName = dItem.hstrTargetName =
                                          DrgAddStrHandle( pszFileName );
            if (fLazy) {

// --------------------------------------------------------------------------
// reallocate the DRAGINFO, adding one DRAGITEM, for pickup-and-drop.  Set
// the item index to last DRAGITEM
// --------------------------------------------------------------------------
               *ppDinfo = pDinfo = ReallocDraginfo( pDinfo );
               litem = pDinfo->cditem - 1;
            } /* endif */

// --------------------------------------------------------------------------
// copy the prototype DRAGITEM to the proper index in the DRAGINFO.
// increment the item index
// --------------------------------------------------------------------------
            DrgSetDragitem( pDinfo, &dItem, sizeof( DRAGITEM ), litem++ );
         } /* endif */

// --------------------------------------------------------------------------
// clean up allocated memory
// --------------------------------------------------------------------------
         free( pszFileName );
      } /* endif */
   } /* endwhile */

   return;
}

// *******************************************************************************
// FUNCTION:     GetItemForFile
//
// FUNCTION USE: Determines the DRAGITEM index for a given file name
//
// DESCRIPTION:  Scans the DRAGITEM structures associated with a DRAGINFO
//               structure and returns the index of the DRAGITEM which matches
//               the given file name
//
// PARAMETERS:   PDRAGINFO    pointer to DRAGINFO structure for the operation.
//               PSZ          pointer to filename to find
//
// RETURNS:      SHORT        DRAGITEM index or -1 if item not found
//
// INTERNALS:    NONE
//
// *******************************************************************************
static SHORT GetItemForFile( PDRAGINFO pDinfo, PSZ pszFileName )
{
   SHORT       item;
   PSZ         pszSource;
   ULONG       cbSource;
   PDRAGITEM   pDitem;

// --------------------------------------------------------------------------
// Scan each of the DRAGITEMs
// --------------------------------------------------------------------------
   for (item = 0; item < pDinfo->cditem; item++) {

// --------------------------------------------------------------------------
// Get a pointer to the DRAGITEM
// --------------------------------------------------------------------------
      pDitem = DrgQueryDragitemPtr( pDinfo, item );

// --------------------------------------------------------------------------
// Get the length of the source file name string and allocate memory
// --------------------------------------------------------------------------
      cbSource = DrgQueryStrNameLen( pDitem->hstrSourceName );
      pszSource = (PSZ)malloc( ++cbSource );

// --------------------------------------------------------------------------
// Copy the file name into to the allocated memory
// --------------------------------------------------------------------------
      DrgQueryStrName( pDitem->hstrSourceName, cbSource, pszSource );

// --------------------------------------------------------------------------
// done if name matches requested name - otherwise check the next DRAGITEM
// --------------------------------------------------------------------------
      if( !strcmp( pszFileName, pszSource )) break;
   } /* endfor */

// --------------------------------------------------------------------------
// return the found index or -1 if all items were scanned without a match
// --------------------------------------------------------------------------
   if( item == pDinfo->cditem ) item = -1;
   return item;
}

// *******************************************************************************
// FUNCTION:     ReallocDraginfo
//
// FUNCTION USE: Reallocate the DRAGINFO structure when additional items are
//               added during a pickup-and-drop operation.
//
// DESCRIPTION:  Saves the current set of DRAGITEM structures, reallocates the
//               DRAGINFO structure and then restores the saved DRAGITEMs.
//
// PARAMETERS:   PDRAGINFO    pointer to DRAGINFO structure for the operation.
//
// RETURNS:      PDRAGINFO    address of DRAGINFO structure after reallocation.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static PDRAGINFO ReallocDraginfo( PDRAGINFO pDinfo )
{
   PDRAGITEM   pdItem;
   ULONG       cdItem = pDinfo->cditem;
   ULONG       iItem;

// --------------------------------------------------------------------------
// allocate memory to hold the all DRAGITEMS
// --------------------------------------------------------------------------
   pdItem = (PDRAGITEM)malloc(cdItem * sizeof(DRAGITEM));

// --------------------------------------------------------------------------
// copy the DRAGITEMs to the allocated memory
// --------------------------------------------------------------------------
   for (iItem = 0; iItem < cdItem; iItem++) {
      DrgQueryDragitem( pDinfo, sizeof(DRAGITEM), &pdItem[iItem], iItem );
   } /* endfor */

// --------------------------------------------------------------------------
// Reallocate the DRAGINFO structure with one more DRAGITEM
// --------------------------------------------------------------------------
   pDinfo = DrgReallocDraginfo( pDinfo, cdItem + 1 );

// --------------------------------------------------------------------------
// Reassociate the DRAGITEMs saved with the new DRAGINFO structure
// --------------------------------------------------------------------------
   for (iItem = 0; iItem < cdItem; iItem++) {
      DrgSetDragitem( pDinfo, &pdItem[iItem], sizeof(DRAGITEM), iItem );
   } /* endfor */

// --------------------------------------------------------------------------
// free the allocated memory
// --------------------------------------------------------------------------
   free(pdItem);

// --------------------------------------------------------------------------
// return a pointer to the new DRAGINFO structure
// --------------------------------------------------------------------------
   return pDinfo;
}

// *******************************************************************************
// FUNCTION:     QueryType
//
// FUNCTION USE: Builds the DRAGITEM hstrType string for a file.
//
// DESCRIPTION:  Builds a full path name for the file and then retrieves
//               the extended attributes for the file.  Returns the EA .TYPE of
//               the file appended to the unknown type.
//
// PARAMETERS:   PSZ          pointer to the file name
//
// RETURNS:      PSZ          pointer to the file type
//
// INTERNALS:    NONE
//
// *******************************************************************************
static PSZ QueryType( PSZ pszFileName )
{
   EAOP2    eaop;
   ULONG    cbList;
   char     szFullName[ CCHMAXPATH ];
   PSZ      pszType;
   PCHAR    pValue;
   USHORT   cbValue = 0;

// --------------------------------------------------------------------------
// Get the full path name for the requested file
// --------------------------------------------------------------------------
   DosQueryPathInfo( pszFileName, FIL_QUERYFULLNAME,
                     szFullName, CCHMAXPATH );

// --------------------------------------------------------------------------
// allocate memory for the GetEAList structure - cblist + one structure +
// length of the attribute name requested (.TYPE)
// --------------------------------------------------------------------------
   cbList = sizeof( ULONG ) + sizeof( GEA2 ) + 5;
   eaop.fpGEA2List = malloc( cbList );
   if (eaop.fpGEA2List == NULL) {
      return NULL;
   } /* endif */

// --------------------------------------------------------------------------
// fill in the length of the list, set the next offset to zero since there
// is only one structure, set the length of the attribute name and copy
// in the attribute name
// --------------------------------------------------------------------------
   eaop.fpGEA2List->cbList = cbList;
   /* Fill in the GetEA structure */
   eaop.fpGEA2List->list->oNextEntryOffset = 0;
   eaop.fpGEA2List->list->cbName = 5;
   strcpy( eaop.fpGEA2List->list->szName, ".TYPE" );

// --------------------------------------------------------------------------
// Allocate memory for the FEAList where the attributes are returned.
// size is the length of the count field, plus the size of a list entry
// base structure, plus 300 bytes for storing the attribute
// --------------------------------------------------------------------------
   cbList = sizeof( ULONG ) + sizeof( FEA2 ) + 300;
   eaop.fpFEA2List = malloc( cbList );
   if (eaop.fpFEA2List == NULL) {
      free( eaop.fpGEA2List );
      return NULL;
   } /* endif */
   eaop.fpFEA2List->cbList = cbList;

// --------------------------------------------------------------------------
// get the file EAs
// --------------------------------------------------------------------------
   DosQueryPathInfo( szFullName, FIL_QUERYEASFROMLIST,
                     &eaop, sizeof( eaop ));

// --------------------------------------------------------------------------
// determine the length required for the return string.  Start with the
// length of the default string
// --------------------------------------------------------------------------
   cbList = strlen( DRT_UNKNOWN ) + 1;

// --------------------------------------------------------------------------
// Now for some fun - parse the FEA list
// --------------------------------------------------------------------------
   if ( eaop.fpFEA2List->list->cbValue != 0) {

// --------------------------------------------------------------------------
// get a pointer to the value - stored in the name, but bypass the name
// portion of the field.
// --------------------------------------------------------------------------
      pValue = eaop.fpFEA2List->list->szName +
               eaop.fpFEA2List->list->cbName + 1;

// --------------------------------------------------------------------------
// pValue now points to the attribute type which must be ASCII for our
// purposes.
// --------------------------------------------------------------------------
      if (*(PUSHORT)pValue == EAT_ASCII) {

// --------------------------------------------------------------------------
// bump pvalue to the next word and get the length of the value - add this
// to the length of the string
// --------------------------------------------------------------------------
         pValue = (PCHAR)((PUSHORT)pValue + 1);
         cbValue = *(PUSHORT)pValue;
         cbList += (ULONG)cbValue;

// --------------------------------------------------------------------------
// bump the pointer to the actual type string
// --------------------------------------------------------------------------
         pValue = (PCHAR)((PUSHORT)pValue + 1);
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// now allocate space for the string
// --------------------------------------------------------------------------
   if ((pszType = malloc( cbList )) == NULL) {
// --------------------------------------------------------------------------
// error allocating - clean up memory allocation before exiting
// --------------------------------------------------------------------------
      free( eaop.fpGEA2List );
      free( eaop.fpFEA2List );
      return NULL;
   }

// --------------------------------------------------------------------------
// If an extended attribute was found copy it to the return string as the
// native type of the file.
// --------------------------------------------------------------------------
   if (cbValue != 0) {
      strncpy( pszType, pValue, cbValue );
      pszType[ cbValue++ ] = ',';
   } /* endif */

// --------------------------------------------------------------------------
// Add in the default type
// --------------------------------------------------------------------------
   pszType[ cbValue ] = '\0';
   strcat( pszType, DRT_UNKNOWN );

// --------------------------------------------------------------------------
// free the allocated memory
// --------------------------------------------------------------------------
   free( eaop.fpGEA2List );
   free( eaop.fpFEA2List );

// --------------------------------------------------------------------------
// return pointer to the type string to the caller
// --------------------------------------------------------------------------
   return pszType;
}

// *******************************************************************************
// FUNCTION:     QueryCurrentDirectory
//
// FUNCTION USE: Determines the full path name of the current working directory
//
// DESCRIPTION:  Queries the operating system for the current drive and directory
//               and allocates and builds the full path name string for the
//               current drive and directory.
//
// PARAMETERS:   void
//
// RETURNS:      PSZ          pointer to the full path name
//
// INTERNALS:    NONE
//
// *******************************************************************************
static PSZ QueryCurrentDirectory( void )
{
   PSZ   pszDirectory = NULL;          /* pointer to buffer to build path */
   ULONG ulCurDisk;                    /* currently selected disk # */
   ULONG ulDriveMap;                   /* unused - disk presence map */
   ULONG cbPath = 0;                   /* # characters remaining */
   ULONG cbDir = 0;                    /* characters in directory */

// --------------------------------------------------------------------------
// Get the maximum length of a path string
// --------------------------------------------------------------------------
   DosQuerySysInfo( QSV_MAX_PATH_LENGTH, QSV_MAX_PATH_LENGTH,
                    (PVOID)&cbPath, sizeof( ULONG ));

// --------------------------------------------------------------------------
// allocate the output buffer
// --------------------------------------------------------------------------
   if ((pszDirectory = (PSZ)malloc( cbPath )) == (PSZ)NULL) {
      return NULL;
   } /* endif */

// --------------------------------------------------------------------------
// Get the current drive
// --------------------------------------------------------------------------
   DosQueryCurrentDisk( &ulCurDisk, &ulDriveMap );

// --------------------------------------------------------------------------
// Translate the drive to a letter and store in beginning of buffer
// --------------------------------------------------------------------------
   pszDirectory[ 0 ] = (char)ulCurDisk + 0x40;
   pszDirectory[ 1 ] = ':';
   pszDirectory[ 2 ] = '\\';

// --------------------------------------------------------------------------
// Get the current directory path into the remainder of the buffer
// --------------------------------------------------------------------------
   cbPath -= 3;
   DosQueryCurrentDir( ulCurDisk, &pszDirectory[ 3 ], &cbPath );

// --------------------------------------------------------------------------
// append terminating backslash
// --------------------------------------------------------------------------
   cbDir = strlen(&pszDirectory[3]);
   if (cbDir && (cbPath - cbDir) > 2) {
      strcat( pszDirectory, "\\" );
   } /* endif */

// --------------------------------------------------------------------------
// return a pointer to the path string to the caller
// --------------------------------------------------------------------------
   return pszDirectory;
}

// *******************************************************************************
// FUNCTION:     DrawEmphasis
//
// FUNCTION USE: Establishes/removes emphasis when objects are being dragged over
//               the window.
//
// DESCRIPTION:  Inverts a thin rectangle just inside the borders of the window.
//
// PARAMETERS:   void
//
// RETURNS:      PSZ          pointer to the full path name
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void DrawEmphasis(HWND hwnd)
{
   HPS      hps;
   RECTL    rclRect;

// --------------------------------------------------------------------------
// Get a presentation space for the window
// --------------------------------------------------------------------------
   hps = DrgGetPS( hwnd );
   if (hps != NULLHANDLE) {
// --------------------------------------------------------------------------
// Get the window rectangle and adjust for the borders and scroll bars
// --------------------------------------------------------------------------
      WinQueryWindowRect( hwnd, &rclRect );
      rclRect.xLeft += WinQuerySysValue( HWND_DESKTOP, SV_CXBORDER );
      rclRect.yTop -= WinQuerySysValue( HWND_DESKTOP, SV_CYBORDER );
      rclRect.xRight -= WinQuerySysValue( HWND_DESKTOP, SV_CXVSCROLL );
      rclRect.yBottom += WinQuerySysValue( HWND_DESKTOP, SV_CYBORDER ) + 1;

// --------------------------------------------------------------------------
// Invert a small border inside the normal border
// --------------------------------------------------------------------------
      WinDrawBorder( hps, &rclRect, 1, 1, 0l, 0l, DB_DESTINVERT | DB_STANDARD );

// --------------------------------------------------------------------------
// release the presentation space
// --------------------------------------------------------------------------
      DrgReleasePS( hps );
   } /* endif */
}

