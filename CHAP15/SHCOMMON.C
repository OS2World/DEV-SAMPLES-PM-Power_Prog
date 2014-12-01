// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      SHCOMMON.C                                       mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Common Functions used by SHOWOFF                              บ
// บ                                                                               บ
// บ  NOTES:         This file contains various nifty routines that are used       บ
// บ                 throughout the application.                                   บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
 
#define INCL_WINWINDOWMGR
#define INCL_WINMESSAGEMGR
#define INCL_WINDIALOGS
#define INCL_WINBUTTONS
#define INCL_WINMENUS
#define INCL_WINFRAMEMGR
#define INCL_WINSHELLDATA
#define INCL_WINHELP
#define INCL_WINSYS
#define INCL_WINSTATICS
#define INCL_WINWORKPLACE
#define INCL_WINPOINTERS

#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "shcommon.h"
#include "showoff.h"

// *******************************************************************************
// FUNCTION:     HideControls
//
// FUNCTION USE: Toggles the visibility of the frame controls
//
// DESCRIPTION:  This routine is used to remove the frame controls, including
//               the system menu, titlebar, minmax buttons and action bar menu.
//
// PARAMETERS:   HWND     window handle of the frame window
//
// RETURNS:      FALSE Success
//               TRUE  Failure
//
// HISTORY:      
//
// *******************************************************************************
VOID HideControls(HWND hwndFrame)
{
 static HWND hwndTitleBar;     // Must be static for initial invocation
 static HWND hwndSysMenu;      // Must be static for initial invocation
 static HWND hwndAppMenu;      // Must be static for initial invocation
 static HWND hwndMinMax;       // Must be static for initial invocation
 static BOOL bHidden = FALSE;  // Must be static for initial invocation


 // --------------------------------------------------------------------------
 // If bHidden is not TRUE, this means that we want to hide the controls.  
 // So we will obtain handles to all of the control windows by querying the 
 // frame window identifiers.  Then we will remove the controls by setting 
 // control's parent window to the constant HWND_OBJECT rather than the frame.  
 // The frame controls are restored by resetting the parent windows back to 
 // the frame window.  This allows us to temporarily remove the frame control 
 // windows without destroying them explicitly.
 // --------------------------------------------------------------------------
 if (!bHidden) // Hide Frame Control Windows
  {
   hwndTitleBar = WinWindowFromID(hwndFrame,  FID_TITLEBAR);
   hwndSysMenu  = WinWindowFromID(hwndFrame,  FID_SYSMENU);
   hwndAppMenu  = WinWindowFromID(hwndFrame,  FID_MENU);
   hwndMinMax   = WinWindowFromID(hwndFrame,  FID_MINMAX);

   WinSetParent(hwndTitleBar,  HWND_OBJECT, FALSE);
   WinSetParent(hwndSysMenu,   HWND_OBJECT, FALSE);
   WinSetParent(hwndAppMenu,   HWND_OBJECT, FALSE);
   WinSetParent(hwndMinMax,    HWND_OBJECT, FALSE);

   WinSendMsg(hwndFrame, WM_UPDATEFRAME, (MPARAM)(FCF_TITLEBAR | FCF_MENU | FCF_SYSMENU | FCF_MINBUTTON), NULL);
   bHidden = TRUE;
  }

 // --------------------------------------------------------------------------           
 // Show the Frame Windows
 // --------------------------------------------------------------------------           
 else  
  {
   WinSetParent(hwndTitleBar,  hwndFrame, FALSE);
   WinSetParent(hwndSysMenu,   hwndFrame, FALSE);
   WinSetParent(hwndAppMenu,   hwndFrame, FALSE);
   WinSetParent(hwndMinMax,    hwndFrame, FALSE);

   WinSendMsg(hwndFrame, WM_UPDATEFRAME, (MPARAM)(FCF_TITLEBAR | FCF_MENU | FCF_SYSMENU | FCF_MINBUTTON), NULL );
   bHidden = FALSE;
  }
 return;
}


