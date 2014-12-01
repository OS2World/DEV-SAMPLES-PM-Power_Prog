// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป 
// บ  FILENAME:      PMSCREEN.C                                       mm/dd/yy     บ 
// บ                                                                               บ 
// บ  DESCRIPTION:   Primary Source File for PMSCREEN/Chap17 Sample Program        บ 
// บ                                                                               บ 
// บ  NOTES:         This file contains the source code for the PMSCREEN           บ 
// บ                 executable.  PMSCREEN is a simple screen capture utility      บ 
// บ                 designed to illustrate the power of PM hooks.                 บ   
// บ                                                                               บ 
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ 
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ 
// บ                                                                               บ 
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ 
// บ                                                                               บ 
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ 

#define INCL_WIN
#define INCL_GPI
#define INCL_DOS

#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "pmscreen.h"                    // Resource identifiers                    
#include "pmhooks.h"                     // Hook Function Prototypes                            
#include "shcommon.h"                    // Common Function Prototypes
#include "gdata.h"                       // Global Data
#include "pmscreen.fnc"                  // Function Prototypes                     

// --------------------------------------------------------------------------     
// Miscellaneous global variables                                         
// --------------------------------------------------------------------------     
BOOL   bCurrentState = FALSE;        // Check to see if CTRL-ALT-DEL is enabled 
ULONG  ulBmpFlags = DBM_NORMAL;      // Draw bitmap flags.  The default is normal.
HWND   hwndFrame;                    // For simplicity, make this global

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
int main (VOID)                                                             
{
  HAB      hab;                              // Handle to Anchor Block                          
  HMQ      hmq;                              // Handle to Message Queue                                        
  QMSG     qmsg;
  BOOL     rc;
  LONG     lcxScreen;
  LONG     lcyScreen;
  HMODULE  hmodule;
  ULONG    flCreate = FCF_STANDARD;          // Frame Window Control Flags                         
  HWND     hwndClient;                       // Handle to Client Window                 

  // --------------------------------------------------------------------------                             
  // Initialize and get an anchor block handle.                                                                         
  // --------------------------------------------------------------------------                             
  hab = WinInitialize(NULLHANDLE);
  if(!hab)
  {
    DosBeep(60, 100);
    exit(TRUE);
  }

  // --------------------------------------------------------------------------                             
  // Create our application message queue.                                                                         
  // --------------------------------------------------------------------------                             
  hmq = WinCreateMsgQueue(hab, NULLHANDLE);
  if(!hmq)
  {
    DosBeep(60, 100);
    WinTerminate(hab);
    exit(TRUE);
  }

  // --------------------------------------------------------------------------                             
  // Load our hook module, PMHOOKS.                                                                                         
  // --------------------------------------------------------------------------                             
  rc = DosLoadModule(NULL, 0, MODULE_HOOKS, &hmodule);
  if (rc) 
   {
    DisplayMessages(NULLHANDLE, "Error Loading Module", MSG_ERROR);
    exit(TRUE);
   } 

  // --------------------------------------------------------------------------                 
  // Register our private class.                                                               
  // --------------------------------------------------------------------------                 
  rc = WinRegisterClass(hab,                // Anchor block handle
                        CLASSNAME,          // Window class name            
                        ClientWndProc,      // Address of client window procedure  
                        CS_SIZEREDRAW,      // Class Style                 
                        NULLHANDLE);        // No extra window words       

  if (!rc)
  {
    DisplayMessages(NULLHANDLE, "Error Registering Window Class", MSG_ERROR);  
    exit(TRUE);
  }

  // --------------------------------------------------------------------------                     
  // Create our frame window.                                                                       
  // --------------------------------------------------------------------------                     
  hwndFrame = WinCreateStdWindow(HWND_DESKTOP,       // Desktop Window is the parent
                                 NULLHANDLE,         // Initial frame window style                  
                                 &flCreate,          // Frame control flags           
                                 CLASSNAME,          // Window class name            
                                 TITLEBAR,           // Window Title Bar Text
                                 NULLHANDLE,         // Initial client window style 
                                 (HMODULE)NULL,      // PM resource are in the executable
                                 ID_FRAME,           // Frame window identifier      
                                 &hwndClient);       // Client window handle         

  if (!hwndFrame)
  {
   DisplayMessages(NULLHANDLE, "Error creating frame window", MSG_ERROR);       
   exit(TRUE);
  }

  // --------------------------------------------------------------------------                     
  // Initialize our hook dynamic link library
  // --------------------------------------------------------------------------           
  rc = DLLInitRoutine(hwndFrame);                                             
  if (!rc)                                                                                    
   {                                                                                          
    DisplayMessages(NULLHANDLE, "Error Initializing Dynamic Link Library", MSG_ERROR);                       
    exit(TRUE);      
   }                                                                                          

  // --------------------------------------------------------------------------                        
  // Set our input hook.                                                                               
  // --------------------------------------------------------------------------                        
  rc = SetInputHook();                                                  
  if (!rc)                                                    
   {                                                                
    DisplayMessages(NULLHANDLE, "Error Setting Input Hook", MSG_ERROR);       
   }                                                                

  // --------------------------------------------------------------------------                              
  // Set our flush buffer hook.                                                                            
  // --------------------------------------------------------------------------                              
  rc = SetFlushBufHook();
  if (!rc)                                                    
   {                                                                
    DisplayMessages(NULLHANDLE, "Error Setting FlushBuffer Hook", MSG_ERROR);       
   }                                                                
                                                                     
  // --------------------------------------------------------------------------                              
  // Set our lockup hook.                                                                            
  // --------------------------------------------------------------------------                              
  rc = SetLockupHook();                                       
  if (!rc)                                                    
   {                                                                
    DisplayMessages(NULLHANDLE, "Error Setting Lockup Hook", MSG_ERROR);       
   }                                                                

  // --------------------------------------------------------------------------            
  // Unload PMHOOKS.                                                                
  // --------------------------------------------------------------------------            
  rc = DosFreeModule(hmodule);
  if (rc)                                                                         
   {                                                                              
    DisplayMessages(NULLHANDLE, "Error Unloading Module", MSG_ERROR);               
    exit(TRUE);                                                                   
   }                                                                              

  // --------------------------------------------------------------------------                
  // Obtain the screen dimensions.                                                                
  // --------------------------------------------------------------------------                
  lcxScreen = WinQuerySysValue (HWND_DESKTOP, SV_CXSCREEN);            
  lcyScreen = WinQuerySysValue (HWND_DESKTOP, SV_CYSCREEN);            

  // --------------------------------------------------------------------------                          
  // Position our frame window.                                                                          
  // --------------------------------------------------------------------------                          
  WinSetWindowPos(hwndFrame,                                                                             
                  NULLHANDLE,                                                                            
                  lcxScreen / 8,                                                                                   
                  lcyScreen / 4,                                                                                   
                  0 ,                                                                                    
                  0 ,                                                                                    
                  SWP_SHOW | SWP_MOVE | SWP_ACTIVATE);                                                                  

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



  // --------------------------------------------------------------------------      
  // NOTE:  When we get to this point we are ready to destroy our application.       
  //        The first thing we will need to do is be kind to everyone else in        
  //        the PM screen group and remove our hooks.  Then we proceed with the      
  //        regular PM cleanup which consists of destroying the frame window,        
  //        destroying the application message queue and terminating our             
  //        anchor block removing our ability to call the Presentation Manager.      
  // --------------------------------------------------------------------------      
  ReleaseInputHook();
  ReleaseLockupHook();
  ReleaseFlushBufHook();

  WinDestroyWindow(hwndFrame);
  WinDestroyMsgQueue(hmq);
  WinTerminate(hab);
  return(FALSE);
}


