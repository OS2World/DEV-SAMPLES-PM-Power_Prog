// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      PMED.C                                           mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Main Source File for PMED/Chapter 03 Sample Program           บ
// บ                                                                               บ
// บ  NOTES:         A powerful text editor that demonstrates keyboard and         บ
// บ                 scrolling concepts.                                           บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#define INCL_PM
#include <os2.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pmed.h"
#include "pmeddlg.h"
#include "fileops.h"
#include "string.h"
#include "shcommon.h"

// --------------------------------------------------------------------------
// Miscellaneous constant definitions used only in this source module.
// --------------------------------------------------------------------------
#define  APP_DATA_POINTER           0
#define  APP_WINDOW_DATA_SIZE       4
#define  DEFAULT_FONT               "10.System VIO"
#define  APP_CLASS_NAME             "Application"
#define  TITLE_BAR_TEXT             "PMED - "
#define  NOFILE_NAME                ".Untitled"
#define  HELP_WINDOW_TITLE          "Help for PMED"
#define  HELPLIB_FILENAME           "pmed.hlp"

#define  DELETE_BACKWARD            1
#define  DELETE_FORWARD             2

#define  CURSOR_UP                  1
#define  CURSOR_DOWN                2
#define  CURSOR_LEFT                3
#define  CURSOR_RIGHT               4
#define  CURSOR_CURRENT             5

// --------------------------------------------------------------------------
// This structure is used to hold the text for an individual line of the file
// and contains the pointers used to maintain a linked list of lines.
// --------------------------------------------------------------------------
typedef struct __TEXT_LINE__ {
   struct __TEXT_LINE__ *next;       // pointer to next in linked list
   struct __TEXT_LINE__ *previous;   // pointer to previous in linked list
   LONG                 lLineNumber; // index of this line
   PSZ                  pszText;     // text of this line
} TEXTLINE, *PLINE;

// --------------------------------------------------------------------------
// This structure is the editor control structure and maintains the current
// state of the editor throughout processing.
// --------------------------------------------------------------------------
typedef struct __APP_DATA__ {
   SHORT          cb;            // size of structure
   PAPPFILE       pfile;         // pointer to file control structure
   PLINE          plineFirst;    // pointer to first line structure
   PLINE          plineLast;     // pointer to last line structure
   PLINE          *pLines;       // array of pointers to line structures
   SHORT          sCurTop;       // index of line at top of window
   SHORT          sCurLine;      // index of current line to edit
   LONG           lCurCol;       // index of current column in line
   LONG           lCurLeft;      // index of column at left of window
   SHORT          sNumLines;     // total number of lines in the file
   LONG           lMaxWidth;     // total number of columns in longest line
   LONG           lHeight;       // height of a line
   LONG           lWidth;        // width of a column
   SHORT          sLinesPerPage; // number of lines displayable in window
   LONG           lColsPerPage;  // number of columns displayable in window
   BOOL           fFlags;        // state flags
   PSZ            pszFileName;   // name of current open file
} APP_DATA, *PAPP_DATA;

// --------------------------------------------------------------------------
// Valid flags for the fFlags element of the editor control structure
// --------------------------------------------------------------------------
#define FLAG_REPLACE          0x00000001
#define FLAG_CHANGED          0x00000002
#define FLAG_CLOSING          0x00000004

// --------------------------------------------------------------------------
// A global variable with the value FALSE, used to prevent compiler warnings
// in loops which are terminated from within.
// --------------------------------------------------------------------------
BOOL  false = FALSE;

