// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      SHOWOFF.C                                        mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Bitmap Presentation Utility                                   บ
// บ                                                                               บ
// บ  NOTES:         This program is a simple OS/2 Bitmap Display Facility.  This  บ
// บ                 sample program allows the user to display bitmap files.  The  บ
// บ                 user can view multiple bitmap files together to create a      บ
// บ                 presentation.                                                 บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
                                
#define INCL_WINSWITCHLIST
#define INCL_WINPALETTE
#define INCL_WINSYS
#define INCL_WINWINDOWMGR
#define INCL_WINMESSAGEMGR
#define INCL_WININPUT
#define INCL_WINDIALOGS
#define INCL_WINFRAMEMGR
#define INCL_WINPOINTERS
#define INCL_WINERRORS
#define INCL_WINSTDFILE
#define INCL_WINSHELLDATA
#define INCL_WINHELP
#define INCL_WINBUTTONS
#define INCL_WINMENUS
#define INCL_WINLISTBOXES
#define INCL_GPICONTROL
#define INCL_GPIPRIMITIVES
#define INCL_GPIBITMAPS
#define INCL_DOSPROCESS
#define INCL_DOSFILEMGR
#define INCL_DOSMEMMGR
#define INCL_DOSSEMAPHORES
#define INCL_DOSMODULEMGR
#define INCL_GPI
#define INCL_GPILOGCOLORTABLE

#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <process.h>
#include "showoff.h"
#include "sherror.h"
#include "showoff.fnc"
#include "shcommon.h"
#include "structs.h"

BOOL    bStopPresentation;
BOOL    bNoLogo         = FALSE;     
BOOL    bPaused         = FALSE;
BOOL    bPaletteSupport = FALSE;

HWND    hwndClient;

USHORT  usBitCount;
USHORT  usXRes;
USHORT  usYRes;
USHORT  usDirection;

// --------------------------------------------------------------------------
// Handles used for Drawing 
// --------------------------------------------------------------------------
static HPAL    hpal      = NULLHANDLE;  // Current Palette 
static HPAL    hpalOld   = NULLHANDLE;  // Original Palette
static HDC     hdcMem    = NULLHANDLE;  // Memory DC Handle
static HDC     hdcWindow = NULLHANDLE;  // Window DC Handle
static HPS     hpsMem    = NULLHANDLE;  // Memory PS Handle
static HPS     hpsWindow = NULLHANDLE;  // Window PS Handle
static HBITMAP hbm       = NULLHANDLE;  // Current Bitmap to be Drawn
static ULONG   ulPalEntries;            // Number of Physical Palette Entries 
                                        //  Changed by WinRealizePalette call

// --------------------------------------------------------------------------
// Semaphore Handles
// --------------------------------------------------------------------------
HEV     hevTimeout;
HEV     hevPause;

// --------------------------------------------------------------------------
// Class and Titlebar Strings
// --------------------------------------------------------------------------
PSZ     pszClassName = "WC_SHOWOFF";
PSZ     pszTitleBar  = "SHOWOFF - Bitmap Presentation Facility";

// --------------------------------------------------------------------------
// Thread Functions
// --------------------------------------------------------------------------
VOID EXPENTRY DrawBitmapThread(PUCHAR szBitmapFile);
VOID EXPENTRY PresentationThread(PSZ pszPresentation);

// --------------------------------------------------------------------------
// Window Procedures specific to this module.
// --------------------------------------------------------------------------
MRESULT EXPENTRY HelpButtonOpenFilterProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);            


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
// HISTORY:
//
// *******************************************************************************
int main(int argc, char *argv[])
{
  HAB       hab;
  ULONG     ulFrameFlags;
  QMSG      qmsg;
  HMQ       hmq;
  BOOL      success;
  LONG      lcxScreen;
  LONG      lcyScreen;
  LONG      lWidth;
  LONG      lLength;
  SWP       swp;
  ULONG     ulLength = sizeof(SWP);
  HELPINIT  helpinit;              // Help initialization structure
  APIRET    rc;
  LONG      lResult;
  PSZ       pszCommandLine;
  TID       tid;
  HPS       hps;
  HSWITCH   hswitch;
  SWCNTRL   swctl;
  PID       pid;
  HWND      hwndHelp;

  static    CHAR szBitmapFile[CCHMAXPATH];
  CHAR      szBuffer[CCHMAXPATH];

  // --------------------------------------------------------------------------
  // See if the user wants to see the Product Information Dialog Box.
  // --------------------------------------------------------------------------
  lResult = PrfQueryProfileInt(HINI_USER, APPNAME, KEY_LOGO, 0);

  if (lResult == 1)
   {
    bNoLogo = TRUE;
   }

  // --------------------------------------------------------------------------
  // Initialize our application with the Presentation Manager
  // --------------------------------------------------------------------------
  hab = WinInitialize(NULLHANDLE);
  if(!hab)
   {
    DosBeep(60, 100);
    exit(TRUE);
   }

  // --------------------------------------------------------------------------
  // Create our application message queue
  // --------------------------------------------------------------------------
  hmq = WinCreateMsgQueue(hab, NULLHANDLE);
  if(!hmq)
   {
    DosBeep(60, 100);
    WinTerminate(hab);
    exit(TRUE);
   }

  // --------------------------------------------------------------------------  
  // Register our window class for our main window.         
  // --------------------------------------------------------------------------  
  success = WinRegisterClass(hab,            // Anchor Block Handle
                             pszClassName,   // Window Class Name
                             ClientWndProc,  // Address of window procedure
                             CS_SIZEREDRAW,  // Class style
                             NULLHANDLE);    // No extra window words

  if (!success)
   {
    DisplayMessages(NULLHANDLE, "Error Registering Window Class", MSG_ERROR);
    exit(TRUE);
   }

  // --------------------------------------------------------------------------                 
  // Populate our HELPINIT structure.
  // --------------------------------------------------------------------------                 
  helpinit.cb                       = sizeof(HELPINIT);
  helpinit.ulReturnCode             = 0;
  helpinit.pszTutorialName          = 0;
  helpinit.phtHelpTable             = (PVOID)(0xffff0000 | HELP_GRAPHIC);
  helpinit.hmodAccelActionBarModule = 0;
  helpinit.idAccelTable             = 0;
  helpinit.idActionBar              = 0;
  helpinit.pszHelpWindowTitle       = "SHOWOFF - Help Display Facility";
  helpinit.hmodHelpTableModule      = 0;
  helpinit.fShowPanelId             = 0;
  helpinit.pszHelpLibraryName       = HELP_FILE;

  // --------------------------------------------------------------------------                 
  // Create a Help Instance.
  // --------------------------------------------------------------------------                 
  hwndHelp = WinCreateHelpInstance(hab, &helpinit);

  ulFrameFlags = FCF_TITLEBAR | FCF_SIZEBORDER | FCF_MINMAX |
                 FCF_SYSMENU  | FCF_MENU       | FCF_ICON   | FCF_ACCELTABLE;

  // --------------------------------------------------------------------------                 
  // Create our main frame window.                                                              
  // --------------------------------------------------------------------------              
  hwndFrame = WinCreateStdWindow(HWND_DESKTOP,    // Desktop window is parent 
                                 NULLHANDLE,      // Set Window Style to Invisible
                                 &ulFrameFlags,   // Frame control flag
                                 pszClassName,    // Window class name
                                 pszTitleBar,     // Window Title Bar
                                 NULLHANDLE,      // Client style
                                 (HMODULE)NULL,   // Resource is in .EXE file
                                 ID_FRAME,        // Frame window identifier
                                 &hwndClient);    // Client window handle

   if (!hwndFrame)
    {
     DisplayMessages(NULLHANDLE, "Error Creating Frame Window", MSG_ERROR);
     exit(TRUE);
    }

   // --------------------------------------------------------------------------                 
   // Add our program to the tasklist
   // --------------------------------------------------------------------------                 
   WinQueryWindowProcess (hwndFrame, &pid, NULL );

   swctl.hwnd          = hwndFrame;
   swctl.hwndIcon      = (HWND) NULL;
   swctl.hprog         = (HPROGRAM) NULL;
   swctl.idProcess     = pid;
   swctl.idSession     = (LONG) NULL;
   swctl.uchVisibility = SWL_VISIBLE;
   swctl.fbJump        = SWL_JUMPABLE;

   sprintf(swctl.szSwtitle, pszTitleBar);
   hswitch = WinCreateSwitchEntry (hab, &swctl);


   if (hwndHelp)
    {
     // --------------------------------------------------------------------------                 
     // Associate our Help Handle with our Frame Handle.
     // --------------------------------------------------------------------------                 
     WinAssociateHelpInstance(hwndHelp, hwndFrame);

     // --------------------------------------------------------------------------                 
     // Store the help handle in the window words for our frame window.
     // --------------------------------------------------------------------------                 
     WinSetWindowULong(hwndFrame, QWL_USER, hwndHelp);
    }

   // --------------------------------------------------------------------------                 
   // Need to ensure we have focus here....
   // --------------------------------------------------------------------------                 
   WinSetFocus(HWND_DESKTOP, hwndFrame);                

   // --------------------------------------------------------------------------                 
   // Check the OS2.INI file to see if window coordinates exist for our 
   // application.  If no entry is found we will just use the current 
   // coordinates, but we will make our window visible.
   // --------------------------------------------------------------------------                 
   if (!PrfQueryProfileData(HINI_PROFILE, APPNAME, "WindowPos", &swp, &ulLength))
    {
     // --------------------------------------------------------------------------                       
     // Obtain the screen dimensions.                                                                    
     // --------------------------------------------------------------------------                       
     lcxScreen = WinQuerySysValue (HWND_DESKTOP, SV_CXSCREEN);                                           
     lcyScreen = WinQuerySysValue (HWND_DESKTOP, SV_CYSCREEN);                                           
                                                                                                         
     lWidth  = lcxScreen / 2;
     lLength = lcyScreen / 2;

     // --------------------------------------------------------------------------                       
     // Position our frame window.                                                                       
     // --------------------------------------------------------------------------                       
     WinSetWindowPos(hwndFrame,                                                                          
                     NULLHANDLE,                                                                         
                     lWidth  - (lWidth  / 2),                                                                      
                     lLength - (lLength / 2),                                                                      
                     lWidth,                                                                                  
                     lLength,                                                                                  
                     SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_ACTIVATE);                                                                          

    }

   // --------------------------------------------------------------------------                 
   // If we have window coordinates stored for our application in the OS2.INI 
   // file then let's use em...
   // --------------------------------------------------------------------------                 
   else  
    {
     WinRestoreWindowPos(APPNAME, "WindowPos", hwndFrame);
    }

  DosCreateEventSem(NULL, &hevPause, 0L, FALSE);
  DosPostEventSem(hevPause);
  DosCreateEventSem(NULL,&hevTimeout, 0L, TRUE);

  // --------------------------------------------------------------------------                 
  // NOTE:  This is our standard PM message loop.  The purpose of this loop                     
  //        is to get and dispatch messages from the application message queue                  
  //        until WinGetMsg returns FALSE, indicating a WM_QUIT message.  In                    
  //        which case we will pop out of the loop and destroy our frame window                 
  //        along with the application message queue then we will terminate our                 
  //        anchor block removing our ability to call the Presentation Manager.                 
  // --------------------------------------------------------------------------                 
  while( WinGetMsg(hab, &qmsg, (HWND)NULL, 0, 0))
    WinDispatchMsg(hab, &qmsg);

  // --------------------------------------------------------------------------          
  // Destroy our Help Instance                                                             
  // --------------------------------------------------------------------------          
  if (hwndHelp)
   {
    WinDestroyHelpInstance(hwndHelp);
   }

  // --------------------------------------------------------------------------
  // Remove our switch entry
  // --------------------------------------------------------------------------
  WinRemoveSwitchEntry(hswitch);

  // --------------------------------------------------------------------------
  // Let's bail out, we are done!!!
  // --------------------------------------------------------------------------
  WinDestroyWindow(hwndFrame);
  WinDestroyMsgQueue(hmq);
  WinTerminate(hab);
  return(FALSE);
}