// *******************************************************************************      
// FUNCTION:     DisplayMessages                                                        
//                                                                                      
// FUNCTION USE: Simple wrapper routine that displays a message          
//                                                                                      
// DESCRIPTION:  This routine is used to display a message to the user    
//               via a message box.  This function simplifies the process 
//               of creating the message box by shielding the caller from 
//               WinMessageBox.  For most message box purposes, this 
//               function will suffice for WinMessageBox.  This function
//               takes three parameters; the message identifier which can
//               be loaded via a STRINGTABLE resource, the message text
//               that is to be displayed to the user, and the type of 
//               message box to be displayed.  
//
// PARAMETERS:   HWND   hwndOwner                                    
//                      The owner window handle.
//
//               ULONG  ulMessageID                                  
//                      The corresponding numeric identifier of     
//                      the resource from the messagetable.         
//                      If this value is a NULLHANDLE, then we need
//                      to evaluate PCH TextString for the text    
//                      string to be displayed from the stringtable. 
//                                                                   
//               PCH    TextString                                   
//                      A simple text string that can be passed     
//                      from the caller, this parameter is ignored  
//                      if ulMessageID contains a valid ID number  
//                      indcating that the message text is to be 
//                      loaded from the STRINGTABLE resource.
//                                                                   
//               USHORT msgFlag                                      
//                      The type of message box.  Valid Types are:  
//                                                                   
//               MSG_ERROR   - System modal box for really errors       
//               MSG_WARNING - Question message box                                                                                         
//               MSG_INFO    - Used to display simple info 
//
// RETURNS:      USHORT MBID                                                                           
//                                                                                      
// HISTORY:                                                                 
//                                                                                      
// *******************************************************************************      
USHORT DisplayMessages(ULONG ulMessageID, PCH pchText, USHORT usMsgType)
{
   CHAR   szTempString[CCHMAXPATH];
   PSZ    pszMessageString;
   APIRET rc;
   HAB    hab;


   // If a valid ulMessageID (a non-zero value) was passed, then we need to 
   // load the appropriate message from the message/string table.  Otherwise
   // if ulMessageID is NULLHANDLE, then pchText contains the text to be displayed.   
   hab = WinQueryAnchorBlock(HWND_DESKTOP);

   if (ulMessageID)
    {
      rc = WinLoadString(hab, (HMODULE)0, ulMessageID, sizeof(szTempString), szTempString);
      if (rc == FALSE)
       {
        DosBeep(1000, 1000);
       }

      else
       {
        pszMessageString = szTempString;
       }
    }

   else
    {
     pszMessageString = pchText;
    }


   switch(usMsgType)
   {
    case MSG_ERROR:
         rc = WinMessageBox(HWND_DESKTOP,
                            HWND_DESKTOP,
                            pszMessageString,
                            TITLEBAR,
                            ID_MESSAGEBOX,
                            MB_OK | MB_SYSTEMMODAL | MB_MOVEABLE | MB_ERROR);
         break;

    case MSG_WARNING:
         rc =  WinMessageBox(HWND_DESKTOP,
                             HWND_DESKTOP,
                             pszMessageString,
                             TITLEBAR,
                             ID_MESSAGEBOX,
                             MB_MOVEABLE | MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON1);
         break;

    case MSG_EXCLAMATION:                                                                   
         rc =  WinMessageBox(HWND_DESKTOP,                                           
                             HWND_DESKTOP,
                             pszMessageString,                                       
                             TITLEBAR,                                               
                             ID_MESSAGEBOX,                                          
                             MB_MOVEABLE | MB_ICONEXCLAMATION | MB_OK | MB_DEFBUTTON1); 
         break;                                                                      

    case MSG_INFO:
         rc =  WinMessageBox(HWND_DESKTOP,
                             HWND_DESKTOP,
                             pszMessageString,
                             TITLEBAR,
                             ID_MESSAGEBOX,
                             MB_MOVEABLE | MB_ICONASTERISK | MB_OK | MB_DEFBUTTON1);
         break;
   }
   return rc;
}