// *******************************************************************************          
// FUNCTION:     ClientWndProc                                                              
//                                                                                          
// FUNCTION USE: Typical client window procedure                                            
//                                                                                          
// DESCRIPTION:  This procedure processes all messages arriving for the client              
//               window of the main application window.                                     
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
  HAB     habTemp;
  HPS     hps;
  HBITMAP hbmClip;
  RECTL   rcl;
  POINTL  ptl;
  APIRET  rc;
  CHAR    szBuffer[CCHMAXPATH];

  static HWND    hwndMenu;
  static HBITMAP hbmPaint;

  switch (msg)
  {
    case WM_INITMENU:                                                                                            
         switch (SHORT1FROMMP (mp1))                                                                             
          {                                                                                                      
           case ID_EDITMENU:                                                                                     
                // --------------------------------------------------------------------------                    
                // Get the window handle of the menu                                                             
                // --------------------------------------------------------------------------                    
                hwndMenu = HWNDFROMMP(mp2);                                                                      
                                                                                                                 
                // --------------------------------------------------------------------------                  
                // Enable/Disable the Edit Menuitems accordingly.                        
                // --------------------------------------------------------------------------                  
                ToggleEditMenu(hwndMenu, IDM_EDITCUT,   hbmPaint);                                                 
                ToggleEditMenu(hwndMenu, IDM_EDITCOPY,  hbmPaint);                                                
                ToggleEditMenu(hwndMenu, IDM_EDITPASTE, hbmPaint);                                               
                ToggleEditMenu(hwndMenu, IDM_EDITCLEAR, hbmPaint);       
                return FALSE;                                                                                    
          }                                                                                                      
         break;                                                                                                  

    case WM_DESTROY:             // If the user abnormally terminates us, for example from   
         if (bCurrentState)      // the Window List or some other application we need to    
          {                      // check the current state, if CTRL-ALT-DEL is still        
           CheckDisableState();  // disabled, then we need to RE-ENABLE it when the          
          }                      // application terminates, regardless of how it ends....    

         if (hbmPaint)
          {
           GpiDeleteBitmap(hbmPaint);
          } 
         break;

    case WM_SAVEAPPLICATION:     // If the user abnormally terminates us by attempting to    
         if (bCurrentState)      // do a shutdown while our application is still running.    
          {                      // Check the current state, if CTRL-ALT-DEL is still        
           CheckDisableState();  // disabled, then we need to RE-ENABLE it before Shutdown   
          }                      // so that the user can CTRL-ALT-DEL at the shutdown dialog 
         break;

    case WM_UPDATEBITMAP:
         // --------------------------------------------------------------------------               
         // WM_USER + 20:  This is a user defined message that is sent from our 
         //                PMHOOKS module to alert us that the user has pressed the 
         //                printscreen key.  We will call the PrintScreenToBitmap 
         //                routine to make a bitmap of the active window.  The bitmap
         //                handle hbmPaint is drawn inside of the WM_PAINT processing
         //                via a call to the WinDrawBitmap API.
         // --------------------------------------------------------------------------               
         hbmPaint = PrintScreenToBitmap(); 
         if (hbmPaint == GPI_ERROR) 
          {
           DisplayMessages(NULLHANDLE, "Error capturing window as a bitmap", MSG_ERROR);
          } 

         WinInvalidateRect(hwnd, NULL, FALSE);  
         return FALSE;

    case WM_LOCKUP:
         // --------------------------------------------------------------------------     
         // WM_USER + 21:  This is a user defined message that is sent from our            
         //                PMHOOKS module during the processing of the lockup hook        
         //                procedure.  For the purposes of this sample program we       
         //                will display the Thank You dialog box whenever the user 
         //                selects Lockup while this application is running.
         // --------------------------------------------------------------------------     
         WinPostMsg(hwnd, WM_COMMAND, (MPARAM)IDM_THANKS, (MPARAM)NULL);   
         return FALSE;

    case WM_COMMAND:
         switch (COMMANDMSG(&msg)->cmd)
         {
           // --------------------------------------------------------------------------                                
           // There is no real help, so just display a simple message box that tells                               
           // the user how to use the functions that make use of the hooks.
           // --------------------------------------------------------------------------                                
           case IDM_HELPGEN:                 
           case IDM_HELPKEYS:                
                DisplayMessages(NULLHANDLE, HELP_TEXT, MSG_INFO);
                return FALSE;

           // --------------------------------------------------------------------------          
           // Don't really know why the user would select this option, but it's here 
           // anyway.  If the Capture Active Window menuitem is selected, the active
           // window would of course be our PMSCREEN window.  So we would get a 
           // bitmap of ourself, not really practical but what the heck....
           // --------------------------------------------------------------------------          
           case IDM_CAPTURE:
                DosBeep (500,500);                                                                             
                WinSendMsg(hwnd, WM_UPDATEBITMAP, NULL, NULL);
                return FALSE;
           
           // --------------------------------------------------------------------------                               
           // The user has asked to change the Draw Bitmap (DBM) flags used for 
           // drawing the bitmap.  The default is to draw the bitmap normally
           // using DBM_NORMAL, but the user may want to stretch the bitmap to fit 
           // the window by using the DBM_STRETCH flag when drawing.  This dialog 
           // uses radiobuttons to allow the user to configure the Draw Bitmap Flags.
           // --------------------------------------------------------------------------                               
           case IDM_DRAWBMP:                                                                          
                WinDlgBox (HWND_DESKTOP, hwnd, DrawBitmapDlgProc, NULLHANDLE, IDD_DRAWBMP, NULL);       
                return FALSE;                                                                          

           // --------------------------------------------------------------------------                                     
           // Display the current bitmap's information.  Note, we pass the bitmap                                                                    
           // handle in the pCreateParams parameter of the WinDlgBox function, so 
           // that the dialog procedure can get access to the bitmap.
           // --------------------------------------------------------------------------                                     
           case IDM_BMPRES:
                WinDlgBox (HWND_DESKTOP, hwnd, GetBitmapResDlgProc, NULLHANDLE, IDD_BMPRES, &hbmPaint);         
                return FALSE;                                                        

           // --------------------------------------------------------------------------                         
           // The product information dialog.                                                                        
           // --------------------------------------------------------------------------                         
           case IDM_PRODINFO:
                WinDlgBox (HWND_DESKTOP, hwnd, ProdInfoDlgProc, NULLHANDLE, IDD_PRODINFO, NULL);
                return FALSE;

           // --------------------------------------------------------------------------                         
           // A special dialog that says thanks....                                                                  
           // --------------------------------------------------------------------------                         
           case IDM_THANKS:
                WinDlgBox (HWND_DESKTOP, hwnd, ProdInfoDlgProc, NULLHANDLE, IDD_THANKS, NULL);
                return FALSE;

           // --------------------------------------------------------------------------                                     
           // This menuitem selection will display a dialog box that will allow the 
           // user to toggle (enable/disable) the reboot hotkey -- Ctrl-Alt-Del. 
           // --------------------------------------------------------------------------                                     
           case IDM_HKREBOOT:
                WinDlgBox (HWND_DESKTOP, hwnd, ToggleHotkeyDlgProc, NULLHANDLE, IDD_HOTKEY, NULL);
                return FALSE;
    
           // --------------------------------------------------------------------------            
           // The Clipboard Cut Operation
           // --------------------------------------------------------------------------            
           case IDM_EDITCUT:
                if (hbmPaint != NULLHANDLE)                                                            
                 {                                                                                    
                  rc = PutBitmapInClipboard(hbmPaint);                                                                                                                                      
                  if (!rc)                                                                              
                   {                                                                                    
                    DisplayMessages(NULLHANDLE, "Error putting bitmap in Clipboard.", MSG_EXCLAMATION); 
                   }                                                                                    
                 }
                hbmPaint = NULLHANDLE;
                WinInvalidateRect (hwnd, NULL, FALSE);                 
                return FALSE;

           // --------------------------------------------------------------------------            
           // The Clipboard Copy Operation
           // --------------------------------------------------------------------------            
           case IDM_EDITCOPY:
                hbmClip = DuplicateBitmap(hbmPaint);
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
                hbmPaint = GetBitmapFromClipboard();                      
                WinInvalidateRect (hwnd, NULL, FALSE);                                    
                return FALSE;                                           
    
           // --------------------------------------------------------------------------            
           // The Clipboard Clear Operation
           // --------------------------------------------------------------------------            
           case IDM_EDITCLEAR:
                if (hbmPaint != NULLHANDLE)                                  
                 {                                                  
                  GpiDeleteBitmap (hbmPaint);                            
                  hbmPaint = NULLHANDLE;                                 
                  WinInvalidateRect (hwnd, NULL, FALSE) ;            
                 }                                                  
                return FALSE;

           // --------------------------------------------------------------------------                  
           // The Exit Operation                                                      
           // --------------------------------------------------------------------------                  
           case IDM_EXIT:
                if (bCurrentState)
                 {
                  CheckDisableState();
                 }
    
                WinPostMsg(hwnd, WM_QUIT, MPFROMLONG(NULL), MPFROMLONG(NULL));
                return FALSE;
         }
      break;
     
    case WM_CLOSE:
         if (bCurrentState)                                                           
          {                                                                          
           CheckDisableState();                                                      
          }                                                                          
         break;                                                                      
     
    case WM_PAINT:
         hps = WinBeginPaint (hwnd, NULLHANDLE, &rcl);
         GpiErase (hps);
         WinFillRect(hps, &rcl, SYSCLR_WINDOW);             
     
         if (hbmPaint) 
          {
           // --------------------------------------------------------------------------         
           // We need to once again get the rectangle coordinates for the                        
           // window in case they changed from when we began painting.
           // --------------------------------------------------------------------------         
           WinQueryWindowRect(hwnd, &rcl);

           // --------------------------------------------------------------------------          
           // Draw the bitmap that we used                       
           // window in case they changed from when we began painting.                            
           // --------------------------------------------------------------------------          
           rc = WinDrawBitmap (hps,                // Presentation Space Handle
                               hbmPaint,           // Bitmap Handle
                               NULL,               // Subrectangle of bitmap
                               (PPOINTL) &rcl,     // Destination
                               0L,                 // Foreground Color
                               0L,                 // Background Color
                               ulBmpFlags);        // Draw Bitmap Flags
            
           if (!rc) 
            {
             // --------------------------------------------------------------------------      
             // If we got an error drawing the bitmap, let's call WinGetLastError         
             // so we can try to figure out exactly what went wrong.                        
             // --------------------------------------------------------------------------      
             ERRORID   eid;                                                                
             eid = WinGetLastError(WinQueryAnchorBlock(hwnd));                                                   
             sprintf(szBuffer, "WinDrawBitmap Failed with Error ID = %08x\n", eid);                    
             DisplayMessages(0, szBuffer, MSG_INFO);                                        
            } 
          }
         WinEndPaint (hps);
         break;
     
   } 
  return WinDefWindowProc(hwnd, msg, mp1, mp2);
}    
     
