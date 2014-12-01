// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      BUTTONS.C                                        mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Main Source File for BUTTONS/Chapter 02 Sample Program        บ
// บ                                                                               บ
// บ  NOTES:         This file contains the source code for the BUTTONS program.   บ
// บ                 BUTTONS is a simple program launcher that can be used as      บ
// บ                 an individual shell.                                          บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ


#define INCL_WINWINDOWMGR
#define INCL_WINMESSAGEMGR
#define INCL_WININPUT
#define INCL_WINDIALOGS
#define INCL_WINFRAMEMGR
#define INCL_WINSHELLDATA
#define INCL_WINHELP
#define INCL_WINBUTTONS
#define INCL_WINSTDFILE
#define INCL_WINSTATICS
#define INCL_WINENTRYFIELDS
#define INCL_WINLISTBOXES
#define INCL_WINPROGRAMLIST
#define INCL_WINPOINTERS
#define INCL_WINSYS
#define INCL_WINSWITCHLIST
#define INCL_GPICONTROL
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES
#define INCL_DOSERRORS
#define INCL_DOSFILEMGR
#define INCL_DOSMISC

#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <process.h>
#include "messages.h"
#include "shcommon.h"
#include "buttons.h"
#include "buttons.fnc"


// --------------------------------------------------------------------------       
// Miscellaneous global variables 
// --------------------------------------------------------------------------       
HAB      hab;
HMQ      hmq;
HWND     hwndClient;
HWND     hwndFrame;

// --------------------------------------------------------------------------       
// This variable is used to determine if a dialog is already up so             
// we can simply switch to it, rather than recreate it
// --------------------------------------------------------------------------       
BOOL bAlreadyRunning  = FALSE;

// --------------------------------------------------------------------------       
// Used for loop control                                                         
// --------------------------------------------------------------------------       
BOOL false = FALSE;    

// --------------------------------------------------------------------------       
// Functions that are specific for this file                                      
// --------------------------------------------------------------------------       
PSZ  SubstituteBootDrive(PSZ pszFilename);                
BOOL PopulateListBox(HWND hwndListBox);
BOOL EXPENTRY FillComboBoxes(HWND hwndCombo);                                                          
VOID EXPENTRY ReadExecutablesFromProfile(PUCHAR szExecutable);


// --------------------------------------------------------------------------       
// These arrays are extremely important, they contain                           
// 
// 1. The Program names of the programs to be started
// 2. The parameters that can be passed to the programs
// 3. The program types 
// --------------------------------------------------------------------------       
PSZ    pszAppName[MAX_EXEC];
PSZ    pszParams[MAX_EXEC];
ULONG  ulPgmType[MAX_EXEC];

