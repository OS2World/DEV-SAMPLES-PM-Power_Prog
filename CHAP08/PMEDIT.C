// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      PMEDIT.C                                         mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Main Source File for PMEDIT/Chapter 08 Sample Program         บ
// บ                                                                               บ
// บ  NOTES:         This file contains the source code for the PMEDIT program.    บ
// บ                 PMEDIT is a simple text editor designed to illustrate dialog  บ
// บ                 and menu concepts.                                            บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#define INCL_DOS
#define INCL_GPI
#define INCL_WIN
#define INCL_WINERRORS

#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "pmedit.fnc"
#include "pmedit.h"
#include "shcommon.h"

// --------------------------------------------------------------------------    
// Miscellaneous constant definitions used only in this source module.           
// --------------------------------------------------------------------------    
#define BACKCOLOR  "BkColor"
#define FORECOLOR  "FgColor"
#define WORDWRAP   "WordWrap"
#define WINDOWPOS  "WindowPos"

// --------------------------------------------------------------------------       
// User defined message to kill the menu.
// --------------------------------------------------------------------------       
#define WM_KILLMENU  WM_USER + 7

// --------------------------------------------------------------------------                  
// Maximum number of buttons in the button bar.                                                      
// --------------------------------------------------------------------------                  
#define MAXBUTTONS  10

// -------------------------------------------------------------------------- 
// Global Variables                                              
// -------------------------------------------------------------------------- 
HWND     hwndClient;         
SWCNTRL  swctl;      
static   HSWITCH hswitch;    
static   USHORT usXPosition[MAXBUTTONS] = {0, 50, 100, 150,  200, 250, 300, 350, 400, 450};       

// --------------------------------------------------------------------------                      
// This array is used for the Button Bar.  Each element of the array                                                                             
// corresponds to a button in the button bar.
// --------------------------------------------------------------------------                      
static   USHORT usID[11] = {IDM_FILENEW,     // File New Button         
                            IDM_FILEOPEN,    // File Open Button               
                            IDM_FILESAVE,    // File Save Button               
                            IDM_FILESAVEAS,  // File Save As Button
                            IDM_FILESEARCH,  // File Search Button               
                            IDM_EDITCUT,     // Edit Clipboard Cut Button                          
                            IDM_EDITCOPY,    // Edit Clipboard Copy Button                    
                            IDM_EDITPASTE,   // Edit Clipboard Paste Button                    
                            IDM_CHANGEFONT,  // Options Change Font Button    
                            IDM_PRODINFO};   // Product Information Button       

// --------------------------------------------------------------------------   
// Function Prototypes                                                        
// --------------------------------------------------------------------------   
VOID UpdateRecallMenuitem(VOID);                       
HWND AddCascadeMenu(HWND hwndFrame);                   
LONG MapColorToMenuitem(LONG lclrValue, BOOL bBack);               
VOID SetBackgroundColor(LONG lColor);
VOID SetForegroundColor(LONG lColor);                                                                 
VOID UpdateLineNumbers(VOID);
HWND CreateEditWindow(HWND hwnd);
BOOL CreateConditionalCascadeMenu(HWND hwndMenu, SHORT sSubMenu, SHORT sDefault);

// --------------------------------------------------------------------------                
// Dialog Window Procedures                                                                
// --------------------------------------------------------------------------                
MRESULT EXPENTRY UserPreferencesDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

// --------------------------------------------------------------------------        
// Subclass Window Procedures                                                             
// --------------------------------------------------------------------------        
MRESULT EXPENTRY ButtonBarSubclassProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);                      
MRESULT EXPENTRY MLESubclassProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);                      