// *******************************************************************************                               
// FUNCTION:     DisableThreeFingerSalute                                                                            
//                                                                                                          
// FUNCTION USE: Toggles the Ctrl-Alt-Del Reboot Hotkey.                                                            
//                                                                                                          
// DESCRIPTION:  This function will issue a Keyboard IOCTL to toggle the                               
//               reboot Hotkey (Ctrl-Alt-Del), which calls the reboot service.                                                                       
//                                                                                                          
// PARAMETERS:   VOID                                                                
//                                                                                                          
// RETURNS:      VOID                                                                                       
//                                                                                                          
// HISTORY:                                                                                                 
//                                                                                                          
// *******************************************************************************                          
VOID DisableThreeFingerSalute(VOID)
{
  HFILE    hfFile;                     
  APIRET   rc;                         
  USHORT   usCounter;                  
  ULONG    ulAction;
  ULONG    ulParmLength;
  ULONG    ulFileSize = 0;

  #define KBD            "KBD$"           // Open Keyboard Device            
  #define KBDCATEGORY     0x04            // Device Category                 
  #define KBDFUNCTION     0x56            // Device Function                 
  #define TRY_AGAIN       1000            // Maximum Device Retry            
  #define FLAGS           OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS

  for (usCounter = 0; usCounter < TRY_AGAIN; usCounter++)
   {
    rc = DosOpen(KBD,                          // File Name - Keyboard Device
                 &hfFile,                      // File Handle
                 &ulAction,                    // Action Taken
                 ulFileSize,                   // File Size
                 FILE_SYSTEM,                  // File Attributes
                 FLAGS,                        // Open Flags
                 OPEN_SHARE_DENYNONE,          // Open Mode
                 NULL);                        // EA Buffer


    if (rc != 0)
     {
      DisplayMessages(NULLHANDLE, "DosOpen Failed", MSG_ERROR);
     }
    break;
   }


  for (usCounter = 0; usCounter < TRY_AGAIN; usCounter++)
   {
    Parm_Packet.KeyState    =  0;  // This is the structure we pass into DosDevIOCtl     
    Parm_Packet.MakeCode    =  0;  // for changing the keyboard hotkeys.  See pmscreen.h 
    Parm_Packet.BreakCode   =  0;  // for structure definition.  The keyID with a value  
    Parm_Packet.KeyID       = -1;  // of negative one acts as a toggle, setting and      
                                   // removing the hotkeys.  See Category 4 Function 56H 

    ulParmLength = sizeof (Parm_Packet);

    rc = DosDevIOCtl  (hfFile,               // Device Handle
                       KBDCATEGORY,          // Device Category
                       KBDFUNCTION,          // Device Function
                       &Parm_Packet,         // Command Arguments List
                       ulParmLength,         // Command Arguments MAX Length
                       &ulParmLength,        // Command Arguments Length
                       NULL,                 // Data Area
                       0,                    // Data Area MAX Length
                       0);                   // Data Area Length

    if (rc != 0)
     {
      DisplayMessages(NULLHANDLE, "DosDevIOCtl Failed", MSG_ERROR); 
     }
    break;

    rc = DosClose(hfFile);

    if (rc != 0)
     {
      DisplayMessages(NULLHANDLE, "DosClose Failed", MSG_ERROR); 
     } 
   } 
 return;
}

