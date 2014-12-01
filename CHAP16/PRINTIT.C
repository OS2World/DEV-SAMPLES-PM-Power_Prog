// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      PRINTIT.C                                        mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Main Source File for PRINTIT/Chapter 16 Sample Program        บ
// บ                                                                               บ
// บ  NOTES:         This program demonstrates printing and font management.       บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#define INCL_WINERRORS
#define INCL_DOSERRORS
#define INCL_GPI
#define INCL_WINSTDFONT
#define INCL_WINSTDFILE
#define INCL_WINWINDOWMGR
#define INCL_WINSCROLLBARS
#define INCL_WINFRAMECTLS
#define INCL_WINFRAMEMGR
#define INCL_DEV

#include "printit.h"
#include "shcommon.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// --------------------------------------------------------------------------
// Miscellaneous constant definitions used only in this source module.
// --------------------------------------------------------------------------
#define  TITLE_BAR_TEXT       "PRINTIT - Sample File Print Utility"
#define  MAX_GPI_STRING_LENGTH     512
#define  LINE_SCROLL                50

// --------------------------------------------------------------------------
// Define variables used to prevent warnings for loop constants
// --------------------------------------------------------------------------
static   BOOL  false = FALSE;

// --------------------------------------------------------------------------
// define class names
// --------------------------------------------------------------------------
static   char  szClassName[ ] = "PRINTIT";

// --------------------------------------------------------------------------
// define global variables used for printing
// --------------------------------------------------------------------------
FIXED       gfxPointSize;
FATTRS      gfattrs;
PPRQINFO3   gpQueueBuf = NULL;
ULONG       gnQueue;
ULONG       gcQueues;
HDC         ghdcInfo;
HPS         ghpsInfo;
LONG        gnForm;
LONG        gcForms;
PHCINFO     gpFormsBuf = NULL;
HPS         ghpsDisplay;
PSZ         gpszCurFile = NULL;
RECTL       grectlMargins = { 0, 0, 0, 0 };
SIZEL       gsizelWin;

// --------------------------------------------------------------------------
// Client Window Procedures
// --------------------------------------------------------------------------
MRESULT APIENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

// -------------------------------------------------------------------------- 
// Product Information Dialog Procedure                                  
// -------------------------------------------------------------------------- 
MRESULT APIENTRY ProdInfoDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );  