USHORT   BtnIdxToType[ ] = {0,3,2,1,7,4,18,19};
USHORT   TypeToBtnIdx[ ] = {0,3,2,1,5,0,0,4,0,0,0,0,0,0,0,0,0,0,6,7,0};



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
int main(VOID)
{
  QMSG        qmsg;
  BOOL        bSuccess;
  LONG        lcx;
  LONG        lcy;
  LONG        lcxScreen;
  LONG        lcyScreen;
  SWP         swp;
  ULONG       flCreate;
  CHAR        szWinTitle[100];
  CHAR        szBuffer[CCHMAXPATH];
  USHORT      usCounter;
  HMTX        hMtxSem;
  APIRET      rc;
  BTNCDATA    btncd;

  ULONG       SwpLen = sizeof(SWP);             
  static      HPOINTER hptrTemp[100];

  // --------------------------------------------------------------------------       
  // Define three static arrays to hold window text, and button window coordinates
  // --------------------------------------------------------------------------       
  static  CHAR   *szResourceNumber[] = {"#101", "#602", "#603", "#604", "", "", "", "", "", "", "", ""};
  static  USHORT usXPosition[12] = {0, 50, 100, 150,  0, 50, 100, 150,   0, 50, 100, 150};
  static  USHORT usYPosition[12] = {0,  0,   0,   0, 50, 50,  50,  50, 100, 100, 100, 100};

  hab = WinInitialize(NULLHANDLE);
  if(!hab)
  {
    DosBeep(60, 100);
    exit(TRUE);
  }

  hmq = WinCreateMsgQueue(hab, NULLHANDLE);
  if(!hmq)
  {
    DosBeep(60, 100);
    WinTerminate(hab);
    exit(TRUE);
  }

  // Since BUTTONS is a program that can be used as a replacement to 
  // the workplace shell, we want to prevent the user from starting 
  // multiple copies of BUTTONS.  To do this we will create a mutex
  // semaphore that will exist throughout the lifetime of the program.
  // If another application(in our case another instance of BUTTONS) 
  // comes along and tries to create a mutex semaphore with the same
  // name then we will terminate the second instance of BUTTONS after
  // telling the user this cannot be done.
  rc = DosCreateMutexSem ("\\SEM32\\Buttons", &hMtxSem, 0, TRUE);
  if (rc == ERROR_DUPLICATE_NAME)
   {
    DisplayMessages(ERROR_ALREADY_RUNNING, NULLHANDLE, MSG_ERROR);
    exit(TRUE);
   }

  bSuccess = WinRegisterClass(hab,
                              CLASSNAME,          // Window Class Name
                              ClientWndProc,      // Address of window procedure
                              CS_SIZEREDRAW,      // Class style
                              0);                 // No extra window words

  if (!bSuccess)
  {
   DisplayMessages(NULLHANDLE, "Unable to Register Window Class", MSG_ERROR);
   exit(TRUE);
  }


  flCreate = FCF_TITLEBAR | FCF_TASKLIST | FCF_SIZEBORDER | FCF_MINBUTTON | FCF_SYSMENU |
             FCF_MENU     | FCF_ICON     | FCF_ACCELTABLE ;

  hwndFrame = WinCreateStdWindow(HWND_DESKTOP,     // Parent Window is Desktop
                                 NULLHANDLE,       // Frame Window Style
                                 &flCreate,        // Frame Control Flags
                                 CLASSNAME,        // Window Class Name
                                 TITLEBAR,         // Window Title Bar
                                 NULLHANDLE,       // Client Window Style
                                (HMODULE)NULL,     // Resource is in .EXE file
                                 ID_MAINWND,       // Frame window identifier
                                 &hwndClient);     // Client window handle

  if (!hwndFrame)
   {
    DisplayMessages(NULLHANDLE, "Error Creating Frame Window", MSG_ERROR);
    exit(TRUE);
   }

  lcxScreen = WinQuerySysValue (HWND_DESKTOP, SV_CXSCREEN);      
  lcyScreen = WinQuerySysValue (HWND_DESKTOP, SV_CYSCREEN);      

  // Obtain window position                                                 
  WinQueryWindowPos(hwndFrame, &swp);                                
                                                                            
  // Center our frame window                                                
  lcx = (lcxScreen - swp.cx ) / 4;                                  
  lcy = (lcyScreen - swp.cy ) / 4;                                  
                                                                            
  WinSetWindowPos(hwndFrame,                                                
                  NULLHANDLE,                                               
                  lcx,                                                      
                  lcy,                                                      
                  lcx,                                                  
                  lcy,                                                  
                  SWP_SIZE | SWP_HIDE | SWP_MOVE);                          

  // Check to see if the user has asked that the application remove
  // frame controls on startup.  If the answer is yes, then we 
  // need to remove the controls via a call to HideControls
  rc = PrfQueryProfileInt(HINI_USER, APPNAME, KEYNAME1, 0);     
  if (rc == CHECKED)                                                         
   {                                                                              
    HideControls(hwndFrame); 
   }

  rc = PrfQueryProfileInt(HINI_USER, APPNAME, KEYNAME2, 0);      
  if (rc == CHECKED)                                             
   {                                                             
    SWCNTRL swctl;                                                           
    HSWITCH hswitch;
                                                                             
    hswitch = WinQuerySwitchHandle(hwndFrame, 0);                            
    WinRemoveSwitchEntry(hswitch);                                                                                                        
   }


  // Check the OS2.INI file to see if window coordinates exist?    
  // If no entry is found, just use our current coordinates, but   
  // make our window visible.                                      
  if (!PrfQueryProfileData(HINI_PROFILE, APPNAME, "WindowPos", &swp, &SwpLen))
   {
    WinSetWindowPos(hwndFrame, HWND_TOP, 0, 0, 0, 0, SWP_SHOW);
   }

  // If an entry is in the OS2.INI file then use it to set our window coordinates
  else  
   {
    WinRestoreWindowPos(APPNAME, "WindowPos", hwndFrame);
   }


  // Fill in most of the Buttons Control Data Structure
  btncd.cb             = sizeof(BTNCDATA);
  btncd.fsCheckState   = NULLHANDLE;
  btncd.fsHiliteState  = NULLHANDLE;

  // Parse the BUTTONS profile(BUTTONS.PRO) looking for the executables to be 
  // started, along with the session types and any command line parameters.
  ReadExecutablesFromProfile(BUTTONSPRO);

  // The function buttons(first/bottom row of four buttons) correspond   
  // to Product-Information/Lockup/Shutdown/Command Prompt.  These icons
  // are going to be loaded as resources from our executable.  To load 
  // these icons using the BS_ICON button style we will need to specify
  // the resource number preceeded by a pound sign as the window text
  // on the call to WinCreateWindow.  The remaining eight buttons are
  // known as the program buttons, and need no window text.
  for (usCounter = BID_ABOUT; usCounter < BID_END; usCounter++)
   {
     strcpy(szWinTitle, szResourceNumber[usCounter]);

     // For our program buttons (all buttons greater than the first row)
     // get the icon from the executable via WinLoadFileIcon().  If 
     // the pointer handle returned by WinLoadFileIcon is NULL, we were
     // unable to get a valid icon from the various methods used by 
     // WinLoadFileIcon, which probably implies that the path and 
     // filename of the executable is bogus.  In which case, we will 
     // load the SPTR_ICONQUESTION default pointer(a simple question mark)
     // to alert the user that something is wrong.
     if (usCounter > BID_VIOCMD)
      {
       hptrTemp[usCounter] = WinLoadFileIcon(pszAppName[usCounter], FALSE);
       if (hptrTemp[usCounter] == NULLHANDLE)
        {
         hptrTemp[usCounter] = WinQuerySysPointer(HWND_DESKTOP, SPTR_ICONQUESTION, FALSE);
        }
       btncd.hImage = (LHANDLE)hptrTemp[usCounter];
      }

     // Ok, so here we are about to create the BUTTONS control panel which consists of 
     // twelve pushbuttons.  The function buttons(first row) are non-configurable unlike
     // the remaining eight buttons(known as the program buttons).  The buttons are a 
     // fixed size 50 x 50, beginning at the origin that is referenced from the 
     // usXPosition and usYPosition arrays.  This is where we pass the button control
     // data structure which contains the icon information for the program buttons.
     WinCreateWindow (hwndClient,                             // Parent Window Handle
                      WC_BUTTON,                              // Class Name
                      szWinTitle,                             // Window Text
                      WS_VISIBLE | BS_PUSHBUTTON | BS_ICON,   // Window Styles
                      usXPosition[usCounter],                 // Initial X coordinate
                      usYPosition[usCounter],                 // Initial Y coordinate
                      50,                                     // Horizontal Length of Button
                      50,                                     // Vertical Length of Button
                      NULLHANDLE,                             // Owner Window Handle
                      HWND_BOTTOM,                            // Sibling Window
                      usCounter,                              // Resource Identifier
                     (PVOID)&btncd,                           // Button Control Data
                      NULL);                                  // Presentation Parameters
   }


  while( WinGetMsg(hab, &qmsg, (HWND)NULL, 0, 0))
    WinDispatchMsg(hab, &qmsg);



  // Destroy Window, Message Queue and Terminate the application gracefully
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
// HISTORY:  
// 
// *******************************************************************************
MRESULT EXPENTRY ClientWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  HPS          hps;
  PSZ          pszCommand;
  APIRET       rc;
  LONG         lReturn;
  CHAR         szBuffer[CCHMAXPATH];
  CHAR         szCommandLine[CCHMAXPATH] = "";
  HWND         hwndActive;

  HWND         hwndProgram;
  ULONG        ulSID = 0;

  switch(msg)
  {
    case WM_COMMAND:
      switch (COMMANDMSG(&msg)->cmd)
      {
        case IDM_ABOUT:
        case BID_ABOUT:
             WinDlgBox (HWND_DESKTOP, hwnd, ProdInfoDlgProc, NULLHANDLE, IDD_ABOUT, NULL);
             break;

        case IDM_SETUP:
             // Configuring the program buttons, is something that should not happen
             // frequently.  The owner and parent of the Setup dialog box are purposefully
             // set to HWND_DESKTOP.  The owner is set to the desktop to allow the dialog
             // box to function without the restriction of being application modal.  If 
             // the user selects the menuitem corresponding to this dialog again, we 
             // do not want to display the dialog again, so in that case we will merely
             // make the dialog the active window.  If it is currently minimized, activate
             // and restore it.
             if (bAlreadyRunning) 
              {
               hwndActive = WinWindowFromID(HWND_DESKTOP, IDD_SETUP);
               WinSetWindowPos(hwndActive, HWND_TOP, 0, 0, 0, 0, SWP_RESTORE | SWP_SHOW | SWP_ACTIVATE | SWP_ZORDER);     
               return FALSE;
              } 

             else
              {
               WinDlgBox(HWND_DESKTOP, HWND_DESKTOP, SetupDlgProc, NULLHANDLE, IDD_SETUP, NULL);            
              }
             break;

        case IDM_CONFIGURE:
             WinDlgBox (HWND_DESKTOP, hwnd, ConfigureDlgProc, NULLHANDLE, IDD_CONFIGURE, NULL);
             break;

        case IDM_PROGLIST:
             WinDlgBox(HWND_DESKTOP, hwnd, ViewProgramListDlgProc, NULLHANDLE, IDD_PROGLIST, NULL);
             break;

        case IDM_CMDBUTTON:
             WinDlgBox(HWND_DESKTOP, HWND_DESKTOP, CommandButtonDlgProc, NULLHANDLE, IDD_COMMAND, NULL);
             break;

        case IDM_COMSPEC:                                                                              
             WinDlgBox(HWND_DESKTOP, hwnd, ConfigureComSpecDlgProc, NULLHANDLE, IDD_COMSPEC, NULL); 
             break;                                                                                      
          
        case BID_SHUTDOWN:
             // The user must be getting tired and wants to go home, so they have asked
             // us to shutdown their system.  Ask for their confirmation, and proceed 
             // with the lockup if the answer is yes!
             rc = DisplayMessages(QUESTION_SHUTDOWN,
                                  NULLHANDLE,
                                  MSG_WARNING);

             if (rc == MBID_YES)
              {
               WinShutdownSystem(hab, hmq);
              }
             return FALSE;

        case BID_LOCKUP:
             // OS/2 2.1, finally gave us an API that will perform the workplace shell
             // lockup feature.  If the user clicks on the lockup buttons, let's lockup.
             WinLockupSystem(hab);
             break;

        case BID_VIOCMD:
             // This button option corresponds to the command button.  The command button
             // will either display an OS/2 windowed command prompt, or a dialog box 
             // that can be used to run utility programs like chkdsk, format etc....
             // The user can configure which option they prefer via the Configure user
             // Preferences dialog box.  The default is the OS/2 command prompt.

             // If the user wants the command line dialog, display it...
             rc = PrfQueryProfileInt(HINI_USER, APPNAME, KEYNAME4, 0);            
             if (rc == CHECKED)                                                   
              {                                                                   
               WinDlgBox(HWND_DESKTOP, HWND_DESKTOP, CommandButtonDlgProc, NULLHANDLE, IDD_COMMAND, NULL);
              }                                                                   

             // If the user wants an OS/2 command prompt, scan the environment to find
             // the currently installed command interpreter(most likely CMD.EXE), but 
             // just in case the user made a change the COMSPEC environment variable
             // will reflect the user configured command interpreter.  There are 
             // already quite a few good, replacement command interpreters that provide
             // the user with additional functionality.  Also, the user may want to 
             // pass a command file to the command interpreter everytime it starts,
             // like to change colors for the command session.  We have given the 
             // user the ability to specify a parameter that they wish to pass to 
             // the command session via the "Configure Command Interpreter" option.
             // If any parameters are set, use them!  Also, set the session title 
             // to OS/2 Window, just for the heck of it!
             else
              {
               DosScanEnv("COMSPEC", &pszCommand);
               lReturn = PrfQueryProfileString(HINI_USER, APPNAME, KEYCOMSPEC, NULL, (PVOID)szBuffer, (LONG)sizeof(szBuffer));    
               if (lReturn != 0)  
                {                 
                 strcpy(szCommandLine, szBuffer);
                }
               ulSID = StartApplication(pszCommand, szCommandLine, "OS/2 Window", 0);

               if (!ulSID)                                                             
                {                                                                      
                 DisplayMessages(ERROR_STARTING_PROGRAM, NULLHANDLE, MSG_EXCLAMATION); 
                }                                                                      
              }
             return FALSE;

        // These are the program buttons, indicating that the user wants to start
        // a program, so let's oblige!
        case BID_PROGRAM1:
        case BID_PROGRAM2:
        case BID_PROGRAM3:
        case BID_PROGRAM4:
        case BID_PROGRAM5:
        case BID_PROGRAM6:
        case BID_PROGRAM7:
        case BID_PROGRAM8:
             if (pszAppName[CMD_MSG]) 
              {
               ulSID = StartApplication(pszAppName[CMD_MSG], pszParams[CMD_MSG], 0, ulPgmType[CMD_MSG]);
              }

             if (!ulSID)                                                                    
              {                                                                             
               DisplayMessages(ERROR_STARTING_PROGRAM, NULLHANDLE, MSG_EXCLAMATION);        
               return FALSE;                                                                  
              }                                                                             


             // The StartApplications routine returns a HAPP which is actually the 
             // session identifier.  We will store the HAPP in the window words of 
             // button window, that way we can call WinTerminateApp if the user 
             // wants to kill a program they started from BUTTONS.
             hwndProgram = WinWindowFromID(hwnd, CMD_MSG);
             WinSetWindowULong(hwndProgram, QWL_USER, ulSID);

             return FALSE;

        case IDM_HIDECTLS:
             HideControls(hwndFrame);
             WinSetFocus(HWND_DESKTOP, HWND_DESKTOP);
             WinSetFocus(HWND_DESKTOP, hwndFrame);
             WinInvalidateRect(hwnd, NULL, FALSE);
             return FALSE;

        case IDM_SAVEWIN:
             rc = DisplayMessages(QUESTION_SAVEWINPOS, NULLHANDLE, MSG_WARNING);
             if (rc == MBID_YES)
              {
               DosBeep(300, 100);
               WinStoreWindowPos(APPNAME, "WindowPos", hwndFrame);
               DisplayMessages(NULLHANDLE, "Window values Saved in OS2.INI", MSG_INFO);
              }
             return FALSE;

        case IDM_HELPGEN:       
        case IDM_HELPKEYS:      
             DisplayMessages(INFO_NOHELP, NULLHANDLE, MSG_INFO);               
             return FALSE;

        case IDM_EXIT:
             rc = PrfQueryProfileInt(HINI_USER, APPNAME, KEYNAME3, 0);        
             if (rc == CHECKED)                                               
              {                                                               
               rc = DisplayMessages(QUESTION_EXIT, NULLHANDLE, MSG_WARNING);                                       
               if (rc == MBID_YES)                                                             
                {                                                                              
                 WinPostMsg(hwnd, WM_QUIT, MPFROMLONG(NULL), MPFROMLONG(NULL));                
                }                                                                              
               return FALSE;                                                                   
              }                                                               

             else
              {
               WinPostMsg(hwnd, WM_QUIT, MPFROMLONG(NULL), MPFROMLONG(NULL));
              }
             return FALSE;
      }
      break;

    case WM_PAINT:
         hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);
         GpiErase(hps);
         WinEndPaint(hps);
         return FALSE;

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
           case DID_OK:
                WinDismissDlg(hwnd, TRUE);
                break;

           case DID_FRAME:
                WinSendMsg(WinWindowFromID(hwndFrame, FID_CLIENT), WM_COMMAND, (MPARAM)IDM_HIDECTLS, (MPARAM)NULL);
                break;

           case DID_SAVE:
                WinDismissDlg(hwnd, TRUE);
                rc = DisplayMessages(NULLHANDLE, "Save Current Window Size and Position?", MSG_WARNING);
                if (rc == MBID_YES)
                 {
                  DosBeep(300, 100);
                  WinStoreWindowPos(APPNAME, "WindowPos", hwndFrame);
                  DisplayMessages(NULLHANDLE, "Window values Saved in OS2.INI", MSG_WARNING);
                 }
                return FALSE;
          }
         break ;
   }
 return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
}