// *******************************************************************************              
// FUNCTION:     CheckDisableState                                                           
//                                                                                              
// FUNCTION USE: Checks the bCurrentState global FLAG to see whether or not                            
//               the reboot hotkey (Ctrl-Alt-Del) sequence is enabled or 
//               disabled via the call to DisableThreeFingerSalute.             
//
// DESCRIPTION:  Depending on the current state of the bCurrentState    
//               flag, we will post a message box indicating the    
//               current state, and then toggle the bCurrentState   
//               flag for the next time.                            
//                                                                  
//               When the application is first invoked this flag is 
//               initialized to 0.  The user via the pushbutton in the     
//               Toggle Reboot Hotkey dialog, can change the state
//               to ENABLE or DISABLE the reboot sequence.                                         
//                                                                  
//               But, here is the problem.  This program is used as 
//               a demonstration ONLY.  If the user DISABLES the    
//               reboot sequence we want to enable it automatically 
//               when the user attempts to end our program, otherwise
//               the user may shutdown with the reboot hotkey disabled
//               and not be able to reboot their machine.
//
//               To do this we must trap the following messages for the          
//               following exit reasons.
//                                                                                              
//               WM_COMMAND/IDM_EXIT -  The application menuitem for exit            
//               WM_CLOSE            -  User close from system menu     
//               WM_DESTROY          -  User may kill us from Window List
//               WM_SAVEAPPLICATION  -  User may attempt Shutdown during 
//
// PARAMETERS:   VOID                                                                          
//                                                                                              
// RETURNS:      BOOL     TRUE  if Ctrl-Alt-Del is Disabled                                     
//                        FALSE if Ctrl-Alt-Del is Enabled
//                                                                                              
// HISTORY:                                                                                     
//                                                                                              
// *******************************************************************************              
BOOL CheckDisableState(VOID)
{
 // --------------------------------------------------------------------------               
 // Disable CTRL-ALT-DEL hotkey, set current state flag to TRUE.                                                         
 // --------------------------------------------------------------------------               
 if (!bCurrentState)                   
  {
   DisableThreeFingerSalute();
   DisplayMessages(NULLHANDLE, "The Ctrl-Alt-Del Hotkey is DISABLED", MSG_INFO);
   bCurrentState = TRUE;
  }

 // --------------------------------------------------------------------------         
 // Enable CTRL-ALT-DEL hotkey, set current state flag to FALSE.         
 // --------------------------------------------------------------------------         
 else                  
  {
   DisableThreeFingerSalute();
   DisplayMessages(NULLHANDLE, "The Ctrl-Alt-Del Hotkey is ENABLED", MSG_INFO); 
   bCurrentState = FALSE;
  }
 return bCurrentState;
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
        CenterDialog(hwnd);
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
// FUNCTION:     DrawBitmapDlgProc
//
// FUNCTION USE: Dialog Procedure used for changing draw colors
//
// DESCRIPTION:  This dialog procedure uses radiobuttons to allow the user to
//               select the method of drawing the bitmap during the WM_PAINT 
//               message.  The radiobuttons correspond to the draw bitmap flags
//               (DBM) for the WinDrawBitmap API.
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
MRESULT EXPENTRY DrawBitmapDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 HWND hwndClient;

 static ULONG ulInitial;

 switch (msg)
 {
  case WM_INITDLG:
       // --------------------------------------------------------------------------            
       // Initialize the system menu.                                                          
       // --------------------------------------------------------------------------            
       SetTheSysMenu(hwnd, NULLHANDLE);

       // -------------------------------------------------------------------------- 
       // Check whatever the global Draw bitmap flags have been set to.        
       // -------------------------------------------------------------------------- 
       WinCheckButton(hwnd, ulBmpFlags, TRUE);

       // --------------------------------------------------------------------------               
       // Store the original flag, just in case the clicks the Cancel button.        
       // --------------------------------------------------------------------------               
       ulInitial = ulBmpFlags;
       return FALSE;

  case WM_CONTROL:
       switch VALUE
       {
        case DBM_NORMAL:
        case DBM_INVERT:
        case DBM_STRETCH:
             ulBmpFlags = VALUE;

             // --------------------------------------------------------------------------         
             // Paint the client window so that we get the new bitmap flags.                       
             // --------------------------------------------------------------------------         
             hwndClient = WinWindowFromID(hwndFrame, FID_CLIENT);
             WinInvalidateRect(hwndClient, NULL, FALSE);                           
             return FALSE;
       }

  case WM_COMMAND:
       switch (COMMANDMSG(&msg)->cmd)
       {
        case DID_OK:
             WinDismissDlg(hwnd, TRUE);
             return FALSE;

        case DID_CANCEL:
             // --------------------------------------------------------------------------          
             // Reset the Draw Bitmap Flags to whatever the original setting was.
             // --------------------------------------------------------------------------          
             ulBmpFlags = ulInitial;

             // --------------------------------------------------------------------------     
             // Paint the client window so that we get the new bitmap flags.                   
             // --------------------------------------------------------------------------     
             hwndClient = WinWindowFromID(hwndFrame, FID_CLIENT);                                   
             WinInvalidateRect(hwndClient, NULL, FALSE);                                            

             // --------------------------------------------------------------------------                
             // Dismiss the dialog box.                         
             // --------------------------------------------------------------------------                
             WinDismissDlg(hwnd, TRUE);               
             return FALSE;                                                                          

        case DID_HELP:
             DisplayMessages(NULLHANDLE,
                             "Select the radiobutton that represents the bitmap flags you want to use, and then select OK.\n",
                             MSG_INFO);
             return FALSE;
       }
 }
 return WinDefDlgProc(hwnd, msg, mp1, mp2);
}