// *******************************************************************************
// FUNCTION:     CenterDialog
//
// FUNCTION USE: Centers a Dialog Window
//
// DESCRIPTION:  There are some instances when a user will position a window 
//               somewhere nearly off the screen.  The result is that the dialog
//               window which is positioned based on the moved window may not be 
//               positioned ideally.  This routine forces the window to be 
//               centered perfectly each time.  
//
// PARAMETERS:   hwnd  The window handle of the window to be centered.
//
// RETURNS:      VOID
//
// HISTORY:                                                                
//
// *******************************************************************************
VOID CenterDialog(HWND hwnd)
{
 LONG  lcx, lcy;
 LONG  lcxScreen;
 LONG  lcyScreen;
 SWP   swp;

 // --------------------------------------------------------------------------
 // Query width and length of the screen device
 // --------------------------------------------------------------------------
 lcxScreen = WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
 lcyScreen = WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);

 // --------------------------------------------------------------------------
 // Query width and length of the dialog box window to be centered
 // --------------------------------------------------------------------------
 WinQueryWindowPos(hwnd, &swp);

 // --------------------------------------------------------------------------
 // Center the window within the screen
 // --------------------------------------------------------------------------
 lcx = (lcxScreen - swp.cx) / 2;
 lcy = (lcyScreen - swp.cy) / 2;

 WinSetWindowPos(hwnd,
                 HWND_TOP,
                 lcx,
                 lcy,
                 NULLHANDLE,
                 NULLHANDLE,
                 SWP_MOVE);
 return; 
}


// *******************************************************************************
// FUNCTION:     SetTheSysMenu
//
// FUNCTION USE: Used to remove unnecessary system menu menuitems
//
// DESCRIPTION:  This routine removes all items from the system menu with the
//               exception of MOVE and CLOSE.  It should only be used for dialog
//               windows that have no need for sizing.  It will also allow for
//               the changing of the CLOSE item text if a valid string is passed
//               in szCloseItem.
//
// PARAMETERS:   HWND     window handle of the window that has a system menu
//               PCH      string containing the new CLOSE menuitem text
//
// RETURNS:      VOID
//
// HISTORY:                                                                 
//
// *******************************************************************************
VOID SetTheSysMenu(HWND hwnd, PCH szCloseItem)
{
 HWND       hwndSysMenu;
 MENUITEM   menuitem;
 ULONG      ulMenuID;
 SHORT      sNumItems;
 SHORT      sIndex = 0;
 BOOL       bDontChangeClose = FALSE;

 // --------------------------------------------------------------------------
 // Check to see if the user passed us a string to replace Close.
 // --------------------------------------------------------------------------
 if (!szCloseItem)
  {
   bDontChangeClose = TRUE;
  }

 // --------------------------------------------------------------------------
 // Obtain the system menu window handle from the identifier
 // --------------------------------------------------------------------------
 hwndSysMenu = WinWindowFromID(hwnd, FID_SYSMENU);

 // --------------------------------------------------------------------------
 // Get the MENUITEM structure for the system menu
 // --------------------------------------------------------------------------
 WinSendMsg(hwndSysMenu,                                      // Window Handle
            MM_QUERYITEM,                                     // Message
            MPFROM2SHORT(SC_SYSMENU, FALSE),                  // Message Parameter 1
            MPFROMP(&menuitem));                              // Message Parameter 2

 // --------------------------------------------------------------------------
 // Determine how many items are in the system menu
 // --------------------------------------------------------------------------
 sNumItems = SHORT1FROMMR(WinSendMsg(menuitem.hwndSubMenu,    // Window Handle
                                     MM_QUERYITEMCOUNT,       // Message
                                     (MPARAM)NULL,            // Message Parameter 1
                                     (MPARAM)NULL));          // Message Parameter 2

 while (sNumItems--)
 {
  ulMenuID = (ULONG) WinSendMsg(menuitem.hwndSubMenu,         // Window Handle
                                MM_ITEMIDFROMPOSITION,        // Message
                                MPFROM2SHORT(sIndex, TRUE),   // Message Parameter 1
                                (MPARAM)NULL);                // Message Parameter 2

  switch (SHORT1FROMMP(ulMenuID))
   {
    case SC_MOVE:       // DO NOT REMOVE THE MOVE  MENUITEM
    case SC_CLOSE:      // DO NOT REMOVE THE CLOSE MENUITEM
         sIndex++;
         break;

    // --------------------------------------------------------------------------
    // For all other menuitems except Move and Close bang em outta here...
    // --------------------------------------------------------------------------
    default:            
         WinSendMsg(menuitem.hwndSubMenu,
                    MM_DELETEITEM,
                    MPFROM2SHORT(ulMenuID, TRUE),
                   (MPARAM)NULL);
   }
 }

 // --------------------------------------------------------------------------
 // If the user passed us a text string to replace the Close menuitem, 
 // then send the system menu a MM_SETITEMTEXT message with the new text.
 // --------------------------------------------------------------------------
 if (bDontChangeClose != TRUE)
  {
   WinSendMsg(hwndSysMenu,
              MM_SETITEMTEXT,
              (MPARAM)SC_CLOSE,
              MPFROMP(szCloseItem));
  }
 return;
}


