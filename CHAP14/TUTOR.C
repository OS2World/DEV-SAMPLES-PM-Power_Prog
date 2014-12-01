// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      TUTOR.C                                          mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Source File for the TUTOR Sample program which demonstrates   บ
// บ                 the TUTORIAL control sample for Chapter 14.                   บ
// บ                                                                               บ
// บ  NOTES:         This code tests the dynamic link library which implements     บ
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
// Define the window IDs used by the application.
// --------------------------------------------------------------------------
#define  ID_APPLICATION    100
#define  ID_CONTROL        101

// --------------------------------------------------------------------------
// Define global data used in this source file
// --------------------------------------------------------------------------
static   BOOL  false = FALSE;
static   char  szText[] =
         "This is the first panel of the tutorial.  To see the next "
         "panel, click on the right half of the window.";
static   char  szText1[] =
         "This is the second panel of the tutorial.  To see the previous "
         "panel, click on the left half of the window.  To see the next "
         "panel, click on the right half of the window.";
static   char szText2[] =
         "This is the last panel of the tutorial.  To see the previous "
         "panel, click on the left half of the window.";
static   HBITMAP     hbm = NULLHANDLE;
static   ULONG       gState = 0;

// --------------------------------------------------------------------------
// Prototype for the main window procedure
// --------------------------------------------------------------------------
MRESULT APIENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------
// Prototypes for the message processing worker functions
// --------------------------------------------------------------------------
MRESULT wmControl( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wmSize( HWND hwnd, MPARAM mp1, MPARAM mp2 );

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
// PARAMETERS:   Standard 'C' language main function parameters.
//
// RETURNS:      int
//
// *******************************************************************************
int main(int argc, char *argv[], char *envp[])
{
   HAB   hab = NULLHANDLE;
   HMQ   hmq = NULLHANDLE;
   QMSG  qmsg;
   HWND  hwndClient = NULLHANDLE;
   ULONG flFCFlags = FCF_STANDARD & ~(FCF_ACCELTABLE | FCF_MENU);

   do {
// --------------------------------------------------------------------------
// Initialize the PM environment for the application
// --------------------------------------------------------------------------
      if ((hab = WinInitialize(0)) == NULLHANDLE) break;

// --------------------------------------------------------------------------
// Create the message queue
// --------------------------------------------------------------------------
      if ((hmq = WinCreateMsgQueue(hab,0)) == NULLHANDLE) break;

// --------------------------------------------------------------------------
// Register the application window class
// --------------------------------------------------------------------------
      if (!WinRegisterClass(hab, "APPLICATION", ClientWndProc, 0, 0)) break;

// --------------------------------------------------------------------------
// Register the TUTORIAL window class
// --------------------------------------------------------------------------
      if (!RegisterTutorialControl(hab)) break;

// --------------------------------------------------------------------------
// Create the application frame window - which creates the tutorial control
// --------------------------------------------------------------------------
      if ((WinCreateStdWindow( HWND_DESKTOP, WS_VISIBLE,
               &flFCFlags, "APPLICATION", "TUTOR - Chapter 14 Sample Program", 0, NULLHANDLE,
               ID_APPLICATION, &hwndClient )) == NULLHANDLE ) break;

// --------------------------------------------------------------------------
// Give focus to the tutorial control window
// --------------------------------------------------------------------------
      WinSetFocus( HWND_DESKTOP, WinWindowFromID( hwndClient, ID_CONTROL ));

// --------------------------------------------------------------------------
// Process messages
// --------------------------------------------------------------------------
      while (WinGetMsg(hab, &qmsg, NULLHANDLE, 0, 0)) {
         WinDispatchMsg(hab, &qmsg);
      } /* endwhile */
   } while ( false ); /* enddo */

// --------------------------------------------------------------------------
// Done - delete message queue and release PM environment
// --------------------------------------------------------------------------
   if (hmq != NULLHANDLE) WinDestroyMsgQueue(hmq);
   if (hab != NULLHANDLE) WinTerminate(hab);

   return 0;
}


// *******************************************************************************
// FUNCTION:     ClientWndProc
//
// FUNCTION USE: Typical client window procedure
//
// DESCRIPTION:  This procedure processes all messages arriving for the client
//               window of the main application window.  In this implementation
//               the window procedure's purpose is to resolve the proper method
//               to be called for the message.  This structure is similar to
//               execution time method binding in an object-oriented system.
//
// PARAMETERS:   HWND     client window handle
//               ULONG    window message
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  WinDefWindowProc for all unprocessed messages;
//                        otherwise, message dependent
//
// *******************************************************************************
MRESULT APIENTRY ClientWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg) 
    {
     case WM_CONTROL:           
          return wmControl(hwnd, mp1, mp2);   

     case WM_CREATE:      
          return wmCreate(hwnd, mp1, mp2);          

     case WM_SIZE:              
          return wmSize(hwnd, mp1, mp2);           

     default:                   
          return WinDefWindowProc(hwnd, msg, mp1, mp2);   

    } /* endswitch */
}

// *******************************************************************************
// FUNCTION:     wmCreate
//
// FUNCTION USE: Handles processing of the WM_CREATE message
//
// DESCRIPTION:  Loads the bitmap used in the tutorial display, sets up the
//               tutorial control data and creates the tutorial window.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   not used by this function
//               MPARAM   not used by this function
//
// RETURNS:      MRESULT  BOOLEAN indicating if errors occurred.
//
// *******************************************************************************
MRESULT wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND     hwndTut;
   TUTORCDATA  tcd;
   HPS         hps;