// --------------------------------------------------------------------------                  
// Subclassed Procedures                                                                       
// --------------------------------------------------------------------------                  
PFNWP    OldButtonProc;                                                                        
PFNWP    OldMLEProc;                                                                           

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
   BOOL     rc;
   HAB      hab;
   HMQ      hmq;
   QMSG     qmsg;
   LONG     lcx;                                
   LONG     lcy;                               
   LONG     lcxScreen;                        
   LONG     lcyScreen;                        
   SWP      swp;                              
   ULONG    ulFrameFlags;                         
   ULONG    ulSwpLength = sizeof(SWP);             
   RECTL    rcl;
   CHAR     szWinTitle[100];
   USHORT   usCounter;
   HWND     hwndButton[MAXBUTTONS];

   // --------------------------------------------------------------------------                   
   // Define the array used for the button bar identifiers.                                     
   // --------------------------------------------------------------------------                   
   static  CHAR   *szResourceNumber[] = {"#600", "#601", "#602", "#603", "#604", "#605", "#606", "#607", "#608", "#609"}; 

   // --------------------------------------------------------------------------                 
   // Initialize ourselves with the presentation manager and get our                                                  
   // anchor block handle.
   // --------------------------------------------------------------------------                 
   hab = WinInitialize(0);
   if (!hab)
    {
     DosBeep(60, 100);
     exit(TRUE);
    }

   // --------------------------------------------------------------------------               
   // Create our application message queue.                                                    
   // --------------------------------------------------------------------------               
   hmq = WinCreateMsgQueue(hab, 0);
   if (!hmq)
    {
     DosBeep(60, 100);
     WinTerminate(hab);
     exit(TRUE);
    }

   // --------------------------------------------------------------------------               
   // Register our private window class.                                                       
   // --------------------------------------------------------------------------               
   rc = WinRegisterClass(hab,                              // Anchor block handle         
                         "EditorClass",                    // Name of Class to Register   
                         ClientWndProc,                    // Address of Window Procedure 
                         CS_SYNCPAINT | CS_SIZEREDRAW,     // Class Styles                
                         0);                               // Window words                

   if (!rc)
    {
     DisplayMessages(NULLHANDLE, "Error Registering Window Class", MSG_ERROR); 
     exit(TRUE);
    }

   ulFrameFlags = FCF_TITLEBAR | FCF_SIZEBORDER | FCF_MINMAX | 
                  FCF_SYSMENU  | FCF_MENU       | FCF_ICON   | FCF_ACCELTABLE;

   // --------------------------------------------------------------------------                      
   // Create the frame window.                                                                     
   // --------------------------------------------------------------------------                      
   hwndFrame = WinCreateStdWindow(HWND_DESKTOP,            // Desktop window is parent          
                                  NULLHANDLE,              // Set Window Style to Invisible     
                                  &ulFrameFlags,           // Frame control flags               
                                  "EditorClass",           // Window class name                 
                                  TITLEBAR,                // Window Title Bar                  
                                  0L,                      // Client style          
                                  NULLHANDLE,              // Resource is in executable          
                                  ID_FRAME,                // Frame window resource identifier  
                                  &hwndClient);            // Client window handle              

   if (!hwndFrame)
    {
     DisplayMessages(NULLHANDLE, "Error creating frame window", MSG_ERROR);
     exit(TRUE);
    }

   // --------------------------------------------------------------------------                           
   // Position our frame window.                                                                           
   // --------------------------------------------------------------------------                           
   lcxScreen = WinQuerySysValue (HWND_DESKTOP, SV_CXSCREEN);              
   lcyScreen = WinQuerySysValue (HWND_DESKTOP, SV_CYSCREEN);              

   lcx = lcxScreen / 8;                                                
   lcy = lcyScreen / 4;                                                
                                                                                          
   // --------------------------------------------------------------------------                                                                                             
   // Reposition the window based on our coordinates but hide the window
   // --------------------------------------------------------------------------             
   WinSetWindowPos(hwndFrame,                                                      
                   NULLHANDLE,                                                     
                   lcx,                                                            
                   lcy,                                                            
                   lcx * 6,                                                            
                   lcy * 3,                                                            
                   SWP_SIZE | SWP_MOVE);                                

   // --------------------------------------------------------------------------            
   // Create our editor window using a multi-line entryfield MLE.
   // --------------------------------------------------------------------------            
   hwndMLE = CreateEditWindow(hwndClient);                                 
                                                                          
   // --------------------------------------------------------------------------            
   // Set the Font for our MLE window.
   // --------------------------------------------------------------------------            
   SetDefaultFont(hwndMLE); 

   // --------------------------------------------------------------------------            
   // Get the rectangle coordinates for the client window 
   // --------------------------------------------------------------------------            
   WinQueryWindowRect(hwndClient, &rcl);      

   // --------------------------------------------------------------------------                           
   // Create our status window.                                                                        
   // --------------------------------------------------------------------------                           
   WinCreateWindow (hwndClient,                   // Parent Window Handle                  
                    WC_STATIC,                    // Class Name                            
                    BOOKTITLE,                    // Window Text                           
                    WS_VISIBLE | SS_TEXT,         // Window Styles                         
                    0,                            // Initial X coordinate                  
                    0,                            // Initial Y coordinate                  
                    lcxScreen,                    // Horizontal Length of Static Text      
                    20,                           // Vertical Width of Static Text         
                    NULLHANDLE,                   // Owner Window Handle                   
                    HWND_BOTTOM,                  // Sibling Window                        
                    ID_STATIC,                    // Resource Identifier                   
                    NULL,                         // Control Data                          
                    NULL ) ;                      // Presentation Parameters               
  
   // --------------------------------------------------------------------------                         
   // Create our button bar.                                                                          
   // --------------------------------------------------------------------------                         
   for (usCounter = 0; usCounter < MAXBUTTONS; usCounter++) 
    {
     strcpy(szWinTitle, szResourceNumber[usCounter]);     
     hwndButton[usCounter] = WinCreateWindow (hwndClient,                           
                                              WC_BUTTON,                            
                                              szWinTitle,                           
                                              WS_VISIBLE | BS_PUSHBUTTON | BS_ICON,
                                              usXPosition[usCounter],               
                                              rcl.yTop - 50,  
                                              50,                                   
                                              50,                                   
                                              NULLHANDLE,                           
                                              HWND_BOTTOM,                          
                                              usID[usCounter],                            
                                              NULL,                 
                                              NULL);                                
  
     // --------------------------------------------------------------------------         
     // We need to subclass each of the buttons in the button bar.  The subclass       
     // routine is used to allow the user to press the right mouse button on 
     // any of the buttons.  When the user presses the right mouse button we 
     // will display some text in the status window that tells the user what the
     // button does.
     // --------------------------------------------------------------------------         
     OldButtonProc = WinSubclassWindow(hwndButton[usCounter], ButtonBarSubclassProc);          
    }

   if (argc > 1)                                              
    {                                                      
     FileOpen(hwndMLE, argv[1]);
    }                                                         

   // --------------------------------------------------------------------------              
   // Careful, we always want the MLE window to have the input pointer, but                         
   // it can be very dangerous to force the focus by calling WinSetFocus.                           
   // Instead, a little PM slight of hand....  trick PM into believing that                         
   // the last child window with focus was the MLE.                                                 
   // --------------------------------------------------------------------------              
   WinSetWindowULong(hwndFrame, QWL_HWNDFOCUSSAVE, hwndMLE);                                    

   // --------------------------------------------------------------------------                   
   // Check the OS2.INI file to see if window coordinates exist?                                        
   // If no entry is found, just use our current coordinates, but                                       
   // make our window visible.                                                                          
   // --------------------------------------------------------------------------                   
   if (!PrfQueryProfileData(HINI_PROFILE, APPNAME, "WindowPos", &swp, &ulSwpLength))                    
    {                                                                                                   
     WinSetWindowPos(hwndFrame, NULLHANDLE, 0, 0, 0, 0, SWP_SHOW);                                        
    }                                                                                                   
                                                                                                        
   // --------------------------------------------------------------------------                        
   // If the window coordinates were stored away in the OS2.INI file, make sure                         
   // we restore to these coordinates.                                                                  
   // --------------------------------------------------------------------------                        
   else                                                                                                 
    {                                                                                                   
     WinRestoreWindowPos(APPNAME, WINDOWPOS, hwndFrame);                                                
//   WinSetWindowPos(hwndFrame, NULLHANDLE, 0, 0, 0, 0, SWP_SHOW);                 
    }                                                                                                   

   // --------------------------------------------------------------------------                    
   // Add the activate flag if the shell said we need to...
   // --------------------------------------------------------------------------                    
   WinQueryTaskSizePos(hab, 0, &swp);                     
                                                                          
   if (swp.hwndInsertBehind == HWND_TOP) 
    {
     WinSetWindowPos(hwndFrame, NULLHANDLE, 0, 0, 0, 0, SWP_ACTIVATE);                           
    }

   // --------------------------------------------------------------------------                          
   // Populate our switch list structure.
   // --------------------------------------------------------------------------                          
   swctl.hwnd          = hwndFrame;        // window handle           
   swctl.hwndIcon      = NULLHANDLE;       // icon handle             
   swctl.hprog         = NULLHANDLE;       // program handle          
   swctl.idProcess     = NULLHANDLE;       // process identifier      
   swctl.idSession     = NULLHANDLE;       // session identifier      
   swctl.uchVisibility = SWL_VISIBLE;      // visibility              
   swctl.fbJump        = SWL_JUMPABLE;     // jump indicator          
   swctl.bProgType     = PROG_DEFAULT;     // program type            

   strcpy(swctl.szSwtitle, TITLEBAR);

   // --------------------------------------------------------------------------                          
   // Add the entry to the switch list
   // --------------------------------------------------------------------------                          
   hswitch = WinAddSwitchEntry(&swctl);                                 

   // --------------------------------------------------------------------------             
   // Get and dispatch messages from the application message queue  
   // until WinGetMsg returns FALSE, indicating a WM_QUIT message   
   // has occured.  When this happens we clean up our resources and 
   // exit the application.                                         
   // --------------------------------------------------------------------------             
   while(WinGetMsg(hab, &qmsg, (HWND)NULL, 0, 0))
    WinDispatchMsg(hab, &qmsg);

   // --------------------------------------------------------------------------          
   // Remove our switch list entry.
   // --------------------------------------------------------------------------          
   WinRemoveSwitchEntry(hswitch);

   // --------------------------------------------------------------------------                         
   // This is where we destroy our applications frame window and    
   // message queue and then call WinTerminate to clean up          
   // --------------------------------------------------------------------------                         
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
// HISTORY:      mm-dd-yy                                                                 
//                                                                                        
// *******************************************************************************        
MRESULT EXPENTRY ClientWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   HPS          hps;
   RECTL        rcl;

   HWND         hwndMenu;
   HWND         hwndToolBar[MAXBUTTONS - 1];
   static  HWND hwndSubMenu;

   USHORT       usCounter;
   APIRET       rc;

   LONG         lColor;
   LONG         lFileSize;                

   CHAR         szBuffer[CCHMAXPATH];
   CHAR         szTemp[CCHMAXPATH];

   switch(msg)
    {
     case WM_CREATE:
          // --------------------------------------------------------------------------
          // Get the window handle of our action bar menu.                         
          // --------------------------------------------------------------------------
          hwndSubMenu = WinWindowFromID (WinQueryWindow (hwnd, QW_PARENT), FID_MENU);                                                                          

          // --------------------------------------------------------------------------       
          // Make our cascade menu a conditional cascade menu.                              
          // --------------------------------------------------------------------------       
          CreateConditionalCascadeMenu(hwndSubMenu, IDM_COLORS, NULLHANDLE);
          CreateConditionalCascadeMenu(hwndSubMenu, IDM_TEXTCOLORS, NULLHANDLE);             
          break;

     case WM_INITMENU:
          // --------------------------------------------------------------------------                     
          // If our popup menu is currently displayed kill it if the user selects                         
          // anything from the action bar menu.
          // --------------------------------------------------------------------------                     
          WinSendMsg(hwndMLE, WM_KILLMENU, NULL, NULL);                 

          switch (SHORT1FROMMP (mp1))                                                                  
           {                                                                                           
            case ID_FILEMENU:
                 if (WinSendMsg(hwndMLE, MLM_QUERYCHANGED, NULL, NULL))        
                  {                                                            
                   WinEnableMenuItem(hwndSubMenu, IDM_FILESAVE, TRUE);                  
                  }                                                            
                 return FALSE;
  
            case ID_EDITMENU:                                                                          
                 // --------------------------------------------------------------------------         
                 // Toggle the Edit menu options for the clipboard based on whether           
                 // text is selected or in the case of the paste option, whether there
                 // is data in the clipboard.
                 // --------------------------------------------------------------------------         
                 ToggleEditMenu(hwndSubMenu, IDM_EDITCUT);                                      
                 ToggleEditMenu(hwndSubMenu, IDM_EDITCOPY);                                     
                 ToggleEditMenu(hwndSubMenu, IDM_EDITPASTE);                                    
                 ToggleEditMenu(hwndSubMenu, IDM_EDITCLEAR);                                    
                 return FALSE;                                                                         
           }                                                                                           
          break;                                                                                       
       
     case WM_SIZE:
          // --------------------------------------------------------------------------                   
          // Get the rectangle coordinates of the window.                                                 
          // --------------------------------------------------------------------------                   
          WinQueryWindowRect(hwnd, &rcl);             

          // --------------------------------------------------------------------------           
          // Reposition the button bar based on the current window coordinates.                   
          // --------------------------------------------------------------------------           
          for (usCounter = 0; usCounter < MAXBUTTONS; usCounter++)                               
           {                                                                            
            hwndToolBar[usCounter] = WinWindowFromID(hwnd, usID[usCounter]);
            WinSetWindowPos(hwndToolBar[usCounter], NULLHANDLE, usXPosition[usCounter], rcl.yTop - 50, 0, 0, SWP_MOVE); 
           }

          // --------------------------------------------------------------------------                    
          // Resize the MLE accordingly.                                                                   
          // --------------------------------------------------------------------------                    
          WinSetWindowPos(hwndMLE, HWND_TOP, rcl.xLeft, rcl.yBottom + 25, rcl.xRight, rcl.yTop - 80, SWP_SHOW | SWP_SIZE); 
          break;
                            
     case WM_CHAR:
          UpdateLineNumbers();                                                                                     
          return FALSE;

     case WM_SAVEAPPLICATION:
          // --------------------------------------------------------------------------                         
          // Whenever the user closes the application we will write our File name                             
          // array to the OS/2 INI file so that we can use it to repopulate the 
          // recall menuitem when the application is started.
          // --------------------------------------------------------------------------                         
          for (usFilePos = 0; usFilePos < 5; usFilePos++)
           {
            if (pszRecall[usFilePos] != NULL)       
             {
              sprintf(szBuffer, "FileRecall %d", usFilePos);
              PrfWriteProfileString(HINI_USER,                             
                                    APPNAME,                               
                                    szBuffer,                    
                                    pszRecall[usFilePos]);                    
             }
           }                                      
          return FALSE;

     case WM_COMMAND:
          switch (COMMANDMSG(&msg)->cmd)
           {
            // --------------------------------------------------------------------------            
            // These are the recall menuitems.                                          
            // --------------------------------------------------------------------------            
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
                 // --------------------------------------------------------------------------                   
                 // Open a file and stick it in the edit window.                                                 
                 // --------------------------------------------------------------------------                   
                 rc = FileOpen(hwndMLE, pszRecall[COMMAND]);                   
                                                                                                                 
                 // --------------------------------------------------------------------------                   
                 // If the user cancelled out of the standard file dialog we want to bail                        
                 // out here and not update the recall menuitem.                                                 
                 // --------------------------------------------------------------------------                   
                 if (rc == DID_CANCEL)                                                                           
                  {                                                                                              
                   return FALSE;                                                                                 
                  }                                                                                              
                                                                                                                 
                 // --------------------------------------------------------------------------                   
                 // If there was an error opening the file display it to the user.                               
                 // --------------------------------------------------------------------------                   
                 else if (rc)                                                                                    
                  {                                                                                              
                   sprintf(szBuffer, "An error occurred opening the file.  The return code was %d", rc);         
                   DisplayMessages(NULLHANDLE, szBuffer, MSG_EXCLAMATION);                                       
                  }                                                                                              

                 return FALSE;

            // --------------------------------------------------------------------------              
            // If the user presses the ESC key when the popup menu is visible within                
            // the MLE, hide it.
            // --------------------------------------------------------------------------              
            case IDM_DISMISS:
                 WinSendMsg(hwndMLE, WM_KILLMENU, NULL, NULL);
                 return FALSE;

            // --------------------------------------------------------------------------       
            // We do not have help!!!
            // --------------------------------------------------------------------------       
            case IDM_HELPINDEX:              
            case IDM_HELPGEN:                
            case IDM_HELPKEYS:               
                 DisplayMessages(NULLHANDLE, HELP_TEXT, MSG_INFO);
                 return FALSE;

            // --------------------------------------------------------------------------       
            // If the user wants to change the background color, let's do it.
            // --------------------------------------------------------------------------       
            case IDM_RED:          
            case IDM_BLUE:         
            case IDM_YELLOW:       
            case IDM_CYAN:         
            case IDM_DEFAULT:                  
            case IDM_WHITE:        
                 SetBackgroundColor(COMMAND);
                 return FALSE;

            // --------------------------------------------------------------------------       
            // If the user wants to change the text color, let's do it.
            // --------------------------------------------------------------------------       
            case IDM_TXT_BLACK:    
            case IDM_TXT_WHITE:    
            case IDM_TXT_RED:      
            case IDM_TXT_BLUE:     
            case IDM_TXT_GREEN:    
            case IDM_TXT_YELLOW:   
                 SetForegroundColor(COMMAND);
                 return FALSE;                                                                        

            case IDM_FILEOPEN:
                 // --------------------------------------------------------------------------          
                 // Open a file and stick it in the edit window.                                       
                 // --------------------------------------------------------------------------          
                 rc = FileOpen(hwndMLE, NULLHANDLE);

                 // --------------------------------------------------------------------------          
                 // If the user cancelled out of the standard file dialog we want to bail
                 // out here and not update the recall menuitem. 
                 // --------------------------------------------------------------------------          
                 if (rc == DID_CANCEL) 
                  {
                   return FALSE;
                  }

                 // --------------------------------------------------------------------------      
                 // If there was an error opening the file display it to the user.
                 // --------------------------------------------------------------------------      
                 else if (rc)
                  {
                   sprintf(szBuffer, "An error occurred opening the file.  The return code was %d", rc);                          
                   DisplayMessages(NULLHANDLE, szBuffer, MSG_EXCLAMATION);
                  }

                 UpdateRecallMenuitem();
                 return FALSE;
              
            case IDM_FILESAVE:
                 FileSave(hwndMLE);
                 return FALSE;

            case IDM_FILESAVEAS: 
                 FileSaveAs(hwndMLE);   
                 return FALSE;        

            case IDM_FILENEW:
                 FileNew(hwndMLE, TRUE);
                 return FALSE;
              
            case IDM_FILESEARCH:
                 WinDlgBox (HWND_DESKTOP, hwndFrame, FileSearchDlgProc, NULLHANDLE, ID_SEARCH, NULL);       
                 break;

            case IDM_PRODINFO:                                                                
                 WinDlgBox (HWND_DESKTOP, hwndFrame, ProdInfoDlgProc, NULLHANDLE, ID_PRODINFO, NULL); 
                 break;                                                                    

            case IDM_EDITCUT: 
                 WinSendMsg(hwndMLE, MLM_CUT, NULL, NULL);     
                 break;       

            case IDM_EDITCOPY: 
                 WinSendMsg(hwndMLE, MLM_COPY, NULL, NULL);    
                 return FALSE;       
                                      
            case IDM_EDITPASTE:         
                 WinSendMsg(hwndMLE, MLM_PASTE, NULL, NULL);             
                 return FALSE;       

            case IDM_EDITCLEAR: 
                 WinSendMsg(hwndMLE, MLM_CLEAR, NULL, NULL);    
                 return FALSE;       

            case IDM_RECALL:
                 for (usFilePos = 0; usFilePos < 5; usFilePos++)                      
                  {                                                             
                   sprintf(szBuffer, "FileRecall %d", usFilePos);                                 

                   rc = PrfQueryProfileString(HINI_USER,               
                                              APPNAME,           
                                              szBuffer,              
                                              NULL,                       
                                              (PVOID)szTemp,               
                                              sizeof(szTemp));         

                   if (!rc)                 
                    {                           
                     AddCascadeMenu(hwndFrame); 
                     return FALSE;              
                    }                           

                   pszRecall[usFilePos] = malloc(strlen(szTemp) + 1);       
                   strcpy(pszRecall[usFilePos], szTemp);                    
                  }
                 AddCascadeMenu(hwndFrame);        
                 return FALSE;

            // --------------------------------------------------------------------------         
            // If the user wants to change the font, let's change it...                                          
            // --------------------------------------------------------------------------         
            case IDM_CHANGEFONT:
                 SelectFont(hwndMLE);                        
                 WinInvalidateRect(hwndMLE, NULL, TRUE);  
                 return FALSE;

            case IDM_EDITSELALL:                                                              
                 {
                  // --------------------------------------------------------------------------            
                  // Get the length of the text.  If the text length is zero, we do                        
                  // nothing since there is no text to select.
                  // --------------------------------------------------------------------------            
                  lFileSize = (LONG) WinSendMsg(hwndMLE, MLM_QUERYTEXTLENGTH, NULL, NULL);                                            
                  if (lFileSize != 0) 
                   {
                    // --------------------------------------------------------------------------                
                    // Select from the beginning of the file all the way until the end.                          
                    // --------------------------------------------------------------------------                
                    WinSendMsg(hwndMLE, MLM_SETSEL, (MPARAM) 0L, (MPARAM) lFileSize);                                                 

                    // --------------------------------------------------------------------------       
                    // Paint the window.                                                      
                    // --------------------------------------------------------------------------       
                    WinEnableWindowUpdate(hwnd, FALSE);                    
                    WinInvalidateRect(hwnd, NULL, TRUE);                                     
                    WinEnableWindowUpdate(hwnd, TRUE);                                       
                   }
                  WinSendMsg(hwndMLE, MLM_RESETUNDO, NULL, NULL);     
                 }                                                                        
                break;                                                                   

            case IDM_CONFIGURE:
                 WinDlgBox (HWND_DESKTOP, hwndFrame, UserPreferencesDlgProc, NULLHANDLE, ID_CONFIGURE, NULL); 
                 return FALSE;

            case IDM_EXIT:
                 WinPostMsg(hwnd, WM_QUIT, MPFROMLONG(NULL), MPFROMLONG(NULL));      
                 break;
              
            // --------------------------------------------------------------------------      
            // If the user wants to save the window position, let's save it...
            // --------------------------------------------------------------------------      
            case IDM_SAVEWIN:                                                                                
                 rc = DisplayMessages(NULLHANDLE, "Save Window Position?", MSG_WARNING);                       
                 if (rc == MBID_YES)                                                                       
                  {                                                                                        
                   DosBeep(300, 100);                                                                      
                   WinStoreWindowPos(APPNAME, "WindowPos", hwndFrame);                                     
                   DisplayMessages(NULLHANDLE, "Window values Saved in OS2.INI", MSG_INFO);                
                  }                                                                                        
                 return FALSE;                                                                             
           }
          break;


     case WM_PAINT:
          hps = WinBeginPaint(hwnd, NULLHANDLE, (PRECTL)&rcl);     
          GpiErase (hps);                                                
          WinFillRect(hps, (PRECTL)&rcl, SYSCLR_WINDOW);           

          WinEndPaint(hps);                                              
          break;                                                         
    }
