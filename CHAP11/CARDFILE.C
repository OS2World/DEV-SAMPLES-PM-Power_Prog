// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      CARDFILE.C                                       mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Main Source File for CARDFILE/Chapter 11 Sample Program       บ
// บ                                                                               บ
// บ  NOTES:                                                                       บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#include <string.h>
#include "cardfile.h"
#include "shcommon.h"
#include "ctype.h"

// --------------------------------------------------------------------------
// Miscellaneous constant definitions used only in this source module.
// --------------------------------------------------------------------------
#define  APP_CLASS_NAME             "CARDFILE"
#define  TITLE_BAR_TEXT             "CARDFILE"
#define  HELP_WINDOW_TITLE          "Help for CARDFILE"
#define  HELPLIB_FILENAME           "cardfile.hlp"
#define  UNTITLED_FILENAME          ".Untitled"
#define  NUM_LETTERS                26

#define  INSTDATA_HWNDOBJ           0
#define  APP_WINDOW_DATA_SIZE       4

// --------------------------------------------------------------------------
// A global variable with the value FALSE, used to prevent compiler warnings
// in loops which are terminated from within.
// --------------------------------------------------------------------------
static BOOL  false = FALSE;

char  achLetterMap[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
ULONG aulAlpha2ID[NUM_LETTERS];
PCARDDATA apcdAlpha2Card[NUM_LETTERS];

PSZ      pszTickText[] = {"0","1","2","3","4","5","6","7","8","9","10"};

RECTL    grectlDefSize;
BOOL     gfDataChanged;
char     gszFileName[CCHMAXPATH];
BOOL     gfClosing;
HPOINTER ghPtr[ 10 ];

// --------------------------------------------------------------------------
// Main Client Window Procedure
// --------------------------------------------------------------------------
MRESULT APIENTRY AppWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT APIENTRY CardDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT APIENTRY CardDspProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------
// Message processing worker functions
// --------------------------------------------------------------------------
static MRESULT umDone( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT umObjhwnd( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT umReady( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmClose( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmCommand( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmControl( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmSize( HWND hwnd, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------
// User Command processing worker functions
// --------------------------------------------------------------------------
static MRESULT midFileNew( HWND hwnd );
static MRESULT midFileOpen( HWND hwnd );
static MRESULT midFileSave( HWND hwnd );
static MRESULT midFileSaveas( HWND hwnd );
static MRESULT midFileExit( HWND hwnd );
static MRESULT midCardCreate( HWND hwnd );
static MRESULT midCardEdit( HWND hwnd );
static MRESULT midCardRemove( HWND hwnd );
static MRESULT midHelpGeneral( HWND hwnd );
static MRESULT midHelpUsing( HWND hwnd );
static MRESULT midHelpProd( HWND hwnd );

// --------------------------------------------------------------------------
// WM_CONTROL notification processing functions
// --------------------------------------------------------------------------
static void bknPageSelected( HWND hwnd, MPARAM mp2 );

// --------------------------------------------------------------------------
// Card Dialog worker functions
// --------------------------------------------------------------------------
static MRESULT cardInitDlg( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT cardCommand( HWND hwnd, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------
// Miscellaneous functions
// --------------------------------------------------------------------------
static void GetItemText( HWND hwnd, USHORT id, USHORT flags, PSZ *ppszText );
static SHORT GetAlphaIndex( char ch );
static void FreeCardData( PCARDDATA pcd );
static ULONG InsertCard( HWND hwnd, PCARDDATA pcd );
static PSZ mystrtok( PSZ pszSearch, PSZ pszMatch );
static int mystrcmpi( PSZ pszStr1, PSZ pszStr2 );
static void ClearCardfile ( HWND hwnd );
static BOOL CheckForChanges( HWND hwnd );
static void EnableSaveMenu( HWND hwnd, BOOL fEnable );
static void EnableEditMenu( HWND hwnd, BOOL fEnable );
static void SetGlobalFilename( HWND hwnd, PSZ pszFilename );
static void RefreshTopPage( HWND hwndNotebook );
static void GetTabSize( HWND hwndNotebook, PUSHORT cx, PUSHORT cy );
static void DeleteCard( HWND hwndNotebook, ULONG ulPageID, PCARDDATA pcd );

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
// PARAMETERS:   VOID
//
// RETURNS:      int
//
// INTERNALS:    NONE
//
// *******************************************************************************
main(int argc, char *argv[], char *envp[])
{
   HAB      hab = (HAB)NULLHANDLE;
   HMQ      hmq = (HMQ)NULLHANDLE;
   QMSG     qmsg;
   HWND     hwndFrame = (HWND)NULLHANDLE;
   HWND     hwndClient = (HWND)NULLHANDLE;
   HWND     hwndHelp = (HWND)NULLHANDLE;
   HELPINIT hInit;
   SHORT    icon;

// --------------------------------------------------------------------------
// Frame has no accelerator table.
// --------------------------------------------------------------------------
   ULONG flCreateFlags = (FCF_STANDARD & ~ (FCF_ACCELTABLE | FCF_SHELLPOSITION));

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
// Now register the window class for the main application window
// --------------------------------------------------------------------------
      if (!WinRegisterClass( hab,                  // anchor block
                          APP_CLASS_NAME,          // class name
                          AppWndProc,              // window procedure
                          CS_SYNCPAINT,            // class style
                          APP_WINDOW_DATA_SIZE)) { // bytes of instance data
         // failed!
         DisplayMessages(NULLHANDLE, "Unable to Register Window Class",
                         MSG_ERROR);
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Finally, now the window can be created
// --------------------------------------------------------------------------
      hwndFrame = WinCreateStdWindow( HWND_DESKTOP,   // Parent window
                                      0l,             // Initial style
                                      &flCreateFlags, // Frame creation flags
                                      APP_CLASS_NAME, // Class name
                                      TITLE_BAR_TEXT, // Title bar text
                                      WS_VISIBLE,     // Client style
                                      NULLHANDLE,     // Resource in .EXE
                                      APPLICATION_ID, // Frame window id
                                      &hwndClient );  // Client window
      if (hwndFrame == (HWND)NULLHANDLE) {
         // no window - gotta leave.
         DisplayMessages(NULLHANDLE, "Error Creating Frame Window",
                         MSG_ERROR);
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Create a HELPINSTANCE for this application
// --------------------------------------------------------------------------
      memset( &hInit, 0, sizeof(HELPINIT));
      hInit.cb = sizeof( HELPINIT );
      hInit.phtHelpTable = (PHELPTABLE)MAKELONG( APPLICATION_ID, 0xffff );
      hInit.pszHelpWindowTitle = HELP_WINDOW_TITLE;
      hInit.pszHelpLibraryName = HELPLIB_FILENAME;

      hwndHelp = WinCreateHelpInstance( hab, &hInit );

// --------------------------------------------------------------------------
// Associate the help instance with the application window chain
// --------------------------------------------------------------------------
      if (hwndHelp == NULLHANDLE ||
          !WinAssociateHelpInstance( hwndHelp, hwndFrame )) {
         DisplayMessages(NULLHANDLE, "Help is not available",
                         MSG_ERROR);
      } /* endif */

// --------------------------------------------------------------------------
// Initiate object window for file I/O handling.
// --------------------------------------------------------------------------
     CreateIOObject( hwndClient );

// --------------------------------------------------------------------------
// Load pointers for card type
// --------------------------------------------------------------------------
   for (icon = 0; icon < 10; icon++) {
      ghPtr[icon] = WinLoadPointer( HWND_DESKTOP, NULLHANDLE, ID_ICON1 + icon );
   } /* endfor */

// --------------------------------------------------------------------------
// The initial client window size required to accomodate the dialog box
// in the notebook page was calculated during creation.  Now determine
// the required frame window size and set the initial size of the
// frame window accordingly.
// --------------------------------------------------------------------------
      WinCalcFrameRect( hwndFrame, &grectlDefSize, FALSE );
      WinSetWindowPos( hwndFrame,
                       NULLHANDLE,
                       0,
                       0,
                       grectlDefSize.xRight - grectlDefSize.xLeft,
                       grectlDefSize.yTop - grectlDefSize.yBottom,
                       SWP_SIZE );

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
      while (WinGetMsg(hab, &qmsg, NULLHANDLE, 0, 0 )) {
         WinDispatchMsg( hab, &qmsg );
      } /* endwhile */


   } while ( false ); /* enddo */

// --------------------------------------------------------------------------
// Clean up the help instance
// --------------------------------------------------------------------------
//   if (hwndHelp != NULLHANDLE) {
//      WinAssociateHelpInstance( NULLHANDLE, hwndFrame );
//      WinDestroyHelpInstance( hwndHelp );
//   } /* endif */

// --------------------------------------------------------------------------
// Clean up on exit - releasing the message queue and PM environment
// --------------------------------------------------------------------------
   if (hmq != (HMQ)NULLHANDLE) {
      WinDestroyMsgQueue( hmq );
   } /* endif */

   if (hab != (HAB)NULLHANDLE) {
      WinTerminate( hab );
   } /* endif */

   return(0);
}

// *******************************************************************************
// FUNCTION:     AppWndProc
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
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT APIENTRY AppWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   switch (msg) {
   case UM_DONE:              return umDone( hwnd, mp1, mp2 );
   case UM_OBJHWND:           return umObjhwnd( hwnd, mp1, mp2 );
   case UM_READY:             return umReady( hwnd, mp1, mp2 );
   case WM_CLOSE:             return wmClose( hwnd, mp1, mp2 );
   case WM_COMMAND:           return wmCommand( hwnd, mp1, mp2 );
   case WM_CONTROL:           return wmControl( hwnd, mp1, mp2 );
   case WM_CREATE:            return wmCreate( hwnd, mp1, mp2 );
   case WM_SIZE:              return wmSize( hwnd, mp1, mp2 );
   default:                   return WinDefWindowProc( hwnd, msg, mp1, mp2 );
   } /* endswitch */
}

// *******************************************************************************
// FUNCTION:     umDone
//
// FUNCTION USE: Worker function to process the UM_DONE message.
//
// DESCRIPTION:  Free the resources used for the file operation.
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
static MRESULT umDone( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PFILEOPCTL        pfoc = (PFILEOPCTL)mp1;

// --------------------------------------------------------------------------
// release memory allocated for processing the operation.
// --------------------------------------------------------------------------
   if (pfoc->buffer != NULL) free(pfoc->buffer);
   if (pfoc->pszFilename != NULL) free(pfoc->pszFilename);
   free( pfoc );

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     umObjhwnd
//
// FUNCTION USE: Worker function to process the UM_OBJHWND message.
//
// DESCRIPTION:  Save the object window handle in the instance data
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
static MRESULT umObjhwnd( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
// --------------------------------------------------------------------------
// save the object window handle in the instance data for later
// --------------------------------------------------------------------------
   WinSetWindowULong( hwnd, INSTDATA_HWNDOBJ, LONGFROMMP(mp1));

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     umReady
//
// FUNCTION USE: Worker function to process the UM_READY message.
//
// DESCRIPTION:  Either processes the next record in file or closes the file.
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
static MRESULT umReady( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PFILEOPCTL        pfoc = (PFILEOPCTL)mp1;
   PCARDDATA         pcd;
   USHORT            usQueryOrder = BKA_NEXT;
   HWND              hwndNotebook;
   HWND              hwndDlg;
   HWND              hwndFrame;
   PSZ               pszTemp;

   do {

// --------------------------------------------------------------------------
// handle processing errors
// --------------------------------------------------------------------------
      if (LONGFROMMP(mp2) != OPSTAT_NOERROR) {

         if (pfoc->ulOp == OPTYPE_READ) {
// --------------------------------------------------------------------------
// read error is most likely end of file - handle termination by
// destroying the "loading" dialog and reenabling the main window
// --------------------------------------------------------------------------
            hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
            WinEnableWindow( hwndFrame, TRUE );
            hwndDlg = WinWindowFromID( hwnd, ID_LOADING );
            WinDestroyWindow( hwndDlg );

// --------------------------------------------------------------------------
// tell the object window to close the file
// --------------------------------------------------------------------------
            WinPostMsg( pfoc->hwndServer, UM_CLOSE, mp1, MPFROMLONG(0L));

// --------------------------------------------------------------------------
// refresh the notebook card dialog to display the card for the top page
// --------------------------------------------------------------------------
            hwndNotebook = WinWindowFromID( hwnd, NOTEBOOK_ID );
            RefreshTopPage( hwndNotebook );
         } /* endif */

// --------------------------------------------------------------------------
// done with error processing - get to the function exit
// --------------------------------------------------------------------------
         break;
      } /* endif */

      if (pfoc->ulOp == OPTYPE_WRITE) {

// --------------------------------------------------------------------------
// doing a write operation - set up to read the next (or first) card from
// the notebook.
// --------------------------------------------------------------------------
         if (pfoc->ulUser == 0) usQueryOrder = BKA_FIRST;

// --------------------------------------------------------------------------
// get the next page from the notebook
// --------------------------------------------------------------------------
         hwndNotebook = WinWindowFromID( hwnd, NOTEBOOK_ID );
         pfoc->ulUser = LONGFROMMR(WinSendMsg( hwndNotebook, BKM_QUERYPAGEID,
                                               MPFROMLONG(pfoc->ulUser),
                                               MPFROM2SHORT( usQueryOrder, 0)));
         if (pfoc->ulUser == 0) {

// --------------------------------------------------------------------------
// if there are no more pages - terminate the operation by destroying the
// "saving" dialog and reenabling the main window
// --------------------------------------------------------------------------
            hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
            WinEnableWindow( hwndFrame, TRUE );
            WinPostMsg( pfoc->hwndServer, UM_CLOSE, mp1, MPFROMLONG(0L));
            hwndDlg = WinWindowFromID( hwnd, ID_SAVING );
            WinDestroyWindow( hwndDlg );

// --------------------------------------------------------------------------
// if the save occurred while the application was closing - issue the WM_QUIT
// --------------------------------------------------------------------------
            if (gfClosing) {
               WinPostMsg( hwnd, WM_QUIT, MPFROMLONG(0L), MPFROMLONG(0L));
            } /* endif */

         } else {
// --------------------------------------------------------------------------
// get the card data for this page
// --------------------------------------------------------------------------
            pcd = (PCARDDATA)WinSendMsg( hwndNotebook, BKM_QUERYPAGEDATA,
                                         MPFROMLONG(pfoc->ulUser),
                                         MPFROMLONG(0L));
            if (pcd == NULL) {
// --------------------------------------------------------------------------
// no card data - must be a tab page - post message to get the next page
// --------------------------------------------------------------------------
               WinPostMsg( hwnd, UM_READY, mp1, mp2 );
            } else {

// --------------------------------------------------------------------------
// run through each element of the card data and compute the number of bytes
// required to hold all the data with field separators between each element
// --------------------------------------------------------------------------
               if (pcd->pszNameL) pfoc->cbBuffer = strlen( pcd->pszNameL );
               pfoc->cbBuffer++;
               if (pcd->pszNameI) pfoc->cbBuffer += strlen( pcd->pszNameI );
               pfoc->cbBuffer++;
               if (pcd->pszCompany) pfoc->cbBuffer += strlen( pcd->pszCompany );
               pfoc->cbBuffer++;
               if (pcd->pszAddr1) pfoc->cbBuffer += strlen( pcd->pszAddr1 );
               pfoc->cbBuffer++;
               if (pcd->pszAddr2) pfoc->cbBuffer += strlen( pcd->pszAddr2 );
               pfoc->cbBuffer++;
               if (pcd->pszCity) pfoc->cbBuffer += strlen( pcd->pszCity );
               pfoc->cbBuffer++;
               if (pcd->pszState) pfoc->cbBuffer += strlen( pcd->pszState );
               pfoc->cbBuffer++;
               if (pcd->pszZip) pfoc->cbBuffer += strlen( pcd->pszZip );
               pfoc->cbBuffer++;
               if (pcd->pszTelephone) pfoc->cbBuffer += strlen( pcd->pszTelephone );
               pfoc->cbBuffer++;
               if (pcd->pszDob) pfoc->cbBuffer += strlen( pcd->pszDob );
               pfoc->cbBuffer++;
               if (pcd->pszAnn) pfoc->cbBuffer += strlen( pcd->pszAnn );
               pfoc->cbBuffer++;
               if (pcd->pszNotes) pfoc->cbBuffer += strlen( pcd->pszNotes);
               pfoc->cbBuffer++;
               pfoc->cbBuffer += 3;  // Type
               pfoc->cbBuffer += 3;  // Rating

// --------------------------------------------------------------------------
// Allocate a new buffer and copy each element from the card data to the
// the new buffer, separating each with a field separator.
// --------------------------------------------------------------------------
               if (pfoc->buffer != NULL) free( pfoc->buffer );
               pfoc->buffer = malloc( pfoc->cbBuffer );
               if (pcd->pszNameL) strcpy( pfoc->buffer, pcd->pszNameL );
               strcat( pfoc->buffer, "\x1c" );
               if (pcd->pszNameI) strcat( pfoc->buffer, pcd->pszNameI );
               strcat( pfoc->buffer, "\x1c" );
               if (pcd->pszCompany) strcat( pfoc->buffer, pcd->pszCompany );
               strcat( pfoc->buffer, "\x1c" );
               if (pcd->pszAddr1) strcat( pfoc->buffer, pcd->pszAddr1 );
               strcat( pfoc->buffer, "\x1c" );
               if (pcd->pszAddr2) strcat( pfoc->buffer, pcd->pszAddr2 );
               strcat( pfoc->buffer, "\x1c" );
               if (pcd->pszCity) strcat( pfoc->buffer, pcd->pszCity );
               strcat( pfoc->buffer, "\x1c" );
               if (pcd->pszState) strcat( pfoc->buffer, pcd->pszState );
               strcat( pfoc->buffer, "\x1c" );
               if (pcd->pszZip) strcat( pfoc->buffer, pcd->pszZip );
               strcat( pfoc->buffer, "\x1c" );
               if (pcd->pszTelephone)
                  strcat( pfoc->buffer, pcd->pszTelephone );
               strcat( pfoc->buffer, "\x1c" );
               if (pcd->pszDob) strcat( pfoc->buffer, pcd->pszDob );
               strcat( pfoc->buffer, "\x1c" );
               if (pcd->pszAnn) strcat( pfoc->buffer, pcd->pszAnn );
               strcat( pfoc->buffer, "\x1c" );
               if (pcd->pszNotes) strcat( pfoc->buffer, pcd->pszNotes );
               strcat( pfoc->buffer, "\x1c" );
               _itoa( pcd->usType,
                      &(((PSZ)(pfoc->buffer))[strlen(pfoc->buffer)]), 10 );
               strcat( pfoc->buffer, "\x1c" );
               _itoa( pcd->usRating,
                      &(((PSZ)(pfoc->buffer))[strlen(pfoc->buffer)]), 10 );

// --------------------------------------------------------------------------
// send the buffer to the object window to be written to the file
// --------------------------------------------------------------------------
               WinPostMsg( pfoc->hwndServer, UM_WRITE, mp1, MPFROMLONG(0L));
            } /* endif */
         } /* endif */
      } else {

// --------------------------------------------------------------------------
// a read operation is in progress - allocate memory to hold the card data
// and scan the file buffer to parse the card data elements.  Note that
// no string buffer is allocated for elements which have no data other than
// than the field separator.
// --------------------------------------------------------------------------
         if (pfoc->buffer != NULL) {
            pcd = (PCARDDATA)malloc(sizeof(CARDDATA));
            if (pcd == NULL) {
               free( pfoc->buffer );
               break;
            } /* endif */
            memset( (PVOID)pcd, 0, sizeof(CARDDATA));
            pszTemp = mystrtok( pfoc->buffer, "\x1c" );
            if (pszTemp && *pszTemp) {
               pcd->pszNameL = malloc(strlen(pszTemp)+1);
               if (pcd->pszNameL) strcpy(pcd->pszNameL, pszTemp );
            } /* endif */
            pszTemp = mystrtok( NULL, "\x1c" );
            if (pszTemp && *pszTemp) {
               pcd->pszNameI = malloc(strlen(pszTemp)+1);
               if (pcd->pszNameI) strcpy(pcd->pszNameI, pszTemp );
            } /* endif */
            pszTemp = mystrtok( NULL, "\x1c" );
            if (pszTemp && *pszTemp) {
               pcd->pszCompany = malloc(strlen(pszTemp)+1);
               if (pcd->pszCompany) strcpy(pcd->pszCompany, pszTemp );
            } /* endif */
            pszTemp = mystrtok( NULL, "\x1c" );
            if (pszTemp && *pszTemp) {
               pcd->pszAddr1 = malloc(strlen(pszTemp)+1);
               if (pcd->pszAddr1) strcpy(pcd->pszAddr1, pszTemp );
            } /* endif */
            pszTemp = mystrtok( NULL, "\x1c" );
            if (pszTemp && *pszTemp) {
               pcd->pszAddr2 = malloc(strlen(pszTemp)+1);
               if (pcd->pszAddr2) strcpy(pcd->pszAddr2, pszTemp );
            } /* endif */
            pszTemp = mystrtok( NULL, "\x1c" );
            if (pszTemp && *pszTemp) {
               pcd->pszCity = malloc(strlen(pszTemp)+1);
               if (pcd->pszCity) strcpy(pcd->pszCity, pszTemp );
            } /* endif */
            pszTemp = mystrtok( NULL, "\x1c" );
            if (pszTemp && *pszTemp) {
               pcd->pszState = malloc(strlen(pszTemp)+1);
               if (pcd->pszState) strcpy(pcd->pszState, pszTemp );
            } /* endif */
            pszTemp = mystrtok( NULL, "\x1c" );
            if (pszTemp && *pszTemp) {
               pcd->pszZip = malloc(strlen(pszTemp)+1);
               if (pcd->pszZip) strcpy(pcd->pszZip, pszTemp );
            } /* endif */
            pszTemp = mystrtok( NULL, "\x1c" );
            if (pszTemp && *pszTemp) {
               pcd->pszTelephone = malloc(strlen(pszTemp)+1);
               if (pcd->pszTelephone) strcpy(pcd->pszTelephone, pszTemp );
            } /* endif */
            pszTemp = mystrtok( NULL, "\x1c" );
            if (pszTemp && *pszTemp) {
               pcd->pszDob = malloc(strlen(pszTemp)+1);
               if (pcd->pszDob) strcpy(pcd->pszDob, pszTemp );
            } /* endif */
            pszTemp = mystrtok( NULL, "\x1c" );
            if (pszTemp && *pszTemp) {
               pcd->pszAnn = malloc(strlen(pszTemp)+1);
               if (pcd->pszAnn) strcpy(pcd->pszAnn, pszTemp );
            } /* endif */
            pszTemp = mystrtok( NULL, "\x1c" );
            if (pszTemp && *pszTemp) {
               pcd->pszNotes = malloc(strlen(pszTemp)+1);
               if (pcd->pszNotes) strcpy(pcd->pszNotes, pszTemp );
            } /* endif */
            pszTemp = mystrtok( NULL, "\x1c" );
            if (pszTemp && *pszTemp) {
               pcd->usType = atoi(pszTemp);
            } /* endif */
            pszTemp = mystrtok( NULL, "\x1c" );
            if (pszTemp && *pszTemp) {
               pcd->usRating = atoi(pszTemp);
            } /* endif */

            pcd->fs |= CARD_CREATED;

// --------------------------------------------------------------------------
// Add the card to the notebook;
// --------------------------------------------------------------------------
            InsertCard( hwnd, pcd );

// --------------------------------------------------------------------------
// release the file buffer
// --------------------------------------------------------------------------
            free( pfoc->buffer);
         } /* endif */

// --------------------------------------------------------------------------
// notify the object window to read the next record
// --------------------------------------------------------------------------
         WinPostMsg( pfoc->hwndServer, UM_READ, mp1, MPFROMLONG(0L));
      } /* endif */
   } while (false); /* enddo */

// --------------------------------------------------------------------------
// done - return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wmClose
//
// FUNCTION USE: Worker function to process the WM_CLOSE message.
//
// DESCRIPTION:  Handles cleanup before the application dies
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
static MRESULT wmClose( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
// --------------------------------------------------------------------------
// set flag to indicate that a close is in progress.  This enables a close
// after the current file is saved if necessary.
// --------------------------------------------------------------------------
   gfClosing = TRUE;

// --------------------------------------------------------------------------
// see if the data has changed since the last file save
// --------------------------------------------------------------------------
   CheckForChanges( hwnd );

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wmCommand
//
// FUNCTION USE: Worker function to process the WM_COMMAND message.
//
// DESCRIPTION:  Calls the appropriate worker for each individual command
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
static MRESULT wmCommand( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   switch (SHORT1FROMMP(mp1)) {
   case MID_FILENEW:          return midFileNew( hwnd );
   case MID_FILEOPEN:         return midFileOpen( hwnd );
   case MID_FILESAVE:         return midFileSave( hwnd );
   case MID_FILESAVEAS:       return midFileSaveas( hwnd );
   case MID_FILEEXIT:         return midFileExit( hwnd );
   case MID_CARDCREATE:       return midCardCreate( hwnd );
   case MID_CARDEDIT:         return midCardEdit( hwnd );
   case MID_CARDREMOVE:       return midCardRemove( hwnd );
   case MID_HELPGENERAL:      return midHelpGeneral( hwnd );
   case MID_HELPUSING:        return midHelpUsing( hwnd );
   case MID_HELPPROD:         return midHelpProd( hwnd );
   default:                   return MRFROMLONG(0L);
   } /* endswitch */
}

// *******************************************************************************
// FUNCTION:     wmControl
//
// FUNCTION USE: Worker function to process the WM_CONTROL message.
//
// DESCRIPTION:  Calls the appropriate worker for notifications from
//               the notebook control.
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
static MRESULT wmControl( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
// --------------------------------------------------------------------------
// only the page selected notification from the notebook is processed.
// --------------------------------------------------------------------------
   if (SHORT1FROMMP(mp1) == NOTEBOOK_ID) {
      switch (SHORT2FROMMP(mp1)) {
      case BKN_PAGESELECTED:       bknPageSelected( hwnd, mp2 );
      } /* endswitch */
   } /* endif */

// --------------------------------------------------------------------------
// return the appropriater reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wmCreate
//
// FUNCTION USE: Worker function to process the WM_CREATE message.
//
// DESCRIPTION:  Creates the notebook window as a child of the client window,
//               adds the basic alpha pages to the notebook.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  FALSE to indicate that window creation can continue.
//                        TRUE to indicate that and error occured.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND     hwndNotebook;
   HWND     hwndDlg;
   ULONG    flStyle;
   USHORT   fsPageStyle;
   ULONG    ulAlpha;
   char     szTabText[2];
   BOOKPAGEINFO   bpi;
   USHORT   cx;
   USHORT   cy;

// --------------------------------------------------------------------------
// Initialize the page information structure to set the major tab text and
// the window associated with each page.  This will be used later to set
// the data for each of the pages associated with a letter.
// --------------------------------------------------------------------------
   memset( (PVOID)&bpi, 0, sizeof(BOOKPAGEINFO));
   bpi.cb = sizeof(BOOKPAGEINFO);
   bpi.fl = BFA_MAJORTABTEXT | BFA_PAGEFROMHWND;
   bpi.cbMajorTab = 1;
   bpi.pszMajorTab = szTabText;

// --------------------------------------------------------------------------
// Set the style for the notebook
// --------------------------------------------------------------------------
   flStyle = BKS_BACKPAGESTR |      // backpage intersection at top/right
             BKS_MAJORTABTOP |      // major tabs on top
             BKS_TABTEXTCENTER |    // center text in tabs
             WS_VISIBLE ;           // show the window

// --------------------------------------------------------------------------
// Create the notebook control window
// --------------------------------------------------------------------------
   hwndNotebook = WinCreateWindow( hwnd,        // Parent is client window
                    WC_NOTEBOOK,                // Class is notebook control
                    NULL,                       // No window text
                    flStyle,                    // Notebook style
                    0l, 0l, 0l, 0l,             // Size rectangle
                    hwnd,                       // Owner is client window
                    HWND_TOP,                   // Place on top of siblings
                    NOTEBOOK_ID,                // Window/control identifier
                    NULL,                       // No control data
                    NULL );                     // No press params
   if (hwndNotebook != NULLHANDLE) {

// --------------------------------------------------------------------------
// Set the size of the tabs to accomodate a single character
// --------------------------------------------------------------------------
      GetTabSize( hwndNotebook, &cx, &cy );
      WinSendMsg( hwndNotebook, BKM_SETDIMENSIONS,
                  MPFROM2SHORT(cx,cy), MPFROMSHORT(BKA_MAJORTAB));

// --------------------------------------------------------------------------
// Create the dialog box to be associated with each of the pages
// --------------------------------------------------------------------------
      hwndDlg = WinLoadDlg( hwnd, hwnd, CardDspProc, NULLHANDLE,
                            DID_CARD, NULL );
      if (hwndDlg != NULLHANDLE) {

// --------------------------------------------------------------------------
// Get the size of the dialog and then determine the size of the notebook
// window required to display the entire dialog.  This is used later to
// establish the initial size of the application window.
// --------------------------------------------------------------------------
         WinQueryWindowRect( hwndDlg, &grectlDefSize );
         WinSendMsg( hwndNotebook, BKM_CALCPAGERECT,
                     MPFROMP(&grectlDefSize), MPFROMLONG(0L));

// --------------------------------------------------------------------------
// Save the window handle in the page info, set the page style.
// --------------------------------------------------------------------------
         bpi.hwndPage = hwndDlg;
         fsPageStyle = BKA_AUTOPAGESIZE | BKA_MAJOR;

// --------------------------------------------------------------------------
// Initialize the major tab text and then loop through each letter.
// --------------------------------------------------------------------------
         szTabText[1] = '\0';
         for (ulAlpha = 0; ulAlpha < NUM_LETTERS; ulAlpha++) {

// --------------------------------------------------------------------------
// Create a notebook page for the letter and format the tab text for the
// page.  Once the page is created, set the page info.
// --------------------------------------------------------------------------
            aulAlpha2ID[ulAlpha] = LONGFROMMR(
                   WinSendMsg( hwndNotebook, BKM_INSERTPAGE,
                               MPFROMLONG( 0l ),
                               MPFROM2SHORT( fsPageStyle, BKA_LAST )));
            szTabText[ 0 ] = achLetterMap[ulAlpha];
            WinSendMsg( hwndNotebook, BKM_SETPAGEINFO,
                        MPFROMLONG(aulAlpha2ID[ulAlpha]), MPFROMP( &bpi ));
         } /* endfor */
      } /* endif */

// --------------------------------------------------------------------------
// Call file new to establish untitled file name
// --------------------------------------------------------------------------
      midFileNew( hwnd );

// --------------------------------------------------------------------------
// Done - indicate that window creation may proceed.
// --------------------------------------------------------------------------
      return MRFROMLONG(FALSE);
   } else {

// --------------------------------------------------------------------------
// Error - abort window creation
// --------------------------------------------------------------------------
      return MRFROMLONG(TRUE);
   } /* endif */
}

// *******************************************************************************
// FUNCTION:     wmSize
//
// FUNCTION USE: Worker function to process the WM_SIZE message.
//
// DESCRIPTION:  Modifies the size of the notebook control to match the
//               size of the client window.
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
static MRESULT wmSize( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND     hwndNotebook;
   RECTL    rectl;

// --------------------------------------------------------------------------
// Get the notebook window handle
// --------------------------------------------------------------------------
   hwndNotebook = WinWindowFromID( hwnd, NOTEBOOK_ID );
   if (hwndNotebook != NULLHANDLE) {

// --------------------------------------------------------------------------
// Get the size and position of the client window
// --------------------------------------------------------------------------
      WinQueryWindowRect( hwnd, &rectl );

// --------------------------------------------------------------------------
// Modify the size and position of the notebook
// --------------------------------------------------------------------------
      WinSetWindowPos( hwndNotebook,
                       NULLHANDLE,
                       rectl.xLeft,
                       rectl.yBottom,
                       rectl.xRight,
                       rectl.yTop,
                       SWP_SIZE | SWP_MOVE );
   } /* endif */

   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     midFileNew
//
// FUNCTION USE: Worker function to process the FILE|NEW menu item.
//
// DESCRIPTION:  Erases the current card file data and starts a new unnamed file.
//
// PARAMETERS:   HWND     client window handle
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT midFileNew( HWND hwnd )
{
   HWND     hwndNotebook;

// --------------------------------------------------------------------------
// See if the user wants to save changes to the current data
// --------------------------------------------------------------------------
   if (CheckForChanges(hwnd)) {
      return MRFROMLONG(0L);
   } /* endif */

// --------------------------------------------------------------------------
// clear out the current data in the card file
// --------------------------------------------------------------------------
   ClearCardfile( hwnd );

// --------------------------------------------------------------------------
// no changes yet...
// --------------------------------------------------------------------------
   gfDataChanged = FALSE;

// --------------------------------------------------------------------------
// Set file name to the defined untitled file name
// --------------------------------------------------------------------------
   SetGlobalFilename( hwnd, UNTITLED_FILENAME );

// --------------------------------------------------------------------------
// disable the save menu item - can't save without a name
// --------------------------------------------------------------------------
   EnableSaveMenu( hwnd, FALSE );

// --------------------------------------------------------------------------
// Reselect the initial page
// --------------------------------------------------------------------------
   hwndNotebook = WinWindowFromID( hwnd, NOTEBOOK_ID );
   WinSendMsg( hwndNotebook, BKM_TURNTOPAGE,
               MPFROMLONG( aulAlpha2ID[0] ),
               MPFROMLONG(0L));

// --------------------------------------------------------------------------
// Return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     midFileOpen
//
// FUNCTION USE: Worker function to process the FILE|OPEN menu item.
//
// DESCRIPTION:  Raises a dialog to obtain a file name and then initiates
//               a conversation with the object window to read cards from
//               the selected file.
//
// PARAMETERS:   HWND     client window handle
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT midFileOpen( HWND hwnd )
{
   FILEDLG        fd;
   PFILEOPCTL     pfoc;
   HWND           hwndNotebook;
   HWND           hwndObj;
   HWND           hwndFrame;

// --------------------------------------------------------------------------
// allocate memory for the file dialog and operation control structures
// --------------------------------------------------------------------------
   pfoc = (PFILEOPCTL)malloc(sizeof(FILEOPCTL));
   memset( (PVOID)&fd, 0, sizeof(FILEDLG));
   memset( (PVOID)pfoc, 0, sizeof(FILEOPCTL));
   fd.cbSize = sizeof(FILEDLG);
   pfoc->cb = sizeof(FILEOPCTL);

// --------------------------------------------------------------------------
// set client window handle and operation type in operation control struct
// --------------------------------------------------------------------------
   pfoc->hwndClient = hwnd;
   pfoc->ulOp = OPTYPE_READ;

// --------------------------------------------------------------------------
// give the user an opportunity to save any changes to current data
// --------------------------------------------------------------------------
   if (CheckForChanges(hwnd)) {
      return MRFROMLONG(0L);
   } /* endif */

// --------------------------------------------------------------------------
// use the standard file dialog to get a file name to open
// --------------------------------------------------------------------------
   fd.fl = FDS_CENTER | FDS_OPEN_DIALOG;
   WinFileDlg( HWND_DESKTOP, hwnd, &fd );

   if (fd.lReturn == DID_OK) {
// --------------------------------------------------------------------------
// First - clear out the current data in the card file
// --------------------------------------------------------------------------
      ClearCardfile( hwnd );

// --------------------------------------------------------------------------
// Enable the save menu item since we now have a file to save
// --------------------------------------------------------------------------
      EnableSaveMenu( hwnd, TRUE );

// --------------------------------------------------------------------------
// Move the file name over, change the window title and set the file
// mode to read
// --------------------------------------------------------------------------
      pfoc->pszFilename = (PSZ)malloc( strlen(fd.szFullFile)+1);
      strcpy( pfoc->pszFilename, fd.szFullFile );
      SetGlobalFilename( hwnd, fd.szFullFile );
      pfoc->pszMode = "r+";

// --------------------------------------------------------------------------
// Fire off a message to the object window to start the load
// --------------------------------------------------------------------------
      hwndObj = WinQueryWindowULong( hwnd, INSTDATA_HWNDOBJ );
      WinPostMsg( hwndObj, UM_OPEN, MPFROMP(pfoc), MPFROMLONG(0L));

// --------------------------------------------------------------------------
// Put up a status dialog and disable the window during loading
// --------------------------------------------------------------------------
      WinLoadDlg( hwnd, hwnd, WinDefDlgProc, NULLHANDLE, ID_LOADING, NULL );
      hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
      WinEnableWindow( hwndFrame, FALSE );

// --------------------------------------------------------------------------
// Reselect the initial page
// --------------------------------------------------------------------------
      hwndNotebook = WinWindowFromID( hwnd, NOTEBOOK_ID );
      WinSendMsg( hwndNotebook, BKM_TURNTOPAGE,
                  MPFROMLONG( aulAlpha2ID[0] ),
                  MPFROMLONG(0L));

// --------------------------------------------------------------------------
// freshly opened file - the data has not changed yet
// --------------------------------------------------------------------------
      gfDataChanged = FALSE;
   } /* endif */

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     midFileSave
//
// FUNCTION USE: Worker function to process the FILE|SAVE menu item.
//
// DESCRIPTION:  Initiates a conversation with the object window to write the
//               current cards to the named file.
//
// PARAMETERS:   HWND     client window handle
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT midFileSave( HWND hwnd )
{
   PFILEOPCTL     pfoc;
   HWND           hwndObj;
   HWND           hwndFrame;

// --------------------------------------------------------------------------
// allocate memory for the operation control structure
// --------------------------------------------------------------------------
   pfoc = (PFILEOPCTL)malloc(sizeof(FILEOPCTL));
   memset( (PVOID)pfoc, 0, sizeof(FILEOPCTL));
   pfoc->cb = sizeof(FILEOPCTL);

// --------------------------------------------------------------------------
// Initialize the structure.  Converse with the client window for a WRITE
// operation.  Write to current file and open in write mode.
// --------------------------------------------------------------------------
   pfoc->hwndClient = hwnd;
   pfoc->ulOp = OPTYPE_WRITE;
   pfoc->pszFilename = (PSZ)malloc( strlen(gszFileName)+1);
   strcpy( pfoc->pszFilename, gszFileName );
   pfoc->pszMode = "w+";

// --------------------------------------------------------------------------
// Post message to the object window to start the operation
// --------------------------------------------------------------------------
   hwndObj = WinQueryWindowULong( hwnd, INSTDATA_HWNDOBJ );
   WinPostMsg( hwndObj, UM_OPEN, MPFROMP(pfoc), MPFROMLONG(0L));

// --------------------------------------------------------------------------
// Put up a status dialog and disable the window during loading
// --------------------------------------------------------------------------
   WinLoadDlg( hwnd, hwnd, WinDefDlgProc, NULLHANDLE, ID_SAVING, NULL );
   hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
   WinEnableWindow( hwndFrame, FALSE );

// --------------------------------------------------------------------------
// clear changed flag since all changes have been saved
// --------------------------------------------------------------------------
   gfDataChanged = FALSE;

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     midFileSaveAs
//
// FUNCTION USE: Worker function to process the FILE|SAVEAS menu item.
//
// DESCRIPTION:  Raises a dialog to obtain a file name and then initiates
//               a conversation with the object window to write the current
//               cards to the named file.
//
// PARAMETERS:   HWND     client window handle
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT midFileSaveas( HWND hwnd )
{
   FILEDLG        fd;
   PFILEOPCTL     pfoc;
   HWND           hwndObj;
   HWND           hwndFrame;

// --------------------------------------------------------------------------
// allocate memory for the file dialog and operation control structures
// --------------------------------------------------------------------------
   pfoc = (PFILEOPCTL)malloc(sizeof(FILEOPCTL));
   memset( (PVOID)&fd, 0, sizeof(FILEDLG));
   memset( (PVOID)pfoc, 0, sizeof(FILEOPCTL));
   fd.cbSize = sizeof(FILEDLG);
   pfoc->cb = sizeof(FILEOPCTL);

// --------------------------------------------------------------------------
// set client window handle and operation type in operation control struct
// --------------------------------------------------------------------------
   pfoc->hwndClient = hwnd;
   pfoc->ulOp = OPTYPE_WRITE;

// --------------------------------------------------------------------------
// set the file dialog structure for a save as file dialog, copy in the
// current file name (leave blank for untitled).  The get the user file
// name choice.
// --------------------------------------------------------------------------
   fd.fl = FDS_CENTER | FDS_SAVEAS_DIALOG;
   if (strcmp(gszFileName, UNTITLED_FILENAME)) {
      strcpy( fd.szFullFile, gszFileName );
   } else {
      strcpy( fd.szFullFile, "" );
   } /* endif */
   WinFileDlg( HWND_DESKTOP, hwnd, &fd );

   if (fd.lReturn == DID_OK) {
// --------------------------------------------------------------------------
// Enable the save menu item since we now have a file to save
// --------------------------------------------------------------------------
      EnableSaveMenu( hwnd, TRUE );

// --------------------------------------------------------------------------
// Move the file name over, change the window title and set the file
// mode to write
// --------------------------------------------------------------------------
      pfoc->pszFilename = (PSZ)malloc( strlen(fd.szFullFile)+1);
      strcpy( pfoc->pszFilename, fd.szFullFile );
      SetGlobalFilename( hwnd, fd.szFullFile );
      pfoc->pszMode = "w+";

// --------------------------------------------------------------------------
// Send message to the object window to start the operation
// --------------------------------------------------------------------------
      hwndObj = WinQueryWindowULong( hwnd, INSTDATA_HWNDOBJ );
      WinPostMsg( hwndObj, UM_OPEN, MPFROMP(pfoc), MPFROMLONG(0L));

// --------------------------------------------------------------------------
// Put up a status dialog and disable the window during loading
// --------------------------------------------------------------------------
      WinLoadDlg( hwnd, hwnd, WinDefDlgProc, NULLHANDLE, ID_SAVING, NULL );
      hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
      WinEnableWindow( hwndFrame, FALSE );

// --------------------------------------------------------------------------
// Clear changed flag since all changes have been saved.
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// Move the file name over, change the window title and set the file
// mode to write
// --------------------------------------------------------------------------
      gfDataChanged = FALSE;
   } /* endif */

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     midFileExit
//
// FUNCTION USE: Worker function to process the FILE|EXIT menu item.
//
// DESCRIPTION:  Close the application.
//
// PARAMETERS:   HWND     client window handle
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT midFileExit( HWND hwnd )
{
   HWND     hwndFrame;

// --------------------------------------------------------------------------
// Send a WM_CLOSE message to the frame window
// --------------------------------------------------------------------------
   hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
   WinSendMsg( hwndFrame, WM_CLOSE, MPFROMLONG(0L), MPFROMLONG(0L));

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     midCardCreate
//
// FUNCTION USE: Worker function to process the Card/Create menu item.
//
// DESCRIPTION:  Raises a dialog to obtain the information for the new card
//               and then adds the card to the application's buffers.
//
// PARAMETERS:   HWND     client window handle
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT midCardCreate( HWND hwnd )
{
   HWND        hwndNotebook;
   ULONG       ulPageID;
   ULONG       ulResult;
   CARDDATA    cd;
   PCARDDATA   pcd;

// --------------------------------------------------------------------------
// initialize the card structure
// --------------------------------------------------------------------------
   memset( (PVOID)&cd, 0, sizeof( CARDDATA ));
   cd.cb = sizeof( CARDDATA );

   do {

// --------------------------------------------------------------------------
// Get the notebook window handle
// --------------------------------------------------------------------------
      hwndNotebook = WinWindowFromID( hwnd, NOTEBOOK_ID );
      if (hwndNotebook == NULLHANDLE) break;

// --------------------------------------------------------------------------
// Get the data for the card
// --------------------------------------------------------------------------
      ulResult = WinDlgBox( HWND_DESKTOP, hwnd, CardDlgProc,
                            NULLHANDLE, DID_EDITCARD, (PVOID)&cd );

      if (ulResult != DID_OK || cd.pszNameL == NULL) break;
// --------------------------------------------------------------------------
// Allocate a data buffer for the new card and copy in the user's data.
// --------------------------------------------------------------------------
      pcd = (PCARDDATA)malloc(sizeof(CARDDATA));
      if (pcd == NULL) break;
      *pcd = cd;
      pcd->fs |= CARD_CREATED;

// --------------------------------------------------------------------------
// Add the card to the notebook;
// --------------------------------------------------------------------------
      ulPageID = InsertCard( hwnd, pcd );

// --------------------------------------------------------------------------
// Refresh the currently displayed page.
// --------------------------------------------------------------------------
      WinSendMsg( hwndNotebook, BKM_TURNTOPAGE,
                  MPFROMLONG(ulPageID), MPFROMLONG(0L));

// --------------------------------------------------------------------------
// Set the flag indicating that the data has changed.
// --------------------------------------------------------------------------
      gfDataChanged = TRUE;
   } while ( false ); /* enddo */

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     midCardEdit
//
// FUNCTION USE: Worker function to process the Card/Edit menu item.
//
// DESCRIPTION:  Raises a dialog to obtain the information for the new card
//               and then adds the card to the application's buffers.
//
// PARAMETERS:   HWND     client window handle
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT midCardEdit( HWND hwnd )
{
   ULONG       ulResult;
   HWND        hwndNotebook;
   ULONG       ulPageID;
   PCARDDATA   pcd;
   PSZ         pszNameL = NULL;
   PSZ         pszNameI = NULL;
   PSZ         pszCompany = NULL;

   do {

// --------------------------------------------------------------------------
// Get the notebook window handle
// --------------------------------------------------------------------------
      hwndNotebook = WinWindowFromID( hwnd, NOTEBOOK_ID );
      if (hwndNotebook == NULLHANDLE) break;

// --------------------------------------------------------------------------
// Get the currently displayed page number
// --------------------------------------------------------------------------
      ulPageID = LONGFROMMR(WinSendMsg( hwndNotebook, BKM_QUERYPAGEID,
                                        MPFROMLONG(0L),
                                        MPFROM2SHORT(BKA_TOP,0)));
      if (ulPageID == 0) break;

// --------------------------------------------------------------------------
// Get the page data for the currently displayed page
// --------------------------------------------------------------------------
      pcd = (PCARDDATA)WinSendMsg( hwndNotebook, BKM_QUERYPAGEDATA,
                                   MPFROMLONG( ulPageID ),
                                   MPFROMLONG( 0L ));
      if (pcd == NULL) break;

// --------------------------------------------------------------------------
// Let the user edit the page after saving current values of key fields
// --------------------------------------------------------------------------
      if (pcd->pszNameL) pszNameL = strdup(pcd->pszNameL);
      if (pcd->pszNameI) pszNameI = strdup(pcd->pszNameI);
      if (pcd->pszCompany) pszCompany = strdup(pcd->pszCompany);
      ulResult = WinDlgBox( HWND_DESKTOP, hwnd, CardDlgProc,
                            NULLHANDLE, DID_EDITCARD, pcd );
      if (ulResult != DID_OK) break;

// --------------------------------------------------------------------------
// Any changes to the keys?
// --------------------------------------------------------------------------
      if (mystrcmpi(pcd->pszNameL, pszNameL) ||
          mystrcmpi(pcd->pszNameI, pszNameI) ||
          mystrcmpi(pcd->pszCompany, pszCompany) ) {

// --------------------------------------------------------------------------
// changed - prevent window updates while cards are shuffled
// --------------------------------------------------------------------------
         WinEnableWindowUpdate(hwndNotebook, FALSE);

// --------------------------------------------------------------------------
// remove the current card
// --------------------------------------------------------------------------
         DeleteCard( hwndNotebook, ulPageID, pcd );

// --------------------------------------------------------------------------
// Re-add the card to the notebook in the correct order
// --------------------------------------------------------------------------
         ulPageID = InsertCard( hwnd, pcd );

// --------------------------------------------------------------------------
// allow window updates
// --------------------------------------------------------------------------
         WinEnableWindowUpdate(hwndNotebook, TRUE);

// --------------------------------------------------------------------------
// Refresh the currently displayed page - notebook will hide the window
// if a back page is added but no page change occurs.
// --------------------------------------------------------------------------
         WinSendMsg( hwndNotebook, BKM_TURNTOPAGE,
                     MPFROMLONG(ulPageID), MPFROMLONG(0L));

      } else {
// --------------------------------------------------------------------------
// no changes - just refresh the data
// --------------------------------------------------------------------------
         RefreshTopPage( hwndNotebook );
      } /* endif */
   } while ( false ); /* enddo */

// --------------------------------------------------------------------------
// free up temporary strings if allocated.
// --------------------------------------------------------------------------
   if (pszNameL) free(pszNameL);
   if (pszNameI) free(pszNameI);
   if (pszCompany) free(pszCompany);

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     midCardRemove
//
// FUNCTION USE: Worker function to process the Card/Remove menu item.
//
// DESCRIPTION:  Removes the currently displayed card from the card file.
//
// PARAMETERS:   HWND     client window handle
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT midCardRemove( HWND hwnd )
{
   ULONG       ulResult;
   HWND        hwndNotebook;
   ULONG       ulPageID;
   PCARDDATA   pcd;

   do {

// --------------------------------------------------------------------------
// Get the notebook window handle
// --------------------------------------------------------------------------
      hwndNotebook = WinWindowFromID( hwnd, NOTEBOOK_ID );
      if (hwndNotebook == NULLHANDLE) break;

// --------------------------------------------------------------------------
// Get the currently displayed page number
// --------------------------------------------------------------------------
      ulPageID = LONGFROMMR(WinSendMsg( hwndNotebook, BKM_QUERYPAGEID,
                                        MPFROMLONG(0L),
                                        MPFROM2SHORT(BKA_TOP,0)));
      if (ulPageID == 0) break;

// --------------------------------------------------------------------------
// Get the page data for the currently displayed page
// --------------------------------------------------------------------------
      pcd = (PCARDDATA)WinSendMsg( hwndNotebook, BKM_QUERYPAGEDATA,
                                   MPFROMLONG( ulPageID ),
                                   MPFROMLONG( 0L ));
      if (pcd == NULL) break;

// --------------------------------------------------------------------------
// Make sure this action is really desired
// --------------------------------------------------------------------------
      ulResult = WinMessageBox( HWND_DESKTOP, hwnd,
                                "Delete the top card?",
                                TITLE_BAR_TEXT,
                                DID_DELCARDMSG,
                                MB_ICONQUESTION | MB_YESNO | MB_HELP );
      if (ulResult == MBID_YES) {
// --------------------------------------------------------------------------
// Delete the card
// --------------------------------------------------------------------------
         DeleteCard( hwndNotebook, ulPageID, pcd );
// --------------------------------------------------------------------------
// Refresh the currently displayed page.
// --------------------------------------------------------------------------
         RefreshTopPage( hwndNotebook );

// --------------------------------------------------------------------------
// Clear out the memory associated with the card
// --------------------------------------------------------------------------
         FreeCardData( pcd );

// --------------------------------------------------------------------------
// Set the flag indicating that the data has changed.
// --------------------------------------------------------------------------
         gfDataChanged = TRUE;
      } /* endif */
   } while ( false ); /* enddo */

   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     midHelpGeneral
//
// FUNCTION USE: Worker function to process the Help/General help menu item.
//
// DESCRIPTION:  Displays the application's general help panel
//
// PARAMETERS:   HWND     client window handle
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT midHelpGeneral( HWND hwnd )
{
   HWND     hwndHelp;

// --------------------------------------------------------------------------
// Get the handle of the associated help instance window
// --------------------------------------------------------------------------
   hwndHelp = WinQueryHelpInstance( hwnd );
   if (hwndHelp != NULLHANDLE) {

// --------------------------------------------------------------------------
// Tell the help instance to display the general help panel
// --------------------------------------------------------------------------
      WinSendMsg( hwndHelp, HM_GENERAL_HELP, MPFROMLONG(0L), MPFROMLONG(0L));
   } /* endif */

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     midHelpUsing
//
// FUNCTION USE: Worker function to process the Help/Using help menu item.
//
// DESCRIPTION:  Displays the OS/2 Using help panel
//
// PARAMETERS:   HWND     client window handle
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT midHelpUsing( HWND hwnd )
{
   HWND     hwndHelp;

// --------------------------------------------------------------------------
// Get the handle of the associated help instance window
// --------------------------------------------------------------------------
   hwndHelp = WinQueryHelpInstance( hwnd );
   if (hwndHelp != NULLHANDLE) {

// --------------------------------------------------------------------------
// Tell the help instance to display the general help panel
// --------------------------------------------------------------------------
      WinSendMsg( hwndHelp, HM_DISPLAY_HELP, MPFROMLONG(0L), MPFROMLONG(0L));
   } /* endif */

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     midHelpProd
//
// FUNCTION USE: Worker function to process the Help/Product information menu item
//
// DESCRIPTION:  Raises the application's product information dialog.
//
// PARAMETERS:   HWND     client window handle
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT midHelpProd( HWND hwnd )
{
// --------------------------------------------------------------------------
// display the product information dialog
// --------------------------------------------------------------------------
   WinDlgBox( HWND_DESKTOP, hwnd, WinDefDlgProc, NULLHANDLE, PRODINFO_ID, NULL );

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     bknPageSelected
//
// FUNCTION USE: Worker function to process BKN_PAGESELECTED notfication
//               from the notebook.
//
// DESCRIPTION:  Retrieves the dialog window handle associated with the
//               page and the page data, then updates the dialog controls
//               with the data.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   pointer to page select notification structure
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void bknPageSelected( HWND hwnd, MPARAM mp2 )
{
   PPAGESELECTNOTIFY ppsn = (PPAGESELECTNOTIFY)PVOIDFROMMP(mp2);

// --------------------------------------------------------------------------
// Fill in the dialog window controls with the card data if it exists.
// --------------------------------------------------------------------------
   RefreshTopPage( ppsn->hwndBook );
}

// *******************************************************************************
// FUNCTION:     CardDspProc
//
// FUNCTION USE: Dialog procedure for the card display dialog
//
// DESCRIPTION:  Handles all messages sent to the card display dialog.
//               Prevents dismissal of dialog when WM_COMMAND received.
//
// PARAMETERS:   HWND     client window handle
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT APIENTRY CardDspProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
// --------------------------------------------------------------------------
// only the WM_COMMAND message is processed to prevent dialog dismissal
// --------------------------------------------------------------------------
   switch (msg) {
   case WM_COMMAND:              return MRFROMLONG(0L);
   default:                      return WinDefDlgProc( hwnd, msg, mp1, mp2 );
   } /* endswitch */
}


// *******************************************************************************
// FUNCTION:     CardDlgProc
//
// FUNCTION USE: Dialog procedure for the card create and edit functions
//
// DESCRIPTION:  Handles all messages sent to the card dialog
//
// PARAMETERS:   HWND     client window handle
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT APIENTRY CardDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   switch (msg) {
   case WM_INITDLG:              return cardInitDlg( hwnd, mp1, mp2 );
   case WM_COMMAND:              return cardCommand( hwnd, mp1, mp2 );
   default:                      return WinDefDlgProc( hwnd, msg, mp1, mp2 );
   } /* endswitch */
}

// *******************************************************************************
// FUNCTION:     cardInitDlg
//
// FUNCTION USE: Handles the WM_INITDLG message for the CARD dialog.
//
// DESCRIPTION:  Initializes the fields in the dialog.  All fields are cleared
//               when creating a card.  All fields are filled when editing
//               a card.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   first message parameter
//               MPARAM   pointer to a CARDDATA structure
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT cardInitDlg( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PCARDDATA      pcd = (PCARDDATA)PVOIDFROMMP(mp2);
   SHORT          row;
   SHORT          col;
   SHORT          tick;

// --------------------------------------------------------------------------
// Save off the card data structure - needed later
// --------------------------------------------------------------------------
   WinSetWindowULong( hwnd, QWL_USER, (ULONG)pcd );

// --------------------------------------------------------------------------
// Establish reasonable text length for names, etc.
// --------------------------------------------------------------------------
   WinSendDlgItemMsg( hwnd, DID_EDITNAMEI, EM_SETTEXTLIMIT,
                      MPFROMSHORT( 64 ), MPFROMLONG(0L));
   WinSendDlgItemMsg( hwnd, DID_EDITNAMEL, EM_SETTEXTLIMIT,
                      MPFROMSHORT( 64 ), MPFROMLONG(0L));
   WinSendDlgItemMsg( hwnd, DID_EDITCOMP, EM_SETTEXTLIMIT,
                      MPFROMSHORT( 64 ), MPFROMLONG(0L));
   WinSendDlgItemMsg( hwnd, DID_EDITADDR1, EM_SETTEXTLIMIT,
                      MPFROMSHORT( 64 ), MPFROMLONG(0L));
   WinSendDlgItemMsg( hwnd, DID_EDITADDR2, EM_SETTEXTLIMIT,
                      MPFROMSHORT( 64 ), MPFROMLONG(0L));
   WinSendDlgItemMsg( hwnd, DID_EDITCITY, EM_SETTEXTLIMIT,
                      MPFROMSHORT( 64 ), MPFROMLONG(0L));

// --------------------------------------------------------------------------
// Limit state to two characters - no international stuff for now, though
// this should probably be added
// --------------------------------------------------------------------------
   WinSendDlgItemMsg( hwnd, DID_EDITSTATE, EM_SETTEXTLIMIT,
                      MPFROMSHORT( 2 ), MPFROMLONG(0L));

// --------------------------------------------------------------------------
// Limit ZIPCODE to 10 characters - no international stuff for now, though
// this should probably be added
// --------------------------------------------------------------------------
   WinSendDlgItemMsg( hwnd, DID_EDITZIP, EM_SETTEXTLIMIT,
                      MPFROMSHORT( 10 ), MPFROMLONG(0L));

// --------------------------------------------------------------------------
// Limit phone number to 20 characters.  This allows for entry in the
// (AAA)PPP-NNNN XXXXXX format - still no international support.
// --------------------------------------------------------------------------
   WinSendDlgItemMsg( hwnd, DID_EDITPHONE, EM_SETTEXTLIMIT,
                      MPFROMSHORT( 20 ), MPFROMLONG(0L));

// --------------------------------------------------------------------------
// Initialize the ICONS in the value set control
// --------------------------------------------------------------------------
   for (row=1; row<3; row++) {
      for (col=1; col<6; col++) {
         WinSendDlgItemMsg( hwnd, DID_EDITTYPE, VM_SETITEMATTR,
                            MPFROM2SHORT(row,col),
                            MPFROM2SHORT(VIA_ICON, TRUE ));
         WinSendDlgItemMsg( hwnd, DID_EDITTYPE, VM_SETITEM,
                            MPFROM2SHORT(row,col),
                            MPFROMLONG((LONG)ghPtr[(row-1) * 5 + (col-1)]));
      } /* endfor */
   } /* endfor */

// --------------------------------------------------------------------------
// Initialize the slider
// --------------------------------------------------------------------------
   for (tick = 0; tick < 11; tick++) {
      WinSendDlgItemMsg( hwnd, DID_EDITRATING, SLM_SETTICKSIZE,
                         MPFROM2SHORT( tick, 10), MPFROMLONG(0L));
      printf( "%s\n", pszTickText[tick] );
      WinSendDlgItemMsg( hwnd, DID_EDITRATING, SLM_SETSCALETEXT,
                         MPFROMSHORT( tick ), MPFROMP( pszTickText[tick]));
   } /* endfor */

   if (pcd->fs & CARD_CREATED) {
// --------------------------------------------------------------------------
// Initialize the data in the fields from the existing record
// --------------------------------------------------------------------------
      WinSetDlgItemText( hwnd, DID_EDITNAMEL, pcd->pszNameL );
      WinSetDlgItemText( hwnd, DID_EDITNAMEI, pcd->pszNameI );
      WinSetDlgItemText( hwnd, DID_EDITCOMP, pcd->pszCompany );
      WinSetDlgItemText( hwnd, DID_EDITADDR1, pcd->pszAddr1 );
      WinSetDlgItemText( hwnd, DID_EDITADDR2, pcd->pszAddr2 );
      WinSetDlgItemText( hwnd, DID_EDITCITY, pcd->pszCity );
      WinSetDlgItemText( hwnd, DID_EDITSTATE, pcd->pszState );
      WinSetDlgItemText( hwnd, DID_EDITZIP, pcd->pszZip );
      WinSetDlgItemText( hwnd, DID_EDITPHONE, pcd->pszTelephone );
      WinSetDlgItemText( hwnd, DID_EDITDOB, pcd->pszDob );
      WinSetDlgItemText( hwnd, DID_EDITANN, pcd->pszAnn );
      WinSetDlgItemText( hwnd, DID_EDITMLE, pcd->pszNotes );
      WinSendDlgItemMsg( hwnd, DID_EDITTYPE, VM_SELECTITEM,
                         MPFROM2SHORT( (pcd->usType / 5 ) + 1,
                                       (pcd->usType % 5 ) + 1),
                         MPFROMLONG( 0L ));
      WinSendDlgItemMsg( hwnd, DID_EDITRATING, SLM_SETSLIDERINFO,
                         MPFROM2SHORT( SMA_SLIDERARMPOSITION,
                                       SMA_INCREMENTVALUE ),
                         MPFROMSHORT( pcd->usRating ));
   } /* endif */

// --------------------------------------------------------------------------
// Done - indicate that no focus change occurred.
// --------------------------------------------------------------------------
   return MRFROMLONG( FALSE );
}

// *******************************************************************************
// FUNCTION:     cardCommand
//
// FUNCTION USE: Handles WM_COMMAND messages for the CARD dialog.
//
// DESCRIPTION:  If OK command, copies dialog data back to the structure passed
//               and dismisses the dialog with a TRUE.  Otherwise, dismisses the
//               dialog with a FALSE.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   first message parameter
//               MPARAM   pointer to a CARDDATA structure
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT cardCommand( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PCARDDATA      pcd;
   USHORT         fs;
   ULONG          ulValItem;

   switch (SHORT1FROMMP(mp1)) {
   case DID_OK:

// --------------------------------------------------------------------------
// ready to close and update - get the pointer to the structure
// --------------------------------------------------------------------------
      pcd = (PCARDDATA)WinQueryWindowULong( hwnd, QWL_USER );
      if (pcd != NULL) {
         fs = pcd->fs;     // optimize a bit

// --------------------------------------------------------------------------
// Get the data for the entry and edit fields into the structure
// --------------------------------------------------------------------------
         GetItemText( hwnd, DID_EDITNAMEI, fs, &pcd->pszNameI );
         GetItemText( hwnd, DID_EDITNAMEL, fs, &pcd->pszNameL );
         GetItemText( hwnd, DID_EDITCOMP, fs, &pcd->pszCompany );
         GetItemText( hwnd, DID_EDITADDR1, fs, &pcd->pszAddr1 );
         GetItemText( hwnd, DID_EDITADDR2, fs, &pcd->pszAddr2 );
         GetItemText( hwnd, DID_EDITCITY, fs, &pcd->pszCity );
         GetItemText( hwnd, DID_EDITSTATE, fs, &pcd->pszState );
         GetItemText( hwnd, DID_EDITZIP, fs, &pcd->pszZip );
         GetItemText( hwnd, DID_EDITPHONE, fs, &pcd->pszTelephone );
         GetItemText( hwnd, DID_EDITDOB, fs, &pcd->pszDob );
         GetItemText( hwnd, DID_EDITANN, fs, &pcd->pszAnn );
         GetItemText( hwnd, DID_EDITMLE, fs, &pcd->pszNotes );

// --------------------------------------------------------------------------
// get the selected value set item
// --------------------------------------------------------------------------
         ulValItem = LONGFROMMR(WinSendDlgItemMsg( hwnd, DID_EDITTYPE,
                                VM_QUERYSELECTEDITEM,
                                MPFROMLONG(0L), MPFROMLONG(0L)));
         pcd->usType = (LOUSHORT(ulValItem)-1) * 5;
         pcd->usType += HIUSHORT(ulValItem) - 1;

// --------------------------------------------------------------------------
// get the slider value
// --------------------------------------------------------------------------
         pcd->usRating = SHORT1FROMMR( WinSendDlgItemMsg( hwnd, DID_EDITRATING,
                                       SLM_QUERYSLIDERINFO,
                                       MPFROM2SHORT( SMA_SLIDERARMPOSITION,
                                                     SMA_INCREMENTVALUE ),
                                       MPFROMLONG( 0L )));

// --------------------------------------------------------------------------
// dismiss the dialog with TRUE to indicate successful modification
// --------------------------------------------------------------------------
         WinDismissDlg( hwnd, TRUE );

      } else {
// --------------------------------------------------------------------------
// dismiss the dialog with FALSE to indicate error
// --------------------------------------------------------------------------
         WinDismissDlg( hwnd, FALSE );
      } /* endif */
      break;

   case DID_CANCEL:
// --------------------------------------------------------------------------
// user canceled - dismiss dialog with FALSE
// --------------------------------------------------------------------------
      WinDismissDlg( hwnd, FALSE );
      break;
   } /* endswitch */

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     GetItemText
//
// FUNCTION USE: Retrieves the text from a dialog entry field.
//
//
// DESCRIPTION:  Allocates memory for the text, retreives the text from the entry
//               field, and stores the memory pointer back into buffer provided by
//               caller.  Free memory pointed to by ppszText if non-NULL
//
// PARAMETERS:   HWND     dialog window handle
//               USHORT   control id to examine
//               USHORT   flags - edit or create?
//               PSZ *    pointer to location where pointer to existing text
//                        is stored.  This area will be freed and a new
//                        area allocated if in edit mode.  The new pointer is
//                        sent back in the location pointed to by ppszText
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void GetItemText( HWND hwnd, USHORT id, USHORT flags, PSZ *ppszText )
{
   PSZ      pszText = NULL;
   ULONG    cbLen;

// --------------------------------------------------------------------------
// Get the current length of the item
// --------------------------------------------------------------------------
   cbLen = (ULONG)WinQueryDlgItemTextLength( hwnd, id );
   if (cbLen) {

// --------------------------------------------------------------------------
// Allocate memory for the text
// --------------------------------------------------------------------------
      pszText = (PSZ)malloc( ++cbLen );
      if (pszText != NULL) {

// --------------------------------------------------------------------------
// Get the text from the control
// --------------------------------------------------------------------------
         cbLen = WinQueryDlgItemText( hwnd, id, cbLen, pszText );
         if (cbLen == 0 ) {

// --------------------------------------------------------------------------
// Clear the allocated text if an error occurred.
// --------------------------------------------------------------------------
            free(pszText);
            pszText = NULL;
         } /* endif */
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// Release existing text if this is an edit and text existed
// --------------------------------------------------------------------------
   if ((flags & CARD_CREATED) && ppszText && *ppszText) {
      free( *ppszText );
   } /* endif */

// --------------------------------------------------------------------------
// Fill in the new text value
// --------------------------------------------------------------------------
   *ppszText = pszText;
}

// *******************************************************************************
// FUNCTION:     GetAlphaIndex
//
// FUNCTION USE:
//
// DESCRIPTION:
//
// PARAMETERS:   HWND     dialog window handle
//               USHORT   control id to examine
//               USHORT   flags - edit or create?
//               PSZ *    pointer to location where pointer to existing text
//                        is stored.  This area will be freed and a new
//                        area allocated if in edit mode.  The new pointer is
//                        sent back in the location pointed to by ppszText
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static SHORT GetAlphaIndex( char ch )
{
   SHORT    index;

// --------------------------------------------------------------------------
// Scan the alphabet to find the index for a letter
// --------------------------------------------------------------------------
   for (index = 0; index < NUM_LETTERS; index++) {
      if (achLetterMap[index] == toupper(ch)) break;
   } /* endfor */

// --------------------------------------------------------------------------
// return the letter value found
// --------------------------------------------------------------------------
   return index;
}

// *******************************************************************************
// FUNCTION:     FreeCardData
//
// FUNCTION USE: Free all memory associated with a card
//
// DESCRIPTION:
//
// PARAMETERS:   PCARDDATA pointer to the card data to be freed.
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void FreeCardData( PCARDDATA pcd )
{
// --------------------------------------------------------------------------
// free memory for the strings in the structure
// --------------------------------------------------------------------------
   if (pcd->pszNameL) free( pcd->pszNameL );
   if (pcd->pszNameI) free( pcd->pszNameI );
   if (pcd->pszCompany) free( pcd->pszCompany );
   if (pcd->pszAddr1) free( pcd->pszAddr1 );
   if (pcd->pszAddr2) free( pcd->pszAddr2 );
   if (pcd->pszCity) free( pcd->pszCity );
   if (pcd->pszState) free( pcd->pszState );
   if (pcd->pszZip) free( pcd->pszZip );
   if (pcd->pszTelephone) free( pcd->pszTelephone );
   if (pcd->pszDob) free( pcd->pszDob );
   if (pcd->pszAnn) free( pcd->pszAnn );

// --------------------------------------------------------------------------
// free the card data
// --------------------------------------------------------------------------
   free(pcd);
}

// *******************************************************************************
// FUNCTION:     InsertCard
//
// FUNCTION USE: Add a new card to the database
//
// DESCRIPTION:
//
// PARAMETERS:   HWND      page window dialog box handle
//               PCARDDATA pointer to the card data to be freed.
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static ULONG InsertCard( HWND hwnd, PCARDDATA pcd )
{
   HWND           hwndNotebook;
   ULONG          alpha;
   ULONG          ulPageID;
   ULONG          ulNewCardPage = 0L;
   PCARDDATA      ppgcd = NULL;
   BOOKPAGEINFO   bpi;
   USHORT         fsPageStyle = BKA_AUTOPAGESIZE;
   int            iCmpRes;

   do {
// --------------------------------------------------------------------------
// get the handle to the notebook window
// --------------------------------------------------------------------------
      hwndNotebook = WinWindowFromID( hwnd, NOTEBOOK_ID );
      if (hwndNotebook == NULLHANDLE ) break;

// --------------------------------------------------------------------------
// Operation was not aborted and data was entered - determine the letter
// tab (notebook section) where the new card belongs.
// --------------------------------------------------------------------------
      alpha = GetAlphaIndex( pcd->pszNameL[0] );
      if (alpha >= NUM_LETTERS) break;

// --------------------------------------------------------------------------
// Get the current page data for the first page of the section
// --------------------------------------------------------------------------
      ulPageID = aulAlpha2ID[alpha];
      if ( ulPageID ) {
         ppgcd = (PCARDDATA)WinSendMsg( hwndNotebook,
                                        BKM_QUERYPAGEDATA,
                                        MPFROMLONG(ulPageID),
                                        MPFROMLONG(0l));
      } /* endif */
      if (ppgcd == NULL) {
// --------------------------------------------------------------------------
// No page data is currently associated with the page, save the new data
// as the page data.
// --------------------------------------------------------------------------
         WinSendMsg( hwndNotebook, BKM_SETPAGEDATA,
                     MPFROMLONG(ulPageID), MPFROMP(pcd));
         ulNewCardPage = ulPageID;

      } else {
// --------------------------------------------------------------------------
// Starting with the tab page - scan the pages in the current section (and
// possibly the first page of the next section) to determine where the new
// card fits.
// --------------------------------------------------------------------------
         while (ppgcd != NULL) {

// --------------------------------------------------------------------------
// Is the new card less than, equal to, or greater than the current card.
// The keys are LAST NAME, FIRST NAME & INITIAL, COMPANY NAME.
// --------------------------------------------------------------------------
            iCmpRes = mystrcmpi(pcd->pszNameL, ppgcd->pszNameL );
            if (iCmpRes == 0) {
               iCmpRes = mystrcmpi(pcd->pszNameI, ppgcd->pszNameI );
            } /* endif */
            if (iCmpRes == 0) {
               iCmpRes = mystrcmpi(pcd->pszCompany, ppgcd->pszCompany);
            } /* endif */

// --------------------------------------------------------------------------
// New less than current - swap new into current page and move current to a
// new page.
// --------------------------------------------------------------------------
            if (iCmpRes < 0) {
               WinSendMsg( hwndNotebook, BKM_SETPAGEDATA,
                           MPFROMLONG(ulPageID), MPFROMP(pcd));
               pcd = ppgcd;
               ulNewCardPage = ulPageID;
               break;

// --------------------------------------------------------------------------
// New same as current - add new after current
// --------------------------------------------------------------------------
            } else if (iCmpRes == 0 ) {
               break;

// --------------------------------------------------------------------------
// New greater than current - check the next one
// --------------------------------------------------------------------------
            } else {
               ulPageID = (ULONG)WinSendMsg( hwndNotebook,
                                             BKM_QUERYPAGEID,
                                             MPFROMLONG(ulPageID),
                                             MPFROM2SHORT(BKA_NEXT,0));
               if (ulPageID != 0) {
                  ppgcd = (PCARDDATA)WinSendMsg( hwndNotebook,
                                                 BKM_QUERYPAGEDATA,
                                                 MPFROMLONG(ulPageID),
                                                 MPFROMLONG(0l));
               } /* endif */
            } /* endif */
         } /* endwhile */

         if (ulPageID == 0) {
// --------------------------------------------------------------------------
// Hit the end of the card chain without finding a card greater than the
// new card - back up to the last available page.
// --------------------------------------------------------------------------
            ulPageID = LONGFROMMR(WinSendMsg( hwndNotebook,
                                              BKM_QUERYPAGEID,
                                              MPFROMLONG(0L),
                                              MPFROM2SHORT(BKA_LAST,0)));
         } /* endif */
         if (ppgcd == NULL) {
// --------------------------------------------------------------------------
// Hit the end of the section without finding a card greater than the
// new card - get the last card of the section
// --------------------------------------------------------------------------
            ulPageID = LONGFROMMR(WinSendMsg( hwndNotebook,
                                              BKM_QUERYPAGEID,
                                              MPFROMLONG(ulPageID),
                                               MPFROM2SHORT(BKA_PREV,0)));
         } /* endif */

// --------------------------------------------------------------------------
// A page has to be created - fill in the page information structure with
// the page dialog window handle and the new card data
// --------------------------------------------------------------------------
         memset( (PVOID)&bpi, 0, sizeof(BOOKPAGEINFO));
         bpi.cb = sizeof(BOOKPAGEINFO);
         bpi.fl = BFA_PAGEDATA | BFA_PAGEFROMHWND;
         bpi.hwndPage = (HWND)WinSendMsg( hwndNotebook,
                                          BKM_QUERYPAGEWINDOWHWND,
                                          MPFROMLONG(ulPageID),
                                          MPFROMLONG(0l));
         bpi.ulPageData = (ULONG)pcd;

// --------------------------------------------------------------------------
// Insert the new page; set the window handle and page data
// --------------------------------------------------------------------------
         ulPageID = LONGFROMMR( WinSendMsg( hwndNotebook,
                                     BKM_INSERTPAGE,
                                     MPFROMLONG( ulPageID ),
                                     MPFROM2SHORT( fsPageStyle, BKA_NEXT )));
         WinSendMsg( hwndNotebook, BKM_SETPAGEINFO,
                     MPFROMLONG(ulPageID), MPFROMP( &bpi ));
         if (ulNewCardPage == 0) ulNewCardPage = ulPageID;
      } /* endif */
   } while (false); /* enddo */

// --------------------------------------------------------------------------
// return the page ID of the new page
// --------------------------------------------------------------------------
   return ulNewCardPage;
}

// *******************************************************************************
// FUNCTION:     mystrtok
//
// FUNCTION USE: tokenizes a string
//
// DESCRIPTION:  same as strtok but does not skip leading/adjacent
//               delimiters
//
// PARAMETERS:   PSZ       string to search/NULL to continue
//               PSZ       chars to search
//
// RETURNS:      PSZ       pointer to token
//
// INTERNALS:    NONE
//
// *******************************************************************************
static PSZ mystrtok( PSZ pszSearch, PSZ pszMatch )
{
   static   PSZ  pszSearch1 = NULL;

// --------------------------------------------------------------------------
// Use new string if passed - else continue with the old string
// --------------------------------------------------------------------------
   if (pszSearch) {
      pszSearch1 = pszSearch;
   } else {
      pszSearch = pszSearch1;
   } /* endif */

   if (pszSearch) {
// --------------------------------------------------------------------------
// find a match character in the string
// --------------------------------------------------------------------------
      pszSearch1 = strpbrk(pszSearch, pszMatch );

// --------------------------------------------------------------------------
// if a match was found - terminate the string
// --------------------------------------------------------------------------
      if (pszSearch1) {
         *(pszSearch1++) = '\0';
      } /*  endif */
   } /* endif */

// --------------------------------------------------------------------------
// return pointer to first character of string
// --------------------------------------------------------------------------
   return pszSearch;
}

// *******************************************************************************
// FUNCTION:     mystrcmpi
//
// FUNCTION USE: compares 2 strings
//
// DESCRIPTION:  same as strcmpi but handles NULL string pointers
//
// PARAMETERS:   PSZ       string 1
//               PSZ       string 2
//
// RETURNS:      int       < 0, string1 < string2
//                         = 0, string1 = string2
//                         > 0, string1 > string2
//
// INTERNALS:    NONE
//
// *******************************************************************************
static int mystrcmpi( PSZ pszStr1, PSZ pszStr2 )
{

// --------------------------------------------------------------------------
// Convert NULL pointers to pointers to NULL string
// --------------------------------------------------------------------------
   if ( !pszStr1 ) pszStr1 = "";
   if ( !pszStr2 ) pszStr2 = "";

// --------------------------------------------------------------------------
// return the result of comparing the strings
// --------------------------------------------------------------------------
   return strcmpi(pszStr1, pszStr2 );
}

// *******************************************************************************
// FUNCTION:     ClearCardfile
//
// FUNCTION USE: Erases all data in the card file.
//
// DESCRIPTION:  Scans the pages in the dialog and delete the CARDDATA
//               attached to each page.  Removes the page unless it has
//               a MAJOR tab.  Note that for large files this operation
//               could consume a bit of time and should probably be split
//               into several iterations.
//
// PARAMETERS:   HWND      handle of the application client window
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void ClearCardfile ( HWND hwnd )
{
   HWND        hwndNotebook;
   ULONG       ulPageID;
   ULONG       ulTabPageID;
   PCARDDATA   pcd;
   USHORT      usOrder;
   USHORT      usStyle;

// --------------------------------------------------------------------------
// start with no tab page and then get the first page
// --------------------------------------------------------------------------
   ulTabPageID = 0;
   usOrder = BKA_FIRST;
   hwndNotebook = WinWindowFromID( hwnd, NOTEBOOK_ID );
   do {

// --------------------------------------------------------------------------
// Get the page id of the next(first) page
// --------------------------------------------------------------------------
      ulPageID = LONGFROMMR(WinSendMsg( hwndNotebook, BKM_QUERYPAGEID,
                            MPFROMLONG(ulTabPageID),
                            MPFROM2SHORT(usOrder, 0 )));

// --------------------------------------------------------------------------
// read next from now on
// --------------------------------------------------------------------------
      usOrder = BKA_NEXT;

      if (ulPageID != 0) {

// --------------------------------------------------------------------------
// Get the style and page data for the page
// --------------------------------------------------------------------------
         usStyle = SHORT1FROMMR( WinSendMsg( hwndNotebook, BKM_QUERYPAGESTYLE,
                                MPFROMLONG(ulPageID), MPFROMLONG(0)));
         pcd = (PCARDDATA)WinSendMsg( hwndNotebook, BKM_QUERYPAGEDATA,
                                      MPFROMLONG(ulPageID), MPFROMLONG(0L));

// --------------------------------------------------------------------------
// release the page data memory and clear pointer in the notebook page
// --------------------------------------------------------------------------
         if (pcd != NULL) {
            FreeCardData( pcd );
            WinSendMsg( hwndNotebook, BKM_SETPAGEDATA, MPFROMLONG(ulPageID),
                                      MPFROMP(NULL));
         } /* endif */

// --------------------------------------------------------------------------
// delete the page if not a major tab page
// --------------------------------------------------------------------------
         if (!(usStyle & BKA_MAJOR)) {
            WinSendMsg( hwndNotebook, BKM_DELETEPAGE, MPFROMLONG(ulPageID),
                                      MPFROMSHORT( BKA_SINGLE ));
         } else {

// --------------------------------------------------------------------------
// otherwise - set the current major tab page
// --------------------------------------------------------------------------
            ulTabPageID = ulPageID;
         } /* endif */
      } /* endif */
   } while ( ulPageID ); /* enddo */
}

// *******************************************************************************
// FUNCTION:     CheckForChanges
//
// FUNCTION USE: Determines whether changes should be saved.
//
// DESCRIPTION:  Examines the global changes flag to determine if the data
//               has been modified; if so, a dialog is present which allows
//               the user to determine the disposition of the changes.
//
// PARAMETERS:   HWND      handle of the application client window
//
// RETURNS:      BOOL      TRUE - user requested cancellation of the current
//                         operation; FALSE - continue current operation.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static BOOL CheckForChanges( HWND hwnd )
{
   BOOL  fCancelOp = FALSE;
   ULONG ulRC = DID_DISCARDCHANGES;

// --------------------------------------------------------------------------
// check the changed flag
// --------------------------------------------------------------------------
   if (gfDataChanged) {

// --------------------------------------------------------------------------
// inform the user that the file has changed
// --------------------------------------------------------------------------
      ulRC = WinDlgBox( HWND_DESKTOP, hwnd, WinDefDlgProc, NULLHANDLE,
                        CHANGED_ID, NULL );
      if (ulRC == DID_SAVECHANGES) {

// --------------------------------------------------------------------------
// save the file if requested by user
// --------------------------------------------------------------------------
         midFileSaveas( hwnd );
      } else if (ulRC == DID_CANCEL ) {

// --------------------------------------------------------------------------
// or cancel the operation
// --------------------------------------------------------------------------
         fCancelOp = TRUE;
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// on discard - if exiting, post message to quit applicaiton
// --------------------------------------------------------------------------
   if (ulRC == DID_DISCARDCHANGES && gfClosing) {
         WinPostMsg( hwnd, WM_QUIT, MPFROMLONG(0L), MPFROMLONG(0L));
   } /* endif */
   return fCancelOp;
}

// *******************************************************************************
// FUNCTION:     EnableSaveMenu
//
// FUNCTION USE: Sets enable state of the FILE|SAVE and menu item.
//
// DESCRIPTION:  Obtains the handle of the menu bar window and then sets the
//               enable state of the FILE|SAVE menu item.
//
// PARAMETERS:   HWND      handle of the application client window
//               BOOL      flag to enable item when set.
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void EnableSaveMenu( HWND hwnd, BOOL fEnable )
{
   HWND     hwndFrame;
   HWND     hwndMenu;

// --------------------------------------------------------------------------
// get the frame window
// --------------------------------------------------------------------------
   hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
   if (hwndFrame != NULLHANDLE) {

// --------------------------------------------------------------------------
// get the menu window
// --------------------------------------------------------------------------
      hwndMenu = WinWindowFromID( hwndFrame, FID_MENU );
      if (hwndMenu != NULLHANDLE) {

// --------------------------------------------------------------------------
// set the FILE|SAVE menu item state
// --------------------------------------------------------------------------
         WinEnableMenuItem( hwndMenu, MID_FILESAVE, fEnable );
      } /* endif */
   } /* endif */
}

// *******************************************************************************
// FUNCTION:     EnableEditMenu
//
// FUNCTION USE: Sets enable state of the CARD|EDIT and CARD|REMOVE menu items.
//
// DESCRIPTION:  Obtains the handle of the menu bar window and then sets the
//               enable state of the CARD|EDIT and CARD|REMOVE menu items.
//
// PARAMETERS:   HWND      handle of the application client window
//               BOOL      flag to enable item when set.
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void EnableEditMenu( HWND hwnd, BOOL fEnable )
{
   HWND     hwndFrame;
   HWND     hwndMenu;

// --------------------------------------------------------------------------
// get the frame window
// --------------------------------------------------------------------------
   hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
   if (hwndFrame != NULLHANDLE) {

// --------------------------------------------------------------------------
// get the menu window
// --------------------------------------------------------------------------
      hwndMenu = WinWindowFromID( hwndFrame, FID_MENU );
      if (hwndMenu != NULLHANDLE) {

// --------------------------------------------------------------------------
// set the FILE|SAVE menu item state
// --------------------------------------------------------------------------
         WinEnableMenuItem( hwndMenu, MID_CARDEDIT, fEnable );
         WinEnableMenuItem( hwndMenu, MID_CARDREMOVE, fEnable );
      } /* endif */
   } /* endif */
}

// *******************************************************************************
// FUNCTION:     SetGlobalFilename
//
// FUNCTION USE: Saves the global file name and updates the title bar.
//
// DESCRIPTION:  Copies the passed file name string to the global variable
//               then obtains the frame window handle and updates the title
//               bar with the file name.
//
// PARAMETERS:   HWND      handle of the application client window
//               PSZ       new file name
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void SetGlobalFilename( HWND hwnd, PSZ pszFilename )
{
   HWND     hwndFrame;
   LONG     cTitle;
   PSZ      pszTitleText;

// --------------------------------------------------------------------------
// save the file name
// --------------------------------------------------------------------------
   strcpy( gszFileName, pszFilename );

// --------------------------------------------------------------------------
// get the frame window handle and compute length of title bar text needed.
// --------------------------------------------------------------------------
   hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
   cTitle = strlen(TITLE_BAR_TEXT) + strlen(pszFilename) + 4;

// --------------------------------------------------------------------------
// Allocate space and build title bar string
// --------------------------------------------------------------------------
   pszTitleText = (PSZ)malloc( cTitle );
   if (pszTitleText) {
      strcpy( pszTitleText, TITLE_BAR_TEXT );
      strcat( pszTitleText, " - " );
      strcat( pszTitleText, pszFilename );

// --------------------------------------------------------------------------
// Modify the title bar text and then free the allocated memory
// --------------------------------------------------------------------------
      WinSetWindowText( hwndFrame, pszTitleText );
      free(pszTitleText);
   } /* endif */
}

// *******************************************************************************
// FUNCTION:     RefreshTopPage
//
// FUNCTION USE: Refresh the page window dialog with the current card data
//
// DESCRIPTION:
//
// PARAMETERS:   HWND      window handle of the notebook window
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void RefreshTopPage( HWND hwndNotebook )
{
   ULONG       ulPageID;
   HWND        hwndClient;
   HWND        hwndPage;
   PCARDDATA   pcd;
   char     szRating[3];

   do {
// --------------------------------------------------------------------------
// get the current top page ID and its associated window handle
// --------------------------------------------------------------------------
      ulPageID = LONGFROMMR(WinSendMsg( hwndNotebook, BKM_QUERYPAGEID,
                                        MPFROMLONG(0L),
                                        MPFROM2SHORT(BKA_TOP,0)));
      if (ulPageID == 0) break;

      hwndPage = (HWND)WinSendMsg( hwndNotebook, BKM_QUERYPAGEWINDOWHWND,
                                   MPFROMLONG(ulPageID),
                                   MPFROMLONG(0L));
      if (hwndPage == NULLHANDLE ) break;

// --------------------------------------------------------------------------
// Get the page data for the currently displayed page
// --------------------------------------------------------------------------
      pcd = (PCARDDATA)WinSendMsg( hwndNotebook, BKM_QUERYPAGEDATA,
                                   MPFROMLONG( ulPageID ),
                                   MPFROMLONG( 0L ));
      if ((LONG)pcd == BOOKERR_INVALID_PARAMETERS) pcd = NULL;
      if (pcd != NULL ) {
// --------------------------------------------------------------------------
// Fill in the dialog window controls with the card data if it exists.
// --------------------------------------------------------------------------
         WinSetDlgItemText( hwndPage, DIT_LASTNAME, pcd->pszNameL );
         WinSetDlgItemText( hwndPage, DIT_NAMEINIT, pcd->pszNameI );
         WinSetDlgItemText( hwndPage, DIT_COMPANY, pcd->pszCompany );
         WinSetDlgItemText( hwndPage, DIT_ADDR1, pcd->pszAddr1 );
         WinSetDlgItemText( hwndPage, DIT_ADDR2, pcd->pszAddr2 );
         WinSetDlgItemText( hwndPage, DIT_CITY, pcd->pszCity );
         WinSetDlgItemText( hwndPage, DIT_STATE, pcd->pszState );
         WinSetDlgItemText( hwndPage, DIT_ZIP, pcd->pszZip );
         WinSetDlgItemText( hwndPage, DIT_TELNO, pcd->pszTelephone );
         WinSetDlgItemText( hwndPage, DIT_DOB, pcd->pszDob );
         WinSetDlgItemText( hwndPage, DIT_ANN, pcd->pszAnn );
         WinSetDlgItemText( hwndPage, DIT_INFO, pcd->pszNotes );
         _itoa( pcd->usRating, szRating, 10 );
         WinSetDlgItemText( hwndPage, DIT_RATING, szRating );
         WinSendDlgItemMsg( hwndPage, DIT_ICON, SM_SETHANDLE,
                            (MPARAM)ghPtr[pcd->usType], MPFROMLONG(0L));
      } else {
// --------------------------------------------------------------------------
// No data - blank out the card (must be the first card for a letter).
// --------------------------------------------------------------------------
         WinSetDlgItemText( hwndPage, DIT_LASTNAME, "" );
         WinSetDlgItemText( hwndPage, DIT_NAMEINIT, "" );
         WinSetDlgItemText( hwndPage, DIT_COMPANY, "" );
         WinSetDlgItemText( hwndPage, DIT_ADDR1, "" );
         WinSetDlgItemText( hwndPage, DIT_ADDR2, "" );
         WinSetDlgItemText( hwndPage, DIT_CITY, "" );
         WinSetDlgItemText( hwndPage, DIT_STATE, "" );
         WinSetDlgItemText( hwndPage, DIT_ZIP, "" );
         WinSetDlgItemText( hwndPage, DIT_TELNO, "" );
         WinSetDlgItemText( hwndPage, DIT_DOB, "" );
         WinSetDlgItemText( hwndPage, DIT_ANN, "" );
         WinSetDlgItemText( hwndPage, DIT_INFO, "" );
         WinSetDlgItemText( hwndPage, DIT_RATING, "" );
         WinSendDlgItemMsg( hwndPage, DIT_ICON, SM_SETHANDLE,
                            (MPARAM)ghPtr[9], MPFROMLONG(0L));
      } /* endif */

// --------------------------------------------------------------------------
// Make sure the edit menu item is set
// --------------------------------------------------------------------------
      hwndClient = WinQueryWindow( hwndNotebook, QW_PARENT );
      if (hwndClient == NULLHANDLE ) break;
      EnableEditMenu( hwndClient, pcd != NULL );

// --------------------------------------------------------------------------
// show the window - with the method in use here, the window can sometimes
// be hidden when a back page is added.
// --------------------------------------------------------------------------
      WinShowWindow( hwndPage, TRUE );
   } while( false );
}

// *******************************************************************************
// FUNCTION:     GetTabSize
//
// FUNCTION USE: Determine the tab size required to fit the default
//               font characters in the tabs.
//
// DESCRIPTION:   Get a presentation space for the window and then
//                query the fontmetrics for the default font.  Return
//                the maximum width and height for characters in the
//                font.
//
// PARAMETERS:   HWND      notebook window handle
//               PUSHORT   pointer to location to store tab width
//               PUSHORT   pointer to location to store tab height
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void GetTabSize( HWND hwndNotebook, PUSHORT cx, PUSHORT cy )
{
   HPS         hps;
   FONTMETRICS fm;

// --------------------------------------------------------------------------
// Get a presentation space for the notebook
// --------------------------------------------------------------------------
   hps = WinGetPS( hwndNotebook );
   if (hps != NULLHANDLE) {

// --------------------------------------------------------------------------
// Get the metrics of the default font for the notebook
// --------------------------------------------------------------------------
      GpiQueryFontMetrics( hps, sizeof(FONTMETRICS), &fm );

// --------------------------------------------------------------------------
// set the tab size to the maximum character sizes plus offsets
// --------------------------------------------------------------------------
      *cx = fm.lMaxCharInc + 7;
      *cy = fm.lMaxBaselineExt + 7;

// --------------------------------------------------------------------------
// release the presentation space
// --------------------------------------------------------------------------
      WinReleasePS( hps );
   } /* endif */
}

// *******************************************************************************
// FUNCTION:     DeleteCard
//
// FUNCTION USE: Remove a card from the card file
//
// DESCRIPTION:  Removes a card from the card file.  If the card is on a
//               major tab page, move the following card to the major tab
//               page then delete the following page from the notebook,
//               otherwise, just delete the page.
//
// PARAMETERS:   HWND      notebook window handle
//               ULONG     page to be deleted
//               PCARDDATA card data for the page
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void DeleteCard( HWND hwndNotebook, ULONG ulPageID, PCARDDATA pcd )
{
   ULONG       alpha;
   ULONG       ulPageIDNext;
   PCARDDATA   pcdNext;
// --------------------------------------------------------------------------
// Find the MAJOR TAB page for this card
// --------------------------------------------------------------------------
         alpha = GetAlphaIndex( pcd->pszNameL[0] );
         if (aulAlpha2ID[alpha] == ulPageID) {

// --------------------------------------------------------------------------
// Removing the card on the major tab page - get the next page
// --------------------------------------------------------------------------
            ulPageIDNext = LONGFROMMR(WinSendMsg( hwndNotebook,
                                      BKM_QUERYPAGEID,
                                      MPFROMLONG(ulPageID),
                                      MPFROM2SHORT(BKA_NEXT,0)));

            if (ulPageIDNext != 0) {
// --------------------------------------------------------------------------
// Get the data for the next page - if it's in the same section then
// reset the MAJOR TAB data to the data for the next page and update
// the pageid to the next page so it will be deleted instead of the
// major tab page.
// --------------------------------------------------------------------------
               pcdNext = (PCARDDATA)WinSendMsg( hwndNotebook,
                                    BKM_QUERYPAGEDATA,
                                    MPFROMLONG(ulPageIDNext),
                                    MPFROMLONG(0L));
               if (pcdNext && pcdNext->pszNameL[0] == pcd->pszNameL[0]) {
                  WinSendMsg( hwndNotebook, BKM_SETPAGEDATA,
                              MPFROMLONG(ulPageID),
                              MPFROMP(pcdNext));
                  ulPageID = ulPageIDNext;
               } /* endif */
            } /* endif */
         } /* endif */

         if (aulAlpha2ID[alpha] != ulPageID) {

// --------------------------------------------------------------------------
// If the page is not the major tab page at this point - delete the page;
// otherwise, just clear out the data for the page
// --------------------------------------------------------------------------
            WinSendMsg( hwndNotebook, BKM_DELETEPAGE,
                        MPFROMLONG(ulPageID), MPFROMSHORT(BKA_SINGLE));
         } else {
            WinSendMsg( hwndNotebook, BKM_SETPAGEDATA,
                        MPFROMLONG(ulPageID), MPFROMLONG(0L));
         } /* endif */

}