// *******************************************************************************
// FUNCTION:     ProdInfoDlgProc
//
// FUNCTION USE: Dialog Procedure used to display Product Information Dialog
//
// DESCRIPTION:  This dialog procedure is used to process the simple product
//               information dialog box.
//
// PARAMETERS:   HWND     client window handle
//               ULONG    window message
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT
//
// HISTORY:
//
// *******************************************************************************
MRESULT EXPENTRY ProdInfoDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 switch (msg)
  {
   case WM_INITDLG:
        SetTheSysMenu(hwnd, NULL);
        return FALSE;

   case WM_COMMAND:
        switch (COMMANDMSG(&msg)->cmd)
         {
          case DID_OK:
          case DID_CANCEL:
               WinDismissDlg(hwnd, TRUE);
               return FALSE;
         }
        break;
  }
 return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
}


// *******************************************************************************
// FUNCTION:     PresentationThread
//
// FUNCTION USE: Thread used to display a presentation
//
// DESCRIPTION:  This thread actually displays all of the bitmap files for a
//               presentation.  The bitmap will be displayed for the user
//               specified amount of time.  The user can configure the display
//               time via the presentation file.  The bitmaps will be displayed
//               until the global variable bStopPresentation is TRUE.
//
//
// PARAMETERS:   PSZ   the path and filename of the Presentation File.
//
// RETURNS:      VOID
//
// HISTORY:
//
// *******************************************************************************
VOID EXPENTRY PresentationThread(PSZ pszPresentation)
{
  HAB           habThread;
  HMQ           hmqThread;
  ULONG         ulCurrentlyChecked;
  ULONG         ulPostCount;
  CHAR          *szTime;
  FILE          *hfPresentation;
  HWND          hwndMenu;
  APIRET        rc;
  CHAR          szBuffer[CCHMAXPATH];
  CHAR          szPresentationLine[CCHMAXPATH];
  BOOL          bFullScreen = FALSE;
  PPRESENTATION pHeadNode;
  PPRESENTATION pCurrNode;


  // --------------------------------------------------------------------------
  // The ulDefTime value is the default amount of time that all the bitmaps
  // in the Presentation should be displayed for.
  // --------------------------------------------------------------------------
  ULONG         ulDefTime = SEM_INDEFINITE_WAIT; 

  // --------------------------------------------------------------------------
  // The ulTimeOut value can be specified after the individual bitmaps in 
  // the Presentation.  This value is the amount of time that the single 
  // bitmap this value was specified with should be displayed.  In other 
  // words if a Timeout value was specified with a bitmap the time specified
  // will override the default time value (ulDefTime).
  // --------------------------------------------------------------------------
  ULONG         ulTimeOut;

  // --------------------------------------------------------------------------
  // Initialize this thread for the Presentation Manager.
  // --------------------------------------------------------------------------
  habThread = WinInitialize(NULLHANDLE);
  hmqThread = WinCreateMsgQueue(habThread, NULLHANDLE);

  // --------------------------------------------------------------------------
  // Don't allow this thread to process a WM_QUIT
  // --------------------------------------------------------------------------
  WinCancelShutdown(hmqThread, TRUE);

  // --------------------------------------------------------------------------
  // Open the Presentation File.
  // --------------------------------------------------------------------------
  hfPresentation = fopen(pszPresentation, "r");

  if (!hfPresentation)
   {
    DisplayMessages(NULLHANDLE, "Error Loading Presentation File", MSG_ERROR);
    return; 
   }

  else
   {
    // --------------------------------------------------------------------------
    // Parse the Presentation File and insert each item into our list.
    // --------------------------------------------------------------------------
    pHeadNode = ParsePresentationFile(hfPresentation);

    if (pHeadNode)
     {
      pCurrNode = pHeadNode;

      // --------------------------------------------------------------------------
      // Close the Presentation File since we are done with it.
      // --------------------------------------------------------------------------
      fclose(hfPresentation);
      usDirection = PRES_MOVE_FORWARD;

      // --------------------------------------------------------------------------
      // Enable Presentation Menu Options and Disable Open File Options
      // so that you cannot open a bitmap, or presentation while a presentation
      // is already running.  This is Common Sense!!
      // --------------------------------------------------------------------------
      hwndMenu = UpdateMenu(hwndFrame, TRUE);

      // --------------------------------------------------------------------------
      // Check the "Forward" menuitem to indicate the direction of the 
      // the Presentation.
      // --------------------------------------------------------------------------
      WinCheckMenuItem(hwndMenu, IDM_FFORWARD, TRUE);

      ulCurrentlyChecked = IDM_FFORWARD;

      while (!bStopPresentation)
       {
        // --------------------------------------------------------------------------
        // Copy the bitmap path and options string out of the PRESENTATION  
        // structure into szPresentationLine.
        // --------------------------------------------------------------------------
        sprintf(szPresentationLine, "%s", pCurrNode->pszBitmapPath);

        // --------------------------------------------------------------------------
        // A single stand alone slash indicates that the Presentation should be 
        // terminated, so let's set the bStopPresentation flag and update the 
        // menuitems accordingly.
        // --------------------------------------------------------------------------
        if (szPresentationLine[0] == '*')
         {
          bStopPresentation = TRUE;
          UpdateMenu(hwndFrame, TRUE);

          // --------------------------------------------------------------------------
          // If we were in displaying this Presentation in "FullScreen" mode, let's
          // restore our window position and frame controls since the Presentation  
          // is complete.
          // --------------------------------------------------------------------------
          if (bFullScreen) 
           {
            WinSetWindowPos(hwndFrame, HWND_TOP, 0, 0, 0, 0, SWP_RESTORE);          
            HideControls(hwndFrame);                                                 
            bFullScreen = FALSE;
           }
         }

        // --------------------------------------------------------------------------
        // See if the user has specified a default display time value.
        // --------------------------------------------------------------------------
        else if (szPresentationLine[0] == '/')
         {
          if (!szPresentationLine[1])
           {
            // --------------------------------------------------------------------------
            // To specify an option after the bitmap path and filename the user can 
            // enter a slash followed by a number.
            //
            // For Example:
            //  
            //   If the user entered /7 this means that the user wishes to override 
            //   the default time value (usDefTime) and display this particular 
            //   bitmap file for seven seconds.  However if the user has somehow 
            //   stuck a / after the bitmap path and filename to indicate an option, 
            //   but with there is no value after the slash, this is an error so 
            //   we need to post an error message.
            // --------------------------------------------------------------------------
            DisplayMessages(NULLHANDLE, "Warning:  An error has occurred in the processing of this presentation.", MSG_EXCLAMATION);
           } // ------------------------------------> if (!szPresentationLine[1]) 

          else
           {
            // --------------------------------------------------------------------------
            // If we have a "/0" specified as the default time between bitmaps in the 
            // Presentation, this means that the user does not want a default time 
            // value, so that the bitmap will not change until the user selects the 
            // forward or rewind option from the "Display" submenu.
            // --------------------------------------------------------------------------
            if (szPresentationLine[1] == '0')
             {
              ulDefTime = SEM_INDEFINITE_WAIT;
             }

            // --------------------------------------------------------------------------
            // If the user has asked us to display this Presentation in "FullScreen" 
            // mode, let's hide our frame controls and maximize our window. 
            // --------------------------------------------------------------------------
            else if (szPresentationLine[1] == '$')
             {
              bFullScreen = TRUE;
              WinSetWindowPos(hwndFrame, HWND_TOP, 0, 0, 0, 0, SWP_MAXIMIZE);         
              HideControls(hwndFrame);
             }

            else
             {
              // --------------------------------------------------------------------------
              // Multiply by 1000 to convert the time specified into our time value.
              // --------------------------------------------------------------------------
              ulDefTime = atol(&szPresentationLine[1]) * 1000;
             }
           } // ------------------------------------> if (!szPresentationLine[1])             
         } // --------------------------------------> if (szPresentationLine[0] == '/') 

        // --------------------------------------------------------------------------
        // See if we have a display time specified for each specific bitmap.
        // --------------------------------------------------------------------------
        else   
         {
          // --------------------------------------------------------------------------
          // See if the user wants to display an individual bitmap for a specific 
          // amount of time.
          // --------------------------------------------------------------------------
          szTime = strrchr(szPresentationLine, '/');

          // --------------------------------------------------------------------------        
          // The TimeOut value overrides the ulDefTime value, so that if the user              
          // has a Presentation with a Default time of 5 seconds, but wants to display         
          // the bitmap X:\\SHOWOFF\\BEAVIS.BMP for 10 seconds, they can add a slash           
          // followed by 10, which means that all other bitmaps will be displayed for          
          // the usDefTime value (5 seconds) while the BEAVIS bitmap will be                   
          // displayed for 10 seconds.                                                         
          //                                                                                   
          // For Example:                                                                      
          //     C:\\OS2\\BITMAP\\OS2LOGO.BMP                                                  
          //     X:\\SHOWOFF\\BEAVIS.BMP /10                                                   
          //     D:\\BITMAP\\LOGO.BMP                                                          
          // --------------------------------------------------------------------------        
          if (szTime)
           {
            // --------------------------------------------------------------------------
            // Multiply by 1000 to convert the time specified into our time value.     
            // --------------------------------------------------------------------------
            ulTimeOut = atol(szTime + 1) * 1000;

            // --------------------------------------------------------------------------
            // Now let's remove the time parameter so we have only our bitmap string.
            // --------------------------------------------------------------------------
            *szTime = '\0';   
           } // --------------------------------------> if (szTime)  

          else      // if (szTime)
           {
            // --------------------------------------------------------------------------
            // If the user has not specified a time for an individual bitmap to 
            // be displayed, then we simply use the default time.
            // --------------------------------------------------------------------------
            ulTimeOut = ulDefTime;
           } // --------------------------------------> if (szTime)   

          // --------------------------------------------------------------------------
          // Load the bitmap file.
          // --------------------------------------------------------------------------
          rc = LoadBitmap(szPresentationLine);

          if (!rc)
           {
            // --------------------------------------------------------------------------
            // Show me paint the fence..., no I mean paint the window Mr. Miyagi...
            // --------------------------------------------------------------------------
            WinInvalidateRect (hwndClient, (PRECTL) NULL, FALSE);

            // --------------------------------------------------------------------------
            // Need to realize palette after painting.
            // --------------------------------------------------------------------------
            if (bPaletteSupport)
             {
              WinRealizePalette(hwndClient, hpsWindow, &ulPalEntries);
             }

            // --------------------------------------------------------------------------
            // Update the titlebar
            // --------------------------------------------------------------------------
            UpdateTitleBarText(hwndFrame, szPresentationLine, usXRes, usYRes, usBitCount);

            // --------------------------------------------------------------------------
            // Show the image for the specified length of time or until the user 
            // selects the Stop Presentation Option.
            // --------------------------------------------------------------------------
            DosResetEventSem(hevTimeout, &ulPostCount);

            // --------------------------------------------------------------------------
            // Wait on our event sem for the Timeout value
            // --------------------------------------------------------------------------
            DosWaitEventSem(hevTimeout,  ulTimeOut);

            // --------------------------------------------------------------------------
            // Reset our timeout value event sem.
            // --------------------------------------------------------------------------
            DosResetEventSem(hevTimeout, &ulPostCount);
            DosWaitEventSem(hevPause,    SEM_INDEFINITE_WAIT);
           }  // --------------------------------------> if (!rc)    


          // --------------------------------------------------------------------------
          // We got an error from the LoadBitmap Function.
          // --------------------------------------------------------------------------
          else  
           {
            if (rc == ERROR_OPENING_FILE)
             {
              strupr(szPresentationLine);                   // Convert the filename to UPPERCASE
              strcpy(szBuffer, "Unable to Load ");          // Copy Unable to Load into our Error Buffer
              strcat(szBuffer, szPresentationLine);         // Add it into the Error Buffer along with an
              strcat(szBuffer, ".");                        // Ending period........
              DisplayMessages(NULLHANDLE, szBuffer, MSG_ERROR);
             }

            // --------------------------------------------------------------------------
            // If we get an invalid Bitmap in a presentation, don't die, but instead 
            // let's note that an error occurred and try to continue the presentation
            // with the next image.  Therefore, we set the bStopPresentation flag
            // to FALSE to continue presentation processing.
            // --------------------------------------------------------------------------
            bStopPresentation = FALSE;
           } // --------------------------------------> if (!rc)     
         } // 

        switch (usDirection)
         {
          case PRES_MOVE_FORWARD:
               pCurrNode = pCurrNode->pNext;
               if (ulCurrentlyChecked == IDM_REWIND)
                {
                 ulCurrentlyChecked = IDM_FFORWARD;

                 // --------------------------------------------------------------------------          
                 // Since we are going Forward, Check the Forward Menuitem and Uncheck the 
                 // Rewind Menuitem.
                 // --------------------------------------------------------------------------          
                 WinCheckMenuItem(hwndMenu, IDM_FFORWARD, TRUE);          
                 WinCheckMenuItem(hwndMenu, IDM_REWIND, FALSE);          
                }
               break;

          case PRES_MOVE_BACKWARD:
               pCurrNode = pCurrNode->pPrev;
               if (ulCurrentlyChecked == IDM_FFORWARD)
                {
                 ulCurrentlyChecked = IDM_REWIND;

                 // --------------------------------------------------------------------------          
                 // Since we are going Backward, Check the Rewind Menuitem and Uncheck the 
                 // Forward Menuitem.
                 // --------------------------------------------------------------------------          
                 WinCheckMenuItem(hwndMenu, IDM_REWIND, TRUE);         
                 WinCheckMenuItem(hwndMenu, IDM_FFORWARD, FALSE);        
                }
               break;

          case PRES_RESTART:
               // --------------------------------------------------------------------------
               // If we are starting over reset our pointer and our direction flag.
               // --------------------------------------------------------------------------
               pCurrNode = pHeadNode;
               usDirection = PRES_MOVE_FORWARD;
               break;
         } // --------------------------------------> switch (usDirection)
       } // --------------------------------------> while (!bStopPresentation) 

      // --------------------------------------------------------------------------
      // When the Presentation is complete uncheck the currently checked menuitem.
      // --------------------------------------------------------------------------
      WinCheckMenuItem(hwndMenu, ulCurrentlyChecked, FALSE);

      // --------------------------------------------------------------------------
      // Update the menuitems accordingly so that the user can use the File 
      // Submenu menuitems, and disable the Presentation menuitems within the 
      // Display Submenu.
      // --------------------------------------------------------------------------
      UpdateMenu(hwndFrame, FALSE);

      // --------------------------------------------------------------------------
      // Free the memory required by our PRESENTATION structure.
      // --------------------------------------------------------------------------
      FreePresentationMemory(pHeadNode);
     } // --------------------------------------> if (pHeadNode)
   } // --------------------------------------> else (!hfPresentation) 

  bStopPresentation = TRUE;
  WinDestroyMsgQueue(hmqThread);

  // --------------------------------------------------------------------------
  // If the Presentation has been completed clear the drawing window.
  // --------------------------------------------------------------------------
  WinPostMsg(hwndClient, WM_COMMAND, MPFROMSHORT(IDM_EDITCLEAR), (MPARAM)NULL);
  return;
}