return WinDefWindowProc(hwnd, msg, mp1, mp2);
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
MRESULT EXPENTRY ProdInfoDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
    case WM_INITDLG:
         CenterDialog(hwnd);
         ReplaceTheSystemMenu(hwnd);
         WinSendMsg(hwnd, WM_UPDATEFRAME, NULL, NULL);           
         break;

    case WM_COMMAND:
         switch (COMMANDMSG(&msg)->cmd)
          {
           case DID_OK:
                WinDismissDlg (hwnd, TRUE);
                break;
          }
         break ;
   }
 return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
}


// *******************************************************************************
// FUNCTION:     CreateEditWindow
//
// FUNCTION USE: Used to create the MLE editor window within our client   
//
// DESCRIPTION:  This function is called from immediately after creating the frame       
//               Window(hwndFrame).  The purpose of this function is to create 
//               an MLE window that will fill our client area.  This MLE window  
//               is the basis of the editor.  First we must obtain the window    
//               coordinates of our client window by querying them via the 
//               WinQueryWindowRect function, then we can create our MLE window 
//               via WinCreateWindow using the MLECTLDATA control data structure.
//
// PARAMETERS:   HWND     window handle of client
//
// RETURNS:      VOID  
//
// HISTORY:
//
// *******************************************************************************
HWND CreateEditWindow(HWND hwnd)
{
  RECTL       rcl;
  MLECTLDATA  mledata;         
  HWND        hwndMLE;
  HWND        hwndSubMenu;
  LONG        lColor;
  LONG        lMenuitem;
  LONG        lWrapState;

  // --------------------------------------------------------------------------     
  // Populate an MLE control data structure.                                  
  // --------------------------------------------------------------------------     
  mledata.cbCtlData     =  sizeof(MLECTLDATA);      // size of structure           
  mledata.cchText       =  -1L;                     // no text limit boundary      
  mledata.iptAnchor     =  0;                       // begin selection pointer     
  mledata.iptCursor     =  0;                       // end selection pointer       
  mledata.cxFormat      =  0;                       // formatrect width            
  mledata.cyFormat      =  0;                       // formatrect height           
  mledata.afFormatFlags =  MLFFMTRECT_MATCHWINDOW;  // formatrect flags            
  mledata.afIEFormat    =  MLFIE_CFTEXT;            // import/export format        

  // --------------------------------------------------------------------------             
  // Get the rectangle coordinates for the Client Window.                                  
  // --------------------------------------------------------------------------             
  WinQueryWindowRect(hwnd, &rcl);

  // --------------------------------------------------------------------------                            
  // Create our editor window by drawing an MLE window within the client                             
  // window.  We leave room at the top of the client window for our button
  // bar windows, and room at the bottom of the client window for a status 
  // window.
  // --------------------------------------------------------------------------                            
  hwndMLE = WinCreateWindow(hwnd,                                                // Parent Window Handle          
                            WC_MLE,                                              // Window Class                  
                            NULL,                                                // Window Text                   
                            MLS_HSCROLL | MLS_VSCROLL | MLS_BORDER | WS_VISIBLE, // Window Styles                 
                            rcl.xLeft,                                           // Starting x coordinate 
                            rcl.yBottom + 25,                                    // Leave room at bottom
                            rcl.xRight,                                          // Horizontal Length
                            rcl.yTop - 80,                                       // Leave room at top
                            hwnd,                                                // Client is Owner Window           
                            HWND_TOP,                                            // Window Placement       
                            ID_MLEWINDOW,                                        // MLE Window Identifier         
                            (PVOID) &mledata,                                    // Control Data                  
                            NULL);                                               // Presentation Parameters       

                                                                           
  // --------------------------------------------------------------------------       
  // Subclass the MLE so that when the user presses button1 anywhere in the
  // MLE edit window, the line numbers are updated properly.
  // --------------------------------------------------------------------------       
  OldMLEProc = WinSubclassWindow(hwndMLE, MLESubclassProc);               

  // --------------------------------------------------------------------------               
  // See if word wrap is set on.  The default is ON, so if no entry exists                    
  // in the OS2.INI file, the call to PrfQueryProfileInt will return FALSE,                   
  // which corresponds to Word Wrap = ON.                                                     
  // --------------------------------------------------------------------------               
  lWrapState = PrfQueryProfileInt(HINI_USER, APPNAME, WORDWRAP, 0);                           
                                                                                              
  // --------------------------------------------------------------------------               
  // If Word Wrap is ON, make sure we alert the MLE.                                             
  // --------------------------------------------------------------------------               
  if (lWrapState == FALSE)                                                                    
   {                                                                                          
    WinSendMsg(hwndMLE, MLM_SETWRAP, MPFROM2SHORT(TRUE, FALSE), NULL);                 
   }                                                                                          

  // --------------------------------------------------------------------------            
  // Send a recall                                                            
  // --------------------------------------------------------------------------            
  WinSendMsg(hwnd, WM_COMMAND, MPFROM2SHORT(IDM_RECALL, FALSE), NULL);

  // --------------------------------------------------------------------------         
  // Get the window handle of the menu window.                                                                     
  // --------------------------------------------------------------------------         
  hwndSubMenu = WinWindowFromID(hwndFrame, FID_MENU);           

  // --------------------------------------------------------------------------               
  // Get the real background color from OS2.INI file and set it.               
  // --------------------------------------------------------------------------               
  lColor = PrfQueryProfileInt(HINI_USER, APPNAME, BACKCOLOR, 0);               

  // --------------------------------------------------------------------------    
  // If no entry is in the USER INI file then set the default to be                    
  // the entryfield background.
  // --------------------------------------------------------------------------    
  if (!lColor) 
   {
    lColor = SYSCLR_ENTRYFIELD;       
   }

  WinSendMsg(hwndMLE, MLM_SETBACKCOLOR, MPFROMLONG(lColor), NULL);              

  // --------------------------------------------------------------------------      
  // Map our real color value back to its appropriate menuitem.                    
  // --------------------------------------------------------------------------      
  lMenuitem = MapColorToMenuitem(lColor, TRUE);                

  // ----------------------------------------------------------------------
  // Check the selected menuitem.                                          
  // ----------------------------------------------------------------------
  WinCheckMenuItem(hwndSubMenu, lMenuitem, TRUE);                             

  // --------------------------------------------------------------------------    
  // Get the real foreground color from OS2.INI file and set it.                   
  // --------------------------------------------------------------------------    
  lColor = PrfQueryProfileInt(HINI_USER, APPNAME, FORECOLOR, 0);                  

  // -------------------------------------------------------------------------
  // If no entry is in the USER INI file then set the default to be black.          
  // -------------------------------------------------------------------------
  if (!lColor)                                                                
   {                                                                          
    lColor = CLR_BLACK;                                               
   }                                                                          

  WinSendMsg(hwndMLE, MLM_SETTEXTCOLOR, MPFROMLONG(lColor), NULL);                
                                                                                  
  // --------------------------------------------------------------------------   
  // Map our real color value back to its appropriate menuitem.                   
  // --------------------------------------------------------------------------   
  lMenuitem = MapColorToMenuitem(lColor, FALSE);                                         
                                                                                  
  // ----------------------------------------------------------------------       
  // Check the selected menuitem.                                                 
  // ----------------------------------------------------------------------       
  WinCheckMenuItem(hwndSubMenu, lMenuitem, TRUE);                                 

  return hwndMLE;
}