// *******************************************************************************
// FUNCTION:     ViewProgramListDlgProc
//
// FUNCTION USE: Display a listing of all of the applications in profile
//
// DESCRIPTION:  This routine uses the listbox control to display all of the
//               programs contained in the BUTTONS profile.  
//               
//               There are three pushbuttons in this dialog:
//                  OK:    Just dismisses the dialog
//                  Kill:  The "Kill" pushbutton will, you guessed it
//                         kill the application selected in the listbox
//                  Help:  This should display help text, but for the purpose
//                         of this sample program will merely display a message
//                         box indicating there is no help
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
MRESULT EXPENTRY ViewProgramListDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 USHORT usIndex;
 ULONG  ulSID;
 HWND   hwndListbox;
 HWND   hwndButton;
 BOOL   rc;

 static CHAR   szBuffer[CCHMAXPATH];


 switch (msg)
  {
   case WM_INITDLG:
        SetTheSysMenu(hwnd, NULL);
        hwndListbox = WinWindowFromID(hwnd, IDL_PROGLIST);
        rc = PopulateListBox(hwndListbox);

        if (!rc) 
         {
          DisplayMessages(ERROR_POPULATING_LISTBOX, NULLHANDLE, MSG_EXCLAMATION);
         } 
        return FALSE;

   case WM_CONTROL:
        if (SHORT1FROMMP (mp1) == IDL_PROGLIST) 
         {
          usIndex = (USHORT) WinSendDlgItemMsg(hwnd, SHORT1FROMMP(mp1), LM_QUERYSELECTION, 0L, 0L);
          WinSendDlgItemMsg(hwnd, SHORT1FROMMP(mp1), LM_QUERYITEMTEXT, MPFROM2SHORT(usIndex, sizeof szBuffer), MPFROMP(szBuffer));

          switch (SHORT2FROMMP (mp1)) 
           {
            case LN_ENTER:
                 ulSID = StartApplication(szBuffer, NULL, NULL, 0);                       

                 if (!ulSID)                                                                   
                  {                                                                            
                   DisplayMessages(ERROR_STARTING_PROGRAM, NULLHANDLE, MSG_EXCLAMATION);       
                   return FALSE;                                                               
                  }                                                                            
                                                                                           
                 // The StartApplications routine returns a HAPP which is actually the         
                 // session identifier.  We will store the HAPP in the window words of         
                 // button window, that way we can call WinTerminateApp if the user            
                 // wants to kill a program they started from BUTTONS.                         
                 usIndex += 4;
                 hwndButton = WinWindowFromID(hwndClient, usIndex);                                 
                 WinSetWindowULong(hwndButton, QWL_USER, ulSID);                              
           }
         } 
        return FALSE;

   case WM_COMMAND:
        switch (COMMANDMSG(&msg)->cmd)
         {
          case DID_KILL:
               usIndex = (USHORT) WinSendDlgItemMsg(hwnd, IDL_PROGLIST, LM_QUERYSELECTION, MPFROM2SHORT(4, 0), 0);   
               usIndex += 4;

               hwndButton = WinWindowFromID(hwndClient, usIndex);
               ulSID = WinQueryWindowULong(hwndButton, QWL_USER);                     

               if (ulSID) 
                {
                 // User has asked to kill the application
                 WinTerminateApp(ulSID);

                 // Reset window words so that the next time the user
                 // asks to kill the program and it is not started, they
                 // get the error message indicating that the program is 
                 // not currently running.
                 WinSetWindowULong(hwndButton, QWL_USER, NULLHANDLE);          
                }

               else
                {
                 DisplayMessages(ERROR_PROGRAM_NOT_RUNNING, NULLHANDLE, MSG_EXCLAMATION); 
                }

               return FALSE;

          case DID_HELP:
               DisplayMessages(NULLHANDLE, "Help Not Available", MSG_INFO);
               return FALSE;

          case DID_OK:
          case DID_CANCEL:
               WinDismissDlg(hwnd, TRUE);
               break;
         }
        break ;
  }
 return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
}

