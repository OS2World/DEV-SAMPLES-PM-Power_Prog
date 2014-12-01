// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      BITMAP.C                                         mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Bitmap Manipulation Routines                                  บ
// บ                                                                               บ
// บ  NOTES:         This file contains the functions that are used to handle      บ
// บ                 bitmaps in the sample program.                                บ
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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "shcommon.h"
#include "clkdraw.h"                     // Resource identifiers
#include "clkdraw.fnc"                   // Function Prototypes


// *******************************************************************************                   
// FUNCTION:     CreateBitmap                                                                       
//                                                                                                   
// FUNCTION USE: Creates a bitmap                                         
//                                                                                                   
// DESCRIPTION:  Calls GpiCreateBitmap to create a bitmap based on the 
//               parameters passed to this call.
//                                                                                                   
// PARAMETERS:   HPS      memory presentation space handle                                           
//               LONG     horizontal size for bitmap 
//               LONG     vertical size for bitmap
//               SHORT    number of colors                                                           
//                                                                                                   
// RETURNS:      GPI_ERROR   if an error occurs                                                                    
//               HBITMAP     bitmap handle                                                                   
//                                                                                                   
// HISTORY:                                                                                          
//                                                                                                   
// *******************************************************************************                   
HBITMAP CreateBitmap(HPS hpsMemory, LONG lcxSize, LONG lcySize,  SHORT sNumColors)
{
 static BITMAPINFOHEADER2  bmp2;               
 static HBITMAP            hbmShadow;

 // --------------------------------------------------------------------------                     
 // Create a bitmap for shadow drawing                                                              
 // --------------------------------------------------------------------------                     
 memset(&bmp2, 0, sizeof(BITMAPINFOHEADER2));                                            
 bmp2.cbFix     = sizeof (bmp2);                                                            
 bmp2.cx        = lcxSize;                                                    
 bmp2.cy        = lcySize;                                                    
 bmp2.cPlanes   = 1;                                                                        

 // --------------------------------------------------------------------------               
 // If the user wants a 2 color (monochrome) bitmap / bit count is 1                                                    
 // --------------------------------------------------------------------------               
 if (sNumColors == 2) 
  {
   bmp2.cBitCount = 1;               
  } 

 // --------------------------------------------------------------------------               
 // If the user wants a 16 color bitmap / bit count is 4                                                   
 // --------------------------------------------------------------------------               
 else if (sNumColors == 16) 
  {
   bmp2.cBitCount = 4;               
  } 

 // --------------------------------------------------------------------------               
 // If the user wants a 256 color bitmap / bit count is 8                                                                
 // --------------------------------------------------------------------------               
 else if (sNumColors == 256)                                                                                           
  {
   bmp2.cBitCount = 8;                
  }

 // --------------------------------------------------------------------------      
 // Create our bitmap                                                              
 // --------------------------------------------------------------------------      
 hbmShadow = GpiCreateBitmap (hpsMemory,  // Memory Presentation Space handle               
                              &bmp2,      // Pointer to bitmap info header structure        
                              NULLHANDLE, // Options                                        
                              NULL,       // Buffer Address                                 
                              NULL);      // Pointer to bitmap info table structure         

 // --------------------------------------------------------------------------                   
 // If we got an error return it back to the caller                                
 // --------------------------------------------------------------------------                   
 if (hbmShadow == GPI_ERROR)                                                   
  {                                                                            
   return GPI_ERROR;                                                   
  }                                                                            

 // --------------------------------------------------------------------------                 
 // If everything is cool we got a valid bitmap handle so give it back 
 // to the caller.
 // --------------------------------------------------------------------------                 
 else
  {
   return hbmShadow;
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
// memset(&bmp2, NULL, sizeof(BITMAPINFOHEADER2));           

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
                                                                                   
 // --------------------------------------------------------------------------                                                 
 // If everything is cool, set the bitmaps into our presentation spaces.                
 // --------------------------------------------------------------------------                   
 if (hbmTarget != GPI_ERROR)                     
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
 // If we get an error creating the bitmap set the bError flag to TRUE      
 // so we can return GPI_ERROR telling the caller that the function failed.
 // --------------------------------------------------------------------------            
 else
  {
   bError = TRUE;
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

// *******************************************************************************
// FUNCTION:     GetBitmapFromTrack
//
// FUNCTION USE: Creates a bitmap from a rectangle structure
//
// DESCRIPTION:  This function takes the rectangle structure that is created 
//               as a result of the tracking operation and creates a bitmap 
//               of that area.  
//
// PARAMETERS:   RECTL    tracking rectangle
//
// RETURNS:      HBITMAP  bitmap handle
//
// INTERNALS:    NONE
//
// HISTORY:
//
// *******************************************************************************
HBITMAP GetBitmapFromTrack (RECTL rclTrack)
{
 HAB                hab;                           
 HBITMAP            hbmTrack;
 HDC                hdcMemory;
 HPS                hpsMemory;
 HPS                hpsScreen;
 LONG               lFormats[2];
 POINTL             aptl[3];
 SIZEL              sizl;
 BITMAPINFOHEADER2  bmp2;

 // --------------------------------------------------------------------------                    
 // Get an anchor block handle                                                                    
 // --------------------------------------------------------------------------                    
 hab = WinQueryAnchorBlock(HWND_DESKTOP);

 // --------------------------------------------------------------------------                    
 // Get a memory device context                                                                  
 // --------------------------------------------------------------------------                    
 hdcMemory = DevOpenDC (hab, OD_MEMORY, "*", 0L, NULL, NULLHANDLE) ;

 // --------------------------------------------------------------------------                    
 // Set presentation space size                                                                
 // --------------------------------------------------------------------------                    
 sizl.cx = sizl.cy = 0 ;

 // --------------------------------------------------------------------------                             
 // Create a micro presentation space and associate it                                                                           
 // --------------------------------------------------------------------------                             
 hpsMemory = GpiCreatePS (hab,
                          hdcMemory,
                          &sizl,
                          PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC);

 // --------------------------------------------------------------------------           
 // Call GpiQueryDeviceBitmapFormats to get the number of bit planes                                   
 // --------------------------------------------------------------------------           
 GpiQueryDeviceBitmapFormats (hpsMemory,
                              2L,
                              lFormats);

 // --------------------------------------------------------------------------        
 // Initialize the bitmap info header structure                 
 // --------------------------------------------------------------------------        
 memset(&bmp2, 0, sizeof(BITMAPINFOHEADER2));

 // --------------------------------------------------------------------------                     
 // Populate the bitmap info header structure                                                    
 // --------------------------------------------------------------------------                     
 bmp2.cbFix     = sizeof bmp2;
 bmp2.cx        = (rclTrack.xRight - rclTrack.xLeft);
 bmp2.cy        = (rclTrack.yTop   - rclTrack.yBottom);
 bmp2.cPlanes   = lFormats[0];
 bmp2.cBitCount = lFormats[1];

 // --------------------------------------------------------------------------         
 // Create a bitmap representing the tracking rectangle                                
 // --------------------------------------------------------------------------         
 hbmTrack = GpiCreateBitmap (hpsMemory, &bmp2, 0L, NULL, NULL);

 if (hbmTrack == GPI_ERROR)
  {
   return GPI_ERROR;
  }

 else
  {
   // --------------------------------------------------------------------------        
   // Set the bitmap into our memory presentation space.                                                                      
   // --------------------------------------------------------------------------        
   GpiSetBitmap (hpsMemory, hbmTrack);
   hpsScreen = WinGetScreenPS (HWND_DESKTOP);
  
   aptl[0].x = 0;
   aptl[0].y = 0;
   aptl[1].x = bmp2.cx;
   aptl[1].y = bmp2.cy;
   aptl[2].x = rclTrack.xLeft;
   aptl[2].y = rclTrack.yBottom;
  
   WinLockVisRegions (HWND_DESKTOP, TRUE);
  
   // --------------------------------------------------------------------------                 
   // Bit blit the contents of the source presentation space into our target                     
   // presentation space.  This is how we copy the bitmap from the source to                     
   // the target.                                                                                
   // --------------------------------------------------------------------------                 
   GpiBitBlt (hpsMemory,      //  Target presentation space handle           
              hpsScreen,      //  Source presentation space handle           
              3L,             //  Number of Points                           
              aptl,           //  Array of Points                            
              ROP_SRCCOPY,    //  Mixing function (source copy)              
              BBO_IGNORE);    //  Options                                    
  
   WinLockVisRegions (HWND_DESKTOP, FALSE);
   WinReleasePS (hpsScreen);
  }

 // --------------------------------------------------------------------------                  
 // Free Graphics Engine Resources 
 // --------------------------------------------------------------------------                  
 GpiDestroyPS (hpsMemory);
 DevCloseDC (hdcMemory);

 // --------------------------------------------------------------------------                  
 // Return bitmap handle           
 // --------------------------------------------------------------------------                  
 return hbmTrack;
}

