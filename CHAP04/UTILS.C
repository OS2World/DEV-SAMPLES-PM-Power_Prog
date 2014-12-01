// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      UTILS.C                                          mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Various utility functions used by all source files.           บ
// บ                                                                               บ
// บ  NOTES:         This file contains routines that are commonly used by         บ
// บ                 various functions in CLKDRAW.                                 บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#define INCL_WINSYS
#define INCL_WINMESSAGEMGR
#define INCL_WINWINDOWMGR        
#define INCL_WINFRAMEMGR
#define INCL_WINMENUS


#include <os2.h>
#include "clkdraw.h"

SYSVALUES GetSysValues(VOID);                                                                

// *******************************************************************************
// FUNCTION:     GetSysValues 
//
// FUNCTION USE: Obtains system values                                  
//
// DESCRIPTION:  This function is used to return valuable system information 
//               in the form of a SYSVALUES structure.  The structure is defined
//               in CLKDRAW.H.
//
// PARAMETERS:   VOID     
//
// RETURNS:      SYSVALUES  a structure containing the required system values
//
// HISTORY:
//
// *******************************************************************************
SYSVALUES GetSysValues(VOID)                                                                
{                                                                                           
 SYSVALUES sysvalues;                                                                       
                                                                                            
 sysvalues.lcxScreen     = WinQuerySysValue (HWND_DESKTOP, SV_CXSCREEN);                    
 sysvalues.lcyScreen     = WinQuerySysValue (HWND_DESKTOP, SV_CYSCREEN);                    
 sysvalues.lcxFullScreen = WinQuerySysValue (HWND_DESKTOP, SV_CXFULLSCREEN);                
 sysvalues.lcyFullScreen = WinQuerySysValue (HWND_DESKTOP, SV_CYFULLSCREEN);                
 sysvalues.lcxPointer    = WinQuerySysValue (HWND_DESKTOP, SV_CXPOINTER);                   
 sysvalues.lcyPointer    = WinQuerySysValue (HWND_DESKTOP, SV_CYPOINTER);                   
 sysvalues.lcxDlgFrame   = WinQuerySysValue (HWND_DESKTOP, SV_CXDLGFRAME);                  
 sysvalues.lcyDlgFrame   = WinQuerySysValue (HWND_DESKTOP, SV_CYDLGFRAME);                  
 return sysvalues;                                                                          
}                                                                                           


// *******************************************************************************
// FUNCTION:     SystemMenu
//
// FUNCTION USE: Used to remove unnecessary system menu menuitems
//
// DESCRIPTION:  This routine differs slightly from the SetTheSysMenu window.
//               It is used to revamp the system menu for the purpose of the 
//               special status window used by this application.                
//
// PARAMETERS:   HWND     window handle of the window that has a system menu
//
// RETURNS:      VOID
//
// INTERNALS:    NONE
//
// HISTORY:
//
// *******************************************************************************
VOID SystemMenu(HWND hwnd)
{
    HWND       hwndSysMenu;
    HWND       hwndFrame;
    MENUITEM   menuitem;
    ULONG      ulMenuID;
    SHORT      sNumItems;
    SHORT      sIndex = 0;

    // --------------------------------------------------------------------------               
    // Get the frame window handle by querying the client's parent              
    // --------------------------------------------------------------------------               
    hwndFrame = WinQueryWindow(hwnd, QW_PARENT);


    // --------------------------------------------------------------------------               
    // Obtain the system menu window handle from the FID_SYSMENU identifier
    // and find out how many items exist in the menu.
    // --------------------------------------------------------------------------               
    hwndSysMenu = WinWindowFromID(hwndFrame, FID_SYSMENU);

    WinSendMsg(hwndSysMenu,                                      // Window Handle
               MM_QUERYITEM,                                     // Message
               MPFROM2SHORT(SC_SYSMENU, FALSE),                  // Message Parameter 1
               MPFROMP(&menuitem));                              // Message Parameter 2

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
       // --------------------------------------------------------------------------            
       // Keep these menuitems                               
       // --------------------------------------------------------------------------            
       case SC_RESTORE:
       case SC_MAXIMIZE:
       case SC_MINIMIZE:
       case SC_MOVE:       
       case SC_CLOSE:      
            sIndex++;
            break;

       // --------------------------------------------------------------------------            
       // Blast all other system menu menuitems            
       // --------------------------------------------------------------------------            
       default:            
            WinSendMsg(menuitem.hwndSubMenu,
                       MM_DELETEITEM,
                       MPFROM2SHORT(ulMenuID, TRUE),
                      (MPARAM)NULL);
      }
    }
 return;
}
