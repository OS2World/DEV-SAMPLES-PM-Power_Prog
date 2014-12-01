// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      SUBCLASS.C                                       mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Main Source File for SUBCLASS/Chapter 10 Sample Program       บ
// บ                                                                               บ
// บ  NOTES:         This program demonstartes the creation of new control classes บ
// บ                 by subclassing existing classes                               บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#define  INCL_WIN
#define  INCL_DOS
#define  INCL_GPI

#include <os2.h>
#include <string.h>                      
#include <stdlib.h>                      
#include <stdio.h>                       

#include "myclass.h"
#include "subclass.h"
#include "shcommon.h"

// --------------------------------------------------------------------------
// Define window instance data constants
// --------------------------------------------------------------------------
#define  WD_HWND_CONTROL         0
#define  APP_WINDOW_DATA_SIZE    4
#define  APPNAME "Subclass Sample"

// --------------------------------------------------------------------------
// Defined control window ids
// --------------------------------------------------------------------------
#define  FIELD_1_LABEL           101
#define  FIELD_2_LABEL           102
#define  FIELD_3_LABEL           103
#define  FIELD_4_LABEL           104
#define  FIELD_1_ENTRY           105
#define  FIELD_2_ENTRY           106
#define  FIELD_3_ENTRY           107
#define  FIELD_4_ENTRY           108

// --------------------------------------------------------------------------
// Global varibles used only in this source file
// --------------------------------------------------------------------------
static   BOOL  false = FALSE;

// --------------------------------------------------------------------------
// Prototype application window procedure
// --------------------------------------------------------------------------
MRESULT APIENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------
// Prototype for Product Information Dialog Procedure
// --------------------------------------------------------------------------
MRESULT EXPENTRY ProdInfoDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