// *******************************************************************************
// FUNCTION:     ReplaceTheSysMenu
//
// FUNCTION USE: Converts a Cascade Menu to a Conditional Cascade Menu
//
// DESCRIPTION:  This function is used to illustrate how to modify the system    
//               menu.  It demonstrates how to populate the MENUITEM structure         
//               and insert items dynamically to fill the system menu.  The      
//               code is essentially creating the system menu manually.  The 
//               function will replace the current system menu bitmap with the
//               good old 1.3 or Windows 3.x system menu bitmap that most GUI 
//               users should already be accustomed to.      
//                 
//               NOTE: To use this routine you cannot specify the FCF_SYSMENU   
//                     flag or FCF_ICON flag on window creation.  These flags 
//                     will force the default window procedure to override 
//                     this routine and as a result you will not get the     
//                     customized system menu, but instead the default system      
//                     menu and system menu bitmap.   
//
// PARAMETERS:   HWND     window handle of the frame window that will contain 
//                        the new system menu.
//
// RETURNS:      VOID              
//
// HISTORY:
//
// *******************************************************************************
VOID ReplaceTheSystemMenu(HWND hwndFrame)                                                                                        
{                                                                                                                      
   HWND     hwndSysMenu;                                                                                               
   HWND     hwndPullDown;                                                                                              
   HWND     hwndObject;                                                                                                
   HWND     hwndSibling;                                                                                               
   HBITMAP  hbm;                                                                                                       
   MENUITEM menuitem;                                                                                                  
   CHAR     szMoveText[40];                                                                                   
   CHAR     szCloseText[40];

   // --------------------------------------------------------------------------          
   // This routine creates the system menu on the fly so we are adding                   
   // the Move and Close menuitems.  Here are the strings for these items.
   // --------------------------------------------------------------------------          
   strcpy (szMoveText,   "~Move\tAlt+F7");                                                                                         
   strcpy (szCloseText,  "~Close Product Information\tAlt+F4");                                                                                        
                                                                                                                       
   // --------------------------------------------------------------------------               
   // Get an object window from the desktop.                     
   // --------------------------------------------------------------------------               
   hwndObject  = WinQueryObjectWindow(HWND_DESKTOP);                                                                   

   // --------------------------------------------------------------------------              
   // Our system menu is a sibling window of the titlebar, so let's get the     
   // titlebar window handle.
   // --------------------------------------------------------------------------              
   hwndSibling = WinWindowFromID(hwndFrame, FID_TITLEBAR);                                                             

   // --------------------------------------------------------------------------                    
   // Get the old system menu bitmap.                                                        
   // --------------------------------------------------------------------------                    
   hbm         = WinGetSysBitmap(HWND_DESKTOP, SBMP_OLD_SYSMENU);    
                                                                                                                       
   // --------------------------------------------------------------------------                          
   // Create our system menu.                                                                     
   // --------------------------------------------------------------------------                          
   hwndSysMenu = WinCreateWindow(hwndFrame,                               // Frame Window is parent                 
                                 WC_MENU,                                 // Class Name                             
                                 NULL,                                    // Resource Identifier                    
                                 MS_ACTIONBAR | MS_TITLEBUTTON,           // Window Styles                          
                                 0,                                       // Initial X coordinate                   
                                 0,                                       // Initial Y coordinate                   
                                 0,                                       // Horizontal Length of Button            
                                 0,                                       // Vertical Length of Button              
                                 hwndFrame,                               // Owner Window Hndle                     
                                 hwndSibling,                             // Sibling Window                         
                                 FID_SYSMENU,                             // System Menu Resource Identifier        
                                 NULL,                                    // Interesting Control Data               
                                 NULL ) ;                                 // Presentation Parameters                
                                                                                                                    
   // --------------------------------------------------------------------------                                                                                                                         
   // Create the system menu pulldown window.                                                           
   // --------------------------------------------------------------------------        
   hwndPullDown = WinCreateWindow(hwndObject,                             // Object Window is parent                
                                 WC_MENU,                                 // Class Name                             
                                 NULL,                                    // Resource Identifier                    
                                 NULLHANDLE,                              // Window Styles                          
                                 0,                                       // Initial X coordinate                   
                                 0,                                       // Initial Y coordinate                   
                                 0,                                       // Horizontal Length of Button            
                                 0,                                       // Vertical Length of Button              
                                 hwndSysMenu,                             // Owner Window Hndle                     
                                 HWND_BOTTOM,                             // Sibling Window                         
                                 FID_SYSMENUPOP,                          // System Menu Resource Identifier        
                                 NULL,                                    // Interesting Control Data               
                                 NULL ) ;                                 // Presentation Parameters                
                                                                                                                       
   // --------------------------------------------------------------------------          
   // Populate MENUITEM structure.                                             
   // --------------------------------------------------------------------------          
   menuitem.iPosition   = MIT_END;                    // SHORT  Index Position                                                       
   menuitem.afStyle     = MIS_BITMAP  | MIS_SUBMENU;  // USHORT System Menu Bitmap                     
   menuitem.afAttribute = NULLHANDLE;                 // USHORT No Attributes                                                       
   menuitem.id          = SC_SYSMENU;                 // USHORT Menuitem Identifier                                                       
   menuitem.hwndSubMenu = hwndPullDown;               // HWND   Submenu Window Handle                                                       
   menuitem.hItem       = hbm;                        // ULONG  Additional Item                                                       
                                                                                                                       
   // --------------------------------------------------------------------------                  
   // Insert the system menu.                                                          
   // --------------------------------------------------------------------------                  
   WinSendMsg(hwndSysMenu,      // Window Handle to send message to                                                                                        
              MM_INSERTITEM,    // Menu Message                                                                                                            
              &menuitem,        // mp1 = MENUITEM structure                                                                                                
              NULL);            // mp2 = Text for Menuitem                                                                                                 
                                                                                                                       
   // --------------------------------------------------------------------------                  
   // Populate MENUITEM structure for the Move menuitem.                               
   // --------------------------------------------------------------------------                  
   menuitem.iPosition   = MIT_END;                    // SHORT                                                         
   menuitem.afStyle     = MIS_TEXT  | MIS_SYSCOMMAND; // USHORT                                                        
   menuitem.afAttribute = NULLHANDLE;                 // USHORT                                                        
   menuitem.id          = SC_MOVE;                    // USHORT                                                        
   menuitem.hwndSubMenu = NULLHANDLE;                 // HWND                                                          
   menuitem.hItem       = NULLHANDLE;                 // ULONG                                                         
                                                                                                                       
   // --------------------------------------------------------------------------            
   // Insert the Move menuitem.                                    
   // --------------------------------------------------------------------------            
   WinSendMsg(hwndPullDown,     // Window Handle to send message to                                                                                       
              MM_INSERTITEM,    // Menu Message                                                                                       
              &menuitem,        // mp1 = MENUITEM structure                                                                                       
              szMoveText);      // mp2 = Text for Menuitem                                                                                  
                                                                                                                       
   // --------------------------------------------------------------------------            
   // Insert the Close menuitem.                                                             
   // --------------------------------------------------------------------------            
   menuitem.id          = SC_CLOSE;                   // USHORT                                                        
   WinSendMsg(hwndPullDown,      // Window Handle to send message to                                                                                                                                                                              
              MM_INSERTITEM,     // Menu Message                                                                                                                                                                                                  
              &menuitem,         // mp1 = MENUITEM structure                                                                                                                                                                                      
              szCloseText);      // mp2 = Text for Menuitem                                                                                                                                                                                  
   return;
}                                                                                                                                                                                                                                                 


