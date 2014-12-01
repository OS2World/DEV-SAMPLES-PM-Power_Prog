// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      CARDIO.C                                         mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   I/O Functions for CARDFILE/Chapter 11 Sample Program          บ
// บ                                                                               บ
// บ  NOTES:         None                                                          บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#include "cardfile.h"
#include "shcommon.h"

// --------------------------------------------------------------------------
// Miscellaneous constant definitions used only in this source module.
// --------------------------------------------------------------------------
#define  OBJ_CLASS_NAME          "OBJWNDCLASS"

// --------------------------------------------------------------------------
// A global variable with the value FALSE, used to prevent compiler warnings
// in loops which are terminated from within.
// --------------------------------------------------------------------------
static BOOL  false = FALSE;

// --------------------------------------------------------------------------
// Main function for the object window thread
// --------------------------------------------------------------------------
void IOObjectThread( PVOID param );

// --------------------------------------------------------------------------
// Window procedure for the object window
// --------------------------------------------------------------------------
MRESULT APIENTRY ObjWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------
// Message processing functions for the object window
// --------------------------------------------------------------------------
static MRESULT umClose( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT umOpen( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT umRead( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT umWrite( HWND hwnd, MPARAM mp1, MPARAM mp2 );

// *******************************************************************************
// FUNCTION:     CreateIOObject
//
// FUNCTION USE: Externally callable function to create the object window
//
// DESCRIPTION:  Starts the thread for the object window
//
// PARAMETERS:   HWND     application client window handle
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
void CreateIOObject( HWND hwnd )
{
   _beginthread( IOObjectThread, 0, 0x8000, (PVOID)hwnd );
}

// *******************************************************************************
// FUNCTION:     IOObjectThread
//
// FUNCTION USE: Main routine for the object window thread
//
// DESCRIPTION:  Initializes the PM environment for the thread, creates the
//               object window, and runs the message processing loop.
//
// PARAMETERS:   PVOID        required parameter - translates to application
//                            client window handle
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
void IOObjectThread( PVOID param )
{
   HAB      hab = (HAB)NULLHANDLE;
   HMQ      hmq = (HMQ)NULLHANDLE;
   QMSG     qmsg;
   HWND     hwndObj;
   HWND     hwndApp = (HWND)param;

// --------------------------------------------------------------------------
// Single iteration loop - makes error handling easier
// --------------------------------------------------------------------------
   do {
// --------------------------------------------------------------------------
// First things first - set up the PM environment for the thread
// --------------------------------------------------------------------------
      hab = WinInitialize( 0 );
      if (hab == (HAB)NULLHANDLE) {
         // Oops - couldn't do it - bail out!
         DosBeep( 60, 100 );
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Next thing we need is a message queue
// --------------------------------------------------------------------------
      hmq = WinCreateMsgQueue( hab, 0 );
      if (hmq == (HMQ)NULLHANDLE) {
         // No message queue equals no PM - quit now
         DosBeep( 60, 100 );
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Now register the window class for object window
// --------------------------------------------------------------------------
      if (!WinRegisterClass( hab,                  // anchor block
                          OBJ_CLASS_NAME,          // class name
                          ObjWndProc,              // window procedure
                          0L,                      // class style
                          0L))                   { // bytes of instance data
         // failed!
         DisplayMessages(NULLHANDLE, "Unable to Register Object Class",
                         MSG_ERROR);
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Finally, now the window can be created
// --------------------------------------------------------------------------
      hwndObj = WinCreateWindow( HWND_OBJECT, // system object wnd is parent
                       OBJ_CLASS_NAME,        // Class is notebook control
                       NULL,                  // No window text
                       0l,                    // No style
                       0l, 0l, 0l, 0l,        // Size rectangle
                       NULLHANDLE,            // No owner
                       HWND_TOP,              // Place on top of siblings
                       OBJWND_ID,             // Window/control identifier
                       NULL,                  // No control data
                       NULL );                // No press params
      if (hwndObj == (HWND)NULLHANDLE) {
         // no window - gotta leave.
         DisplayMessages(NULLHANDLE, "Error Creating Frame Window",
                         MSG_ERROR);
         break;
      } /* endif */

// --------------------------------------------------------------------------
// All set, signal the application window with the object window handle
// --------------------------------------------------------------------------
      WinPostMsg( hwndApp, UM_OBJHWND, MPFROMHWND(hwndObj), MPFROMLONG(0L) );

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
// Clean up on exit - releasing the message queue and PM environment
// --------------------------------------------------------------------------
   if (hmq != (HMQ)NULLHANDLE) {
      WinDestroyMsgQueue( hmq );
   } /* endif */

   if (hab != (HAB)NULLHANDLE) {
      WinTerminate( hab );
   } /* endif */
}

// *******************************************************************************
// FUNCTION:     ObjWndProc
//
// FUNCTION USE: Procedure to receive messages for the object window.
//
// DESCRIPTION:  This procedure processes all messages arriving for the object
//               window.  In this implementation the window procedure's purpose
//               is to resolve the proper method to be called for the message.
//
// PARAMETERS:   HWND     object window handle
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
   switch (msg) {
   case UM_CLOSE:             return umClose( hwnd, mp1, mp2 );
   case UM_OPEN:              return umOpen( hwnd, mp1, mp2 );
   case UM_READ:              return umRead( hwnd, mp1, mp2 );
   case UM_WRITE:             return umWrite( hwnd, mp1, mp2 );
   default:                   return WinDefWindowProc( hwnd, msg, mp1, mp2 );
   } /* endswitch */
}

// *******************************************************************************
// FUNCTION:     umClose
//
// FUNCTION USE: Worker function to process the UM_CLOSE message.
//
// DESCRIPTION:  Closes a file for main application thread
//
// PARAMETERS:   HWND     object window handle
//               MPARAM   pointer to file operation control structure
//               MPARAM   reserved
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT umClose( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PFILEOPCTL     pfoc = (PFILEOPCTL)mp1;

// --------------------------------------------------------------------------
// close the file
// --------------------------------------------------------------------------
   fclose( pfoc->file );

// --------------------------------------------------------------------------
// tell the application client window that we're done
// --------------------------------------------------------------------------
   WinPostMsg( pfoc->hwndClient, UM_DONE, MPFROMP(pfoc), MPFROMLONG(0L));

   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     umOpen
//
// FUNCTION USE: Worker function to process the UM_OPEN message.
//
// DESCRIPTION:  Opens a file for main application thread
//
// PARAMETERS:   HWND     object window handle
//               MPARAM   pointer to file operation control structure
//               MPARAM   reserved
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT umOpen( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PFILEOPCTL        pfoc = (PFILEOPCTL)mp1;
   ULONG             ulRC = OPSTAT_NOERROR;

// --------------------------------------------------------------------------
// Initialize the stream pointer in the file operation control structure
// --------------------------------------------------------------------------
   pfoc->file = NULL;

// --------------------------------------------------------------------------
// Tell the application client who we are
// --------------------------------------------------------------------------
   pfoc->hwndServer = hwnd;

// --------------------------------------------------------------------------
// Open the file
// --------------------------------------------------------------------------
   pfoc->file = fopen( pfoc->pszFilename, pfoc->pszMode );
   if (pfoc->file == NULL) {
      ulRC = OPSTAT_FILEOPENERROR;
   } /* endif */

// --------------------------------------------------------------------------
// Send result of the open operation back to the application client window
// --------------------------------------------------------------------------
   WinPostMsg( pfoc->hwndClient, UM_READY, MPFROMP(pfoc), MPFROMLONG(ulRC));

   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     umRead
//
// FUNCTION USE: Worker function to process the UM_READ message.
//
// DESCRIPTION:  Reads a record from the file
//
// PARAMETERS:   HWND     object window handle
//               MPARAM   pointer to file operation control structure
//               MPARAM   reserved
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT umRead( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PFILEOPCTL        pfoc = (PFILEOPCTL)mp1;
   ULONG             ulRC = OPSTAT_NOERROR;

// --------------------------------------------------------------------------
// The first four bytes contain the length of the remainder of the record
// --------------------------------------------------------------------------
   if (!(fread(&pfoc->cbBuffer, sizeof(ULONG), 1, pfoc->file))) {
      ulRC = OPSTAT_FILEREADERROR;
   } else {

// --------------------------------------------------------------------------
// allocate memory for the record
// --------------------------------------------------------------------------
      pfoc->buffer = malloc( pfoc->cbBuffer );

// --------------------------------------------------------------------------
// read the record
// --------------------------------------------------------------------------
      if (!(fread(pfoc->buffer, pfoc->cbBuffer, 1, pfoc->file))) {
         ulRC = OPSTAT_FILEREADERROR;
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// send results of the read back to the application client window
// --------------------------------------------------------------------------
   WinPostMsg( pfoc->hwndClient, UM_READY, MPFROMP(pfoc), MPFROMLONG(ulRC));

   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     umWrite
//
// FUNCTION USE: Worker function to process the UM_WRITE message.
//
// DESCRIPTION:  Writes a record to the file
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   pointer to file operation control structure
//               MPARAM   reserved
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT umWrite( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PFILEOPCTL        pfoc = (PFILEOPCTL)mp1;
   ULONG             ulRC = OPSTAT_NOERROR;

// --------------------------------------------------------------------------
// write the length of the record into the first four bytes
// --------------------------------------------------------------------------
   if (!(fwrite( &pfoc->cbBuffer, sizeof(ULONG), 1, pfoc->file ))) {
      ulRC = OPSTAT_FILEWRITEERROR;

// --------------------------------------------------------------------------
// write the remainder of the record
// --------------------------------------------------------------------------
   } else if (!(fwrite( pfoc->buffer, pfoc->cbBuffer, 1, pfoc->file ))) {
      ulRC = OPSTAT_FILEWRITEERROR;
   } /* endif */

// --------------------------------------------------------------------------
// report status of the write operation back to the application client window
// --------------------------------------------------------------------------
   WinPostMsg( pfoc->hwndClient, UM_READY, MPFROMP(pfoc), MPFROMLONG(ulRC));

   return MRFROMLONG(0L);
}

