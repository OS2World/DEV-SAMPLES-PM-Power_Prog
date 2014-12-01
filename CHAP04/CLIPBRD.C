// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป                 
// บ  FILENAME:      CLIPBRD.C                                        mm/dd/yy     บ                 
// บ                                                                               บ                 
// บ  DESCRIPTION:   Clipboard Functions                                           บ                 
// บ                                                                               บ                 
// บ  NOTES:         This file contains routines that are used to handle the       บ                 
// บ                 processing of the PM Clipboard.                               บ                 
// บ                                                                               บ                 
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ                 
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ                 
// บ                                                                               บ                 
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ                 
// บ                                                                               บ                 
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ                 

#define INCL_WIN                          
#define INCL_GPI
                                          
#include <os2.h>                          
#include "clkdraw.h"                      
#include "shcommon.h"                     
                                          

// *******************************************************************************
// FUNCTION:     PutBitmapInClipboard
//
// FUNCTION USE: Copies a bitmap into the system clipboard
//
// DESCRIPTION:  This function is used to copy the bitmap data captured as a
//               result of the tracking to status window, to the system
//               clipboard.
//
// PARAMETERS:   HBITMAP  The bitmap handle of the bitmap to be placed
//                        in the clipboard.
//
// RETURNS:      TRUE     success
//               FALSE    error
//
// HISTORY:
//   mm-dd-yy    ####   Programmer Name   Created the routine
//
// *******************************************************************************
BOOL PutBitmapInClipboard(HBITMAP hbmClipboard)
{
  HAB  habTemp;
  BOOL rc;


  // --------------------------------------------------------------------------                         
  // Get an anchor block handle                                                                        
  // --------------------------------------------------------------------------                         
  habTemp = WinQueryAnchorBlock(HWND_DESKTOP);

  // --------------------------------------------------------------------------                         
  // Attempt to open the Clipboard                                                                      
  // --------------------------------------------------------------------------                         
  rc = WinOpenClipbrd(habTemp);

  // --------------------------------------------------------------------------       
  // If we get an error opening, return FALSE and post message           
  // --------------------------------------------------------------------------       
  if (rc != TRUE)  
   {
    DisplayMessages(NULLHANDLE, "Error Opening Clipboard", MSG_ERROR);
    return rc;
   }

  // --------------------------------------------------------------------------                    
  // OK, no error so let's empty the clipboard and
  // place our bitmap in there!
  // --------------------------------------------------------------------------                    
  else
   {
    rc = WinEmptyClipbrd(habTemp);
    if (rc != TRUE)
     {
      DisplayMessages(NULLHANDLE, "Error Emptying Data In Clipboard", MSG_ERROR);
      return rc;
     }

    rc = WinSetClipbrdData(habTemp,         // anchor block handle
                           hbmClipboard,    // bitmap handle
                           CF_BITMAP,       // clipboard data format
                           CFI_HANDLE);     // format information

    if (rc != TRUE)
     {
      DisplayMessages(NULLHANDLE, "Error Placing Data In Clipboard", MSG_ERROR);
      return rc;
     }

    // --------------------------------------------------------------------------     
    // Close the clipboard                      
    // --------------------------------------------------------------------------     
    rc = WinCloseClipbrd(habTemp);
    if (rc != TRUE)
     {
      DisplayMessages(NULLHANDLE, "Error Closing Clipboard", MSG_ERROR);
      return rc;
     }
   }
  return TRUE;
}


