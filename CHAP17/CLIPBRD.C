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
#include <memory.h>

#include "pmscreen.fnc"
#include "pmscreen.h"
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
//               NOTE: This routine has been slightly modified from the routine
//                     used in the Chapter 4 sample program.
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
HBITMAP GetBitmapFromClipboard(VOID)
{
 HAB     habTemp;
 HBITMAP hbmTemp;
 APIRET  rc;

 static HBITMAP hbmClip;

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
 hbmTemp = (HBITMAP) WinQueryClipbrdData (habTemp, CF_BITMAP);                        

 // --------------------------------------------------------------------------                       
 // WinQueryClipbrdData will return a NULLHANDLE if there is no 
 // bitmap data in the clipboard or if an error occurred.
 // --------------------------------------------------------------------------             
 if (hbmTemp != NULLHANDLE)                                                        
  {                                                                                
   // --------------------------------------------------------------------------                     
   // Duplicate our bitmap handle.                                                            
   // --------------------------------------------------------------------------                     
   hbmClip = DuplicateBitmap (hbmTemp);                                          
   if (hbmClip == NULLHANDLE)                                                     
    {                                                                              
     DisplayMessages(NULLHANDLE, "Error Duplicating Bitmap", MSG_EXCLAMATION);     
    }                                                                              
  }                                                                                

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
//               HBITMAP  The bitmap handle
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
  case IDM_EDITCLEAR:            
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
       return;

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
       return;                                                                                  
 }                                                                                             
}

// *******************************************************************************
// FUNCTION:     DuplicateBitmap
//
// FUNCTION USE: Duplicates a bitmap
//
// DESCRIPTION:  Uses GpiBitBlt to make a copy of a bitmap by bit-bliting the
//               contents of the source PS containing the bitmap, to the target
//               presentation space.
//
// PARAMETERS:   HBITMAP    source bitmap handle to copy
//
// RETURNS:      HBITMAP    target bitmap handle
//
// HISTORY:
//
// *******************************************************************************
HBITMAP DuplicateBitmap (HBITMAP hbmSource)
{
 HAB                hab;
 HBITMAP            hbmTarget;
 HDC                hdcSource;
 HDC                hdcTarget;
 HPS                hpsSource;
 HPS                hpsTarget;
 SIZEL              sizl;
 POINTL             aptl[3];
 BOOL               bError = FALSE;
 BITMAPINFOHEADER2  bmp2;

 // --------------------------------------------------------------------------
 // Get an anchor block handle
 // --------------------------------------------------------------------------
 hab = WinQueryAnchorBlock(HWND_DESKTOP);

 // --------------------------------------------------------------------------
 // Create memory device context handles and presentation space handles
 // --------------------------------------------------------------------------
 hdcSource = DevOpenDC (hab, OD_MEMORY, "*", 0L, NULL, NULLHANDLE);
 hdcTarget = DevOpenDC (hab, OD_MEMORY, "*", 0L, NULL, NULLHANDLE);

 // --------------------------------------------------------------------------
 // Set presentation space size
 // --------------------------------------------------------------------------
 sizl.cx = 0;
 sizl.cy = 0;

 // --------------------------------------------------------------------------
 // Create our source presentation space
 // --------------------------------------------------------------------------
 hpsSource = GpiCreatePS (hab,
                          hdcSource,
                          &sizl,
                          PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC);

 // --------------------------------------------------------------------------
 // Create our target presentation space
 // --------------------------------------------------------------------------
 hpsTarget = GpiCreatePS (hab,
                          hdcTarget,
                          &sizl,
                          PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC);


 // --------------------------------------------------------------------------
 // Initialize the bitmap info header structure
 // --------------------------------------------------------------------------
 memset(&bmp2, 0, sizeof(BITMAPINFOHEADER2));

 // --------------------------------------------------------------------------
 // Set the size of the structure
 // --------------------------------------------------------------------------
 bmp2.cbFix = sizeof (BITMAPINFOHEADER2);

 // --------------------------------------------------------------------------
 // Get the bitmap information header from the source bitmap
 // --------------------------------------------------------------------------
 GpiQueryBitmapInfoHeader (hbmSource, &bmp2);

 // --------------------------------------------------------------------------
 // Create our target bitmap using the bitmap information header from
 // our source bitmap.
 // --------------------------------------------------------------------------
 hbmTarget = GpiCreateBitmap (hpsTarget, &bmp2, 0L, NULL, NULL);

 if (hbmTarget == GPI_ERROR) 
  {
   // --------------------------------------------------------------------------           
   // If we get an error creating the bitmap set the bError flag to TRUE                   
   // so we can return GPI_ERROR telling the caller that the function failed.              
   // --------------------------------------------------------------------------           
   bError = TRUE;                                                                        
  }                                                                                      

 // --------------------------------------------------------------------------
 // If everything is cool, set the bitmaps into our presentation spaces.
 // --------------------------------------------------------------------------
 else
  {
   GpiSetBitmap (hpsSource, hbmSource);
   GpiSetBitmap (hpsTarget, hbmTarget);

   aptl[0].x = 0;
   aptl[0].y = 0;
   aptl[1].x = bmp2.cx;
   aptl[1].y = bmp2.cy;
   aptl[2].x = 0;
   aptl[2].y = 0;

   // --------------------------------------------------------------------------
   // Bit blit the contents of the source presentation space into our target
   // presentation space.  This is how we copy the bitmap from the source to
   // the target.
   // --------------------------------------------------------------------------
   bError = GpiBitBlt (hpsTarget,      //  Target presentation space handle
                       hpsSource,      //  Source presentation space handle
                       3L,             //  Number of Points
                       aptl,           //  Array of Points
                       ROP_SRCCOPY,    //  Mixing function (source copy)
                       BBO_IGNORE);    //  Options

   // --------------------------------------------------------------------------
   // If we get an error copying from our source PS to our target PS
   // set the error flag to TRUE so that we can return GPI_ERROR back
   // to the caller.
   // --------------------------------------------------------------------------
   if (bError == GPI_ERROR)
    {
     bError = TRUE;
    }

   // --------------------------------------------------------------------------
   // If we got a value back from GpiBitBlt reset our error flag since
   // bError would be TRUE.
   // --------------------------------------------------------------------------
   else
    {
     bError = FALSE;
    }
  }

 // --------------------------------------------------------------------------
 // Regardless of what happens, we will free all of our graphics engine
 // resources by destroying our presentation spaces and device context
 // handles.
 // --------------------------------------------------------------------------
 GpiDestroyPS (hpsSource);
 GpiDestroyPS (hpsTarget);
 DevCloseDC (hdcSource);
 DevCloseDC (hdcTarget);

 // --------------------------------------------------------------------------
 // If we get an error trying to duplicate the bitmap, return GPI_ERROR.
 // --------------------------------------------------------------------------
 if (bError)
  {
   return GPI_ERROR;
  }

 // --------------------------------------------------------------------------
 // If everthing worked like a champ, return the bitmap handle.
 // --------------------------------------------------------------------------
 else
  {
   return hbmTarget;
  }
}