// *******************************************************************************
// FUNCTION:     CreateConditionalCascadeMenu
//
// FUNCTION USE: Converts a Cascade Menu to a Conditional Cascade Menu
//
// DESCRIPTION:  This routine takes a traditional cascade menu and converts the 
//               SUBMENU to a conditional cascade menu.  
//
// PARAMETERS:   HWND     window handle of menu window
//               SHORT    window identifier of SUBMENU to convert
//               SHORT    window identifier of DEFAULT item to check
//
// RETURNS:      FALSE Success
//               TRUE  Failure
//
// HISTORY:
//
// *******************************************************************************
BOOL CreateConditionalCascadeMenu(HWND hwndMenu, SHORT sSubMenu, SHORT sDefault)
{
   ULONG    ulSubMenuStyle;
   MENUITEM menuitem;

   WinSendMsg(hwndMenu, 
              MM_QUERYITEM,
              MPFROM2SHORT(sSubMenu, TRUE),
              MPFROMP(&menuitem));

   ulSubMenuStyle = WinQueryWindowULong(menuitem.hwndSubMenu, QWL_STYLE);

   WinSetWindowULong(menuitem.hwndSubMenu,
                     QWL_STYLE,
                     ulSubMenuStyle | MS_CONDITIONALCASCADE);

   WinSendMsg(menuitem.hwndSubMenu, 
              MM_SETDEFAULTITEMID, 
              MPFROMSHORT(sDefault), 
              NULL);

   return FALSE;
}