// *******************************************************************************
// FUNCTION:     SetupDlgProc           
//
// FUNCTION USE: Dialog procedure used to configure all of the program buttons
//
// DESCRIPTION:  This routine is by far the most complex of our dialog box  
//               procedures.  This procedure is used to configure all of the    
//               program buttons.                                             
//
//               The dialog contains two entryfields:
//                Program File: Used to obtain executable path and filename (IDE_EXECUTABLE)
//                Parameters:   Used to obtain any command line parameters  (IDE_PARAMETERS)
//
//               The dialog contains three pushbuttons:
//                Find:    Used to display standard file dialog to find a program      
//                Icon:    Used to display standard file dialog to change icon
//                Default: Used to restore icon to default icon for executable
// 
//               There are two other pushbuttons that navigate through the      
//               program list. The button text looks like this:
//                [1] navigate backward <<
//                [2] navigate forward  >> 
//        
//               There are four other pushbuttons:
//                Apply:  Updates the program list in memory but does not 
//                        write the changes to the BUTTONS profile (BUTTONS.PRO)
//                Save:   Saves the current program list to the BUTTONS profile.
//                Close:  Simply dismisses the dialog (note the use of the word 
//                        "Close" in lieu of "Cancel".  A cancel button would imply
//                        that any operation done would be undone when the dialog 
//                        is dismissed.
//                Help:   This would normally display help text, but since we 
//                        don't cover the IPFC stuff yet, we won't use any help.
//               
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
MRESULT EXPENTRY SetupDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 HPOINTER        hptr;
 HWND            hwndButton;
 HWND            hwndEntryField;
 CHAR            szBuffer[CCHMAXPATH];
 FILEDLG         filedlg;
 HFILE           hfFile;
 ULONG           ulAction;
 ULONG           ulWritten;
 ULONG           ulLength;
 USHORT          usCounter;
 BOOL            rc;


 static CHAR     szEntryField[CCHMAXPATH];
 static SHORT    sCounter;


 switch (msg)
  {
   case WM_INITDLG:
        CenterDialog(hwnd);
        bAlreadyRunning = TRUE;

        sCounter = BID_PROGRAM1;
        hwndButton = WinWindowFromID(hwnd, ID_MAINWND);

        WinSendDlgItemMsg (hwnd, IDE_EXECUTABLE,  EM_SETTEXTLIMIT, MPFROM2SHORT (CCHMAXPATH, 0), NULL);
        WinSetDlgItemText (hwnd, IDE_EXECUTABLE, pszAppName[sCounter]);

        WinSendDlgItemMsg (hwnd, IDE_PARAMETERS,  EM_SETTEXTLIMIT, MPFROM2SHORT (CCHMAXPATH, 0), NULL);
        WinSetDlgItemText (hwnd, IDE_PARAMETERS, pszParams[sCounter]);
           
        hptr = WinLoadFileIcon(pszAppName[sCounter], FALSE);

        if (hptr) 
         {
          WinSendMsg(hwndButton, SM_SETHANDLE, (MPARAM)hptr, NULL);
          WinSendDlgItemMsg(hwnd, IDR_DEFAULT + TypeToBtnIdx[ulPgmType[sCounter]], BM_SETCHECK, MPFROMSHORT(1), MPFROMLONG(0L)); 
           
          strcpy(szEntryField, pszAppName[sCounter]);
         }

        else
         {
          WinSetFocus(HWND_DESKTOP, WinWindowFromID(hwnd, IDE_EXECUTABLE));
          WinSendDlgItemMsg(hwnd, IDR_DEFAULT, BM_SETCHECK, MPFROMSHORT(1), MPFROMLONG(0L)); 
          UpdateIcon(hwnd, ID_MAINWND, NULLHANDLE);                       
          return (MRESULT)TRUE;
         }
        return FALSE;

   case WM_CONTROL:
        // szEntryField contains the text in the Program File entryfield.
        // at any given time.
         
        // For any executable program the default program type is used 
        // until the user specifically changes the program type via the 
        // Program Type radiobuttons.  This code ensures that if the 
        // user changes the text in the Program File entryfield, the 
        // Program Type default radiobutton is reset.  

        // For instance:
        // If the Program File entryfield contained X:\\OS2\\E.EXE and
        // the Program Type radiobutton indicated that the program type 
        // was a PM program, the Program Type will be reset to indicate
        // a default Program Type once the user changes the text in the 
        // entryfield and moves the edit cursor outside the entryfield.

        // This piece of logic is needed to "idiot proof" the program 
        // since it is possible that the user could have a DOS program 
        // type for the current program, but change the program to 
        // a PM application and then click the Apply button.  Without this
        // logic the PM program would still attempt to start with the 
        // DOS program type.


        // First things first, check to see that we have gotten the 
        // WM_CONTROL message as a result of the Program File entryfield.
        if (SHORT1FROMMP(mp1) == IDE_EXECUTABLE)     
         {
          switch (SHORT2FROMMP(mp1)) 
           {                         
            // Now check to see if the user has changed the focus 
            // away from the Program File entryfield by handling the 
            // EN_KILLFOCUS notification message.
            case EN_KILLFOCUS:       

                 // Since focus is switching away from the entryfield
                 // check to see if the actual text has been modified 
                 // by the user.  The new text is stored in szBuffer 
                 // and will be compared to the original text 
                 // that is stored in szEntryField.
                 hwndEntryField = WinWindowFromID(hwnd, IDE_EXECUTABLE);   
                 WinQueryWindowText(hwndEntryField, sizeof(szBuffer), szBuffer);                          

                 rc = strcmp(szEntryField, szBuffer);                 
                 if (rc) 
                  {
                   // If the text of the entryfield after the entryfield focus switch 
                   // does not match the original program file, this means the 
                   // user has modified the text in the entryfield, so we want 
                   // to reset the Program Type radiobuttons to indicate that the 
                   // program type is the default.  Of course, all of this will be 
                   // lost if the user does not select the Apply/Save pushbuttons to 
                   // record the changes for this session or permanently.
                   WinSendDlgItemMsg(hwnd, IDR_DEFAULT, BM_SETCHECK, MPFROMSHORT(1), MPFROMLONG(0L));                   

                   // A little recursion here.  We first call UpdateIcon to try and  
                   // set the icon based on the text in the entryfield.  If all goes 
                   // well the entryfield contains a valid path and filename for an 
                   // executable program.  However, if no icon can be pulled from the 
                   // executable, it probably means our EXE program is totally bogus 
                   // so we will call UpdateIcon again, only this time with a NULLHANDLE
                   // as the last parameter.  A NULLHANDLE indicates that the static icon
                   // should be changed to the system pointer for a question mark???
                   // This little hack is much faster than actually validating the 
                   // executable...  The question mark icon provides a visual clue to 
                   // the user that the path and filename for the executable is probably
                   // not valid and therefore, they should not Apply/Save the changes.
                   rc = UpdateIcon(hwnd, ID_MAINWND, szBuffer);         
                   if (!rc) 
                    {
                     UpdateIcon(hwnd, ID_MAINWND, NULLHANDLE);
                    } 
                  }
                 return FALSE;      
           }                         
         }
        break;

   case WM_DESTROY:
        bAlreadyRunning = FALSE;
        rc = WinFreeFileIcon(hptr);
        if (rc != TRUE)
         {
          DisplayMessages(NULLHANDLE, "Error Destroying Icon", MSG_ERROR);
         }
        break;

   case WM_COMMAND:
        switch (COMMANDMSG(&msg)->cmd)
         {
          case DID_RIGHT:
               if (!WinIsControlEnabled(hwnd, DID_LEFT))
                 WinEnableControl(hwnd, DID_LEFT, TRUE);

               if (sCounter < BID_PROGRAM8)
                {
                 sCounter++;
                 UpdateIcon(hwnd, ID_MAINWND, pszAppName[sCounter]);

                 WinSetDlgItemText(hwnd, IDE_EXECUTABLE, pszAppName[sCounter]);
                 WinSetDlgItemText(hwnd, IDE_PARAMETERS, pszParams[sCounter]);
                 WinSendDlgItemMsg(hwnd,
                                   IDR_DEFAULT + TypeToBtnIdx[ulPgmType[sCounter]],
                                   BM_SETCHECK,
                                   MPFROMSHORT(1),
                                   MPFROMLONG(0L));

                 if (pszAppName[sCounter]) 
                  {
                   strcpy(szEntryField, pszAppName[sCounter]);    
                  }
                }

               else
                {
                 WinEnableControl(hwnd, DID_RIGHT, FALSE);
                }
               return FALSE;

          case DID_LEFT:
               if (!WinIsControlEnabled(hwnd, DID_RIGHT))
                 WinEnableControl(hwnd, DID_RIGHT, TRUE);

               if (sCounter > BID_PROGRAM1)
                {
                 sCounter--;
                 UpdateIcon(hwnd, ID_MAINWND, pszAppName[sCounter]);
                 WinSetDlgItemText(hwnd, IDE_EXECUTABLE, pszAppName[sCounter]);
                 WinSetDlgItemText(hwnd, IDE_PARAMETERS, pszParams[sCounter]);
                 WinSendDlgItemMsg(hwnd,
                                   IDR_DEFAULT + TypeToBtnIdx[ulPgmType[sCounter]],
                                   BM_SETCHECK,
                                   MPFROMSHORT(1),
                                   MPFROMLONG(0L));

                 if (pszAppName[sCounter])  
                  {                             
                   strcpy(szEntryField, pszAppName[sCounter]);     
                  }
                }

               else
                {
                 WinEnableControl(hwnd, DID_LEFT, FALSE);
                }
               return FALSE;

          case DID_FIND:
               memset(&filedlg, 0, sizeof(filedlg));
               filedlg.cbSize          = sizeof(filedlg);
               filedlg.fl              = FDS_CENTER | FDS_OPEN_DIALOG;
               filedlg.pszTitle        = "BUTTONS - Find Program";

               strcpy(filedlg.szFullFile, EXT_EXECUTABLE);

               if (WinFileDlg(HWND_DESKTOP, hwnd, &filedlg) && filedlg.lReturn == DID_OK)
                {
                 strcpy(szBuffer, filedlg.szFullFile);
                 WinSetDlgItemText (hwnd, IDE_EXECUTABLE, szBuffer);
                 UpdateIcon(hwnd, ID_MAINWND, szBuffer);
                }

               return FALSE;

          case DID_DEFAULT:
               ChangeIcon(hwnd, ID_MAINWND, pszAppName[sCounter], NULLHANDLE);    
               return FALSE;

          case DID_ICON:
               memset(&filedlg, 0, sizeof(filedlg));
               filedlg.cbSize          = sizeof(filedlg);
               filedlg.fl              = FDS_CENTER | FDS_OPEN_DIALOG;
               filedlg.pszTitle        = "BUTTONS - Add Icon File to Executable";

               strcpy(filedlg.szFullFile, EXT_ICON);

               if (WinFileDlg(HWND_DESKTOP, hwnd, &filedlg) && filedlg.lReturn == DID_OK)
                {
                 strcpy(szBuffer, filedlg.szFullFile);
                 ChangeIcon(hwnd, ID_MAINWND, pszAppName[sCounter], szBuffer);
                }
               return FALSE;

          case DID_OK: // The Apply Button
               // Retrieve the program name from the entryfield
               WinQueryWindowText(WinWindowFromID(hwnd, IDE_EXECUTABLE), CCHMAXPATH, szBuffer);
               if (pszAppName[sCounter] != NULL) 
                {
                 free( pszAppName[sCounter]);
                } // endif 

               pszAppName[sCounter] = malloc(strlen(szBuffer) + 1);
               strcpy(pszAppName[sCounter], szBuffer);

               // Retrieve the startup parameters
               WinQueryWindowText(WinWindowFromID(hwnd, IDE_PARAMETERS), CCHMAXPATH, szBuffer);
               if (pszParams[sCounter] != NULL) 
                {
                 free( pszParams[sCounter]);
                } // endif 

               pszParams[sCounter] = malloc(strlen(szBuffer) + 1);
               strcpy(pszParams[sCounter], szBuffer);

               // Change the icon in the dialog
               UpdateIcon(hwnd, ID_MAINWND, pszAppName[sCounter]);

               // Now change main program icon
               UpdateBtnIcon(hwndClient, BID_PROGRAM1 + (sCounter-4), pszAppName[sCounter]);

               ulPgmType[sCounter] = BtnIdxToType[SHORT1FROMMR(WinSendDlgItemMsg(hwnd,
                                     IDR_DEFAULT, BM_QUERYCHECKINDEX, NULL, NULL)) ];

               return FALSE;

          case DID_HELP:
               DisplayMessages(INFO_NOHELP, NULLHANDLE, MSG_INFO);
               return FALSE;

          case DID_CANCEL:
               WinDismissDlg(hwnd, TRUE);
               break;

          case DID_SAVE:
               // Save the current program list to the BUTTONS profile
               DosOpen (BUTTONSPRO,        // Profile Name
                        &hfFile,           // File Handle
                        &ulAction,         // Action returned 
                        0L,                // Initial FileSize
                        FILE_ARCHIVED,     // File Attributes
                        OPEN_FLAGS,        // Open Flags
                        OPEN_MODE,         // Open Mode
                        0L);               // Extended Attribute Info
                                        

               DosWrite(hfFile, TEXT_HEADER, strlen(TEXT_HEADER), &ulWritten);
               DosWrite (hfFile, "\r\n", 2, &ulWritten);

               DosWrite(hfFile, SIGNATURE_NAME, strlen(SIGNATURE_NAME), &ulWritten); 
               DosWrite (hfFile, "\r\n", 2, &ulWritten);

               for (usCounter = BID_PROGRAM1; usCounter < BID_END; usCounter++)
               {
                  if(pszAppName[usCounter] != NULL) 
                   {
                     DosWrite (hfFile, pszAppName[usCounter], strlen(pszAppName[usCounter]), &ulWritten); 

                     switch (ulPgmType[usCounter]) 
                      {
                       case PROG_PM:
                            DosWrite(hfFile, " /p", 3, &ulWritten);
                            break;
                         
                       case PROG_FULLSCREEN:
                            DosWrite(hfFile, " /F", 3, &ulWritten);
                            break;
                      
                       case PROG_WINDOWABLEVIO:
                            DosWrite(hfFile, " /f", 3, &ulWritten);
                            break;
                         
                       case PROG_VDM:
                            DosWrite(hfFile, " /D", 3, &ulWritten);
                            break;
                         
                       case PROG_WINDOWEDVDM:
                            DosWrite(hfFile, " /d", 3, &ulWritten);
                            break;
                     
                       case PROG_31_ENH:
                            DosWrite(hfFile, " /W", 3, &ulWritten);
                            break;
                    
                       case PROG_31_ENHSEAMLESSCOMMON:
                            DosWrite(hfFile, " /w", 3, &ulWritten);
                            break;
                      } /* endswitch */

                     if(pszParams[usCounter] != NULL) 
                      {
                        DosWrite (hfFile, " /%", 3, &ulWritten);
                        DosWrite (hfFile, pszParams[usCounter], strlen(pszParams[usCounter]), &ulWritten); 
                      } /* endif */
                   } /* endif */
                  DosWrite (hfFile, "\r\n", 2, &ulWritten);
               }

               DosWrite (hfFile, "*", 1, &ulWritten);
               DosClose(hfFile);
               return FALSE;

          // Trap all other WM_COMMAND messages, so our dialog does not 
          // get inadvertantly dismissed by a value falling through to 
          // the default window procedure.
          default:
           return FALSE;

         }
        break ;
  }
 return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
}


