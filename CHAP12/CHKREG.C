// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      CHKREG.C                                         mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Main Source File for CHKREG/Chapter 12 Sample Program         บ
// บ                                                                               บ
// บ  NOTES:         This program is a simple check register which demonstrates    บ
// บ                 coding for the container control.                             บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#define  INCL_GPIBITMAPS
#define  INCL_WINERRORS
#define  INCL_WINSTDCNR
#define  INCL_WINFRAMEMGR
#define  INCL_WINWINDOWMGR
#define  INCL_WINMENUS
#define  INCL_WINPOINTERS
#define  INCL_WINSTDFILE
#define  INCL_WINSYS
#define  INCL_WINBUTTONS
#define  INCL_WINDIALOGS
#define  INCL_WINSHELLDATA

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "chkreg.h"
#include "shcommon.h"

// --------------------------------------------------------------------------
// Miscellaneous constant definitions used only in this source module.
// --------------------------------------------------------------------------
#define  MAX_FILE_RECORD      256
#define  FIELD_SEP            '\x1f'
#define  SIGNATURE_STR_LEN    4
#define  FILE_SIGNATURE       "Sm10"
#define  NUM_FILE_FIELDS      6
#define  BAL_STR_LEN          (AMT_STR_LEN + 1)
#define  FILE_UNTITLED        0x00000001UL

// --------------------------------------------------------------------------
// Offset definitions for the application's private window instance data.
// --------------------------------------------------------------------------
#define  WP_FILEP             0
#define  WL_FLAGS             4
#define  WP_SELRECORD         8
#define  WL_WITHDRAW         12
#define  WL_DEPOSIT          16
#define  WL_WITHDRAWM        20
#define  WL_DEPOSITM         24
#define  WL_WITHDRAWB        28
#define  WL_DEPOSITB         32
#define  WL_WITHDRAWMB       36
#define  WL_DEPOSITMB        40
#define  WP_MENURECORD       44
#define  WL_MENUHWND         48
#define  SIZE_WINDOW_DATA    52

// --------------------------------------------------------------------------
// Definition of the record data included with the container record
// --------------------------------------------------------------------------
typedef struct __TRANSREC__ {
   char  *pszRef;
   char  *pszDate;
   char  *pszDescription;
   char  *pszWithdrawal;
   char  *pszDeposit;
   char  *pszBalance;
   LONG  amount;
   LONG  balance;
   fpos_t Offset;
   char  filebuf[MAX_FILE_RECORD];
   char  szBalance[ BAL_STR_LEN ];
} TRANSREC, *PTRANSREC;

// --------------------------------------------------------------------------
// Definition of the combined container record core and the actual data
// --------------------------------------------------------------------------
typedef struct __TRANSACTION__ {
   RECORDCORE     record;
   TRANSREC       data;
} TRANSACTION, *PTRANSACTION;

// --------------------------------------------------------------------------
// Miscellaneous constant definitions used only in this source module.
// --------------------------------------------------------------------------
BOOL     false = FALSE;
BOOL     true = TRUE;
BOOL     bEnable = FALSE;
PFNWP    pfnCnrProc = NULL;               // used when subclassing
CHAR     *szClassName = "CHKREG";
CHAR     *szTitlePrefix = "Check Register";
CHAR     *szNewFile = "Untitled";

// --------------------------------------------------------------------------
// Predefined field info structures for DETAIL view columns
// --------------------------------------------------------------------------
FIELDINFO   fiColumns [ ] = {
   /* reference field */
   { 0, CFA_STRING | CFA_SEPARATOR | CFA_HORZSEPARATOR | CFA_RIGHT | CFA_FIREADONLY,
     CFA_FITITLEREADONLY | CFA_CENTER, "Ref.", 0, NULL, NULL, 0 },
 /* date field */
   { 0, CFA_STRING | CFA_SEPARATOR | CFA_HORZSEPARATOR | CFA_RIGHT | CFA_FIREADONLY,
     CFA_FITITLEREADONLY | CFA_CENTER, "Date", 0, NULL, NULL, 0 },
   /* description field */
   { 0, CFA_STRING | CFA_SEPARATOR | CFA_HORZSEPARATOR | CFA_LEFT | CFA_FIREADONLY,
     CFA_FITITLEREADONLY | CFA_CENTER, "Description", 0, NULL, NULL, 0 },
   /* withdrawal field */
   { 0, CFA_STRING | CFA_SEPARATOR | CFA_HORZSEPARATOR | CFA_RIGHT | CFA_FIREADONLY,
     CFA_FITITLEREADONLY | CFA_CENTER, "Withdrawal", 0, NULL, NULL, 0 },
   /* deposit field */
   { 0, CFA_STRING | CFA_SEPARATOR | CFA_HORZSEPARATOR | CFA_RIGHT | CFA_FIREADONLY,
     CFA_FITITLEREADONLY | CFA_CENTER, "Deposit", 0, NULL, NULL, 0 },
   /* balance field */
   { 0, CFA_STRING | CFA_SEPARATOR | CFA_HORZSEPARATOR | CFA_RIGHT | CFA_FIREADONLY,
     CFA_FITITLEREADONLY | CFA_CENTER, "Balance", 0, NULL, NULL, 0 },
};

#define  N_FIELDS    (sizeof(fiColumns)/sizeof(FIELDINFO))


// --------------------------------------------------------------------------
// Prototype for the client window procedure
// --------------------------------------------------------------------------
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------
// Prototype for dialog box used to configure the background bitmap
// --------------------------------------------------------------------------
MRESULT EXPENTRY BackBitmapDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------          
// Prototype for the subclass procedure                                             
// --------------------------------------------------------------------------          
MRESULT APIENTRY cnrSubclassProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );        