// *******************************************************************************                       
// FUNCTION:     ButtonBarSubclassProc                                                                    
//                                                                                                       
// FUNCTION USE: Used to allow the user to use the right mouse button on the                        
//               button bar. 
//                                                                                                       
// DESCRIPTION:  This is a suclass procedure that will allow the user to click                                                     
//               on the button bar buttons with the right mouse button to display
//               text information indicating what the button will do when pressed.
//               The status window is a simple static text window drawn at the 
//               bottom of the client window.
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
MRESULT EXPENTRY ButtonBarSubclassProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)                      
{                                                                                                        
  MRESULT  rc;
  USHORT   usID;

  if (msg == WM_BUTTON2DOWN)                                                                                    
   {                                                                                                     
      // --------------------------------------------------------------------------       
      // Determine which button the user has clicked on by getting the window 
      // identifier from the subclassed window.
      // --------------------------------------------------------------------------       
      usID = WinQueryWindowUShort(hwnd, QWS_ID);
      if (!usID) 
       {
        DisplayMessages(NULLHANDLE, "Error obtaining window identifier from window words.", MSG_EXCLAMATION);
       } 

      // --------------------------------------------------------------------------            
      // Depending on the identifier, display some text to the user in the status         
      // window, telling the user what this button does.                                    
      // --------------------------------------------------------------------------            
      switch (usID)   
       {                                     
        case IDM_FILENEW:
             UpdateStatusWindow(ID_STATIC, "Create a New File");
             return FALSE;

        case IDM_FILEOPEN:                     
             UpdateStatusWindow(ID_STATIC, "Open a File");
             return FALSE;

        case IDM_FILESAVE:   
             UpdateStatusWindow(ID_STATIC, "Save");
             return FALSE;

        case IDM_FILESAVEAS:   
             UpdateStatusWindow(ID_STATIC, "Save As");
             return FALSE;

        case IDM_FILESEARCH: 
             UpdateStatusWindow(ID_STATIC, "File Search");
             return FALSE;

        case IDM_EDITCUT:    
             UpdateStatusWindow(ID_STATIC, "Cut to Clipboard");
             return FALSE;

        case IDM_EDITCOPY:    
             UpdateStatusWindow(ID_STATIC, "Copy to Clipboard");
             return FALSE;

        case IDM_EDITPASTE:    
             UpdateStatusWindow(ID_STATIC, "Paste from Clipboard");
             return FALSE;

        case IDM_CHANGEFONT:                                                  
             UpdateStatusWindow(ID_STATIC, "Change the Font");          
             return FALSE;                                                   

        case IDM_PRODINFO:      
             UpdateStatusWindow(ID_STATIC, "Product Information");
             return FALSE;
       }
    return ((*OldButtonProc)(hwnd, msg, mp1, mp2));                                           
   }                                                                                               

  // --------------------------------------------------------------------------            
  // When the user lets go of the second mouse button we will clear the                  
  // status window.  We need to handle the WM_ENDDRAG message because if             
  // the user holds down the second mouse button and drags over another window
  // like the MLE window for instance, the status text will not be reset.
  // --------------------------------------------------------------------------            
  else if ((msg == WM_BUTTON2UP) || (msg == WM_ENDDRAG))         
   {
    UpdateStatusWindow(ID_STATIC, " ");
   }

  // --------------------------------------------------------------------------                    
  // If the user presses anywhere on the button bar while the popup menu is
  // still visible, then kill the popup menu.
  // --------------------------------------------------------------------------                    
  else if (msg == WM_BUTTON1DOWN)                                          
   {                                                                  
    rc =  ((*OldButtonProc)(hwnd, msg, mp1, mp2));                       
    WinSendMsg(hwndMLE, WM_KILLMENU, NULL, NULL);                                                                                
    return rc;                                                        
   }                                                                  

  else
   {
    return ((*OldButtonProc)(hwnd, msg, mp1, mp2));                                           
   }
}                                                                                                        

// *******************************************************************************      
// FUNCTION:     MLESubclassProc                                                  
//                                                                                      
// FUNCTION USE: Used to subclass Button 1 down for updating the line number 
//               status window and Button 2 down for a popup menu.
//                                                                                      
// DESCRIPTION:  Used to update the line numbers when the user presses the mouse           
//               button anywhere in the MLE.  Also, if the user presses the                
//               right mouse button anywhere in the MLE window, we will display            
//               a popup menu full of options.                                             
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
MRESULT EXPENTRY MLESubclassProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)                      
{                                                                                                        
 MRESULT rc;
 ULONG   ulOptions;
 POINTL  ptlCurrent;
 CHAR    szFont[30];


 static HWND    hwndPopup;      

 // --------------------------------------------------------------------------               
 // if the user presses the first mouse button anywhere in the MLE obtain
 // the position of the cursor and update the line numbers.
 // --------------------------------------------------------------------------               
 if (msg == WM_BUTTON1DOWN)                                                                                    
  {                                                                                                     
   rc =  ((*OldMLEProc)(hwnd, msg, mp1, mp2));                                           
   UpdateLineNumbers();
   return rc;
  } 

 // --------------------------------------------------------------------------             
 // If the user presses the escape key while we have a popup menu up, we           
 // hide the popup menu.  Also, if we get the WM_KILLMENU message we will
 // set the owner window to HWND_DESKTOP so that we do not get any 
 // additional WM_COMMAND messages.
 // --------------------------------------------------------------------------             
 else if (msg == WM_KILLMENU)                                                                                 
  {
   if (hwndPopup) 
    {
     WinShowWindow(hwndPopup, FALSE);
     WinSetOwner(hwndPopup, HWND_OBJECT);
    }
   return FALSE;
  }


 // --------------------------------------------------------------------------                              
 // When the user presses the second mouse button anywhere in our edit 
 // window we want to display a popup menu.
 // --------------------------------------------------------------------------                              
 else if (msg == WM_BUTTON2DOWN)                                                                         
  {
   WinQueryPointerPos(HWND_DESKTOP, &ptlCurrent);                                     
   WinMapWindowPoints(HWND_DESKTOP, hwnd, &ptlCurrent, 1);                    

   // --------------------------------------------------------------------------         
   // If we don't have the popup window handle load it and set the font.
   // --------------------------------------------------------------------------         
   if (!hwndPopup) 
    {
     hwndPopup = WinLoadMenu (hwnd, NULLHANDLE, ID_POPUPMENU);  
     strcpy(szFont, "8.Courier");                                                             
     WinSetPresParam(hwndPopup, PP_FONTNAMESIZE, sizeof(szFont) + 1, szFont);                 
    } 


   ulOptions = PU_NONE | PU_MOUSEBUTTON1 | PU_KEYBOARD | PU_HCONSTRAIN | PU_VCONSTRAIN;    


   // --------------------------------------------------------------------------                 
   // Draw our popup menu based on where the user clicks in the edit window.      
   // --------------------------------------------------------------------------                 
   WinPopupMenu (hwnd,         // Parent Window is MLE
                 hwndClient,   // Owner is Client Window
                 hwndPopup,    // Window handle of Popup Menu
                 ptlCurrent.x, // x coordinate
                 ptlCurrent.y, // y coordinate                              
                 0,            // Item identity                    
                 ulOptions);   // Option Flags                             

   WinSetOwner(hwndPopup, hwndClient);                     
   return ((*OldMLEProc)(hwnd, msg, mp1, mp2));                                           
  }

 else
  {
   return ((*OldMLEProc)(hwnd, msg, mp1, mp2));                                           
  }                                                                                               
}                                                                                                        


