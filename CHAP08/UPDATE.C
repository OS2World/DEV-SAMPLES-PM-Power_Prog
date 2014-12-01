// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      UPDATE.C                                         mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Routines used to update various control windows               บ
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

#define INCL_WIN             
                             
#include <os2.h>             
#include <string.h>          
#include <stdlib.h>          
#include <stdio.h>                                 
#include "pmedit.h"          
#include "pmedit.fnc"        
#include "shcommon.h"

// --------------------------------------------------------------------------     
// Used to obtain line number                                                    
// --------------------------------------------------------------------------                                                                                    
LONG     lGlobal = FALSE;


// *******************************************************************************
// FUNCTION:     UpdateLineNumbers
//
// FUNCTION USE: Used to display the current line number within the edit window.
//
// DESCRIPTION:  This simple function obtains the current line number and the
//               total number of lines within the MLE edit window.  The line
//               counts are displayed in the status window whenever the user
//               presses a key.
//
// PARAMETERS:   VOID
//
// RETURNS:      VOID
//
// INTERNALS:    Calls UpdateStatusWindow to display the line counts
//               in the status window.
//
// HISTORY:
//
// *******************************************************************************
VOID UpdateLineNumbers(VOID)
{
 LONG line;
 LONG lTotalLines;
 CHAR szLineNumber[100];

 // --------------------------------------------------------------------------       
 // get the current line number
 // --------------------------------------------------------------------------       
 line = (LONG) WinSendMsg(hwndMLE, MLM_LINEFROMCHAR, (MPARAM) -1, 0);

 // --------------------------------------------------------------------------       
 // The lGlobal variable is initialized to zero.  It is updated at the
 // end of this routine with the current line number.  This conditional
 // is used to check whether or not the current line has been changed
 // by the user as a result of a pressing a character on the keyboard or
 // pressing the mouse.  If the current line is different from the value in
 // lGlobal, the function will obtain the total line count and update
 // the status window with a string of the form.
 //
 // ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
 // ณ Line xxx of xxx                                                     ณ
 // ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
 //
 // --------------------------------------------------------------------------       

 if (line != lGlobal)
  {
    // --------------------------------------------------------------------------       
    // Obtain total number of lines within edit window.
    // --------------------------------------------------------------------------       
    lTotalLines = (LONG) WinSendMsg(hwndMLE, MLM_QUERYLINECOUNT, 0, 0);

    // --------------------------------------------------------------------------       
    // This is funky, but we need to add one to the line count
    // since it starts the line count with line 0 (yuck!!!)
    // --------------------------------------------------------------------------       
    sprintf(szLineNumber, "Line %d of %d", line + 1, lTotalLines);
    UpdateStatusWindow(ID_STATIC, szLineNumber);
    lGlobal = line;
  }
 return;
}


// *******************************************************************************
// FUNCTION:     SetDefaultFont
//
// FUNCTION USE: Used to set the default font for the editor to a monospaced font
//
// DESCRIPTION:  The proportional fonts are difficult to read some times and some
//               users are too lazy to change the font via the font dialog.  This
//               routine is used to initialize the font used by the editor window
//               to a monospaced font rather than the default proportional font.
//               This routine will be called prior to calling our FileOpen
//               routine.  However, this routine will not be called if the user
//               specifies a different font.
//
// PARAMETERS:   HWND     window handle of editor window
//
// RETURNS:      VOID
//
// HISTORY:
//
// *******************************************************************************
BOOL SetDefaultFont(HWND hwndMLE)
{
  CHAR   szFontNameSize[30];
  BOOL   rc;

  // --------------------------------------------------------------------------       
  // See if the user font is stored in the OS2.INI file
  // --------------------------------------------------------------------------       
  PrfQueryProfileString(HINI_USER,
                        APPNAME,
                        "User Font",
                        NULL,
                        (PVOID)szFontNameSize,
                        sizeof(szFontNameSize));


  // --------------------------------------------------------------------------       
  // If no font name is stored in the USER profile, then 
  // let's set the default font to be Courier 8.
  // --------------------------------------------------------------------------       
  if (strlen(szFontNameSize) == NULLHANDLE)
   {
    strcpy(szFontNameSize, "8.Courier");
    WinSetPresParam(hwndMLE, PP_FONTNAMESIZE, sizeof(szFontNameSize) + 1, szFontNameSize);
   }

  // --------------------------------------------------------------------------       
  // If a font is stored in the profile, use it!
  // --------------------------------------------------------------------------       
  else
   {
    WinSetPresParam(hwndMLE, PP_FONTNAMESIZE, sizeof(szFontNameSize) + 1, szFontNameSize);
   }
  return rc;
}

