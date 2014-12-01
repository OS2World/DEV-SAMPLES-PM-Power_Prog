// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      SHCOMMON.C                                       mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Common Functions                                              บ
// บ                                                                               บ
// บ  NOTES:         This file contains various nifty routines that are used       บ
// บ                 throughout the sample programs.                               บ
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
// INTERNALS:    NONE
//
// HISTORY:
//   02-10-95    1000   U. Stern        Created the routine
//
// *******************************************************************************
VOID HideControls(HWND hwndFrame)
{
   static HWND hwndTitleBar;     // Must be static for initial invocation
   static HWND hwndSysMenu;      // Must be static for initial invocation
   static HWND hwndAppMenu;      // Must be static for initial invocation
   static HWND hwndMinMax;       // Must be static for initial invocation
   static BOOL bHidden = FALSE;  // Must be static for initial invocation


   // If bHidden is not TRUE, this means that we want to hide the controls.  So
   // we will obtains handles to all of the control windows by querying the 
   // frame window identifiers.  Then we will remove the controls by setting the
   // parent windows to HWND_OBJECT rather than the frame.  The frame controls
   // are restored by resetting the parent windows back to the frame window.  This
   // allows us to temporily remove the frame control windows without destroying
   // them explicitly.

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

   else  // Show Frame Control Windows
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
// PARAMETERS:   ULONG  ulMessageID                                  
//                     - The corresponding numeric identifier of     
//                       the resource from the messagetable.         
//                       If this value is a NULLHANDLE, then we need
//                       to evaluate PCH TextString for the text    
//                       string to be displayed from the stringtable. 
//                                                                   
//               PCH    TextString                                   
//                     - A simple text string that can be passed     
//                       from the caller, this parameter is ignored  
//                       if ulMessageID contains a valid ID number  
//                       indcating that the message text is to be 
//                       loaded from the STRINGTABLE resource.
//                                                                   
//               USHORT msgFlag                                      
//                     - The type of message box.  Valid Types are:  
//                                                                   
//               MSG_ERROR   - System modal box for really errors       
//               MSG_WARNING - Question message box                                                                                         
//               MSG_INFO    - Used to display simple info 
//
// RETURNS:      USHORT MBID                                                                           
//                                                                                      
// INTERNALS:    NONE                                                                   
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
//               centered perfectly each time.  It is used primarily to allow  
//               our "Configure Buttons" dialog  which is filled with controls
//               to be centered, for the maximum visual effect.
//
// PARAMETERS:   hwnd       The window handle of the window to be centered.
//
// RETURNS:      VOID
//
// INTERNALS:    NONE
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

  // Query width and depth of screen device
  lcxScreen = WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
  lcyScreen = WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);

  // Query width and depth of dialog box
  WinQueryWindowPos(hwnd, &swp );

  // Center dialog box within the screen
  lcx = (lcxScreen - swp.cx) / 2;
  lcy = (lcyScreen - swp.cy) / 2;

  WinSetWindowPos(hwnd,
                  HWND_TOP,
                  lcx,
                  lcy,
                  NULLHANDLE,
                  NULLHANDLE,
                  SWP_MOVE);

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
// INTERNALS:    NONE
//
// HISTORY:
//   02-10-95    1000   U. Stern        Created the routine
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

    if (!szCloseItem)
     {
      bDontChangeClose = TRUE;
     }

    // Obtain the system menu window handle from the identifier
    // and find out how many items exist in the menu.

    hwndSysMenu = WinWindowFromID(hwnd, FID_SYSMENU);

    WinSendMsg(hwndSysMenu,                                      // Window Handle
               MM_QUERYITEM,                                     // Message
               MPFROM2SHORT(SC_SYSMENU, FALSE),                  // Message Parameter 1
               MPFROMP((PCH) &menuitem));                        // Message Parameter 2

    sNumItems = SHORT1FROMMR(WinSendMsg(menuitem.hwndSubMenu,    // Window Handle
                                        MM_QUERYITEMCOUNT,       // Message
                                       (MPARAM)NULL,             // Message Parameter 1
                                       (MPARAM)NULL));           // Message Parameter 2

    while (sNumItems--)
    {
     ulMenuID = (ULONG) WinSendMsg(menuitem.hwndSubMenu,         // Window Handle
                                   MM_ITEMIDFROMPOSITION,        // Message
                                   MPFROM2SHORT(sIndex, TRUE),   // Message Parameter 1
                                  (MPARAM)NULL);                 // Message Parameter 2

     switch (SHORT1FROMMP(ulMenuID))
      {
       case SC_MOVE:       // DO NOT REMOVE THE MOVE  MENUITEM
       case SC_CLOSE:      // DO NOT REMOVE THE CLOSE MENUITEM
            sIndex++;
            break;

       default:            // DELETE ALL OTHER MENUITEMS
            WinSendMsg(menuitem.hwndSubMenu,
                       MM_DELETEITEM,
                       MPFROM2SHORT(ulMenuID, TRUE),
                      (MPARAM)NULL);
      }
    }

    if (bDontChangeClose != TRUE)
     {
      WinSendMsg(hwndSysMenu,
                 MM_SETITEMTEXT,
                 (MPARAM)SC_CLOSE,
                 MPFROMP(szCloseItem));
     }
}