// *******************************************************************************                   
// FUNCTION:     AddCascadeMenu                                                                   
//                                                                                                   
// FUNCTION USE: Dynamically adds a Cascade Menu.                                
//                                                                                                   
// DESCRIPTION:  This routine is used to implement a recall menuitem.  The                           
//               function will add a cascade menu for recalling the five most 
//               recently opened files.  The implementation of the recall is 
//               poor(we just use a simple array of the files, and the most 
//               recently used file may not be at the top).
//                                                                                                   
// PARAMETERS:   HWND     window handle of frame window                                               
//                                                                                                   
// RETURNS:      HWND     window handle of submenu                                                                      
//                                                                                                   
// HISTORY:                                                                                          
//                                                                                                   
// *******************************************************************************                   
HWND AddCascadeMenu(HWND hwndFrame)
{
 HWND        hwndObject;
 HWND        hwndMenu;
 HWND        hwndCascade;
 static HWND hwndSubMenu;                
 MENUITEM    menuitem;
 PSZ         pszNewItemString;
 PSZ         pszClose;

 USHORT      usCounter;
 BOOL        rc;

 // --------------------------------------------------------------------------    
 // Get an object window
 // --------------------------------------------------------------------------    
 hwndObject = WinQueryObjectWindow(HWND_DESKTOP);

 // --------------------------------------------------------------------------    
 // Get main menu handle
 // --------------------------------------------------------------------------    
 hwndMenu   = WinWindowFromID(hwndFrame, FID_MENU);

 // --------------------------------------------------------------------------    
 // Get a populated MENUITEM structure for the File menu
 // --------------------------------------------------------------------------    
 WinSendMsg(hwndMenu, MM_QUERYITEM, MPFROM2SHORT(ID_FILEMENU, TRUE), (MPARAM) &menuitem);

 // --------------------------------------------------------------------------    
 // Get the window handle for the submenu out of the MENUITEM structure
 // --------------------------------------------------------------------------    
 hwndSubMenu = menuitem.hwndSubMenu;

 // --------------------------------------------------------------------------    
 // Create a cascade menu
 // --------------------------------------------------------------------------    
 hwndCascade = WinCreateWindow(hwndObject,
                               WC_MENU,
                               NULL,
                               NULLHANDLE,
                               0,
                               0,
                               0,
                               0,
                               hwndSubMenu,
                               HWND_BOTTOM,
                               1060,
                               NULL,
                               NULL ) ;

 // --------------------------------------------------------------------------    
 // Delete the Recall Menu so we can create it again
 // --------------------------------------------------------------------------    
 WinSendMsg(hwndSubMenu, MM_DELETEITEM, MPFROM2SHORT(IDM_FILERECALL, FALSE), NULL);   

 // --------------------------------------------------------------------------    
 // Add a recall menuitem on file menu
 // --------------------------------------------------------------------------    
 pszNewItemString      = "~Recall";
 menuitem.iPosition    = MIT_END;
 menuitem.afStyle      = MIS_SUBMENU | MIS_TEXT;
 menuitem.afAttribute  = 0;
 menuitem.id           = IDM_FILERECALL;
 menuitem.hwndSubMenu  = hwndCascade;
 menuitem.hItem        = 0;
 WinSendMsg(hwndSubMenu, MM_INSERTITEM, (MPARAM) &menuitem, (MPARAM) pszNewItemString);

 for (usCounter = 0; usCounter < 5; usCounter++)
  {
   if (pszRecall[usCounter] != NULL)
    {
     menuitem.iPosition    = MIT_END;
     menuitem.afStyle      = MIS_TEXT;
     menuitem.afAttribute  = 0;
     menuitem.id           = usCounter;       // Change this to the index for the array
     menuitem.hwndSubMenu  = 0;
     menuitem.hItem        = 0;

     // --------------------------------------------------------------------------              
     // Add each of the individual menuitems for the files to be recalled                                                                     
     // --------------------------------------------------------------------------              
     WinSendMsg(hwndCascade, MM_INSERTITEM, (MPARAM) &menuitem, (MPARAM) pszRecall[usCounter]);
    }
  } // endfor 

 // --------------------------------------------------------------------------    
 // Update frame window
 // --------------------------------------------------------------------------    
 WinSendMsg(hwndFrame, WM_UPDATEFRAME, NULL, NULL);

 return hwndSubMenu;
}

// *******************************************************************************                
// FUNCTION:     UpdateRecallMenuitem                                                         
//                                                                                                
// FUNCTION USE: Allocates Memory for Strings to be added to Cascade menu.                       
//                                                                                                
// DESCRIPTION:  This routine is used to maintain the recall array that is                        
//               used to fill the recall menuitem.
//                                                                                                
// PARAMETERS:   VOID     
//                        
// RETURNS:      VOID     
//                        
// HISTORY:                                                                                       
//                                                                                                
// *******************************************************************************                
VOID UpdateRecallMenuitem(VOID)
{
 static HWND    hwndSubMenu;                 
 CHAR szBuffer[CCHMAXPATH];

 if (usFilePos == 5)       
  {                      
   usFilePos = 0;          
  }                      

 if (pszRecall[usFilePos] != NULL)                                                       
  {                                                                                     
   free(pszRecall[usFilePos]);                                                           
  }                                                                                     
                                                                                          
 // --------------------------------------------------------------------------                    
 // Allocate storage for the string and copy the global szPathAndFilename                    
 // string into our recall array.
 // --------------------------------------------------------------------------                    
 pszRecall[usFilePos] = malloc(strlen(szPathAndFilename) + 1);                           
 strcpy(pszRecall[usFilePos], szPathAndFilename);                                        

 hwndSubMenu = AddCascadeMenu(hwndFrame);                                               
                                                                                          
 usFilePos++;                                                                             
 return;
}

// *******************************************************************************                 
// FUNCTION:     UserPreferencesDlgProc                                                            
//                                                                                                 
// FUNCTION USE: Dialog procedure used to configure the user preferences                           
//                                                                                                 
// DESCRIPTION:  This dialog procedure is used to allow the user to change                         
//               various program settings for the editor.  The dialog                              
//               contains radiobuttons that allow the user to enable or 
//               disable the Word Wrap option for the MLE edit window.  The 
//               changes are written immediately to the OS2.INI file.
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
MRESULT EXPENTRY UserPreferencesDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 CHAR szWordWrap[10];
 static LONG lWrapState;
 static LONG lFlag;

 switch (msg)
  {
   case WM_INITDLG:
        // --------------------------------------------------------------------------            
        // Remove the system menuitems that are not required.                                   
        // --------------------------------------------------------------------------            
        SetTheSysMenu(hwnd, NULLHANDLE);                   

        // --------------------------------------------------------------------------     
        // See if word wrap is set on.  The default is ON, so if no entry exists                    
        // in the OS2.INI file, the call to PrfQueryProfileInt will return FALSE,
        // which corresponds to Word Wrap = ON.
        // --------------------------------------------------------------------------     
        lWrapState = PrfQueryProfileInt(HINI_USER, APPNAME, WORDWRAP, 0);                    

        // --------------------------------------------------------------------------                          
        // Store the Word Wrap state away so that we can check if it changed when
        // the user selects the cancel button.
        // --------------------------------------------------------------------------                          
        lFlag = lWrapState;

        // --------------------------------------------------------------------------           
        // If Word Wrap is On check the ON radiobutton.
        // --------------------------------------------------------------------------            
        if (lWrapState == FALSE)
         {
          WinCheckButton(hwnd, IDR_WORDON, TRUE);             
         }

        // --------------------------------------------------------------------------            
        // If Word Wrap is Off check the OFF radiobutton.
        // --------------------------------------------------------------------------            
        else
         {
          WinCheckButton(hwnd, IDR_WORDOFF, TRUE);             
         }
        return FALSE;

   case WM_CONTROL:                                    
        switch VALUE                                   
        {                                              
         case IDR_WORDON:                            
         case IDR_WORDOFF:              
              // --------------------------------------------------------------------------                
              // Set the Word Wrap flag based on the radiobutton selected by the user.
              // --------------------------------------------------------------------------                
              lWrapState = VALUE;                          

              // --------------------------------------------------------------------------                      
              // Convert the VALUE to a string and write it to the OS2.INI file.                               
              // --------------------------------------------------------------------------                      
              _itoa (VALUE, szWordWrap, 10);            
              PrfWriteProfileString(HINI_USER, APPNAME, WORDWRAP, szWordWrap);         
              return FALSE;                            
        }

   case WM_COMMAND:
        switch (COMMANDMSG(&msg)->cmd)
         {
          case DID_OK:
               WinDismissDlg (hwnd, TRUE);
               return FALSE;

          case DID_CANCEL:
               if (lFlag != lWrapState) 
                {
                 // --------------------------------------------------------------------------    
                 // If the user selects the Cancel button restore the whatever option   
                 // was set when the dialog was initially displayed.
                 // --------------------------------------------------------------------------    
                 _itoa (lFlag, szWordWrap, 10);                                              
                 PrfWriteProfileString(HINI_USER, APPNAME, WORDWRAP, szWordWrap);            
                }
               WinDismissDlg (hwnd, TRUE);                            
               return FALSE;

          case DID_HELP:
               DisplayMessages(IDMSG_HELP, NULLHANDLE, MSG_INFO);         
               return FALSE;
         }
        break;
  }
 return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
}