// *******************************************************************************
// FUNCTION:     PopulateListBox
//
// FUNCTION USE: Used to populate the listboxes
//
// DESCRIPTION:  This function demonstrates the use of the new LM_INSERTMULTITEMS 
//               listbox message.  This message is used to insert an array of 
//               pointers to NULL terminated strings into the listbox.  This 
//               function is called to display the contents of the program list 
//               within a listbox.  
//
// PARAMETERS:   HWND     Handle to the listbox 
//
// RETURNS:      TRUE     success                                                   
//               FALSE    error
//
// HISTORY:
//
// *******************************************************************************
BOOL PopulateListBox(HWND hwndListBox)
{
 LBOXINFO lboxinfo;
 LONG     lCount;

 // Populate LBOXINFO structure
 lboxinfo.lItemIndex  = LIT_NONE;       // Don't sort
 lboxinfo.ulItemCount = BID_END;        // Number of items in array

 // Insert the program list array starting with pszAppName[4].  We do 
 // this because LM_INSERTMULTITEMS is kinda kludgy.  If a NULL string 
 // exists no items will be inserted, and our program list starts
 // at pszAppName[4], since it is the first program button
 lCount = (LONG) WinSendMsg(hwndListBox, LM_INSERTMULTITEMS, &lboxinfo, &pszAppName[4]);

 // According to the documentation for the message                                
 // The LM_INSERTMULTITEMS message returns the number of items that               
 // have been inserted into the listbox.  If this number does not                 
 // equal the ulItemCount element of the LBOXINFO structure then                  
 // something went wrong, so return an error back to the caller of the            
 // function, but wait just a minute Batman!!!                                                                     
 // ----------------------------------------------------------------------------                                                                                 
 // The docs are not really perfect with regard to this message                   
 // since lCount won't equal BID_END because we are only inserting starting       
 // with the fourth element of the array, so we need to subtract 4 from           
 // BID_END when we make a comparison.  Also, if we are only inserting            
 // ----------------------------------------------------------------------------
 // if (lCount != BID_END - 4)     
 //
 // But, this is not perfect either because, we are only inserting a 
 // portion of the program list, lCount will not equal the total count                                          
 // of items in the ulItemCount element of LBOXINFO.  
 // ----------------------------------------------------------------------------

 // So we only check to see if we didn't insert anything
 // then we assume we have an error 
 if (lCount == 0) 
  {
   return FALSE;
  } 

 else
  {
   // If no error occurred, then everything is kosher, so select the     
   // first item in the listbox and return TRUE
   WinSendMsg(hwndListBox, LM_SELECTITEM, MPFROMSHORT(FALSE), MPFROMSHORT(TRUE));
   return TRUE;                                
  }
}