// *******************************************************************************
// FUNCTION:     GetBitmapFromClipboard
//
// FUNCTION USE: Retrieves bitmap data from the clipboard
//
// DESCRIPTION:  This function is used to handle the paste functionality, by 
//               retrieving bitmap data (CF_BITMAP) from the clipboard.  The
//               function will set a bitmap into an HPS if passed a valid PS
//               handle.
//
// PARAMETERS:   HPS      Presentation space handle to place the bitmap in.
//
// RETURNS:      TRUE     success
//               FALSE    error
//
// HISTORY:
//   mm-dd-yy    ####   Programmer Name   Created the routine
//
// *******************************************************************************
HBITMAP GetBitmapFromClipboard(HPS hpsMemory)
{
 HAB     habTemp;
 HBITMAP hbmClip;
 APIRET  rc;

 // --------------------------------------------------------------------------           
 // Get an anchor block handle                                                           
 // --------------------------------------------------------------------------           
 habTemp = WinQueryAnchorBlock(HWND_DESKTOP);                                                            

 // --------------------------------------------------------------------------             
 // Open the clipboard                                                              
 // --------------------------------------------------------------------------             
 rc = WinOpenClipbrd (habTemp);                                                      

 // --------------------------------------------------------------------------                  
 // If we get an error opening, return FALSE and post message  
 // --------------------------------------------------------------------------                  
 if (!rc)  
  {                                                                                   
   return rc;                                                                         
  }                                                                                   
                                                                                 
 // --------------------------------------------------------------------------                 
 // Get the bitmap data out of the clipboard                                 
 // --------------------------------------------------------------------------                 
 hbmClip = (HBITMAP) WinQueryClipbrdData (habTemp, CF_BITMAP);                        

 // --------------------------------------------------------------------------                       
 // WinQueryClipbrdData will return a NULLHANDLE if there is no 
 // bitmap data in the clipboard or if an error occurred.
 // --------------------------------------------------------------------------             
 if (hbmClip == NULLHANDLE) 
  {
   return FALSE;
  }

 // --------------------------------------------------------------------------                     
 // Set our bitmap into our memory presentation space                                                      
 // --------------------------------------------------------------------------                     
 GpiSetBitmap (hpsMemory, hbmClip);                                               

 // --------------------------------------------------------------------------                          
 // Close the clipboard and return our bitmap handle                                                
 // --------------------------------------------------------------------------                          
 WinCloseClipbrd (habTemp);                                                           
 return hbmClip;
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
//               HBITMAP  The bitmap handle of the bitmap from the tracking 
//                        operation.  If no tracking is done, we do not enable
//                        the cut and copy menuitems.
//
// RETURNS:      VOID                                                                        
//                                                                                           
// HISTORY:                                                                                  
//   mm-dd-yy    ####   Programmer Name   Created the routine                                
//                                                                                           
// *******************************************************************************           
VOID ToggleEditMenu(HWND hwndMenu, USHORT usID, HBITMAP hbmClip)                                                
{                                                                                              
 BOOL   rc;                                                                                      
 ULONG  ulFormat;                                                                               
 HAB    hab;                                                                                    
                                                                                               
 // --------------------------------------------------------------------------                         
 // Get an anchor block handle                                                                        
 // --------------------------------------------------------------------------                         
 hab = WinQueryAnchorBlock(WinQueryWindow(hwndMenu, QW_PARENT));                               
                                                                                               
 switch (usID)                                                                                 
 {                                                                                             
  case IDM_EDITCUT:
  case IDM_EDITCOPY:
       // --------------------------------------------------------------------------                 
       // Enable cut and copy if we have a valid bitmap handle from our                       
       // tracking operation.
       // --------------------------------------------------------------------------                 
       if (hbmClip) 
        {
         WinEnableMenuItem(hwndMenu, usID, TRUE);                         
        }

       // --------------------------------------------------------------------------                 
       // Disable cut and copy if we don't have a bitmap                          
       // --------------------------------------------------------------------------                 
       else
        {
         WinEnableMenuItem(hwndMenu, usID, FALSE);                         
        }
       return FALSE;

  case IDM_EDITPASTE:                                                                              
       // --------------------------------------------------------------------------             
       // Enable paste if we have a valid bitmap handle in the clipboard.                
       // --------------------------------------------------------------------------             
       rc = WinQueryClipbrdFmtInfo (hab, CF_BITMAP, &ulFormat);                                
       if (rc)                                                                         
        {                                                                                      
         WinEnableMenuItem(hwndMenu, usID, TRUE);                                              
        }                                                                                      

       // --------------------------------------------------------------------------             
       // Disable paste if we do not have a valid bitmap.                
       // --------------------------------------------------------------------------             
       else                                                                                    
        {                                                                                      
         WinEnableMenuItem(hwndMenu, usID, FALSE);                                             
        }                                                                                      
       break;                                                                                  
 }                                                                                             
 return;
}