// *******************************************************************************                 
// FUNCTION:     SetBackgroundColor                                                            
//                                                                                                 
// FUNCTION USE: Sets the background color of the MLE edit window.                             
//                                                                                                 
// DESCRIPTION:  This function converts the menuitem identifiers that represent                  
//               the background colors, into the actual background colors.                       
//               The function then changes the background color of the MLE 
//               window and writes the color to the OS2.INI file.
//
// PARAMETERS:   LONG     The menuitem representing the background color                           
//
// RETURNS:      VOID                                                                              
//                                                                                                 
// HISTORY:      mm-dd-yy                                                                          
//                                                                                                 
// *******************************************************************************                 
VOID SetBackgroundColor(LONG lColor)
{
 LONG lBackColor;
 LONG lCounter;
 CHAR szColor[10];
 HWND hwndSubMenu;

 // --------------------------------------------------------------------------                    
 // The lColor value that is passed in to this call is the menuitem 
 // identifier.  Ideally we would have used the actual CLR color values 
 // to correspond to the menuitem identifiers, but the recall menuitems 
 // use 0 through 4 which correspond to CLR_BACKGROUND, CLR_BLUE, CLR_RED,      
 // CLR_PINK, and CLR_GREEN.  This function maps the appropriate identifier 
 // back into its color value and then sends an MLM_SETBACKCOLOR message
 // to the edit window to change the color.  Also, the routine will write 
 // the color to the OS2.INI file, so that the color will be set whenever 
 // the MLE window is created.
 // --------------------------------------------------------------------------                    
 switch (lColor) 
  {
   case IDM_DEFAULT:
        lBackColor = SYSCLR_ENTRYFIELD;
        break;

   case IDM_WHITE:
        lBackColor = CLR_WHITE;
        break;

   case IDM_RED:                                                                             
        lBackColor = CLR_RED;
        break;

   case IDM_BLUE:                                                                            
        lBackColor = CLR_BLUE;
        break;

   case IDM_YELLOW:                                                                          
        lBackColor = CLR_YELLOW;                            
        break;

   case IDM_CYAN:                                                                            
        lBackColor = CLR_CYAN;                    
        break;
  }


 hwndSubMenu = WinWindowFromID(hwndFrame, FID_MENU);                      

 // --------------------------------------------------------------------------    
 // Uncheck all of the menuitems.             
 // --------------------------------------------------------------------------    
 for (lCounter = IDM_DEFAULT; lCounter <= IDM_CYAN; lCounter++)                         
  {                                                                               
   WinCheckMenuItem(hwndSubMenu, lCounter, FALSE);                                  
  }                                                                               

 // --------------------------------------------------------------------------                    
 // Check the selected menuitem.
 // --------------------------------------------------------------------------                    
 WinCheckMenuItem(hwndSubMenu, lColor, TRUE);               

 // --------------------------------------------------------------------------           
 // Change the MLE background color accordingly.
 // --------------------------------------------------------------------------           
 WinSendMsg(hwndMLE, MLM_SETBACKCOLOR, MPFROMLONG(lBackColor), NULL);                         

 // --------------------------------------------------------------------------               
 // Write the color to the OS2.INI file.                                                    
 // --------------------------------------------------------------------------               
 _itoa (lBackColor, szColor, 10);               
 PrfWriteProfileString(HINI_USER, APPNAME, BACKCOLOR, szColor);               
 return;
}

// *******************************************************************************                  
// FUNCTION:     MapColorToMenuitem                                                                
//                                                                                                  
// FUNCTION USE: Maps a CLR color value back into the menuitem value.                              
//                                                                                                  
// DESCRIPTION:  This routine is really not the greatest way to do this, but                      
//               in the interests of keeping it simple, here we go.  This routine                 
//               takes a color value CLR_ defined in PMGPI.H and maps it back                     
//               into the menuitem identifier that represents that color.  This                     
//               is done because the menuitem identifiers for the some colors 
//               would be duplicates of the recall menuitems. 
//                                                                                                  
// PARAMETERS:   LONG     Color value (for example CLR_RED).                                     
//                                                                                                  
// RETURNS:      LONG     Menuitem identifier (for example IDM_RED).                                
//                                                                                                  
// HISTORY:      mm-dd-yy                                                                           
//                                                                                                  
// *******************************************************************************                  
LONG MapColorToMenuitem(LONG lclrValue, BOOL bBack)
{
 switch (lclrValue) 
  {
     case SYSCLR_ENTRYFIELD:                  
          return IDM_DEFAULT;

     case CLR_WHITE:
          if (bBack) 
           return IDM_WHITE;      
          else
           return IDM_TXT_WHITE;                   

     case CLR_RED:
          if (bBack) 
           return IDM_RED;        
          else
           return IDM_TXT_RED;             

     case CLR_BLUE:
          if (bBack) 
           return IDM_BLUE;       
          else
           return IDM_TXT_BLUE;                    

     case CLR_YELLOW:
          if (bBack)                  
           return IDM_YELLOW;     
          else
           return IDM_TXT_YELLOW;                        

     case CLR_CYAN:
          return IDM_CYAN;       

     case CLR_BLACK:                                 
          return IDM_TXT_BLACK;                      

  }
}

// *******************************************************************************                   
// FUNCTION:     SetForegroundColor                                                                  
//                                                                                                   
// FUNCTION USE: Sets the text color of the MLE edit window.                                   
//                                                                                                   
// DESCRIPTION:  This function converts the menuitem identifiers that represent                      
//               the foreground colors, into the actual foreground colors.                           
//               The function then changes the foreground color of the MLE                           
//               window and writes the color to the OS2.INI file.                                    
//                                                                                                   
// PARAMETERS:   LONG     The menuitem representing the foreground color                             
//                                                                                                   
// RETURNS:      VOID                                                                                
//                                                                                                   
// HISTORY:      mm-dd-yy                                                                            
//                                                                                                   
// *******************************************************************************                   
VOID SetForegroundColor(LONG lColor)                                                                 
{                                                                                                    
 LONG lForeColor;                                                                                    
 LONG lCounter;                                                                                      
 CHAR szColor[10];                                                                                   
 HWND hwndSubMenu;                                                                                   
                                                                                                     
 // --------------------------------------------------------------------------                       
 // The lColor value that is passed in to this call is the menuitem                                  
 // identifier.  Ideally we would have used the actual CLR color values                              
 // to correspond to the menuitem identifiers, but the recall menuitems                              
 // use 0 through 4 which correspond to CLR_BACKGROUND, CLR_BLUE, CLR_RED,                           
 // CLR_PINK, and CLR_GREEN.  This function maps the appropriate identifier                          
 // back into its color value and then sends an MLM_SETBACKCOLOR message                             
 // to the edit window to change the color.  Also, the routine will write                            
 // the color to the OS2.INI file, so that the color will be set whenever                            
 // the MLE window is created.                                                                       
 // --------------------------------------------------------------------------                       
 switch (lColor)                                                                                     
  {                                                                                                  
   case IDM_TXT_BLACK:                                                                                 
        lForeColor = CLR_BLACK;                                                              
        break;                                                                                       
                                                                                                     
   case IDM_TXT_WHITE:                                                                                   
        lForeColor = CLR_WHITE;                                                                      
        break;                                                                                       
                                                                                                     
   case IDM_TXT_RED:                                                                                     
        lForeColor = CLR_RED;                                                                        
        break;                                                                                       
                                                                                                     
   case IDM_TXT_BLUE:                                                                                    
        lForeColor = CLR_BLUE;                                                                       
        break;                                                                                       
                                                                                                     
   case IDM_TXT_GREEN:                                                                                  
        lForeColor = CLR_GREEN;                                                                       
        break;                                                                                       
                                                                                                     
   case IDM_TXT_YELLOW:                                                                                    
        lForeColor = CLR_YELLOW;                                                                     
        break;                                                                                       
  }                                                                                                  
                                                                                                     
                                                                                                     
 hwndSubMenu = WinWindowFromID(hwndFrame, FID_MENU);                                                 
                                                                                                     
 // --------------------------------------------------------------------------                       
 // Uncheck all of the menuitems.                                                                    
 // --------------------------------------------------------------------------                       
 for (lCounter = IDM_TXT_BLACK; lCounter <= IDM_TXT_YELLOW; lCounter++)                                      
  {                                                                                                  
   WinCheckMenuItem(hwndSubMenu, lCounter, FALSE);                                                   
  }                                                                                                  
                                                                                                     
 // --------------------------------------------------------------------------                       
 // Check the selected menuitem.                                                                     
 // --------------------------------------------------------------------------                       
 WinCheckMenuItem(hwndSubMenu, lColor, TRUE);                                                        
                                                                                                     
 // --------------------------------------------------------------------------                       
 // Change the MLE background color accordingly.                                                     
 // --------------------------------------------------------------------------                       
 WinSendMsg(hwndMLE, MLM_SETTEXTCOLOR, MPFROMLONG(lForeColor), NULL);                    
                                                                                                     
 // --------------------------------------------------------------------------                       
 // Write the color to the OS2.INI file.                                                             
 // --------------------------------------------------------------------------                       
 _itoa (lForeColor, szColor, 10);                                                                    
 PrfWriteProfileString(HINI_USER, APPNAME, FORECOLOR, szColor);                                      
 return;                                                                                             
}                                                                                                    