// *******************************************************************************
// FUNCTION:     ReadExecutablesFromProfile
//
// FUNCTION USE: Read the executable path and filename from profile
//
// DESCRIPTION:  This function will parse the profile configuration file 
//               BUTTONS.PRO looking for a valid executable path and filename,
//               the program type(if there is one, otherwise the default program
//               type is used to start the session), and any command line 
//               parameters that the user may choose to pass along to the 
//               application.  All this information is stored in the global 
//               arrays.
//
// PARAMETERS:   PUCHAR     The BUTTONS profile(BUTTONS.PRO) to be parsed.
//
// RETURNS:      VOID
//
// HISTORY:
//
// *******************************************************************************
VOID EXPENTRY ReadExecutablesFromProfile(PUCHAR szExecutable)
{
   FILE     *instream;
   char     szBuffer[CCHMAXPATH * 2 + 10];
   char     *pszBufPtr;
   char     *pszToken;
   int      nExec = 4;
   USHORT   cbBuffer;
   BOOL     bDone;
   BOOL     bFirst = TRUE;


   // First thing is to get the file open 
   do {
       if (!(instream = fopen(szExecutable, "r"))) 
        {
         DosBeep(100, 100);
         DisplayMessages(ERROR_OPENING_PROFILE, NULLHANDLE, MSG_EXCLAMATION);
         break;
        } // endif 

       // scan till we hit the end of the file
       while (nExec < MAX_EXEC && !feof(instream)) 
        {
         // get a line in from the file
         if (fgets(szBuffer, sizeof(szBuffer), instream)) 
          {
           // ignore comment lines
           if (szBuffer[0] == ';') 
            {
             continue;
            } // endif 
       
           // kill leading spaces
           pszBufPtr = szBuffer;
           while (*pszBufPtr == ' ') pszBufPtr++;
       
           // remove CR or LF at end
           cbBuffer = strlen( pszBufPtr );
       
           while (pszBufPtr[cbBuffer-1] == '\n' || pszBufPtr[cbBuffer-1] == '\r') 
            {
             pszBufPtr[--cbBuffer] = '\0';
            } // endwhile 
       
           // skip blank lines
           if (cbBuffer == 0) 
            {
             continue;
            } // endif 
       
           // a single standalone '*' indicates end of file
           if (cbBuffer == 1 && *pszBufPtr == '*') 
            {
             break;
            } 
       
           // we now consider everything up to the first space as a file name.
           pszToken = strchr( pszBufPtr, ' ' );
           if (pszToken) 
            {
             *(pszToken++) = '\0';
            } /* endif */
       
           // allocate space for the file name and copy to the global buffer
           cbBuffer = strlen(pszBufPtr);
           pszAppName[nExec] = malloc(cbBuffer+1);
       
           if (pszAppName[nExec] == NULL) 
            {
             DosBeep(100, 100);
             DisplayMessages(NULLHANDLE, "Error Allocating Memory", MSG_ERROR ); 
             break;
            } // endif 
       
           // first executable found should match our signature name
           if (bFirst) 
            {
             if (strcmp(pszBufPtr, SIGNATURE_NAME)) 
              {
               DosBeep(100, 100);
               DisplayMessages(ERROR_INVALID_PROFILE, NULLHANDLE, MSG_ERROR ); 
               break;
              } 
       
             else
              {
               bFirst = FALSE;
               continue;
              } /* endif */
            } /* endif */
       
           // now we're ready to copy the file name to one of our button slots
           strcpy(pszAppName[nExec], pszBufPtr);
       
       
           // first find the end of the token - note: we use this method instead
           // of strtok since the final parameters token may have delimiters embedded. 
           bDone = FALSE;
           while (pszToken && !bDone) 
            {
             // is this a proper token - must start with a '/'
             if (*(pszToken++) == '/') 
              {
               switch (*(pszToken++)) 
                {
                 case 'p':
                      ulPgmType[nExec] = PROG_PM;
                      break;
       
                 case 'D':
                      ulPgmType[nExec] = PROG_VDM;
                      break;
       
                 case 'd':
                      ulPgmType[nExec] = PROG_WINDOWEDVDM;
                      break;
       
                 case 'F':
                      ulPgmType[nExec] = PROG_FULLSCREEN;
                      break;
       
                 case 'f':
                      ulPgmType[nExec] = PROG_WINDOWABLEVIO;
                      break;
       
                 case 'W':
                      ulPgmType[nExec] = PROG_31_ENH;
                      break;
       
                 case 'w':
                      ulPgmType[nExec] = PROG_31_ENHSEAMLESSCOMMON;
                      break;
       
                 case '%':
                      cbBuffer = strlen( pszToken );
                      pszParams[nExec] = malloc( cbBuffer + 1 );
       
                      if (pszParams[nExec] == NULL) 
                       {
                        DosBeep( 100, 100 );
                        DisplayMessages( NULLHANDLE, "Error Allocating Memory", MSG_ERROR ); 
                       } 
       
                      else
                       {
                        strcpy( pszParams[nExec], pszToken );
                        pszToken += cbBuffer;
                       } 
       
                      bDone = TRUE;     // no more processing for this line
                      break;
                } 
              } // end if
       
             // find the start of the next token
             pszToken = strchr( pszToken, '/' );
            } // end while 
       
           // if we got here we added an executable - bump the count
           nExec++;
          } // endif 
        } // end while 

        fclose(instream);
      } while (false); /* enddo */
}