// --------------------------------------------------------------------------
// Prototype message processing worker functions
// --------------------------------------------------------------------------
static MRESULT wmCommand( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wmPaint( HWND hwnd, MPARAM mp1, MPARAM mp2 );

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
// *******************************************************************************
void main(void)
{
   HAB   hab;
   HMQ   hmq;
   QMSG  qmsg;
   HWND  hwndFrame;
   HWND  hwndClient;
   BOOL  bSuccess;
   ULONG flCreateFlags = FCF_STANDARD & ~(FCF_ACCELTABLE);         

// --------------------------------------------------------------------------
// First things first - set up the PM environment for the thread
// --------------------------------------------------------------------------
      hab = WinInitialize(NULLHANDLE);

      if (!hab) 
       {
         // Oops - couldn't do it - bail out!
         DosBeep( 60, 100 );
         exit (TRUE);
       } 


// --------------------------------------------------------------------------
// Next thing we need is a message queue
// --------------------------------------------------------------------------
      hmq = WinCreateMsgQueue (hab, NULLHANDLE);

      if (!hmq) 
       {
        DosBeep(60, 100);   
        WinTerminate(hab);  
        exit(TRUE);         
       } 

// --------------------------------------------------------------------------
// Register the class for the new control
// --------------------------------------------------------------------------
      RegisterMyClass( hab );

// --------------------------------------------------------------------------
// Now register the window class for the main application window
// --------------------------------------------------------------------------
      bSuccess = WinRegisterClass (hab,
                                   APPNAME,
                                   ClientWndProc,
                                   CS_SIZEREDRAW | CS_CLIPSIBLINGS | CS_CLIPCHILDREN,
                                   APP_WINDOW_DATA_SIZE);
      if (!bSuccess) 
       {
        // failed!
        DisplayMessages(NULLHANDLE, "Unable to Register Window Class", MSG_ERROR);   
        exit (TRUE);
       } 


// --------------------------------------------------------------------------
// Now create the application's main frame window
// --------------------------------------------------------------------------
      hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
                                     NULLHANDLE, 
                                     &flCreateFlags, 
                                     APPNAME, 
                                     TITLEBAR, 
                                     0L, 
                                     (HMODULE)NULL, 
                                     APPLICATION_ID,
                                     &hwndClient); 

      if (!hwndFrame) 
       {
         // no window - gotta leave.
         DisplayMessages(NULLHANDLE, "Error Creating Frame Window", MSG_ERROR);  
         exit(TRUE);                             
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
  while(WinGetMsg(hab, &qmsg, (HWND)NULL, 0, 0 ))                           
    WinDispatchMsg(hab, &qmsg);                                             
                                                                            
  WinDestroyWindow(hwndFrame);                                              
  WinDestroyMsgQueue(hmq);                                                  
  WinTerminate(hab);                                                        
  return(FALSE);                                                            
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
MRESULT APIENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   switch (msg) 
    {
     case WM_COMMAND:           
          return wmCommand (hwnd, mp1, mp2);                

     case WM_CREATE:            
          return wmCreate (hwnd, mp1, mp2);                

     case WM_PAINT:           
          return wmPaint (hwnd, mp1, mp2);           

     default:            
          return WinDefWindowProc (hwnd, msg, mp1, mp2);              
    } /* endswitch */
}

// *******************************************************************************
// FUNCTION:     wmCommand
//
// FUNCTION USE: Process WM_COMMAND messages for the application client window
//
// DESCRIPTION:  This function handles the WM_COMMAND messages for the
//               application's various client windows.  The routine processes
//               the user's menu selections for the application.
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
   HWND     hwndFrame = WinQueryWindow( hwnd, QW_PARENT );
   HWND     hwndCtrl;

   switch (SHORT1FROMMP(mp1)) 
    {
     // --------------------------------------------------------------------------
     // The application menu allows editing of the controls from the keyboard.
     // There are four menuitems, one for each control.  When a menuitem is
     // selected, an MC_ALLOWEDIT message is sent to the control to enable
     // and set focus to the entry field.
     // --------------------------------------------------------------------------
     case MID_F1:
          hwndCtrl = WinWindowFromID( hwndFrame, FIELD_1_LABEL );
          break;

     case MID_F2:
          hwndCtrl = WinWindowFromID( hwndFrame, FIELD_2_LABEL );
          break;

     case MID_F3:
          hwndCtrl = WinWindowFromID( hwndFrame, FIELD_3_LABEL );
          break;

     case MID_F4:
          hwndCtrl = WinWindowFromID( hwndFrame, FIELD_4_LABEL );
          break;

     case MID_HELPGEN:           
     case MID_HELPKEYS:          
          DisplayMessages(NULLHANDLE, HELP_TEXT, MSG_INFO);
          return FALSE;

     case MID_PRODINFO:   
          WinDlgBox(HWND_DESKTOP, hwnd, ProdInfoDlgProc, NULLHANDLE, IDD_PRODINFO, NULL);                
          return FALSE;
    } /* endswitch */ 

   // --------------------------------------------------------------------------
   // Send the allow edit to the selected control window
   // --------------------------------------------------------------------------
   if (hwndCtrl != (HWND)NULLHANDLE) 
    {
      WinSendMsg (hwndCtrl, MC_ALLOWEDIT, (MPARAM)TRUE, (MPARAM)0l ); 
    }

// --------------------------------------------------------------------------
// return the reserved value
// --------------------------------------------------------------------------
   return (MRESULT)0l;
}