// *******************************************************************************
// FUNCTION:     GetDefaultDirectory
//
// FUNCTION USE: Obtains the current drive and directory.
//
// DESCRIPTION:  This function gets the current drive and directory from where
//               the program was started.  We need this routine so that we can
//               determine where we need to grab the icons that will update
//               the printer object.
//
// PARAMETERS:   PSZ  character string, that on return will contain the
//                    valid path and filename of the current directory and drive.
//
// RETURNS:      VOID
//
// HISTORY:                                               
//
// *******************************************************************************
VOID GetDefaultDirectory (CHAR *szDefault)
 {
  ULONG  ulDriveNum;
  ULONG  ulDriveMap;
  ULONG  ulCurPathLen;

  // --------------------------------------------------------------------------
  // Get the current fixed disk number, and convert it to an alpha-numeric.
  // --------------------------------------------------------------------------
  DosQueryCurrentDisk (&ulDriveNum, &ulDriveMap);
  szDefault [0] = (CHAR) ulDriveNum + '@' ;
  szDefault [1] = ':' ;
  szDefault [2] = '\\' ;

  // --------------------------------------------------------------------------
  // Append the current directory
  // --------------------------------------------------------------------------
  ulCurPathLen = CCHMAXPATH;
  DosQueryCurrentDir (0, szDefault + 3, &ulCurPathLen);
  return;
}