// *******************************************************************************
// FUNCTION:     DrawBitmapThread
//
// FUNCTION USE: Separate thread to load and display a bitmap
//
// DESCRIPTION:  This routine is the dispatched thread used to carry
//               out the loading of a bitmap after the user selects a
//               given bitmap file from the standard file dialog.
//
// PARAMETERS:   PUCHAR   the bitmap file
//
// RETURNS:      VOID
//
// HISTORY:      mm/dd/yy Created this routine
//
// *******************************************************************************
VOID EXPENTRY DrawBitmapThread(PUCHAR szBitmapFile)
{
  HAB      habThread;
  HMQ      hmqThread;
  CHAR     szError[100];
  APIRET   rc;

  LONG     lColors;
  RECTL    rcl;

  // --------------------------------------------------------------------------
  // Initialize this thread for the Presentation Manager.
  // --------------------------------------------------------------------------
  habThread = WinInitialize(NULLHANDLE);
  hmqThread = WinCreateMsgQueue(habThread, NULLHANDLE);

  // --------------------------------------------------------------------------
  // Don't allow this thread to process a WM_QUIT
  // --------------------------------------------------------------------------
  WinCancelShutdown(hmqThread, TRUE);

  // --------------------------------------------------------------------------
  // Load our Bitmap File.
  // --------------------------------------------------------------------------
  rc = LoadBitmap(szBitmapFile);

  if (!rc)
   {
    // --------------------------------------------------------------------------
    // Paint the window
    // --------------------------------------------------------------------------
    PaintBitmap(hwndClient);

    // --------------------------------------------------------------------------
    // Update the titlebar text.
    // --------------------------------------------------------------------------
    UpdateTitleBarText(hwndFrame, szBitmapFile, usXRes, usYRes, usBitCount);
   }

  else
   {
    sprintf (szError, "The LoadBitmap routine failed with rc = %d", rc);
    DisplayMessages(NULLHANDLE, szError, MSG_ERROR);
   }

  WinDestroyMsgQueue(hmqThread);
  return;
}