// --------------------------------------------------------------------------
// Prototypes for the message processing worker functions
// --------------------------------------------------------------------------
MRESULT um_openfile( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT um_nexttran( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_create( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_command( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_control( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_size( HWND hwnd, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------
// Prototypes for miscellaneous utility functions
// --------------------------------------------------------------------------
void DefineCnrCols( HWND hwndCnr );
void ProcessTransaction( HWND hwnd );
void InsertCnrRecord( HWND hwnd, fpos_t FileOffset, char *pszFileRec, BOOL fPaint );
PTRANSACTION FindInsertionPoint( HWND hwndCnr, PTRANSACTION pRec );
void UpdateBalance( HWND hwndCnr, PTRANSACTION pRec, BOOL fPaint );
BOOL CopyFile( HWND hwnd, char *szFile );
BOOL IsValidFile( HWND hwnd, char *szFile, BOOL bShowMsg );
void DeleteTransaction( HWND hwnd, PTRANSACTION prec );
void BalanceString( LONG balance, char *BalanceString );
void SetCnrView( HWND hwnd, HWND hwndMenu );

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
//    ARG1 may be a file name to be opened on startup.
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
main(int argc, char *argv[], char *envp[])
{
   HAB      hab = NULLHANDLE;           // Process anchor block handle 
   HMQ      hmq = NULLHANDLE;           // Process message queue 
   QMSG     qmsg =                      // message queue data structure 
               { NULLHANDLE, 0UL, (MPARAM)0, (MPARAM)0, 0UL, {0UL, 0UL}, 0UL };

   HWND     hwndFrame = NULLHANDLE;     // handle of the application's frame window 
   HWND     hwndClient = NULLHANDLE;    // handle of the application's client window 
   ULONG    flStyle = FCF_STANDARD;     // create style for application's window 
   char     *szFileName;                // initial file to open 

// --------------------------------------------------------------------------
// Single iteration loop - makes error handling easier
// --------------------------------------------------------------------------
   do {

// --------------------------------------------------------------------------
// Retrieve file name argument if present.  If not, use the default.
// --------------------------------------------------------------------------
      /* process startup parameters */
      if (argc > 1) {
         szFileName = argv[1];
      } else {
         szFileName = szNewFile;
      } /* endif */


// --------------------------------------------------------------------------
// Now set up the PM environment for the thread
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
// Register a class for the main application window */
// --------------------------------------------------------------------------
      if (!WinRegisterClass( hab,
                          szClassName,
                          ClientWndProc,
                          CS_SYNCPAINT | CS_SIZEREDRAW | CS_CLIPCHILDREN,
                          SIZE_WINDOW_DATA)) {
         // failed!
         DisplayMessages(NULLHANDLE, "Unable to Register Window Class",
                         MSG_ERROR);
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Finally, now the window can be created
// --------------------------------------------------------------------------
      hwndFrame = WinCreateStdWindow( HWND_DESKTOP,   // Parent window
                                      WS_VISIBLE,     // Initial style
                                      &flStyle,       // Frame creation flags
                                      szClassName,    // Class name
                                      szTitlePrefix,  // Title bar text
                                      0UL,            // Client style
                                      NULLHANDLE,     // Resource in .EXE
                                      RID_CHKREG,     // Frame window id
                                      &hwndClient );  // Client window
      if (hwndFrame == (HWND)NULLHANDLE) {
         // no window - gotta leave.
         DisplayMessages(NULLHANDLE, "Error Creating Frame Window",
                         MSG_ERROR);
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Open the requested file - or a new, untitled file */
// --------------------------------------------------------------------------
      WinPostMsg( hwndClient, UM_OPENFILE, MPFROMP(szFileName), MPFROMP(NULL) );

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
// Clean up on exit - releasing the message queue and PM environment
// --------------------------------------------------------------------------
   if (hmq != NULLHANDLE) {
      WinDestroyMsgQueue( hmq );
   } /* endif */

   if (hab != NULLHANDLE) {
      WinTerminate( hab );
   } /* endif */

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
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
 switch (msg) 
  {
   case UM_OPENFILE:        
        return um_openfile (hwnd, mp1, mp2);          
 
   case UM_NEXTTRAN:          
        return um_nexttran (hwnd, mp1, mp2);                     
 
   case WM_CREATE:            
        return wm_create (hwnd, mp1, mp2);                      
 
   case WM_COMMAND:           
        return wm_command (hwnd, mp1, mp2);
 
   case WM_CONTROL:           
        return wm_control (hwnd, mp1, mp2);
 
   case WM_SIZE:      
        return wm_size (hwnd, mp1, mp2);             
 
   default:                   
        return WinDefWindowProc (hwnd, msg, mp1, mp2);      
  } /* endswitch */
}

// *******************************************************************************
// FUNCTION:     um_openfile
//
// FUNCTION USE: Worker function to process the UM_OPENFILE message.
//
// DESCRIPTION:  Close any previously opened file, then open the new file and
//               verify that the file is of the proper type.  Begin the process
//               of loading the file into the container.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   pointer to name of file to open
//               MPARAM   reserved
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT um_openfile( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   FILE     *filep;
   char     szFileBuf[ SIGNATURE_STR_LEN + 1 ];
   char     szTitleString[ FILENAME_MAX + 20 ];
   ULONG    flags;
   HWND     hwndCnr;


// --------------------------------------------------------------------------
// Single iteration loop for easy exit and error handling
// --------------------------------------------------------------------------
   do {
// --------------------------------------------------------------------------
// Get the current stream pointer from the window instance data and
// check to see if a file is already open.
// --------------------------------------------------------------------------
      filep = WinQueryWindowPtr( hwnd, WP_FILEP );
      if (filep != (FILE *)NULL) {

// --------------------------------------------------------------------------
// A file is open - first close the file
// --------------------------------------------------------------------------
         fclose( filep );
// --------------------------------------------------------------------------
// If this was an untitled file - delete it, don't leave hanging around
// --------------------------------------------------------------------------
         flags = WinQueryWindowULong( hwnd, WL_FLAGS );
         if (flags & FILE_UNTITLED) {
            remove( szNewFile );
         } /* endif */
// --------------------------------------------------------------------------
// Clear the stream pointer in the window instance data
// --------------------------------------------------------------------------
         WinSetWindowPtr( hwnd, WP_FILEP, NULL );
      } /* endif */

// --------------------------------------------------------------------------
// Open the new file - in binary read mode
// --------------------------------------------------------------------------
      filep = fopen( (char *)mp1, "rb+" );
      if (filep != (FILE *)NULL) {

// --------------------------------------------------------------------------
// Read the signature for the file and compare to the valid signature
// --------------------------------------------------------------------------
         if (!fread( szFileBuf, SIGNATURE_STR_LEN + 1, 1, filep) ||
             strcmp( szFileBuf, FILE_SIGNATURE)) {

// --------------------------------------------------------------------------
// Invalid signature - sorry, can't open this file
// --------------------------------------------------------------------------
            WinMessageBox( HWND_DESKTOP, hwnd,
               "The selected file is not a proper data file",
               TITLEBAR, RID_MSG_BADFILE,
               MB_INFORMATION | MB_OK | MB_HELP );

// --------------------------------------------------------------------------
// Try to open the untitled file
// --------------------------------------------------------------------------
            WinSendMsg( hwnd, UM_OPENFILE, MPFROMP(szNewFile), MPFROMLONG(0L));
            break;
         } /* endif */
      } else {

// --------------------------------------------------------------------------
// must be opening a new file - open in write mode to create
// --------------------------------------------------------------------------
         filep = fopen( (char *)mp1, "wb+" );

// --------------------------------------------------------------------------
// if the file can't be created - then exit the program after notification
// --------------------------------------------------------------------------
         if (filep == (FILE *)NULL) {
            WinMessageBox( HWND_DESKTOP, hwnd,
               "Unable to open the selected file - terminating process",
               TITLEBAR, RID_MSG_NOFILE,
               MB_INFORMATION | MB_OK | MB_HELP );

            WinSendMsg( hwnd, WM_CLOSE, MPFROMLONG(0L), MPFROMLONG(0L));
            break;
         } else {

// --------------------------------------------------------------------------
// write the signature into the file
// --------------------------------------------------------------------------
            fwrite( FILE_SIGNATURE, SIGNATURE_STR_LEN + 1, 1, filep);
         } /* endif */
      } /* endif */

// --------------------------------------------------------------------------
// save stream pointer in the window data
// --------------------------------------------------------------------------
      WinSetWindowPtr( hwnd, WP_FILEP, filep );

// --------------------------------------------------------------------------
// set/clear untitled flag
// --------------------------------------------------------------------------
      flags = WinQueryWindowULong( hwnd, WL_FLAGS );
      if (!stricmp( (char *)mp1, szNewFile)) {
         flags |= FILE_UNTITLED;
      } else {
         flags &= ~FILE_UNTITLED;
      } /* endif */
      WinSetWindowULong( hwnd, WL_FLAGS, flags );

// --------------------------------------------------------------------------
// set window text to contain the file name
// --------------------------------------------------------------------------
      strcpy( szTitleString, szTitlePrefix );
      strcat( szTitleString, " - " );
      strcat( szTitleString, (char *)mp1 );
      WinSetWindowText( WinQueryWindow( hwnd, QW_PARENT ), szTitleString );

// --------------------------------------------------------------------------
// clear the container
// --------------------------------------------------------------------------
      hwndCnr = WinWindowFromID( hwnd, CID_REGISTER );
      if (hwndCnr != NULLHANDLE) {
         WinSendMsg( hwndCnr, CM_REMOVERECORD, MPFROMP(NULL),
                        MPFROM2SHORT( 0, CMA_FREE | CMA_INVALIDATE ));
      } /* endif */

// --------------------------------------------------------------------------
// start reading in the data
// --------------------------------------------------------------------------
      WinPostMsg( hwnd, UM_NEXTTRAN, MPFROMP(filep), MPFROMLONG(ftell(filep)));

   } while ( false  ); /* enddo */

// --------------------------------------------------------------------------
// return the reserved value
// --------------------------------------------------------------------------
   return (MRESULT)0;
}

// *******************************************************************************
// FUNCTION:     um_nexttran
//
// FUNCTION USE: Worker function to process the UM_NEXTTRAN message.
//
// DESCRIPTION:  Reads a record from the file at the specified location and
//               insert a container record for the file record.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   stream pointer for open file
//               MPARAM   position in file from which to read
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT um_nexttran( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND        hwndCnr = NULLHANDLE;      /* container window handle */
   char        record[ 256 ] =  "";
   int         i;
   fpos_t      Offset;
   char        *pszRecord;

   do {
// --------------------------------------------------------------------------
// Get handle to the container window
// --------------------------------------------------------------------------
      hwndCnr = WinWindowFromID( hwnd, CID_REGISTER );
      /* validate handle before proceeding */
      if (hwndCnr == NULLHANDLE) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// read a record in from the file
// --------------------------------------------------------------------------
      if (fseek( (FILE *)mp1, (LONG)mp2, SEEK_SET)) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// get the position of the file so we can rewrite the record later
// --------------------------------------------------------------------------
      fgetpos( (FILE *)mp1, &Offset );

// --------------------------------------------------------------------------
// read in the record by getting characters from the file and counting the
// number of field separators
// --------------------------------------------------------------------------
      pszRecord = record;
      for (i = 0; i < NUM_FILE_FIELDS; i++ ) {
         while (!feof((FILE *)mp1) &&
                (*(pszRecord++) = (char)getc((FILE *)mp1)) != FIELD_SEP ) {
            continue;
         } /* endwhile */
      } /* endfor */

// --------------------------------------------------------------------------
// if this is not a deleted record - add it to the container
// --------------------------------------------------------------------------
      if (record[0] != 'D') {
         InsertCnrRecord( hwnd, Offset, record, FALSE );
      } /* endif */

// --------------------------------------------------------------------------
// if not at the end of file - get the next record
// --------------------------------------------------------------------------
      if (!feof((FILE *)mp1)) {
         WinPostMsg( hwnd, UM_NEXTTRAN, mp1, MPFROMLONG(ftell((FILE *)mp1)));
      } else {
// --------------------------------------------------------------------------
// otherwise - paint the container with the data
// --------------------------------------------------------------------------
         WinSendMsg( hwndCnr, CM_INVALIDATERECORD, MPFROMP(NULL),
                     MPFROM2SHORT( 0, CMA_TEXTCHANGED ));
      } /* endif */

   } while ( false );

// --------------------------------------------------------------------------
// return the reserved value
// --------------------------------------------------------------------------
   return (MRESULT)0;
}

// *******************************************************************************
// FUNCTION:     wm_create
//
// FUNCTION USE: Worker function to process the WM_CREATE message.
//
// DESCRIPTION:  Creates the container window and loads the bitmaps and
//               pointers that will be used to display the container records.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   pointer to control data from WinCreateWindow()
//               MPARAM   pointer to PCREATESTRUCT
//
// RETURNS:      MRESULT  TRUE, error occurred, abort window creation.
//                        FALSE, no error, continue window creation.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wm_create( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND        hwndCnr = NULLHANDLE;      // container window handle 
   CNRINFO     cnrInfo;                   // container info struct 
   HPOINTER    hPtr;
   HPS         hps;
   HBITMAP     hbmp;
   LONG        lReturn;                   // check return for Prf Call
   MRESULT     mr = MRFROMLONG(TRUE);     // assume error occurs 

// --------------------------------------------------------------------------
// single iteration loop for error handling
// --------------------------------------------------------------------------
   do {
// --------------------------------------------------------------------------
// Create the container control window */
// --------------------------------------------------------------------------
      hwndCnr = WinCreateWindow( hwnd,
                                 WC_CONTAINER,
                                 "Just a test",
                                 CCS_AUTOPOSITION | WS_VISIBLE,
                                 0, 0, 0, 0,
                                 hwnd,
                                 HWND_TOP,
                                 CID_REGISTER,
                                 NULL,
                                 NULL );
      if (hwndCnr == NULLHANDLE) {
         DisplayMessages(NULLHANDLE, "Error creating container",
                         MSG_ERROR);
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Subclass the container window to paint the container background if
// the user has indicated that they want to see it.  
// --------------------------------------------------------------------------
      lReturn = PrfQueryProfileInt(HINI_USER, APPNAME, KEYNAME, 0);       
      if (lReturn == USEIT)                                              
       {
        pfnCnrProc = WinSubclassWindow( hwndCnr, cnrSubclassProc );
       }

// --------------------------------------------------------------------------
// Initially display the details view with column titles.  First get the
// current container data, modify the attribute flags, the pass the new
// container data back to the container.
// --------------------------------------------------------------------------
      if (WinSendMsg( hwndCnr, CM_QUERYCNRINFO,
                     (MPARAM)&cnrInfo, (MPARAM)sizeof(CNRINFO)) == 0) {
         DisplayMessages(NULLHANDLE, "Error creating container",
                         MSG_ERROR);
         break;
      } /* endif */
      cnrInfo.flWindowAttr = CV_DETAIL | CA_DETAILSVIEWTITLES |
                             CA_OWNERPAINTBACKGROUND;
      WinSendMsg( hwndCnr, CM_SETCNRINFO, (MPARAM)&cnrInfo, (MPARAM)CMA_FLWINDOWATTR );

// --------------------------------------------------------------------------
// Build the detail view column definitions
// --------------------------------------------------------------------------
      DefineCnrCols( hwndCnr );

// --------------------------------------------------------------------------
// Load the pointers and bitmaps used to represent records in the various
// views.  Store the handle for each in the client window instance data.
// Pointers are loaded for normal-sized withdrawals and deposits and for
// mini-sized withdrawals and deposits.  Bitmaps are loaded for normal-sized
// withdrawals and deposits and for mini-sized withdrawals and deposits.
// --------------------------------------------------------------------------
      hPtr = WinLoadPointer( HWND_DESKTOP, NULLHANDLE, ICON_WITHDRAW );
      WinSetWindowULong( hwnd, WL_WITHDRAW, (ULONG)hPtr );
      hPtr = WinLoadPointer( HWND_DESKTOP, NULLHANDLE, ICON_DEPOSIT );
      WinSetWindowULong( hwnd, WL_DEPOSIT, (ULONG)hPtr );
      hPtr = WinLoadPointer( HWND_DESKTOP, NULLHANDLE, ICON_WITHDRAWM );
      WinSetWindowULong( hwnd, WL_WITHDRAWM, (ULONG)hPtr );
      hPtr = WinLoadPointer( HWND_DESKTOP, NULLHANDLE, ICON_DEPOSITM );
      WinSetWindowULong( hwnd, WL_DEPOSITM, (ULONG)hPtr );

// --------------------------------------------------------------------------
// Get a presentation space for loading the bitmaps.
// --------------------------------------------------------------------------
      hps = WinGetPS( hwnd );
      if (hps != NULLHANDLE) {
         hbmp = GpiLoadBitmap( hps, NULLHANDLE, BMP_WITHDRAWB, 0, 0 );
         WinSetWindowULong( hwnd, WL_WITHDRAWB, (ULONG)hbmp );
         hbmp = GpiLoadBitmap( hps, NULLHANDLE, BMP_DEPOSITB, 0, 0 );
         WinSetWindowULong( hwnd, WL_DEPOSITB, (ULONG)hbmp );
         hbmp = GpiLoadBitmap( hps, NULLHANDLE, BMP_WITHDRAWM, 0, 0 );
         WinSetWindowULong( hwnd, WL_WITHDRAWMB, (ULONG)hbmp );
         hbmp = GpiLoadBitmap( hps, NULLHANDLE, BMP_DEPOSITM, 0, 0 );
         WinSetWindowULong( hwnd, WL_DEPOSITMB, (ULONG)hbmp );
         WinReleasePS( hps );
      } /* endif */

// --------------------------------------------------------------------------
// Clear the window instance data locations for the record which presented a
// popup menu and the window handle of the popup menu.
// --------------------------------------------------------------------------
      WinSetWindowPtr( hwnd, WP_MENURECORD, NULL );
      WinSetWindowULong( hwnd, WL_MENUHWND, NULLHANDLE );

      mr = MRFROMLONG(FALSE);
   } while ( false ); /* enddo */

   return mr;
}

// *******************************************************************************
// FUNCTION:     wm_command
//
// FUNCTION USE: Worker function to process the WM_COMMAND message.
//
// DESCRIPTION:  Processes user menu selections
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   menu id in low-order word
//               MPARAM   source information
//
// RETURNS:      MRESULT  reserved value -- 0
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wm_command( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   FILEDLG        fd;
   PTRANSACTION   prec;
   HWND           hwndFrame;
   HWND           hwndMenu;
   BOOL           bChecked;

   switch (SHORT1FROMMP( mp1 )) {

// --------------------------------------------------------------------------
// MID_FILENEW:   Create a new empty file.  Send the UM_OPENFILE message
//                with the "Untitled" string for a file name.
// --------------------------------------------------------------------------
   case MID_FILENEW:
      WinSendMsg( hwnd, UM_OPENFILE, MPFROMP(szNewFile), MPFROMP(NULL));
      break;

// --------------------------------------------------------------------------
// MID_FILEOPEN:  Open an existing file.  Get the filename from the user
//                with the standard file dialog, then verify that the
//                untitled file was not selected.  If a valid filename was
//                selected send the UM_OPENFILE message to load the contents
//                of the file.
// --------------------------------------------------------------------------
   case MID_FILEOPEN:
      memset((PVOID)&fd, 0, sizeof( FILEDLG ));

      fd.cbSize   = sizeof(FILEDLG);
      fd.fl       = FDS_OPEN_DIALOG;
      fd.pszTitle = "CHKREG - Open Check Register Data";         
      strcpy(fd.szFullFile, "*.DAT");            


      do {
         WinFileDlg( HWND_DESKTOP, hwnd, &fd );
         if (fd.lReturn == DID_OK && !IsValidFile( hwnd, fd.szFullFile, TRUE )) {
            continue;
         } else {
            break;
         } /* endif */
      } while ( true ); /* enddo */
      if (fd.lReturn == DID_OK) {
         WinSendMsg( hwnd, UM_OPENFILE, MPFROMP(fd.szFullFile), MPFROMP(NULL));
      } /* endif */
      break;

// --------------------------------------------------------------------------
// MID_FILESAVEAS:  Save the contents of the current file under a new file
//                  name.  Retrieve the name of the new file from the user
//                  with the standard file dialog and verify that the
//                  untitled file was not selected.  Then copy the current
//                  file to the new file and send the UM_OPENFILE message
//                  to load the contents of the new file.
// --------------------------------------------------------------------------
   case MID_FILESAVEAS:
      memset((PVOID)&fd, 0, sizeof( FILEDLG ));

      fd.cbSize   = sizeof( FILEDLG );
      fd.fl       = FDS_SAVEAS_DIALOG;
      fd.pszTitle = "CHKREG - Save Check Register Data";         

      do {
         WinFileDlg( HWND_DESKTOP, hwnd, &fd );
         if (fd.lReturn == DID_OK && !IsValidFile( hwnd, fd.szFullFile, TRUE )) {
            continue;
         } else {
            break;
         } /* endif */
      } while ( true ); /* enddo */
      if (fd.lReturn == DID_OK) {
         if (CopyFile( hwnd, fd.szFullFile )) {
            WinSendMsg( hwnd, UM_OPENFILE, MPFROMP(fd.szFullFile), MPFROMP(NULL));
         } /* endif */
      } /* endif */
      break;

// --------------------------------------------------------------------------
// MID_EXIT:      Terminate the application.  Send the WM_CLOSE message to
//                handle cleanup.
// --------------------------------------------------------------------------
   case MID_EXIT:
      WinSendMsg( hwnd, WM_CLOSE, (MPARAM)0, (MPARAM)0 );
      break;

// --------------------------------------------------------------------------
// MID_TRN_ENTER: Add a new transaction to the database by calling the
//                worker function for processing transactions.
// --------------------------------------------------------------------------
   case MID_TRN_ENTER:
      ProcessTransaction( hwnd );
      break;

// --------------------------------------------------------------------------
// MID_TRN_DELETE: Delete the currently selected transaction by calling the
//                 the worker function to delete a transaction.
// --------------------------------------------------------------------------
   case MID_TRN_DELETE:
      DeleteTransaction( hwnd, NULL );
      break;

// --------------------------------------------------------------------------
// MID_POP_DELETE: Delete the transaction which raised the popup menu.
//                 Get the transaction pointer saved when the menu was
//                 raised and call the worker function to delete a
//                 transaction.
// --------------------------------------------------------------------------
   case MID_POP_DELETE:
      prec = (PTRANSACTION)WinQueryWindowPtr( hwnd, WP_MENURECORD );
      DeleteTransaction( hwnd, prec );
      break;

// --------------------------------------------------------------------------
// MID_VIEWICON:   Select the container ICON view.  Check the ICON view
//                 menu item and uncheck the other views.  Set the proper
//                 enable state for the mode items.  Then call the worker
//                 function to establish the correct view of the container.
// --------------------------------------------------------------------------
   case MID_VIEWICON:
      hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
      if (hwndFrame != NULLHANDLE) {
         hwndMenu = WinWindowFromID( hwndFrame, FID_MENU );
         if (hwndMenu != NULLHANDLE) {
            WinCheckMenuItem( hwndMenu, MID_VIEWICON, TRUE );
            WinCheckMenuItem( hwndMenu, MID_VIEWNAME, FALSE );
            WinCheckMenuItem( hwndMenu, MID_VIEWTEXT, FALSE );
            WinCheckMenuItem( hwndMenu, MID_VIEWDETAIL, FALSE );
            WinEnableMenuItem( hwndMenu, MID_VIEWTREE, TRUE );
            WinEnableMenuItem( hwndMenu, MID_VIEWMINI, TRUE );
            WinEnableMenuItem( hwndMenu, MID_VIEWFLOW, FALSE );
            WinEnableMenuItem( hwndMenu, MID_VIEWTYPE, TRUE );
            SetCnrView( hwnd, hwndMenu );
         } /* endif */
      } /* endif */
      break;

// --------------------------------------------------------------------------
// MID_VIEWNAME:   Select the container NAME view.  Check the NAME view
//                 menu item and uncheck the other views.  Set the proper
//                 enable state for the mode items.  Then call the worker
//                 function to establish the correct view of the container.
// --------------------------------------------------------------------------
   case MID_VIEWNAME:
      hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
      if (hwndFrame != NULLHANDLE) {
         hwndMenu = WinWindowFromID( hwndFrame, FID_MENU );
         if (hwndMenu != NULLHANDLE) {
            WinCheckMenuItem( hwndMenu, MID_VIEWNAME, TRUE );
            WinCheckMenuItem( hwndMenu, MID_VIEWICON, FALSE );
            WinCheckMenuItem( hwndMenu, MID_VIEWTEXT, FALSE );
            WinCheckMenuItem( hwndMenu, MID_VIEWDETAIL, FALSE );
            WinEnableMenuItem( hwndMenu, MID_VIEWTREE, TRUE );
            WinEnableMenuItem( hwndMenu, MID_VIEWMINI, TRUE );
            WinEnableMenuItem( hwndMenu, MID_VIEWFLOW, TRUE );
            WinEnableMenuItem( hwndMenu, MID_VIEWTYPE, TRUE );
            SetCnrView( hwnd, hwndMenu );
         } /* endif */
      } /* endif */
      break;

// --------------------------------------------------------------------------
// MID_VIEWTEXT:   Select the container TEXT view.  Check the TEXT view
//                 menu item and uncheck the other views.  Set the proper
//                 enable state for the mode items.  Then call the worker
//                 function to establish the correct view of the container.
// --------------------------------------------------------------------------
   case MID_VIEWTEXT:
      hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
      if (hwndFrame != NULLHANDLE) {
         hwndMenu = WinWindowFromID( hwndFrame, FID_MENU );
         if (hwndMenu != NULLHANDLE) {
            WinCheckMenuItem( hwndMenu, MID_VIEWTEXT, TRUE );
            WinCheckMenuItem( hwndMenu, MID_VIEWICON, FALSE );
            WinCheckMenuItem( hwndMenu, MID_VIEWNAME, FALSE );
            WinCheckMenuItem( hwndMenu, MID_VIEWDETAIL, FALSE );
            WinEnableMenuItem( hwndMenu, MID_VIEWTREE, TRUE );
            WinEnableMenuItem( hwndMenu, MID_VIEWMINI, FALSE );
            WinEnableMenuItem( hwndMenu, MID_VIEWFLOW, TRUE );
            WinEnableMenuItem( hwndMenu, MID_VIEWTYPE, TRUE );
            SetCnrView( hwnd, hwndMenu );
         } /* endif */
      } /* endif */
      break;

// --------------------------------------------------------------------------
// MID_VIEWDETAIL:   Select the container DETAIL view.  Check the DETAIL view
//                   menu item and uncheck the other views.  Set the proper
//                   enable state for the mode items.  Then call the worker
//                   function to establish the correct view of the container.
// --------------------------------------------------------------------------
   case MID_VIEWDETAIL:
      hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
      if (hwndFrame != NULLHANDLE) {
         hwndMenu = WinWindowFromID( hwndFrame, FID_MENU );
         if (hwndMenu != NULLHANDLE) {
            WinCheckMenuItem( hwndMenu, MID_VIEWDETAIL, TRUE );
            WinCheckMenuItem( hwndMenu, MID_VIEWICON, FALSE );
            WinCheckMenuItem( hwndMenu, MID_VIEWNAME, FALSE );
            WinCheckMenuItem( hwndMenu, MID_VIEWTEXT, FALSE );
            WinEnableMenuItem( hwndMenu, MID_VIEWTREE, FALSE );
            WinEnableMenuItem( hwndMenu, MID_VIEWMINI, FALSE );
            WinEnableMenuItem( hwndMenu, MID_VIEWFLOW, FALSE );
            WinEnableMenuItem( hwndMenu, MID_VIEWTYPE, FALSE );
            SetCnrView( hwnd, hwndMenu );
         } /* endif */
      } /* endif */
      break;

// --------------------------------------------------------------------------
// MID_VIEWTREE:
// MID_VIEWMINI:
// MID_VIEWFLOW: Toggle the select state of one of the three container mode
//               menu items, then call the worker function to establish the
//               correct view of the container.
// --------------------------------------------------------------------------
   case MID_VIEWTREE:
   case MID_VIEWMINI:
   case MID_VIEWFLOW:
      hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
      if (hwndFrame != NULLHANDLE) {
         hwndMenu = WinWindowFromID( hwndFrame, FID_MENU );
         if (hwndMenu != NULLHANDLE) {
            bChecked = WinIsMenuItemChecked( hwndMenu, SHORT1FROMMP(mp1));
            WinCheckMenuItem( hwndMenu, SHORT1FROMMP(mp1), !bChecked );
            SetCnrView( hwnd, hwndMenu );
         } /* endif */
      } /* endif */
      break;

// --------------------------------------------------------------------------
// MID_VIEWICONS: Cause the container to display ICON graphics.  Check the
//                ICON menu item and uncheck the BITMAP menu item.  Call the
//                worker function to establish the selected view of the
//                container.
// --------------------------------------------------------------------------
   case MID_VIEWICONS:
      hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
      if (hwndFrame != NULLHANDLE) {
         hwndMenu = WinWindowFromID( hwndFrame, FID_MENU );
         if (hwndMenu != NULLHANDLE) {
            WinCheckMenuItem( hwndMenu, MID_VIEWICONS, TRUE );
            WinCheckMenuItem( hwndMenu, MID_VIEWBITMAPS, FALSE );
            SetCnrView( hwnd, hwndMenu );
         } /* endif */
      } /* endif */
      break;

// --------------------------------------------------------------------------
// MID_VIEWBITMAPS: Cause the container to display BITMAP graphics.  Check
//                  the BITMAP menu item and uncheck the ICON menu item.
//                  Call the worker function to establish the selected view
//                  of the container.
// --------------------------------------------------------------------------
   case MID_VIEWBITMAPS:
      hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
      if (hwndFrame != NULLHANDLE) {
         hwndMenu = WinWindowFromID( hwndFrame, FID_MENU );
         if (hwndMenu != NULLHANDLE) {
            WinCheckMenuItem( hwndMenu, MID_VIEWBITMAPS, TRUE );
            WinCheckMenuItem( hwndMenu, MID_VIEWICONS, FALSE );
            SetCnrView( hwnd, hwndMenu );
         } /* endif */
      } /* endif */
      break;

// --------------------------------------------------------------------------
// MID_PRODINFO: Display the product information dialog
// --------------------------------------------------------------------------
   case MID_PRODINFO:
        WinDlgBox( HWND_DESKTOP, hwnd, WinDefDlgProc, NULLHANDLE, RID_PRODINFO, NULL );
        break;

   case MID_BACKBMP:     
        WinDlgBox( HWND_DESKTOP, hwnd, BackBitmapDlgProc, NULLHANDLE, RID_BITMAP, NULL );
        break;                                              

   } /* endswitch */

// --------------------------------------------------------------------------
// Done - return the reserved value.
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wm_control
//
// FUNCTION USE: Worker function to process the WM_CONTROL message.
//
// DESCRIPTION:  Processes various notifications from the container window.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   Control ID in low-order word, notication code in
//                        high order work.
//               MPARAM   Notification data
//
// RETURNS:      MRESULT  reserved value -- 0
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wm_control( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   USHORT      cid;
   USHORT      code;
   HWND        hwndMenu;
   POINTL      pt;
   PNOTIFYRECORDEMPHASIS      pEmphasis;
   PTRANSACTION               prec;

// --------------------------------------------------------------------------
// Extract the control ID and the notification code from the parameters.
// --------------------------------------------------------------------------
   cid = SHORT1FROMMP( mp1 );
   code = SHORT2FROMMP( mp1 );

// --------------------------------------------------------------------------
// we're only interested in what comes from the container */
// --------------------------------------------------------------------------
   if (cid == CID_REGISTER) {
      switch (code) {

      case CN_EMPHASIS:
// --------------------------------------------------------------------------
// record emphasis changed, if a new record was selected, save a pointer to
// the record in the window instance data.
// --------------------------------------------------------------------------
         pEmphasis = (PNOTIFYRECORDEMPHASIS)mp2;

         if (pEmphasis->fEmphasisMask & CRA_SELECTED) {
             WinSetWindowPtr( hwnd, WP_SELRECORD, (PVOID)(pEmphasis->pRecord) );
         } /* endif */
         break;

      case CN_CONTEXTMENU:
// --------------------------------------------------------------------------
// a popup menu was requested.
// --------------------------------------------------------------------------
         prec = (PTRANSACTION)mp2;

// --------------------------------------------------------------------------
// Do nothing if the request was over whitespace - not over a record.
// --------------------------------------------------------------------------
         if (prec != (PTRANSACTION)NULL) {

// --------------------------------------------------------------------------
// save a pointer to the record that raised the menu in the window instance
// data - this will be used later if a menu item is actually selected.
// --------------------------------------------------------------------------
            WinSetWindowPtr( hwnd, WP_MENURECORD, (PVOID)prec );

// --------------------------------------------------------------------------
// Load the menu from the resource file if not already loaded.
// --------------------------------------------------------------------------
            hwndMenu = (HWND)WinQueryWindowULong( hwnd, WL_MENUHWND );
            if (hwndMenu == NULLHANDLE) {
               hwndMenu = WinLoadMenu( hwnd, NULLHANDLE, MID_POPUP );
               WinSetWindowULong( hwnd, WL_MENUHWND, hwndMenu );
            } /* endif */

// --------------------------------------------------------------------------
// Determine the location of the mouse pointer so that the menu can be
// raised at the pointer location.
// --------------------------------------------------------------------------
            WinQueryPointerPos( HWND_DESKTOP, &pt );
            WinMapWindowPoints( HWND_DESKTOP, hwnd, &pt, 1 );

// --------------------------------------------------------------------------
// Display the menu to the user
// --------------------------------------------------------------------------
            WinPopupMenu( hwnd, hwnd, hwndMenu, pt.x, pt.y, 0,
                  PU_KEYBOARD | PU_MOUSEBUTTON1 );
         } /* endif */
         break;
      } /* endswitch */
   } /* endif */

// --------------------------------------------------------------------------
// return the reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wm_size
//
// FUNCTION USE: Worker function to process the WM_SIZE message.
//
// DESCRIPTION:  Maintains the container window size when the size of the client
//               window changes to ensure that the container always completely
//               fills the client.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   old cx & cy
//               MPARAM   new cx & cy
//
// RETURNS:      MRESULT  reserved value -- 0
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wm_size( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND        hwndCnr = NULLHANDLE;      /* container window handle */
   RECTL       rectl = { 0, 0, 0, 0 };

// --------------------------------------------------------------------------
// Get the client window rectangle
// --------------------------------------------------------------------------
   WinQueryWindowRect(hwnd, &rectl );

// --------------------------------------------------------------------------
// Get the container window handle
// --------------------------------------------------------------------------
   hwndCnr = WinWindowFromID( hwnd, CID_REGISTER );
   if (hwndCnr != NULLHANDLE) {

// --------------------------------------------------------------------------
// modify the size of the container window to match the client window.
// --------------------------------------------------------------------------
      WinSetWindowPos( hwndCnr, HWND_TOP,
                       rectl.xLeft, rectl.yBottom,
                       rectl.xRight, rectl.yTop,
                       SWP_SIZE | SWP_MOVE | SWP_SHOW );
   } /* endif */

// --------------------------------------------------------------------------
// return the reserved value.
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     DefineCnrCols
//
// FUNCTION USE: Function to load the column definitions for the container
//               DETAIL view.
//
// DESCRIPTION:  Allocates the column definition data structures and then fills
//               the structures from predefined tables.
//
// PARAMETERS:   HWND     container window handle
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
void DefineCnrCols( HWND hwndCnr )
{
   PFIELDINFO        pFieldInfo = NULL;
   PFIELDINFO        pField = NULL;
   PFIELDINFO        pFieldNext;
   FIELDINFOINSERT   fii;

   do {
// --------------------------------------------------------------------------
// Allocate column data structures for all columns.  These will be returned
// as a linked list of FIELDINFO structures.
// --------------------------------------------------------------------------
      pFieldInfo = WinSendMsg( hwndCnr, CM_ALLOCDETAILFIELDINFO,
                               MPFROMSHORT( N_FIELDS ),
                               MPFROMLONG( 0l ));
      if (pFieldInfo == (PFIELDINFO)NULL) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Set pointer to the first field.  For each field, fill save the pointer
// to the next field, then copy in data from the predefined column
// structures, set the offset to the data, and then set the next field
// pointer back to the save value.
// --------------------------------------------------------------------------
      pField = pFieldInfo;

// --------------------------------------------------------------------------
// set the "reference" (check number) field
// --------------------------------------------------------------------------
      pFieldNext = pField->pNextFieldInfo;
      *pField = fiColumns[ 0 ];
      pField->offStruct = (ULONG)FIELDOFFSET( TRANSACTION, data.pszRef );
      pField = pField->pNextFieldInfo = pFieldNext;
      if (pField == NULL) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// set the transaction data field.
// --------------------------------------------------------------------------
      pFieldNext = pField->pNextFieldInfo;
      *pField = fiColumns[ 1 ];
      pField->offStruct = (ULONG)FIELDOFFSET( TRANSACTION, data.pszDate );
      pField = pField->pNextFieldInfo = pFieldNext;
      if (pField == NULL) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// set the transaction description field.
// --------------------------------------------------------------------------
      pFieldNext = pField->pNextFieldInfo;
      *pField = fiColumns[ 2 ];
      pField->offStruct = (ULONG)FIELDOFFSET( TRANSACTION, data.pszDescription );
      pField = pField->pNextFieldInfo = pFieldNext;
      if (pField == NULL) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// set the withdrawal amount field.
// --------------------------------------------------------------------------
      pFieldNext = pField->pNextFieldInfo;
      *pField = fiColumns[ 3 ];
      pField->offStruct = (ULONG)FIELDOFFSET( TRANSACTION, data.pszWithdrawal );
      pField = pField->pNextFieldInfo = pFieldNext;
      if (pField == NULL) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// set the deposit amount field.
// --------------------------------------------------------------------------
      pFieldNext = pField->pNextFieldInfo;
      *pField = fiColumns[ 4 ];
      pField->offStruct = (ULONG)FIELDOFFSET( TRANSACTION, data.pszDeposit );
      pField = pField->pNextFieldInfo = pFieldNext;
      if (pField == NULL) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// set the account balance amount field.
// --------------------------------------------------------------------------
      pFieldNext = pField->pNextFieldInfo;
      *pField = fiColumns[ 5 ];
      pField->offStruct = (ULONG)FIELDOFFSET( TRANSACTION, data.pszBalance );
      pField = pField->pNextFieldInfo = pFieldNext;

// --------------------------------------------------------------------------
// now fill in the structure used to send the field definitions to the
// container.  The fields will be inserted at the beginning of the
// column definitions and cause a repaint.
// --------------------------------------------------------------------------
      fii.cb = sizeof( FIELDINFOINSERT );
      fii.pFieldInfoOrder = (PFIELDINFO)CMA_FIRST;
      fii.fInvalidateFieldInfo = TRUE;
      fii.cFieldInfoInsert = N_FIELDS;

// --------------------------------------------------------------------------
// Send the column definitions to the container
// --------------------------------------------------------------------------
      WinSendMsg( hwndCnr, CM_INSERTDETAILFIELDINFO, pFieldInfo, &fii );

   } while ( false ); /* enddo */

   return;
}

// *******************************************************************************
// FUNCTION:     ProcessTransaction
//
// FUNCTION USE: Function to add a new transaction to the database and container
//
// DESCRIPTION:  Retrieves the transaction information from the user, then
//               formats the data into a record which is stored into the file
//               and inserted into the container.
//
// PARAMETERS:   HWND     container window handle
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void ProcessTransaction( HWND hwnd )
{
   TRANSDLG       params;
   char           szFileRec[MAX_FILE_RECORD];
   char           *pszPointer;
   char           *pszTemp;
   FILE           *file;
   fpos_t         Offset;

// --------------------------------------------------------------------------
// Initialize the data structure use to communicate with the transaction dlg.
// --------------------------------------------------------------------------
   memset( (PVOID)&params, 0, sizeof( TRANSDLG ));
   params.cb = sizeof(TRANSDLG);

// --------------------------------------------------------------------------
// retrieve the transaction data from the user
// --------------------------------------------------------------------------
   if (WinDlgBox( HWND_DESKTOP, hwnd, TransactDlgProc,
                  NULLHANDLE, RID_TRANSACTION, &params ) == DID_OK) {

// --------------------------------------------------------------------------
// fill in the file buffer for the record
// --------------------------------------------------------------------------
      pszPointer = szFileRec;

// --------------------------------------------------------------------------
// indicate an active record
// --------------------------------------------------------------------------
      *(pszPointer++) = ' ';
      *(pszPointer++) = FIELD_SEP;

// --------------------------------------------------------------------------
// copy in the date
// --------------------------------------------------------------------------
      pszTemp = params.date;
      while (*pszTemp) {
         *(pszPointer++) = *(pszTemp++);
      } /* endwhile */
      *(pszPointer++) = FIELD_SEP;

// --------------------------------------------------------------------------
// copy in the reference (check number) field
// --------------------------------------------------------------------------
      pszTemp = params.ref;
      while (*pszTemp) {
         *(pszPointer++) = *(pszTemp++);
      } /* endwhile */
      *(pszPointer++) = FIELD_SEP;

// --------------------------------------------------------------------------
// copy in the memo field
// --------------------------------------------------------------------------
      pszTemp = params.memo;
      while (*pszTemp) {
         *(pszPointer++) = *(pszTemp++);
      } /* endwhile */
      *(pszPointer++) = FIELD_SEP;

// --------------------------------------------------------------------------
// copy in the withdrawal amount field
// --------------------------------------------------------------------------
      pszTemp = params.amount;
      if (!params.bDeposit) {
         while (*pszTemp) {
            *(pszPointer++) = *(pszTemp++);
         } /* endwhile */
      } /* endif */
      *(pszPointer++) = FIELD_SEP;

// --------------------------------------------------------------------------
// copy in the deposit amount field
// --------------------------------------------------------------------------
      if (params.bDeposit) {
         while (*pszTemp) {
            *(pszPointer++) = *(pszTemp++);
         } /* endwhile */
      } /* endif */
      *(pszPointer++) = FIELD_SEP;
      *(pszPointer++) = '\0';

// --------------------------------------------------------------------------
// record complete - add the record at the end of the file
// --------------------------------------------------------------------------
      file = (FILE *)WinQueryWindowPtr( hwnd, WP_FILEP );
      if (file != (FILE *)NULL) {
         fseek( file, 0, SEEK_END );
         fgetpos( file, &Offset );
         fwrite( szFileRec, strlen(szFileRec), 1, file );
         fflush( file );
      } /* endif */

// --------------------------------------------------------------------------
// add the record to the container
// --------------------------------------------------------------------------
      InsertCnrRecord( hwnd, Offset, szFileRec, TRUE );
   } /* endif */

   return;
}

// *******************************************************************************
// FUNCTION:     InsertCnrRec
//
// FUNCTION USE: Add a new record to the container
//
// DESCRIPTION:
//
// PARAMETERS:   HWND     container window handle
//               fpos_t   byte position of record in file
//               char*    pointer to record data
//               BOOL     flag indicating the need to repaint after insertion
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void InsertCnrRecord( HWND hwnd, fpos_t FileOffset, char *pszFileRec,
                             BOOL fPaint )
{
   PTRANSACTION   pRec;
   PTRANSACTION   pRecPrev;
   RECORDINSERT   ri;
   HWND           hwndCnr;
   char           *pszPointer;

   do {
// --------------------------------------------------------------------------
// get container window handle
// --------------------------------------------------------------------------
      hwndCnr = WinWindowFromID( hwnd, CID_REGISTER );
      if (hwndCnr == NULLHANDLE) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// allocate a new record for the container
// --------------------------------------------------------------------------
      pRec =  (PTRANSACTION)WinSendMsg( hwndCnr, CM_ALLOCRECORD,
                    (MPARAM)sizeof( TRANSREC ), (MPARAM)1 );
      if (pRec == (PTRANSACTION)NULL) {
         break;
      } /* endif */

      pRec->data.Offset = FileOffset;
      pRec->data.amount = 0;

// --------------------------------------------------------------------------
// copy file buffer to the record
// --------------------------------------------------------------------------
      strcpy( pRec->data.filebuf, pszFileRec );
      pszPointer = pRec->data.filebuf;

// --------------------------------------------------------------------------
// validate and convert the record data.  First, scan past the deleted record
// field.
// --------------------------------------------------------------------------
      while (*pszPointer && *pszPointer != FIELD_SEP) {
         pszPointer++;
      } /* endwhile */

// --------------------------------------------------------------------------
// check for end of record - replace the field separator with a zero
// --------------------------------------------------------------------------
      if (*pszPointer) {
         *(pszPointer++) = '\0';
      } else {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// the date starts at the current point position.  Scan past the date field.
// --------------------------------------------------------------------------
      /* current position is start of date string */
      pRec->data.pszDate = pszPointer;

      while (*pszPointer && *pszPointer != FIELD_SEP) {
         pszPointer++;
      } /* endwhile */

// --------------------------------------------------------------------------
// check for end of record - replace the field separator with a zero
// --------------------------------------------------------------------------
      if (*pszPointer) {
         *(pszPointer++) = '\0';
      } else {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// the reference field starts at the current position.  Scan past the field.
// --------------------------------------------------------------------------
      /* current position is start of reference string */
      pRec->data.pszRef = pszPointer;

      while (*pszPointer && *pszPointer != FIELD_SEP) {
         pszPointer++;
      } /* endwhile */

// --------------------------------------------------------------------------
// check for end of record - replace the field separator with a zero
// --------------------------------------------------------------------------
      if (*pszPointer) {
         *(pszPointer++) = '\0';
      } else {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// the description field starts at the current position.  Scan past the field
// --------------------------------------------------------------------------
      pRec->data.pszDescription = pszPointer;

      while (*pszPointer && *pszPointer != FIELD_SEP) {
         pszPointer++;
      } /* endwhile */

// --------------------------------------------------------------------------
// check for end of record - replace the field separator with a zero.
// --------------------------------------------------------------------------
      if (*pszPointer) {
         *(pszPointer++) = '\0';
      } else {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// The current position is the start of withdrawal string.  Scan past the
// field and convert the string to an integer.
// --------------------------------------------------------------------------
      pRec->data.pszWithdrawal = pszPointer;

      while (*pszPointer && *pszPointer != FIELD_SEP) {
         if (isdigit( *pszPointer )) {
            pRec->data.amount *= 10;
            pRec->data.amount += *pszPointer - '0';
         } /* endif */
         pszPointer++;
      } /* endwhile */
// --------------------------------------------------------------------------
// Change to a negative number for withdrawals
// --------------------------------------------------------------------------
      pRec->data.amount *= -1L;

// --------------------------------------------------------------------------
// check for end of record - replace the field separator with a zero.
// --------------------------------------------------------------------------
      if (*pszPointer) {
         *(pszPointer++) = '\0';
      } else {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// The current position is the start of deposit string.  Scan past the
// field and convert the string to an integer.
// --------------------------------------------------------------------------
      pRec->data.pszDeposit = pszPointer;

      while (*pszPointer && *pszPointer != FIELD_SEP) {
         if (pRec->data.amount >= 0 && isdigit( *pszPointer )) {
            pRec->data.amount *= 10;
            pRec->data.amount += *pszPointer - '0';
         } /* endif */
         pszPointer++;
      } /* endwhile */

// --------------------------------------------------------------------------
// check for end of record - replace the field separator with a zero.
// --------------------------------------------------------------------------
      if (*pszPointer) {
         *(pszPointer++) = '\0';
      } else {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Compute the balance.  Initialize the balance to the amount of the current
// transaction.  Then find the insertion point for the record and, unless
// this is the first record, add the current amount to the previous balance.
// Then convert the balance to a string for display.
// --------------------------------------------------------------------------
      pRec->data.balance = pRec->data.amount;
      pRecPrev = FindInsertionPoint( hwndCnr, pRec );
      if (pRecPrev != (PTRANSACTION)CMA_FIRST) {
         pRec->data.balance += pRecPrev->data.balance;
      } /* endif */
      BalanceString( pRec->data.balance, pRec->data.szBalance );
      pRec->data.pszBalance = pRec->data.szBalance;

// --------------------------------------------------------------------------
// Setup the display characteristics for the record.  ICON view displays the
// reference field as the record text.  The NAME and TEXT views and TREE mode
// use the memo field as the record text unless the memo is blank, in which
// case the reference field is used.
// --------------------------------------------------------------------------
      pRec->record.pszIcon = pRec->data.pszRef;
      pRec->record.pszName = pRec->data.pszDescription;
      pRec->record.pszText = pRec->data.pszDescription;
      pRec->record.pszTree = pRec->data.pszDescription;
      /* if no description - display the reference */
      if (strlen( pRec->record.pszName ) == 0) {
         pRec->record.pszName = pRec->data.pszRef;
         pRec->record.pszText = pRec->data.pszRef;
         pRec->record.pszTree = pRec->data.pszRef;
      } /* endif */

// --------------------------------------------------------------------------
// Now set the handles for the record graphic.  If the transaction is a
// deposit, use the deposit graphics;  If the transaction is a withdrawal
// use the withdrawal graphics.
// --------------------------------------------------------------------------
      if (pRec->data.amount > 0) {
         pRec->record.hptrIcon =
                     (HPOINTER)WinQueryWindowULong( hwnd, WL_DEPOSIT );
         pRec->record.hptrMiniIcon =
                     (HPOINTER)WinQueryWindowULong( hwnd, WL_DEPOSITM );
         pRec->record.hbmBitmap =
                        (HBITMAP)WinQueryWindowULong( hwnd, WL_DEPOSITB );
         pRec->record.hbmMiniBitmap =
                        (HBITMAP)WinQueryWindowULong( hwnd, WL_DEPOSITMB );
      } else {
         pRec->record.hptrIcon =
                     (HPOINTER)WinQueryWindowULong( hwnd, WL_WITHDRAW );
         pRec->record.hptrMiniIcon =
                     (HPOINTER)WinQueryWindowULong( hwnd, WL_WITHDRAWM );
         pRec->record.hbmBitmap =
                        (HBITMAP)WinQueryWindowULong( hwnd, WL_WITHDRAWB );
         pRec->record.hbmMiniBitmap =
                        (HBITMAP)WinQueryWindowULong( hwnd, WL_WITHDRAWMB );
      } /* endif */

// --------------------------------------------------------------------------
// Initialize the RECORDINSERT data structure to place the new record after
// the previous record, update the screen, and place the new record at the
// of the zorder.
// --------------------------------------------------------------------------
      /* Insert the record into the container */
      ri.cb = sizeof( RECORDINSERT );
      ri.pRecordOrder = (PRECORDCORE)((PVOID)pRecPrev);
      ri.pRecordParent = NULL;
      ri.fInvalidateRecord = fPaint;
      ri.zOrder = CMA_TOP;
      ri.cRecordsInsert = 1;

// --------------------------------------------------------------------------
// Finally... insert the record into the container
// --------------------------------------------------------------------------
      WinSendMsg( hwndCnr, CM_INSERTRECORD, pRec, &ri );

// --------------------------------------------------------------------------
// Now scan the records which follow and update the balance for each record.
// --------------------------------------------------------------------------
      UpdateBalance( hwndCnr, pRec, fPaint );

   } while ( false ); /* enddo */

   return;
}

// *******************************************************************************
// FUNCTION:     FindInsertionPoint
//
// FUNCTION USE: Determine where a new record should be inserted.
//
// DESCRIPTION:  Starting at the end of the record chain, read each record
//               until a record with an earlier date is found.
//
// PARAMETERS:   HWND         container window handle
//               PTRANSACTION Container record data to be inserted.
//
// RETURNS:      PTRANSACTION pointer to record just earlier than new
//                            insertion date.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static PTRANSACTION FindInsertionPoint( HWND hwndCnr, PTRANSACTION pRec )
{
   PTRANSACTION   pRecPrev;
   ULONG         usYear = 0;
   ULONG         usMonth = 0;
   ULONG         usDay = 0;
   ULONG         usPrevYear = 0;
   ULONG         usPrevMonth = 0;
   ULONG         usPrevDay = 0;

// --------------------------------------------------------------------------
// convert transaction date for current record to integers
// --------------------------------------------------------------------------
   sscanf( pRec->data.pszDate, "%d/%d/%d", &usMonth, &usDay, &usYear );

// --------------------------------------------------------------------------
// scan the existing records starting with the last record.
// --------------------------------------------------------------------------
   pRecPrev = (PTRANSACTION)WinSendMsg( hwndCnr, CM_QUERYRECORD,
                     (MPARAM)NULL, MPFROM2SHORT( CMA_LAST, CMA_ITEMORDER ));
   while ( pRecPrev != (PTRANSACTION)NULL) {

// --------------------------------------------------------------------------
// convert this record's data to integers
// --------------------------------------------------------------------------
      sscanf( pRecPrev->data.pszDate, "%d/%d/%d", &usPrevMonth, &usPrevDay,
                        &usPrevYear );

// --------------------------------------------------------------------------
// is it earlier or equal to current record?
// --------------------------------------------------------------------------
      if (usPrevYear < usYear) {
         break;
      } else if (usPrevYear == usYear) {
         if (usPrevMonth < usMonth) {
            break;
         } else if (usPrevMonth == usMonth) {
            if (usPrevDay <= usDay) {
               break;
            } /* endif */
         } /* endif */
      } /* endif */

// --------------------------------------------------------------------------
// get the next previous record
// --------------------------------------------------------------------------
      pRecPrev = (PTRANSACTION)WinSendMsg( hwndCnr, CM_QUERYRECORD,
                        (MPARAM)pRecPrev, MPFROM2SHORT( CMA_PREV, CMA_ITEMORDER ));
   } /* endwhile */

// --------------------------------------------------------------------------
// No earlier records, set the record pointer to the FIRST record.
// --------------------------------------------------------------------------
   if (pRecPrev == (PTRANSACTION)NULL) {
      pRecPrev = (PTRANSACTION)CMA_FIRST;
   } /* endif */

// --------------------------------------------------------------------------
// return the previous record pointer.
// --------------------------------------------------------------------------
   return pRecPrev;
}

// *******************************************************************************
// FUNCTION:     UpdateBalance
//
// FUNCTION USE: Adjust balances after a record is inserted.
//
// DESCRIPTION:  Scan all records following the inserted record and recompute
//               the balance for each.
//
// PARAMETERS:   HWND         container window handle
//               PTRANACTION  record data for record just inserted.
//               BOOL         flag to indicate if data should be repainted.
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void UpdateBalance( HWND hwndCnr, PTRANSACTION pRec, BOOL fPaint )
{
   PTRANSACTION      pRecNext;

// --------------------------------------------------------------------------
// get the next record from the container to start things off
// --------------------------------------------------------------------------
   pRecNext = (PTRANSACTION)WinSendMsg( hwndCnr, CM_QUERYRECORD,
                     (MPARAM)pRec, MPFROM2SHORT( CMA_NEXT, CMA_ITEMORDER ));
   while ( pRecNext != (PTRANSACTION)NULL) {

// --------------------------------------------------------------------------
// at the amount of the next record to the current record's balance to
// obtain the new balance for the next record and build the string for
// the balance amount.
// --------------------------------------------------------------------------
      pRecNext->data.balance = pRec->data.balance + pRecNext->data.amount;
      BalanceString( pRecNext->data.balance, pRecNext->data.szBalance );

// --------------------------------------------------------------------------
// Change current record to next record and then get a new next record.
// --------------------------------------------------------------------------
      pRec = pRecNext;
      pRecNext = (PTRANSACTION)WinSendMsg( hwndCnr, CM_QUERYRECORD,
                        (MPARAM)pRec, MPFROM2SHORT( CMA_NEXT, CMA_ITEMORDER ));
   } /* endwhile */

// --------------------------------------------------------------------------
// Done - if paint requested, invalidate the container data
// --------------------------------------------------------------------------
   if (fPaint) {
      WinSendMsg( hwndCnr, CM_INVALIDATERECORD, (MPARAM)NULL,
                        MPFROM2SHORT( 0, CMA_TEXTCHANGED) );
   } /* endif */

// --------------------------------------------------------------------------
// done - return
// --------------------------------------------------------------------------
   return;
}

// *******************************************************************************
// FUNCTION:     CopyFile
//
// FUNCTION USE: Save as the contents of the current file to a new file
//
// DESCRIPTION:  Checks to see if the new file already exists.  If it does the
//               user must confirm overwriting the file.  The contents of the
//               currently open file are then written to the new fle.
//
// PARAMETERS:   HWND         client window handle
//               char *       name of new file
//
// RETURNS:      BOOL         TRUE for a successful copy
//
// INTERNALS:    NONE
//
// *******************************************************************************
static BOOL CopyFile( HWND hwnd, char *szFile )
{
   FILE     *fSrc = (FILE *)NULL;
   FILE     *fDst = (FILE *)NULL;
   char     szBuffer[ 1024 ];
   char     szMessage[ FILENAME_MAX * 2 ];
   ULONG    rc;
   size_t   nRead;
   BOOL     bCopyOK;


   do {
// --------------------------------------------------------------------------
// get the stream pointer for the current file from the window data.
// --------------------------------------------------------------------------
      fSrc = WinQueryWindowPtr( hwnd, WP_FILEP );
      if (fSrc == (FILE *)NULL) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// open the new file for read - this will error when the file does not exist.
// --------------------------------------------------------------------------
      fDst = fopen( szFile, "rb+" );
      if (fDst != (FILE *)NULL) {

// --------------------------------------------------------------------------
// Get user confirmation before overwriting an existing file
// --------------------------------------------------------------------------
         sprintf( szMessage, "%s already exists.  Continuing will cause the "
                  "file to be overwritten. Continue?", szFile );
         rc = WinMessageBox( HWND_DESKTOP, hwnd, szMessage, TITLEBAR,
                  RID_MSG_FILEEXISTS, MB_ICONQUESTION | MB_YESNO );

// --------------------------------------------------------------------------
// Close the destination file in read mode and return if not overwrite
// --------------------------------------------------------------------------
         fclose( fDst );
         if (rc == MBID_NO) {
            return FALSE;
         } /* endif */
      } /* endif */

// --------------------------------------------------------------------------
// Open the file in write mode (creating if the file doesn't already exist).
// --------------------------------------------------------------------------
      fDst = fopen( szFile, "wb+" );
      if (fDst != (FILE *)NULL) {

// --------------------------------------------------------------------------
// move to the beginning of the source file and assume this will work OK.
// --------------------------------------------------------------------------
         fseek( fSrc, 0, SEEK_SET );
         bCopyOK = TRUE;

// --------------------------------------------------------------------------
// read data from the source file and then write the number of bytes read
// back out to the destination file.  Continue until all data has been
// copied.
//
// NOTE: This type of operation (in fact all the file operations) really
//       belong in a separate thread.
// --------------------------------------------------------------------------
         while (!feof(fSrc)) {
            nRead = fread(szBuffer, sizeof(char), sizeof(szBuffer), fSrc);
            if (nRead != 0) {
               if (!fwrite( szBuffer, sizeof(char), nRead, fDst)) {
                  bCopyOK = FALSE;
               } /* endif */
            } else {
               bCopyOK = FALSE;
            } /* endif */
         } /* endwhile */

// --------------------------------------------------------------------------
// break out if an error occurred.
// --------------------------------------------------------------------------
         if (!bCopyOK) {
            break;
         } /* endif */

// --------------------------------------------------------------------------
// close the destination and return success.
// --------------------------------------------------------------------------
         fclose( fDst );
         return TRUE;
      } /* endif */
   } while ( false  ); /* enddo */

// --------------------------------------------------------------------------
// A failure occurred - notify the user, cleanup, and return failure.
// --------------------------------------------------------------------------
   DisplayMessages( NULLHANDLE, "Error saving new file", MSG_ERROR );
   if (fDst != (FILE *)NULL) {
      fclose(fDst);
      remove(szFile);
   } /* endif */

   return FALSE;
}

// *******************************************************************************
// FUNCTION:     IsValidFile
//
// FUNCTION USE: Verifies that the user has not entered the reserved file name
//               used for a new, unsaved file.
//
// DESCRIPTION:  Removes the path information, then compares the name and
//               extension to the name of the untitled file.  If the name
//               matches, a message may be displayed to inform the user.
//
// PARAMETERS:   HWND         client window handle
//               char *       name of new file
//               BOOL         flag indicating if message shown for invalid name
//
// RETURNS:      BOOL         TRUE if filename is valid
//
// INTERNALS:    NONE
//
// *******************************************************************************
static BOOL IsValidFile( HWND hwnd, char *szFile, BOOL bShowMsg )
{
   char     szMessage[ FILENAME_MAX * 2 ];
   char     *szFileName;
   BOOL     retval = FALSE;

   do {
// --------------------------------------------------------------------------
// find the name & extension portion of the file
// --------------------------------------------------------------------------
      szFileName = strrchr( szFile, (int)'\\' );
      if (szFileName == (char *)NULL) {
         szFileName = szFile;
      } else {
         szFileName++;
      } /* endif */

// --------------------------------------------------------------------------
// check for same name as new file, "untitled"
// --------------------------------------------------------------------------
      if (!stricmp(szFileName, szNewFile)) {
         if (bShowMsg) {
            sprintf( szMessage, "%s cannot be used as a file name - try again",
                     szFileName );
            DisplayMessages( NULLHANDLE, szMessage, MSG_WARNING );
         } /* endif */
      } else {

// --------------------------------------------------------------------------
// valid file - set successful return code
// --------------------------------------------------------------------------
         retval = TRUE;
      } /* endif */
   } while ( false ); /* enddo */

// --------------------------------------------------------------------------
// return result back to the caller
// --------------------------------------------------------------------------
   return retval;
}

// *******************************************************************************
// FUNCTION:     DeleteTransaction
//
// FUNCTION USE: Remove a record from the container and the database
//
// DESCRIPTION:  After obtaining the record pointer, update the balances for
//               subsequent records, mark the record as deleted in the file
//               and remove the record from the container.
//
// PARAMETERS:   HWND         client window handle
//               PTRANSACTION pointer to the container record to delete, NULL
//                            pointer is a flag to delete the selected record.
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void DeleteTransaction( HWND hwnd, PTRANSACTION pRec )
{
   HWND           hwndCnr;
   FILE           *file;
   PTRANSACTION   pRecPrev;

   do {
// --------------------------------------------------------------------------
// Get the container window handle
// --------------------------------------------------------------------------
      hwndCnr = WinWindowFromID( hwnd, CID_REGISTER );
      if (hwndCnr == NULLHANDLE) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// get a pointer to the record to delete.  If NULL was passed in then try
// to get the selected record in the container.
// --------------------------------------------------------------------------
      if (pRec == (PTRANSACTION)NULL) {
         /* Find the selected record */
         pRec = (PTRANSACTION)WinQueryWindowPtr( hwnd, WP_SELRECORD );
         if (pRec == (PTRANSACTION)NULL) {
            break;
         } /* endif */
      } /* endif */

// --------------------------------------------------------------------------
// Get the file stream pointer from the window data
// --------------------------------------------------------------------------
      file = (FILE *)WinQueryWindowPtr( hwnd, WP_FILEP );
      if (file == (FILE *)NULL) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Update the file record data to mark the record as deleted, then write the
// data back to the proper location in the file.
// --------------------------------------------------------------------------
      pRec->data.filebuf[ 0 ] = 'D';
      fsetpos( file, &pRec->data.Offset );
      if (!fwrite( pRec->data.filebuf, sizeof( char ), 1, file)) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Get the previous record in the file.  If there is no previous record, then
// set the previous record to the next record (since this must be the first
// record) and set the records balance to its amount.
// --------------------------------------------------------------------------
      pRecPrev = (PTRANSACTION)WinSendMsg( hwndCnr, CM_QUERYRECORD,
                        (MPARAM)pRec, MPFROM2SHORT( CMA_PREV, CMA_ITEMORDER ));
      if (pRecPrev == (PTRANSACTION)NULL) {
         pRecPrev = (PTRANSACTION)WinSendMsg( hwndCnr, CM_QUERYRECORD,
                           (MPARAM)pRec, MPFROM2SHORT( CMA_NEXT, CMA_ITEMORDER ));
         if (pRecPrev != (PTRANSACTION)NULL) {
            pRecPrev->data.balance = pRecPrev->data.amount;
            BalanceString( pRecPrev->data.balance, pRecPrev->data.szBalance );
         } /* endif */
      } /* endif */

// --------------------------------------------------------------------------
// Remove the record from the container and free it's space
// --------------------------------------------------------------------------
      WinSendMsg( hwndCnr, CM_REMOVERECORD, MPFROMP(&pRec),
                     MPFROM2SHORT( 1, CMA_FREE ));

// --------------------------------------------------------------------------
// Now scan all following records and adjust the balance for each.
// --------------------------------------------------------------------------
      if (pRecPrev != (PTRANSACTION)NULL) {
         UpdateBalance( hwndCnr, pRecPrev, TRUE );
      } else {
         WinSendMsg( hwndCnr, CM_INVALIDATERECORD, MPFROMP(NULL),
                     MPFROM2SHORT( 0, CMA_TEXTCHANGED ));
      } /* endif */

   } while ( false ); /* enddo */

// --------------------------------------------------------------------------
// back to the caller
// --------------------------------------------------------------------------
   return;
}

// *******************************************************************************
// FUNCTION:     BalanceString
//
// FUNCTION USE: Converts the balance to a string.
//
// DESCRIPTION:  Formats a dollars and cents string by interpreting the balance
//               amount as a 2 decimal place fixed point number
//
// PARAMETERS:   LONG         amount to be converted.
//               char *       pointer to location to stored converted string.
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void BalanceString( LONG balance, char *BalanceString )
{
      LONG  lDollars;
      LONG  lCents;
      char  cSign = ' ';

// --------------------------------------------------------------------------
// Make balance positive but preserve sign for later
// --------------------------------------------------------------------------
      if (balance < 0) {
         balance *= -1L;
         cSign = '-';
      } /* endif */

// --------------------------------------------------------------------------
// Break balance apart into dollars and cents
// --------------------------------------------------------------------------
      lDollars = balance / 100;
      lCents = balance % 100;

// --------------------------------------------------------------------------
// Format the results into a string.
// --------------------------------------------------------------------------
      sprintf( BalanceString, "%c%ld.%02ld", cSign, lDollars, lCents );

// --------------------------------------------------------------------------
// Add commas to the string for display usage.
// --------------------------------------------------------------------------
      IsValidAmount( BalanceString );
}

// *******************************************************************************
// FUNCTION:     SetCnrView
//
// FUNCTION USE: Modifies the current view of the container
//
// DESCRIPTION:  Modifies the flWindowAttr field of the container data to
//               match the current selections in the app's view menu.
//
// PARAMETERS:   HWND      client window handle
//               HWND      menu window handle
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void SetCnrView( HWND hwnd, HWND hwndMenu )
{
   HWND     hwndCnr;
   CNRINFO  ci;

   do {
// --------------------------------------------------------------------------
// Get the container window handle
// --------------------------------------------------------------------------
      hwndCnr = WinWindowFromID( hwnd, CID_REGISTER );
      if (hwndCnr == NULLHANDLE) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Get the current container data
// --------------------------------------------------------------------------
      if (!WinSendMsg( hwndCnr, CM_QUERYCNRINFO, MPFROMP(&ci),
                       MPFROMLONG(sizeof(CNRINFO)))) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Set/Clear the ICON view bit
// --------------------------------------------------------------------------
      if (WinIsMenuItemChecked( hwndMenu, MID_VIEWICON)) {
         ci.flWindowAttr |= CV_ICON;
      } else {
         ci.flWindowAttr &= ~(ULONG)CV_ICON;
      } /* endif */

// --------------------------------------------------------------------------
// Set/Clear the NAME view bit
// --------------------------------------------------------------------------
      if (WinIsMenuItemChecked( hwndMenu, MID_VIEWNAME)) {
         ci.flWindowAttr |= CV_NAME;
      } else {
         ci.flWindowAttr &= ~(ULONG)CV_NAME;
      } /* endif */

// --------------------------------------------------------------------------
// Set/Clear the TEXT view bit
// --------------------------------------------------------------------------
      if (WinIsMenuItemChecked( hwndMenu, MID_VIEWTEXT)) {
         ci.flWindowAttr |= CV_TEXT;
      } else {
         ci.flWindowAttr &= ~(ULONG)CV_TEXT;
      } /* endif */

// --------------------------------------------------------------------------
// Set/Clear the DETAIL view bit
// --------------------------------------------------------------------------
      if (WinIsMenuItemChecked( hwndMenu, MID_VIEWDETAIL)) {
         ci.flWindowAttr |= CV_DETAIL;
      } else {
         ci.flWindowAttr &= ~(ULONG)CV_DETAIL;
      } /* endif */

// --------------------------------------------------------------------------
// Set/Clear the TREE mode bit
// --------------------------------------------------------------------------
      if (WinIsMenuItemEnabled( hwndMenu, MID_VIEWTREE) &&
          WinIsMenuItemChecked( hwndMenu, MID_VIEWTREE)) {
         ci.flWindowAttr |= CV_TREE;
      } else {
         ci.flWindowAttr &= ~(ULONG)CV_TREE;
      } /* endif */

// --------------------------------------------------------------------------
// Set/Clear the MINI mode bit
// --------------------------------------------------------------------------
      if (WinIsMenuItemEnabled( hwndMenu, MID_VIEWMINI) &&
          WinIsMenuItemChecked( hwndMenu, MID_VIEWMINI)) {
         ci.flWindowAttr |= CV_MINI;
      } else {
         ci.flWindowAttr &= ~(ULONG)CV_MINI;
      } /* endif */

// --------------------------------------------------------------------------
// Set/Clear the FLOW mode bit
// --------------------------------------------------------------------------
      if (WinIsMenuItemEnabled( hwndMenu, MID_VIEWFLOW) &&
          WinIsMenuItemChecked( hwndMenu, MID_VIEWFLOW)) {
         ci.flWindowAttr |= CV_FLOW;
      } else {
         ci.flWindowAttr &= ~(ULONG)CV_FLOW;
      } /* endif */

// --------------------------------------------------------------------------
// Set/Clear the DRAW ICON bit
// --------------------------------------------------------------------------
      if (WinIsMenuItemEnabled( hwndMenu, MID_VIEWTYPE) &&
          WinIsMenuItemChecked( hwndMenu, MID_VIEWICONS)) {
         ci.flWindowAttr |= CA_DRAWICON;
      } else {
         ci.flWindowAttr &= ~(ULONG)CA_DRAWICON;
      } /* endif */

// --------------------------------------------------------------------------
// Set/Clear the DRAW BITMAP bit
// --------------------------------------------------------------------------
      if (WinIsMenuItemEnabled( hwndMenu, MID_VIEWTYPE) &&
          WinIsMenuItemChecked( hwndMenu, MID_VIEWBITMAPS)) {
         ci.flWindowAttr |= CA_DRAWBITMAP;
      } else {
         ci.flWindowAttr &= ~(ULONG)CA_DRAWBITMAP;
      } /* endif */

// --------------------------------------------------------------------------
// Update the flWindowAttr in the container data
// --------------------------------------------------------------------------
      WinSendMsg( hwndCnr, CM_SETCNRINFO, MPFROMP( &ci),
                  MPFROMLONG( CMA_FLWINDOWATTR ));

   } while ( false ); /* enddo */

// --------------------------------------------------------------------------
// back to the caller
// --------------------------------------------------------------------------
   return;
}

// *******************************************************************************
// FUNCTION:     LoadBitmap
//
// FUNCTION USE: Creates a bitmap from the application's resources for use in
//               painting the container's background.
//
// DESCRIPTION:  Uses GpiLoadBitmap to load a bitmap from the application's
//               resources and stretch the bitmap to fill a given area.
//
// PARAMETERS:   HPS       handle of presentation space in which to load bitmap
//               PRECTL    pointer to rectangle for area to fill
//
// RETURNS:      HBITMAP   handle of the created bitmap
//
// INTERNALS:    NONE
//
// *******************************************************************************
HBITMAP LoadBitmap( HPS hps, PRECTL prectl )
{
   HBITMAP     hbm;

// --------------------------------------------------------------------------
// Get the container bitmap from the resources.
// --------------------------------------------------------------------------
   hbm = GpiLoadBitmap( hps, NULLHANDLE, ID_CNRBITMAP,
                        prectl->xRight - prectl->xLeft,
                        prectl->yTop - prectl->yBottom );

// --------------------------------------------------------------------------
// return the bitmap handle
// --------------------------------------------------------------------------
   return hbm;
}

// *******************************************************************************
// FUNCTION:     cnrSubclassProc
//
// FUNCTION USE: Subclass procedure for managing drawing of the container's
//               background.
//
// DESCRIPTION:  Processes the CM_PAINTBACKGROUND message to fill the container
//               background with a bitmap.  Scroll and sizing messages are
//               also processed to properly maintain the bitmap.
//
// PARAMETERS:   HWND     container window handle
//               ULONG    window message
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  dependent on the message received
//
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT APIENTRY cnrSubclassProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   POWNERBACKGROUND     pob = (POWNERBACKGROUND)mp1;
   static HBITMAP       hbm = NULLHANDLE;
   RECTL                rectl;
   POINTL               ptl;
   MRESULT              rc;

// --------------------------------------------------------------------------
// Draw the background of the bitmap
// --------------------------------------------------------------------------
   if (msg == CM_PAINTBACKGROUND)
   {
// --------------------------------------------------------------------------
// only draw the main window or the left and right column windows.
// --------------------------------------------------------------------------
      if( pob->idWindow != CID_REGISTER &&
          pob->idWindow != CID_LEFTDVWND &&
          pob->idWindow != CID_RIGHTDVWND ) return MRFROMLONG(FALSE);

// --------------------------------------------------------------------------
// Load the background bitmap if not already loaded.
// --------------------------------------------------------------------------
      if( hbm == NULLHANDLE ) {
         WinQueryWindowRect( hwnd, &rectl );
         hbm = LoadBitmap(pob->hps, &rectl);
      }

// --------------------------------------------------------------------------
// Draw the bitmap onto the background
// --------------------------------------------------------------------------
      ptl.x = pob->rclBackground.xLeft;
      ptl.y = pob->rclBackground.yBottom;
      WinDrawBitmap( pob->hps, hbm, &(pob->rclBackground), &ptl,
                     0l, 0l, DBM_NORMAL );

// --------------------------------------------------------------------------
// indicate that the background was drawn
// --------------------------------------------------------------------------
      return MRFROMLONG(TRUE);
   }

// --------------------------------------------------------------------------
// prevent updates during scrolling to keep the bitmap in sync.
// --------------------------------------------------------------------------
   if (msg == WM_VSCROLL || msg == WM_HSCROLL) {
      WinLockWindowUpdate( HWND_DESKTOP, hwnd );
      rc = (*pfnCnrProc)(hwnd, msg, mp1, mp2 );
      WinLockWindowUpdate( HWND_DESKTOP, NULLHANDLE );
      return rc;
   }

// --------------------------------------------------------------------------
// When size changes - kill the bitmap so it can be reloaded at the correct
// new size.
// --------------------------------------------------------------------------
   if (msg == WM_SIZE ) {
      GpiDeleteBitmap( hbm );
      hbm = NULLHANDLE;
   }

// --------------------------------------------------------------------------
// let all other messages flow through to the normal container window proc.
// --------------------------------------------------------------------------
   return (*pfnCnrProc)(hwnd, msg, mp1, mp2);
}


// *******************************************************************************
// FUNCTION:     BackBitmapDlgProc
//
// FUNCTION USE: The dialog code to handle the background bitmap
//
// DESCRIPTION:  This dialog box allows the user to configure whether or 
//               not they want to see the OS/2 Warp Presentation Manager for 
//               Power Programmers bitmap when the application is started.
//
// PARAMETERS:   this right usCounter will contain:
//               CHAR *     on exit contains X:\DIRECTORY
//               CHAR *     on exit contains FILE.TXT
//
// RETURNS:      MRESULT
//
// INTERNALS:    DisplayMessages
//               SetTheSysMenu
//
// HISTORY:
//
// *******************************************************************************
MRESULT EXPENTRY BackBitmapDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 USHORT   usCheckState;
 USHORT   rc;
 LONG     lReturn;

 switch(msg)
  {
   case WM_INITDLG:
// --------------------------------------------------------------------------          
// Remove the system menu menuitems that are not needed by this dialog
// --------------------------------------------------------------------------                  
        SetTheSysMenu(hwnd, NULL);

// --------------------------------------------------------------------------          
// Check to see whether the background bitmap option is enabled or disabled 
// and check the appropriate radiobutton.
// --------------------------------------------------------------------------                  
        lReturn = PrfQueryProfileInt(HINI_USER, APPNAME, KEYNAME, 0);
        if (lReturn == LOSEIT)
         {
          WinCheckButton(hwnd, IDC_LOSEIT, TRUE);
         }

        // Use the background bitmap is the default
        else
         {
          WinCheckButton(hwnd, IDC_USEIT, TRUE); 
         }
        return FALSE;

   case WM_COMMAND:
        switch(SHORT1FROMMP(mp1))
         {
          case DID_OK:

// --------------------------------------------------------------------------   
// This processing allows the user to determine whether or not they want    
// to see the background bitmap when the program is started.  If the user
// selects the LOSEIT radiobutton, the background bitmap will not be 
// displayed on startup.  If the USEIT radiobutton is selected the background
// bitmap will be displayed on startup.  The default option is to display
// the bitmap.  The changes are written to the OS2.INI file when the user 
// presses the OK pushbutton.
// --------------------------------------------------------------------------   

               usCheckState = WinQueryButtonCheckstate(hwnd, IDC_USEIT);

               switch (usCheckState)
                {
                 case LOSEIT:  
                      rc = PrfWriteProfileString(HINI_USER, APPNAME, KEYNAME, OPTION_NO);
                      if (!rc)
                       {
                        DisplayMessages(NULLHANDLE, "Error writing profile information", MSG_EXCLAMATION);
                       }
                      break;

                 case USEIT:    
                      rc = PrfWriteProfileString(HINI_USER, APPNAME, KEYNAME, OPTION_YES);
                      if (!rc)
                       {
                        DisplayMessages(NULLHANDLE, "Error writing profile information", MSG_EXCLAMATION);
                       }
                      break;
                } // End of switch
               WinDismissDlg(hwnd, TRUE);
               return FALSE;

          case DID_CANCEL:
               WinDismissDlg(hwnd, TRUE);
               return FALSE;

          case DID_HELP:
               DisplayMessages(NULLHANDLE, "If you need help with this dialog, then you are a real dummy!!!", MSG_INFO);
               return FALSE;

          default:
               return FALSE;
         }
        break;

   default:
        return WinDefDlgProc(hwnd, msg, mp1, mp2);
  }
 return FALSE;
}