// *******************************************************************************                          
// FUNCTION:     UpdateStatusWindow                                                                        
//                                                                                                          
// FUNCTION USE: Used to update a simple static text status window.                                    
//                                                                                                          
// DESCRIPTION:  The purpose of the status window is threefold.  First, the                            
//               status window is used to display the current line number as the                            
//               user moves the arrow keys or mouse.  The status window will also                           
//               display text when the user presses the right mouse button on any                          
//               button in the button bar.  A brief description of what the                      
//               button does will be displayed.  The status window is also used                          
//               to indicate to the user that some important file event has                             
//               occurred, for example (file saved).
//                                                                                                          
// PARAMETERS:   USHORT     The window identifier for the static window.                             
//               PCH        The text to be displayed in the static window.
//                                                                                                          
// RETURNS:      VOID                                                                                       
//                                                                                                          
// HISTORY:                                                                                                 
//   mm-dd-yy                                                   
//                                                                                                          
// *******************************************************************************                          
VOID UpdateStatusWindow(USHORT usStaticID, PCH pchBuffer)
{
 HWND   hwndClient;                      
 HWND   hwndStatic;                      
 CHAR   szBuffer[CCHMAXPATH];            
 CHAR   szFont[25];                      

 // --------------------------------------------------------------------------                 
 // Get the window handles for the client window and the static text window.                
 // --------------------------------------------------------------------------                 
 hwndClient = WinWindowFromID(hwndFrame, FID_CLIENT);                           
 hwndStatic = WinWindowFromID(hwndClient, usStaticID);                           
                                                                               
 // --------------------------------------------------------------------------                 
 // Set the font for the text.                                      
 // --------------------------------------------------------------------------                 
 strcpy(szFont, "8.Courier");                                                   
 WinSetPresParam(hwndStatic, PP_FONTNAMESIZE, sizeof(szFont) + 1, szFont);      
                                                                               
 // --------------------------------------------------------------------------                
 // Set the text to the text string that was passed by the caller.                        
 // --------------------------------------------------------------------------                
 WinSetWindowText(hwndStatic, pchBuffer);                                        
 return;
}

// *******************************************************************************                 
// FUNCTION:     UpdateTitleBar                                                                
//                                                                                                 
// FUNCTION USE: Used to update the title bar.                                                     
//                                                                                                 
// DESCRIPTION:  The purpose of this routine is to change the titlebar text.                      
//                                                                                                 
// PARAMETERS:   HWND    The frame window handle.                                               
//               PCH     The title bar text. 
//                                                                                                 
// RETURNS:      VOID                                                                              
//                                                                                                 
// HISTORY:                                                                                        
//   mm-dd-yy                                                                                      
//                                                                                                 
// *******************************************************************************                 
VOID UpdateTitleBar(HWND hwnd, PCH pchBuffer)                                                                  
{                                                                                                
  HSWITCH hswitch;              
  SWCNTRL swctl;                
  BOOL    rc;

  CHAR szTitle[CCHMAXPATH];
                                                                                                 

  // --------------------------------------------------------------------------                
  // Put the path and filename into the titlebar string, so that it looks                                                                 
  // like this:
  // 
  // PMEDIT - X:\PATH\FILE.TXT
  // --------------------------------------------------------------------------                

  rc = strcmp(TITLEBAR, pchBuffer);                             

  if (rc) 
   {
    strcpy(szTitle, PREFIX);
    strcat(szTitle, pchBuffer);
    WinSetWindowText(WinWindowFromID(hwnd, FID_TITLEBAR), szTitle);                   
   }

  else
   {
    WinSetWindowText(WinWindowFromID(hwnd, FID_TITLEBAR), pchBuffer);             
   }

  // --------------------------------------------------------------------------          
  // Change the switch list entry to include the new filename.
  // --------------------------------------------------------------------------          
  hswitch = WinQuerySwitchHandle(hwnd, 0);            
  WinQuerySwitchEntry(hswitch, &swctl);                    
                                                           
  strcpy(swctl.szSwtitle, pchBuffer);             
  WinChangeSwitchEntry(hswitch, &swctl);                   
  return;
}          

// *******************************************************************************
// FUNCTION:     ToggleEditMenu
//
// FUNCTION USE: Will enable or disable menuitems from the edit menu accordingly
//
// DESCRIPTION:  This function will enable or disable the cut, copy and paste
//               menuitems.
//
// PARAMETERS:   HWND     The handle of the menu window
//               USHORT   The identifier of the menuitem to enable or disable
//
// RETURNS:      VOID
//
// HISTORY:
//   mm-dd-yy    ####   Programmer Name   Created the routine
//
// *******************************************************************************
VOID ToggleEditMenu(HWND hwndMenu, USHORT usID)
{
 BOOL   rc;
 ULONG  ulFormat;
 HAB    hab;
 PCH    pchBuffer;
 MRESULT min;
 MRESULT max;

 // --------------------------------------------------------------------------
 // Get an anchor block handle
 // --------------------------------------------------------------------------
 hab = WinQueryAnchorBlock(WinQueryWindow(hwndMenu, QW_PARENT));

 switch (usID)
 {
  case IDM_EDITCUT:
  case IDM_EDITCOPY:
  case IDM_EDITCLEAR: 
       // --------------------------------------------------------------------------
       // Enable cut and copy if the user has some text selected. 
       // --------------------------------------------------------------------------
       min = WinSendMsg(hwndMLE, MLM_QUERYSEL, MPFROMSHORT(MLFQS_MINSEL), NULL);                                                      
       max = WinSendMsg(hwndMLE, MLM_QUERYSEL, MPFROMSHORT(MLFQS_MAXSEL), NULL);                                                      
       if (min != max)                                                         
        {
         WinEnableMenuItem(hwndMenu, usID, TRUE);
        }

       // --------------------------------------------------------------------------
       // Disable cut and copy if the user does not have some text selected.
       // --------------------------------------------------------------------------
       else
        {
         WinEnableMenuItem(hwndMenu, usID, FALSE);
        }
       return;

  case IDM_EDITPASTE:
       // --------------------------------------------------------------------------
       // Enable paste if we have valid data in the clipboard.
       // --------------------------------------------------------------------------
       rc = WinQueryClipbrdFmtInfo (hab, CF_TEXT, &ulFormat);
       if (rc)
        {
         WinEnableMenuItem(hwndMenu, usID, TRUE);
        }

       // --------------------------------------------------------------------------
       // Disable paste if we do not have anything in the clipboard.
       // --------------------------------------------------------------------------
       else
        {
         WinEnableMenuItem(hwndMenu, usID, FALSE);
        }
       return;
 }
}