// *******************************************************************************
// FUNCTION:     UpdateTitleBarText
//
// FUNCTION USE: Changes the text for the title bar window
//
// DESCRIPTION:  This routine is used to update the title bar with the bitmap
//               filename and the resolution of the bitmap.  It is passed a
//               complete path and filename, but we strip the drive and directory
//               information to obtain just the filename.
//
// PARAMETERS:   HWND     window handle of the frame window
//               UCHAR    string containing the path and filename of the bitmap
//               ULONG    the number of colors
//
// RETURNS:      FALSE Success
//               TRUE  Failure
//
// HISTORY:
//
// *******************************************************************************
VOID UpdateTitleBarText(HWND hwnd, UCHAR *pszPathAndFilename, USHORT usXRes, USHORT usYRes, ULONG ulNumberOfColors)
{
  CHAR  *pszFilename;
  HWND  hwndTitleBar;
  CHAR  szBuffer[CCHMAXPATH];
  CHAR  szColors[50];

  // --------------------------------------------------------------------------
  // Get the window handle of the titlebar.
  // --------------------------------------------------------------------------
  hwndTitleBar = WinWindowFromID(hwnd, FID_TITLEBAR);

  // --------------------------------------------------------------------------
  // Get the bitmap filename from the path and filename string.
  // --------------------------------------------------------------------------
  pszFilename = (PUCHAR)max(strrchr(pszPathAndFilename, ':'), strrchr(pszPathAndFilename, '\\'));

  if (pszFilename)
   {
    pszFilename++;
   }

  else
   {
    pszFilename = pszPathAndFilename;
   }

  // --------------------------------------------------------------------------
  // Convert the string to UPPERCASE
  // --------------------------------------------------------------------------
  strupr(pszFilename);               

  // --------------------------------------------------------------------------
  // Convert number of bits to number of colors and pop in TitleBar
  // To do this take the number 2 to ulNumberOfColors.
  // --------------------------------------------------------------------------
  switch (ulNumberOfColors)
   {
    case 1:
         strcpy(szColors, "2 Color Bitmap");              //  Cheesy 2 Color Bitmap File
         break;

    case 4:
         strcpy(szColors, "16 Color Bitmap");             //  16 Color Bitmap File
         break;

    case 8:
         strcpy(szColors, "256 Color Bitmap");            //  256 Color Bitmap File
         break;

    case 24:
         strcpy(szColors, "16.7 Million Color Bitmap");   //  Wow 24bit graphics
         break;

    default:
         strcpy(szColors, "??? Unknown Color Value");     //  Unknown number of colors???
         break;

   }

  // --------------------------------------------------------------------------      
  // Format our temp string with the filename and the X/Y Resolution                 
  // --------------------------------------------------------------------------      
  sprintf(szBuffer, "%s - %dx%d [%s]", pszFilename, usXRes, usYRes, szColors);                       

  // --------------------------------------------------------------------------
  // Set the Titlebar Window Text.
  // --------------------------------------------------------------------------
  WinSetWindowText(hwndTitleBar, szBuffer);
  return;
}

