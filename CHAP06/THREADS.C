// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      THREADS.C                                        mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Main Source File for THREADS/Chapter 06 Sample Program        บ
// บ                                                                               บ
// บ  NOTES:         This program demonstartes several methods for implmenting     บ
// บ                 long running operations in PM applications.                   บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#define INCL_PM
#include "threads.h"
#include "shcommon.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// --------------------------------------------------------------------------
// Miscellaneous constant definitions used only in this source module.
// --------------------------------------------------------------------------
#define  APP_HWNDOBJ                0
#define  APP_WINDOW_DATA_SIZE       4

#define  MAX_RECORD              1000
#define  RECORD_SIZE             1024

// --------------------------------------------------------------------------
// Message return codes
// --------------------------------------------------------------------------
#define  INIT_COMPLETE           0
#define  INIT_ERROR              1
#define  READY_OK                0
#define  READY_ERROR             1

// --------------------------------------------------------------------------
// Application (USER) defined messages used by this application.
// --------------------------------------------------------------------------
#define  UM_INIT                 (WM_USER)
#define  UM_READY                (WM_USER + 1)
#define  UM_WRITE                (WM_USER + 2)
#define  UM_SETOBJ               (WM_USER + 3)
#define  UM_DONE                 (WM_USER + 4)
#define  UM_CLOSE                (WM_USER + 5)
#define  UM_DELETE               (WM_USER + 6)

// --------------------------------------------------------------------------
// Definition of the structure used to pass information to a secondary
// thread when using the method which performs the whole operation in a
// separate thread.
// --------------------------------------------------------------------------
typedef struct __thread_param__ {
   HWND     hwnd;
   HWND     hwndSlider;
   char     szFile[CCHMAXPATH];
} TPARM, *PTPARM;

// --------------------------------------------------------------------------
// Definition of the structure used to pass information between the primary
// and secondary threads when using an object window in the secondary thread.
// --------------------------------------------------------------------------
typedef struct __objwnd_param__ {
   HWND     hwnd;
   HWND     hwndProgress;
   char     szFileName[CCHMAXPATH];
   FILE     *file;
   ULONG    ulRecord;
   ULONG    cbRecord;
   char     record[ RECORD_SIZE ];
} OBJPARM, *POBJPARM;


// --------------------------------------------------------------------------
// Miscellaneous global variables used only in this source module.
// --------------------------------------------------------------------------
static   BOOL  false = FALSE;
static   char  szClassName[] = "THREADS";
static   char  szObjClassName[] = "THREADSOBJ";
static   char  szTitleBar[] = "THREADS - Chapter 6 Sample";
static   char  szHelpTitleBar[] = "Help for THREADS";
static   char  szHelpLibraryName[] = "threads.hlp";

