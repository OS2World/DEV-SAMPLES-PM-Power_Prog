// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      MATCH.C                                          mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Main Source File for MATCH/Chapter 13 Sample Program          บ
// บ                                                                               บ
// บ  NOTES:         This program is a matching game that demonstrates the use     บ
// บ                 of the graphic button control                                 บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#define  INCL_PM
#define  INCL_SW
#define  INCL_DOS
#include <os2.h>
#include <os2me.h>
#include <shcommon.h>
#include <stdlib.h>
#include <stdio.h>
#include "match.h"

// --------------------------------------------------------------------------
// Miscellaneous constant definitions used only in this source module.
// --------------------------------------------------------------------------
#define  APP_WINDOW_DATA_SIZE          0
#define  GBTNID                        1100
#define  MAX_BITMAPS                   7
#define  NUM_BUTTONS                   25
#define  BUTTONS_PER_ROW               5
#define  WILD_PRIZE                    0
#define  TAKE_PRIZE                    1

// --------------------------------------------------------------------------
// Define application-defined message used in this program
// --------------------------------------------------------------------------
#define  UM_SIZEBTN                    (WM_USER)
#define  UM_SIZEWIN                    (WM_USER+1)

// --------------------------------------------------------------------------
// Define timer id's
// --------------------------------------------------------------------------
#define  ID_TIMERMATCH                 (TID_USERMAX-1)
#define  ID_TIMERNOMATCH               (TID_USERMAX-2)