// *******************************************************************************
// FUNCTION:     CommandButtonDlgProc
//
// FUNCTION USE: Dialog procedure used to change the command interpreter button
//
// DESCRIPTION:  This dialog procedure is used to allow the user to re-configure
//               the command interpreter function button.
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
MRESULT EXPENTRY CommandButtonDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  HWND    hwndCombo;
  CHAR    szBuffer[CCHMAXPATH];
  CHAR    szParameters[CCHMAXPATH];
  APIRET  rc;
  LONG    lReturn;

  switch (msg)
   {
    case WM_INITDLG:
         // Remove the system menuitems that are not needed by this dialog
         SetTheSysMenu(hwnd, NULL);

         // Obtain the window handle of the listbox window
         hwndCombo  = WinWindowFromID(hwnd, IDL_COMLIST); 

         // Populate the combo-box window
         FillComboBoxes(hwndCombo);

         lReturn = PrfQueryProfileString(HINI_USER, APPNAME, KEYCOMMAND, NULL, (PVOID)szBuffer, (LONG)sizeof(szBuffer));                                                                                             
         if (lReturn != 0)                              
          {
           WinSetDlgItemText(hwnd, IDL_COMLIST, szBuffer);   
          } 

         lReturn = PrfQueryProfileString(HINI_USER, APPNAME, KEYPARAMS, NULL, (PVOID)szParameters, (LONG)sizeof(szParameters)); 
         if (lReturn != 0)                                                                                         
          {                                                                                                    
           WinSetDlgItemText(hwnd, IDE_PARAMETERS, szParameters);                                                     
          }                                                                                                    
         break;

    case WM_COMMAND:
         switch (COMMANDMSG(&msg)->cmd)
          {
           case DID_RUN:
                WinQueryDlgItemText(hwnd, IDL_COMLIST,    sizeof(szBuffer), (PSZ)szBuffer);   
                WinQueryDlgItemText(hwnd, IDE_PARAMETERS, sizeof(szParameters), (PSZ)szParameters);    

                StartApplication(szBuffer, szParameters, 0, NULLHANDLE);  
                WinDismissDlg(hwnd, TRUE);
                break;
             
           case DID_CANCEL:
                WinDismissDlg(hwnd, TRUE); 
                return FALSE;                     

           case DID_SAVE:
                WinQueryDlgItemText(hwnd, IDL_COMLIST, sizeof(szBuffer), (PSZ)szBuffer);                                                        
                PrfWriteProfileString(HINI_USER, APPNAME, KEYCOMMAND, (PSZ)szBuffer);                      

                WinQueryDlgItemText(hwnd, IDE_PARAMETERS, sizeof(szParameters), (PSZ)szParameters);                                                         
                PrfWriteProfileString(HINI_USER, APPNAME, KEYPARAMS, (PSZ)szParameters);                                                            
                return FALSE;
          }
         break ;
   }
 return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
}


