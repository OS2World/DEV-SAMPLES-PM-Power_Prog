// UJSFMT,1
// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      ICON.C                                           mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Miscellaneous Icon Functions                                  บ
// บ                                                                               บ
// บ  NOTES:         This file contains routines that are used to update icons.    บ
// บ                                                                               บ
// บ               - The UpdateIcon function is used to update the static icon     บ
// บ                 that is in the Configure Program Button dialog box.           บ
// บ                                                                               บ
// บ               - The UpdateBtnIcon function is used to update the program      บ
// บ                 button icons whenever the user makes a change to the          บ
// บ                 program buttons.                                              บ
// บ                                                                               บ
// บ               - The ChangeIcon function is used to change the ICON            บ
// บ                 stored in the executables EA information, or reset the        บ
// บ                 icon back to the default for the executable.                  บ
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
#include "buttons.h"
#include "messages.h"
#include "shcommon.h"



// *******************************************************************************
// FUNCTION:     UpdateIcon
//
// FUNCTION USE: Updates a static icon in a dialog
//
// DESCRIPTION:  This function is used to change a static icon that is    
//               displayed in a dialog box(SS_ICON).
//
// PARAMETERS:   hwndDlg       The window handle of the dialog window
//               ulIconID      The icon identifier
//
//
// RETURNS:      TRUE   if the icon could be found in the EXE
//               FALSE  if WinLoadFileIcon failed
//
// INTERNALS:    NONE
//
// HISTORY:
//
// *******************************************************************************
BOOL UpdateIcon(HWND hwndDlg, ULONG ulIconID, PSZ pszIcon)
{
   HWND      hwndButton;
   HPOINTER  hptr;
   BOOL      rc;

   // Obtain the handle of the static icon window and hide 
   // the window until we update it with the new icon.
   hwndButton = WinWindowFromID(hwndDlg, ulIconID);                     
   WinShowWindow(hwndButton, FALSE);                                    

   // If no executable is passed in pszIcon
   // then let's punt and just use the system pointer 
   // that resembles a question mark???
   if (pszIcon == NULLHANDLE) 
    {
     hptr = WinQuerySysPointer(HWND_DESKTOP, SPTR_ICONQUESTION, FALSE);  
     if (!hptr) 
      {
       DisplayMessages(NULLHANDLE, "test", MSG_INFO);
      } 
    } 

   else
    {
     // If the caller passed us a valid executable program
     // load its icon and store the handle
     hptr = WinLoadFileIcon(pszIcon, FALSE);
     if (hptr == NULLHANDLE) 
      {
       // If WinLoadFileIcon failed return FALSE
       return FALSE;
      } 
    }

   // Update our static icon and return TRUE
   WinSendMsg(hwndButton, SM_SETHANDLE, (MPARAM)hptr, NULL);
   WinShowWindow(hwndButton, TRUE);
   return TRUE;
}

// *******************************************************************************
// FUNCTION:     UpdateBtnIcon
//
// FUNCTION USE: Updates a icon in a button
//
// DESCRIPTION:  This function is used to change an icon that is displayed
//               in a button(BS_ICON).
//
// PARAMETERS:   hwndDlg      The window handle of the window
//               ulIconID     The icon identifier
//               pszIcon      The executable program
//
// RETURNS:      VOID
//
// INTERNALS:    NONE
//
// HISTORY:
//
// *******************************************************************************
VOID UpdateBtnIcon(HWND hwndDlg, ULONG ulIconID, PSZ pszIcon)
{
   HWND        hwndButton;
   HPOINTER    hptr;
   BOOL        rc;
   WNDPARAMS   wp;
   BTNCDATA    bcd;

   // Always fill in the cb element of a control
   // data structure, and include the button control
   // data in the pCtlData element of the WNDPARAMS
   // structure.
   bcd.cb = sizeof(BTNCDATA);
   wp.fsStatus = WPM_CTLDATA;
   wp.pCtlData = &bcd;

   // Get the handle of the pushbutton window and hide 
   // it until we update the window with the new icon.
   hwndButton = WinWindowFromID(hwndDlg, ulIconID);
   WinShowWindow(hwndButton, FALSE);

   // We don't really do anything with this, but if we 
   // needed information from the old control data we 
   // can get it by sending this WM_QUERYWINDOWPARAMS
   // message.
   WinSendMsg(hwndButton, WM_QUERYWINDOWPARAMS, (MPARAM)&wp, NULL);

   // Load the icon from the executable and send the button 
   // window a WM_SETWINDOWPARAMS message with the new control
   // data, containing the updated icon. 
   bcd.hImage = WinLoadFileIcon(pszIcon, FALSE);
   WinSendMsg(hwndButton, WM_SETWINDOWPARAMS, (MPARAM)&wp, NULL);
   WinShowWindow(hwndButton, TRUE);

   return;
}

// *******************************************************************************
// FUNCTION:     ChangeIcon
//
// FUNCTION USE: Stores an ICON in the executables EA's
//
// DESCRIPTION:  This function is used to set the ICONDATA within the executable
//               so that it contains the specified icon.  Make sure you have 
//               INCL_WINPOINTERS defined or this will fail!!! The function will
//               turn around and call UpdateIcon to repaint the icon.
//
// PARAMETERS:   hwndDlg        The window handle of the dialog window.
//               ulIconID       The icon identifier.
//               pszExecutable  The program executable that we are inserting 
//                              ICON ea information for.
//               pszIcon        The icon to store in the executables 
//                              ICON information.
//
//
// RETURNS:      TRUE          Success
//               FALSE         Failure
//
// INTERNALS:    UpdateIcon
//
// HISTORY:
//
// *******************************************************************************
BOOL ChangeIcon(HWND hwndDlg, ULONG ulIconID, PSZ pszExecutable, PSZ pszIcon)
{
   BOOL      rc;
   ICONINFO  iconinfo;

   // If the user did not pass us an icon file, we will
   // clear all of the icon information.
   if (!pszIcon) 
    {
     iconinfo.fFormat      = ICON_CLEAR;
    } 

   // If we got a valid icon file passed try to change 
   // the icon information.
   else
    {
     iconinfo.fFormat      = ICON_FILE;
    }

   // Populate our ICONINFO structure
   iconinfo.cb           = sizeof(ICONINFO);
   iconinfo.pszFileName  = pszIcon;
   iconinfo.hmod         = (HMODULE)NULL;
   iconinfo.resid        = NULLHANDLE;
   iconinfo.cbIconData   = NULLHANDLE;
   iconinfo.pIconData    = (PVOID)NULL;

   // If we cannot set the ICON information, WinSetFileIcon
   // will return FALSE.  One reason we would fail would be 
   // if the program executable is already running, you cannot
   // update its ICON information.  Use DisplayMessages to 
   // display a meaningful error message to the user, and 
   // return FALSE back to the caller.
   rc = WinSetFileIcon(pszExecutable, &iconinfo);
   if (rc != TRUE)
    {
     DisplayMessages(ERROR_CHANGING_ICON, NULLHANDLE, MSG_ERROR);
     return FALSE;
    }

   // If we get here, then everything is fine and dandy.  The 
   // ICON information has been successfully updated, so we 
   // should update the static icon in the Configure Program Buttons
   // dialog box, since this function is called in the context of 
   // that window.  This way the user can see the icon that they 
   // have changed.
   UpdateIcon(hwndDlg, ulIconID, pszExecutable);
   return TRUE;
}