// --------------------------------------------------------------------------
// Message processing procedures
// --------------------------------------------------------------------------
MRESULT wm_command( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_create( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_destroy( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_hscroll( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_paint( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_size( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT wm_vscroll( HWND hwnd, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------
// command processing procedures
// --------------------------------------------------------------------------
MRESULT mid_print( HWND hwnd );
MRESULT mid_font( HWND hwnd );
MRESULT mid_queue( HWND hwnd );
MRESULT mid_open( HWND hwnd );
MRESULT mid_margins( HWND hwnd );

// --------------------------------------------------------------------------
// Miscellaneous procedures
// --------------------------------------------------------------------------
BOOL QueryQueueData(void);
HDC  OpenDeviceContext( HAB hab, ULONG ulDCType, PSZ pszComment );
BOOL QueryFormsData(HDC hdc);
ULONG PrintString( HPS hps, PPOINTL pptl, ULONG cbText,
                  PSZ pszText, PRECTL rectlPage, HPS hpsInfo );
void CalcScrollData( HWND hwnd );
void DoPrint( PVOID arg );

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
// Now create the application's main frame window
// --------------------------------------------------------------------------
      flStyle &= ~(FCF_ACCELTABLE);
      flStyle |= FCF_VERTSCROLL | FCF_HORZSCROLL;
      hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                      0L,
                                      &flStyle,
                                      szClassName,
                                      TITLE_BAR_TEXT,
                                      WS_VISIBLE,
                                      NULLHANDLE,
                                      RID_APPWND,
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
   case WM_COMMAND:           
        return wm_command(hwnd, mp1, mp2);            

   case WM_CREATE:            
        return wm_create(hwnd, mp1, mp2);          

   case WM_DESTROY:           
        return wm_destroy(hwnd, mp1, mp2);               
                                                           
   case WM_HSCROLL:           
        return wm_hscroll(hwnd, mp1, mp2);                      

   case WM_PAINT:            
        return wm_paint(hwnd, mp1, mp2);              

   case WM_SIZE:       
        return wm_size(hwnd, mp1, mp2);               

   case WM_VSCROLL:          
        return wm_vscroll(hwnd, mp1, mp2);                      

   default:                   
        return WinDefWindowProc( hwnd, msg, mp1, mp2 );                  
  } /* endswitch */
}

// *******************************************************************************
// FUNCTION:     wm_command
//
// FUNCTION USE: Process WM_COMMAND messages for the application client window
//
// DESCRIPTION:  Dispatch to the appropriate command handling routine.
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
   switch(SHORT1FROMMP(mp1)) 
    {
     case MID_PRINT:              
          return mid_print(hwnd);
    
     case MID_FONT:                
          return mid_font(hwnd);  
    
     case MID_QUEUE:              
          return mid_queue(hwnd);
    
     case MID_OPEN:                
          return mid_open(hwnd);      
    
     case MID_MARGINS:            
          return mid_margins(hwnd);           
    
     case MID_HELPGEN:     
     case MID_HELPKEYS:    
          DisplayMessages(NULLHANDLE, "Help text would normally go here, but there is none.", MSG_INFO);                       
          return FALSE;

     case MID_PRODINFO:            
          WinDlgBox (HWND_DESKTOP, hwnd, ProdInfoDlgProc, NULLHANDLE, MID_PRODINFO, NULL);     

     default:                     
          return MRFROMLONG(0L);  
   } /* endswitch */
}

// *******************************************************************************
// FUNCTION:     wm_create
//
// FUNCTION USE: Process WM_CREATE messages for the application client window
//
// DESCRIPTION:  Handle application initialization
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
MRESULT wm_create( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   MRESULT  rc = MRFROMLONG(TRUE);
   STR8     str8Name;
   SIZEL    sizel = { 0, 0 };
   SIZEF    sizef;

// --------------------------------------------------------------------------
// Get a list of all the print queues
// --------------------------------------------------------------------------
   if (QueryQueueData()) {

// --------------------------------------------------------------------------
// Open an information display context for the default queue
// --------------------------------------------------------------------------
      ghdcInfo = OpenDeviceContext( WinQueryAnchorBlock(hwnd),
                                    OD_INFO, NULL );
      if (ghdcInfo != DEV_ERROR) {

// --------------------------------------------------------------------------
// Initialize the forms data for the default device
// --------------------------------------------------------------------------
      QueryFormsData( ghdcInfo );

// --------------------------------------------------------------------------
// Get a presentation space for the queue so we can get the default font
// --------------------------------------------------------------------------
         ghpsInfo = GpiCreatePS( WinQueryAnchorBlock(hwnd),
                                 ghdcInfo, &sizel, PU_LOMETRIC | GPIA_ASSOC );
         if (ghpsInfo != NULLHANDLE) {

// --------------------------------------------------------------------------
// set the global fattrs structure for the default font
// --------------------------------------------------------------------------
            GpiQueryLogicalFont( ghpsInfo, 0, &str8Name,
                                 &gfattrs, sizeof(FATTRS));
            if (gfattrs.fsFontUse & FATTR_FONTUSE_OUTLINE ) {

// --------------------------------------------------------------------------
// make sure the point size is set for outline fonts
// --------------------------------------------------------------------------
               GpiQueryCharBox( ghpsInfo, &sizef );
               gfxPointSize = (sizef.cy / 254) * 72;
               gfxPointSize += 16384; // round to nearest .5 pt
               gfxPointSize &= 0xffff8000;
            } /* endif */

// --------------------------------------------------------------------------
// establish a permanent device context and PS for the display
// --------------------------------------------------------------------------
            WinOpenWindowDC( hwnd );
            ghpsDisplay = GpiCreatePS( WinQueryAnchorBlock(hwnd),
                                       WinQueryWindowDC(hwnd), &sizel,
                                       PU_LOMETRIC | GPIA_ASSOC );


// --------------------------------------------------------------------------
// indicate that window creation can continue
// --------------------------------------------------------------------------
            rc = MRFROMLONG(FALSE);
         } /* endif */
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// return with flag which indicates if creation can continue
// --------------------------------------------------------------------------
   return rc;
}

// *******************************************************************************
// FUNCTION:     wm_destroy
//
// FUNCTION USE: Process WM_DESTROY messages for the application client window
//
// DESCRIPTION:  Handle application cleanup
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
// --------------------------------------------------------------------------
// Cleanup the global display presentation space
// --------------------------------------------------------------------------
   if (ghpsDisplay != NULLHANDLE) {
      GpiAssociate( ghpsDisplay, NULLHANDLE );
      GpiDestroyPS( ghpsDisplay );
   } /* endif */

// --------------------------------------------------------------------------
// Cleanup the global information context presentation space
// --------------------------------------------------------------------------
   if (ghpsInfo != NULLHANDLE) {
      GpiAssociate( ghpsInfo, NULLHANDLE );
      GpiDestroyPS( ghpsInfo );
   } /* endif */

// --------------------------------------------------------------------------
// Cleanup the global information context
// --------------------------------------------------------------------------
   if (ghdcInfo != NULLHANDLE) {
      DevCloseDC( ghdcInfo );
   } /* endif */

// --------------------------------------------------------------------------
// Free up the allocated global buffers
// --------------------------------------------------------------------------
   if (gpFormsBuf != NULL) {
      free( gpFormsBuf );
   } /* endif */
   if (gpQueueBuf != NULL) {
      free( gpQueueBuf );
   } /* endif */

// --------------------------------------------------------------------------
// return the reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0l);
}

// *******************************************************************************
// FUNCTION:     wm_hscroll
//
// FUNCTION USE: Process WM_HSCROLL messages for the application client window
//
// DESCRIPTION:  Adjust position of scroll bars
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
MRESULT wm_hscroll( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND     hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
   HWND     hwndScroll = WinWindowFromID( hwndFrame, SHORT1FROMMP(mp1));
   USHORT   usPos;
   USHORT   usRange;

// --------------------------------------------------------------------------
// Get the scroll bar range and current position
// --------------------------------------------------------------------------
   usPos = SHORT1FROMMR( WinSendMsg( hwndScroll, SBM_QUERYPOS,
                                     MPFROMLONG(0L), MPFROMLONG(0L)));
   usRange = SHORT2FROMMR( WinSendMsg( hwndScroll, SBM_QUERYRANGE,
                                     MPFROMLONG(0L), MPFROMLONG(0L)));

   switch( SHORT2FROMMP(mp2)) {
   case SB_LINELEFT:
// --------------------------------------------------------------------------
// Incremental shift left requested - reduce the current position by the
// defined amount, but not less than zero (since this is unsigned).
// --------------------------------------------------------------------------
      if( usPos > LINE_SCROLL ) {
         usPos -= LINE_SCROLL;
      } else {
         usPos = 0;
      } /* endif */
      break;

   case SB_LINERIGHT:
// --------------------------------------------------------------------------
// Incremental shift right requested - increase the current position by the
// defined amount, but not beyond the range of the slider.
// --------------------------------------------------------------------------
      if( usPos < usRange - LINE_SCROLL ) {
         usPos += LINE_SCROLL;
      } else {
         usPos = usRange;
      } /* endif */
      break;

   case SB_PAGELEFT:
// --------------------------------------------------------------------------
// Page shift left requested - reduce the current position by the current
// window size, but not less than zero (since this is unsigned).
// --------------------------------------------------------------------------
      if( usPos > gsizelWin.cx ) {
         usPos -= gsizelWin.cx;
      } else {
         usPos = 0;
      } /* endif */
      break;

   case SB_PAGERIGHT:
// --------------------------------------------------------------------------
// Page shift right requested - increase the current position by the
// defined amount, but not beyond the range of the slider.
// --------------------------------------------------------------------------
      if( usPos < usRange - gsizelWin.cx ) {
         usPos += gsizelWin.cx;
      } else {
         usPos = usRange;
      } /* endif */
      break;

   case SB_SLIDERPOSITION:
// --------------------------------------------------------------------------
// Absolute positioning requested - set position to the requested position
// --------------------------------------------------------------------------
      if( SHORT1FROMMP(mp2) ) {
         usPos = SHORT1FROMMP(mp2);
      } /* endif */
      break;

   default:
      return MRFROMLONG(0L);
   } /* endswitch */

// --------------------------------------------------------------------------
// Adjust the scroll bar position
// --------------------------------------------------------------------------
   WinSendMsg( hwndScroll, SBM_SETPOS, MPFROMSHORT(usPos),
                                       MPFROMLONG(0L));

// --------------------------------------------------------------------------
// redraw the window to show the new position
// --------------------------------------------------------------------------
   WinInvalidateRect( hwnd, NULL, TRUE );

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}


// *******************************************************************************
// FUNCTION:     wm_paint
//
// FUNCTION USE: Process WM_PAINT messages for the application client window
//
// DESCRIPTION:  Erase the window area
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
   HPS         hps = NULLHANDLE;
   RECTL       rectl;
   RECTL       rectlPage;
   RECTL       rectlWin;
   SIZEF       sizef;
   SIZEL       sizel = {0,0};
   POINTL      ptl;
   PAPPFILE    pFile;
   PSZ         pBuffer;
   size_t      cbBuffer;
   POINTL      ptlXform;
   MATRIXLF    pmat;
   ULONG       cbOut;
   HWND        hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
   HWND        hwndVScroll = WinWindowFromID( hwndFrame, FID_VERTSCROLL );
   HWND        hwndHScroll = WinWindowFromID( hwndFrame, FID_HORZSCROLL );

// --------------------------------------------------------------------------
// Start a job.  Without the job - we can't select a font into the
// presentation space.
// --------------------------------------------------------------------------
   DevEscape( ghdcInfo, DEVESC_STARTDOC, strlen( TITLE_BAR_TEXT ),
              TITLE_BAR_TEXT, 0, NULL );

// --------------------------------------------------------------------------
// Initialize the repaint operation and obtain a presentation space handle.
// --------------------------------------------------------------------------
   hps = WinBeginPaint( hwnd, ghpsDisplay, &rectl );
   if (hps != NULLHANDLE) {

// --------------------------------------------------------------------------
// Fill in the background
// --------------------------------------------------------------------------
      WinFillRect( hps, &rectl, CLR_WHITE );
      if (gpszCurFile != NULL) {

// --------------------------------------------------------------------------
// Initially set the drawing or page rectangle to the page size less the
// margins
// --------------------------------------------------------------------------
         rectlPage.xLeft = grectlMargins.xLeft;
         rectlPage.yBottom = grectlMargins.yBottom;
         rectlPage.xRight = (gpFormsBuf[gnForm].cx * 10) - grectlMargins.xRight;
         rectlPage.yTop = (gpFormsBuf[gnForm].cy * 10) - grectlMargins.yBottom;

// --------------------------------------------------------------------------
// If any of these are not within the clip boundary of the printer - adjust
// --------------------------------------------------------------------------
         if( rectlPage.xLeft < gpFormsBuf[gnForm].xLeftClip * 10)
            rectlPage.xLeft = gpFormsBuf[gnForm].xLeftClip * 10;
         if( rectlPage.yBottom < gpFormsBuf[gnForm].yBottomClip * 10)
            rectlPage.yBottom = gpFormsBuf[gnForm].yBottomClip * 10;
         if( rectlPage.xRight > gpFormsBuf[gnForm].xRightClip * 10)
            rectlPage.xRight = gpFormsBuf[gnForm].xRightClip * 10;
         if( rectlPage.yTop > gpFormsBuf[gnForm].yTopClip * 10)
            rectlPage.yTop = gpFormsBuf[gnForm].yTopClip * 10;

// --------------------------------------------------------------------------
// Save off the page rectangle and then get the window rectangle and
// convert to presentation space units.
// --------------------------------------------------------------------------
         rectl = rectlPage;
         WinQueryWindowRect( hwnd, &rectlWin );
         GpiConvert( hps, CVTC_DEVICE, CVTC_WORLD, 2, (PPOINTL)&rectlWin );

// --------------------------------------------------------------------------
// Determine translation needed to display current portion of the file
// in the window.
// --------------------------------------------------------------------------
         ptlXform.y = ptlXform.x = 0;
         if( rectl.yTop > rectlWin.yTop ) {

// --------------------------------------------------------------------------
// We want the top when the vertical scroll bar is 0, so set a negative
// translation based on how near the bottom the scroll bar is positioned.
// --------------------------------------------------------------------------
            ptlXform.y = (LONG)WinSendMsg( hwndVScroll, SBM_QUERYPOS, 0, 0) -
               SHORT2FROMMR(WinSendMsg( hwndVScroll, SBM_QUERYRANGE, 0, 0 ));
         } /* endif */

// --------------------------------------------------------------------------
// We want the left when the vertical scroll bar is 0, so set a negative
// translation of the scroll bar position
// --------------------------------------------------------------------------
         if( rectl.xRight > rectlWin.xRight ) {
            ptlXform.x = -(LONG)WinSendMsg( hwndHScroll, SBM_QUERYPOS, 0, 0);
         } /* endif */

// --------------------------------------------------------------------------
// Set the default transformation matrix to effect our translation.
// --------------------------------------------------------------------------
         GpiTranslate( hps, &pmat, TRANSFORM_REPLACE, &ptlXform );
         GpiSetDefaultViewMatrix( hps, 9, &pmat, TRANSFORM_REPLACE );


// --------------------------------------------------------------------------
// Setup the requested font in both presentation spaces, as before the
// character box is not really needed for device fonts, but does not affect
// the output.
// --------------------------------------------------------------------------
         GpiCreateLogFont( hps, NULL, 1, &gfattrs );
         GpiCreateLogFont( ghpsInfo, NULL, 2, &gfattrs );
         GpiSetCharSet( hps, 1 );
         GpiSetCharSet( ghpsInfo, 2);
         sizef.cx = (gfxPointSize / 72) * 254;
         sizef.cy = (gfxPointSize / 72) * 254;
         GpiSetCharBox( hps, &sizef );
         GpiSetCharBox( ghpsInfo, &sizef );

// --------------------------------------------------------------------------
// Draw a border around the margins of the print area
// --------------------------------------------------------------------------
         ptl.x = rectlPage.xLeft;
         ptl.y = rectlPage.yBottom;
         GpiMove( hps, &ptl );
         ptl.x = rectlPage.xRight;
         ptl.y = rectlPage.yTop;
         GpiBox( hps, DRO_OUTLINE, &ptl, 0, 0 );
         ptl.x = rectlPage.xLeft;
         ptl.y = rectlPage.yTop;

// --------------------------------------------------------------------------
// Now we're ready to print the data - open the selected file
// --------------------------------------------------------------------------
         pFile = OpenFile( gpszCurFile );
         if (pFile != NULL) {

// --------------------------------------------------------------------------
// Assume we'll print the whole file, but we bail after the frist page
// --------------------------------------------------------------------------
            while (!IsEOF( pFile )) {

// --------------------------------------------------------------------------
// Get a line of text - if it's a blank line simulate a line with a single
// space character.
// --------------------------------------------------------------------------
               cbBuffer = ReadFile( pFile, &pBuffer );
               if (cbBuffer == 0) {
                  cbBuffer = 1;
                  pBuffer = " ";
               } /* endif */

// --------------------------------------------------------------------------
// Call the routine to wrap the text.  If this returns without printing all
// the text, we need to start a new page, which for now means we're done --
// we only display one page on the screen
// --------------------------------------------------------------------------
               cbOut = 0;
               while (cbOut != cbBuffer) {
                  cbOut += PrintString( ghpsInfo, &ptl, cbBuffer, pBuffer,
                                        &rectlPage, hps );
                  if (cbOut != cbBuffer) {
                     break;
                  } /* endif */
               } /* endwhile */
               if (cbOut != cbBuffer) {
                  break;
               } /* endif */
            } /* endwhile */

// --------------------------------------------------------------------------
// Close the file
// --------------------------------------------------------------------------
            CloseFile( pFile );
         } /* endif */

// --------------------------------------------------------------------------
// Reset the transformation matrix so scroll calculation work properly
// --------------------------------------------------------------------------
         ptlXform.x = ptlXform.y = 0;
         GpiTranslate( hps, &pmat, TRANSFORM_REPLACE, &ptlXform );
         GpiSetDefaultViewMatrix( hps, 9, &pmat, TRANSFORM_REPLACE );
      } /* endif */
// --------------------------------------------------------------------------
// Done - release the presentation space handle.
// --------------------------------------------------------------------------
      WinEndPaint( hps );
   } /* endif */

// --------------------------------------------------------------------------
// abort the job
// --------------------------------------------------------------------------
   DevEscape( ghdcInfo, DEVESC_ABORTDOC, 0, NULL, 0, NULL );

// --------------------------------------------------------------------------
// return proper reserved value to the caller
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wm_size
//
// FUNCTION USE: Process WM_SIZE messages for the application client window
//
// DESCRIPTION:  Adjust scroll bars to match the new window size
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
MRESULT wm_size( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
// --------------------------------------------------------------------------
// Adjust scroll bars to handle the new window size
// --------------------------------------------------------------------------
   CalcScrollData( hwnd );

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wm_vscroll
//
// FUNCTION USE: Process WM_VSCROLL messages for the application client window
//
// DESCRIPTION:  Adjust position of scroll bars
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
MRESULT wm_vscroll( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND     hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
   HWND     hwndScroll = WinWindowFromID( hwndFrame, SHORT1FROMMP(mp1));
   USHORT   usPos;
   USHORT   usRange;

// --------------------------------------------------------------------------
// Get the scroll bar range and current position
// --------------------------------------------------------------------------
   usPos = SHORT1FROMMR( WinSendMsg( hwndScroll, SBM_QUERYPOS,
                                     MPFROMLONG(0L), MPFROMLONG(0L)));
   usRange = SHORT2FROMMR( WinSendMsg( hwndScroll, SBM_QUERYRANGE,
                                     MPFROMLONG(0L), MPFROMLONG(0L)));

   switch( SHORT2FROMMP(mp2)) {
   case SB_LINEUP:
// --------------------------------------------------------------------------
// Incremental shift up requested - reduce the current position by the
// defined amount, but not less than zero (since this is unsigned).
// --------------------------------------------------------------------------
      if( usPos > LINE_SCROLL ) {
         usPos -= LINE_SCROLL;
      } else {
         usPos = 0;
      } /* endif */
      break;

   case SB_LINEDOWN:
// --------------------------------------------------------------------------
// Incremental shift down requested - increase the current position by the
// defined amount, but not beyond the range of the slider.
// --------------------------------------------------------------------------
      if( usPos < usRange - LINE_SCROLL ) {
         usPos += LINE_SCROLL;
      } else {
         usPos = usRange;
      } /* endif */
      break;

   case SB_PAGEUP:
// --------------------------------------------------------------------------
// Page shift up requested - reduce the current position by the current
// window size, but not less than zero (since this is unsigned).
// --------------------------------------------------------------------------
      if( usPos > gsizelWin.cy ) {
         usPos -= gsizelWin.cy;
      } else {
         usPos = 0;
      } /* endif */
      break;

   case SB_PAGEDOWN:
// --------------------------------------------------------------------------
// Page shift down requested - increase the current position by the
// defined amount, but not beyond the range of the slider.
// --------------------------------------------------------------------------
      if( usPos < usRange - gsizelWin.cy ) {
         usPos += gsizelWin.cy;
      } else {
         usPos = usRange;
      } /* endif */
      break;

   case SB_SLIDERPOSITION:
// --------------------------------------------------------------------------
// Absolute positioning requested - set position to the requested position
// --------------------------------------------------------------------------
      if( SHORT1FROMMP(mp2) ) {
         usPos = SHORT1FROMMP(mp2);
      } /* endif */
      break;

   default:
      return MRFROMLONG(0L);
   } /* endswitch */

// --------------------------------------------------------------------------
// Adjust the scroll bar position
// --------------------------------------------------------------------------
   WinSendMsg( hwndScroll, SBM_SETPOS, MPFROMSHORT(usPos),
                                       MPFROMLONG(0L));

// --------------------------------------------------------------------------
// redraw the window to show the new position
// --------------------------------------------------------------------------
   WinInvalidateRect( hwnd, NULL, TRUE );

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     mid_print
//
// FUNCTION USE: Process the FILE | PRINT menu item.
//
// DESCRIPTION:  Start thread to send output to the printer
//
// PARAMETERS:   HWND     client window handle
//
// RETURNS:      MRESULT  Reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT mid_print( HWND hwnd )
{
// --------------------------------------------------------------------------
// Skip the whole mess if no file has been selected.
// --------------------------------------------------------------------------
   if (gpszCurFile == NULL) {
      DisplayMessages( NULLHANDLE, "There is not a file open to print",
                       MSG_ERROR );
      return MRFROMLONG(0L);
   } /* endif */

// --------------------------------------------------------------------------
// Start the print thread
// --------------------------------------------------------------------------
   _beginthread( DoPrint, NULL, 0x8000, NULL );

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     mid_font
//
// FUNCTION USE: Process the FILE | SET FONT menu item.
//
// DESCRIPTION:  Retrieve user font selection
//
// PARAMETERS:   HWND     client window handle
//
// RETURNS:      MRESULT  Reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT mid_font( HWND hwnd )
{
   FONTDLG     fd;
   char        szFamilyname[ 256 ];
   SIZEL       sizel = {0,0};
   LONG        cFonts;
   LONG        nFont;
   PFONTMETRICS pfm;

// --------------------------------------------------------------------------
// clear the dialog control structure
// --------------------------------------------------------------------------
   memset( (PVOID)&fd, 0, sizeof( FONTDLG ));

// --------------------------------------------------------------------------
// initialize the structure - pull in the current font attributes and size
// --------------------------------------------------------------------------
   fd.cbSize = sizeof( FONTDLG );
   fd.hpsScreen = NULLHANDLE;
   fd.hpsPrinter = ghpsInfo;
   fd.pszFamilyname = szFamilyname;
   fd.usDlgId = 500;
   fd.usFamilyBufLen = 256;
   fd.clrFore = CLR_BLACK;
   fd.clrBack = CLR_WHITE;
   fd.fAttrs = gfattrs;
   fd.fxPointSize = gfxPointSize;
   fd.fl= FNTS_CENTER | FNTS_INITFROMFATTRS;

// --------------------------------------------------------------------------
// Raise the font dialog
// --------------------------------------------------------------------------
   WinFontDlg( HWND_DESKTOP, hwnd, &fd );
   if (fd.lReturn == DID_OK) {

// --------------------------------------------------------------------------
// Now we need to solve a couple of problems with the font dialog!
//
// First, if a device font with a non-integral point size is selected,
// the FONTDLG structure will contain data for the selected font but the
// FATTRS structure will not be filled in properly.
//
// Second, when a font is selected which can be either a device font
// or a vector font based on the point size, the font dialog seems to
// confuse which should be used and returns a FATTRS structure for a
// vector font when the point size is supported by the printer, and
// a device font lMatch when the point size is not supported.
//
// To resolve these problems, we will examine the available device
// fonts to see if any match the font specifications in the FONTDLG
// structure.  If so, the FATTRS is changed to select this font.  If
// a matching device font is not found, the FATTRS is changed so that
// the nearest matching vector font is chosen.
//
// We begin by loading the available fonts for the select font family.
// --------------------------------------------------------------------------
      cFonts = 0;
      nFont = GpiQueryFonts( ghpsInfo, QF_PUBLIC, fd.pszFamilyname,
                             &cFonts, 0, 0);
      if (nFont != GPI_ALTERROR && nFont != 0) {
         cFonts = nFont;

// --------------------------------------------------------------------------
// allocate memory to load the font metrics for the selected font and
// then bring in the information.  Any failures will abort this little
// bug fix.
// --------------------------------------------------------------------------
         pfm = (PFONTMETRICS)malloc( cFonts * sizeof(FONTMETRICS));
         if (pfm != NULL) {
            GpiQueryFonts( ghpsInfo, QF_PUBLIC, fd.pszFamilyname,
                           &cFonts, sizeof(FONTMETRICS), pfm );
         } else {
            cFonts = 0;
         } /* endif */
      } else {
         cFonts = 0;
      } /* endif */

// --------------------------------------------------------------------------
// Now scan the returned fonts.  Compare each device font (lMatch < 0) to
// see if the point size, width, and weight class are correct.  If one is
// found break out of the loop.
// --------------------------------------------------------------------------
      for( nFont = 0; nFont < cFonts; nFont++ ) {
         if( pfm[nFont].lMatch < 0 ) {
            if( pfm[nFont].sNominalPointSize == FIXEDINT(fd.fxPointSize*10)) {
               if(pfm[nFont].usWeightClass == fd.usWeight ) {
                  if( pfm[nFont].usWidthClass == fd.usWidth ) {
                     break;
                  } /* endif */
               } /* endif */
            } /* endif */
         } /* endif */
      } /* endfor */

      if( nFont != cFonts ) {
// --------------------------------------------------------------------------
// If a device font was found, fill in the FATTRS structure with the lMatch
// value and any of the additional information available in the metrics.
// --------------------------------------------------------------------------
         fd.fAttrs.lMatch = pfm[nFont].lMatch;
         fd.fAttrs.lMaxBaselineExt = pfm[nFont].lMaxBaselineExt;
         fd.fAttrs.lAveCharWidth = pfm[nFont].lAveCharWidth;
         fd.fAttrs.idRegistry = pfm[nFont].idRegistry;
         fd.fAttrs.usCodePage = pfm[nFont].usCodePage;
         fd.fAttrs.fsFontUse &= ~FATTR_FONTUSE_OUTLINE;
         strcpy( fd.fAttrs.szFacename, pfm[nFont].szFacename );

      } else {
// --------------------------------------------------------------------------
// No device font was found, clear the lMatch values and set the FATTRS to
// select an appropriate vector font.
// --------------------------------------------------------------------------
         fd.fAttrs.lMatch = 0;
         fd.fAttrs.lMaxBaselineExt = 0;
         fd.fAttrs.lAveCharWidth = 0;
         fd.fAttrs.fsFontUse |= FATTR_FONTUSE_OUTLINE;
      } /* endif */

// --------------------------------------------------------------------------
// free the font data
// --------------------------------------------------------------------------
      if (pfm != NULL) {
         free(pfm);
      } /* endif */

// --------------------------------------------------------------------------
// The user selected a font - save off the attributes and size
// --------------------------------------------------------------------------
      gfxPointSize = fd.fxPointSize;
      gfattrs = fd.fAttrs;

// --------------------------------------------------------------------------
// Now repaint the window to show the new font
// --------------------------------------------------------------------------
      WinInvalidateRect( hwnd, NULL, TRUE );
   } /* endif */

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     mid_queue
//
// FUNCTION USE: Process the FILE | SELECT QUEUE menu item.
//
// DESCRIPTION:  Retrieve user print queue selection
//
// PARAMETERS:   HWND     client window handle
//
// RETURNS:      MRESULT  Reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT mid_queue( HWND hwnd )
{
// --------------------------------------------------------------------------
// Raise the print queue selection dialog
// --------------------------------------------------------------------------
   if (SelectPrinter( hwnd )) {

// --------------------------------------------------------------------------
// If a printer was selected - close and reopen the information context
// --------------------------------------------------------------------------
      GpiAssociate( ghpsInfo, NULLHANDLE );
      DevCloseDC( ghdcInfo );
      ghdcInfo = OpenDeviceContext( WinQueryAnchorBlock(hwnd),
                                    OD_INFO, NULL );
      GpiAssociate( ghpsInfo, ghdcInfo );

// --------------------------------------------------------------------------
// reload the forms data - after freeing any existing data
// --------------------------------------------------------------------------
      if (gpFormsBuf != NULL) {
         free( gpFormsBuf );
      } /* endif */
      QueryFormsData( ghdcInfo );

// --------------------------------------------------------------------------
// Adjust the scroll bar information
// --------------------------------------------------------------------------
      CalcScrollData( hwnd );

// --------------------------------------------------------------------------
// repaint the display for the new printer/job properties
// --------------------------------------------------------------------------
      WinInvalidateRect( hwnd, NULL, TRUE );
   } /* endif */

// --------------------------------------------------------------------------
// return the appropriate reserved data
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     mid_open
//
// FUNCTION USE: Process the FILE | OPEN... menu item.
//
// DESCRIPTION:  Retrieve user file selection
//
// PARAMETERS:   HWND     client window handle
//
// RETURNS:      MRESULT  Reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT mid_open( HWND hwnd )
{
   FILEDLG     fd;

// --------------------------------------------------------------------------
// initialize the font dialog to display all files in the current directory
// --------------------------------------------------------------------------
   memset( &fd, 0, sizeof( FILEDLG ));
   fd.cbSize = sizeof(FILEDLG);
   fd.fl = FDS_CENTER | FDS_OPEN_DIALOG;
   strcpy( fd.szFullFile, "*" );

// --------------------------------------------------------------------------
// raise the font dialog
// --------------------------------------------------------------------------
   if (WinFileDlg( HWND_DESKTOP, hwnd, &fd )) {
      if (fd.lReturn == DID_OK) {

// --------------------------------------------------------------------------
// free current file name, then copy user's file name to a new buffer
// --------------------------------------------------------------------------
         if (gpszCurFile != NULL) {
            free( gpszCurFile );
         } /* endif */
         gpszCurFile = malloc(strlen(fd.szFullFile) + 1);
         if (gpszCurFile != NULL) {
            strcpy(gpszCurFile, fd.szFullFile);

// --------------------------------------------------------------------------
// Repaint the display with the new file
// --------------------------------------------------------------------------
            WinInvalidateRect( hwnd, NULL, TRUE );
         } /* endif */
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     mid_margins
//
// FUNCTION USE: Process the FILE | SET MARGINS... menu item.
//
// DESCRIPTION:  Retrieve user margins selection
//
// PARAMETERS:   HWND     client window handle
//
// RETURNS:      MRESULT  Reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT mid_margins( HWND hwnd )
{
// --------------------------------------------------------------------------
// Raise the Set Margins dialog
// --------------------------------------------------------------------------
   if (SetPageFormat(hwnd, &grectlMargins)) {

// --------------------------------------------------------------------------
// Redraw the display with the new margins
// --------------------------------------------------------------------------
      WinInvalidateRect( hwnd, NULL, TRUE );
   } /* endif */
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     QueryQueueData
//
// FUNCTION USE: Obtain info for all printer queues and which is the
//               default printer queue
//
// DESCRIPTION:  Use SPL functions to enumerate the queues and scan
//               to determine the current default queue.
//
// PARAMETERS:   void
//
// RETURNS:      BOOL      TRUE - successfully obtained queue data
//
// INTERNALS:    NONE
//
// *******************************************************************************
BOOL QueryQueueData( )
{
   BOOL        bSuccess = FALSE;
   ULONG       rc;
   ULONG       cTotal;
   ULONG       cbNeeded;

// --------------------------------------------------------------------------
// Find out how much memory we need to store the queue information
// --------------------------------------------------------------------------
   rc = SplEnumQueue( NULL, 3, NULL, 0, &gcQueues, &cTotal, &cbNeeded, NULL );
   if( rc == ERROR_MORE_DATA ) {

// --------------------------------------------------------------------------
// Get memory for the queue inforamation
// --------------------------------------------------------------------------
      gpQueueBuf = (PPRQINFO3)malloc( cbNeeded );
      if (gpQueueBuf != NULL) {

// --------------------------------------------------------------------------
// Now get the real queue information
// --------------------------------------------------------------------------
         rc = SplEnumQueue( NULL, 3, gpQueueBuf, cbNeeded, &gcQueues, &cTotal,
                            &cbNeeded, NULL );
         if (rc == NO_ERROR) {

// --------------------------------------------------------------------------
// find the queue which is the current default
// --------------------------------------------------------------------------
            for (gnQueue = 0; gnQueue < gcQueues; gnQueue++) {
               if (gpQueueBuf[gnQueue].fsType & PRQ3_TYPE_APPDEFAULT) {
                  break;
               } /* endif */
            } /* endfor */

// --------------------------------------------------------------------------
// If default was found - return success
// --------------------------------------------------------------------------
            if (gnQueue != gcQueues) {
               bSuccess = TRUE;
            } /* endif */
         } /* endif */
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// return with status of the operation
// --------------------------------------------------------------------------
   return bSuccess;
}

// *******************************************************************************
// FUNCTION:     OpenDeviceContext
//
// FUNCTION USE: Open a device context for the currently selected printer
//               queue
//
// DESCRIPTION:  Opens a printer device context for the currently selected
//               printer using the data in the global print queue information.
//
// PARAMETERS:   HAB      Thread's anchor block handle
//               ULONG    Device context type (QUEUED, INFO, etc.)
//               PSZ      Comment string for opening the device
//
// RETURNS:      HDC      Handle of the opened device context or
//                        DEV_ERROR
//
// INTERNALS:    NONE
//
// *******************************************************************************
HDC OpenDeviceContext( HAB hab, ULONG ulDCType, PSZ pszComment )
{
   DEVOPENSTRUC      dos;
   HDC               hdc;
   char              *tmp, *tmp1;

// --------------------------------------------------------------------------
// Fill the device open structure with the appropriate information from
// the queue data.
// --------------------------------------------------------------------------
   dos.pszLogAddress = gpQueueBuf[gnQueue].pszName;

// --------------------------------------------------------------------------
// Split the driver name off from the queue data driver/printer name
// --------------------------------------------------------------------------
   dos.pszDriverName = malloc(strlen(gpQueueBuf[gnQueue].pszDriverName) + 1);
   if (dos.pszDriverName != NULL) {
      tmp = gpQueueBuf[gnQueue].pszDriverName;
      tmp1 = dos.pszDriverName;
      while( *tmp && *tmp != '.' ) *tmp1++ = *tmp++;
      if( *tmp ) *tmp1 = '\0';

// --------------------------------------------------------------------------
// Continue filling the rest of the structure
// --------------------------------------------------------------------------
      dos.pdriv = gpQueueBuf[gnQueue].pDriverData;
      dos.pszDataType = "PM_Q_STD";
      dos.pszComment = pszComment;
      dos.pszQueueProcName = gpQueueBuf[gnQueue].pszPrProc;
      dos.pszQueueProcParams = NULL;
      dos.pszSpoolerParams = NULL;
      dos.pszNetworkParams = NULL;

// --------------------------------------------------------------------------
// Open the device context
// --------------------------------------------------------------------------
      hdc = DevOpenDC( hab, OD_QUEUED, "*", 9,
                      (PDEVOPENDATA)&dos, NULLHANDLE );

// --------------------------------------------------------------------------
// release the memory used for the driver name
// --------------------------------------------------------------------------
      free( dos.pszDriverName );
   } /* endif */

// --------------------------------------------------------------------------
// return the device context to the caller - this could be an error
// --------------------------------------------------------------------------
   return hdc;
}

// *******************************************************************************
// FUNCTION:     QueryFormData
//
// FUNCTION USE: Obtain info for all forms for the current printer
//
// DESCRIPTION:  Use SPL functions to enumerate the forms and scan
//               to determine the current selected form.
//
// PARAMETERS:   HDC      Device context handle for the selected printer
//
// RETURNS:      MRESULT  Reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
BOOL QueryFormsData(HDC hdc)
{
   BOOL        bSuccess = FALSE;

// --------------------------------------------------------------------------
// Find out how many forms there are to retrieve
// --------------------------------------------------------------------------
   gcForms = DevQueryHardcopyCaps( hdc, 0, 0, NULL );
   if( gcForms != DQHC_ERROR ) {

// --------------------------------------------------------------------------
// allocate memory for the forms data
// --------------------------------------------------------------------------
      gpFormsBuf = (PHCINFO)malloc( gcForms * sizeof(HCINFO));
      if (gpFormsBuf != NULL) {

// --------------------------------------------------------------------------
// Now get the real data
// --------------------------------------------------------------------------
         gcForms = DevQueryHardcopyCaps( hdc, 0, gcForms, gpFormsBuf );
         if (gcForms != DQHC_ERROR) {

// --------------------------------------------------------------------------
// find the currently selected form - user can change through job
// properties.
// --------------------------------------------------------------------------
            for (gnForm = 0; gnForm < gcForms; gnForm++) {
               if (gpFormsBuf[gnForm].flAttributes & HCAPS_CURRENT) {
                  break;
               } /* endif */
            } /* endfor */

// --------------------------------------------------------------------------
// Set successful return if the current form was found
// --------------------------------------------------------------------------
            if (gnForm != gcForms) {
               bSuccess = TRUE;
            } /* endif */
         } /* endif */
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// return operation status to the caller
// --------------------------------------------------------------------------
   return bSuccess;
}

// *******************************************************************************
// FUNCTION:     PrintString
//
// FUNCTION USE: Draw text to a presentation space
//
// DESCRIPTION:  If necessary, splits the text up to fit with the GPI
//               text length limits, handles word wrapping of text too
//               long to fit within the defined margins, and handles
//               WYSIWYG drawing between two different presentation
//               spaces.
//
// PARAMETERS:   HPS      presentation space handle used for determine
//                        how the text will be drawn
//               PPOINTL  pointer to coordinate to use as top, left corner
//                        of the text.
//               ULONG    number of characters to draw
//               PSZ      text to be drawn
//               PRECTL   pointer to rectangle defining the area in which
//                        text must be drawn
//               HPS      presentation space handle used for drawing.  If
//                        NULLHANDLE, the first hps is used.
//
//
// RETURNS:      ULONG    number of characters drawn
//
// INTERNALS:    NONE
//
// *******************************************************************************
ULONG PrintString( HPS hps, PPOINTL pptl, ULONG cbText, PSZ pszText,
                   PRECTL rectlPage, HPS hpsDraw )
{
   char     pszTemp[MAX_GPI_STRING_LENGTH + 1];
   ULONG    cbTemp;
   ULONG    cTemp;
   POINTL   aptl[ TXTBOX_COUNT];
   POINTL   aptlPos[MAX_GPI_STRING_LENGTH + 1];
   LONG     alVecs[MAX_GPI_STRING_LENGTH + 1];
   ULONG    cbOut = 0;

// --------------------------------------------------------------------------
// process until all characters have been drawn
// --------------------------------------------------------------------------
   while (cbText > 0) {

// --------------------------------------------------------------------------
// determine number of characters in this pass and copy to temp buffer
// --------------------------------------------------------------------------
      cbTemp = cbText;
      if (cbText > MAX_GPI_STRING_LENGTH) {
         cbTemp = MAX_GPI_STRING_LENGTH;
      } /* endif */
      strncpy( pszTemp, pszText, cbTemp );
      pszTemp[cbTemp] = '\0';

// --------------------------------------------------------------------------
// get the relative coordinates of the space occupied by the string
// --------------------------------------------------------------------------
      GpiQueryTextBox( hps, cbTemp, pszTemp, TXTBOX_COUNT, aptl );

// --------------------------------------------------------------------------
// Adjust the y coodinate to the baseline of the string.
// --------------------------------------------------------------------------
      pptl->y -= aptl[TXTBOX_TOPLEFT].y;

// --------------------------------------------------------------------------
// if the bottom of the string is below the page limit - bail for a new page
// --------------------------------------------------------------------------
      if (pptl->y + aptl[TXTBOX_BOTTOMLEFT].y < rectlPage->yBottom) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Get the positions at which each character will be drawn.
// --------------------------------------------------------------------------
      GpiQueryCharStringPosAt( hps, pptl, 0l, cbTemp,
                               pszTemp, NULL, aptlPos );

// --------------------------------------------------------------------------
// find the last character that fits within the page limit
// --------------------------------------------------------------------------
      cTemp = 0;
      while( cTemp < cbTemp && aptlPos[cTemp+1].x < rectlPage->xRight ) {
         cTemp++;
      } /* endwhile */

// --------------------------------------------------------------------------
// if the whole string can't be drawn then we have to split
// --------------------------------------------------------------------------
      if (cbTemp != cTemp) {
         cbTemp = cTemp;

// --------------------------------------------------------------------------
// if the split occurred at a space, go ahead and print the space and any
// additional spaces following.
// --------------------------------------------------------------------------
         while( pszTemp[cTemp] == ' ' ) cTemp++;

// --------------------------------------------------------------------------
// if not now at a space, backup till we find one so we don't split in the
// middle of a word
// --------------------------------------------------------------------------
         while( pszTemp[cTemp] != ' ' ) {
            if (cTemp == 0) {
               break;
            } /* endif */
            cTemp--;
         } /* endwhile */

// --------------------------------------------------------------------------
// use the current location unless we went all the way back to the beginning
// of the line.  In this latter case, we just print what we can and go back
// to the beginning.
// --------------------------------------------------------------------------
         if (cTemp != 0) {
            cbTemp = cTemp+1;
         } /* endif */
      } /* endif */

// --------------------------------------------------------------------------
// if we drawing into a different presentation space, build a increment
// vector for placing each character.
// --------------------------------------------------------------------------
      if ( hpsDraw != NULLHANDLE ) {
         for (cTemp = 0; cTemp < cbTemp; cTemp++ ) {
            alVecs[cTemp] = aptlPos[cTemp+1].x - aptlPos[cTemp].x;
         } /* endfor */

// --------------------------------------------------------------------------
// now draw the string using the increment vector
// --------------------------------------------------------------------------
         GpiCharStringPosAt( hpsDraw, pptl, NULL, CHS_VECTOR, cbTemp,
                             pszTemp, alVecs );
      } else {

// --------------------------------------------------------------------------
// same presentation space - draw the string
// --------------------------------------------------------------------------
         GpiCharStringAt( hps, pptl, cbTemp, pszTemp );
      } /* endif */

// --------------------------------------------------------------------------
// count off what we've done so far
// --------------------------------------------------------------------------
      cbText -= cbTemp;
      cbOut += cbTemp;
      pszText += cbTemp;

// --------------------------------------------------------------------------
// set y coordinate to the bottom of the string.  The top of the next string
// starts here.
// --------------------------------------------------------------------------
      pptl->y += aptl[TXTBOX_BOTTOMLEFT].y;
   } /* endwhile */

// --------------------------------------------------------------------------
// return the number of characters actually output.
// --------------------------------------------------------------------------
   return cbOut;
}

// *******************************************************************************
// FUNCTION:     CalcScrollData
//
// FUNCTION USE: Compute scrolling information based on window size
//               and form size
//
// DESCRIPTION:  Compares the rectangles of the window and form to determine
//               if scrolling is required and the number of units to be
//               scrolled
//
// PARAMETERS:   HWND      handle of the window
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
void CalcScrollData( HWND hwnd )
{
   RECTL rectlPage, rectlWin;
   SIZEL sizel = { 0, 0 };
   POINTL      ptlXform;
   USHORT      usCurPos;
   HWND  hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
   HWND  hwndVScroll = WinWindowFromID( hwndFrame, FID_VERTSCROLL );
   HWND  hwndHScroll = WinWindowFromID( hwndFrame, FID_HORZSCROLL );

// --------------------------------------------------------------------------
// Define a rectangle describing the printer's form size
// --------------------------------------------------------------------------
   rectlPage.xLeft = 0;
   rectlPage.yBottom = 0;
   rectlPage.xRight = gpFormsBuf[gnForm].cx * 10;
   rectlPage.yTop = gpFormsBuf[gnForm].cy * 10;

// --------------------------------------------------------------------------
// Get the current window size rectangle and convert to common units
// --------------------------------------------------------------------------
   WinQueryWindowRect( hwnd, &rectlWin );
   GpiConvert( ghpsDisplay, CVTC_DEVICE, CVTC_WORLD, 2, (PPOINTL)&rectlWin );

// --------------------------------------------------------------------------
// Save off the window size for scrolling
// --------------------------------------------------------------------------
   gsizelWin.cx = rectlWin.xRight;
   gsizelWin.cy = rectlWin.yTop;

// --------------------------------------------------------------------------
// Determine the translation required to move to the furthest extents of
// printer size.
// --------------------------------------------------------------------------
   ptlXform.y = rectlPage.yTop - rectlWin.yTop;
   ptlXform.x = rectlPage.xRight - rectlWin.xRight;

// --------------------------------------------------------------------------
// If the vertical form size is larger than the window, set the scroll bar
// range for the vertical scroll bar.
// --------------------------------------------------------------------------
   if( ptlXform.y > 0 ) {
      usCurPos = SHORT1FROMMR( WinSendMsg( hwndVScroll, SBM_QUERYPOS,
                               MPFROMLONG(0L), MPFROMLONG(0L)));
      WinSendMsg( hwndVScroll, SBM_SETSCROLLBAR, MPFROMSHORT(usCurPos),
                  MPFROM2SHORT( 0, ptlXform.y ));
   } else {
      WinSendMsg( hwndVScroll, SBM_SETSCROLLBAR, MPFROMLONG(0L),
                  MPFROM2SHORT( 0, 0 ));
   } /* endif */

// --------------------------------------------------------------------------
// If the horizontal form size is larger than the window, set the scroll bar
// range for the horizontal scroll bar.
// --------------------------------------------------------------------------
   if( ptlXform.x > 0 ) {
      usCurPos = SHORT1FROMMR( WinSendMsg( hwndHScroll, SBM_QUERYPOS,
                               MPFROMLONG(0L), MPFROMLONG(0L)));
      WinSendMsg( hwndHScroll, SBM_SETSCROLLBAR, MPFROMSHORT(usCurPos),
                  MPFROM2SHORT( 0, ptlXform.x ));
   } else {
      WinSendMsg( hwndHScroll, SBM_SETSCROLLBAR, MPFROMLONG(0L),
                  MPFROM2SHORT( 0, 0 ));
   } /* endif */
}

// *******************************************************************************
// FUNCTION:     DoPrint
//
// FUNCTION USE: Thread function to handle printing
//
// DESCRIPTION:  Send output to the printer
//
// PARAMETERS:   PVOID    thread argument
//
// RETURNS:      void
//
// *******************************************************************************
void DoPrint( PVOID arg )
{
   HAB         hab;
   ULONG       rc;
   HDC         hdc;
   HPS         hps;
   SIZEL       sizel;
   SIZEF       sizef;
   POINTL      ptl;
   LONG        cbDevOut;
   USHORT      usJob;
   RECTL       rectlPage;
   PAPPFILE    pFile;
   PSZ         pBuffer;
   size_t      cbBuffer;
   ULONG       cbOut;

// --------------------------------------------------------------------------
// Initialize the PM environment for the thread
// --------------------------------------------------------------------------
   hab = WinInitialize( 0l );

// --------------------------------------------------------------------------
// Open up a queued device context for printing
// --------------------------------------------------------------------------
   hdc = OpenDeviceContext( hab, OD_QUEUED, gpszCurFile );
   if (hdc != DEV_ERROR) {

// --------------------------------------------------------------------------
// Initially set the page rectangle to the page size less the margins
// --------------------------------------------------------------------------
      rectlPage.xLeft = grectlMargins.xLeft;
      rectlPage.yBottom = grectlMargins.yBottom;
      rectlPage.xRight = (gpFormsBuf[gnForm].cx * 10) - grectlMargins.xRight;
      rectlPage.yTop = (gpFormsBuf[gnForm].cy * 10) - grectlMargins.yBottom;

// --------------------------------------------------------------------------
// If any of these are not within the clip boundary of the printer - adjust
// --------------------------------------------------------------------------
      if( rectlPage.xLeft < gpFormsBuf[gnForm].xLeftClip * 10)
         rectlPage.xLeft = gpFormsBuf[gnForm].xLeftClip * 10;
      if( rectlPage.yBottom < gpFormsBuf[gnForm].yBottomClip * 10)
         rectlPage.yBottom = gpFormsBuf[gnForm].yBottomClip * 10;
      if( rectlPage.xRight > gpFormsBuf[gnForm].xRightClip * 10)
         rectlPage.xRight = gpFormsBuf[gnForm].xRightClip * 10;
      if( rectlPage.yTop > gpFormsBuf[gnForm].yTopClip * 10)
         rectlPage.yTop = gpFormsBuf[gnForm].yTopClip * 10;

// --------------------------------------------------------------------------
// Now adjust the rectangle so the origin is at the lower left clip limit
// --------------------------------------------------------------------------
      rectlPage.xLeft -= gpFormsBuf[gnForm].xLeftClip * 10;
      rectlPage.xRight -= gpFormsBuf[gnForm].xLeftClip * 10;
      rectlPage.yBottom -= gpFormsBuf[gnForm].yBottomClip * 10;
      rectlPage.yTop -= gpFormsBuf[gnForm].yBottomClip * 10;

// --------------------------------------------------------------------------
// Create a presentation space for drawing
// --------------------------------------------------------------------------
      sizel.cx = 0;
      sizel.cy = 0;
      hps = GpiCreatePS( hab, hdc, &sizel, PU_LOMETRIC | GPIA_ASSOC );
      if( hps != NULLHANDLE ) {

// --------------------------------------------------------------------------
// Initiate the job - the spooler metafile is created at this point, so
// don't do anything in the PS that needs to be metafiled before here.
// --------------------------------------------------------------------------
         rc = DevEscape( hdc, DEVESC_STARTDOC,
                         strlen( gpszCurFile ), gpszCurFile,
                         &cbDevOut, NULL );
         if( rc == DEV_OK ) {

// --------------------------------------------------------------------------
// Normally I like to set the alignment to top left, but WARP seems to
// have a problem with this.  If the following statement is included,
// device fonts will print fine and ATM fonts show up in the picture
// viewer, but do not display on the printer.
//
// For now, the default base left alignment is used and the calculations
// below assume this alignment.
// --------------------------------------------------------------------------
//            GpiSetTextAlignment( hps, TA_LEFT, TA_TOP );

// --------------------------------------------------------------------------
// Setup the requested font.  Note that the character box size is not
// necessary for device fonts, but doesn't harm anything.
// --------------------------------------------------------------------------
            GpiCreateLogFont( hps, NULL, 1, &gfattrs );
            GpiSetCharSet( hps, 1 );
            sizef.cx = (gfxPointSize / 72) * 254;
            sizef.cy = (gfxPointSize / 72) * 254;
            GpiSetCharBox( hps, &sizef );

// --------------------------------------------------------------------------
// Draw a border around the area to be printed
// --------------------------------------------------------------------------
            ptl.x = rectlPage.xLeft;
            ptl.y = rectlPage.yBottom;
            GpiMove( hps, &ptl );
            ptl.x = rectlPage.xRight;
            ptl.y = rectlPage.yTop;
            GpiBox( hps, DRO_OUTLINE, &ptl, 0, 0 );
            ptl.x = rectlPage.xLeft;
            ptl.y = rectlPage.yTop;

// --------------------------------------------------------------------------
// Now we're ready to do some printing - open the selected file
// --------------------------------------------------------------------------
            pFile = OpenFile( gpszCurFile );
            if (pFile != NULL) {

// --------------------------------------------------------------------------
// Keep going until the whole file is printed.
// --------------------------------------------------------------------------
               while (!IsEOF( pFile )) {

// --------------------------------------------------------------------------
// Get a line of text - if it's a blank line simulate a line with a single
// space character.
// --------------------------------------------------------------------------
                  cbBuffer = ReadFile( pFile, &pBuffer );
                  if (cbBuffer == 0) {
                     cbBuffer = 1;
                     pBuffer = " ";
                  } /* endif */

// --------------------------------------------------------------------------
// Call the routine to wrap the text.  If this returns without printing
// all the text, we need to start a new page, then continue the rest
// of the line.
// --------------------------------------------------------------------------
                  cbOut = 0;
                  while (cbOut != cbBuffer) {
                     cbOut += PrintString( hps, &ptl, cbBuffer, pBuffer,
                                           &rectlPage, NULLHANDLE );
                     if (cbOut != cbBuffer) {

// --------------------------------------------------------------------------
// Start the new page and draw the box
// --------------------------------------------------------------------------
                        DevEscape( hdc, DEVESC_NEWFRAME, 0, NULL, 0, NULL );
                        ptl.x = rectlPage.xLeft;
                        ptl.y = rectlPage.yBottom;
                        GpiMove( hps, &ptl );
                        ptl.x = rectlPage.xRight;
                        ptl.y = rectlPage.yTop;
                        GpiBox( hps, DRO_OUTLINE, &ptl, 0, 0 );

// --------------------------------------------------------------------------
// Remember to set the drawing point back to the top of the page
// --------------------------------------------------------------------------
                        ptl.x = rectlPage.xLeft;
                        ptl.y = rectlPage.yTop;
                     } /* endif */
                  } /* endwhile */
               } /* endwhile */

// --------------------------------------------------------------------------
// Done - stop the job and close the file
// --------------------------------------------------------------------------
               cbDevOut = 2;
               DevEscape( hdc, DEVESC_ENDDOC, 0, NULL,
                          &cbDevOut, (PCHAR)&usJob );
               CloseFile( pFile );
            } else {

// --------------------------------------------------------------------------
// Couldn't open the file - abort the job
// --------------------------------------------------------------------------
               DevEscape( hdc, DEVESC_ABORTDOC, 0, NULL, 0, NULL );
            } /* endif */
         } /* endif */

// --------------------------------------------------------------------------
// Release the presentation space
// --------------------------------------------------------------------------
         GpiDestroyPS( hps );
      } /* endif */

// --------------------------------------------------------------------------
// Close out the device context
// --------------------------------------------------------------------------
      rc = DevCloseDC( hdc );
   } /* endif */

// --------------------------------------------------------------------------
// Done!!!!!!!!!
// --------------------------------------------------------------------------
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
// HISTORY:
//
// *******************************************************************************
MRESULT EXPENTRY ProdInfoDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  APIRET  rc;

  switch (msg)
   {
    case WM_INITDLG:
         CenterDialog(hwnd);
         SetTheSysMenu(hwnd, NULL);
         break;

    case WM_COMMAND:
         switch (COMMANDMSG(&msg)->cmd)
          {
           case DID_CANCEL:
           case DID_OK:
                WinDismissDlg(hwnd, TRUE);
                break;
          }
         break ;
   }
 return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
}