// *******************************************************************************
// FUNCTION:     wmCreate
//
// FUNCTION USE: Process WM_CREATE messages for the application client window
//
// DESCRIPTION:  Handles application initialization which should occur as
//               the main application window is created.  In this case the
//               controls for the window are created.
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
static MRESULT wmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND           hwndParent = WinQueryWindow( hwnd, QW_PARENT);
   HWND           hwndControl;
   ULONG          yCoord;
   ULONG          ulHeight;
   HPS            hps;
   FATTRS         fattrs;
   SIZEF          sizef;
   STR8           name;

   // --------------------------------------------------------------------------
   // Get the default font infomation to determine the character dimensions
   // --------------------------------------------------------------------------
   hps = WinGetPS(hwnd);

   if (hps == NULLHANDLE) 
    {
     // --------------------------------------------------------------------------
     // if no presentation space - just use a default value
     // --------------------------------------------------------------------------
     ulHeight = 50;
    } 

   else
    {
     // --------------------------------------------------------------------------
     // Get the default font via a font attribute FATTRS structure   
     // --------------------------------------------------------------------------
     // NOTE BUG: under the current implementation of both r206 (OS2 2.x) and 
     // r207 (OS/2 Warp), there is no validation check done for the Font Name 
     // parameter, so if we pass a NULL value we will trap in the worker routine 
     // for Gpi32QueryLogicalFont which is QueryLogicalFont32 in the graphics 
     // engine.  This is really stupid and we need to fix it.  In the interim 
     // we work around the trap here, since trapping in the graphics engine 
     // can be ugly.
     // --------------------------------------------------------------------------
     GpiQueryLogicalFont( hps, LCID_DEFAULT, &name, &fattrs, sizeof(FATTRS));
     if (fattrs.fsFontUse & FATTR_FONTUSE_OUTLINE) 
      {
       // --------------------------------------------------------------------------
       // Get the character box size for outline fonts
       // --------------------------------------------------------------------------
       GpiQueryCharBox( hps, &sizef );
       ulHeight = FIXEDINT(sizef.cy) + 10;
      } 

     else
      {
       // --------------------------------------------------------------------------
       // Use the character data for image fonts.
       // --------------------------------------------------------------------------
       ulHeight = fattrs.lMaxBaselineExt + 10;
      } /* endif */
      WinReleasePS(hps);
   } /* endif */



// --------------------------------------------------------------------------
// Create the control windows as siblings of the main client window.  We
// start at the top of the window and move down, creating a editable static
// field and an entry field at each desired y coordinate.
//
// Create field one
// --------------------------------------------------------------------------
   yCoord = 200;
   hwndControl = WinCreateWindow (hwndParent,
                                  "MYCLASS", "Field One:",
                    WS_VISIBLE | ES_RIGHT | MC_STATIC, 10,
                    yCoord, 120l, ulHeight, hwndParent,
                    HWND_TOP, FIELD_1_LABEL, NULL, NULL );

   hwndControl = WinCreateWindow (hwndParent,                            // Parent Window Handle                   
                                  WC_ENTRYFIELD,                         // Class Name                             
                                  "Sample Text for Field 1",             // Window Text                            
                                  WS_VISIBLE | WS_TABSTOP | ES_MARGIN,   // Window Styles                          
                                  140,                                   // Initial X coordinate                   
                                  yCoord,                                // Initial Y coordinate                   
                                  300l,                                  // Horizontal Length of window            
                                  ulHeight,                              // Vertical Width of window              
                                  hwndParent,                            // Owner Window Handle                    
                                  hwndControl,                           // Sibling Window                         
                                  FIELD_1_ENTRY,                         // Resource Identifier                    
                                  NULL,                                  // Button Control Data                    
                                  NULL);                                 // Presentation Parameters                



// --------------------------------------------------------------------------
// Create field 2
// --------------------------------------------------------------------------
   yCoord -= 50;
   hwndControl = WinCreateWindow( WinQueryWindow(hwnd, QW_PARENT),
                    "MYCLASS", "Field Two:",
                    WS_VISIBLE | ES_RIGHT | MC_STATIC, 10,
                    yCoord, 120l, ulHeight, hwndParent,
                    hwndControl, FIELD_2_LABEL, NULL, NULL );

   hwndControl = WinCreateWindow (hwndParent,                            // Parent Window Handle          
                                  WC_ENTRYFIELD,                         // Class Name                    
                                  "Sample Text for Field 2",             // Window Text                   
                                  WS_VISIBLE | WS_TABSTOP | ES_MARGIN,   // Window Styles                 
                                  140,                                   // Initial X coordinate          
                                  yCoord,                                // Initial Y coordinate          
                                  300l,                                  // Horizontal Length of window   
                                  ulHeight,                              // Vertical Width of window      
                                  hwndParent,                            // Owner Window Handle           
                                  hwndControl,                           // Sibling Window                
                                  FIELD_2_ENTRY,                         // Resource Identifier           
                                  NULL,                                  // Button Control Data           
                                  NULL);                                 // Presentation Parameters       