// *******************************************************************************
// FUNCTION:     ClientWndProc
//
// FUNCTION USE: Typical client window procedure
//
// DESCRIPTION:  This window procedure processes all messages received by the
//               client window.
//
// PARAMETERS:   HWND     client window handle
//               ULONG    window message
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  WinDefWindowProc for all unprocessed messages
//
// HISTORY:      mm/dd/yy Created this routine
//
// *******************************************************************************
MRESULT EXPENTRY ClientWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  FILEDLG          filedlg;
  HAB              hab;
  TID              tid;
  APIRET           rc;
  ULONG            ulPostCount;
  ULONG            ulCounter;
  RECTL            rcl;
  HBITMAP          hbmClip;
  PSZ              pszDevData[9] = {0, "MEMORY", 0, 0, 0, 0, 0, 0, 0};        
  SIZEL            sizel         = {0, 0};

  static HWND      hwndMenu;
  static HWND      hwndHelp;
  static CHAR      szBitmapFile[CCHMAXPATH];
  CHAR             szBuffer[CCHMAXPATH];
  CHAR             szFile[CCHMAXPATH];         
  CHAR             szPath[CCHMAXPATH];

  hab = WinQueryAnchorBlock(hwndFrame);

  switch(msg)
    {
    case WM_CREATE:
         // --------------------------------------------------------------------------      
         // Check if we need to display the Product Information Dialog Box when 
         // the application is started.
         // --------------------------------------------------------------------------      
         if (bNoLogo == TRUE)
          {
           WinSetWindowPos(hwndFrame, HWND_TOP, 0, 0, 0, 0, SWP_SHOW);
          }

         else
          {
           WinDlgBox(HWND_DESKTOP, HWND_DESKTOP, ProdInfoDlgProc, NULLHANDLE, IDD_PRODINFO, NULL);
          }

         // --------------------------------------------------------------------------      
         // Get the window handle for the application menu.
         // --------------------------------------------------------------------------      
         hwndMenu = WinWindowFromID(hwndFrame, FID_MENU);

         // --------------------------------------------------------------------------      
         // Get a window device context.
         // --------------------------------------------------------------------------      
         hdcWindow = WinOpenWindowDC(hwnd);
         if (!hdcWindow)
          {
           DisplayMessages(0, "Invalid Device Context Handle Returned from WinOpenWindowDC.", MSG_INFO);
          }

         // --------------------------------------------------------------------------      
         // Does this machine support the Palette Manager?
         // --------------------------------------------------------------------------      
         DevQueryCaps(hdcWindow, CAPS_ADDITIONAL_GRAPHICS, 1, (PLONG)&ulPalEntries);
         bPaletteSupport = (ulPalEntries&CAPS_PALETTE_MANAGER) == CAPS_PALETTE_MANAGER;

         // --------------------------------------------------------------------------      
         // Create our Window Presentation Space using a MICRO PS.
         // --------------------------------------------------------------------------      
         hpsWindow = GpiCreatePS(hab, hdcWindow, &sizel, PU_PELS | GPIT_MICRO | GPIA_ASSOC);

         if (hpsWindow == GPI_ERROR)
          {
           DisplayMessages(0, "GpiCreatePS failed creating Window Presentation Space.", MSG_EXCLAMATION);
          }

         // --------------------------------------------------------------------------                  
         // Get a memory device context handle                                                          
         // --------------------------------------------------------------------------                  
         hdcMem = DevOpenDC(hab, OD_MEMORY, "*", 8L, (PDEVOPENDATA)pszDevData, hdcWindow);         
                                                                                                     
         if (!hdcMem)                                                                                 
          {                                                                                           
           DisplayMessages(NULLHANDLE, "DevOpenDC failed creating Memory Device Context", MSG_EXCLAMATION);             
          }                                                                                           
                                                                                                         
         // --------------------------------------------------------------------------                  
         // Get a normal PS and associate it to our device context                                      
         // --------------------------------------------------------------------------                  
         hpsMem = GpiCreatePS(hab,                                     // Anchor block handle         
                              hdcMem,                                  // Memory Device Context       
                              &sizel,                                  // PS page size                
                              PU_PELS | GPIT_NORMAL | GPIA_ASSOC);     // Options                     
                                                                                                      
         if (hpsMem == GPI_ERROR)                                                                     
          {                                                                                           
           DisplayMessages(NULLHANDLE, "error creating ps", MSG_INFO);                                
          }                                                                                           
         break;

    case WM_INITMENU:                                                                              
         if (!hwndHelp) 
          {
           // --------------------------------------------------------------------------
           // Get the window handle for the help window out of the window words for
           // our frame window.
           // --------------------------------------------------------------------------
           hwndHelp = WinQueryWindowULong(hwndFrame, QWL_USER);
          }

         switch (SHORT1FROMMP (mp1))                                                               
          {                                                                                        
           case ID_EDIT_MENU:                                                                       
                // --------------------------------------------------------------------------      
                // Get the window handle of the menu                                               
                // --------------------------------------------------------------------------      
                hwndMenu = HWNDFROMMP(mp2);                                                        
                                                                                                   
                // --------------------------------------------------------------------------      
                // Enable/Disable the Edit Menuitems accordingly.                                  
                // --------------------------------------------------------------------------      
                ToggleEditMenu(hwndMenu, IDM_EDITCUT,   hbm);                                 
                ToggleEditMenu(hwndMenu, IDM_EDITCOPY,  hbm);                                 
                ToggleEditMenu(hwndMenu, IDM_EDITPASTE, hbm);                                 
                ToggleEditMenu(hwndMenu, IDM_EDITCLEAR, hbm);                                 
                return FALSE;                                                                      
          }                                                                                        
         break;                                                                                    

    case WM_REALIZEPALETTE:                                                           
         if (bPaletteSupport)                                                         
          {                                                                           
           if (WinRealizePalette(hwnd, hpsWindow, &ulPalEntries) > 0)                   
            {                                                                         
             WinInvalidateRect(hwnd, (PRECTL) NULL, FALSE);
            }                                                                         
           return FALSE;                                                              
          }                                                                           
                                                                                      
         else                                                                         
          {                                                                           
           return FALSE;                                                              
          }                                                                           
         break;                                                                       
                                                                                      
    case WM_BUTTON1DBLCLK:
         HideControls(hwndFrame);
         break;

    case HM_ERROR:
         if (hwndHelp && ((ULONG)mp1) == HMERR_NO_MEMORY)
          {
           DisplayMessages (NULLHANDLE, "Unfortunately there is not enough memory to display the Help Facility", MSG_ERROR);
           WinDestroyHelpInstance(hwndHelp);
          }
         break;

    case WM_COMMAND:
         switch (SHORT1FROMMP(mp1))
         {
          // --------------------------------------------------------------------------                        
          // The user has asked us to display a bitmap, so we will call the standard
          // file dialog to allow the user to select a bitmap.
          // --------------------------------------------------------------------------                        
          case IDM_OPENBMP:
               memset(&filedlg, 0, sizeof(filedlg));
               filedlg.cbSize          = sizeof(filedlg);
               filedlg.fl              = FDS_CENTER | FDS_OPEN_DIALOG | FDS_HELPBUTTON;
               filedlg.pszTitle        = "SHOWOFF - Display a Bitmap File";
               filedlg.pfnDlgProc      = (PFNWP)HelpButtonOpenFilterProc;                    

               strcpy(filedlg.szFullFile, EXT_BITMAP);

               if (WinFileDlg(HWND_DESKTOP, hwnd, &filedlg) && filedlg.lReturn == DID_OK)
                {
                 strcpy(szBitmapFile, filedlg.szFullFile);

                 // --------------------------------------------------------------------------                        
                 // Mark the Drive And Directory
                 // --------------------------------------------------------------------------                        
                 ParsePathAndSetDirectory(szBitmapFile, szPath, szFile);

                 // --------------------------------------------------------------------------                        
                 // Create a secondary thread to draw the bitmap, so that the user interface
                 // is not tied down while the bitmap is being displayed.  This keeps us 
                 // in line with the infamous 1/10 second rule.
                 // --------------------------------------------------------------------------                        
                 rc = DosCreateThread(&tid,
                                      (PFNTHREAD)DrawBitmapThread,
                                      (ULONG)szBitmapFile,
                                      0,
                                      8192);

                 if (rc != NULLHANDLE)
                  {
                   DisplayMessages(NULLHANDLE, "Error Dispatching Worker Thread", MSG_ERROR);
                  }
                }
               return FALSE;

          // --------------------------------------------------------------------------                        
          // The user has asked us to preview all of the bitmaps in a presentation 
          // show, so we will call the standard file dialog to allow the user to 
          // select a presentation.
          // --------------------------------------------------------------------------                        
          case IDM_PREVIEW:
               memset(&filedlg, 0, sizeof(filedlg));
               filedlg.cbSize          = sizeof(filedlg);
               filedlg.fl              = FDS_CENTER | FDS_OPEN_DIALOG;
               filedlg.pszTitle        = "SHOWOFF - Preview a Presentation";
               strcpy(filedlg.szFullFile, EXT_PRESENTATION);

               // --------------------------------------------------------------------------                        
               // If the user has selected a presentation to preview, call the preview
               // presentation routine. 
               // --------------------------------------------------------------------------                        
               if (WinFileDlg(HWND_DESKTOP, hwnd, &filedlg) && filedlg.lReturn == DID_OK)
                {
                 strcpy(szBitmapFile, filedlg.szFullFile);

                 // --------------------------------------------------------------------------  
                 // Mark the Drive And Directory                                                
                 // --------------------------------------------------------------------------  
                 ParsePathAndSetDirectory(szBitmapFile, szPath, szFile);                        


                 PreviewPresentation(hwndFrame, szBitmapFile);
                }
               return FALSE;

          // --------------------------------------------------------------------------                        
          // The user has asked us to display a presentation, so we will call the 
          // standard file dialog to allow the user to select a presentation.
          // --------------------------------------------------------------------------                        
          case IDM_STARTSHOW:
               memset(&filedlg, 0, sizeof(filedlg));
               filedlg.cbSize          = sizeof(filedlg);
               filedlg.fl              = FDS_CENTER | FDS_OPEN_DIALOG;
               filedlg.pszTitle        = "SHOWOFF - Display a Presentation";
               strcpy(filedlg.szFullFile, EXT_PRESENTATION);

               // --------------------------------------------------------------------------                        
               // If the user has selected a presentation to display, kick off a 
               // secondary thread to display all of the bitmaps in a presentation.
               // --------------------------------------------------------------------------                        
               if (WinFileDlg(HWND_DESKTOP, hwnd, &filedlg) && filedlg.lReturn == DID_OK)
                {
                 strcpy(szBitmapFile, filedlg.szFullFile);

                 // --------------------------------------------------------------------------  
                 // Mark the Drive And Directory                                                
                 // --------------------------------------------------------------------------  
                 ParsePathAndSetDirectory(szBitmapFile, szPath, szFile);                        

                 bStopPresentation = FALSE;
                                                  
                 DosPostEventSem(hevPause);
                 DosPostEventSem(hevTimeout);

                 // --------------------------------------------------------------------------                        
                 // Create a secondary thread to display the bitmaps in a presentation so 
                 // that the user interface is not tied down while the bitmap is being 
                 // displayed.  This keeps us in line with the infamous 1/10 second rule. 
                 // --------------------------------------------------------------------------                        
                 rc = DosCreateThread(&tid,
                                      (PFNTHREAD)PresentationThread,
                                      (ULONG)szBitmapFile,
                                      0,
                                      8192);

                 if (rc != NULLHANDLE)
                  {
                   DisplayMessages(NULLHANDLE, "Error Dispatching Worker Thread", MSG_ERROR);
                  }
                }
               return FALSE;

          // --------------------------------------------------------------------------                        
          // The Clipboard Cut Operation                                                                       
          // --------------------------------------------------------------------------                        
          case IDM_EDITCUT:                                                                                    
               if (hbm != NULLHANDLE)                                                                     
                {                                                                                              
                 hbmClip = DuplicateBitmap(hbm, hpsMem);                                                 
                 if (hbmClip == GPI_ERROR)                                                               
                  {                                                                                      
                   DisplayMessages(NULLHANDLE, "Error duplicating bitmap.", MSG_EXCLAMATION);            
                  }                                                                                      
                                                                                                         
                 else                                                                                    
                  {                                                                                      
                   rc = PutBitmapInClipboard(hbmClip);                                                   
                   if (!rc)                                                                                   
                    {                                                                                         
                     DisplayMessages(NULLHANDLE, "Error putting bitmap in Clipboard.", MSG_EXCLAMATION);      
                    }                                                                                         
                  }

                 // --------------------------------------------------------------------------                        
                 // Reset the Title Bar Window 
                 // --------------------------------------------------------------------------                        
                 WinSetWindowText(hwndFrame, pszTitleBar);

                 // --------------------------------------------------------------------------                        
                 // Remove our Bitmap and Paint the Window.
                 // --------------------------------------------------------------------------                        
                 WinPostMsg(hwndClient, WM_COMMAND, MPFROMSHORT(IDM_EDITCLEAR), (MPARAM)NULL);
                }                                                                                              

               else
                {
                 DisplayMessages(NULLHANDLE, "There is no bitmap to currently being displayed.", MSG_EXCLAMATION);
                }
               return FALSE;                                                                                   
                                                                                                               
          // --------------------------------------------------------------------------                        
          // The Clipboard Copy Operation                                                                      
          // --------------------------------------------------------------------------                        
          case IDM_EDITCOPY:                                                                                   
               hbmClip = DuplicateBitmap(hbm, hpsMem);                                                            
               if (hbmClip == GPI_ERROR)                                                                       
                {                                                                                              
                 DisplayMessages(NULLHANDLE, "Error duplicating bitmap.", MSG_EXCLAMATION);                    
                }                                                                                              
                                                                                                               
               else                                                                                            
                {                                                                                              
                 rc = PutBitmapInClipboard(hbmClip);                                                           
                 if (!rc)                                                                                      
                  {                                                                                            
                   DisplayMessages(NULLHANDLE, "Error putting bitmap in Clipboard.", MSG_EXCLAMATION);         
                  }                                                                                            
                }                                                                                              
               return FALSE;                                                                                   
                                                                                                               
          // --------------------------------------------------------------------------                        
          // The Clipboard Paste Operation                                                                     
          // --------------------------------------------------------------------------                        
          case IDM_EDITPASTE:                                                                                  
               hbm = GetBitmapFromClipboard(hpsMem);                                                            

               GpiSetBitmap(hpsMem, hbm);
               GpiSetBitmap(hpsWindow, hbm);
               PaintBitmap(hwnd);
               return FALSE;                                                                                   
                                                                                                               
          // --------------------------------------------------------------------------
          // The user has asked us to create a presentation.
          // --------------------------------------------------------------------------
          case IDM_CREATE:
               WinDlgBox(HWND_DESKTOP, HWND_DESKTOP, CreatePresentationDlgProc, NULLHANDLE, IDD_CREATEPRES, NULL);
               return FALSE;

          // --------------------------------------------------------------------------
          // The user has asked us to edit a presentation
          // --------------------------------------------------------------------------
          case IDM_EDITPRES:
               memset(&filedlg, 0, sizeof(filedlg));
               filedlg.cbSize          = sizeof(filedlg);
               filedlg.fl              = FDS_CENTER | FDS_OPEN_DIALOG;
               filedlg.pszTitle        = "SHOWOFF - Edit a Presentation";
               strcpy(filedlg.szFullFile, EXT_PRESENTATION);

               // --------------------------------------------------------------------------                        
               // If the user has selected a presentation to preview, call the preview
               // presentation routine. 
               // --------------------------------------------------------------------------                        
               if (WinFileDlg(HWND_DESKTOP, hwnd, &filedlg) && filedlg.lReturn == DID_OK)
                {
                 strcpy(szBitmapFile, filedlg.szFullFile);
                 WinDlgBox(HWND_DESKTOP, HWND_DESKTOP, CreatePresentationDlgProc, NULLHANDLE, IDD_CREATEPRES, (PVOID)szBitmapFile);
                }
               return FALSE;
          
          // --------------------------------------------------------------------------
          // The user has selected the General Help Menuitem.
          // --------------------------------------------------------------------------
          case IDM_HELPGEN:          
               if (hwndHelp)
                WinSendMsg(hwndHelp, HM_DISPLAY_HELP, MPFROMSHORT(PANEL_GENERAL_HELP), 0L);
               return FALSE;

          // --------------------------------------------------------------------------
          // The user has selected the Keys Help Menuitem.
          // --------------------------------------------------------------------------
          case IDM_HELPKEYS:         
               if (hwndHelp)
                WinSendMsg(hwndHelp, HM_DISPLAY_HELP, MPFROMSHORT(PANEL_KEYS_HELP), 0L);
               return FALSE;

          // --------------------------------------------------------------------------
          // The user has asked us to lockup the system.
          // --------------------------------------------------------------------------
          case IDM_LOCKUP:
               WinLockupSystem(hab);
               return FALSE;

          // --------------------------------------------------------------------------
          // The user has asked us to remove the frame controls.
          // --------------------------------------------------------------------------
          case IDM_FULLSCN:
               HideControls(hwndFrame);
               return FALSE;

          // --------------------------------------------------------------------------
          // The user has asked us to display the User Settings Dialog Box.
          // --------------------------------------------------------------------------
          case IDM_CONFIGURE:
               WinDlgBox(HWND_DESKTOP, hwndFrame, ConfigureUserDlgProc, NULLHANDLE, IDD_CONFIGURE, NULL);
               return FALSE;
          
          // --------------------------------------------------------------------------
          // The user has asked to see the Product Information Dialog Box.
          // --------------------------------------------------------------------------
          case IDM_PRODINFO:
               WinDlgBox(HWND_DESKTOP, HWND_DESKTOP, ProdInfoDlgProc, NULLHANDLE, IDD_PRODINFO, NULL);
               return FALSE;

          // --------------------------------------------------------------------------
          // The user has asked us to display the screen resolution supported by 
          // the video device installed on the user's machine.
          // --------------------------------------------------------------------------
          case IDM_DEVICEINF:
               QueryDeviceInfoStatus(hwndFrame);
               return FALSE;

          // --------------------------------------------------------------------------
          // The user has asked us to display a bitmap on the OS/2 desktop.
          // --------------------------------------------------------------------------
          case IDM_DESKTOP:
               WinChangeDesktopBitmap(hwndFrame, NULL);
               return FALSE;

          case IDM_EDITCLEAR:
               if (bPaletteSupport)                           
                {
                 // --------------------------------------------------------------------------               
                 // Delete our old Palette                                                                  
                 // --------------------------------------------------------------------------               
                 GpiSelectPalette(hpsMem, NULLHANDLE);
                 GpiSelectPalette(hpsWindow, NULLHANDLE);
                 GpiDeletePalette(hpal);                                                                     
                                                                                                           
                 if (hbm) 
                  {
                   GpiDeleteBitmap(hbm);                 
                   hbm = NULLHANDLE;                     

                   ulPostCount = WinRealizePalette(hwndFrame, hpsWindow, &ulPalEntries); 
                   while (ulPalEntries > 0) 
                    {
                     WinRealizePalette(hwndFrame, hpsWindow, &ulPalEntries);  
                    }
                  }
                 WinInvalidateRect (hwnd, (PRECTL) NULL, FALSE);                                             
                }

               GpiDeleteBitmap(hbm);
               hbm = NULLHANDLE;

               // --------------------------------------------------------------------------
               // Reset the window title text...
               // --------------------------------------------------------------------------
               WinSetWindowText(hwndFrame, pszTitleBar);
               return FALSE;

          case IDM_RESTART:
               usDirection = PRES_RESTART;
               DosPostEventSem(hevTimeout);
               DosPostEventSem(hevPause);
               break;

          case IDM_PAUSE:
               // --------------------------------------------------------------------------
               // If the pause flag is ON and the user selects the Pause menuitem 
               // again, then stop pausing and post our event sem.
               // --------------------------------------------------------------------------
               if (bPaused)   
                {
                 hwndMenu = WinWindowFromID(hwndFrame, FID_MENU);
                 WinSetMenuItemText(hwndMenu, IDM_PAUSE, IDS_PAUSE_OFF);
                 DosPostEventSem(hevPause);
                 bPaused = FALSE;
                }

               // --------------------------------------------------------------------------
               // If the pause flag is OFF and the user selects the Pause menuitem 
               // then start pausing and check the Pause menuitem.
               // --------------------------------------------------------------------------
               else          
                {
                 hwndMenu = WinWindowFromID(hwndFrame, FID_MENU);
                 WinSetMenuItemText(hwndMenu, IDM_PAUSE, IDS_PAUSE_ON);   
                 DosResetEventSem(hevPause, &ulPostCount);
                 bPaused = TRUE;
                }
               return FALSE;

          case IDM_STOPSHOW:
               if (bStopPresentation == FALSE)
                {
                 bStopPresentation = TRUE;
                 DosPostEventSem(hevTimeout);
                 DosPostEventSem(hevPause);
                }
               return FALSE;

          case IDM_REWIND:
               usDirection = PRES_MOVE_BACKWARD;
               DosPostEventSem(hevTimeout);
               DosPostEventSem(hevPause);
               return FALSE;

          case IDM_FFORWARD:
               usDirection = PRES_MOVE_FORWARD;
               DosPostEventSem(hevTimeout);
               DosPostEventSem(hevPause);
               return FALSE;

          // --------------------------------------------------------------------------
          // The user has asked us to save the current window size and position.  We
          // will store the window coordinates in the OS2.INI via WinStoreWindowPos.
          // --------------------------------------------------------------------------
          case IDM_SAVEWND:
               rc = DisplayMessages(NULLHANDLE, "Save Current Window Size and Position?", MSG_WARNING);
               if (rc == MBID_YES)
                {
                 DosBeep(300, 100);
                 WinStoreWindowPos(APPNAME, "WindowPos", hwndFrame);
                 DisplayMessages(NULLHANDLE, "Window values Saved in OS2.INI", MSG_INFO);
                }
               return FALSE;

          // --------------------------------------------------------------------------
          // The user has asked us to say goodbye, so let's exit the application.
          // --------------------------------------------------------------------------
          case IDM_EXIT:
               WinPostMsg(hwnd, WM_CLOSE, NULL, NULL);
               return FALSE;

          default:
               return WinDefWindowProc(hwnd, msg, mp1, mp2 );
         }
        break;

    case WM_PAINT:
         PaintBitmap(hwnd);
         return FALSE;

    case WM_DESTROY:
         GpiDeleteBitmap(hbm);
         GpiDestroyPS(hpsMem);
         GpiDestroyPS(hpsWindow);
         DevCloseDC(hdcMem);
         DevCloseDC(hdcWindow);
         break;

    case WM_CLOSE:
         if (bPaletteSupport)
          {
           // --------------------------------------------------------------------------
           // Delete our current palette.
           // --------------------------------------------------------------------------
           GpiDeletePalette(hpal);

           // --------------------------------------------------------------------------
           // Restore our old Palette
           // --------------------------------------------------------------------------
           GpiSelectPalette(hpsWindow, hpalOld);

           // --------------------------------------------------------------------------
           // Realize our old Palette.
           // --------------------------------------------------------------------------
           WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDM_EDITCLEAR), (MPARAM)NULL);
          }


        // --------------------------------------------------------------------------
        // See if the user wants to save the window position on exit?
        // --------------------------------------------------------------------------
        ulCounter = PrfQueryProfileInt(HINI_USER, APPNAME, KEY_WNDEXIT, 0);            
                                                                                  
        if (ulCounter == TRUE)                                                         
         {                                                                        
          WinStoreWindowPos(APPNAME, "WindowPos", hwndFrame);                                                                                    
         }                                                                        

         WinPostMsg(hwnd, WM_QUIT, 0L, 0L); 
         break;
     
    // --------------------------------------------------------------------------
    // Allow all other messages to be handled by the default window procedure.
    // --------------------------------------------------------------------------
    default:
        break; 
   }
 return WinDefWindowProc(hwnd, msg, mp1, mp2);
}