// *******************************************************************************                     
// FUNCTION:     ToggleHotkeyDlgProc                                                                     
//                                                                                                     
// FUNCTION USE: Dialog Procedure used to toggle Ctrl-Alt-Del Hotkey                                       
//                                                                                                     
// DESCRIPTION:  Creates a Dialog Window that will have a pushbutton                         
//               that will be used to toggle the bCurrentState flag to enable  
//               or disable the Ctrl-Alt-Del hotkey via a call to the      
//               CheckDisableState function.                   
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
MRESULT EXPENTRY ToggleHotkeyDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 HWND hwndButton;
 static BOOL rc;


 switch (msg)
  {
   case WM_INITDLG:
        SetTheSysMenu(hwnd, NULL);
        hwndButton = WinWindowFromID(hwnd, DID_DISABLE);

        if (rc == TRUE)
         {
          WinSetWindowText (hwndButton, "~Enable");
         }

        else
         {
          WinSetWindowText (hwndButton, "~Disable");
         }
        return FALSE;

   case WM_COMMAND:
        switch (COMMANDMSG(&msg)->cmd)
         {
          case DID_DISABLE:
               WinDismissDlg(hwnd, TRUE);
               rc = CheckDisableState();
               break;
         }
        break;
  }
 return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
}



// *******************************************************************************                     
// FUNCTION:     GetBitmapResDlgProc                                                                     
//                                                                                                     
// FUNCTION USE: Dialog Procedure used to obtain the resolution of the bitmap                              
//                                                                                                     
// DESCRIPTION:  Shows important information from the BITMAPINFOHEADER2 structure                        
//               for the current displayed bitmap.
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
MRESULT EXPENTRY GetBitmapResDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 BITMAPINFOHEADER2 bmp2;
 HWND              hwndStatic;

 LONG              lScreenHeight;                     
 LONG              lScreenWidth;                      
 LONG              lScreenColors;                     
 LONG              alColors[CAPS_COLORS];         
 HDC               hdcTemp;                           
 APIRET            rc;                                

 static PHBITMAP   phbm;
 static HBITMAP    hbmQuery;

 CHAR szBuffer[CCHMAXPATH];

 switch (msg)
  {
   case WM_INITDLG:
        // --------------------------------------------------------------------------   
        // Remove any system menu menuitems that we don't really need 
        // --------------------------------------------------------------------------   
        SetTheSysMenu(hwnd, NULL);

        // --------------------------------------------------------------------------   
        // First get screen height and Width from PM using WinQuerySysValue    
        // --------------------------------------------------------------------------   
        lScreenHeight = WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);           
        lScreenWidth  = WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);           
                                                                               
        // --------------------------------------------------------------------------   
        // Get a device context handle for our window
        // --------------------------------------------------------------------------   
        hdcTemp = WinOpenWindowDC(hwnd);                                  
                                                                               
        // --------------------------------------------------------------------------   
        // Get the colors array
        // --------------------------------------------------------------------------   
        rc = DevQueryCaps(hdcTemp, CAPS_COLORS, CAPS_COLORS, alColors);                                                                                          
                                                                               
        // --------------------------------------------------------------------------   
        // If we got an error, we need to display it to the user.
        // --------------------------------------------------------------------------   
        if (rc != TRUE)                                                        
         {                                                                     
          sprintf(szBuffer, "DevQueryCaps failed with RC = %d", rc);           
          DisplayMessages(NULLHANDLE, szBuffer, MSG_EXCLAMATION);                     
         }                                                                     
                                                                               
        // --------------------------------------------------------------------------   
        // Get the screen colors 
        // --------------------------------------------------------------------------   
        lScreenColors = alColors[0];                                       

        // --------------------------------------------------------------------------   
        // Get the bitmap handle if any
        // --------------------------------------------------------------------------   
        phbm = PVOIDFROMMP(mp2);
        hbmQuery = *phbm;

        // --------------------------------------------------------------------------   
        // Initialize the bitmap info header structure
        // --------------------------------------------------------------------------   
        memset(&bmp2, 0, sizeof(BITMAPINFOHEADER2));                                      
        bmp2.cbFix = sizeof (BITMAPINFOHEADER2);                                             

        // --------------------------------------------------------------------------   
        // Get the BITMAPINFOHEADER2 structure for our bitmap.
        // --------------------------------------------------------------------------   
        if (hbmQuery)
         {
          GpiQueryBitmapInfoHeader (hbmQuery, &bmp2);
         }


        // --------------------------------------------------------------------------   
        // Format the static text windows with the bitmap/screen information 
        // that we just obtained, and then display the information in the static 
        // text windows.
        // --------------------------------------------------------------------------   
        sprintf(szBuffer, "Width         = %d", bmp2.cx);
        hwndStatic = WinWindowFromID(hwnd, ID_WIDTH);
        WinSetWindowText (hwndStatic, szBuffer);            

        sprintf(szBuffer, "Height        = %d", bmp2.cy);           
        hwndStatic = WinWindowFromID(hwnd, ID_HEIGHT);
        WinSetWindowText (hwndStatic, szBuffer);                   
                                                                                 
        sprintf(szBuffer, "Color Planes  = %d", bmp2.cPlanes);                    
        hwndStatic = WinWindowFromID(hwnd, ID_CPLANE);                           
        WinSetWindowText (hwndStatic, szBuffer);                   

        sprintf(szBuffer, "Bits per Pel  = %d", bmp2.cBitCount);      
        hwndStatic = WinWindowFromID(hwnd, ID_BPPPEL);            
        WinSetWindowText (hwndStatic, szBuffer);                   

        sprintf(szBuffer, "Screen Width  = %d", lScreenWidth);            
        hwndStatic = WinWindowFromID(hwnd, ID_SCRWIDTH);                     
        WinSetWindowText (hwndStatic, szBuffer);                           

        sprintf(szBuffer, "Screen Height = %d", lScreenHeight);            
        hwndStatic = WinWindowFromID(hwnd, ID_SCRHEIGHT);                     
        WinSetWindowText (hwndStatic, szBuffer);                           

        sprintf(szBuffer, "Screen Colors = %d", lScreenColors);            
        hwndStatic = WinWindowFromID(hwnd, ID_NUMCOLORS);                     
        WinSetWindowText (hwndStatic, szBuffer);                           

        return FALSE;                                                      

   case WM_COMMAND:
        switch (COMMANDMSG(&msg)->cmd)
         {
          case DID_OK:
          case DID_CANCEL:
               WinDismissDlg(hwnd, TRUE);
               break;
         }
        break;
  }
 return WinDefDlgProc (hwnd, msg, mp1, mp2);
}