// --------------------------------------------------------------------------
// Get a presentation space for the current window and load the bitmap from
// the application's resources.
// --------------------------------------------------------------------------
   hps = WinGetPS( hwnd );
   hbm = GpiLoadBitmap( hps, NULLHANDLE, 1000, 0, 0 );
   WinReleasePS( hps );

// --------------------------------------------------------------------------
// fill in the initial control data - text in panel 1, bitmap in panel 2,
// and automation set to 10 sec intervals.
// --------------------------------------------------------------------------
   tcd.cb = sizeof(tcd);
   tcd.hWin1Data = szText;
   tcd.hWin2Data = (PVOID)hbm;
   tcd.ulTimerMS = 10000;

// --------------------------------------------------------------------------
// Create the tutorial window with a vertically split display, a bitmap in
// the right half of the window, and automation enabled.
// --------------------------------------------------------------------------
   hwndTut = WinCreateWindow (hwnd,
                              WC_TUTORIAL,
                              NULL,
                              WS_VISIBLE | TS_SPLITVERT |
                              TS_PANL2BITMAP | TS_AUTO,
                              0,
                              0,
                              0,
                              0,
                              hwnd,
                              HWND_TOP,
                              ID_CONTROL,
                              &tcd,
                              NULL);

// --------------------------------------------------------------------------
// Start the automation running
// --------------------------------------------------------------------------
   WinSendMsg( hwndTut, TM_AUTOMATE, MPFROMLONG(TRUE), MPFROMLONG(0L));

// --------------------------------------------------------------------------
// indicate no errors - creation processing can continue
// --------------------------------------------------------------------------
   return MRFROMLONG(FALSE);
}

// *******************************************************************************
// FUNCTION:     wmSize
//
// FUNCTION USE: Handles processing of the WM_SIZE message
//
// DESCRIPTION:  Adjusts the size of the tutorial control to match the size
//               of the client window.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   not used by this function
//               MPARAM   new width and height of the client window
//
// RETURNS:      MRESULT  reserved value
//
// *******************************************************************************
MRESULT wmSize( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{

// --------------------------------------------------------------------------
// modifiy the control window size
// --------------------------------------------------------------------------
   WinSetWindowPos( WinWindowFromID( hwnd, ID_CONTROL ), NULLHANDLE,
                    0, 0, SHORT1FROMMP(mp2), SHORT2FROMMP(mp2),
                    SWP_SIZE | SWP_MOVE );

// --------------------------------------------------------------------------
// return the reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wmControl
//
// FUNCTION USE: Handles processing of the WM_CONTROL message
//
// DESCRIPTION:  Processes notifications from the tutorial control window.
//               In particular, handles the TN_CLICKED notification to
//               change the contents of the control.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   Control ID in the low order 16 bits.
//                        Notification code in the high order 16 bits.
//               MPARAM   notification code dependent.  For TN_CLICKED
//                        is either TA_BACKWARD or TA_FORWARD
//
// RETURNS:      MRESULT  reserved value
//
// *******************************************************************************
MRESULT wmControl( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   BOOL       bAuto;
   TUTORCDATA tcd;
   HWND       hwndTutor;

// --------------------------------------------------------------------------
// Make sure the notification is from the desired control
// --------------------------------------------------------------------------
   if (SHORT1FROMMP(mp1) == ID_CONTROL) {

// --------------------------------------------------------------------------
// and only process TN_CLICKED notification.
// --------------------------------------------------------------------------
      if (SHORT2FROMMP(mp1) == TN_CLICKED ) {

// --------------------------------------------------------------------------
// Get the control window handle, the automation state, and the current
// control data for the control
// --------------------------------------------------------------------------
         hwndTutor = WinWindowFromID( hwnd, ID_CONTROL );
         bAuto = (BOOL)WinSendMsg( hwndTutor, TM_QUERYSTATE,
                                   MPFROMLONG(0L), MPFROMLONG(0L));
         tcd.cb = sizeof(TUTORCDATA);
         WinSendMsg( hwndTutor, TM_QUERYDATA, MPFROMP(&tcd), MPFROMLONG(0L));

// --------------------------------------------------------------------------
// If this is a move forward, increment the panel to be displayed, unless
// at the end.  If automation is enabled, roll back to the beginning
// --------------------------------------------------------------------------
         if( LONGFROMMP(mp2) == TA_FORWARD ) {
            if (gState < 2) gState++;
            else if (bAuto) gState = 0;

         } else {
// --------------------------------------------------------------------------
// If a move backward, decrement the panel to be displayed, but not below 0
// --------------------------------------------------------------------------
            if (gState) gState--;
         } /* endif */

// --------------------------------------------------------------------------
// set the control data based on the current state
// --------------------------------------------------------------------------
         switch (gState) {
         case 0:
            tcd.hWin1Data = szText;
            break;
         case 1:
            tcd.hWin1Data = szText1;
            break;
         case 2:
            tcd.hWin1Data = szText2;
            break;
         } /* endswitch */

// --------------------------------------------------------------------------
// Send the modified control data back to the control
// --------------------------------------------------------------------------
         WinSendMsg( hwndTutor, TM_SETDATA, MPFROMP(&tcd), MPFROMLONG(TRUE));
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// return the reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0l);
}