// *******************************************************************************
// FUNCTION:     PaintBitmap
//
// FUNCTION USE: Called by WM_PAINT processing.
//
// DESCRIPTION:  This function is called by the WM_PAINT message in our 
//               client window procedure.  This function will bitblit the loaded
//               bitmap from our memory PS to our window PS.
//
// PARAMETERS:   HWND     window handle
//
// RETURNS:      HBITMAP
//
// HISTORY:      mm/dd/yy Created this routine
//
// *******************************************************************************
HBITMAP EXPENTRY PaintBitmap(HWND hwnd)
{
 HAB              hab;
 HPS              hps;
 RECTL            rcl;
 POINTL           aptl[5];
 USHORT           usCounter;
 APIRET           rc;
 LONG             lHits;
 LONG             lValue;

 BITMAPINFOHEADER bmapinfo;
 CHAR szBuffer[CCHMAXPATH];

 // --------------------------------------------------------------------------
 // Get an anchor block handle.
 // --------------------------------------------------------------------------
 hab = WinQueryAnchorBlock(hwndFrame);

 // --------------------------------------------------------------------------
 // On the first invocation we do not have a valid bitmap handle
 // --------------------------------------------------------------------------
 if (!hbm)
  {
   hps = WinBeginPaint(hwnd, NULLHANDLE, &rcl);
   GpiErase (hps);
   WinFillRect(hps, &rcl, SYSCLR_WINDOW);
   WinEndPaint(hps);
  }


 else
  {
   WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP, SPTR_WAIT, FALSE));

   // --------------------------------------------------------------------------
   // Start paint processing
   // --------------------------------------------------------------------------
   hps = WinBeginPaint(hwnd, NULLHANDLE, &rcl);
   WinQueryWindowRect(hwnd, &rcl);

   GpiErase(hpsWindow);             

   // --------------------------------------------------------------------------
   // Set our background mix
   // --------------------------------------------------------------------------
   rc = GpiSetBackMix(hps, BM_OVERPAINT);
   if (rc == GPI_ERROR)
    {
     DisplayMessages(NULLHANDLE, "GpiSetBackMix Error", MSG_EXCLAMATION);
     return rc;
    }

   // --------------------------------------------------------------------------
   // Get a BITMAPINFOHEADER Structure for the bitmap
   // --------------------------------------------------------------------------
   rc = GpiQueryBitmapParameters(hbm, &bmapinfo);
   if (rc == GPI_ERROR)
    {
     DisplayMessages(NULLHANDLE, "GpiQueryBitmapParameters Error", MSG_EXCLAMATION);
     return rc;
    }

   // --------------------------------------------------------------------------
   // This is the Source Rectangle in Device Coordinates
   // --------------------------------------------------------------------------
   aptl[2].x = 0;
   aptl[2].y = 0;
   aptl[3].x = bmapinfo.cx;
   aptl[3].y = bmapinfo.cy;

   // --------------------------------------------------------------------------
   // Query our options stored in the OS2.INI file to see how the user wants
   // the bitmap displayed.  The bitmap can be shown actual size or stretched
   // to the size of the client window.
   // --------------------------------------------------------------------------
   lValue = PrfQueryProfileInt(HINI_USER, APPNAME, KEY_STRETCH, 0);

   // --------------------------------------------------------------------------
   // For the target rectangle, let's check to see if the user has asked us
   // to stretch the bitmap to the size of the window.
   // --------------------------------------------------------------------------
   if (lValue == 1)
    {
     // --------------------------------------------------------------------------
     // If we are displaying the bitmap as the actual size, draw it at 
     // the coordinates 1,1.
     // --------------------------------------------------------------------------
     aptl[0].x = 1;
     aptl[0].y = 1;
     aptl[1].x = bmapinfo.cx;
     aptl[1].y = bmapinfo.cy;
    }

   else
    {
     // --------------------------------------------------------------------------
     // Otherwise we are gonna stretch this bitmap to the size of the window.
     // --------------------------------------------------------------------------
     aptl[0].x = rcl.xLeft;
     aptl[0].y = rcl.yBottom;
     aptl[1].x = rcl.xRight;
     aptl[1].y = rcl.yTop;
    }

   if ((!hpsWindow) || (!hpsMem))
    {
     DisplayMessages(ERROR_BOGUS_HPS, NULLHANDLE, MSG_EXCLAMATION);
    }

   // --------------------------------------------------------------------------
   // Copy the bitmap from our memory PS to our window PS
   // --------------------------------------------------------------------------
   lHits = GpiBitBlt(hpsWindow,     // Target presentation space handle
                     hpsMem,        // Source presentation space handle
                     4L,            // Number of Points
                     aptl,          // Array of Points
                     ROP_SRCCOPY,   // Mixing function
                     BBO_IGNORE);   // Options

   if (lHits == GPI_ERROR)
    {
     DisplayMessages(NULLHANDLE, "Error with GpiBitBlt", MSG_EXCLAMATION);
    }

   WinEndPaint(hps);
   WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, FALSE));
  }
 return (HBITMAP)NULL;
}