// --------------------------------------------------------------------------
// Main Client Window Procedure
// --------------------------------------------------------------------------
MRESULT APIENTRY AppWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT APIENTRY NotSavedDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------
// Message processing procedures
// --------------------------------------------------------------------------
static MRESULT hmQueryKeysHelp( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmChar( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmClose( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmCommand( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmDestroy( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmHScroll( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmPaint( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmSetFocus( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmSize( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmVScroll( HWND hwnd, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------
// Application command procedures
// --------------------------------------------------------------------------
static MRESULT MidFileNew(HWND hwnd, MPARAM mp1, MPARAM mp2);
static MRESULT MidFileOpen(HWND hwnd, MPARAM mp1, MPARAM mp2);
static MRESULT MidFileSave(HWND hwnd, MPARAM mp1, MPARAM mp2);
static MRESULT MidFileSaveas(HWND hwnd, MPARAM mp1, MPARAM mp2);
static MRESULT MidFileExit(HWND hwnd, MPARAM mp1, MPARAM mp2);
static MRESULT MidHelpGen(HWND hwnd, MPARAM mp1, MPARAM mp2);
static MRESULT MidHelpUsing(HWND hwnd, MPARAM mp1, MPARAM mp2);
static MRESULT MidHelpKeys(HWND hwnd, MPARAM mp1, MPARAM mp2);
static MRESULT MidHelpProd(HWND hwnd, MPARAM mp1, MPARAM mp2);

// --------------------------------------------------------------------------
// Other editor functions
// --------------------------------------------------------------------------
static void ReadText(PAPP_DATA pAppData);
static void WriteText(PAPP_DATA pAppData);
static void CalcCurrentPos(HWND hwnd, PAPP_DATA pAppData, PLONG plx, PLONG ply); 
static void InsertCharacter(HWND hwnd, char chInsert);
static void DeleteCharacter(HWND hwnd, ULONG ulDirection);
static void UpdateCursor(HWND hwnd, PAPP_DATA pAppData);
static void InsertNewLine(HWND hwnd, PAPP_DATA pAppData);
static void DeleteLine(HWND hwnd, PAPP_DATA pAppData);
static void ChangeFileName(HWND hwnd, PAPP_DATA pAppData, PSZ pszNewName);
static void RecalcScrollRange(HWND hwnd, PAPP_DATA pAppData);

// --------------------------------------------------------------------------
// Linked list processing functions
// --------------------------------------------------------------------------
static void DestroyList( PAPP_DATA pAppData );
static void DeleteListElement( PAPP_DATA pAppData, PLINE plineItem );
static PLINE InsertListElement(PAPP_DATA pAppData, char *pText, ULONG cbText ); 

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

// --------------------------------------------------------------------------
// Frame has no accelerator table or icon. Scroll bars are enabled.
// --------------------------------------------------------------------------
   ULONG flCreateFlags = (FCF_STANDARD & ~ FCF_ACCELTABLE) |
                         FCF_VERTSCROLL | FCF_HORZSCROLL;

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
   hInit.phtHelpTable = (PHELPTABLE)MAKELONG( APP_HELP_TABLE, 0xffff );
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
   if (hwndHelp != NULLHANDLE) {
      WinAssociateHelpInstance( NULLHANDLE, hwndFrame );
      WinDestroyHelpInstance( hwndHelp );
   } /* endif */

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
MRESULT APIENTRY AppWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg) 
    {
     case HM_QUERY_KEYS_HELP:   
          return hmQueryKeysHelp(hwnd, mp1, mp2); 

     case WM_CHAR:              
          return wmChar(hwnd, mp1, mp2); 

     case WM_CLOSE:             
          return wmClose(hwnd, mp1, mp2); 

     case WM_COMMAND:           
          return wmCommand(hwnd, mp1, mp2); 

     case WM_CREATE:            
          return wmCreate(hwnd, mp1, mp2); 

     case WM_DESTROY:           
          return wmDestroy(hwnd, mp1, mp2); 

     case WM_HSCROLL:           
          return wmHScroll(hwnd, mp1, mp2); 

     case WM_PAINT:             
          return wmPaint(hwnd, mp1, mp2); 

     case WM_SETFOCUS:          
          return wmSetFocus(hwnd, mp1, mp2); 

     case WM_SIZE:              
          return wmSize(hwnd, mp1, mp2); 

     case WM_VSCROLL:          
          return wmVScroll(hwnd, mp1, mp2); 

     default:                   
          return WinDefWindowProc( hwnd, msg, mp1, mp2 ); 

    } // endswitch 
}

// *******************************************************************************
// FUNCTION:     hmQueryKeysHelp
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
static MRESULT hmQueryKeysHelp( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   return MRFROMSHORT( HID_KEYS_HELP );
}

// *******************************************************************************
// FUNCTION:     wmChar
//
// FUNCTION USE: Process WM_CHAR messages for the application client window
//
// DESCRIPTION:  This routine examines the message to determine if a
//               the character received is either a recognized special
//               character or a keystroke which is inserted into the
//               text.
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
static MRESULT wmChar( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   USHORT      fsflags = SHORT1FROMMP( mp1 );  // pull out keystoke flags
   MRESULT     mrReturn = FALSE;               // assume we won't process the char
   PAPP_DATA   pAppData = NULL;                // app control structure
   LONG        lCurX;                          // cursor x coordinate
   LONG        lCurY;                          // cursor y position
   LONG        lCurcX;                         // cursor width

// --------------------------------------------------------------------------
// Retrieve the pointer to the application control structure from the
// window instance data.  If the structure has for some reason not yet
// been established, don't process the keystroke.
// --------------------------------------------------------------------------
   pAppData = (PAPP_DATA)WinQueryWindowPtr( hwnd, APP_DATA_POINTER );
   if (pAppData != (PAPP_DATA)NULL ) {

// --------------------------------------------------------------------------
// Only process keys on the downstroke
// --------------------------------------------------------------------------
      if (!(fsflags & KC_KEYUP)) {
         mrReturn = (MRESULT)TRUE;        // assume processed for now

// --------------------------------------------------------------------------
// Process CTRL+VIRTUALKEY keystrokes (make sure neither ALT or SHIFT
// are pressed).
// --------------------------------------------------------------------------
         if (fsflags & KC_VIRTUALKEY && fsflags & KC_CTRL &&
             !(fsflags & (KC_ALT | KC_SHIFT))) {
            switch (CHAR3FROMMP(mp2)) {

// --------------------------------------------------------------------------
// CTRL+PAGE UP:   standard behavior is to scroll one page left.
//                 Calculate the new input position and then scroll the data
//                 displayed in the window with a scroll bar message
// --------------------------------------------------------------------------
            case VK_PAGEUP:
               pAppData->lCurCol -= pAppData->lColsPerPage;
               if (pAppData->lCurCol < 0) pAppData->lCurCol = 0;
               WinSendMsg( hwnd, WM_HSCROLL, (MPARAM)FID_HORZSCROLL,
                           MPFROM2SHORT( 0, SB_PAGELEFT ));
               break;

// --------------------------------------------------------------------------
// CTRL+PAGE DOWN: standard behavior is to scroll one page right.
//                 Calculate the new input position and then scroll the data
//                 displayed in the window with a scroll bar message
// --------------------------------------------------------------------------
            case VK_PAGEDOWN:
               pAppData->lCurCol += pAppData->lColsPerPage;
               if (pAppData->lCurCol > pAppData->lMaxWidth)
                  pAppData->lCurCol = pAppData->lMaxWidth;
               WinSendMsg( hwnd, WM_HSCROLL, (MPARAM)FID_HORZSCROLL,
                           MPFROM2SHORT( 0, SB_PAGERIGHT ));
               break;

// --------------------------------------------------------------------------
// CTRL+HOME:      standard behavior is to move the cursor to the start of
//                 the data.  Set new input position and then send scroll
//                 message to move to top and left of data.
// --------------------------------------------------------------------------
            case VK_HOME:
               pAppData->sCurLine = 0;
               pAppData->lCurCol = 0;
               WinSendMsg( hwnd, WM_VSCROLL, (MPARAM)FID_VERTSCROLL,
                           MPFROM2SHORT( 0, SB_SLIDERTRACK ));
               WinSendMsg( hwnd, WM_HSCROLL, (MPARAM)FID_HORZSCROLL,
                           MPFROM2SHORT( 0, SB_SLIDERTRACK ));
               break;

// --------------------------------------------------------------------------
// CTRL+END:       standard behavior is to scroll to the end of the data.
//                 We could just but the window position such that the last
//                 character is at the top left of the display.  But it is
//                 much more useful to display as much data as possible, so
//                 we calculate a position that moves the last character
//                 as near the bottom right of the display as possible.
// --------------------------------------------------------------------------
            case VK_END:
               pAppData->sCurLine = pAppData->sNumLines - 1;
               pAppData->lCurCol = strlen(pAppData->plineLast->pszText );
               WinSendMsg( hwnd, WM_VSCROLL, (MPARAM)FID_VERTSCROLL,
                 MPFROM2SHORT( max(0L, pAppData->sCurLine -
                                       pAppData->sLinesPerPage + 1),
                               SB_SLIDERTRACK ));
               WinSendMsg( hwnd, WM_HSCROLL, (MPARAM)FID_HORZSCROLL,
                 MPFROM2SHORT( max(0L,
                               pAppData->lCurCol - pAppData->lColsPerPage),
                               SB_SLIDERTRACK ));
               break;

// --------------------------------------------------------------------------
// CTRL+BACKSPACE: Remove the current line from the file.
// --------------------------------------------------------------------------
            case VK_BACKSPACE:
               DeleteLine( hwnd, pAppData );
               break;

// --------------------------------------------------------------------------
// CTRL+other:     Not processed.
// --------------------------------------------------------------------------
            default:
               mrReturn = (MRESULT)FALSE;
              break;
            } /* endswitch */

// --------------------------------------------------------------------------
// Process VIRTUALKEY keystrokes (make sure neither ALT or SHIFT or CTRL
// are pressed).
// --------------------------------------------------------------------------
         } else if (fsflags & KC_VIRTUALKEY &&
                    !(fsflags & (KC_ALT | KC_CTRL | KC_SHIFT ))) {
            switch (CHAR3FROMMP(mp2)) {

// --------------------------------------------------------------------------
// SPACE:          the spacebar is a virtual key - just insert the space
//                 into the text.
// --------------------------------------------------------------------------
            case VK_SPACE:
               InsertCharacter( hwnd, ' ' );
               break;

// --------------------------------------------------------------------------
// BACKSPACE:      standard behavior is to delete the character to the
//                 left of the input cursor.
// --------------------------------------------------------------------------
            case VK_BACKSPACE:   /* BACKSPACE = delete left */
               DeleteCharacter( hwnd, DELETE_BACKWARD );
               break;

// --------------------------------------------------------------------------
// DELETE:         standard behavior is to delete the character under the
//                 input cursor in overstrike mode or the character to the
//                 to the right of the cursor in insert mode.
//                 NOTE: Delete is a numeric keypad key so we first have
//                       to check for the KC_CHAR flag which means that the
//                       keypad was hit in NUMLOCK mode.
// --------------------------------------------------------------------------
            case VK_DELETE:
               if (fsflags & KC_CHAR) {
                  InsertCharacter( hwnd, CHAR1FROMMP(mp2));
               } else {
                  DeleteCharacter( hwnd, DELETE_FORWARD );
               } /* endif */
               break;

// --------------------------------------------------------------------------
// INSERT:         standard behavior is to toggle between insert and
//                 overstrike mode.
//                 NOTE: Insert is a numeric keypad key so we first have
//                       to check for the KC_CHAR flag which means that the
//                       keypad was hit in NUMLOCK mode.
// --------------------------------------------------------------------------
            case VK_INSERT:
               if (fsflags & KC_CHAR) {
                  InsertCharacter( hwnd, CHAR1FROMMP(mp2));
               } else {
                  pAppData->fFlags ^= FLAG_REPLACE;
                  WinDestroyCursor( hwnd );
                  CalcCurrentPos( hwnd, pAppData, &lCurX, &lCurY );
                  if( pAppData->fFlags & FLAG_REPLACE ) {
                     lCurcX = pAppData->lWidth;
                  } else {
                     lCurcX = 0;
                  }
                  WinCreateCursor( hwnd, lCurX, lCurY, lCurcX,
                      pAppData->lHeight, CURSOR_SOLID | CURSOR_FLASH, NULL );
                  WinShowCursor( hwnd, TRUE );
               } /* endif */
               break;

// --------------------------------------------------------------------------
// PAGE UP:        standard behavior is to move up one page in the data.
//                 Calculate a new current line and then send a scroll
//                 message to adjust the displayed data.
//                 NOTE: Page Up is a numeric keypad key so we first have
//                       to check for the KC_CHAR flag which means that the
//                       keypad was hit in NUMLOCK mode.
// --------------------------------------------------------------------------
            case VK_PAGEUP:
               if (fsflags & KC_CHAR) {
                  InsertCharacter( hwnd, CHAR1FROMMP(mp2) );
               } else {
                  pAppData->sCurLine -= pAppData->sLinesPerPage;
                  pAppData->sCurLine = max(0, pAppData->sCurLine);
                  WinSendMsg( hwnd, WM_VSCROLL, (MPARAM)FID_VERTSCROLL,
                              MPFROM2SHORT( 0, SB_PAGEUP ));
               } /* endif */
               break;

// --------------------------------------------------------------------------
// PAGE DOWN:      standard behavior is to move up one page in the data.
//                 Calculate a new current line and then send a scroll
//                 message to adjust the displayed data.
//                 NOTE: Page Down is a numeric keypad key so we first have
//                       to check for the KC_CHAR flag which means that the
//                       keypad was hit in NUMLOCK mode.
// --------------------------------------------------------------------------
            case VK_PAGEDOWN:
               if (fsflags & KC_CHAR) {
                  InsertCharacter( hwnd, CHAR1FROMMP(mp2) );
               } else {
                  pAppData->sCurLine += pAppData->sLinesPerPage;
                  pAppData->sCurLine = min( pAppData->sCurLine,
                                            pAppData->sNumLines - 1);
                  WinSendMsg( hwnd, WM_VSCROLL, (MPARAM)FID_VERTSCROLL,
                              MPFROM2SHORT( 0, SB_PAGEDOWN ));
               } /* endif */
               break;

// --------------------------------------------------------------------------
// HOME:           standard behavior is to move the cursor to the beginning
//                 of the current line.
//                 NOTE: Home is a numeric keypad key so we first have
//                       to check for the KC_CHAR flag which means that the
//                       keypad was hit in NUMLOCK mode.
// --------------------------------------------------------------------------
            case VK_HOME:
               if (fsflags & KC_CHAR) {
                  InsertCharacter( hwnd, CHAR1FROMMP(mp2) );
               } else {
                  pAppData->lCurCol = 0;
                  WinSendMsg( hwnd, WM_HSCROLL, (MPARAM)FID_HORZSCROLL,
                              MPFROM2SHORT( 0, SB_SLIDERTRACK ));
                  UpdateCursor( hwnd, pAppData );
               } /* endif */
               break;

// --------------------------------------------------------------------------
// END:            standard behavior is to move the input cursor to the
//                 end of the current line.  In order to provide maximum
//                 display of characters in the line, the cursor is place
//                 as near the right edge of the window as possible.
//                 NOTE: End is a numeric keypad key so we first have
//                       to check for the KC_CHAR flag which means that the
//                       keypad was hit in NUMLOCK mode.
// --------------------------------------------------------------------------
            case VK_END:
               if (fsflags & KC_CHAR) {
                  InsertCharacter( hwnd, CHAR1FROMMP(mp2) );
               } else {
                  pAppData->lCurCol = strlen(pAppData->pLines
                                             [pAppData->sCurLine]->pszText);
                  WinSendMsg( hwnd, WM_HSCROLL, (MPARAM)FID_HORZSCROLL,
                              MPFROM2SHORT(
                              max(0,
                              pAppData->lCurCol - pAppData->lColsPerPage + 1),
                              SB_SLIDERTRACK ));
                  UpdateCursor( hwnd, pAppData );
               } /* endif */
               break;

// --------------------------------------------------------------------------
// Up Arrow:       standard behavior is to move the input cursor up one
//                 line, scrolling if necessary to keep the cursor line in
//                 the display area.
//                 NOTE: Up arrow is a numeric keypad key so we first have
//                       to check for the KC_CHAR flag which means that the
//                       keypad was hit in NUMLOCK mode.
// --------------------------------------------------------------------------
            case VK_UP:
               if (fsflags & KC_CHAR) {
                  InsertCharacter( hwnd, CHAR1FROMMP(mp2) );
               } else {
                  if( pAppData->sCurLine != 0 ) {
                     pAppData->sCurLine--;
                     if( pAppData->sCurLine < pAppData->sCurTop ) {
                        WinSendMsg( hwnd, WM_VSCROLL, (MPARAM)FID_VERTSCROLL,
                                    MPFROM2SHORT( 0, SB_LINEUP ));
                     } /* endif */
                     UpdateCursor( hwnd, pAppData );
                  } /* endif */
               } /* endif */
               break;

// --------------------------------------------------------------------------
// Down Arrow:     standard behavior is to move the cursor down by one
//                 line, scrolling if necessary to keep the cursor line in
//                 the display area.
//                 NOTE: Down Arrow is a numeric keypad key so we first have
//                       to check for the KC_CHAR flag which means that the
//                       keypad was hit in NUMLOCK mode.
// --------------------------------------------------------------------------
            case VK_DOWN:
               if (fsflags & KC_CHAR) {
                  InsertCharacter( hwnd, CHAR1FROMMP(mp2) );
               } else {
                  if( pAppData->sCurLine < pAppData->sNumLines - 1 ) {
                     pAppData->sCurLine++;
                     if( pAppData->sCurLine >=
                         pAppData->sCurTop + pAppData->sLinesPerPage ) {
                        WinSendMsg( hwnd, WM_VSCROLL, (MPARAM)FID_VERTSCROLL,
                                    MPFROM2SHORT( 0, SB_LINEDOWN ));
                     } /* endif */
                     UpdateCursor( hwnd, pAppData );
                  } /* endif */
               } /* endif */
               break;

// --------------------------------------------------------------------------
// Left Arrow:     standard behavior is to move the cursor left by one
//                 column, scrolling if necessary to keep the cursor column
//                 in the display.
//                 NOTE: Left Arrow is a numeric keypad key so we first have
//                       to check for the KC_CHAR flag which means that the
//                       keypad was hit in NUMLOCK mode.
// --------------------------------------------------------------------------
            case VK_LEFT:
               if (fsflags & KC_CHAR) {
                  InsertCharacter( hwnd, CHAR1FROMMP(mp2) );
               } else {
                  if( pAppData->lCurCol != 0 ) {
                     pAppData->lCurCol--;
                     if( pAppData->lCurCol < pAppData->lCurLeft ) {
                        WinSendMsg( hwnd, WM_HSCROLL, (MPARAM)FID_HORZSCROLL,
                                    MPFROM2SHORT( 0, SB_LINELEFT ));
                     } /* endif */
                     UpdateCursor( hwnd, pAppData );
                  } /* endif */
               } /* endif */
               break;

// --------------------------------------------------------------------------
// Right Arrow:    standard behavior is to move the cursor to the right by
//                 one column, scrolling if necessary to keep the cursor
//                 column in the display.
//                 NOTE: Right Arrow is a numeric keypad key so we first have
//                       to check for the KC_CHAR flag which means that the
//                       keypad was hit in NUMLOCK mode.
// --------------------------------------------------------------------------
            case VK_RIGHT:
               if (fsflags & KC_CHAR) {
                  InsertCharacter( hwnd, CHAR1FROMMP(mp2) );
               } else {
                  if( pAppData->lCurCol != pAppData->lMaxWidth ) {
                     pAppData->lCurCol++;
                     if( pAppData->lCurCol >
                         pAppData->lCurLeft + pAppData->lColsPerPage ) {
                        WinSendMsg( hwnd, WM_HSCROLL, (MPARAM)FID_HORZSCROLL,
                                    MPFROM2SHORT( 0, SB_LINERIGHT ));
                     } /* endif */
                     UpdateCursor( hwnd, pAppData );
                  } /* endif */
               } /* endif */
               break;

// --------------------------------------------------------------------------
// ENTER:          Insert a new, blank line in the file.
// --------------------------------------------------------------------------
            case VK_ENTER:
               InsertNewLine( hwnd, pAppData );
               break;

// --------------------------------------------------------------------------
// NEWLINE:        Insert a new, blank line in the file.
// --------------------------------------------------------------------------
            case VK_NEWLINE:
               InsertNewLine( hwnd, pAppData );
               break;

// --------------------------------------------------------------------------
// other:          The remaining virtual keys are not processed.  Set the
//                 result code to indicate that the key was not processed.
// --------------------------------------------------------------------------
            default:
               mrReturn = (MRESULT)FALSE;
               break;
            } /* endswitch */

// --------------------------------------------------------------------------
// The remaining keystrokes that contain combinations of the CTRL, ALT,
// or VIRTUAL KEY flags are not processed by this application.  The else
// clause below will cause a FALSE result, indicating that the keys
// have not been processed.
//
// Any other keystrokes at this point are either shifted or unshifted
// displayable characters and are inserted into the data at the current
// cursor location.
// --------------------------------------------------------------------------
         } else if (fsflags & KC_CHAR &&
                !(fsflags & (KC_VIRTUALKEY | KC_ALT | KC_CTRL ))) {
            InsertCharacter( hwnd, CHAR1FROMMP(mp2) );

         } else {
            mrReturn = (MRESULT)FALSE;

         } /* endif */
      } /* endif */
   } /* endif */
   return mrReturn;
}

// *******************************************************************************
// FUNCTION:     wmClose
//
// FUNCTION USE: Process WM_CLOSE messages for the application client window
//
// DESCRIPTION:  Provide the user with an opportunity to save any changes
//               to the file, then post the WM_QUIT message to terminate
//               the application.
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
static MRESULT wmClose( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PAPP_DATA         pAppData;
   ULONG             ulRC;

// --------------------------------------------------------------------------
// Obtain the editor control structure from the window instance data.
// If pointer is NULL, there is no need to create the input cursor, just
// return.
// --------------------------------------------------------------------------
   pAppData = (PAPP_DATA)WinQueryWindowPtr( hwnd, APP_DATA_POINTER );
   if (pAppData != (PAPP_DATA)NULL) {

      if (pAppData->fFlags & FLAG_CHANGED) {
// --------------------------------------------------------------------------
// the file has changed - see what the user wants to do
// FLAG_CLOSING disables the CANCEL operation, since this could be
// occuring at shutdown.
// --------------------------------------------------------------------------
         pAppData->fFlags |= FLAG_CLOSING;
         ulRC = WinDlgBox( HWND_DESKTOP, hwnd, NotSavedDlgProc, NULLHANDLE,
                           DID_NOTSAVED, pAppData );
// --------------------------------------------------------------------------
// OK equates to the save as button - so call save as
// --------------------------------------------------------------------------
         if (ulRC == DID_OK ) {
            MidFileSaveas( hwnd, mp1, mp2 );
         } /* endif */
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// Tell the application to shut down
// --------------------------------------------------------------------------
   WinPostMsg( hwnd, WM_QUIT, MPFROMLONG(0L), MPFROMLONG(0L));

// --------------------------------------------------------------------------
// Return the reserved value expected for the message
// --------------------------------------------------------------------------
   return (MRESULT)0l;
}

// *******************************************************************************
// FUNCTION:     wmCommand
//
// FUNCTION USE: Process WM_COMMAND messages for the application client window
//
// DESCRIPTION:  This routine examines the command code passed in the low
//               order 16 bits of parameter mp1.  If the command code is supported
//               by the application, the appropriate command processor is called;
//               otherwise, the result code is set to zero (the expected reserved
//               return value).
//
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
static MRESULT wmCommand( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   switch (SHORT1FROMMP(mp1)) 
    {
     case MID_FILENEW:          
          return MidFileNew(hwnd, mp1, mp2); 

     case MID_FILEOPEN:         
          return MidFileOpen(hwnd, mp1, mp2); 

     case MID_FILESAVE:         
          return MidFileSave(hwnd, mp1, mp2); 

     case MID_FILESAVEAS:       
          return MidFileSaveas(hwnd, mp1, mp2); 

     case MID_FILEEXIT:         
          return MidFileExit(hwnd, mp1, mp2);        

     case MID_HELPGEN:          
          return MidHelpGen(hwnd, mp1, mp2);     

     case MID_HELPUSING:        
          return MidHelpUsing(hwnd, mp1, mp2);    

     case MID_HELPKEYS:         
          return MidHelpKeys(hwnd, mp1, mp2); 

     case MID_HELPPROD:         
          return MidHelpProd(hwnd, mp1, mp2);   

     default:                
          return MRFROMLONG(FALSE);  
    } // endswitch 
}

// *******************************************************************************
// FUNCTION:     wmCreate
//
// FUNCTION USE: Process WM_CREATE messages for the application client window
//
// DESCRIPTION:  This function handles the initialization of the window.
//
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  TRUE, error occured -- abort window creation.
//                        FALSE, continue window creation.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PAPP_DATA   pAppData = (PAPP_DATA)NULL;
   HPS         hps;
   RECTL       rectl;

// --------------------------------------------------------------------------
// Allocate memory for the editor control structure.  If this memory is
// successfully allocated, continue processing; otherwise, return TRUE
// to abort the window creation process and ultimately the application.
// --------------------------------------------------------------------------
   pAppData = (PAPP_DATA)malloc( sizeof( APP_DATA ));
   if (pAppData != (PAPP_DATA)NULL) {

// --------------------------------------------------------------------------
// Initialize the data structure with zero's. Then save a pointer to
// the structure in the window instance data.
// --------------------------------------------------------------------------
      memset( (PVOID)pAppData, 0, sizeof( APP_DATA ));
      pAppData->cb = sizeof( PAPP_DATA );
      WinSetWindowPtr( hwnd, APP_DATA_POINTER, pAppData );

// --------------------------------------------------------------------------
// Initialize the characters height and width elements of the editor
// control structure.  This application only uses monospace fonts so
// set the font with a presentation parameter and draw a space into a
// large rectangle.  The resulting rectangle then represents the
// width and height of the each character of the font.
// --------------------------------------------------------------------------
      WinSetPresParam( hwnd, PP_FONTNAMESIZE, strlen(DEFAULT_FONT) + 1,
                       DEFAULT_FONT );
      hps = WinGetPS( hwnd );
      rectl.xLeft = 0;
      rectl.yTop = 1000;
      rectl.xRight = 1000;
      rectl.yBottom = 0;
      WinDrawText( hps, -1, " ", &rectl, 0, 0,
             DT_TOP | DT_LEFT | DT_QUERYEXTENT | DT_EXTERNALLEADING );
      pAppData->lHeight = rectl.yTop - rectl.yBottom;
      pAppData->lWidth = rectl.xRight - rectl.xLeft;
      WinReleasePS( hps );

// --------------------------------------------------------------------------
// Start off with a new file
// --------------------------------------------------------------------------
      MidFileNew( hwnd, MPFROMLONG(0L), MPFROMLONG(0L));

// --------------------------------------------------------------------------
// Done, let the creation process continue by returning FALSE.
// --------------------------------------------------------------------------
      return MRFROMLONG(FALSE);
   } else {

// --------------------------------------------------------------------------
// Can't allocate the control structure - bail out now
// --------------------------------------------------------------------------
   DisplayMessages(NULLHANDLE, "Error Allocating Memory",
                   MSG_ERROR);
      return (MRESULT)TRUE;
   } /* endif */
}

// *******************************************************************************
// FUNCTION:     wmDestroy
//
// FUNCTION USE: Process WM_DESTROY messages for the application client window
//
// DESCRIPTION:  This function releases allocated resource and performs.
//               general cleanup when the window is destroyed.
//
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
static MRESULT wmDestroy( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PAPP_DATA   pAppData = (PAPP_DATA)NULL;

// --------------------------------------------------------------------------
// Retrieve the pointer to the editor control structure from the
// window instance data.  If the pointer is NULL, no further action
// is necessary.
// --------------------------------------------------------------------------
   pAppData = WinQueryWindowPtr( hwnd, APP_DATA_POINTER );
   if (pAppData != (PAPP_DATA)NULL) {

// --------------------------------------------------------------------------
// First scan the control structure's linked list of lines.
// If lines are allocated - go and destroy the list.
// --------------------------------------------------------------------------
      if (pAppData->plineFirst != (PLINE)NULL) {
         DestroyList( pAppData );
      } /* endif */

// --------------------------------------------------------------------------
// If a file is currently open - go and close the file
// --------------------------------------------------------------------------
      if (pAppData->pfile != (PAPPFILE)NULL) {
         CloseFile( pAppData->pfile );
         pAppData->pfile = (PAPPFILE)NULL;
      } /* endif */

// --------------------------------------------------------------------------
// Free the editor control structure
// --------------------------------------------------------------------------
      free(pAppData);
   } /* endif */

// --------------------------------------------------------------------------
// Return with the proper reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wmPaint
//
// FUNCTION USE: Process WM_PAINT messages for the application client window
//
// DESCRIPTION:  This function draws the editor text into the window whenever
//               all or part of the window requires repainting.
//
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
static MRESULT wmPaint( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HPS         hps;
   RECTL       rectl;
   RECTL       rectlHwnd;
   PLINE       plineDraw = (PLINE)NULL;
   PAPP_DATA   pAppData = (PAPP_DATA)NULL;
   LONG        iLineFirst;
   LONG        lColFirst;
   LONG        lNumCols;

// --------------------------------------------------------------------------
// Get the presentation space handle for the area to be painted.
// If no presentation space obtained then bail out - no drawing
// can be performed.
// --------------------------------------------------------------------------
   hps = WinBeginPaint( hwnd, NULLHANDLE, &rectl );
   if (hps != (HPS)NULLHANDLE) {

// --------------------------------------------------------------------------
// Fill in the background of the client area */
// --------------------------------------------------------------------------
      WinFillRect( hps, &rectl, CLR_WHITE );

// --------------------------------------------------------------------------
// Get the editor control data pointer from the instance data.  If
// the pointer is NULL, then no text drawing is necessary.
// --------------------------------------------------------------------------
      pAppData = (PAPP_DATA)WinQueryWindowPtr( hwnd, APP_DATA_POINTER );
      if (pAppData != (PAPP_DATA)NULL) {

// --------------------------------------------------------------------------
// Fill in a rectangle which represents the size of the entire window.
// The delta's between the edges of this rectangle and the rectangle
// enclosing the area to be painted determine which rows and columns
// of the text require painting.
// --------------------------------------------------------------------------
         WinQueryWindowRect( hwnd, &rectlHwnd );

// --------------------------------------------------------------------------
// Calculate the index of the first line which requires drawing, the first
// line which falls partially within the invalid area.
//
// Begin by obtaining the delta between the top of the window and the top
// of the invalid area.  Then divide by the height of each line to determine
// the number of lines which do not require any drawing.  Add this number
// to the index of the line displayed at the top of the window to obtain
// the index of the first line to be drawn.
//
// If the first line to draw is not past the end of the file, set a
// pointer to the line structure for the line.
//
// Finally, using the window rectangle as the drawing rectangle, adjust the
// the top coordinate down to the proper position for the top of the first
// line that requires drawing.  This may not be the same as the top of the
// invalid area if a partial line needs updating.
// --------------------------------------------------------------------------
         iLineFirst = rectlHwnd.yTop - rectl.yTop;
         iLineFirst /= pAppData->lHeight;
         iLineFirst += (LONG)(pAppData->sCurTop);
         if (iLineFirst < pAppData->sNumLines) {
            if (pAppData->pLines != (PLINE *)NULL) {
               plineDraw = pAppData->pLines[iLineFirst];
               rectlHwnd.yTop -= (iLineFirst - pAppData->sCurTop ) *
                                 pAppData->lHeight;

            } /* endif */
         } /* endif */

// --------------------------------------------------------------------------
// Calculate the number of characters to draw and the first column which
// falls partially within the invalid area.
//
// Begin by obtaining the delta between the left edge of the window and
// the left edge of the invalid area.  Then divide by the width of a
// character and add the result to the column number currently displayed
// at the left edge of the window.  The result is the number of the first
// column to be painted.  If this column is beyond the longest line, there
// is nothing to paint so the LINE pointer is cleared to prevent drawing.
// The left edge of the drawing rectangle is set to the proper coordinates
// for the first column to be drawn.
//
// The number of columns to draw is then computed by taking the delta
// between the right edge of the invalid area and the left edge of the
// drawing rectangle and dividing by the width of a character.  The
// result is incremented to insure that all partial characters are drawn.
// --------------------------------------------------------------------------
         lColFirst = rectl.xLeft - rectlHwnd.xLeft;
         lColFirst /= pAppData->lWidth;
         lColFirst += pAppData->lCurLeft;
         if (lColFirst > pAppData->lMaxWidth) {
            plineDraw = (PLINE)NULL;    /* nothing to draw */
         } else {
            rectlHwnd.xLeft += (lColFirst - pAppData->lCurLeft ) *
                     pAppData->lWidth;
            /* compute the last column to paint */
            lNumCols = (rectl.xRight - rectlHwnd.xLeft) / pAppData->lWidth;
            lNumCols++;
         } /* endif */

// --------------------------------------------------------------------------
// Now begin a loop which will draw each line from the top of the invalid
// area to the bottom of the invalid area, but only draw complete lines.
// --------------------------------------------------------------------------
         while (plineDraw != (PLINE)NULL &&
                rectlHwnd.yTop > rectl.yBottom &&
                rectlHwnd.yTop - pAppData->lHeight >= 0) {

// --------------------------------------------------------------------------
// Drawing only occurs if the text of the line falls within the invalid area.
// The number of characters to be drawn is determined based on whether the
// text spans the invalid area or ends within the area.  In the former case,
// the number of characters is the quantity which fits in the area; in the
// latter case the number of characters is the length of the remainder of the
// text from the first character to be drawn.
// --------------------------------------------------------------------------
            if (strlen(plineDraw->pszText) > lColFirst) {
               WinDrawText( hps,
                         min(lNumCols,strlen(&plineDraw->pszText[lColFirst])),
                         &plineDraw->pszText[lColFirst],
                         &rectlHwnd, 0, 0, DT_TOP | DT_LEFT | DT_TEXTATTRS );
            } /* endif */

            /* bump down to top position of next line */
// --------------------------------------------------------------------------
// Set up to draw the next line.  Move the top of the rectangle down
// by the height of a line and bump the current line pointer to the
// next line.
// --------------------------------------------------------------------------
            rectlHwnd.yTop -= pAppData->lHeight;
            plineDraw = plineDraw->next;
         } /* endwhile */
      } /* endif */

// --------------------------------------------------------------------------
// Release the presentation space and terminate the paint operation.
// --------------------------------------------------------------------------
      WinEndPaint( hps );
   } /* endif */

// --------------------------------------------------------------------------
// Return the reserved value expected for the message
// --------------------------------------------------------------------------
   return (MRESULT)0l;
}

// *******************************************************************************
// FUNCTION:     wmSetFocus
//
// FUNCTION USE: Process WM_SETFOCUS messages for the application client window.
//
// DESCRIPTION:  This function handles tasks that are necessary when the
//               window gains or loses the focus.  In this application, the
//               the keyboard cursor is created and displayed when the window
//               receives the focus and is destroyed when the window loses
//               the focus
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
static MRESULT wmSetFocus( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PAPP_DATA   pAppData = (PAPP_DATA)NULL;
   USHORT      usFocus = SHORT1FROMMP( mp2 );
   LONG        lx = 0;
   LONG        ly = 0;
   LONG        lcx = 0;

// --------------------------------------------------------------------------
// Obtain the editor control structure from the window instance data.
// If pointer is NULL, there is no need to create the input cursor, just
// return.
// --------------------------------------------------------------------------
   pAppData = (PAPP_DATA)WinQueryWindowPtr( hwnd, APP_DATA_POINTER );
   if (pAppData != (PAPP_DATA)NULL) {

// --------------------------------------------------------------------------
// If the window is gaining the focus - create and display the input
// cursor at the current input location.
// --------------------------------------------------------------------------
      if (usFocus) {
         CalcCurrentPos( hwnd, pAppData, &lx, &ly );
         if( pAppData->fFlags & FLAG_REPLACE ) {
            lcx = pAppData->lWidth;
         } else {
            lcx = 0;
         }
         WinCreateCursor( hwnd, lx, ly, lcx, pAppData->lHeight,
                          CURSOR_SOLID | CURSOR_FLASH, NULL );
         WinShowCursor( hwnd, TRUE );

// --------------------------------------------------------------------------
// If the window is losing the focus - destroy the cursor so the
// next window can create one.
// --------------------------------------------------------------------------
      } else {
         WinDestroyCursor( hwnd );

      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// Return the reserved value expected for the message
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wmSize
//
// FUNCTION USE: Process WM_SIZE messages for the application client window.
//
// DESCRIPTION:  This function performs any tasks necessary when the size
//               of the window changes.  Here, the function recalculates
//               the number of lines and columns which fit on the page.
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
static MRESULT wmSize( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   SHORT       scx = SHORT1FROMMP(mp2);
   SHORT       scy = SHORT2FROMMP(mp2);
   HWND        hwndFrame;
   HWND        hwndScroll;
   PAPP_DATA   pAppData;
   RECTL       rectl;
   LONG        lOldLastLine;

// --------------------------------------------------------------------------
// Obtain the editor control structure from the window instance data.
// If pointer is NULL, there is no need to create the input cursor, just
// return.
// --------------------------------------------------------------------------
   pAppData = WinQueryWindowPtr( hwnd, APP_DATA_POINTER );
   if (pAppData != (PAPP_DATA)NULL) {

// --------------------------------------------------------------------------
// Get the new window size and compute the vertical location where the
// last line was drawn at the previous size.
// --------------------------------------------------------------------------
      WinQueryWindowRect( hwnd, &rectl );
      lOldLastLine = rectl.yTop - ( pAppData->sLinesPerPage * pAppData->lHeight );

// --------------------------------------------------------------------------
// Compute the new lines per page as new window height divided by the height
// of a line.  Compute the new columns per page as the new page width divided
// by the width of a character.
// --------------------------------------------------------------------------
      pAppData->sLinesPerPage = scy / pAppData->lHeight;
      pAppData->lColsPerPage = scx / pAppData->lWidth;

// --------------------------------------------------------------------------
// If the current line has moved off the top of the page, then recompute top
// line such that the current line falls in the middle of the page and
// force repainting of the entire window.
// --------------------------------------------------------------------------
      if (pAppData->sCurLine > pAppData->sCurTop + pAppData->sLinesPerPage - 1) {
         pAppData->sCurTop = pAppData->sCurLine - (pAppData->sLinesPerPage / 2L );
         WinInvalidateRect( hwnd, NULL, TRUE );
      } /* endif */

// --------------------------------------------------------------------------
// Reset both scroll bars so that the slider represents the proper proportion
// between the visible area of the text and the entire size of the text.
// --------------------------------------------------------------------------
      hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
      hwndScroll = WinWindowFromID( hwndFrame, FID_HORZSCROLL );
      WinSendMsg( hwndScroll, SBM_SETTHUMBSIZE,
                  MPFROM2SHORT( pAppData->lColsPerPage, pAppData->lMaxWidth ),
                  0L );
      hwndScroll = WinWindowFromID( hwndFrame, FID_VERTSCROLL );
      WinSendMsg( hwndScroll, SBM_SETTHUMBSIZE,
                  MPFROM2SHORT( pAppData->sLinesPerPage, pAppData->sNumLines ),
                  0L );

// --------------------------------------------------------------------------
// compute the top of the first line which will not completely fit in the
// new window size.  If the window is now larger than before, this will be
// below the original last line so bump the top of the rectangle back up to
// insure that both the last line and any following lines are drawn.
// --------------------------------------------------------------------------
      rectl.yTop -= pAppData->sLinesPerPage * pAppData->lHeight;
      if (lOldLastLine > rectl.yTop) {
         rectl.yTop = lOldLastLine;
      } /* endif */

// --------------------------------------------------------------------------
// Make sure that any new lines are repainted
// --------------------------------------------------------------------------
      WinInvalidateRect( hwnd, &rectl, TRUE );
   } /* endif */


// --------------------------------------------------------------------------
// Return the reserved value expected for the message
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wmVScroll
//
// FUNCTION USE: Process WM_VSCROLL messages for the application client window.
//
// DESCRIPTION:  This function adjusts the scroll bar position, the window
//               contents, and the input cursor position in response to user
//               requests to vertically scroll the contents of the window.
//
//               NOTE: The scrolling nomenclature is based on the direction
//                     which the window would move if the data were perceived
//                     to be stationary; thus a scroll up moves the data down
//                     in the window.
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
static MRESULT wmVScroll( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PAPP_DATA   pAppData;
   HWND        hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
   HWND        hwndScroll = WinWindowFromID( hwndFrame, SHORT1FROMMP( mp1 ));
   RECTL       rectl;
   LONG        lDy;
   SHORT       sDLines = 0;
   SHORT       sCurPos;

// --------------------------------------------------------------------------
// Obtain the editor control structure from the window instance data.
// If pointer is NULL, there is no need to create the input cursor, just
// return.
// --------------------------------------------------------------------------
   pAppData = WinQueryWindowPtr( hwnd, APP_DATA_POINTER );
   if (pAppData != (PAPP_DATA)NULL) {

// --------------------------------------------------------------------------
// Determine the number of lines to be scrolled based on the command.
// --------------------------------------------------------------------------
      switch (SHORT2FROMMP(mp2)) {
      case SB_LINEUP:               // move up one line
         sDLines = -1L;
         break;
      case SB_LINEDOWN:             // move down one line
         sDLines = 1L;
         break;
      case SB_PAGEUP:               // move up one page
         sDLines = (-1L) * pAppData->sLinesPerPage;
         break;
      case SB_PAGEDOWN:             // move down one page
         sDLines = pAppData->sLinesPerPage;
         break;
      case SB_SLIDERTRACK:          // move to specific location
         // get the original location - pos has not yet officially changed.
         sDLines = (SHORT)WinSendMsg( hwndScroll, SBM_QUERYPOS,
                                      MPFROMLONG(0L), MPFROMLONG(0L) );
         // delta is new position minus current position
         sDLines = SHORT1FROMMP(mp2) - sDLines;
         break;
      } /* endswitch */

// --------------------------------------------------------------------------
// If movement has actually occured, adjust the current position
// --------------------------------------------------------------------------
      if (sDLines != 0) {

// --------------------------------------------------------------------------
// Get the original position of the slider and add the delta.  If this is
// above the first line of data, set to the first line of data.  If below
// the last line of data, set to the last line of data, then update the
// position of the slider.
// --------------------------------------------------------------------------
         sCurPos = SHORT1FROMMR(WinSendMsg( hwndScroll, SBM_QUERYPOS,
                                            MPFROMLONG(0L), MPFROMLONG(0L) ));
         sCurPos += sDLines;
         if( sCurPos < 0 ) sCurPos = 0;
         if( (LONG)sCurPos >= pAppData->sNumLines )
            sCurPos = pAppData->sNumLines - 1;
         WinSendMsg( hwndScroll, SBM_SETPOS, MPFROMSHORT( sCurPos ), 0l );

// --------------------------------------------------------------------------
// Use the resulting current position to compute the actual delta, then
// set the new top of the window to the new position.
// --------------------------------------------------------------------------
         sDLines = sCurPos - pAppData->sCurTop;
         pAppData->sCurTop = sCurPos;

// --------------------------------------------------------------------------
// Now determine how far to scroll the window contents.  If the whole window
// is affected, don't scroll, just repaint.
//
// Begin by computing the vertical distance as the number of lines multiplied
// by the height of each line.  If the result is greater than the number of
// lines on the page multiplied by the height of a line, the whole page is
// affected, so repaint the whole page.
//
// If the whole window is not affected, get the window rectangle and compute
// the portion of the window which can display complete lines.  Then scroll
// this portion of the window by the computed vertical scrolling distance.
// --------------------------------------------------------------------------
         lDy = (LONG)sDLines * pAppData->lHeight;
         if (lDy >= pAppData->sLinesPerPage * pAppData->lHeight) {
            WinInvalidateRect( hwnd, NULL, TRUE );
         } else {
            WinQueryWindowRect( hwnd, &rectl );
            rectl.yBottom = rectl.yTop -
                            ( pAppData->sLinesPerPage * pAppData->lHeight );
            WinScrollWindow( hwnd, 0L, lDy, &rectl, &rectl,
                             NULLHANDLE, NULL, SW_INVALIDATERGN );
         } /* endif */

// --------------------------------------------------------------------------
// Now adjust the input cursor.  Since the location of the current line has
// moved, the cursor must be moved.  However, if the current line has moved
// off the page, the current line must be adjusted so that the cursor will
// be displayed.  If the current line is above the top of the window, set
// the new current line to the line at the top of the window; if the current
// line is below the bottom of the window, set the new current line to
// the last line displayed in the window.
//
// After the current line has been adjusted, call the routine to redisplay
// the cursor in the correct location.
// --------------------------------------------------------------------------
         if (pAppData->sCurLine < pAppData->sCurTop) {
            pAppData->sCurLine = pAppData->sCurTop;
         } else if (pAppData->sCurLine >=
                    pAppData->sCurTop + pAppData->sLinesPerPage) {
            pAppData->sCurLine = pAppData->sCurTop + pAppData->sLinesPerPage - 1;
         } /* endif */
         UpdateCursor( hwnd, pAppData );

      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// Return the reserved value expected for the message
// --------------------------------------------------------------------------
   return (MRESULT)0L;
}

// *******************************************************************************
// FUNCTION:     wmHScroll
//
// FUNCTION USE: Process WM_VSCROLL messages for the application client window.
//
// DESCRIPTION:  This function adjusts the scroll bar position, the window
//               contents, and the input cursor position in response to user
//               requests to horizontally scroll the contents of the window.
//
//               NOTE: The scrolling nomenclature is based on the direction
//                     which the window would move if the data were perceived
//                     to be stationary; thus a scroll left moves the data
//                     to the right in the window.
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
static MRESULT wmHScroll( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PAPP_DATA   pAppData;
   HWND        hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
   HWND        hwndScroll = WinWindowFromID( hwndFrame, SHORT1FROMMP( mp1 ));
   LONG        lDCols = 0;
   LONG        lCurCol;
   LONG        lDx;

// --------------------------------------------------------------------------
// Obtain the editor control structure from the window instance data.
// If pointer is NULL, there is no need to create the input cursor, just
// return.
// --------------------------------------------------------------------------
   pAppData = WinQueryWindowPtr( hwnd, APP_DATA_POINTER );
   if (pAppData != (PAPP_DATA)NULL) {

// --------------------------------------------------------------------------
// Determine the number of columns to be scrolled based on the command.
// --------------------------------------------------------------------------
      switch (SHORT2FROMMP(mp2)) {
      case SB_LINELEFT:             // move contents right one column
         lDCols = -1L;
         break;
      case SB_LINERIGHT:            // move contents left one column
         lDCols = 1L;
         break;
      case SB_PAGELEFT:             // move contents one page right
         lDCols = (-1L) * pAppData->lColsPerPage;
         break;
      case SB_PAGERIGHT:            // move contents one page left
         lDCols = pAppData->lColsPerPage;
         break;
      case SB_SLIDERTRACK:          // move to specific location
         // get the original location - pos has not yet officially changed.
         lDCols = (LONG)WinSendMsg( hwndScroll, SBM_QUERYPOS, 0l, 0l );
         // delta is new position minus current position
         lDCols = (LONG)SHORT1FROMMP(mp2) - lDCols;
         break;
      } /* endswitch */

// --------------------------------------------------------------------------
// If movement has actually occured, adjust the current position
// --------------------------------------------------------------------------
      if (lDCols != 0) {

// --------------------------------------------------------------------------
// Get the original position of the slider and add the delta.  If the result
// is less than the first column, set to the first column.  If past the last
// column, set to the last column of data, then update the position of the
// scroll bar's slider.
// --------------------------------------------------------------------------
         lCurCol = (LONG)WinSendMsg( hwndScroll, SBM_QUERYPOS, 0l, 0l );
         lCurCol += lDCols;
         if( lCurCol < 0 ) lCurCol = 0;
         if( lCurCol >= pAppData->lMaxWidth )
            lCurCol = pAppData->lMaxWidth - 1;
         WinSendMsg( hwndScroll, SBM_SETPOS, MPFROMLONG( lCurCol ), 0l );

// --------------------------------------------------------------------------
// Use the resulting current position to compute the actual delta, then
// set the new left column to the new position.
// --------------------------------------------------------------------------
         lDCols = lCurCol - pAppData->lCurLeft;
         pAppData->lCurLeft = lCurCol;

// --------------------------------------------------------------------------
// Now determine how far to scroll the window contents.  If the whole window
// is affected, don't scroll, just repaint.
//
// Begin by computing the horizontal distance as the number of columns times
// the width of a character.  If the result is greater than the number of
// columns on the page multiplied by the width of a character, the whole page
// is affected, so repaint the whole page.  Otherwise, scroll the window by
// the required amount.
// --------------------------------------------------------------------------
         lDx = lDCols * pAppData->lWidth * (-1L);
         if (lDx < pAppData->lColsPerPage * pAppData->lWidth) {
            WinScrollWindow( hwnd, lDx, 0L, NULL, NULL,
                             NULLHANDLE, NULL, SW_INVALIDATERGN );
         } else {
            WinInvalidateRect( hwnd, NULL, TRUE );
         } /* endif */


// --------------------------------------------------------------------------
// Now adjust the input cursor.  Since the location of the current column has
// moved, the cursor must be moved.  However, if the current column has moved
// off the page, the column must be adjusted so that the cursor will be
// displayed.  If the current column is to the left of the window, set
// the new current column to the column at the left of the window; if the
// current column is below the right of the window, set the new current
// column to the last column displayed in the window.
//
// After the current column has been adjusted, call the routine to redisplay
// the cursor in the correct location.
// --------------------------------------------------------------------------
         if( pAppData->lCurCol < lCurCol ) {
            pAppData->lCurCol = lCurCol;
         } else if (pAppData->lCurCol > lCurCol + pAppData->lColsPerPage) {
            pAppData->lCurCol = lCurCol + pAppData->lColsPerPage - 1;
         }
         UpdateCursor( hwnd, pAppData );

      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// Return the reserved value expected for the message
// --------------------------------------------------------------------------
   return (MRESULT)0L;
}


// *******************************************************************************
//
// Application command processing functions
//
// *******************************************************************************

// *******************************************************************************
// FUNCTION:     MidFileNew
//
// FUNCTION USE: Execute the application's FILE NEW command.
//
// DESCRIPTION:  After insuring that any currently opened, and changed, file
//               has been properly closed, initialize a one line file with
//               zero length.
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
static MRESULT MidFileNew( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PAPP_DATA      pAppData = (PAPP_DATA)NULL;
   ULONG          ulRC;

// --------------------------------------------------------------------------
// Obtain the editor control structure from the window instance data.
// If pointer is NULL, there is no need to create the input cursor, just
// return.
// --------------------------------------------------------------------------
   pAppData = (PAPP_DATA)WinQueryWindowPtr( hwnd, APP_DATA_POINTER );
   if (pAppData != (PAPP_DATA)NULL) {

      if (pAppData->fFlags & FLAG_CHANGED) {
// --------------------------------------------------------------------------
// the file has changed - see what the user wants to do
// --------------------------------------------------------------------------

         WinAlarm(HWND_DESKTOP, WA_WARNING);
         ulRC = WinDlgBox( HWND_DESKTOP, hwnd, NotSavedDlgProc, NULLHANDLE,
                           DID_NOTSAVED, pAppData );
         switch (ulRC) {
// --------------------------------------------------------------------------
// OK equates to the save as button - so call save as
// --------------------------------------------------------------------------
         case DID_OK:
            MidFileSaveas( hwnd, mp1, mp2 );
            break;

// --------------------------------------------------------------------------
// DISCARD - toss out any changes in current file
// --------------------------------------------------------------------------
         case DID_DISCARD:
            pAppData->fFlags &= ~FLAG_CHANGED;
            break;

// --------------------------------------------------------------------------
// Cancel - do nothing, since the change flag is set no new file is created
// --------------------------------------------------------------------------
         } /* endswitch */
      } /* endif */

// --------------------------------------------------------------------------
// If no new file was selected, then do nothing here -- the user will
// continue editting the current file, even though it may have been saved.
// Otherwise, reinitialize for the new file.
// --------------------------------------------------------------------------
      if ( !(pAppData->fFlags & FLAG_CHANGED)) {

// --------------------------------------------------------------------------
// First hide the input cursor, it will be created again later when needed.
// --------------------------------------------------------------------------
         WinShowCursor( hwnd, FALSE );

// --------------------------------------------------------------------------
// Clean up the current file - release all memory allocated and then close
// the file itself.
// --------------------------------------------------------------------------
         if (pAppData->plineFirst != (PLINE)NULL) DestroyList( pAppData );
         if (pAppData->pfile != (PAPPFILE)NULL) CloseFile( pAppData->pfile );


// --------------------------------------------------------------------------
// Now change the name to the "no file" name and load memory
// --------------------------------------------------------------------------
         ChangeFileName( hwnd, pAppData, NOFILE_NAME );
         ReadText( pAppData );

// --------------------------------------------------------------------------
// Clean up the display and initialize the cursor position
// --------------------------------------------------------------------------
         WinInvalidateRect( hwnd, NULL, TRUE );
         UpdateCursor( hwnd, pAppData );
         WinShowCursor( hwnd, TRUE );

// --------------------------------------------------------------------------
// Set scroll bar ranges and thumb sizes based on new file
// --------------------------------------------------------------------------
         RecalcScrollRange( hwnd, pAppData );

      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// Return with the proper reserved value
// --------------------------------------------------------------------------
   return (MRESULT)0L;
}

// *******************************************************************************
// FUNCTION:     MidFileOpen
//
// FUNCTION USE: Execute the application's FILE OPEN command.
//
// DESCRIPTION:  After insuring that any currently opened, and changed, file
//               has been properly closed, this function raises the standard
//               file dialog to allow the user to select a new file to open.
//               The function then reinitializes the editor control structure
//               and resets the scroll bars to begin editting the new file.
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
static MRESULT MidFileOpen( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PAPP_DATA      pAppData = (PAPP_DATA)NULL;
   FILEDLG        fd;
   ULONG          ulRC;

// --------------------------------------------------------------------------
// Obtain the editor control structure from the window instance data.
// If pointer is NULL, there is no need to create the input cursor, just
// return.
// --------------------------------------------------------------------------
   pAppData = (PAPP_DATA)WinQueryWindowPtr( hwnd, APP_DATA_POINTER );
   if (pAppData != (PAPP_DATA)NULL) {

      if (pAppData->fFlags & FLAG_CHANGED) {
// --------------------------------------------------------------------------
// the file has changed - see what the user wants to do
// --------------------------------------------------------------------------
         ulRC = WinDlgBox( HWND_DESKTOP, hwnd, NotSavedDlgProc, NULLHANDLE,
                           DID_NOTSAVED, pAppData );
         switch (ulRC) {
// --------------------------------------------------------------------------
// OK equates to the save as button - so call save as
// --------------------------------------------------------------------------
         case DID_OK:
            MidFileSaveas( hwnd, mp1, mp2 );
            break;

// --------------------------------------------------------------------------
// DISCARD - toss out any changes in current file
// --------------------------------------------------------------------------
         case DID_DISCARD:
            pAppData->fFlags &= ~FLAG_CHANGED;
            break;

// --------------------------------------------------------------------------
// Cancel - do nothing, since the change flag is set no new file is created
// --------------------------------------------------------------------------
         } /* endswitch */
      } /* endif */

// --------------------------------------------------------------------------
// Initialize the standard file dialog control structure for a FILE OPEN
// dialog which lists all files in the current directory.  Then raise and
// process the standard file dialog.
// --------------------------------------------------------------------------
      if( !(pAppData->fFlags & FLAG_CHANGED)) {
         memset( &fd, 0, sizeof( FILEDLG ));
         fd.cbSize = sizeof(FILEDLG);
         fd.fl = FDS_CENTER | FDS_OPEN_DIALOG;
         strcpy( fd.szFullFile, "*" );
         WinFileDlg( HWND_DESKTOP, hwnd, &fd );
      } /* endif */

// --------------------------------------------------------------------------
// If no new file was selected, then do nothing here -- the user will
// continue editting the current file, even though it may have been saved.
// Otherwise, reinitialize for the new file.
// --------------------------------------------------------------------------
      if ( !(pAppData->fFlags & FLAG_CHANGED) && fd.lReturn == DID_OK ) {

// --------------------------------------------------------------------------
// First hide the input cursor, it will be created again later when needed.
// --------------------------------------------------------------------------
         WinShowCursor( hwnd, FALSE );

// --------------------------------------------------------------------------
// Clean up the current file - release all memory allocated and then close
// the file itself.
// --------------------------------------------------------------------------
         if (pAppData->plineFirst != (PLINE)NULL) DestroyList( pAppData );
         if (pAppData->pfile != (PAPPFILE)NULL) CloseFile( pAppData->pfile );


// --------------------------------------------------------------------------
// Now open the new file, save its name, and load its contents into memory
// --------------------------------------------------------------------------
         pAppData->pfile = OpenFile( fd.szFullFile );
         if (pAppData->pfile != (PAPPFILE)NULL) {
            ChangeFileName( hwnd, pAppData, fd.szFullFile );
            ReadText( pAppData );
         } /* endif */

// --------------------------------------------------------------------------
// Draw the contents of the new file, reposition and display the cursor
// --------------------------------------------------------------------------
         WinInvalidateRect( hwnd, NULL, TRUE );
         UpdateCursor( hwnd, pAppData );
         WinShowCursor( hwnd, TRUE );

// --------------------------------------------------------------------------
// Set scroll bar ranges and thumb sizes based on new file
// --------------------------------------------------------------------------
         RecalcScrollRange( hwnd, pAppData );

      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// Return with the proper reserved value
// --------------------------------------------------------------------------
   return (MRESULT)0L;
}

// *******************************************************************************
// FUNCTION:     MidFileSave
//
// FUNCTION USE: Execute the application's FILE SAVE command.
//
// DESCRIPTION:  Writes the current in memory file back to the file
//               on disk.
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
static MRESULT MidFileSave( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PAPP_DATA   pAppData = (PAPP_DATA)NULL;

// --------------------------------------------------------------------------
// Obtain the editor control structure from the window instance data.
// If pointer is NULL, there is no need to create the input cursor, just
// return.
// --------------------------------------------------------------------------
   pAppData = (PAPP_DATA)WinQueryWindowPtr( hwnd, APP_DATA_POINTER );
   if (pAppData != (PAPP_DATA)NULL) {

// --------------------------------------------------------------------------
// Write the current contents back out to the file, then set the
// flag to indicate that no changes have occurred.
// --------------------------------------------------------------------------
      WriteText( pAppData );
      pAppData->fFlags &= ~FLAG_CHANGED;
   } /* endif */

// --------------------------------------------------------------------------
// Return with the proper reserved value
// --------------------------------------------------------------------------
   return (MRESULT)0L;
}

// *******************************************************************************
// FUNCTION:     MidFileSaveas
//
// FUNCTION USE: Execute the application's FILE SAVEAS command.
//
// DESCRIPTION:  After obtaining a file name from the user, closes
//               the current file and writes the in memory file to
//               disk under the new filename.
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
static MRESULT MidFileSaveas( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PAPP_DATA   pAppData = (PAPP_DATA)NULL;
   FILEDLG     fd;

// --------------------------------------------------------------------------
// Obtain the editor control structure from the window instance data.
// If pointer is NULL, there is no need to create the input cursor, just
// return.
// --------------------------------------------------------------------------
   pAppData = (PAPP_DATA)WinQueryWindowPtr( hwnd, APP_DATA_POINTER );
   if (pAppData != (PAPP_DATA)NULL) {

// --------------------------------------------------------------------------
// Initialize the standard file dialog control structure for a FILE SAVEAS
// dialog which lists all files in the current directory.  Then raise and
// process the standard file dialog.
// --------------------------------------------------------------------------
      memset( &fd, 0, sizeof( FILEDLG ));
      fd.cbSize = sizeof(FILEDLG);
      fd.fl = FDS_CENTER | FDS_SAVEAS_DIALOG;
      if (pAppData->pszFileName  &&
          strcmp(pAppData->pszFileName, NOFILE_NAME)) {
         strcpy( fd.szFullFile, pAppData->pszFileName );
      } else {
         strcpy( fd.szFullFile, "" );
      } /* endif */
      WinFileDlg( HWND_DESKTOP, hwnd, &fd );

// --------------------------------------------------------------------------
// If no new file was selected, then do nothing here -- the user will
// continue editing the current file.  If a file name was entered, close
// the current file then open the new file and update the file name in
// the editor control structure.
// --------------------------------------------------------------------------
      if (fd.lReturn == DID_OK) {

         CloseFile( pAppData->pfile );
         pAppData->pfile = OpenFile( fd.szFullFile );
         ChangeFileName( hwnd, pAppData, fd.szFullFile );

// --------------------------------------------------------------------------
// Write the current contents back out to the file, then set the
// flag to indicate that no changes have occurred.
// --------------------------------------------------------------------------
         WriteText( pAppData );
         pAppData->fFlags &= ~FLAG_CHANGED;
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// Return with the proper reserved value
// --------------------------------------------------------------------------
   return (MRESULT)0L;
}

// *******************************************************************************
// FUNCTION:     MidFileExit
//
// FUNCTION USE: Close the application.
//
// DESCRIPTION:  Sends a WM_CLOSE message to the window
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
static MRESULT MidFileExit( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND     hwndFrame;

// --------------------------------------------------------------------------
// Post the close message to the frame window
// --------------------------------------------------------------------------
   hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
   WinSendMsg( hwndFrame, WM_CLOSE, MPFROMLONG(0L), MPFROMLONG(0L));

// --------------------------------------------------------------------------
// Return with the proper reserved value
// --------------------------------------------------------------------------
   return (MRESULT)0L;
}

// *******************************************************************************
// FUNCTION:     MidHelpGen
//
// FUNCTION USE: Display the general help panel for the application
//
// DESCRIPTION:  After obtaining a valid help instance handle, send
//               a HM_GENERAL_HELP message to the instance.
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
static MRESULT MidHelpGen( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND        hwndHelp;

// --------------------------------------------------------------------------
// Get the help instance handle and send the message if valid.
// --------------------------------------------------------------------------
   hwndHelp = WinQueryHelpInstance( hwnd );
   if (hwndHelp != NULLHANDLE) {
      WinSendMsg( hwndHelp, HM_GENERAL_HELP, MPFROMLONG(0L), MPFROMLONG(0L));
   } /* endif */

// --------------------------------------------------------------------------
// Return with the proper reserved value
// --------------------------------------------------------------------------
   return (MRESULT)0L;
}

// *******************************************************************************
// FUNCTION:     MidHelpUsing
//
// FUNCTION USE: Display the IPF help for using IPF.
//
// DESCRIPTION:  After obtaining a valid help instance handle, send
//               a HM_DISPLAY_HELP message with mp1 and mp2 set to
//               NULL values.
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
static MRESULT MidHelpUsing( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND        hwndHelp;

// --------------------------------------------------------------------------
// Get the help instance handle and send the message if valid.
// --------------------------------------------------------------------------
   hwndHelp = WinQueryHelpInstance( hwnd );
   if (hwndHelp != NULLHANDLE) {
      WinSendMsg( hwndHelp, HM_DISPLAY_HELP, MPFROMLONG(0L), MPFROMLONG(0L));
   } /* endif */

// --------------------------------------------------------------------------
// Return with the proper reserved value
// --------------------------------------------------------------------------
   return (MRESULT)0L;
}

// *******************************************************************************
// FUNCTION:     MidHelpKeys
//
// FUNCTION USE: Display help for the application's special keys
//
// DESCRIPTION:  After obtaining a valid help instance handle, send
//               a HM_KEYS_HELP message to the help instance.
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
static MRESULT MidHelpKeys( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND     hwndHelp;

// --------------------------------------------------------------------------
// Get the help instance handle and send the message if valid.
// --------------------------------------------------------------------------
   hwndHelp = WinQueryHelpInstance( hwnd );
   if (hwndHelp != NULLHANDLE) {
      WinSendMsg( hwndHelp, HM_KEYS_HELP, MPFROMLONG(0L), MPFROMLONG(0L));
   } /* endif */

// --------------------------------------------------------------------------
// Return with the proper reserved value
// --------------------------------------------------------------------------
   return (MRESULT)0L;
}

// *******************************************************************************
// FUNCTION:     MidHelpProd
//
// FUNCTION USE: Display the application's product information dialog
//
// DESCRIPTION:  Call WinDlgBox to display the dialog and exit on return.
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
static MRESULT MidHelpProd( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
// --------------------------------------------------------------------------
// Display the product information dialog
// --------------------------------------------------------------------------
   WinDlgBox( HWND_DESKTOP, hwnd, WinDefDlgProc, NULLHANDLE,
              DLGID_PRODINFO, NULL );

// --------------------------------------------------------------------------
// Return with the proper reserved value
// --------------------------------------------------------------------------
   return (MRESULT)0L;
}

// *******************************************************************************
//
// Miscellaneous utility functions
//
// *******************************************************************************

// *******************************************************************************
// FUNCTION:     ReadText
//
// FUNCTION USE: Build internal structures from file contents.
//
// DESCRIPTION:  Read each line of text from the file and build a LINE
//               structure from the text.  When all lines have been read
//               build an array of pointers to the line structures.
//
// PARAMETERS:   PAPP_DATA editor control structure
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void ReadText( PAPP_DATA pAppData )
{
   size_t   cbLine;
   char     *pText;
   LONG     iLine;
   PLINE    pline;

// --------------------------------------------------------------------------
// Clear the line and character counts in the editor control structure
// --------------------------------------------------------------------------
   pAppData->sNumLines = 0;
   pAppData->lMaxWidth = 0;

// --------------------------------------------------------------------------
// The file is only processed if opened.  No file will exist when the a
// new file is created.
// --------------------------------------------------------------------------
   if( pAppData->pfile != NULL ) {

// --------------------------------------------------------------------------
// Read lines of text until the end of file is reached.  Insert each
// line into a linked list and count the number of lines.  If the current
// line contains more characters than previous lines, update the maximum
// line size in the editor control structure.
// --------------------------------------------------------------------------
      while (!IsEOF( pAppData->pfile )) {
         cbLine = ReadFile( pAppData->pfile, &pText );
         if (InsertListElement( pAppData, pText, cbLine) != (PLINE)NULL) {
            pAppData->sNumLines++;
            if (pAppData->lMaxWidth < cbLine) {
               pAppData->lMaxWidth = cbLine;
            } /* endif */
         } /* endif */
      } /* endwhile */
   } /* endif */

// --------------------------------------------------------------------------
// Were any lines found?  If not, force one empty line into the structures
// --------------------------------------------------------------------------
   if (pAppData->sNumLines == 0L) {
      if (InsertListElement( pAppData, "", 0) != (PLINE)NULL) {
         pAppData->sNumLines = 1;
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// Allocate an array of sufficient size to hold pointers to all the lines.
// --------------------------------------------------------------------------
   pAppData->pLines = (PLINE *)malloc( sizeof(PLINE) * pAppData->sNumLines );


// --------------------------------------------------------------------------
// Scan the linked list of lines, placing pointers to each line in
// consecutive locations in the array.
// --------------------------------------------------------------------------
   if (pAppData->pLines != (PLINE *)NULL) {
      pline = pAppData->plineFirst;
      iLine = 0;
      while (pline != (PLINE)NULL) {
         pAppData->pLines[ iLine ] = pline;
         iLine++;
         pline = pline->next;
      } /* endwhile */
   } /* endif */

// --------------------------------------------------------------------------
// Reset the current line and cursor locations in the editor control
// structure to the first character in the file.  Indicate that the file
// has not yet been changed.
// --------------------------------------------------------------------------
   pAppData->sCurTop = pAppData->sCurLine = 0;
   pAppData->lCurLeft = pAppData->lCurCol = 0;
   pAppData->fFlags &= ~FLAG_CHANGED;

// --------------------------------------------------------------------------
// Done, back to the caller.
// --------------------------------------------------------------------------
   return;
}

// *******************************************************************************
// FUNCTION:     WriteText
//
// FUNCTION USE:
//
// DESCRIPTION:
//
// PARAMETERS:   PAPP_DATA editor control structure
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void WriteText( PAPP_DATA pAppData )
{
   LONG     iLine;
   PLINE    pline;
   LONG     cLines;

   cLines = pAppData->sNumLines;
   iLine = 0;
   while (cLines--) {
      pline = pAppData->pLines[iLine];
      WriteFile( pAppData->pfile, pline->pszText, iLine == 0 );
      iLine++;
   } /* endwhile */

   return;
}

// *******************************************************************************
// FUNCTION:     CalcCurrentPos
//
// FUNCTION USE: Determine coordinates of the current input location.
//
// DESCRIPTION:  Calculate the x and y coordinates of the lower left corner
//               at which the current input character is drawn.
//
// PARAMETERS:   HWND      client window handle.
//               PAPP_DATA editor control structure
//               PULONG    pointer to location to store x coordinate
//               PULONG    pointer to location to store y coordinate
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void CalcCurrentPos( HWND hwnd, PAPP_DATA pAppData,
                            PLONG plx, PLONG ply )
{
   LONG  cLines = 1;
   RECTL rectl;

   *plx = ( pAppData->lCurCol - pAppData->lCurLeft ) * pAppData->lWidth;

   cLines += pAppData->sCurLine - pAppData->sCurTop;

   WinQueryWindowRect( hwnd, &rectl );
   *ply = rectl.yTop - (cLines * pAppData->lHeight);
}

// *******************************************************************************
// FUNCTION:     InsertCharacter
//
// FUNCTION USE: Insert a new character or overly an existing character
//               at the current cursor location.
//
// DESCRIPTION:  Modify the text of the current line by updating the
//               character at the current cursor location and redraw
//               the line as necessary to reflect the change.  Move
//               the current location to the next character.
//
//
// PARAMETERS:   HWND      client window handle.
//               char      character to be inserted at current location
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void InsertCharacter( HWND hwnd, char chInsert )
{
   PAPP_DATA   pAppData = (PAPP_DATA)NULL;
   PLINE       pline = (PLINE)NULL;
   HPS         hps;
   PSZ         pszNewText = (PSZ)NULL;
   RECTL       rectlText;
   LONG        cchText;
   LONG        nchText;

// --------------------------------------------------------------------------
// Obtain the editor control structure from the window instance data.
// If pointer is NULL, there is no need to create the input cursor, just
// return.
// --------------------------------------------------------------------------
   pAppData = (PAPP_DATA)WinQueryWindowPtr( hwnd, APP_DATA_POINTER );
   if (pAppData != (PAPP_DATA)NULL) {

// --------------------------------------------------------------------------
// Obtain a pointer to the current line and obtain the number of characters
// in the line.
// --------------------------------------------------------------------------
      pline = pAppData->pLines[pAppData->sCurLine];
      cchText = strlen( pline->pszText );

// --------------------------------------------------------------------------
// Determine the coordinates of the current character.  This location will
// be used as the origin for later drawing operations.
// --------------------------------------------------------------------------
      CalcCurrentPos( hwnd, pAppData,
                      &rectlText.xLeft, &rectlText.yBottom );

// --------------------------------------------------------------------------
// If the user has requested overstrike mode, just replace and redraw
// the current character, unless the current location is the end of the
// line, in which case, overstrike mode is ignored.
// --------------------------------------------------------------------------
      if (pAppData->fFlags & FLAG_REPLACE &&
          pAppData->lCurCol < cchText ) {
         pline->pszText[pAppData->lCurCol] = chInsert;
         rectlText.xRight = pAppData->lWidth + rectlText.xLeft;

// --------------------------------------------------------------------------
// otherwise, the character must be inserted.
// --------------------------------------------------------------------------
      } else {

// --------------------------------------------------------------------------
// allocate memory for the expanded text.  If the current column is at or
// beyond the end of the current text, allocate enough space for a line
// reaching to the current column location.  Otherwise, increase the space
// allocation by one character (add 2 to account for the new character and
// the null terminator.
// --------------------------------------------------------------------------
         if( pAppData->lCurCol >= cchText ) {
            pszNewText = (PSZ)malloc(pAppData->lCurCol+2);
         } else {
            pszNewText = (PSZ)malloc( cchText + 2 );
         } /* endif */

         if (pszNewText != (PSZ)NULL) {

// --------------------------------------------------------------------------
// first copy over the existing text and, if necessary, insert spaces to fill
// to the current location.
// --------------------------------------------------------------------------
            strcpy( pszNewText, pline->pszText);
            for (nchText = cchText; nchText < pAppData->lCurCol; nchText++) {
               pszNewText[nchText] = ' ';
            } /* endfor */

// --------------------------------------------------------------------------
// now put the new character in a the current column location and follow
// with a null terminator.
// --------------------------------------------------------------------------
            pszNewText[ pAppData->lCurCol ] = chInsert;
            pszNewText[ pAppData->lCurCol + 1 ] = '\0';

// --------------------------------------------------------------------------
// finally, copy over any text from the original text which follows the
// current cursor location - possibly overlaying the null terminator just
// added.
// --------------------------------------------------------------------------
            if( pAppData->lCurCol <= cchText ) {
               strcat( pszNewText, &pline->pszText[ pAppData->lCurCol ] );
            } /* endif */

// --------------------------------------------------------------------------
// set the line structure text pointer to point to the new text and free
// the memory for the old text.
// --------------------------------------------------------------------------
            free( pline->pszText );
            pline->pszText = pszNewText;

// --------------------------------------------------------------------------
// compute x coordinate of rectangle required to draw characters from the
// current location out to the end of the line.
// --------------------------------------------------------------------------
            rectlText.xRight = strlen(&pline->pszText[pAppData->lCurCol]) *
                           pAppData->lWidth + rectlText.xLeft;
         } /* endif */
      } /* endif */

// --------------------------------------------------------------------------
// compute y coordinate for top of the drawing rectangle and hide the cursor
// before the drawing operation.
// --------------------------------------------------------------------------
      rectlText.yTop = rectlText.yBottom + pAppData->lHeight;
      WinShowCursor( hwnd, FALSE );

// --------------------------------------------------------------------------
// get the presentation space for the window and draw the text
// --------------------------------------------------------------------------
      hps = WinGetPS( hwnd );
      if (hps != NULLHANDLE) {
         WinDrawText( hps, -1, &pline->pszText[pAppData->lCurCol], &rectlText,
                      0, 0, DT_TOP | DT_LEFT | DT_TEXTATTRS | DT_ERASERECT );
         WinReleasePS( hps );
      } /* endif */

// --------------------------------------------------------------------------
// a change has occured so set the changed flag.
// --------------------------------------------------------------------------
      pAppData->fFlags |= FLAG_CHANGED;


// --------------------------------------------------------------------------
// see if the change has increased the maximum number of characters in a
// line.  If so, update the editor control structure and recompute the
// scoll bar ranges and thumb sizes.
// --------------------------------------------------------------------------
      cchText = strlen( pline->pszText );
      if( cchText > pAppData->lMaxWidth ) {
         pAppData->lMaxWidth = cchText;
         RecalcScrollRange( hwnd, pAppData );
      } /* endif */

// --------------------------------------------------------------------------
// simulate and right arrow key stroke to move the input cursor to the next
// character (handles cursor movement, scrolling, etc.).
// --------------------------------------------------------------------------
      WinSendMsg( hwnd, WM_CHAR, MPFROM2SHORT( KC_VIRTUALKEY, 0 ),
                                 MPFROM2SHORT( 0, VK_RIGHT ));
   } /* endif */

// --------------------------------------------------------------------------
// back to the caller.
// --------------------------------------------------------------------------
   return;
}

// *******************************************************************************
// FUNCTION:     DeleteCharacter
//
// FUNCTION USE: Removes the character at or preceding the current input
//               location.
//
// DESCRIPTION:  Modifies the text of the current line by removing either
//               the character at the current location or the character
//               preceding the current location and then updating the
//               display.
//
//
// PARAMETERS:   HWND      client window handle.
//               ULONG     direction flag.
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void DeleteCharacter( HWND hwnd, ULONG ulDirection )
{
   PAPP_DATA   pAppData = (PAPP_DATA)NULL;
   PLINE       pline = (PLINE)NULL;
   HPS         hps;
   RECTL       rectlText;
   ULONG       nchChar;

// --------------------------------------------------------------------------
// Obtain the editor control structure from the window instance data.
// If pointer is NULL, there is no need to create the input cursor, just
// return.
// --------------------------------------------------------------------------
   pAppData = (PAPP_DATA)WinQueryWindowPtr( hwnd, APP_DATA_POINTER );
   if (pAppData != (PAPP_DATA)NULL) {

// --------------------------------------------------------------------------
// retrieve pointer to the current line
// --------------------------------------------------------------------------
      pline = pAppData->pLines[pAppData->sCurLine];

// --------------------------------------------------------------------------
// If deleting previous character, make sure that current location is
// not at the beginning of the line.  Otherwise, move the cursor location
// back one and proceed as if deleting forward.
// --------------------------------------------------------------------------
      if (ulDirection == DELETE_BACKWARD) {
         if (pAppData->lCurCol != 0) {
            WinSendMsg( hwnd, WM_CHAR, MPFROM2SHORT( KC_VIRTUALKEY, 0 ),
                                       MPFROM2SHORT( 0, VK_LEFT ));
         } else {
            return;
         } /* endif */
      } /* endif */

// --------------------------------------------------------------------------
// Can't delete forward at the end of the line.
// --------------------------------------------------------------------------
      if( pAppData->lCurCol >= strlen( pline->pszText )) return;

// --------------------------------------------------------------------------
// delete the current character by copying the following characters up on
// in the array.
// --------------------------------------------------------------------------
      for (nchChar = pAppData->lCurCol;
           pline->pszText[nchChar] != '\0';
           nchChar++) {
         pline->pszText[nchChar] = pline->pszText[nchChar+1];
      } /* endfor */
//      strcpy( &pline->pszText[pAppData->lCurCol],
//              &pline->pszText[pAppData->lCurCol + 1] );

// --------------------------------------------------------------------------
// Set flag to indicate that a change has occurred
// --------------------------------------------------------------------------
      pAppData->fFlags |= FLAG_CHANGED;

// --------------------------------------------------------------------------
// Get coordinates of current cursor location.  Build a rectangle for drawing
// from this location to the end of the line.
// --------------------------------------------------------------------------
      CalcCurrentPos( hwnd, pAppData,
                      &rectlText.xLeft, &rectlText.yBottom );
      rectlText.xRight = (strlen(&pline->pszText[pAppData->lCurCol]) + 1 ) *
                     pAppData->lWidth + rectlText.xLeft;
      rectlText.yTop = rectlText.yBottom + pAppData->lHeight;

// --------------------------------------------------------------------------
// Hide the cursor for painting, then get the presentation space for the
// window and draw the changed text back to the screen.
// --------------------------------------------------------------------------
      WinShowCursor( hwnd, FALSE );
      hps = WinGetPS( hwnd );
      if( hps != NULLHANDLE ) {
         WinDrawText( hps, -1, &pline->pszText[pAppData->lCurCol], &rectlText,
                      0, 0, DT_TOP | DT_LEFT | DT_TEXTATTRS | DT_ERASERECT );
         WinReleasePS( hps );
      } /* endif */

// --------------------------------------------------------------------------
// redisplay the cursor at the proper location
// --------------------------------------------------------------------------
      UpdateCursor( hwnd, pAppData );
   } /* endif */

// --------------------------------------------------------------------------
// back to the caller.
// --------------------------------------------------------------------------
   return;
}

// *******************************************************************************
// FUNCTION:     UpdateCursor
//
// FUNCTION USE: Display the keyboard input cursor at the current
//               input location.
//
// DESCRIPTION:  Obtain the coordinates of current location then recreate
//               and display the cursor at these coordinates.
//
//
// PARAMETERS:   HWND      client window handle.
//               PAPP_DATA pointer to editor control structure
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void UpdateCursor( HWND hwnd, PAPP_DATA pAppData )
{
   LONG     lx;
   LONG     ly;

// --------------------------------------------------------------------------
// Get coordinates of current input location
// --------------------------------------------------------------------------
   CalcCurrentPos( hwnd, pAppData, &lx, &ly );

// --------------------------------------------------------------------------
// Move the cursor by recreating with the CURSOR_SETPOS flag
// --------------------------------------------------------------------------
   WinCreateCursor( hwnd, lx, ly, 0, 0, CURSOR_SETPOS, NULL );

// --------------------------------------------------------------------------
// Display the cursor
// --------------------------------------------------------------------------
   WinShowCursor( hwnd, TRUE );

// --------------------------------------------------------------------------
// Back to the caller
// --------------------------------------------------------------------------
   return;
}

// *******************************************************************************
// FUNCTION:     InsertNewLine
//
// FUNCTION USE: Adds a blank line to the file after the current line.
//
// DESCRIPTION:
//
// PARAMETERS:   HWND      client window handle.
//               PAPP_DATA pointer to editor control structure
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void InsertNewLine( HWND hwnd, PAPP_DATA pAppData )
{
   PLINE    pLine;
   PLINE    pLineCur;
   PLINE    *pLineA;
   ULONG    iLine;
   RECTL    rectl;
   LONG     lx;
   LONG     ly;

// --------------------------------------------------------------------------
// Allocate a line structure for the new line
// --------------------------------------------------------------------------
   pLine = (PLINE)malloc( sizeof(LINE) );
   if (pLine != NULL) {

// --------------------------------------------------------------------------
// Allocate a text buffer of one character and store a null terminator.
// --------------------------------------------------------------------------
      pLine->pszText = (PSZ)malloc( 1 );
      if (pLine->pszText != NULL) {
         pLine->pszText[0] = '\0';

// --------------------------------------------------------------------------
// Get a pointer to the current line's line structure
// --------------------------------------------------------------------------
         pLineCur = pAppData->pLines[ pAppData->sCurLine ];

// --------------------------------------------------------------------------
// Add the structure to the linked list.
// --------------------------------------------------------------------------
         pLine->previous = pLineCur;
         pLine->next = pLineCur->next;
         pLineCur->next = pLine;
         if (pLine->next != NULL) {
            pLine->next->previous = pLine;
         } else {
            pAppData->plineLast = pLine;
         } /* endif */

// --------------------------------------------------------------------------
// Reallocate and rebuild the array of line pointers by scanning the linked
// list and storing a pointer to each line in the array.
// --------------------------------------------------------------------------
         pAppData->sNumLines++;
         pLineA = (PLINE *)malloc( sizeof(PLINE) * pAppData->sNumLines);
         if (pLineA != NULL) {
            pLine = pAppData->plineFirst;
            iLine = 0;
            while (pLine != (PLINE)NULL) {
               pLineA[ iLine ] = pLine;
               iLine++;
               pLine = pLine->next;
            } /* endwhile */

// --------------------------------------------------------------------------
// release the original array and store the pointer to the new array in the
// editor control structure.
// --------------------------------------------------------------------------
            free( pAppData->pLines );
            pAppData->pLines = pLineA;

// --------------------------------------------------------------------------
// set the current line to the new line, then update the screen to include
// the new blank line.
// --------------------------------------------------------------------------
            WinQueryWindowRect( hwnd, &rectl );
            CalcCurrentPos( hwnd, pAppData, &lx, &ly );
            rectl.yTop = ly;
            WinInvalidateRect( hwnd, &rectl, TRUE );
            pAppData->sCurLine++;
            pAppData->lCurCol = 0;
            RecalcScrollRange( hwnd, pAppData );
            WinSendMsg( hwnd, WM_CHAR, MPFROM2SHORT( KC_VIRTUALKEY, 0 ),
                                       MPFROM2SHORT( 0, VK_HOME ));
            UpdateCursor( hwnd, pAppData );

// --------------------------------------------------------------------------
// free any allocated memory on failure
// --------------------------------------------------------------------------
         } else {
            free( pLine->pszText );
            free( pLine );
         } /* endif */
      } else {
         free( pLine );
      } /* endif */
   } /* endif */

   return;
}

// *******************************************************************************
// FUNCTION:     DeleteLine
//
// FUNCTION USE: Removes the current line from the file.
//
// DESCRIPTION:
//
// PARAMETERS:   HWND      client window handle.
//               PAPP_DATA pointer to editor control structure
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void DeleteLine( HWND hwnd, PAPP_DATA pAppData )
{
   PLINE    pLine;
   RECTL    rectl;
   ULONG    iLine;
   LONG     lx;
   LONG     ly;

// --------------------------------------------------------------------------
// Get a pointer to the line structure for the current line.
// --------------------------------------------------------------------------
   pLine = pAppData->pLines[ pAppData->sCurLine ];
   DeleteListElement( pAppData, pLine );


// --------------------------------------------------------------------------
// Now move the remaining elements of the array up by one line
// --------------------------------------------------------------------------
   for (iLine = pAppData->sCurLine;
        iLine < pAppData->sNumLines - 1; iLine++) {
      pAppData->pLines[ iLine ] = pAppData->pLines[ iLine+1 ];
   } /* endfor */

// --------------------------------------------------------------------------
// Free memory allocated for the current line structure
// --------------------------------------------------------------------------
   DeleteListElement( pAppData, pLine );

// --------------------------------------------------------------------------
// Compute the area of the window which needs to be updated - the
// current line and below
// --------------------------------------------------------------------------
   pAppData->sNumLines--;
   WinQueryWindowRect( hwnd, &rectl );
   CalcCurrentPos( hwnd, pAppData, &lx, &ly );
   rectl.yTop = ly + pAppData->lHeight;
   WinInvalidateRect( hwnd, &rectl, TRUE );

// --------------------------------------------------------------------------
// Adjust scroll bars and set current cursor position
// --------------------------------------------------------------------------
   RecalcScrollRange( hwnd, pAppData );
   UpdateCursor( hwnd, pAppData );

// --------------------------------------------------------------------------
// Back to the caller.
// --------------------------------------------------------------------------
   return;
}

// *******************************************************************************
// FUNCTION:     ChangeFileName
//
// FUNCTION USE: Update the file name in the editor control structure
//               and the title bar.
//
// DESCRIPTION:  Release the space allocated for any previous file name, then
//               allocate space for the new file name if it is non-null. Update
//               the title bar to display the new file name, or the untitled
//               file name if a new name was not given.
//
// PARAMETERS:   HWND      client window handle
//               PAPP_DATA editor control structure
//               PSZ       pointer to new file name
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void ChangeFileName( HWND hwnd, PAPP_DATA pAppData, PSZ pszNewName )
{
   HWND  hwndFrame;
   int   cChars;
   PSZ   pszNewTitle;

// --------------------------------------------------------------------------
// If a file name is already registered - deallocate its memory and set the
// file name pointer to NULL.
// --------------------------------------------------------------------------
   if (pAppData->pszFileName) {
      free( pAppData->pszFileName );
      pAppData->pszFileName = (PSZ)NULL;
   } /* endif */

// --------------------------------------------------------------------------
// If a new file name was given - allocate space and store the new name.
// Save the pointer to the name in the editor control structure.  Otherwise,
// temporarily set the name to the "untitled" name.
// --------------------------------------------------------------------------
   if( pszNewName && (cChars = strlen(pszNewName))) {
      pAppData->pszFileName = (PSZ)malloc( cChars + 1 );
      if( pAppData->pszFileName ) strcpy( pAppData->pszFileName, pszNewName );
   } else {
      pszNewName = NOFILE_NAME;
   } /* endif */

// --------------------------------------------------------------------------
// Allocate space to build the title bar string.  If successful, build the
// new title bar string as the application name + the file name, update
// the title bar and release the allocated memory.
// --------------------------------------------------------------------------
   cChars = strlen(TITLE_BAR_TEXT) + strlen(pszNewName);
   pszNewTitle = (PSZ)malloc( cChars + 1 );
   if( pszNewTitle ) {
      strcpy( pszNewTitle, TITLE_BAR_TEXT );
      strcat( pszNewTitle, pszNewName );

      hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
      WinSetWindowText( hwndFrame, pszNewTitle );
      free( pszNewTitle );
   } /* endif */
}

// *******************************************************************************
// FUNCTION:     RecalcScrollRange
//
// FUNCTION USE:
//
// DESCRIPTION:
//
//
// PARAMETERS:   HWND      client window handle.
//               char      character to be inserted at current location
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void RecalcScrollRange( HWND hwnd, PAPP_DATA pAppData )
{
   HWND     hwndFrame;
   HWND     hwndScroll;


// --------------------------------------------------------------------------
// Move the vertical scroll bar to the top of the file and update the thumb
// size based on the visible portion of the file.
// --------------------------------------------------------------------------
   hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
   hwndScroll = WinWindowFromID( hwndFrame, FID_VERTSCROLL );
   WinSendMsg( hwndScroll, SBM_SETSCROLLBAR,
               MPFROM2SHORT( (SHORT)pAppData->sCurTop, 0 ),
               MPFROM2SHORT( 0, (SHORT)pAppData->sNumLines - 1));
   WinSendMsg( hwndScroll, SBM_SETTHUMBSIZE,
               MPFROM2SHORT( (SHORT)pAppData->sLinesPerPage,
                             (SHORT)pAppData->sNumLines), 0L );

// --------------------------------------------------------------------------
// Move the horizontal scroll bar to the far left and update the thumb size
// based on the visible portion of the file.
// --------------------------------------------------------------------------
   hwndScroll = WinWindowFromID( hwndFrame, FID_HORZSCROLL );
   WinSendMsg( hwndScroll, SBM_SETSCROLLBAR,
               MPFROM2SHORT( (SHORT)pAppData->lCurLeft, 0 ),
               MPFROM2SHORT( 0, (SHORT)pAppData->lMaxWidth - 1));
   WinSendMsg( hwndScroll, SBM_SETTHUMBSIZE,
               MPFROM2SHORT( (SHORT)pAppData->lColsPerPage,
                             (SHORT)pAppData->lMaxWidth), 0L );
}

// *******************************************************************************
//
// List handling functions
//
// *******************************************************************************

// *******************************************************************************
// FUNCTION:     DestroyList
//
// FUNCTION USE: Remove all elements of a list
//
// DESCRIPTION:  Repeatedly delete the first list items until no
//               items remain.
//
// PARAMETERS:   PAPP_DATA pointer to editor control structure.
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void DestroyList( PAPP_DATA pAppData )
{

   while (pAppData->plineFirst != (PLINE)NULL) {
      DeleteListElement( pAppData, pAppData->plineFirst );
   }

   return;
}

// *******************************************************************************
// FUNCTION:     DeleteListElement
//
// FUNCTION USE: Remove a single item from a linked list.
//
// DESCRIPTION:  Remove the item from the list and free all related
//               memory.
//
// PARAMETERS:   PAPP_DATA pointer to editor control structure
//               PLINE     pointer to item to remove
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void DeleteListElement( PAPP_DATA pAppData, PLINE plineItem )
{
// --------------------------------------------------------------------------
// If not the first item in the list - set the next pointer of the previous
// item to the item following this one.
// --------------------------------------------------------------------------
   if (plineItem->previous != (PLINE)NULL) {
      plineItem->previous->next = plineItem->next;
   } else {
      pAppData->plineFirst = plineItem->next;
   } /* endif */

// --------------------------------------------------------------------------
// If not the last item in the list - set the previous pointer of the next
// item to the item preceding this one.
// --------------------------------------------------------------------------
   if (plineItem->next != (PLINE)NULL) {
      plineItem->next->previous = plineItem->previous;
   } else {
      pAppData->plineLast = plineItem->previous;
   } /* endif */

// --------------------------------------------------------------------------
// free the memory allocated for this item's text
// --------------------------------------------------------------------------
   if (plineItem->pszText != (PSZ)NULL) {
      free( plineItem->pszText );
   } /* endif */

// --------------------------------------------------------------------------
// free the memory allocated for this item.
// --------------------------------------------------------------------------
   free( plineItem );

// --------------------------------------------------------------------------
// Back to the caller
// --------------------------------------------------------------------------
   return;
}

// *******************************************************************************
// FUNCTION:     InsertListElement
//
// FUNCTION USE: Create a new LINE item and insert in list.
//
// DESCRIPTION:  Allocates a new line item and memory for the text of the
//               line and then adds the new line to the end of a linked
//               list of LINE structures.
//
//
// PARAMETERS:   PAPP_DATA pointer to editor control structure
//               char*     pointer to text for the line
//               ULONG     number of characters in text
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static PLINE InsertListElement( PAPP_DATA pAppData, char *pText,
                                ULONG cbText )
{
   PLINE    plineNew = (PLINE)NULL;

// --------------------------------------------------------------------------
// allocate a new list element
// --------------------------------------------------------------------------
   plineNew = (PLINE)malloc( sizeof(LINE) );
   if (plineNew != (PLINE)NULL) {

// --------------------------------------------------------------------------
// initialize the new element's contents and allocate memory to store the
// text associated with the element.
// --------------------------------------------------------------------------
      memset( (PVOID)plineNew, 0, sizeof( TEXTLINE ));
      plineNew->pszText = (PSZ)malloc( cbText + 1 );
      if (plineNew->pszText != (PSZ)NULL) {

// --------------------------------------------------------------------------
// copy the text into the allocated buffer
// --------------------------------------------------------------------------
         strncpy( plineNew->pszText, pText, cbText );
         plineNew->pszText[ cbText ] = '\0';

// --------------------------------------------------------------------------
// add the new item to the end of the linked list.
// --------------------------------------------------------------------------
         if (pAppData->plineFirst == NULL) {
            pAppData->plineFirst = pAppData->plineLast = plineNew;
         } else {
            plineNew->previous = pAppData->plineLast;
            pAppData->plineLast->next = plineNew;
            pAppData->plineLast = plineNew;
         } /* endif */
      } else {

// --------------------------------------------------------------------------
// no memory for the text - abort the operation
// --------------------------------------------------------------------------
         free( plineNew );
         plineNew = (PLINE)NULL;
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// return the pointer to the new list element back to the caller.
// --------------------------------------------------------------------------
   return plineNew;
}

// *******************************************************************************
// FUNCTION:     NotSavedDlgProc
//
// FUNCTION USE: Dialog Procedure for the "Not Saved" dialog box.
//
// DESCRIPTION:  Insert the file name at initialization time and returns
//               the button used to dismiss the dialog.
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
MRESULT APIENTRY NotSavedDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   LONG     nch;
   PSZ      pch;
   PSZ      pout;
   PAPP_DATA   pAppData = (PAPP_DATA)mp2;

   switch (msg) {
   case WM_INITDLG:
// --------------------------------------------------------------------------
// Get the editor control structure
// --------------------------------------------------------------------------
      pAppData = (PAPP_DATA)mp2;
      if( pAppData != NULL ) {

// --------------------------------------------------------------------------
// Disable the cancel button if closing the application.  Do it now or
// forget it.
// --------------------------------------------------------------------------
         if( pAppData->fFlags & FLAG_CLOSING ) {
            WinEnableControl( hwnd, DID_CANCEL, FALSE );
         } /* endif */

// --------------------------------------------------------------------------
// Get length of static text, allocate a buffer, then get the text itself
// --------------------------------------------------------------------------
         nch = WinQueryDlgItemTextLength( hwnd, DID_MESSAGE );
         pch = malloc( nch + 1 );
         if ( pch != NULL ) {
            WinQueryDlgItemText( hwnd, DID_MESSAGE, nch + 1, pch );

// --------------------------------------------------------------------------
// The text contains a printf format descriptor for displaying the filename,
// so add the length of the file name and build the actual display text.
// --------------------------------------------------------------------------
            nch += strlen( pAppData->pszFileName );
            pout = malloc( nch );
            if( pout != NULL ) {
               sprintf( pout, pch, pAppData->pszFileName );

// --------------------------------------------------------------------------
// Update the control text
// --------------------------------------------------------------------------
               WinSetDlgItemText( hwnd, DID_MESSAGE, pout );
               free( pout );
            } /* endif */
            free(pch);
         } /* endif */
      } /* endif */

// --------------------------------------------------------------------------
// No focus change - return FALSE
// --------------------------------------------------------------------------
      return MRFROMLONG(FALSE);

// --------------------------------------------------------------------------
// Button pressed, dismiss the dialog, returning the button id
// --------------------------------------------------------------------------
   case WM_COMMAND:
        WinDismissDlg( hwnd, SHORT1FROMMP(mp1));
        return MRFROMLONG(FALSE);


// --------------------------------------------------------------------------
// all other messages go to the default dialog procedure
// --------------------------------------------------------------------------
   default:
        return WinDefDlgProc(hwnd, msg, mp1, mp2);
   } /* endswitch */
}