// *******************************************************************************
// FUNCTION:     FillComboBoxes      
//
// FUNCTION USE: Routine used to update insert items into a combobox control    
//
// DESCRIPTION:  This routine is used by the Run Utility Programs dialog box.  It
//               is used to display a list of utility programs, for example chkdsk
//               that may need to be run, but not frequently enough to be added 
//               to the program list.
//
// PARAMETERS:   HWND     Window handle of the combobox.
//
// RETURNS:      BOOL    
//
// HISTORY:      mm-dd-yy
//
// *******************************************************************************
BOOL EXPENTRY FillComboBoxes(HWND hwndCombo)                                                          
{                                                                                                               
 USHORT usCounter;                                                                                              
 APIRET rc;                                                                                                     
 PSZ    pszTemp;

 // This is our utility program list.  
 // The SubstituteBootDrive routine is designed to return a NULL
 // PSZ if the boot drive does not need to be added to the path 
 // and filename, then we do not need to loop through and call
 // SubstituteBootDrive recursively.  Instead, we assume that we
 // are still pointing to the valid program utilities, and merely update 
 // the contents of the combo-box with the array.

 CHAR   *szUtilities[] = {"?:\\OS2\\CHKDSK.COM",   
                          "?:\\OS2\\FORMAT.COM",
                          "?:\\OS2\\DISKCOPY.COM", 
                          "?:\\OS2\\LABEL.COM", 
                          "?:\\OS2\\E.EXE", 
                          "?:\\OS2\\SYSLEVEL.EXE",  
                          "?:\\OS2\\XCOPY.EXE",   
                          "?:\\OS2\\PSTAT.EXE",   
                          "?:\\OS2\\PMCHKDSK.EXE",   
                          "?:\\OS2\\ICONEDIT.EXE",      
                         };


 for(usCounter = 0; usCounter < 10; usCounter ++)                                         
  {                                                                                                             
   // Loop through and see check to see if SubstituteBootDrive 
   // returns a valid PSZ.  If pszTemp is NULL then let's enter
   // another loop just to update the combo-box.
   pszTemp = SubstituteBootDrive(szUtilities[usCounter]); 
   if (pszTemp == NULL) 
    {
     WinEnableWindowUpdate(hwndCombo, FALSE);                                                               
     for(usCounter = 0; usCounter < 10; usCounter ++)                                         
      {
       WinSendMsg(hwndCombo, LM_INSERTITEM, MPFROMSHORT(LIT_SORTASCENDING), MPFROMP(szUtilities[usCounter])); 
      }
     break;
    } 

   // Otherwise, SubstituteBootDrive returned a valid PSZ containing
   // the complete path and filename of the utility programs.  Copy the 
   // contents of the string back into our szUtilities array and then 
   // update the combo-box.
   else
    {
     strcpy(szUtilities[usCounter], pszTemp);
     WinEnableWindowUpdate(hwndCombo, FALSE);                                                                 
     WinSendMsg(hwndCombo, LM_INSERTITEM, MPFROMSHORT(LIT_SORTASCENDING), MPFROMP(szUtilities[usCounter]));    
    }
  }

 // By the time we get to here, one way or the other, we should have a valid 
 // array containing the utility programs.  So, we need to show the combo-box
 // once the update is complete and hilite the first item.
 WinShowWindow(hwndCombo, TRUE);                                                                          
 WinSendMsg(hwndCombo, CBM_HILITE, MPFROMSHORT(TRUE), NULL);                                                                                                           
 return;                                                                                                   
}                                                                                                               

// *******************************************************************************         
// FUNCTION:     SubstituteBootDrive                                                       
//                                                                                         
// FUNCTION USE: Routine used to query the boot drive and insert it into a string.         
//                                                                                         
// DESCRIPTION:  This routine takes the path and filename of a program that is            
//               known to be in the \\OS2 directory and locates the correct boot           
//               drive to resolve the complete path.  The complete path and               
//               filename is returned.                                                     
//                                                                                         
// PARAMETERS:   PSZ     The path and filename containing the string to             
//                       a utility program with a question mark representing 
//                       the boot drive.  For example ?:\\OS2\\ICONEDIT.EXE.
//                                                                                         
// RETURNS:      PSZ     The complete path and filename including the boot drive.      
//                                                                                         
// HISTORY:                                                                
//                                                                                         
// *******************************************************************************         
PSZ SubstituteBootDrive(PSZ pszFilename)                                                
{                                                                        
   static CHAR szDriveLetter = '\0';                                       
   ULONG       ulDrive;                                                  
   PSZ         pszCompletePath;                                                   
   APIRET      rc;
                                                                         
   // Extract the boot drive letter                                      
   if (!szDriveLetter)                                                     
   {                                                                     
      rc = DosQuerySysInfo(QSV_BOOT_DRIVE,                              
                           QSV_BOOT_DRIVE,                              
                           &ulDrive,                                    
                           sizeof(ulDrive));                           

      if (rc != NULLHANDLE) 
       {
        DisplayMessages(NULLHANDLE, "Error obtaining Boot Drive", MSG_ERROR);
       } 

      else
       {
        szDriveLetter = (CHAR)('A' + ulDrive - 1);                        
       }
   }                                                                     
                                                                         
   // If the string starts with "?:", substitute the boot drive letter!
   pszCompletePath = strstr(pszFilename,"?:");                                    
   if (pszCompletePath)                                                           
    {
      *pszCompletePath = szDriveLetter;                                             
      return pszCompletePath;
    }

   else 
    {
     return NULL;
    }
}                                                                        


// *******************************************************************************
// FUNCTION:     ConfigureComSpecDlgProc
//
// FUNCTION USE: Dialog procedure used to configure the command interpreter 
//
// DESCRIPTION:  This dialog procedure is used to display the current command 
//               interpreter and allow the user to pass a parameter everytime
//               the command interpreter is invoked.
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
MRESULT EXPENTRY ConfigureComSpecDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 APIRET  rc;
 HWND    hwndStatic;
 PSZ     pszComspec;
 CHAR    szBuffer[CCHMAXPATH];
 CHAR    szParameters[CCHMAXPATH];

 switch (msg)
  {
   case WM_INITDLG:
        CenterDialog(hwnd);
        SetTheSysMenu(hwnd, NULL);
        hwndStatic = WinWindowFromID(hwnd, ID_STATIC);
        DosScanEnv("COMSPEC", &pszComspec);                                           
        sprintf(szBuffer, "The current command interpreter is %s", pszComspec);       
        WinSetWindowText(hwndStatic, szBuffer);                                       

        rc = PrfQueryProfileString(HINI_USER, APPNAME, KEYCOMSPEC, NULL, (PVOID)szParameters, (LONG)sizeof(szParameters)); 
        if (rc != 0)                                                                                                      
         {                                                                                                                     
          WinSetDlgItemText(hwnd, IDE_PARAMETERS, szParameters);                                                               
         }                                                                                                                     
        break;

   case WM_COMMAND:
        switch (COMMANDMSG(&msg)->cmd)
         {
          case DID_OK:
               WinQueryDlgItemText(hwnd, IDE_PARAMETERS, sizeof(szParameters), (PSZ)szParameters);                   
               PrfWriteProfileString(HINI_USER, APPNAME, KEYCOMSPEC, (PSZ)szParameters);                              

               WinDismissDlg(hwnd, TRUE);
               break;
         }
        break;
  }
 return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
}