// --------------------------------------------------------------------------
// Client and Dialog window procedures
// --------------------------------------------------------------------------
MRESULT APIENTRY AppWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT APIENTRY ProgDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT APIENTRY ObjWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------
// Message processing procedures
// --------------------------------------------------------------------------
static MRESULT umDone( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT umReady( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT umSetObj( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmCommand( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmDestroy( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmPaint( HWND hwnd, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------
// Thread initialization procedures
// --------------------------------------------------------------------------
static void InitThread( void *param );
static void InitObjWnd( void *param );

// --------------------------------------------------------------------------
// Thread deletion procedure
// --------------------------------------------------------------------------
static void DeleteFileInThread( void *param );

// --------------------------------------------------------------------------
// Miscellaneous procedures
// --------------------------------------------------------------------------
static BOOL GetFileName( HWND hwnd, PSZ pszFileName );
static void InitializeFile( HWND hwnd, PSZ pszFileName );
static void InitFileInLoop( HWND hwnd, PSZ pszFileName );
static void InitFileInThread( HWND hwnd, PSZ pszFileName );
static void InitFileInObjWnd( HWND hwnd, PSZ pszFileName );

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
   HAB   hab = (HAB)NULLHANDLE;
   HMQ   hmq = (HMQ)NULLHANDLE;
   QMSG  qmsg;
   HWND  hwndFrame = (HWND)NULLHANDLE;
   HWND  hwndClient = (HWND)NULLHANDLE;
   HWND  hwndHelp = (HWND)NULLHANDLE;
   ULONG flCreateFlags = FCF_STANDARD & ~FCF_ACCELTABLE;
   HELPINIT    hi;

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
// Now register the window class for the main application window
// --------------------------------------------------------------------------
      if (!WinRegisterClass( hab,
                             szClassName,
                             AppWndProc,
                             CS_SYNCPAINT | CS_SIZEREDRAW | CS_CLIPCHILDREN,
                             APP_WINDOW_DATA_SIZE) ) {
         // failed!
         DisplayMessages(NULLHANDLE, "Unable to Register Window Class",
                         MSG_ERROR);
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Now create the application's main frame window
// --------------------------------------------------------------------------
      flCreateFlags &= ~FCF_ACCELTABLE;
      hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                      0L,
                                      &flCreateFlags,
                                      szClassName,
                                      szTitleBar,
                                      WS_VISIBLE,
                                      NULLHANDLE,
                                      APPLICATION_ID,
                                      &hwndClient );
      if (hwndFrame == NULLHANDLE) {
         // no window - gotta leave.
         DisplayMessages(NULLHANDLE, "Error Creating Frame Window",
                         MSG_ERROR);
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Create a help instance for the application
// --------------------------------------------------------------------------
      memset( (PVOID)&hi, 0, sizeof(HELPINIT));
      hi.cb = sizeof(HELPINIT);
      hi.phtHelpTable = (PHELPTABLE)MAKEULONG(APPLICATION_ID, 0xFFFF);
      hi.pszHelpWindowTitle = szHelpTitleBar;
      hi.pszHelpLibraryName = szHelpLibraryName;
      hwndHelp = WinCreateHelpInstance( hab, &hi );
      if ( hwndHelp == NULLHANDLE ) {
         // no window - gotta leave.
         DisplayMessages(NULLHANDLE, "Error Creating Help",
                         MSG_ERROR);
      } else {
// --------------------------------------------------------------------------
// Establish the link between the help instance and the application frame
// --------------------------------------------------------------------------
         WinAssociateHelpInstance( hwndHelp, hwndFrame );
      } /* endif */

// --------------------------------------------------------------------------
// All set, display the window and get started
// --------------------------------------------------------------------------
      WinShowWindow( hwndFrame, TRUE );

// --------------------------------------------------------------------------
// NOTE:  This is our standard PM message loop.  The purpose of this loop
//        is to get and dispatch messages from the application message queue
//        until WinGetMsg returns FALSE, indicating a WM_QUIT message.  In
//        which case we will pop out of the loop and destroy our frame window
//        along with the application message queue then we will terminate our
//        anchor block removing our ability to call the Presentation Manager.
// --------------------------------------------------------------------------
      while (WinGetMsg( hab, &qmsg, NULLHANDLE, 0UL, 0UL)) {
         WinDispatchMsg( hab, &qmsg );
      } /* endwhile */

   } while ( false ); /* enddo */

// --------------------------------------------------------------------------
// Kill the help instance if created
// --------------------------------------------------------------------------
   if ( hwndHelp != NULLHANDLE ) {
      WinAssociateHelpInstance( hwndHelp, NULLHANDLE );
      WinDestroyHelpInstance( hwndHelp );
   } /* endif */

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
// FUNCTION:     AppWndProc
//
// FUNCTION USE: The window procedure for the primary application client window.
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
MRESULT APIENTRY AppWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   switch (msg) {
   case UM_DONE:              return umDone( hwnd, mp1, mp2 );
   case UM_READY:             return umReady( hwnd, mp1, mp2 );
   case UM_SETOBJ:            return umSetObj( hwnd, mp1, mp2 );
   case WM_COMMAND:           return wmCommand( hwnd, mp1, mp2 );
   case WM_CREATE:            return wmCreate( hwnd, mp1, mp2 );
   case WM_DESTROY:           return wmDestroy( hwnd, mp1, mp2 );
   case WM_PAINT:             return wmPaint( hwnd, mp1, mp2 );
   default:                   return WinDefWindowProc( hwnd, msg, mp1, mp2 );
   } /* endswitch */
}

// *******************************************************************************
// FUNCTION:     umDone
//
// FUNCTION USE: Processes the UM_DONE message for the application.
//
// DESCRIPTION:  The UM_DONE message is sent from the secondary thread routine
//               in the method whichs uses a second thread but no object window.
//               When the message is received, the operation is terminated and
//               the OK button in the progress indicator dialog is enabled.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   handle of the progress indicator slider.
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT umDone( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND  hwndProgress = (HWND)mp1;

// --------------------------------------------------------------------------
// Get the handle of the dialog associated with the progress indicator slider
// --------------------------------------------------------------------------
   HWND  hwndDialog = WinQueryWindow( hwndProgress, QW_PARENT );

// --------------------------------------------------------------------------
// Enable the OK button in the dialog
// --------------------------------------------------------------------------
   WinEnableWindow( WinWindowFromID( hwndDialog, DID_OK ), TRUE );

// --------------------------------------------------------------------------
// return to caller with the reserved value.
// --------------------------------------------------------------------------
   return (MRESULT)0L;

}

// *******************************************************************************
// FUNCTION:     umReady
//
// FUNCTION USE: Processes the UM_READY message for the application.
//
// DESCRIPTION:  The UM_READY message is sent by the object window procedure in
//               the method whichs uses a second thread with an object window.
//               The message indicates that a single iteration of the file
//               operation has been completed.  If additional iterations are
//               required, a message is sent back to the object window with
//               the necessary data to perform the next iteration.  If no
//               further processing is required, no message is sent, the
//               operation is terminated and the OK button in the progress
//               indicator dialog window is enabled.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   pointer to an OBJPARM structure
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT umReady( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   POBJPARM    pObj = (POBJPARM)mp1;
// --------------------------------------------------------------------------
// Get the object window handle from the window instance data.
// --------------------------------------------------------------------------
   HWND        hwndObject = WinQueryWindowULong( hwnd, APP_HWNDOBJ );

   if (LONGFROMMP(mp2) == READY_OK &&
       pObj->ulRecord < MAX_RECORD) {

// --------------------------------------------------------------------------
// If the iteration was completed successfull and there are still more
// iterations required to complete the operation, first increment the
// current record number.
// --------------------------------------------------------------------------
      pObj->ulRecord++;

// --------------------------------------------------------------------------
// update the position of the progress indicator slider
// --------------------------------------------------------------------------
      WinSendDlgItemMsg( pObj->hwndProgress, DID_PROGRESS, SLM_SETSLIDERINFO,
                         MPFROM2SHORT(SMA_SLIDERARMPOSITION,
                                      SMA_INCREMENTVALUE ),
                         MPFROMLONG( (pObj->ulRecord * 100) /
                                     (MAX_RECORD - 1)));

// --------------------------------------------------------------------------
// Start the next iteration of the operation.  Application processing
// required before the next iteration should be inserted here.
// --------------------------------------------------------------------------
      if (hwndObject != NULLHANDLE ) {
         WinPostMsg( hwndObject, UM_WRITE, mp1, MPFROMLONG(0L));
      } else {

// --------------------------------------------------------------------------
// The object window disappeared somehow - just free up the structures and
// stop the operation.
// --------------------------------------------------------------------------
         free(pObj);
      } /* endif */

   } else {

// --------------------------------------------------------------------------
// error or done - enable the OK button in progress dialog
// --------------------------------------------------------------------------
      WinEnableWindow( WinWindowFromID( pObj->hwndProgress, DID_OK ), TRUE );

      if (hwndObject != NULLHANDLE ) {
// --------------------------------------------------------------------------
// Inform the object window that the operation is complete so that it
// can free up any resources it might have allocated.  Note that the
// object window frees the control structure
// --------------------------------------------------------------------------
         WinPostMsg( hwndObject, UM_CLOSE, MPFROMP(pObj), MPFROMLONG(0L));
      } else {
// --------------------------------------------------------------------------
// object window gone - free the control structure
// --------------------------------------------------------------------------
         free(pObj);
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// return to caller with the reserved value.
// --------------------------------------------------------------------------
   return (MRESULT)0L;
}

// *******************************************************************************
// FUNCTION:     umSetObj
//
// FUNCTION USE: Processes the UM_SETOBJ message for the application.
//
// DESCRIPTION:  The UM_SETOBJ message is posted by the secondary thread in
//               the method which uses an object window in a secondary thread
//               to inform the primary thread of the window handle for the
//               object window.  The primary thread stores this information
//               in its window instance data for later use in sending
//               messages to the object window.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   window handle of the object window
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT umSetObj( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
// --------------------------------------------------------------------------
// Save the object window handle in the private instance data
// --------------------------------------------------------------------------
   WinSetWindowULong( hwnd, APP_HWNDOBJ, (ULONG)mp1 );

// --------------------------------------------------------------------------
// return to caller with the reserved value.
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wmCommand
//
// FUNCTION USE: Processes the WM_COMMAND message for the application.
//
// DESCRIPTION:  Implements the menu item functionality for the application.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   command to execute in low-order 16 bits.
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wmCommand( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND  hwndFrame;
   HWND  hwndMenu;
   HWND  hwndHelp;
   HWND  hwndObj;
   char  szFileName[CCHMAXPATH];
   PSZ   pszFileName;

// --------------------------------------------------------------------------
// Get the window handles of the frame window and the menu bar window
// --------------------------------------------------------------------------
   hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
   hwndMenu = WinWindowFromID( hwndFrame, FID_MENU );

// --------------------------------------------------------------------------
// branch based on the command received
// --------------------------------------------------------------------------
   switch (SHORT1FROMMP(mp1)) {

   case MID_FILEINITIALIZE:
// --------------------------------------------------------------------------
// INITIALIZE FILE - This routine obtains the name of the file which the
//                   user wishes to initialize and then calls the
//                   appropriate initialization routine based on the mode
//                   which has been selected.
// --------------------------------------------------------------------------
      if (GetFileName( hwnd, szFileName )) {
         if (WinIsMenuItemChecked( hwndMenu, MID_MODESINGLE)) {
// --------------------------------------------------------------------------
// The user selected basic, single thread initialization
// --------------------------------------------------------------------------
            InitializeFile( hwnd, szFileName );

         } else if (WinIsMenuItemChecked( hwndMenu, MID_MODEPEEK)) {
// --------------------------------------------------------------------------
// The user selected single thread initialization with a peek loop.
// --------------------------------------------------------------------------
            InitFileInLoop( hwnd, szFileName );

         } else if (WinIsMenuItemChecked( hwndMenu, MID_MODEMULTI)) {
// --------------------------------------------------------------------------
// The user selected multiple thread initialization without an object win.
// --------------------------------------------------------------------------
            InitFileInThread( hwnd, szFileName );

         } else if (WinIsMenuItemChecked( hwndMenu, MID_MODEOBJ)) {
// --------------------------------------------------------------------------
// The user selected multiple thread initialization with an object window.
// --------------------------------------------------------------------------
            InitFileInObjWnd( hwnd, szFileName );
         } /* endif */
      } /* endif  */
      break;

   case MID_FILEDELETE:
// --------------------------------------------------------------------------
// DELETE FILE - This routine obtains the name of the file to delete and
//               then calls the appropriate file deletion routine based on
//               the mode selected.
// --------------------------------------------------------------------------
      if (GetFileName( hwnd, szFileName )) {
         if (WinIsMenuItemChecked( hwndMenu, MID_MODESINGLE)) {

// --------------------------------------------------------------------------
// The user selected single thread deletion
// --------------------------------------------------------------------------
            unlink( szFileName );

         } else if (WinIsMenuItemChecked( hwndMenu, MID_MODEPEEK)) {
// --------------------------------------------------------------------------
// The user selected single thread deletion with a peek loop -
// Note: since the deletion operation is an integral operation, we
//       can't peek for messages.
// --------------------------------------------------------------------------
            unlink( szFileName );

         } else if (WinIsMenuItemChecked( hwndMenu, MID_MODEMULTI)) {
// --------------------------------------------------------------------------
// The user selected multiple thread deletion without an object win.
// --------------------------------------------------------------------------
            pszFileName = (PSZ)malloc( strlen(szFileName) + 1 );
            if (pszFileName != NULL) {
               strcpy( pszFileName, szFileName );
               _beginthread( DeleteFileInThread, 0, 0x2000, pszFileName );
            } /* endif */

         } else if (WinIsMenuItemChecked( hwndMenu, MID_MODEOBJ)) {
// --------------------------------------------------------------------------
// The user selected multiple thread initialization with an object window.
// --------------------------------------------------------------------------
            hwndObj = WinQueryWindowULong( hwnd, APP_HWNDOBJ );
            if( hwndObj != NULLHANDLE ) {
               pszFileName = (PSZ)malloc( strlen(szFileName) + 1 );
               if (pszFileName != NULL) {
                  strcpy( pszFileName, szFileName );
                  WinPostMsg( hwndObj, UM_DELETE, MPFROMP(pszFileName),
                              MPFROMLONG(0l));
               } /* endif */
            } /* endif */
         } /* endif */
      } /* endif */
      break;

   case MID_FILEEXIT:
// --------------------------------------------------------------------------
// EXIT - Terminate the program by sending a WM_CLOSE message.
// --------------------------------------------------------------------------
      WinSendMsg( hwnd, WM_CLOSE, MPFROMLONG(0L), MPFROMLONG(0L));
      break;

   case MID_MODESINGLE:
// --------------------------------------------------------------------------
// SINGLE THREAD - The single thread, no peek loop, method has been chosen.
//                 Check this menu item and uncheck all others.
// --------------------------------------------------------------------------
      WinCheckMenuItem( hwndMenu, MID_MODESINGLE, TRUE );
      WinCheckMenuItem( hwndMenu, MID_MODEPEEK, FALSE );
      WinCheckMenuItem( hwndMenu, MID_MODEMULTI, FALSE );
      WinCheckMenuItem( hwndMenu, MID_MODEOBJ, FALSE );
      break;

   case MID_MODEPEEK:
// --------------------------------------------------------------------------
// PEEK LOOP - The single thread with peek loop method has been chosen.
//             Check this menu item and uncheck all others.
// --------------------------------------------------------------------------
      WinCheckMenuItem( hwndMenu, MID_MODESINGLE, FALSE );
      WinCheckMenuItem( hwndMenu, MID_MODEPEEK, TRUE );
      WinCheckMenuItem( hwndMenu, MID_MODEMULTI, FALSE );
      WinCheckMenuItem( hwndMenu, MID_MODEOBJ, FALSE );
      break;

   case MID_MODEMULTI:
// --------------------------------------------------------------------------
// MULTITHREAD - The multiple thread no object window method has been chosen.
//               Check this menu item and uncheck all others.
// --------------------------------------------------------------------------
      WinCheckMenuItem( hwndMenu, MID_MODESINGLE, FALSE );
      WinCheckMenuItem( hwndMenu, MID_MODEPEEK, FALSE );
      WinCheckMenuItem( hwndMenu, MID_MODEMULTI, TRUE );
      WinCheckMenuItem( hwndMenu, MID_MODEOBJ, FALSE );
      break;

   case MID_MODEOBJ:
// --------------------------------------------------------------------------
// OBJECT WINDOW - The multiple thread with object window method has been
//                 chosen.  Check this menu item and uncheck all others.
// --------------------------------------------------------------------------
      WinCheckMenuItem( hwndMenu, MID_MODESINGLE, FALSE );
      WinCheckMenuItem( hwndMenu, MID_MODEPEEK, FALSE );
      WinCheckMenuItem( hwndMenu, MID_MODEMULTI, FALSE );
      WinCheckMenuItem( hwndMenu, MID_MODEOBJ, TRUE );
      break;

   case MID_HELPGENERAL:
// --------------------------------------------------------------------------
// Display the general help panel for the application
// --------------------------------------------------------------------------
      hwndHelp = WinQueryHelpInstance( hwnd );
      if (hwndHelp != NULLHANDLE) {

// --------------------------------------------------------------------------
// Send HM_GENERAL_HELP to the help instance
// --------------------------------------------------------------------------
         WinSendMsg( hwndHelp, HM_GENERAL_HELP, NULL, NULL );
      } /* endif */
      break;

   case MID_HELPUSING:
// --------------------------------------------------------------------------
// Display the Using Help Panel
// --------------------------------------------------------------------------
   hwndHelp = WinQueryHelpInstance( hwndFrame );
      if (hwndHelp != NULLHANDLE) {

// --------------------------------------------------------------------------
// Send HM_DISPLAY_HELP to the help instance
// --------------------------------------------------------------------------
         WinSendMsg( hwndHelp, HM_DISPLAY_HELP,
                     MPFROMLONG(0L), MPFROMLONG(0L));
      } /* endif */
      break;

   case MID_HELPPRODINFO:
// --------------------------------------------------------------------------
// Display the product information dialog
// --------------------------------------------------------------------------
      WinDlgBox(HWND_DESKTOP, hwnd, WinDefDlgProc, NULLHANDLE, ID_PRODINFO, NULL); 
      break;

   } /* endswitch */


// --------------------------------------------------------------------------
// return to caller with the reserved value.
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wmCreate
//
// FUNCTION USE: Processes the WM_CREATE message for the application.
//
// DESCRIPTION:  Handles the instance initialization for new windows.  In this
//               case, the thread for the multithread with object window is
//               started.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  FALSE to continue creation, TRUE to abort creation.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
// --------------------------------------------------------------------------
// start the secondary thread
// --------------------------------------------------------------------------
   _beginthread( InitObjWnd, 0, 0x2000, (PVOID)hwnd );

// --------------------------------------------------------------------------
// return - allowing window creation to continue.
// --------------------------------------------------------------------------
   return MRFROMLONG(FALSE);
}

// *******************************************************************************
// FUNCTION:     wmDestroy
//
// FUNCTION USE: Processes the WM_DESTROY message for the application.
//
// DESCRIPTION:  Handles instance destruction for the primary window, allowing
//               cleanup of any allocated resources.  In this case, the
//               object window in the secondary thread is destroyed.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wmDestroy( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND  hwndObj;

// --------------------------------------------------------------------------
// Get the object window handle from the instance data, clear the handle
// from the instance data, and destory the object window.
// --------------------------------------------------------------------------
   hwndObj = (HWND)WinQueryWindowULong( hwnd, APP_HWNDOBJ );
   if (hwndObj != NULLHANDLE ) {
      WinSetWindowULong( hwnd, APP_HWNDOBJ, (ULONG)NULLHANDLE );
      WinDestroyWindow( hwndObj );
//      WinSendMsg( hwndObj, WM_QUIT, MPFROMLONG(0L), MPFROMLONG(0L));
   } /* endif */

// --------------------------------------------------------------------------
// return to caller with the reserved value.
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wmPaint
//
// FUNCTION USE: Processes the WM_PAINT message for the application.
//
// DESCRIPTION:  Nothing to output, just clear the background.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wmPaint( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HPS      hps;
   RECTL    rectl;

// --------------------------------------------------------------------------
// Get the presentation space handle and the area to be painted.
// --------------------------------------------------------------------------
   hps = WinBeginPaint( hwnd, NULLHANDLE, &rectl );
   if (hps != NULLHANDLE) {

// --------------------------------------------------------------------------
// Clear the area specified.
// --------------------------------------------------------------------------
      WinFillRect( hps, &rectl, CLR_BACKGROUND );

// --------------------------------------------------------------------------
// release the presentation space
// --------------------------------------------------------------------------
      WinEndPaint( hps );
   } /* endif */

// --------------------------------------------------------------------------
// return to caller with the reserved value.
// --------------------------------------------------------------------------
   return (MRESULT)0L;
}

// *******************************************************************************
// FUNCTION:     GetFileName
//
// FUNCTION USE: Obtain the name of the file to be initialized or deleted.
//
// DESCRIPTION:  Uses the standard file dialog to allow user selection of a
//               file name
//
// PARAMETERS:   HWND     client window handle
//               PSZ      pointer to location to store the file name.  This
//                        buffer must be large enough to hold the maximum
//                        allowable file name.
//
// RETURNS:      BOOL     TRUE if file selected, FALSE if nothing selected.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static BOOL GetFileName( HWND hwnd, PSZ pszFileName )
{
   FILEDLG     fd;

// --------------------------------------------------------------------------
// First clear the standard file dialog control structure and then
// initialize the structure size element (cb)
// --------------------------------------------------------------------------
   memset( (PVOID)&fd, 0, sizeof(FILEDLG));
   fd.cbSize = sizeof(FILEDLG);

// --------------------------------------------------------------------------
// Process as an "OPEN FILE" dialog, allowing the user to select a file from
// the list of files in the dialog.  Since we are not actually opening a file
// a custom title is specified.
// --------------------------------------------------------------------------
   fd.fl = FDS_OPEN_DIALOG;
   fd.pszTitle = "Select File";

// --------------------------------------------------------------------------
// The dialog initially displays a list of all files available.
// --------------------------------------------------------------------------
   /* Initially display all files */
   strcpy(fd.szFullFile, "*" );
//   fd.usDlgId = SELECTFILE_ID;

// --------------------------------------------------------------------------
// Raise the dialog and get the user response
// --------------------------------------------------------------------------
   /* Get the user's selection */
   if ((BOOL)(WinFileDlg( HWND_DESKTOP, hwnd, &fd )) &&
       fd.lReturn == DID_OK ) {
// --------------------------------------------------------------------------
// The user either selected or specified a file - copy back to the caller.
// --------------------------------------------------------------------------
      strcpy( pszFileName, fd.szFullFile );

   } else {
// --------------------------------------------------------------------------
// no file name was specified - clear out the caller's file name buffer.
// --------------------------------------------------------------------------
      strcpy( pszFileName, "" );
   } /* endif */

// --------------------------------------------------------------------------
// return - indicating if the buffer was filled with a file name
// --------------------------------------------------------------------------
   return (strlen(pszFileName) != 0 );
}

// *******************************************************************************
// FUNCTION:     InitializeFile
//
// FUNCTION USE: Build an empty file.
//
// DESCRIPTION:  This routine initializes a file in the primary thread without
//               using a peek loop or any other method of preventing the
//               application from blocking the user's input to the computer.
//
// PARAMETERS:   HWND     client window handle
//               PSZ      pointer to the name of the file to initialize
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void InitializeFile( HWND hwnd, PSZ pszFileName )
{
   FILE  *file;
   HWND  hwndProgress;
   char  buffer[ RECORD_SIZE ];
   ULONG record;
   ULONG i;

// --------------------------------------------------------------------------
// Attempt to open the requested file, if the file cannot be opened, then
// no further initialization processing is performed.
// --------------------------------------------------------------------------
   file = fopen( pszFileName, "w" );
   if (file != (FILE *)NULL) {

// --------------------------------------------------------------------------
// Got the file open - raise a modeless dialog box containing a slider which
// acts as a progress indicator and initialize the title of the dialog to
// indicate the name of the file being initialized.
// --------------------------------------------------------------------------
      hwndProgress = WinLoadDlg( HWND_DESKTOP, hwnd, ProgDlgProc, NULLHANDLE,
                                 PROGRESS_ID, NULL );
      WinSetWindowText( WinWindowFromID( hwndProgress, DID_FILENAME ),
                        pszFileName );

// --------------------------------------------------------------------------
// Initially disable the OK button in the dialog.  This button will be
// enabled when the operation is complete as a further indication of the
// status of the operation.  When done, make the dialog visible.
// --------------------------------------------------------------------------
      WinEnableWindow( WinWindowFromID( hwndProgress, DID_OK ), FALSE );
      WinShowWindow( hwndProgress, TRUE );

// --------------------------------------------------------------------------
// Now we're ready to initialize the file.  To enhance the effect for demo
// purposes, the file is initialized in two loops, one for each record in the
// the file and one for each byte in the file (initilizing a record at a
// time would be much more effective).
// --------------------------------------------------------------------------
      memset( (PVOID)buffer, 0, RECORD_SIZE );
      for ( record = 0; record < MAX_RECORD; record++ ) {
         for (i = 0; i < RECORD_SIZE; i++ )
            fwrite( buffer, 1, 1, file );

// --------------------------------------------------------------------------
// Update the progress indicator after each record is complete.
// --------------------------------------------------------------------------
         WinSendDlgItemMsg( hwndProgress, DID_PROGRESS, SLM_SETSLIDERINFO,
                            MPFROM2SHORT(SMA_SLIDERARMPOSITION,
                                         SMA_INCREMENTVALUE ),
                            MPFROMLONG( (record * 100) / (MAX_RECORD - 1)));
      } /* endfor */

// --------------------------------------------------------------------------
// All done - close the file
// --------------------------------------------------------------------------
      fclose( file );

// --------------------------------------------------------------------------
// Now enable the OK button in the dialog containing the progress indicator,
// the user can terminate the dialog when convenient.
// --------------------------------------------------------------------------
      WinEnableWindow( WinWindowFromID( hwndProgress, DID_OK ), TRUE );

   } else {
// --------------------------------------------------------------------------
// The file could not be opened - notify the user.
// --------------------------------------------------------------------------
      DisplayMessages(NULLHANDLE, "Unable to access file",
                      MSG_ERROR);
   } /* endif */
}

// *******************************************************************************
// FUNCTION:     InitFileInLoop
//
// FUNCTION USE: Build an empty file.
//
// DESCRIPTION:  This routine initializes a file in the primary thread but
//               uses a peek loop to help reduce the operation's negative
//               impact on the user's ability to input to the computer.
//
// PARAMETERS:   HWND     client window handle
//               PSZ      pointer to the name of the file to initialize
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void InitFileInLoop( HWND hwnd, PSZ pszFileName )
{
   FILE  *file;
   ULONG record;
   char  buffer = '\0';
   int   i;
   HWND  hwndProgress;
   HAB   hab = WinQueryAnchorBlock( hwnd );
   QMSG  qmsg;

// --------------------------------------------------------------------------
// Attempt to open the requested file, if the file cannot be opened, then
// no further initialization processing is performed.
// --------------------------------------------------------------------------
   file = fopen( pszFileName, "w" );
   if (file != (FILE *)NULL) {

// --------------------------------------------------------------------------
// Got the file open - raise a modeless dialog box containing a slider which
// acts as a progress indicator and initialize the title of the dialog to
// indicate the name of the file being initialized.
// --------------------------------------------------------------------------
      hwndProgress = WinLoadDlg( HWND_DESKTOP, hwnd, ProgDlgProc, NULLHANDLE,
                                 PROGRESS_ID, NULL );
      WinSetWindowText( WinWindowFromID( hwndProgress, DID_FILENAME ),
                        pszFileName );

// --------------------------------------------------------------------------
// Initially disable the OK button in the dialog.  This button will be
// enabled when the operation is complete as a further indication of the
// status of the operation.  When done, make the dialog visible.
// --------------------------------------------------------------------------
      WinEnableWindow( WinWindowFromID( hwndProgress, DID_OK ), FALSE );
      WinShowWindow( hwndProgress, TRUE );

// --------------------------------------------------------------------------
// Now we're ready to initialize the file.  To enhance the effect for demo
// purposes, the file is initialized in two loops, one for each record in the
// the file and one for each byte in the file (initilizing a record at a
// time would be much more effective).
// --------------------------------------------------------------------------
      for ( record = 0; record < MAX_RECORD; record++ ) {
         for (i = 0; i < RECORD_SIZE; i++ )
            fwrite( &buffer, 1, 1, file );

// --------------------------------------------------------------------------
// Update the progress indicator after each record is complete.
// --------------------------------------------------------------------------
         WinSendDlgItemMsg( hwndProgress, DID_PROGRESS, SLM_SETSLIDERINFO,
                            MPFROM2SHORT(SMA_SLIDERARMPOSITION,
                                         SMA_INCREMENTVALUE ),
                         MPFROMLONG( (record * 100) / (MAX_RECORD - 1)));

// --------------------------------------------------------------------------
// HERE'S THE DIFFERENCE: Each time a record is completed, take a peek at the
//                        message queue.  If the queue has a message, process
//                        the message.
// --------------------------------------------------------------------------
         if (WinPeekMsg(hab, &qmsg, NULLHANDLE, 0L, 0L, PM_REMOVE )) {
            WinDispatchMsg( hab, &qmsg );
         } /* endif */

      } /* endfor */

// --------------------------------------------------------------------------
// All done - close the file
// --------------------------------------------------------------------------
      fclose( file );

// --------------------------------------------------------------------------
// Now enable the OK button in the dialog containing the progress indicator,
// the user can terminate the dialog when convenient.
// --------------------------------------------------------------------------
      WinEnableWindow( WinWindowFromID( hwndProgress, DID_OK ), TRUE );

   } else {
// --------------------------------------------------------------------------
// The file could not be opened - notify the user.
// --------------------------------------------------------------------------
      DisplayMessages(NULLHANDLE, "Unable to access file",
                      MSG_ERROR);
   } /* endif */
}

// *******************************************************************************
// FUNCTION:     InitFileInThread
//
// FUNCTION USE: Build an empty file.
//
// DESCRIPTION:  This routine initializes a file in a secondary thread without
//               the use of an object window.  This method does not prevent
//               the user from inputting to the computer, but also does not
//               allow the primary thread to participate in the operation, thus
//               all data required to complete the operation must be provided
//               when the operation is initiated.
//
// PARAMETERS:   HWND     client window handle
//               PSZ      pointer to the name of the file to initialize
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void InitFileInThread( HWND hwnd, PSZ pszFileName )
{
   HWND     hwndProgress;
   PTPARM   ptp;

// --------------------------------------------------------------------------
// Allocate memory for the structure used to pass information to the
// secondary thread - stop if the memory is not available.
// --------------------------------------------------------------------------
   ptp = (PTPARM)malloc( sizeof(TPARM));
   if (ptp != (PTPARM)NULL ) {

// --------------------------------------------------------------------------
// Got the memory - raise a modeless dialog box containing a slider which
// acts as a progress indicator and initialize the title of the dialog to
// indicate the name of the file being initialized.
// --------------------------------------------------------------------------
      hwndProgress = WinLoadDlg( HWND_DESKTOP, hwnd, ProgDlgProc,
                                      NULLHANDLE, PROGRESS_ID, NULL );
      WinSetWindowText( WinWindowFromID( hwndProgress, DID_FILENAME ),
                        pszFileName );

// --------------------------------------------------------------------------
// Initially disable the OK button in the dialog.  This button will be
// enabled when the operation is complete as a further indication of the
// status of the operation.  When done, make the dialog visible.
// --------------------------------------------------------------------------
      WinEnableWindow( WinWindowFromID( hwndProgress, DID_OK ), FALSE );
      WinShowWindow( hwndProgress, TRUE );

// --------------------------------------------------------------------------
// now fill in the parameter structure for the secondary thread.  We pass
// over the handle to the progress indicator slider, the application window
// (for the complete message), and the name of the file.
// --------------------------------------------------------------------------
      ptp->hwndSlider = WinWindowFromID( hwndProgress, DID_PROGRESS );
      ptp->hwnd = hwnd;
      strcpy( ptp->szFile, pszFileName );

// --------------------------------------------------------------------------
// now kick off the new thread - passing over the parameter structure.
// The secondary thread opens the file and executes the initialization
// loop used in the single thread methods, updating the progress
// indicator as each record is completed.  When all records have been
// processed, the thread posts a UM_DONE message back to this client
// window which then performs any needed cleanup -- in this instance,
// enabling the OK button in the progress indicator dialog.
// --------------------------------------------------------------------------
      _beginthread( InitThread, 0, 0x2000, (PVOID)ptp );

   } else {
// --------------------------------------------------------------------------
// The parameter structure memory was not available - notify the user
// --------------------------------------------------------------------------
      DisplayMessages(NULLHANDLE, "Memory allocation error",
                      MSG_ERROR);
   } /* endif */
}

// *******************************************************************************
// FUNCTION:     InitFileInObjWnd
//
// FUNCTION USE: Build an empty file.
//
// DESCRIPTION:  This routine initializes a file in a secondary thread using
//               an object window.  This method does not prevent the user from
//               inputting to the computer and DOES allow the primary thread
//               to participate in the operation.
//
// PARAMETERS:   HWND     client window handle
//               PSZ      pointer to the name of the file to initialize
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void InitFileInObjWnd( HWND hwnd, PSZ pszFileName )
{
   HWND     hwndProgress;
   POBJPARM objp;
   HWND     hwndObj = WinQueryWindowULong( hwnd, APP_HWNDOBJ );

// --------------------------------------------------------------------------
// Allocate memory for the operation control structure used to communicate
// between the primary and secondary threads (or the client window and the
// object window).  Stop if the memory cannot be allocated.
// --------------------------------------------------------------------------
   objp = (POBJPARM)malloc( sizeof(OBJPARM));
   if (objp != (POBJPARM)NULL ) {

// --------------------------------------------------------------------------
// Got the memory - raise a modeless dialog box containing a slider which
// acts as a progress indicator and initialize the title of the dialog to
// indicate the name of the file being initialized.
// --------------------------------------------------------------------------
      hwndProgress = WinLoadDlg( HWND_DESKTOP, hwnd, ProgDlgProc,
                                       NULLHANDLE, PROGRESS_ID, NULL );
      WinSetWindowText( WinWindowFromID(hwndProgress, DID_FILENAME ),
                        pszFileName );

// --------------------------------------------------------------------------
// Initially disable the OK button in the dialog.  This button will be
// enabled when the operation is complete as a further indication of the
// status of the operation.  When done, make the dialog visible.
// --------------------------------------------------------------------------
      WinEnableWindow( WinWindowFromID( hwndProgress, DID_OK ), FALSE );
      WinShowWindow( hwndProgress, TRUE );

// --------------------------------------------------------------------------
// now fill in the parameter structure to be passed to the object window.
// This structure contains the handle of the progress indicator dialog,
// the client window handle, the name of the file, the current record
// number, the size of the record, and the buffer of data to write to the
// record.
// --------------------------------------------------------------------------
      objp->hwndProgress = hwndProgress;
      objp->hwnd = hwnd;
      strcpy( objp->szFileName, pszFileName );
      objp->ulRecord = 0;
      objp->cbRecord = RECORD_SIZE;
      memset( objp->record, 0, RECORD_SIZE );

// --------------------------------------------------------------------------
// Now send the UM_INIT message to the object window to begin the
// file initialization operation.  The object window will open the file
// and use the inner loop from the single thread methods to initialize
// the first record.  A UM_READY message is then posted back to the client
// window which update the progress indicator as necessary, and, if more
// records are required, prepares the buffer for the next record.
// A UM_WRITE message is then sent back to the object window.  This cycle
// continues until all records are written at which time the client window
// enables the OK button in the progress indicator dialog and post a
// UM_CLOSE message to the object window to allow it to clean up the
// operation.
// --------------------------------------------------------------------------
      WinPostMsg( hwndObj, UM_INIT, MPFROMP(objp), MPFROMLONG(0L));

   } else {
// --------------------------------------------------------------------------
// The parameter structure memory was not available - notify the user
// --------------------------------------------------------------------------
      DisplayMessages(NULLHANDLE, "Memory allocation error",
                      MSG_ERROR);
   } /* endif */
}

// *******************************************************************************
// FUNCTION:     ProgDlgProc
//
// FUNCTION USE: Handles initialization of the progress indicator dialog.
//
// DESCRIPTION:  This routine generically handles all messages sent to the dialog
//               window; however, all but the WM_INITDLG message are passed to
//               the default procedure.  The initialization message is processed
//               to properly annotate the slider for our purposes.
//
// PARAMETERS:   HWND     client window handle
//               ULONG    window message
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  dependent on the message received.
//
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT APIENTRY ProgDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   switch( msg ) {
   case WM_INITDLG:
// --------------------------------------------------------------------------
// The slider is created with 101 intervals from 0 - 100.  Elongate the tick
// marks for intervals 0, 25, 50, 75, and 100.
// --------------------------------------------------------------------------
      WinSendDlgItemMsg( hwnd, DID_PROGRESS, SLM_SETTICKSIZE,
                         MPFROM2SHORT(0,6), MPFROMLONG(0L));
      WinSendDlgItemMsg( hwnd, DID_PROGRESS, SLM_SETTICKSIZE,
                         MPFROM2SHORT(25,6), MPFROMLONG(0L));
      WinSendDlgItemMsg( hwnd, DID_PROGRESS, SLM_SETTICKSIZE,
                         MPFROM2SHORT(50,6), MPFROMLONG(0L));
      WinSendDlgItemMsg( hwnd, DID_PROGRESS, SLM_SETTICKSIZE,
                         MPFROM2SHORT(75,6), MPFROMLONG(0L));
      WinSendDlgItemMsg( hwnd, DID_PROGRESS, SLM_SETTICKSIZE,
                         MPFROM2SHORT(100,6), MPFROMLONG(0L));

// --------------------------------------------------------------------------
// Now annotate the intervals with the interval number.
// --------------------------------------------------------------------------
      WinSendDlgItemMsg( hwnd, DID_PROGRESS, SLM_SETSCALETEXT,
                         MPFROMSHORT(0), MPFROMP("0"));
      WinSendDlgItemMsg( hwnd, DID_PROGRESS, SLM_SETSCALETEXT,
                         MPFROMSHORT(25), MPFROMP("25"));
      WinSendDlgItemMsg( hwnd, DID_PROGRESS, SLM_SETSCALETEXT,
                         MPFROMSHORT(50), MPFROMP("50"));
      WinSendDlgItemMsg( hwnd, DID_PROGRESS, SLM_SETSCALETEXT,
                         MPFROMSHORT(75), MPFROMP("75"));
      WinSendDlgItemMsg( hwnd, DID_PROGRESS, SLM_SETSCALETEXT,
                         MPFROMSHORT(100), MPFROMP("100"));

// --------------------------------------------------------------------------
// No focus change - so return FALSE
// --------------------------------------------------------------------------
      return FALSE;

   default:
      return WinDefDlgProc( hwnd, msg, mp1, mp2 );
   }
}

// *******************************************************************************
// FUNCTION:     InitThread
//
// FUNCTION USE: Thread procedure which initializes a file without an object win.
//
// DESCRIPTION:  This function opens and initializes the file in the same
//               manner as the single thread method, updating the progress
//               indicator as each record is completed.  When the initialization
//               is complete, a message is sent back to the client window of the
//               application indicating the success of the operation.
//
// PARAMETERS:   void*    thread parameter - pointer to a PTPARM structure
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void InitThread( void *param ) {
   PTPARM   ptp = (PTPARM)param;
   HWND     hwndProgress = ptp->hwndSlider;
   FILE     *file;
   char     buffer = '\0';
   ULONG    record;
   ULONG    i;
   ULONG    status = INIT_ERROR;

// --------------------------------------------------------------------------
// NOTE: Since the only Win calls made by this thread are to WinPostMsg,
//       there is no need to connect to the PM environment or to create
//       a message queue.
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Attempt to open the requested file, if the file cannot be opened, then
// no further initialization processing is performed.
// --------------------------------------------------------------------------
   file = fopen( ptp->szFile, "w" );
   if (file != (FILE *)NULL) {

// --------------------------------------------------------------------------
// Now we're ready to initialize the file.  To enhance the effect for demo
// purposes, the file is initialized in two loops, one for each record in the
// the file and one for each byte in the file (initilizing a record at a
// time would be much more effective).
// --------------------------------------------------------------------------
      for ( record = 0; record < MAX_RECORD; record++ ) {
         for (i = 0; i < RECORD_SIZE; i++ ) {
            fwrite( &buffer, 1, 1, file );
         } /* endfor */

// --------------------------------------------------------------------------
// Update the progress indicator after each record is complete.
// --------------------------------------------------------------------------
         WinPostMsg( hwndProgress, SLM_SETSLIDERINFO,
                     MPFROM2SHORT(SMA_SLIDERARMPOSITION, SMA_INCREMENTVALUE ),
                     MPFROMLONG( (record * 100) / (MAX_RECORD - 1)));
      } /* endfor */

// --------------------------------------------------------------------------
// All done - close the file
// --------------------------------------------------------------------------
      fclose( file );

// --------------------------------------------------------------------------
// Post a final message to the progress indicator to force 100% completion.
// In this case, wait until the message is actually posted successfully.
// There was no harm in missing one of the previous messages.
// --------------------------------------------------------------------------
      while (!(BOOL)WinPostMsg( hwndProgress, SLM_SETSLIDERINFO,
                    MPFROM2SHORT(SMA_SLIDERARMPOSITION, SMA_INCREMENTVALUE ),
                    MPFROMLONG( (record * 100) / (MAX_RECORD - 1)))) continue;

// --------------------------------------------------------------------------
// Set the status to indicate a successful initialization.
// --------------------------------------------------------------------------
      status = INIT_COMPLETE;
   } /* endif */

// --------------------------------------------------------------------------
// Now post a UM_DONE message back to the application's client window to
// indicate the completion and status of the operation.
// --------------------------------------------------------------------------
   while( !(BOOL)WinPostMsg(
       ptp->hwnd,
       UM_DONE, MPFROMHWND(hwndProgress), MPFROMLONG(status))) continue;

// --------------------------------------------------------------------------
// clean up resources
// --------------------------------------------------------------------------
   free(param);
}

// *******************************************************************************
// FUNCTION:     DeleteFileInThread
//
// FUNCTION USE: Thread procedure for deleting a file.
//
// DESCRIPTION:  If a file name is given - delete the file.
//
// PARAMETERS:   void*    thread parameter - pointer to a filename
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void DeleteFileInThread( void *param ) {
   if ( param != NULL ) {
      unlink( param );
      free( param );
   } /* endif */
}

// *******************************************************************************
// FUNCTION:     InitObjWnd
//
// FUNCTION USE: Thread procedure which creates an object window to perform
//               file initialization procedures.
//
// DESCRIPTION:  This function is similar to the main function of the primary
//               thread.  It creates a PM environment and a message queue. An
//               object window is created and a message loop run to process
//               messages for the object window.
//
// PARAMETERS:   void*    thread parameter - not used
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void InitObjWnd( void *param ) {
   HAB   hab = (HAB)NULLHANDLE;
   HMQ   hmq = (HMQ)NULLHANDLE;
   QMSG  qmsg;
   HWND  hwndObject = (HWND)NULLHANDLE;
   HWND  hwndMain = (HWND)param;

// --------------------------------------------------------------------------
// Single iteration loop - makes error handling easier
// --------------------------------------------------------------------------
   do {

// --------------------------------------------------------------------------
// First things first - set up the PM environment for the thread
// --------------------------------------------------------------------------
      hab = WinInitialize( 0 );
      if (hab == (HAB)NULLHANDLE) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Next thing we need is a message queue
// --------------------------------------------------------------------------
      hmq = WinCreateMsgQueue( hab, 0 );
      if (hmq == (HMQ)NULLHANDLE) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Now register the window class for the object window
// --------------------------------------------------------------------------
      if (!WinRegisterClass( hab, szObjClassName, ObjWndProc,0L, 0L )) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Now create the object main frame window
// --------------------------------------------------------------------------
      hwndObject = WinCreateWindow( HWND_OBJECT,
                                    szObjClassName,
                                    NULL,
                                    0L,
                                    0L, 0L,
                                    0L, 0L,
                                    NULLHANDLE,
                                    HWND_TOP,
                                    0L, NULL, NULL );
      if (hwndObject == (HWND)NULLHANDLE) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Notify the application of the object window handle
// --------------------------------------------------------------------------
      WinPostMsg( hwndMain, UM_SETOBJ, MPFROMHWND(hwndObject), MPFROMLONG(0L));

// --------------------------------------------------------------------------
// NOTE:  This is our standard PM message loop.  The purpose of this loop
//        is to get and dispatch messages from the application message queue
//        until WinGetMsg returns FALSE, indicating a WM_QUIT message.  In
//        which case we will pop out of the loop and destroy our frame window
//        along with the application message queue then we will terminate our
//        anchor block removing our ability to call the Presentation Manager.
// --------------------------------------------------------------------------
      while (WinGetMsg(hab, &qmsg, NULLHANDLE, 0, 0 )) {
         WinDispatchMsg( hab, &qmsg );
      } /* endwhile */


   } while ( false ); /* enddo */

// --------------------------------------------------------------------------
// Kill the message queue if successfully created
// --------------------------------------------------------------------------
   if (hmq != (HMQ)NULLHANDLE) {
      WinDestroyMsgQueue( hmq );
   } /* endif */

// --------------------------------------------------------------------------
// Disconnect from PM if necessary
// --------------------------------------------------------------------------
   if (hab != (HAB)NULLHANDLE) {
      WinTerminate( hab );
   } /* endif */

}

// *******************************************************************************
// FUNCTION:     ObjWndProc
//
// FUNCTION USE: The window procedure for the object window.
//
// DESCRIPTION:  Handles message processing for the user defined messages which
//               are used to handle initialization of a file using the object
//               window in a separate thread method.
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
MRESULT APIENTRY ObjWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   POBJPARM    pObj;
   int         i;

   switch(msg) {

   case UM_INIT:

// --------------------------------------------------------------------------
// UM_INIT: begin processing of a file initialization operation.  Start by
//          retrieving the control structure from mp1
// --------------------------------------------------------------------------
      pObj = (POBJPARM)mp1;

// --------------------------------------------------------------------------
// Now attempt to open the file
// --------------------------------------------------------------------------
      pObj->file = fopen( pObj->szFileName, "w" );

      if (pObj->file != (FILE *)NULL) {

// --------------------------------------------------------------------------
// file opened successfully - send ourself a UM_WRITE message to initialize
// the first record.
// --------------------------------------------------------------------------
         WinPostMsg( hwnd, UM_WRITE, mp1, mp2 );

      } else {
// --------------------------------------------------------------------------
// error opening the file - send a UM_READY message back to the application
// client window with an error indicated.
// --------------------------------------------------------------------------
         WinPostMsg( pObj->hwnd, UM_READY, mp1, MPFROMLONG(READY_ERROR) );
      } /* endif */

// --------------------------------------------------------------------------
// Done - return reserved value of zero.
// --------------------------------------------------------------------------
      return 0L;

   case UM_WRITE:

// --------------------------------------------------------------------------
// UM_WRITE: perform the inner loop of the normal initialization procedure
//           to write the bytes of a single record to the file.  First
//           get the control structure pointer from mp1.
// --------------------------------------------------------------------------
      pObj = (POBJPARM)mp1;

// --------------------------------------------------------------------------
// write the record
// --------------------------------------------------------------------------
      for (i = 0; i < pObj->cbRecord; i++ )
         fwrite( &(pObj->record[i]), 1, 1, pObj->file );

// --------------------------------------------------------------------------
// post a UM_READY message back to the application client indicating that
// the record initialization was successful.
// --------------------------------------------------------------------------
      WinPostMsg( pObj->hwnd, UM_READY, mp1, MPFROMLONG(READY_OK));

// --------------------------------------------------------------------------
// Done - return reserved value of zero.
// --------------------------------------------------------------------------
      return 0L;

   case UM_CLOSE:

// --------------------------------------------------------------------------
// UM_CLOSE: is the notification from the main application client that the
//           initialization operation is complete.  After obtaining the
//           control structure pointer from mp1, close the file and release
//           the space allocated for the control structure.
// --------------------------------------------------------------------------
      pObj = (POBJPARM)mp1;
      fclose(pObj->file);
      free(pObj);

// --------------------------------------------------------------------------
// Done - return reserved value of zero
// --------------------------------------------------------------------------
      return 0L;

   case UM_DELETE:

// --------------------------------------------------------------------------
// UM_DELETE: Delete the file whose name is passed in parameter mp1
// --------------------------------------------------------------------------
      if ( mp1 != NULL ) {
         unlink( mp1 );
         free( mp1 );
      }

// --------------------------------------------------------------------------
// Done - return reserved value of zero
// --------------------------------------------------------------------------
      return 0L;

   default:
// --------------------------------------------------------------------------
// All other messages are passed to the default window procedure
// --------------------------------------------------------------------------
      return WinDefWindowProc( hwnd, msg, mp1, mp2 );
   }
}
