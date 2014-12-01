// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      HELPME.C                                         mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Main Source File for HELPME/Chapter 05 Sample Program         บ
// บ                                                                               บ
// บ  NOTES:         This program demonstartes the use of the OS/2 IPF facility.   บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#define  INCL_WINFRAMEMGR
#define  INCL_WINWINDOWMGR
#define  INCL_WINSTDFILE
#define  INCL_WINHELP
#define  INCL_DOSPROCESS
#include "helpme.h"
#include "shcommon.h"
#include <process.h>
#include <string.h>

// --------------------------------------------------------------------------
// Miscellaneous constant definitions used only in this source module.
// --------------------------------------------------------------------------
#define  TITLE_BAR_TEXT       "HELPME - Chapter 5 Sample Program"
#define  MT_TITLE_BAR_TEXT    "HELPME - Auxiliary Thread"

// --------------------------------------------------------------------------
// Define variables used to prevent warnings for loop constants
// --------------------------------------------------------------------------
static   BOOL  false = FALSE;

// --------------------------------------------------------------------------
// define class names
// --------------------------------------------------------------------------
static   char  szClassName[ ] = "HELPME";
static   char  szChildClassName[ ] = "HELPMECHILD";

// --------------------------------------------------------------------------
// Client Window Procedures
// --------------------------------------------------------------------------
MRESULT APIENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT APIENTRY ChildWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT APIENTRY StdFileDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------
// Message processing procedures
// --------------------------------------------------------------------------
MRESULT hm_querykeyshelp( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_activate( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_command( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_create( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_destroy( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_initmenu( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_menuend( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_paint( HWND hwnd, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------
// Miscellaneous procedures
// --------------------------------------------------------------------------
void _Optlink thread2( PVOID pvParam );

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
int main(int argc, char *argv[], char *envp[])
{
   HAB      hab = NULLHANDLE;          /* Anchor Block handle */
   HMQ      hmq = NULLHANDLE;          /* message queue handle */
   HWND     hwndFrame = NULLHANDLE;    /* handle of frame window */
   HWND     hwndClient = NULLHANDLE;   /* handle of client window */
   ULONG    flStyle = FCF_STANDARD;    /* Frame creation flags */
   QMSG     qmsg;                      /* PM message structure */

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
                             ClientWndProc,
                             CS_SIZEREDRAW | CS_CLIPCHILDREN,
                             0UL) ) {
         // failed!
         DisplayMessages(NULLHANDLE, "Unable to Register Window Class",
                         MSG_ERROR);
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Now register the window class for child windows
// --------------------------------------------------------------------------
      if (!WinRegisterClass( hab,
                             szChildClassName,
                             ChildWndProc,
                             CS_SIZEREDRAW,
                             0UL) ) {
         // failed!
         DisplayMessages(NULLHANDLE, "Unable to Register Window Class",
                         MSG_ERROR);
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Now create the application's main frame window
// --------------------------------------------------------------------------
      flStyle &= ~FCF_ACCELTABLE;
      hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                      0L,
                                      &flStyle,
                                      szClassName,
                                      TITLE_BAR_TEXT,
                                      WS_VISIBLE,
                                      NULLHANDLE,
                                      RID_HELPME,
                                      &hwndClient );
      if (hwndFrame == NULLHANDLE) {
         // no window - gotta leave.
         DisplayMessages(NULLHANDLE, "Error Creating Frame Window",
                         MSG_ERROR);
         break;
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
// Kill the message queue if successfully created
// --------------------------------------------------------------------------
   if (hmq != NULLHANDLE) {
      WinDestroyMsgQueue( hmq );
   } /* endif */

// --------------------------------------------------------------------------
// Disconnect from PM is necessary
// --------------------------------------------------------------------------
   if (hab != NULLHANDLE) {
      WinTerminate( hab );
   } /* endif */

   return 0;
}

// *******************************************************************************
// FUNCTION:     ClientWndProc
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
MRESULT APIENTRY ClientWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg) 
    {
     case HM_QUERY_KEYS_HELP:   
          return hm_querykeyshelp(hwnd, mp1, mp2);  

     case WM_COMMAND:          
          return wm_command(hwnd, mp1, mp2);             

     case WM_CREATE:           
          return wm_create(hwnd, mp1, mp2);   

     case WM_DESTROY:           
          return wm_destroy(hwnd, mp1, mp2);          

     case WM_INITMENU:         
          return wm_initmenu(hwnd, mp1, mp2);          

     case WM_MENUEND:          
          return wm_menuend(hwnd, mp1, mp2);          

     case WM_PAINT:             
          return wm_paint(hwnd, mp1, mp2);              

     default:                   
          return WinDefWindowProc(hwnd, msg, mp1, mp2);  

    } /* endswitch */
}

// *******************************************************************************
// FUNCTION:     ChildWndProc
//
// FUNCTION USE: The window procedure for the child window clients.
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
MRESULT APIENTRY ChildWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   switch (msg) {
   case HM_QUERY_KEYS_HELP:   return hm_querykeyshelp( hwnd, mp1, mp2 );
   case WM_ACTIVATE:          return wm_activate( hwnd, mp1, mp2 );
   case WM_COMMAND:           return wm_command( hwnd, mp1, mp2 );
   case WM_INITMENU:          return wm_initmenu( hwnd, mp1, mp2 );
   case WM_PAINT:             return wm_paint( hwnd, mp1, mp2 );
   default:                   return WinDefWindowProc( hwnd, msg, mp1, mp2 );
   } /* endswitch */
}

// *******************************************************************************
// FUNCTION:     hm_querykeyshelp
//
// FUNCTION USE: Notify IPF of the panel ID containing help for the
//               application's special keys.
//
// DESCRIPTION:  Return the constant which is the ID of the desired panel.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  TRUE if message processed, FALSE if not processed
//
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT hm_querykeyshelp( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
// --------------------------------------------------------------------------
// This is a simple function which normally just returns a predefined
// panel ID number.
// --------------------------------------------------------------------------

   return MRFROMSHORT(HID_KEYSUSAGE);
}

// *******************************************************************************
// FUNCTION:     wm_activate
//
// FUNCTION USE: Process WM_ACTIVATE messages for the application's windows.
//               This function is only called by child window clients.
//
// DESCRIPTION:  Applications which utilize "child" windows and provide
//               online help via IPF must notify IPF when the activation
//               state of the application's windows changes.  The child
//               window(s) must notify IPF whenever it becomes active
//               by setting IPF's active window.  When the child window(s)
//               is deactivated, it must clear IPF's active window, allowing
//               IPF to process help requests using its normal defaults.
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
MRESULT wm_activate( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND     hwndHelpInstance;
   HWND     hwndParent;

// --------------------------------------------------------------------------
// Since both the parent and child window are "active" at the same
// time, it is necessary to notify IPF which window to consider as
// the active window for help.
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// First get the help instance window handle
// --------------------------------------------------------------------------
   hwndHelpInstance = WinQueryHelpInstance( hwnd );

// --------------------------------------------------------------------------
// Now get the parent window
// --------------------------------------------------------------------------
   hwndParent = WinQueryWindow( hwnd, QW_PARENT );

   if (hwndHelpInstance != NULLHANDLE && hwndParent != NULLHANDLE) {

// --------------------------------------------------------------------------
// Is this window being activated?  If so, notify IPF that this is
// the active window for help. Otherwise, clear the active window
// setting allowing IPF to select the default window -- the parent
// --------------------------------------------------------------------------
      if (SHORT1FROMMP(mp1)) {

// --------------------------------------------------------------------------
// Window being activated - set the active window to the frame
// --------------------------------------------------------------------------
         WinSendMsg( hwndHelpInstance, HM_SET_ACTIVE_WINDOW,
                     (MPARAM)hwndParent, (MPARAM)hwndParent );
      } else {

// --------------------------------------------------------------------------
// Window being deactivated - clear the active window
// --------------------------------------------------------------------------
         WinSendMsg( hwndHelpInstance, HM_SET_ACTIVE_WINDOW,
                     NULLHANDLE, NULLHANDLE );
      } /* endif */

   } /* endif */

// --------------------------------------------------------------------------
// return reserved value back to the caller.
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wm_Command
//
// FUNCTION USE: Process WM_COMMAND messages for the application client window
//
// DESCRIPTION:  This function handles the WM_COMMAND messages for the
//               application's various client windows.  The majority of
//               this function is fluff to make the menu items work.
//               Important for illustrative purposes is the handling
//               of the items for the HELP menu.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  Reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT wm_command( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{

// --------------------------------------------------------------------------
// NOTE: Text book coding of this type of routine would place the variables
//       required for each message within a scope limited to that message.
//       However, some compiler implementations do not optimize the stack
//       usage in this case.  Placing common variables as shown here reduces
//       the programs stack usage and circumvents problems which may occur
//       the stack becomes excessively large.
// --------------------------------------------------------------------------
   FILEDLG        fd;
   HAB            hab;
   HWND           hwndFrame;
   HWND           hwndFParent;
   HWND           hwndDesktop;
   HWND           hwndHelp;
   ULONG          flStyle;
   HWND           hwndClient;
   RECTL          rectl;

   switch (SHORT1FROMMP(mp1)) {


// --------------------------------------------------------------------------
// The FILE NEW menu item is non-functional -- provide a message box
// --------------------------------------------------------------------------
   case MID_FILENEW:
      WinMessageBox( HWND_DESKTOP, hwnd,
                     "The FILE NEW function is not implemented",
                     TITLE_BAR_TEXT, RID_UNSUPPORTED,
                     MB_HELP | MB_OK | MB_WARNING );
      break;

// --------------------------------------------------------------------------
// The FILE SAVE menu item is non-functional -- provide a message box
// --------------------------------------------------------------------------
   case MID_FILESAVE:
      WinMessageBox( HWND_DESKTOP, hwnd,
                     "The FILE SAVE function is not implemented",
                     TITLE_BAR_TEXT, RID_UNSUPPORTED,
                     MB_HELP | MB_OK | MB_WARNING );
      break;

// --------------------------------------------------------------------------
// The FILE OPEN menu item raises the standard file dialog box.  A custom
// window procedure is used in this case so that the ID of the dialog
// window can be modified and the correct help obtained when requested.
// --------------------------------------------------------------------------
   case MID_FILEOPEN:
      memset( &fd, 0, sizeof( FILEDLG ) );
      fd.cbSize = sizeof( FILEDLG );
      fd.fl = FDS_OPEN_DIALOG | FDS_HELPBUTTON; // OPEN dialog w/ HELP
      fd.pfnDlgProc = StdFileDlgProc;           // Custom dialog proc

      WinFileDlg( HWND_DESKTOP, hwnd, &fd );
      break;

// --------------------------------------------------------------------------
// The FILE SAVEAS menu item raises the standard file dialog box.  A custom
// window procedure is used in this case so that the ID of the dialog
// window can be modified and the correct help obtained when requested.
// --------------------------------------------------------------------------
   case MID_FILESAVEAS:
      memset( &fd, 0, sizeof( FILEDLG ) );
      fd.cbSize = sizeof( FILEDLG );
      fd.fl = FDS_SAVEAS_DIALOG | FDS_HELPBUTTON; // SAVEAS dialog w/ HELP
      fd.pfnDlgProc = StdFileDlgProc;             // Custom dialog proc

      WinFileDlg( HWND_DESKTOP, hwnd, &fd );
      break;

// --------------------------------------------------------------------------
// The FILE EXIT menu item normally closes the application.  In this example;
// however, all child windows flow through this code and there may also be
// multiple instances of the main client window.
//
// When a client window whose frame is a direct descendent of the desktop
// receives this message, close the window (killing the thread).
//
// When a child client window receives this message, just destroy the
// window and it's frame.
// --------------------------------------------------------------------------
   case MID_EXIT:
// --------------------------------------------------------------------------
// Get this window's frame's parent.
// --------------------------------------------------------------------------
      hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
      hwndFParent = WinQueryWindow( hwndFrame, QW_PARENT );
// --------------------------------------------------------------------------
// Get the real window handle of the desktop
// --------------------------------------------------------------------------
      hab = WinQueryAnchorBlock( hwnd );
      hwndDesktop = WinQueryDesktopWindow( hab, NULLHANDLE );

      if (hwndFParent == hwndDesktop ) {
// --------------------------------------------------------------------------
// Top level window - issue WM_CLOSE to terminate the thread
// --------------------------------------------------------------------------
         WinSendMsg( hwnd, WM_CLOSE, NULL, NULL );
      } /* endif */
      break;

// --------------------------------------------------------------------------
// The SAMPLE CHILD WINDOW menu item creates two child windows to demonstrate
// how help is supplied for this type of window.
//
// Note that the child windows are for demonstartion purposes only and
// therefore have the same menus as the main application window.  Notice
// how the help tables are defined in the resource script.  The parent
// window menu items are referenced in one subtable and display one set of
// help panels.  The menu items for the left child window are referenced
// in another subtable and receive a different set of panels.  The right
// hand child window has no table entry and thus displays no help.
// --------------------------------------------------------------------------
   case MID_SAMPLECHLD:
      flStyle = FCF_STANDARD;

// --------------------------------------------------------------------------
// Get the size of the main windows client area for use in positioning
// the child windows.
// --------------------------------------------------------------------------
      WinQueryWindowRect( hwnd, &rectl );

// --------------------------------------------------------------------------
// Create the first child window.
// --------------------------------------------------------------------------
      flStyle &= ~(FCF_ACCELTABLE | FCF_SYSMENU);
      flStyle |= FCF_NOBYTEALIGN;
      hwndFrame = WinCreateStdWindow( hwnd,
                                      0L,
                                      &flStyle,
                                      szChildClassName,
                                      TITLE_BAR_TEXT,
                                      0UL,
                                      NULLHANDLE,
                                      RID_SAMPLECHLD,
                                      &hwndClient );

// --------------------------------------------------------------------------
// Make the window half the size of the parent and positioned in the
// left half of the parent client window.
// --------------------------------------------------------------------------
      WinSetWindowPos( hwndFrame, HWND_TOP, rectl.xLeft,
                       rectl.yBottom,
                       rectl.xRight/2,
                       rectl.yTop,
                       SWP_SIZE | SWP_MOVE | SWP_SHOW);

// --------------------------------------------------------------------------
// Create the second child window
// --------------------------------------------------------------------------
      hwndFrame = WinCreateStdWindow( hwnd,
                                      0L,
                                      &flStyle,
                                      szChildClassName,
                                      TITLE_BAR_TEXT,
                                      0UL,
                                      NULLHANDLE,
                                      RID_SAMPLECHLD2,
                                      &hwndClient );

// --------------------------------------------------------------------------
// Display the second child window in the right half of the parent client.
// --------------------------------------------------------------------------
      WinSetWindowPos( hwndFrame, HWND_TOP, rectl.xRight/2,
                       rectl.yBottom,
                       rectl.xRight/2,
                       rectl.yTop,
                       SWP_SIZE | SWP_MOVE | SWP_SHOW );
      break;

// --------------------------------------------------------------------------
// The SAMPLE DIALOG menu item raises a dialog box with several controls
// to illustrate the programming required to display help for dialog items.
// --------------------------------------------------------------------------
   case MID_SAMPLEDLG:
      WinDlgBox( HWND_DESKTOP, hwnd, WinDefDlgProc, NULLHANDLE,
                 RID_SAMPLEDLG, NULL );
      break;

// --------------------------------------------------------------------------
// The SAMPLE MESSAGE BOX menu item displays a message box with a HELP
// button to illustrate how the help tables are configured to display
// HELP for message boxes.
// --------------------------------------------------------------------------
   case MID_SAMPLEMSG:
      WinMessageBox( HWND_DESKTOP, hwnd,
            "Sample Message Box\nSelect OK to continue",
            TITLE_BAR_TEXT, RID_SAMPLEMSG,
            MB_OK | MB_HELP );
      break;

// --------------------------------------------------------------------------
// The SAMPLE THREAD menu item raises a second primary window from a
// a new thread.  Examine the code for this thread to see the actions
// necessary to display help in a second thread.
// --------------------------------------------------------------------------
   case MID_SAMPLETHD:
      _beginthread( thread2, 0L, 0x4000L, NULL );
      break;

// --------------------------------------------------------------------------
// The HELP INDEX menu item is used to display a list of the index
// entries in the help file.  This is accomplished by sending an
// HM_HELP_INDEX message to the help instance hwnd.
// --------------------------------------------------------------------------
   case MID_HELPINDEX:

// --------------------------------------------------------------------------
// Get the handle to the help instance associated with this window
// --------------------------------------------------------------------------
      hwndHelp = WinQueryHelpInstance( hwnd );
      if (hwndHelp != NULLHANDLE) {

// --------------------------------------------------------------------------
// Send HM_HELP_INDEX message to the help instance
// --------------------------------------------------------------------------
         WinSendMsg( hwndHelp, HM_HELP_INDEX, NULL, NULL );
      } /* endif */
      break;

// --------------------------------------------------------------------------
// The HELP GENERAL HELP menu item normally displays a help panel which
// which provides an overview of the application.  This panel is identified
// as the 3rd item in the HELPTABLE for the current focus window.
// --------------------------------------------------------------------------
   case MID_GENHELP:

// --------------------------------------------------------------------------
// Get the handle to the help instance
// --------------------------------------------------------------------------
      hwndHelp = WinQueryHelpInstance( hwnd );
      if (hwndHelp != NULLHANDLE) {

// --------------------------------------------------------------------------
// Send HM_GENERAL_HELP to the help instance
// --------------------------------------------------------------------------
         WinSendMsg( hwndHelp, HM_GENERAL_HELP, NULL, NULL );
      } /* endif */
      break;

// --------------------------------------------------------------------------
// The HELP USING HELP menu item displays help on how to use the online
// help subsystem.  This panel is normally provided internally by IPF
// and is displayed by sending an HM_DISPLAY_HELP message with zero values
// in both mp1 and mp2.
// --------------------------------------------------------------------------
   case MID_USEHELP:

// --------------------------------------------------------------------------
// Get the handle to the help instance
// --------------------------------------------------------------------------
      hwndHelp = WinQueryHelpInstance( hwnd );
      if (hwndHelp != NULLHANDLE) {

// --------------------------------------------------------------------------
// Send HM_DISPLAY_HELP to the help instance
// --------------------------------------------------------------------------
         WinSendMsg( hwndHelp, HM_DISPLAY_HELP,
                     MPFROMLONG(0L), MPFROMLONG(0L));
      } /* endif */
      break;

// --------------------------------------------------------------------------
// The HELP KEYS HELP menu item is used to display help for any special
// keyboard assignments used by the application.  The panel is raised by
// sending an HM_KEYS_HELP message to the help window.
//
// The help window then sends an HM_QUERY_KEYS_HELP message back to the
// application window to determine the id of the panel to be displayed.
// This allows IPF to display the application keys help when the user
// selects the keys help menu item from the help panel menu.
// --------------------------------------------------------------------------
   case MID_KEYSHELP:

// --------------------------------------------------------------------------
// Get the handle to the help instance
// --------------------------------------------------------------------------
      hwndHelp = WinQueryHelpInstance( hwnd );
      if (hwndHelp != NULLHANDLE) {

// --------------------------------------------------------------------------
// Send HM_KEYS_HELP to the help instance
// --------------------------------------------------------------------------
         WinSendMsg( hwndHelp, HM_KEYS_HELP, NULL, NULL );
      } /* endif */
      break;

   case MID_PRODINFO:
// --------------------------------------------------------------------------
// Display the product information dialog
// --------------------------------------------------------------------------
      WinDlgBox( HWND_DESKTOP, hwnd, WinDefDlgProc, NULLHANDLE,
                 RID_PRODINFO, NULL );
      break;

   } /* endswitch */

// --------------------------------------------------------------------------
// return with the proper reserved value
// --------------------------------------------------------------------------
   return (MRESULT)0;
}

// *******************************************************************************
// FUNCTION:     wm_create
//
// FUNCTION USE: Process WM_CREATE messages for the application client window
//
// DESCRIPTION:  Create the help instance for the application and associate
//               the new instance with the application window.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  TRUE to abort window creation, FALSE to continue
//
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT wm_create( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   MRESULT        retval = MRFROMLONG(TRUE);   /* Assume error condition */
   HELPINIT       hi;
   HWND           hwndHelpInstance;

// --------------------------------------------------------------------------
// Fill in the help manager initialization structure
// --------------------------------------------------------------------------
   hi.cb = sizeof( HELPINIT );         /* structure size */
   hi.ulReturnCode = 0l;               /* initialize return code to 0 */
   hi.pszTutorialName = NULL;          /* no tutorial */
   hi.phtHelpTable = (PHELPTABLE)MAKEULONG( RID_HELPME, 0xffff );
                                       /* help table in resource file */
   hi.hmodHelpTableModule = NULLHANDLE;
                                       /* help table in application resource */
   hi.hmodAccelActionBarModule = NULLHANDLE;
                                       /* customization in application resource */
   hi.idAccelTable = 0l;               /* no customized accelator table */
   hi.idActionBar = 0l;                /* no customized menu */
   hi.pszHelpWindowTitle = "Help for HELPME - Chapter 5 Sample";
                                       /* simple title for help window */
   hi.fShowPanelId = CMIC_HIDE_PANEL_ID;
                                       /* don't show the panel id's */
   hi.pszHelpLibraryName = "HELPME.HLP";

// --------------------------------------------------------------------------
// create an instance of the help manager for this application
// --------------------------------------------------------------------------
   hwndHelpInstance = WinCreateHelpInstance( WinQueryAnchorBlock(hwnd),
                                             &hi );
   if (hwndHelpInstance != NULLHANDLE) {

// --------------------------------------------------------------------------
// associate the help instance with the frame window
// --------------------------------------------------------------------------
      if (WinAssociateHelpInstance( hwndHelpInstance,
                                    WinQueryWindow( hwnd, QW_PARENT ))) {

// --------------------------------------------------------------------------
// set return value to indicate success -- continue window creation.
// --------------------------------------------------------------------------
         retval = MRFROMLONG(FALSE);
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// back to the caller with the return value
// --------------------------------------------------------------------------
   return retval;
}

// *******************************************************************************
// FUNCTION:     wm_destroy
//
// FUNCTION USE: Process WM_DESTROY messages for the application client window
//
// DESCRIPTION:  Destroy the help instance created for the application thread.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  Reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT wm_destroy( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND     hwndHelp;
   HWND     hwndFrame;

// --------------------------------------------------------------------------
// Get the associated help instance window handle
// --------------------------------------------------------------------------
   hwndHelp = WinQueryHelpInstance( hwnd );
   if (hwndHelp != NULLHANDLE) {

// --------------------------------------------------------------------------
// Get the parent frame window's handle
// --------------------------------------------------------------------------
      hwndFrame = WinQueryWindow( hwnd, QW_PARENT );

// --------------------------------------------------------------------------
// Remove the association between the frame and the help instance.
// --------------------------------------------------------------------------
      WinAssociateHelpInstance( NULLHANDLE, hwndFrame );

// --------------------------------------------------------------------------
// Destroy the help instance
// --------------------------------------------------------------------------
      WinDestroyHelpInstance( hwndHelp );
   } /* endif */

// --------------------------------------------------------------------------
// return proper reserved value to the caller
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wm_initmenu
//
// FUNCTION USE: Process WM_INITMENU messages for the application client window
//
// DESCRIPTION:  Set the "active help window" to the frame window which
//               activated the menu.  This message should be processed when
//               child windows are used to insure that the help for the
//               proper window is displayed.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  Reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT wm_initmenu( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND     hwndHelpInstance;
   HWND     hwndParent;

// --------------------------------------------------------------------------
// First get the help instance handle
// --------------------------------------------------------------------------
   hwndHelpInstance = WinQueryHelpInstance( hwnd );

// --------------------------------------------------------------------------
// Now get the parent window
// --------------------------------------------------------------------------
   hwndParent = WinQueryWindow( hwnd, QW_PARENT );

   if (hwndHelpInstance != NULLHANDLE && hwndParent != NULLHANDLE) {


// --------------------------------------------------------------------------
// Set the active help window to the window which raised the menu item,
// the parent frame of the window which received this message.
// --------------------------------------------------------------------------
      WinSendMsg( hwndHelpInstance, HM_SET_ACTIVE_WINDOW,
                      (MPARAM)hwndParent, (MPARAM)hwndParent );
   } /* endif */

// --------------------------------------------------------------------------
// return proper reserved value to the caller
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wm_menuend
//
// FUNCTION USE: Process WM_MENUEND messages for the application client window
//
// DESCRIPTION:  Remove the "active help window" setting. This message should
//               be processed when child windows are used to insure that the
//               help for the proper window is displayed.  Note that this
//               message is processed by the main application window but not
//               by the created child window.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  Reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT wm_menuend( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND     hwndHelpInstance;

// --------------------------------------------------------------------------
// First get the help instance handle
// --------------------------------------------------------------------------
   hwndHelpInstance = WinQueryHelpInstance( hwnd );

   if (hwndHelpInstance != NULLHANDLE) {

// --------------------------------------------------------------------------
// Clear the active help window setting.
// --------------------------------------------------------------------------
      WinSendMsg( hwndHelpInstance, HM_SET_ACTIVE_WINDOW,
                      (MPARAM)NULLHANDLE, (MPARAM)NULLHANDLE );
   } /* endif */

// --------------------------------------------------------------------------
// return proper reserved value to the caller
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wm_paint
//
// FUNCTION USE: Process WM_PAINT messages for the application client window
//
// DESCRIPTION:  Erase the window area and display a message in the center
//               of the window.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  Reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT wm_paint( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HPS         hps;        /* Presentation space for painting */
   RECTL       rectl;      /* Rectangle structure for painting, etc. */

// --------------------------------------------------------------------------
// Initialize the repaint operation and obtain a presentation space handle.
// --------------------------------------------------------------------------
   hps = WinBeginPaint( hwnd, NULLHANDLE, &rectl );
   if (hps != NULLHANDLE) {

// --------------------------------------------------------------------------
// Fill in the background
// --------------------------------------------------------------------------
      WinFillRect( hps, &rectl, CLR_WHITE );

// --------------------------------------------------------------------------
// Get actual size of the window since the invalid area may not represent
// the entire window, but the text drawing needs to be centered in the window
// --------------------------------------------------------------------------
      WinQueryWindowRect( hwnd, &rectl );

// --------------------------------------------------------------------------
// Write text to the center of the window
// --------------------------------------------------------------------------
      WinDrawText( hps, -1L, "HELP ME! -- Press F1", &rectl,
                   CLR_BLACK, CLR_WHITE,
                   DT_CENTER | DT_VCENTER );

// --------------------------------------------------------------------------
// Done - release the presentation space handle.
// --------------------------------------------------------------------------
      WinEndPaint( hps );
   } /* endif */

// --------------------------------------------------------------------------
// return proper reserved value to the caller
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     StdFileDlgProc
//
// FUNCTION USE: Customized dialog procedure for the standard file dialog.
//
// DESCRIPTION:  Processes the WM_INITDLG message to modify the window ID
//               of the dialog box, allowing a different set of help panels
//               to be displayed for the OPEN function and the SAVE AS
//               function.
//
// PARAMETERS:   HWND     client window handle
//               ULONG    message id
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  Reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT _System StdFileDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   PFILEDLG pfd;

   if (msg == WM_INITDLG) {

// --------------------------------------------------------------------------
// Obtain the pointer to the FILEDLG structure for the dialog.
// --------------------------------------------------------------------------
      pfd = (PFILEDLG)WinQueryWindowULong( hwnd, QWL_USER );

// --------------------------------------------------------------------------
// Determine the functionality of the dialog displayed and set the
// window id accordingly.
// --------------------------------------------------------------------------
      if (pfd->fl & FDS_OPEN_DIALOG) {
         WinSetWindowUShort( hwnd, QWS_ID, RID_FILEOPEN );
      } else if (pfd->fl & FDS_SAVEAS_DIALOG){
         WinSetWindowUShort( hwnd, QWS_ID, RID_FILESAVEAS );
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// pass all messages through the default file dialog procedure.
// --------------------------------------------------------------------------
   return WinDefFileDlgProc( hwnd, msg, mp1, mp2 );
}

// *******************************************************************************
// FUNCTION:     thread2
//
// FUNCTION USE: "main" function for the second and subsequent threads.
//
// DESCRIPTION:  This is the main function for secondary threads.  This function
//               obtains an anchor block handle, creates the application message
//               queue and creates the frame and client windows.  This routine
//               will subsequently enter the typical message loop which polls
//               the message queue, getting and dispatching messages until
//               WinGetMsg returns FALSE indicating a WM_QUIT message was
//               received, thereby terminating the application.
//
//
// PARAMETERS:   ULONG    not used - provided for prototype compatibility.
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
void _Optlink thread2( PVOID pvParam )
{
   HAB      hab = NULLHANDLE;          /* Anchor Block handle */
   HMQ      hmq = NULLHANDLE;          /* message queue handle */
   HWND     hwndFrame = NULLHANDLE;    /* handle of frame window */
   HWND     hwndClient = NULLHANDLE;   /* handle of client window */
   ULONG    flStyle = FCF_STANDARD;    /* Frame creation flags */
   QMSG     qmsg;                      /* PM message structure */

// --------------------------------------------------------------------------
// All threads which make calls to the PMWIN and HELPMGR API's must
// establish an anchor block and create a Help Instance; in many
// instances, such as following, the thread function is identical
// to the "main" function of a typical application.
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Initialize loop for error handling
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
// Note: since the classes have already been registered in the main thread,
//       there is no need to register them again in this thread.
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Now create the thread's main frame window
// --------------------------------------------------------------------------
      flStyle &= ~FCF_ACCELTABLE;
      hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                      0L,
                                      &flStyle,
                                      szClassName,
                                      MT_TITLE_BAR_TEXT,
                                      WS_VISIBLE,
                                      NULLHANDLE,
                                      RID_HELPME,
                                      &hwndClient );
      if (hwndFrame == NULLHANDLE) {
         // no window - gotta leave.
         DisplayMessages(NULLHANDLE, "Error Creating Frame Window",
                         MSG_ERROR);
         break;
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
// Kill the message queue if successfully created
// --------------------------------------------------------------------------
   if (hmq != NULLHANDLE) {
      WinDestroyMsgQueue( hmq );
   } /* endif */

// --------------------------------------------------------------------------
// Disconnect from PM is necessary
// --------------------------------------------------------------------------
   if (hab != NULLHANDLE) {
      WinTerminate( hab );
   } /* endif */
}