// *******************************************************************************
// FUNCTION:     UpdateMenu
//
// FUNCTION USE: Disables/Enables certain menuitems based on operation
//
// DESCRIPTION:  The SHOWOFF program has two modes for displaying bitmaps.  The
//               single image mode allows the user to view one bitmap at a time.
//               The single image mode is the normal bitmap operation mode.
//               The Presentation mode allows the user to display multiple
//               bitmaps in a slide show.  This function will reformat the
//               application menu when a presentation show begins by disabling
//               the single image mode menuitems, File/Open, File/Create etc.
//               The single image mode menuitems will be re-enabled when the
//               presentation has concluded.
//
// PARAMETERS:   HWND    window handle of the frame window.
//               BOOL    bDisplayMode  The mode (single image or presentation).
//
// RETURNS:      HWND    window handle of the application menu.
//
// HISTORY:
//
// *******************************************************************************
HWND UpdateMenu(HWND hwndOwner, BOOL bStartShow)
{
 HWND   hwndMenu;

 // --------------------------------------------------------------------------
 // Get the window handle of the application menu.
 // --------------------------------------------------------------------------
 hwndMenu = WinWindowFromID(hwndOwner, FID_MENU);

 if (bStartShow)
  {
   // --------------------------------------------------------------------------
   // Disable these menuitems during a Presentation
   // --------------------------------------------------------------------------
   WinEnableMenuItem(hwndMenu, IDM_OPENBMP,   FALSE);
   WinEnableMenuItem(hwndMenu, IDM_STARTSHOW, FALSE);
   WinEnableMenuItem(hwndMenu, IDM_EDITCLEAR, FALSE);
   WinEnableMenuItem(hwndMenu, IDM_CREATE,    FALSE);
   WinEnableMenuItem(hwndMenu, IDM_EDITPRES,  FALSE);
   WinEnableMenuItem(hwndMenu, IDM_PREVIEW,  FALSE);

   // --------------------------------------------------------------------------
   // Enable these menuitems during a Presentation
   // --------------------------------------------------------------------------
   WinEnableMenuItem(hwndMenu, IDM_RESTART,  TRUE);
   WinEnableMenuItem(hwndMenu, IDM_PAUSE,    TRUE);
   WinEnableMenuItem(hwndMenu, IDM_STOPSHOW, TRUE);
   WinEnableMenuItem(hwndMenu, IDM_FFORWARD, TRUE);
   WinEnableMenuItem(hwndMenu, IDM_REWIND,   TRUE);
  }

 else
  {
   // --------------------------------------------------------------------------
   // Enable these menuitems for normal bitmap operation
   // --------------------------------------------------------------------------
   WinEnableMenuItem(hwndMenu, IDM_OPENBMP,   TRUE);
   WinEnableMenuItem(hwndMenu, IDM_STARTSHOW, TRUE);
   WinEnableMenuItem(hwndMenu, IDM_EDITCLEAR, TRUE);
   WinEnableMenuItem(hwndMenu, IDM_CREATE,    TRUE);
   WinEnableMenuItem(hwndMenu, IDM_EDITPRES,  TRUE);
   WinEnableMenuItem(hwndMenu, IDM_PREVIEW,   TRUE);

   // --------------------------------------------------------------------------
   // Disable the Presentation menuitems during normal bitmap operation
   // --------------------------------------------------------------------------
   WinEnableMenuItem(hwndMenu, IDM_RESTART,  FALSE);
   WinEnableMenuItem(hwndMenu, IDM_PAUSE,    FALSE);
   WinEnableMenuItem(hwndMenu, IDM_STOPSHOW, FALSE);
   WinEnableMenuItem(hwndMenu, IDM_FFORWARD, FALSE);
   WinEnableMenuItem(hwndMenu, IDM_REWIND,   FALSE);
  }
 return(hwndMenu);
}


// *******************************************************************************
// FUNCTION:     ParsePathAndSetDirectory
//
// FUNCTION USE: Takes a valid path and filename and separates the path
//               string from the filename string.
//
// DESCRIPTION:  This function will take a valid path and filename and
//               return two individual strings; one containing the directory
//               path and the other returning the valid filename.
//
// PARAMETERS:   CHAR *     Complete Path and Filename   X:\DIRECTORY\FILE.TXT
//               CHAR *     on exit contains X:\DIRECTORY
//               CHAR *     on exit contains FILE.TXT
//
// RETURNS:      VOID
//
// HISTORY:
//
// *******************************************************************************
VOID ParsePathAndSetDirectory (CHAR *pszPathAndFile, CHAR *pszPath, CHAR *pszFile)
{
   USHORT  usCounter;
   CHAR    szBuffer[CCHMAXPATH];
   ULONG   ulDriveNum;
   PSZ     pszTemp;


   for (usCounter = strlen(pszPathAndFile) - 1; usCounter > 0; usCounter--)
    {
     if (*(pszPathAndFile + usCounter) == '\\')
      break;
    }

   strcpy (pszFile, pszPathAndFile + usCounter + 1);
   strncpy (pszPath, pszPathAndFile, usCounter);
   pszPath[usCounter] = '\0';

   // --------------------------------------------------------------------------
   // Convert the string to UPPERCASE
   // --------------------------------------------------------------------------
   strupr(pszPath);

   // --------------------------------------------------------------------------
   // Get the Drive Number
   // --------------------------------------------------------------------------
   if (pszPath[1] == ':') 
    {
     ulDriveNum = (ULONG) pszPath[0] - '@';                
     DosSetDefaultDisk(ulDriveNum);
     DosSetCurrentDir(pszPath);
    }
   return;
}