// --------------------------------------------------------------------------
// Prototype for window/dialog procedures
// --------------------------------------------------------------------------
MRESULT APIENTRY ClientWndProc   (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY ProdInfoDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

// --------------------------------------------------------------------------
// Prototypes for the message processing worker functions
// --------------------------------------------------------------------------
static MRESULT umSizeBtn( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmCommand( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmControl( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmPaint( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmSize( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmTimer( HWND hwnd, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------
// Prototypes for miscellaneous utility functions
// --------------------------------------------------------------------------
static MRESULT NewGame( HWND hwnd, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------
// Structure and type definitions
// --------------------------------------------------------------------------
#pragma pack(1)
typedef struct {
   GBTNCDATA   btndata;
   USHORT      bitmaps[MAX_BITMAPS - 1];
} GRPHBTN, *PGRPHBTN;

typedef struct {
   USHORT      btnNumber[2];
   USHORT      cBitmap;
   PSZ         pszPrize;
   USHORT      bitmaps[MAX_BITMAPS - 1];
} PRIZE, *PPRIZE;

#pragma pack()

// --------------------------------------------------------------------------
// Control data for the game buttons
// --------------------------------------------------------------------------
GRPHBTN  gbtndata[ 25 ] = {
   GB_STRUCTURE, NULL, NULLHANDLE, 2,  1, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2,  2, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2,  3, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2,  4, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2,  5, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2,  6, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2,  7, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2,  8, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2,  9, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2, 10, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2, 11, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2, 12, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2, 13, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2, 14, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2, 15, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2, 16, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2, 17, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2, 18, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2, 19, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2, 20, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2, 21, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2, 22, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2, 23, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2, 24, 0, 0, 0, 0, 0, 0,
   GB_STRUCTURE, NULL, NULLHANDLE, 2, 25, 0, 0, 0, 0, 0, 0
};

// --------------------------------------------------------------------------
// Array of prizes to be won and ICON definitions
// --------------------------------------------------------------------------
PRIZE prizes[12] = {
   0, 0, 5,  "Wild", 150, 151, 152, 153, 154,   0,
   0, 0, 6,  "Take", 170, 171, 172, 173, 174, 175,
   0, 0, 5,  "Trip", 100, 101, 102, 103, 104, 150,
   0, 0, 1,   "Car", 110,   0,   0,   0,   0,   0,
   0, 0, 3,  "Ring", 120, 121, 122,   0,   0,   0,
   0, 0, 1,  "Cash", 130,   0,   0,   0,   0,   0,
   0, 0, 1,  "Boat", 140,   0,   0,   0,   0,   0,
   0, 0, 1, "China", 160,   0,   0,   0,   0,   0,
   0, 0, 3, "Piano", 180, 181, 182,   0,   0,   0,
   0, 0, 3, "Cycle", 190, 191, 192,   0,   0,   0,
   0, 0, 4,    "TV", 200, 201, 202, 203,   0,   0,
   0, 0, 1,   "Bed", 210,   0,   0,   0,   0,   0
};

// --------------------------------------------------------------------------
// Global variables
// --------------------------------------------------------------------------
BOOL        false = FALSE;
HBITMAP     hbm;
USHORT      btnPrize[25];
HWND        gbtnhwnd[25];
USHORT      gbtnDownID = 0xffff;
USHORT      gbtnDownID1;
USHORT      gbtnDownID2;
char        szClassName[] = "MATCH";
char        szTitle[] = "MATCH - Sample MMPM Match Game";
BOOL        fTimerWait = FALSE;
RECTL       gbtnDefRectl;
USHORT      gMatched;
USHORT      gTries;

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
// INTERNALS:    NONE
//
// *******************************************************************************
main(int argc, char *argv[], char *envp[])
{
   HAB      hab;
   HMQ      hmq;
   QMSG     qmsg;
   HWND     hwndFrame;
   HWND     hwndClient;
   RECTL    rectl;
   ULONG    flCreateFlags = (FCF_STANDARD &
        ~( FCF_ACCELTABLE | FCF_SHELLPOSITION )) | FCF_NOBYTEALIGN;

// --------------------------------------------------------------------------
// Single iteration loop - makes error handling easier
// --------------------------------------------------------------------------
   do {
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
// Register the graphic button class for later
// --------------------------------------------------------------------------
      WinRegisterGraphicButton();

// --------------------------------------------------------------------------
// Register a class for the main application window */
// --------------------------------------------------------------------------
      if (!WinRegisterClass( hab,
                          szClassName,
                          ClientWndProc,
                          CS_SIZEREDRAW | CS_CLIPCHILDREN,
                          APP_WINDOW_DATA_SIZE)) {
         // failed!
         DisplayMessages(NULLHANDLE, "Unable to Register Window Class",
                         MSG_ERROR);
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Finally, now the window can be created
// --------------------------------------------------------------------------
      hwndFrame = WinCreateStdWindow( HWND_DESKTOP,   // Parent window
                                      0UL,            // Initial style
                                      &flCreateFlags, // Frame creation flags
                                      szClassName,    // Class name
                                      szTitle,        // Title bar text
                                      0UL,            // Client style
                                      NULLHANDLE,     // Resource in .EXE
                                      ID_MAINWND,     // Frame window id
                                      &hwndClient );  // Client window
      if (hwndFrame == (HWND)NULLHANDLE) {
         // no window - gotta leave.
         DisplayMessages(NULLHANDLE, "Error Creating Frame Window",
                         MSG_ERROR);
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Adjust window size to match default button size.  First get the rect
// for a button - these are all at default because of how they were created.
// --------------------------------------------------------------------------
      WinQueryWindowRect( gbtnhwnd[1], &gbtnDefRectl );
      rectl = gbtnDefRectl;

// --------------------------------------------------------------------------
// Compute the rectange required hold all buttons
// --------------------------------------------------------------------------
      rectl.xRight *= BUTTONS_PER_ROW;
      rectl.yTop *= BUTTONS_PER_ROW;

// --------------------------------------------------------------------------
// Map to the desktop and get the frame size required for this client size.
// --------------------------------------------------------------------------
      WinMapWindowPoints( hwndClient, HWND_DESKTOP, (PPOINTL)&rectl, 2 );
      WinCalcFrameRect( hwndFrame, &rectl, FALSE );

// --------------------------------------------------------------------------
// Compute the width and height of the frame then change the size
// --------------------------------------------------------------------------
      rectl.xRight -= rectl.xLeft;
      rectl.yTop -= rectl.yBottom;
      WinSetWindowPos( hwndFrame, NULLHANDLE, 0, 0,
                       rectl.xRight, rectl.yTop, SWP_SIZE | SWP_MOVE );

// --------------------------------------------------------------------------
// Show the window to the user
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
// Clean up on exit - releasing the message queue and PM environment
// --------------------------------------------------------------------------
   if (hmq != (HMQ)NULLHANDLE) WinDestroyMsgQueue( hmq );

   if (hab != (HAB)NULLHANDLE) WinTerminate( hab );

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
MRESULT APIENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   switch (msg) 
    {
     case UM_SIZEBTN:           
          return umSizeBtn(hwnd, mp1, mp2); 
 
     case WM_CREATE:            
          return wmCreate(hwnd, mp1, mp2); 

     case WM_COMMAND:           
          return wmCommand(hwnd, mp1, mp2); 

     case WM_CONTROL:           
          return wmControl(hwnd, mp1, mp2); 

     case WM_PAINT:             
          return wmPaint(hwnd, mp1, mp2); 

     case WM_SIZE:              
          return wmSize(hwnd, mp1, mp2);  

     case WM_TIMER:             
          return wmTimer(hwnd, mp1, mp2); 

     default:                   
          return WinDefWindowProc( hwnd, msg, mp1, mp2 ); 

    } /* endswitch */
}

// *******************************************************************************
// FUNCTION:     umSizeBtn
//
// FUNCTION USE: Worker function to process the UM_SIZEBTN message.
//
// DESCRIPTION:  This routine modifies the size and position of the graphic
//               buttons when the size of the client window changes.
//               Unfortunately, graphics buttons have a minimum size based
//               on the size of the graphic.  If we try to size and position
//               below this minimum, the button size is not adjusted and
//               buttons overlay each other.  To avoid this, we saved the
//               the minimum size when the buttons were created.  If the
//               client area is too small to draw all the buttons, we just
//               size and position the buttons as required for the minimum,
//               this may mean that some buttons are partially or completely
//               hidden.  If the client is larger than this minimum size, the
//               buttons size and position are modified to fill the client
//               window area.
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
static MRESULT umSizeBtn( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   RECTL       rectlClient;
   LONG        lBtnIndex;
   LONG        column;
   LONG        row;
   LONG        x, y, cx, cy;

// --------------------------------------------------------------------------
// Get the client window size
// --------------------------------------------------------------------------
   WinQueryWindowRect( hwnd, &rectlClient );

// --------------------------------------------------------------------------
// Get the button for this message and compute the row and column
// --------------------------------------------------------------------------
   lBtnIndex = LONGFROMMP( mp1 );
   column = lBtnIndex % BUTTONS_PER_ROW;
   row = (lBtnIndex / BUTTONS_PER_ROW) + 1;

// --------------------------------------------------------------------------
// Compute the minimum client size to display all buttons at minimum size
// --------------------------------------------------------------------------
   cx = gbtnDefRectl.xRight * BUTTONS_PER_ROW;
   cy = gbtnDefRectl.yTop * BUTTONS_PER_ROW;

// --------------------------------------------------------------------------
// If the client width is too small to display all buttons, then compute
// button size and location based on the minimum button width.
// --------------------------------------------------------------------------
   if (cx > rectlClient.xRight) {
      cx = gbtnDefRectl.xRight;
   } else {

// --------------------------------------------------------------------------
// Otherwise, enlarge the button so that a row of buttons (approx) match the
// width of the client area.
// --------------------------------------------------------------------------
      cx = rectlClient.xRight / BUTTONS_PER_ROW;
   } /* endif */

// --------------------------------------------------------------------------
// If the client height is too small to display all buttons, then compute
// button size and location based on the minimum button height.
// --------------------------------------------------------------------------
   if (cy > rectlClient.yTop) {
      cy = gbtnDefRectl.yTop;
   } else {

// --------------------------------------------------------------------------
// Otherwise, enlarge the button so that a column of buttons (approx) match
// the height of the client area.
// --------------------------------------------------------------------------
      cy = rectlClient.yTop / BUTTONS_PER_ROW;
   } /* endif */

// --------------------------------------------------------------------------
// Now that we have the button width and height, compute the position based
// on the row and column where the button is displayed.
// --------------------------------------------------------------------------
   x = cx * column;
   y = (cy * BUTTONS_PER_ROW) - (cy * row);

// --------------------------------------------------------------------------
// Size/Move the button
// --------------------------------------------------------------------------
   WinSetWindowPos( gbtnhwnd[lBtnIndex], NULLHANDLE,
                    x, y, cx, cy, SWP_SIZE | SWP_MOVE );

// --------------------------------------------------------------------------
// Do more buttons if necessary
// --------------------------------------------------------------------------
   if (lBtnIndex < NUM_BUTTONS ) {
      WinPostMsg( hwnd, UM_SIZEBTN, MPFROMLONG(lBtnIndex+1), MPFROMLONG(0L));
   } /* endif */

   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wmCreate
//
// FUNCTION USE: Worker function to process the WM_CREATE message.
//
// DESCRIPTION:  Handles initialization of the client window.  In this
//               case, we seed a random number generator, create the
//               graphic buttons for the application, and initialize the
//               prize locations, etc. for the game.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   pointer to name of file to open
//               MPARAM   reserved
//
// RETURNS:      MRESULT  TRUE, error occurred, abort.  FALSE, continue
//                        window creation.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HPS         hps;
   int         i;
   ULONG       seed;

// --------------------------------------------------------------------------
// Get the current millisecond count to seed the random number generator.
// Probably not the best seed, but this will give us some variation in
// the startup of the game.
// --------------------------------------------------------------------------
   DosQuerySysInfo( QSV_MS_COUNT, QSV_MS_COUNT, &seed, sizeof(seed));
   srand(seed);

// --------------------------------------------------------------------------
// Create the graphic buttons for the application.  Each button is created
// with animation enabled and automatic state transitions.  The control data
// is predefined in an array.
// --------------------------------------------------------------------------
   for( i=0; i<NUM_BUTTONS; i++ ) {
      gbtnhwnd[i] = WinCreateWindow( hwnd,
                    WC_GRAPHICBUTTON, NULL,
                    GBS_ANIMATION | GBS_AUTOTWOSTATE | GBS_DISABLEBITMAP,
                    1, 1, 1, 1, hwnd, HWND_TOP, GBTNID + i,
                    &gbtndata[i], NULL );
   } /* endfor */

// --------------------------------------------------------------------------
// Load the background bitmap.
// --------------------------------------------------------------------------
   hps = WinGetPS( hwnd );
   hbm = GpiLoadBitmap( hps, NULLHANDLE, 1000, 0, 0 );
   WinReleasePS( hps );

// --------------------------------------------------------------------------
// Initialize the game parameters.
// --------------------------------------------------------------------------
   NewGame( hwnd, MPFROMSHORT(2), MPFROMLONG(0L));

// --------------------------------------------------------------------------
// Return FALSE to continue window creation.
// --------------------------------------------------------------------------
   return MRFROMLONG(FALSE);
}

// *******************************************************************************
// FUNCTION:     wmCommand
//
// FUNCTION USE: Worker function to process the WM_COMMAND message.
//
// DESCRIPTION:  Handles menu item selections for the application.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   pointer to name of file to open
//               MPARAM   reserved
//
// RETURNS:      MRESULT  reserved value of 0.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wmCommand( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{

// --------------------------------------------------------------------------
// The only command for this application is to start a new game, so call
// the function to initialize a new game.
// --------------------------------------------------------------------------
   switch( SHORT1FROMMP(mp1)) 
    {
     case IDM_START:           
          return NewGame(hwnd, mp1, mp2); 

     case IDM_PRODINFO:
          WinDlgBox (HWND_DESKTOP, hwnd, ProdInfoDlgProc, NULLHANDLE, ID_PRODINFO, NULL);
          return FALSE;
    }

// --------------------------------------------------------------------------
// return the reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(FALSE);
}

// *******************************************************************************
// FUNCTION:     wmControl
//
// FUNCTION USE: Worker function to process the WM_CONTROL message.
//
// DESCRIPTION:  Handles notifications from owned windows, in this case the
//               graphic buttons.  Whenever a button transitions to the
//               down state, we determine if a match has occurred.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   control id in low order word, notification code
//                        in high order word.
//               MPARAM   notification data
//
// RETURNS:      MRESULT  reserved value of 0.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wmControl( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   USHORT   btnID;
   USHORT   prize = ~WILD_PRIZE;

// --------------------------------------------------------------------------
// don't do anything unless the button is transitioning to the down state
// --------------------------------------------------------------------------
   if (SHORT2FROMMP(mp1) == GBN_BUTTONDOWN) {

// --------------------------------------------------------------------------
// only check for a match if we not currently displaying two prizes
// --------------------------------------------------------------------------
      if (!fTimerWait) {

         if (gbtnDownID != 0xffff) {
// --------------------------------------------------------------------------
// This is the second button.  Compute the button id and animate the button.
// --------------------------------------------------------------------------
            gbtnDownID1 = SHORT1FROMMP(mp1) - GBTNID;
            WinEnableWindow( gbtnhwnd[gbtnDownID1], FALSE );
            WinSendMsg( gbtnhwnd[gbtnDownID1], GBM_ANIMATE,
                        MPFROMSHORT(TRUE), MPFROMSHORT(FALSE));

// --------------------------------------------------------------------------
// Count one attempt
// --------------------------------------------------------------------------
            gTries++;

// --------------------------------------------------------------------------
// do we have a wild card -- if so, determine if the button just pushed or
// the first button was the wild card.  Then find the prize for the non-wild
// button and get the matching prize button.  All three will count as
// matched.
// --------------------------------------------------------------------------
            gbtnDownID2 = 0xffff;
            if (btnPrize[gbtnDownID1] == WILD_PRIZE) {
               prize = btnPrize[gbtnDownID];
               gbtnDownID2 = (prizes[prize].btnNumber[0] == gbtnDownID) ?
                     prizes[prize].btnNumber[1] : prizes[prize].btnNumber[0];
            } /* endif */
            if (btnPrize[gbtnDownID] == WILD_PRIZE) {
               prize = btnPrize[gbtnDownID1];
               gbtnDownID2 = (prizes[prize].btnNumber[0] == gbtnDownID1) ?
                     prizes[prize].btnNumber[1] : prizes[prize].btnNumber[0];
            } /* endif */

// --------------------------------------------------------------------------
// If we had a wild card - "turn over" the third button
// --------------------------------------------------------------------------
            if (gbtnDownID2 != 0xffff) {
               WinSendMsg( gbtnhwnd[gbtnDownID2], GBM_SETSTATE,
                           MPFROMSHORT(GB_UP), MPFROMSHORT(TRUE));
               WinSendMsg( gbtnhwnd[gbtnDownID2], GBM_ANIMATE,
                           MPFROMSHORT(TRUE), MPFROMSHORT(FALSE));
               prize = WILD_PRIZE;
            } /* endif */

// --------------------------------------------------------------------------
// If the first and second buttons match or we have a wild card match,
// start a timer which will indicate when to remove the buttons.
// --------------------------------------------------------------------------
            if (btnPrize[gbtnDownID1] == btnPrize[gbtnDownID] ||
                prize == WILD_PRIZE) {
               WinStartTimer( WinQueryAnchorBlock(hwnd), hwnd,
                              ID_TIMERMATCH, 1000L );
            } else {

// --------------------------------------------------------------------------
// Otherwise, start a timer to tell us when to hide the prizes again.
// --------------------------------------------------------------------------
               WinStartTimer( WinQueryAnchorBlock(hwnd), hwnd,
                              ID_TIMERNOMATCH, 1000L );
            } /* endif */

// --------------------------------------------------------------------------
// Indicate that we are waiting on the timer - this will prevent additional
// prizes from being revealed.
// --------------------------------------------------------------------------
            fTimerWait = TRUE;
         } else {

// --------------------------------------------------------------------------
// Got the first button - disable it so it can't be clicked again and start
// the animation of the prize.
// --------------------------------------------------------------------------
            gbtnDownID = SHORT1FROMMP(mp1) - GBTNID;
            WinEnableWindow( gbtnhwnd[gbtnDownID], FALSE );
            WinSendMsg( gbtnhwnd[gbtnDownID], GBM_ANIMATE,
                        MPFROMSHORT(TRUE), MPFROMSHORT(FALSE));
         } /* endif */
      } else {

// --------------------------------------------------------------------------
// we already have two buttons.  Set this third one back to the up state.
// --------------------------------------------------------------------------
         btnID = SHORT1FROMMP(mp1) - GBTNID;
         WinSendMsg( gbtnhwnd[btnID], GBM_SETSTATE,
                     MPFROMSHORT( GB_UP ), MPFROMSHORT(TRUE));
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// return the reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wmPaint
//
// FUNCTION USE: Worker function to process the WM_PAINT message.
//
// DESCRIPTION:  Paints the client area of the main application window
//               with a specified bitmap
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   reserved
//               MPARAM   reserved
//
// RETURNS:      MRESULT  reserved value of 0.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wmPaint( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HPS         hps;
   RECTL    rectl;

// --------------------------------------------------------------------------
// Start the paint operation and get the presentation space for drawing.
// --------------------------------------------------------------------------
   hps = WinBeginPaint( hwnd, NULLHANDLE, &rectl );
   if (hps != NULLHANDLE ) {

// --------------------------------------------------------------------------
// Get the size of the client window
// --------------------------------------------------------------------------
      WinQueryWindowRect( hwnd, &rectl );

// --------------------------------------------------------------------------
// Draw the bitmap, stretching or compressing to fill the client area.
// --------------------------------------------------------------------------
      WinDrawBitmap( hps, hbm, NULL, (PPOINTL)&rectl, 0, 0, DBM_STRETCH );

// --------------------------------------------------------------------------
// Terminate the paint operation and release the presentation space
// --------------------------------------------------------------------------
      WinEndPaint( hps );
   } /* endif */

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wmSize
//
// FUNCTION USE: Worker function to process the WM_SIZE message.
//
// DESCRIPTION:  Starts the process of updating the size and placement of the
//               graphic buttons by posting the UM_SIZEBTN message.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   reserved
//               MPARAM   reserved
//
// RETURNS:      MRESULT  reserved value of 0.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wmSize( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   WinPostMsg( hwnd, UM_SIZEBTN, MPFROMLONG(0L), MPFROMLONG(0L));
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wmTimer
//
// FUNCTION USE: Worker function to process the WM_TIMER message.
//
// DESCRIPTION:  Handles processing to finish a match attempt by hiding
//               matched buttons or setting non-matched buttons back to
//               the upstate.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   timer identity
//               MPARAM   reserved
//
// RETURNS:      MRESULT  reserved value of 0.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wmTimer( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   char     szMessage[ 80 ];

   if (LONGFROMMP(mp1) == ID_TIMERMATCH ) {

// --------------------------------------------------------------------------
// Processing a timer for a match - enable the windows participating in the
// match.
// --------------------------------------------------------------------------
      WinEnableWindow( gbtnhwnd[gbtnDownID], TRUE );
      WinEnableWindow( gbtnhwnd[gbtnDownID1], TRUE );

// --------------------------------------------------------------------------
// Now hide the first two buttons to reveal the background
// --------------------------------------------------------------------------
      WinShowWindow( gbtnhwnd[gbtnDownID], FALSE );
      WinShowWindow( gbtnhwnd[gbtnDownID1], FALSE );

// --------------------------------------------------------------------------
// Now count these two buttons as matched.
// --------------------------------------------------------------------------
      gMatched -= 2;

// --------------------------------------------------------------------------
// hide the third button for wildcard matches.
// --------------------------------------------------------------------------
      if( gbtnDownID2 != 0xffff) {
         WinShowWindow( gbtnhwnd[gbtnDownID2], FALSE );
         gMatched--;
      }

// --------------------------------------------------------------------------
// clear the first button id so we know a new attempt is starting and stop
// the timer - we only need one hit.
// --------------------------------------------------------------------------
      gbtnDownID = 0xffff;
      WinStopTimer( WinQueryAnchorBlock(hwnd), hwnd, ID_TIMERMATCH );

// --------------------------------------------------------------------------
// if all buttons, or all but one wild card is matched - tell'em
// the games over.
// --------------------------------------------------------------------------
      if (gMatched < 2 ) {
         sprintf( szMessage, "Congratulations, you found all the matches "
                  "in only %d attempts", gTries );
         WinMessageBox( HWND_DESKTOP, hwnd, szMessage,
                        "MATCH.EXE - A WINNER!!!!!", 1, MB_OK );
      }

   } else if (LONGFROMMP(mp1) == ID_TIMERNOMATCH) {

// --------------------------------------------------------------------------
// no match occurred - stop the animation and return the button to the up
// state, reenabling the windows.
// --------------------------------------------------------------------------
      WinSendMsg( gbtnhwnd[gbtnDownID1], GBM_ANIMATE,
                  MPFROMSHORT( FALSE), MPFROMSHORT(FALSE));
      WinSendMsg( gbtnhwnd[gbtnDownID], GBM_ANIMATE,
                  MPFROMSHORT( FALSE), MPFROMSHORT(FALSE));
      WinSendMsg( gbtnhwnd[gbtnDownID1], GBM_SETSTATE,
                  MPFROMSHORT( GB_UP ), MPFROMSHORT(TRUE));
      WinSendMsg( gbtnhwnd[gbtnDownID], GBM_SETSTATE,
                  MPFROMSHORT( GB_UP ), MPFROMSHORT(TRUE));
      WinEnableWindow( gbtnhwnd[gbtnDownID], TRUE );
      WinEnableWindow( gbtnhwnd[gbtnDownID1], TRUE );

// --------------------------------------------------------------------------
// setup to start a new attempt
// --------------------------------------------------------------------------
      gbtnDownID = 0xffff;

// --------------------------------------------------------------------------
// stop the timer
// --------------------------------------------------------------------------
      WinStopTimer( WinQueryAnchorBlock(hwnd), hwnd, ID_TIMERNOMATCH );

   } else {

// --------------------------------------------------------------------------
// route other timer message to the default window procedure.
// --------------------------------------------------------------------------
      return WinDefWindowProc( hwnd, WM_TIMER, mp1, mp2 );
   } /* endif */

// --------------------------------------------------------------------------
// not waiting on a timer message anymore
// --------------------------------------------------------------------------
   fTimerWait = FALSE;

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     NewGame
//
// FUNCTION USE: Initialize data structures for a new game
//
// DESCRIPTION:  Randomizes the assignment of prizes to buttons, displays
//               the buttons and establishes the global counters.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   reserved
//               MPARAM   reserved
//
// RETURNS:      MRESULT  reserved value of 0.
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT NewGame( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   int   i, j, prize, btn;

// --------------------------------------------------------------------------
// clear the prize to button array
// --------------------------------------------------------------------------
   for( i=0; i < 12; i++ ) {
      prizes[i].btnNumber[0] = prizes[i].btnNumber[1] = 0xffff;
   }

// --------------------------------------------------------------------------
// clear the button to prize array
// --------------------------------------------------------------------------
   for( i=0; i < 25; i++ ) btnPrize[i] = 0xffff;

// --------------------------------------------------------------------------
// assign prizes to 24 of the buttons
// --------------------------------------------------------------------------
   for( i=0; i < 24; i++ ) {

// --------------------------------------------------------------------------
// randomly test buttons to find a button with no prize assigned
// --------------------------------------------------------------------------
      for(btn = rand() % 25; btnPrize[btn] != 0xffff; btn = rand() % 25) {
         if (btnPrize[btn] == 0xffff) {
            break;
         } /* endif */
      } /* endfor */

// --------------------------------------------------------------------------
// randomly select a prize which does not have both buttons assigned
// --------------------------------------------------------------------------
      prize = rand() % 12;
      while( prizes[prize].btnNumber[0] != 0xffff &&
             prizes[prize].btnNumber[1] != 0xffff ) {
         prize = rand() % 12;
      }

// --------------------------------------------------------------------------
// assign the prize to the selected button and the button to one of the two
// buttons for the selected prize
// --------------------------------------------------------------------------
      btnPrize[btn] = prize;
      if (prizes[prize].btnNumber[0] == 0xffff) {
         prizes[prize].btnNumber[0] = btn;
      } else {
         prizes[prize].btnNumber[1] = btn;
      } /* endif */

// --------------------------------------------------------------------------
// copy the bitmaps for the prize to the global button array
// --------------------------------------------------------------------------
      for( j=0; j < MAX_BITMAPS - 1; j++ ) {
         gbtndata[btn].bitmaps[j] = prizes[prize].bitmaps[j];
      }
      gbtndata[btn].btndata.cBitmaps = prizes[prize].cBitmap + 1;

// --------------------------------------------------------------------------
// send the new bitmap data to the button, set the button in the up state,
// set the indexes for the button states, and the animation rate.
// --------------------------------------------------------------------------
      WinSendMsg( gbtnhwnd[btn], GBM_SETGRAPHICDATA,
                  MPFROMP(&gbtndata[btn]), MPFROMLONG(0L));
      WinSendMsg( gbtnhwnd[btn], GBM_SETSTATE,
                  MPFROMLONG( GB_UP ), MPFROMLONG( TRUE ));
      WinSendMsg( gbtnhwnd[btn], GBM_SETBITMAPINDEX,
                  MPFROMSHORT(GB_UP), MPFROMSHORT(0));
      WinSendMsg( gbtnhwnd[btn], GBM_SETBITMAPINDEX,
                  MPFROMSHORT(GB_DISABLE), MPFROMSHORT(1));
      WinSendMsg( gbtnhwnd[btn], GBM_SETBITMAPINDEX,
                  MPFROMSHORT(GB_ANIMATIONBEGIN), MPFROMSHORT(1));
      WinSendMsg( gbtnhwnd[btn], GBM_SETANIMATIONRATE,
                  MPFROMLONG(200), MPFROMLONG(0L));

// --------------------------------------------------------------------------
// show the button window
// --------------------------------------------------------------------------
      WinShowWindow( gbtnhwnd[btn], TRUE );
   } /* endfor */

// --------------------------------------------------------------------------
// hide the button with no prize assigned.
// --------------------------------------------------------------------------
   for( i=0; i<25; i++ ) {
      if (btnPrize[i] == 0xffff) {
         WinShowWindow( gbtnhwnd[i], FALSE );
         break;
      } /* endif */
   } /* endfor */

// --------------------------------------------------------------------------
// set the number of buttons to be matched and clear the attempt counter.
// --------------------------------------------------------------------------
   gMatched = 24;
   gTries = 0;

// --------------------------------------------------------------------------
// return the appropriate reserved value.
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}


// *******************************************************************************
// FUNCTION:     ProdInfoDlgProc
//
// FUNCTION USE: Dialog procedure used to display product information dialog
//
// DESCRIPTION:  This dialog procedure is used for the product information
//               dialog box.
//
// PARAMETERS:   HWND     Window handle
//               ULONG    Message to be processed
//               MP1      First message parameter mp1
//               MP2      Second message parameter mp2
//
// RETURNS:      MRESULT
//
// INTERNALS:    NONE
//
// HISTORY:      mm-dd-yy
//
// *******************************************************************************
MRESULT EXPENTRY ProdInfoDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  switch (msg)
   {
    case WM_INITDLG:
         CenterDialog(hwnd);
         SetTheSysMenu(hwnd, NULL);
         break;

    case WM_COMMAND:
         switch (COMMANDMSG(&msg)->cmd)
          {
           case DID_OK:
           case DID_CANCEL:
                WinDismissDlg(hwnd, TRUE);
                return FALSE;
          }
         break ;
   }
 return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
}