// *******************************************************************************
// FUNCTION:     LoadBitmap
//
// FUNCTION USE: Loads a Bitmap for Display
//
// DESCRIPTION:  This routine will load a bitmap from a bitmap file 
//
// PARAMETERS:   PSZ        the file of the bitmap to display
//
// RETURNS:      FALSE      Success
//               OTHERWISE  Error code defined in SHERROR.H
//               
//
// HISTORY:      mm/dd/yy Created this routine
//
// *******************************************************************************
ULONG LoadBitmap(PSZ pszFilename)
{
  PBITMAPFILEHEADER2      pbmpfilehdr2;
  PBITMAPFILEHEADER       pbmpfilehdr;
  PBITMAPARRAYFILEHEADER  pbafh;
  PBITMAPARRAYFILEHEADER  pbafhFirst;
  PBITMAPINFOHEADER       pbmpinfohdr;
  BITMAPINFOHEADER2       bmp2;             
  FILESTATUS3             filestatus3;

  APIRET                  rc;
  ULONG                   ulAction;
  PCH                     pvBitmap;
  SIZEL                   sizel;
  HFILE                   hfBitmap;
  HAB                     habRet;
  BOOL                    bArrayMatchFound = FALSE;
  BOOL                    bDone = FALSE;
  USHORT                  usBits;
  PBYTE                   palette;
  RGB2                    argb2[256];

  LONG                    lCounter;
  LONG                    cBitCount        = 0;
  LONG                    cBitCountBestFit = 0;

  PSZ                     pszDevData[9] = {0, "MEMORY", 0, 0, 0, 0, 0, 0, 0};
  CHAR                    szBuffer[CCHMAXPATH];


  // --------------------------------------------------------------------------
  // Initialize our PS page size.
  // --------------------------------------------------------------------------
  sizel.cx = 0;
  sizel.cy = 0;

  // --------------------------------------------------------------------------
  // Get our anchor block handle.
  // --------------------------------------------------------------------------
  habRet = WinQueryAnchorBlock(hwndFrame);

  // --------------------------------------------------------------------------
  // Set the busy pointer.
  // --------------------------------------------------------------------------
  WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP, SPTR_WAIT, FALSE));

  // --------------------------------------------------------------------------
  // Open the bitmap file.
  // --------------------------------------------------------------------------
  rc =  DosOpen(pszFilename,                  // Valid Path and Filename
                &hfBitmap,                    // File handle
                &ulAction,                    // Action taken Returned 1=Existed 2=Created
                0,                            // Initial File Size
                0,                            // File Attributes
                OPEN_ACTION_OPEN_IF_EXISTS,   // Just open it if it exists
                OPENMODE_FLAGS,               // File open mode flags
                0);                           // Extended Attributes

  if (rc)
   {
    return ERROR_OPENING_FILE;
   }


  // --------------------------------------------------------------------------
  // Query the file information.
  // --------------------------------------------------------------------------
  rc = DosQueryFileInfo(hfBitmap,             // File Handle
                        FIL_STANDARD,         // Level Information
                        (PBYTE)&filestatus3,  // File Information Buffer
                        sizeof(filestatus3)); // File Information Buffer Size

  if (rc)
   {
    DosClose(hfBitmap);
    return ERROR_OPENING_FILE;
   }

  // --------------------------------------------------------------------------
  // Allocate memory for the bitmap.
  // --------------------------------------------------------------------------
  rc = DosAllocMem((PVOID)&pvBitmap,
                   filestatus3.cbFile,
                   PAG_READ | PAG_WRITE | PAG_COMMIT);

  if (rc)
   {
    DosClose(hfBitmap);
    return ERROR_ALLOC_MEM;
   }

  // --------------------------------------------------------------------------
  // Read the bitmap into memory.
  // --------------------------------------------------------------------------
  rc = DosRead(hfBitmap,
               (PVOID)pvBitmap,
               filestatus3.cbFile,
               &ulAction);

  if (rc)
   {
    DosClose(hfBitmap);
    return ERROR_READING_FILE;
   }

 // --------------------------------------------------------------------------
 // Obtain the bitcount that this display device will support, and store
 // it in cBitCount.
 // --------------------------------------------------------------------------
 DevQueryCaps ((HDC) GpiQueryDevice (hpsWindow), CAPS_COLOR_BITCOUNT, 1L, &cBitCount);

 // --------------------------------------------------------------------------
 // See if our bitmap file is of the BFT_BITMAPARRAY type.  If it is
 // lets find the best match.
 // --------------------------------------------------------------------------
 pbmpfilehdr = (PBITMAPFILEHEADER) pvBitmap;

 if (pbmpfilehdr->usType == BFT_BITMAPARRAY)
  {
   pbafh = (PBITMAPARRAYFILEHEADER) pvBitmap;

   // --------------------------------------------------------------------------
   // Store away the first bitmap
   // --------------------------------------------------------------------------
   pbafhFirst = pbafh;

   do
    {
     // --------------------------------------------------------------------------
     // Look into the bitmap info header structure
     // --------------------------------------------------------------------------
     pbmpfilehdr = &pbafh->bfh;
     pbmpinfohdr     = &pbmpfilehdr->bmp;

     if (pbmpinfohdr->cbFix == sizeof (BITMAPINFOHEADER))
      {
       if (pbmpinfohdr->cBitCount == cBitCount)
        {
         bArrayMatchFound = TRUE;
        }

       else
        {
         // --------------------------------------------------------------------------
         // If the bitmap has fewer colors than the device supports, yet more colors
         // than the current best fit...
         // --------------------------------------------------------------------------
         if ((pbmpinfohdr->cBitCount < cBitCount) && (pbmpinfohdr->cBitCount > cBitCountBestFit))
          {
           cBitCountBestFit = pbmpinfohdr->cBitCount;
           pbafhFirst = pbafh;
          }
        }
      }

     else
      {
       if (((PBITMAPINFOHEADER2) pbmpinfohdr)->cBitCount == cBitCount)
        {
         bArrayMatchFound = TRUE;
        }

       else
        {
         // --------------------------------------------------------------------------
         // If the bitmap has fewer colors than the device supports, yet more colors
         // than the current best fit...
         // --------------------------------------------------------------------------
         if ((pbmpinfohdr->cBitCount < cBitCount) && (pbmpinfohdr->cBitCount > cBitCountBestFit))
          {
           cBitCountBestFit = pbmpinfohdr->cBitCount;
           pbafhFirst = pbafh;
          }
        }
      }


     // --------------------------------------------------------------------------
     // We are done if we found a match or the offset to the next bitmap is zero.
     // --------------------------------------------------------------------------
     bDone = ((pbafh->offNext == 0) || bArrayMatchFound);

     if (!bDone)
      {
       pbafh = (PVOID)(pvBitmap + pbafh->offNext);
      }
    } while (!bDone );

   if (!bArrayMatchFound)
    {
     pbafh = pbafhFirst;
    }

   pbmpfilehdr = (PBITMAPFILEHEADER) &pbafh->bfh;
  }

 pbmpfilehdr2 = (PBITMAPFILEHEADER2) pbmpfilehdr;

 // --------------------------------------------------------------------------
 // Check to see if our bitmap is a PM 1.X bitmap?
 // --------------------------------------------------------------------------
 if (pbmpfilehdr->bmp.cbFix == sizeof (BITMAPINFOHEADER))
  {
   sizel.cx = pbmpfilehdr->bmp.cx;
   sizel.cy = pbmpfilehdr->bmp.cy;
   palette  = (PBYTE)&(pbmpfilehdr->bmp) + pbmpfilehdr->bmp.cbFix;
   usBits   = pbmpfilehdr->bmp.cPlanes * pbmpfilehdr->bmp.cBitCount;

   if (usBits == 4 || usBits == 8)
    {
     RGB *argb = (RGB *)palette;

     // --------------------------------------------------------------------------
     // Convert the RGB formats.
     // --------------------------------------------------------------------------
     for (lCounter = 0; lCounter < (1 << usBits); lCounter++)
      {
       argb2[lCounter].bBlue     = argb[lCounter].bBlue;
       argb2[lCounter].bGreen    = argb[lCounter].bGreen;
       argb2[lCounter].bRed      = argb[lCounter].bRed;
       argb2[lCounter].fcOptions = NULLHANDLE;
      }

     // --------------------------------------------------------------------------
     // Create our Palette.
     // --------------------------------------------------------------------------
     hpal = GpiCreatePalette(habRet,              // Anchor block handle
                             NULLHANDLE,          // No options
                             LCOLF_CONSECRGB,     // Format of entries in table is array of RGB values
                             1 << usBits,         // Count of elements in the table
                             (PULONG) argb2);     // The application data table

     if (!hpal)
      {
       DisplayMessages(NULLHANDLE, "An error occurred creating a palette", MSG_EXCLAMATION);
      } // if (!hpal)
    }
  }

 else
  {                                     
   sizel.cx = pbmpfilehdr2->bmp2.cx;
   sizel.cy = pbmpfilehdr2->bmp2.cy;
   palette  = (PBYTE)&(pbmpfilehdr2->bmp2) + pbmpfilehdr2->bmp2.cbFix;
   usBits   = pbmpfilehdr2->bmp2.cPlanes * pbmpfilehdr2->bmp2.cBitCount;

   if (usBits == 4 || usBits == 8)
    {
     // --------------------------------------------------------------------------
     // Create our Palette.
     // --------------------------------------------------------------------------
     hpal = GpiCreatePalette(habRet,              // Anchor block handle
                             NULLHANDLE,          // No options
                             LCOLF_CONSECRGB,     // Format of entries in table is array of RGB values
                             1 << usBits,         // Count of elements in the table
                             (PULONG) palette);   // The application data table

     if (!hpal)
      {
       DisplayMessages(NULLHANDLE, "An error occurred creating a palette", MSG_EXCLAMATION);
      } // if (!hpal)
    }
  }

 // --------------------------------------------------------------------------
 // If we have a palette handle then select it into our Presentation Space.
 // --------------------------------------------------------------------------
 if (hpal)
  {
   hpalOld = GpiSelectPalette(hpsMem, hpal);
   if (hpalOld == PAL_ERROR)
    {
     DisplayMessages(NULLHANDLE, "An error occurred selecting the palette into hpsMem", MSG_EXCLAMATION);
    }

   hpalOld = GpiSelectPalette(hpsWindow, hpal);
   if (hpalOld == PAL_ERROR)
    {
     DisplayMessages(NULLHANDLE, "An error occurred selecting the palette into hpsWindow", MSG_EXCLAMATION);
    }
  }

 // --------------------------------------------------------------------------
 // Call GpiCreateBitmap to get a bitmap handle
 // --------------------------------------------------------------------------
 hbm = GpiCreateBitmap (hpsMem,                                 // Presentation Space Handle
                        &pbmpfilehdr2->bmp2,                    // Pointer to Bitmap Information Header
                        CBM_INIT,                               // Options
                        pvBitmap + pbmpfilehdr2->offBits,       // Buffer Address                           
                        (PBITMAPINFO2) &pbmpfilehdr2->bmp2);    // Pointer to bitmap info table structure   

 if (hbm == GPI_ERROR)
  {
   DisplayMessages(NULLHANDLE, "An error occurred creating the bitmap to be displayed", MSG_EXCLAMATION);
   DosFreeMem (pvBitmap);
   return ERROR_CREATING_BITMAP;
  }

 // --------------------------------------------------------------------------
 // Associate the created bitmap with the corresponding presentation space
 // --------------------------------------------------------------------------
 GpiSetBitmap (hpsMem, hbm);


 memset(&bmp2, 0, sizeof(BITMAPINFOHEADER2));
 bmp2.cbFix = sizeof(BITMAPINFOHEADER2);

 // --------------------------------------------------------------------------
 // Get the complete Bitmap Info Header structure for the bitmap
 // --------------------------------------------------------------------------
 GpiQueryBitmapInfoHeader(hbm, &bmp2);
 usBitCount = bmp2.cBitCount;
 usXRes     = bmp2.cx;
 usYRes     = bmp2.cy;

 // --------------------------------------------------------------------------
 // Free our memory and close the bitmap file.
 // --------------------------------------------------------------------------
 DosFreeMem (pvBitmap);
 DosClose(hfBitmap);

 // --------------------------------------------------------------------------
 // Before we pop outta here lets realize our palette!
 // --------------------------------------------------------------------------
 WinRealizePalette(hwndFrame, hpsWindow, &ulPalEntries);
 return FALSE;
}