// --------------------------------------------------------------------------
// Create field 3
// --------------------------------------------------------------------------
   yCoord -= 50;
   hwndControl = WinCreateWindow( WinQueryWindow(hwnd, QW_PARENT),
                    "MYCLASS", "Field Three:",
                    WS_VISIBLE | ES_RIGHT | MC_STATIC, 10,
                    yCoord, 120l, ulHeight, hwndParent,
                    hwndControl, FIELD_3_LABEL, NULL, NULL );

   hwndControl = WinCreateWindow (hwndParent,                            // Parent Window Handle           
                                  WC_ENTRYFIELD,                         // Class Name                     
                                  "Sample Text for Field 3",             // Window Text                    
                                  WS_VISIBLE | WS_TABSTOP | ES_MARGIN,   // Window Styles                  
                                  140,                                   // Initial X coordinate           
                                  yCoord,                                // Initial Y coordinate           
                                  300l,                                  // Horizontal Length of window    
                                  ulHeight,                              // Vertical Width of window       
                                  hwndParent,                            // Owner Window Handle            
                                  hwndControl,                           // Sibling Window                 
                                  FIELD_3_ENTRY,                         // Resource Identifier            
                                  NULL,                                  // Button Control Data            
                                  NULL);                                 // Presentation Parameters        

// --------------------------------------------------------------------------
// create field 4
// --------------------------------------------------------------------------
   yCoord -= 50;
   hwndControl = WinCreateWindow( WinQueryWindow(hwnd, QW_PARENT),
                    "MYCLASS", "Field Four:",
                    WS_VISIBLE | ES_RIGHT | MC_STATIC, 10,
                    yCoord, 120l, ulHeight, hwndParent,
                    hwndControl, FIELD_4_LABEL, NULL, NULL );

   hwndControl = WinCreateWindow (hwndParent,                            // Parent Window Handle                
                                  WC_ENTRYFIELD,                         // Class Name                          
                                  "Sample Text for Field 4",             // Window Text                         
                                  WS_VISIBLE | WS_TABSTOP | ES_MARGIN,   // Window Styles                       
                                  140,                                   // Initial X coordinate                
                                  yCoord,                                // Initial Y coordinate                
                                  300l,                                  // Horizontal Length of window         
                                  ulHeight,                              // Vertical Width of window            
                                  hwndParent,                            // Owner Window Handle                 
                                  hwndControl,                           // Sibling Window                      
                                  FIELD_4_ENTRY,                         // Resource Identifier                 
                                  NULL,                                  // Button Control Data                 
                                  NULL);                                 // Presentation Parameters             

// --------------------------------------------------------------------------
// back with reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     wmCreate
//
// FUNCTION USE: Process WM_PAINT messages for the application client window
//
// DESCRIPTION:  Draws the contents of the application's client window, in
//               this case by filling in the background.
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
   HPS      hps;
   RECTL    rectl;

// --------------------------------------------------------------------------
// Begin the paint operation, obtaining a presentation space handle
// --------------------------------------------------------------------------
   hps = WinBeginPaint( hwnd, (HPS)NULLHANDLE, &rectl );
   if (hps != (HPS)NULLHANDLE) {

// --------------------------------------------------------------------------
// fill in the client window background
// --------------------------------------------------------------------------
      WinFillRect( hps, &rectl, CLR_WHITE );

// --------------------------------------------------------------------------
// complete the paint operation and release the presentation space
// --------------------------------------------------------------------------
      WinEndPaint( hps );
   } /* endif */

// --------------------------------------------------------------------------
// send back the reserved value
// --------------------------------------------------------------------------
   return (MRESULT)0l;
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
// HISTORY:      mm-dd-yy
//
// *******************************************************************************
MRESULT EXPENTRY ProdInfoDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 switch (msg)
  {
   case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
         {
          case DID_CANCEL:
          case DID_OK:
               WinDismissDlg(hwnd,TRUE);
               return FALSE;
         }
  }
 return WinDefDlgProc(hwnd,msg,mp1,mp2);
}
