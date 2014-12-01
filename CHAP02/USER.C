// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      USER.C                                           mm/dd/yy     บ
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
#include "buttons.h"
#include "shcommon.h"
#include "messages.h"

PSZ szKeynames[] = {KEYNAME1, KEYNAME2, KEYNAME3, KEYNAME4};

// *******************************************************************************      
// FUNCTION:     ConfigureUserDlgProc                                                   
//                                                                                      
// FUNCTION USE: The dialog code to handle the user configurable settings               
//                                                                                      
// DESCRIPTION:  Within BUTTONS.H the following defines are setup for the
//               checkbox identifiers.  These numbers MUST be consecutive
//               integers because we will use a counter called usCounter 
//               to determine which window identifier we have.  
//                                         
//                #define IDC_FRAMECTLS   0                  
//                #define IDC_WINLIST     1                  
//                #define IDC_CONFIRM     2                  
//                #define IDC_COMLINE     3                  
//                #define IDC_LAST        4                  
//
// PARAMETERS:   this right usCounter will contain:                      
//               CHAR *     on exit contains X:\DIRECTORY                               
//               CHAR *     on exit contains FILE.TXT                                   
//                                                                                      
// RETURNS:      MRESULT                                                                
//                                                                                      
// INTERNALS:    DisplayMessages                                                        
//                                                                                      
// HISTORY:                                                                             
//                                                                                      
// *******************************************************************************      
MRESULT EXPENTRY ConfigureDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 USHORT   usCheckState;
 USHORT   usPrfRC;
 USHORT   usCounter;
 LONG     lReturn;

 switch(msg)
  {
   case WM_INITDLG:
        // Remove the system menu items that are not required
        SetTheSysMenu(hwnd, NULL);

        // Check all of the settings in the INI file, and check 
        // the appropriate checkboxes for those items that are 
        // currently selected.
        for (usCounter = IDC_FRAMECTLS; usCounter < IDC_LAST; usCounter++)
         {
          lReturn = PrfQueryProfileInt(HINI_USER, APPNAME, szKeynames[usCounter], 0);
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
               // Loop through all of our checkboxes and determine 
               // whether or not the button is checked.
               for (usCounter = IDC_FRAMECTLS; usCounter < IDC_LAST; usCounter++)
                {
                 usCheckState = WinQueryButtonCheckstate(hwnd, usCounter);
                 switch (usCheckState)
                  {
                   case UNCHECKED:  // Unchecked
                        usPrfRC = PrfWriteProfileString(HINI_USER, APPNAME, szKeynames[usCounter], OPTION_NO); 
                        if (usPrfRC != PRFSUCCESS)
                         {
                          DisplayMessages(ERROR_WRITING_PROFILE, NULLHANDLE, MSG_ERROR);
                         }
                        break;

                   case CHECKED:    // Checked
                        usPrfRC = PrfWriteProfileString(HINI_USER, APPNAME, szKeynames[usCounter], OPTION_YES); 
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
               DisplayMessages(INFO_NOHELP, NULLHANDLE, MSG_INFO);
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
