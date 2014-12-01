// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      OPTIONS.C                                        mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Alternate Options                                             บ
// บ                                                                               บ
// บ  NOTES:         This file contains routines for the options menu.  Primarily  บ
// บ                 this file contains a dialog procedure to process the user     บ
// บ                 preferences configuration dialog.  This dialog box allows     บ
// บ                 the user to write global changes to the OS2.INI file that     บ
// บ                 will be maintained throughout the program.                    บ
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

#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "showoff.h"
#include "shcommon.h"
#include "messages.h"
#include "sherror.h"

#define OPTION_YES      "1"
#define OPTION_NO       "0"
#define PRFSUCCESS      TRUE

PSZ pszKeynames[] = {KEY_LOGO, KEY_STRETCH, KEY_WNDEXIT};

// *******************************************************************************      
// FUNCTION:     ConfigureUserDlgProc                                                   
//                                                                                      
// FUNCTION USE: The dialog code to handle the user configurable settings               
//                                                                                      
// DESCRIPTION:  Within SHOWOFF.H the following defines are setup for the
//               checkbox identifiers.  These numbers MUST be consecutive
//               integers because we will use a counter called usCounter 
//               to determine which window identifier we have.  
//                                         
//               #define IDC_SUPLOGO               0                       
//               #define IDC_ACTUALSIZE            1                       
//               #define IDC_SAVEWNDEXIT           2                       
//               #define IDC_END                   3                       
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
MRESULT EXPENTRY ConfigureUserDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 USHORT   usCheckState;
 USHORT   usPrfRC;
 USHORT   usCounter;
 LONG     lReturn;

 static   HWND  hwndHelp;

 switch(msg)
  {
   case WM_INITDLG:
        // --------------------------------------------------------------------------
        // Remove the system menu items that are not required
        // --------------------------------------------------------------------------
        SetTheSysMenu(hwnd, NULL);

        // --------------------------------------------------------------------------         
        // Get the window handle for the help window out of the window words for              
        // our frame window.                                                                  
        // --------------------------------------------------------------------------         
        hwndHelp = WinQueryWindowULong(hwndFrame, QWL_USER);                                 

        // --------------------------------------------------------------------------      
        // Check all of the settings in the INI file, and check the appropriate
        // checkboxes for those items that are currently selected.
        // --------------------------------------------------------------------------
        for (usCounter = IDC_SUPLOGO; usCounter < IDC_END; usCounter++)
         {
          lReturn = PrfQueryProfileInt(HINI_USER, APPNAME, pszKeynames[usCounter], 0);
          if (lReturn == CHECKED)
           {
            WinCheckButton(hwnd, usCounter, CHECKED);
           }
         }
        return FALSE;

   case WM_COMMAND:
        switch(SHORT1FROMMP(mp1))
         {
          case DID_OK:
               // --------------------------------------------------------------------------
               // Loop through all of our checkboxes and determine whether or not the 
               // checkbox is checked.
               // --------------------------------------------------------------------------
               for (usCounter = IDC_SUPLOGO; usCounter < IDC_END; usCounter++)
                {
                 usCheckState = WinQueryButtonCheckstate(hwnd, usCounter);
                 switch (usCheckState)
                  {
                   case UNCHECKED:  // Unchecked
                        usPrfRC = PrfWriteProfileString(HINI_USER, APPNAME, pszKeynames[usCounter], OPTION_NO); 
                        if (usPrfRC != PRFSUCCESS)
                         {
                          DisplayMessages(ERROR_WRITING_PROFILE, NULLHANDLE, MSG_ERROR);
                         }
                        break;

                   case CHECKED:    // Checked
                        usPrfRC = PrfWriteProfileString(HINI_USER, APPNAME, pszKeynames[usCounter], OPTION_YES); 
                        if (usPrfRC != PRFSUCCESS)
                         {
                          DisplayMessages(ERROR_WRITING_PROFILE, NULLHANDLE, MSG_ERROR);
                         }
                        break;
                  } // End of switch
                }  // End for loop
               WinDismissDlg(hwnd, TRUE);
               return FALSE;

          case DID_CANCEL:
               WinDismissDlg(hwnd, TRUE);
               return FALSE;

          case DID_HELP:
               if (hwndHelp)
                WinSendMsg(hwndHelp, HM_DISPLAY_HELP, MPFROMSHORT(20817), 0L);
               return FALSE;

          default:
               return FALSE;
         }
        break;
     
   default:
        return WinDefDlgProc(hwnd, msg, mp1, mp2);
  }
 return FALSE;
}

// *******************************************************************************
// FUNCTION:     QueryDeviceInfoStatus
//
// FUNCTION USE: Queries the current screen resolution.
//
// DESCRIPTION:  This routine is used to determine what the current
//               screen resolution is, along with the amount of colors
//               that the video device driver is currently supporting.
//
// PARAMETERS:   HWND     window handle
//
// RETURNS:      VOID
//
// HISTORY:
//
// *******************************************************************************
VOID QueryDeviceInfoStatus(HWND hwndFrame)
{
   SHORT  sScreenHeight;
   SHORT  sScreenWidth;
   HDC    hdcTemp;
   LONG   lColorsArray[CAPS_COLORS];
   CHAR   szBuffer[150];
   APIRET rc;

   // --------------------------------------------------------------------------
   // First get screen height and Width from PM using WinQuerySysValue
   // --------------------------------------------------------------------------
   sScreenHeight = (SHORT)WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);
   sScreenWidth  = (SHORT)WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);

   // --------------------------------------------------------------------------
   // Get a Device Context Handle.
   // --------------------------------------------------------------------------
   hdcTemp = WinOpenWindowDC(hwndFrame);

   // --------------------------------------------------------------------------
   // Get the number if Colors supported.
   // --------------------------------------------------------------------------
   rc = DevQueryCaps(hdcTemp,
                     CAPS_COLORS,
                     CAPS_COLORS,
                     lColorsArray);

   if (!rc)
    {
     sprintf(szBuffer, "DevQueryCaps failed with RC = %d", rc);
     DisplayMessages(NULLHANDLE, szBuffer, MSG_INFO);
    }

   // --------------------------------------------------------------------------
   // Display the Current Resolution.
   // --------------------------------------------------------------------------
   sprintf(szBuffer, "The current screen resolution is: \n%ld x %ld x %ld colors", sScreenWidth, sScreenHeight, lColorsArray[0]);
   DisplayMessages(NULLHANDLE, szBuffer, MSG_INFO);
   DevCloseDC(hdcTemp);
   return;
}