// *******************************************************************************               
// FUNCTION:     HelpButtonOpenFilterProc                                                        
//                                                                                               
// FUNCTION USE: Used to Process the Help Button on a Standard File Dialog Box                   
//                                                                                               
// DESCRIPTION:  This procedure is dispatched by the processing of the standard               
//               file dialog.  This routine allows us to display help information
//               via the help pushbutton from the standard file dialog.  If an
//               OS/2 user doesn't know how to use the SFD by now then they
//               really are in deep doody....
//                                                                                               
// PARAMETERS:   HWND     client window handle                                                  
//               ULONG    window message                                                        
//               MPARAM   first message parameter                                               
//               MPARAM   second message parameter                                              
//                                                                                              
// RETURNS:      MRESULT                                                                        
//                                                                                              
// HISTORY:                                                                                     
//                                                                                              
// *******************************************************************************              
MRESULT EXPENTRY HelpButtonOpenFilterProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)                                                 
{                                                                                        
 HWND hwndHelp;

 if(msg == WM_HELP)                                                                
  {                                                                                     
   hwndHelp = WinQueryWindowULong(hwndFrame, QWL_USER);
   WinSendMsg(hwndHelp, HM_DISPLAY_HELP, MPFROMSHORT(PANEL_OPEN_BUTTON), 0L);
   return FALSE;                                                            
  }                                                                                     
                                                                                         
 return WinDefFileDlgProc(hwnd, msg, mp1, mp2 ) ;                                 
}  // End of HelpButtonOpenFilterProc                                                   
