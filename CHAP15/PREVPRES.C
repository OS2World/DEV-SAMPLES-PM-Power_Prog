// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      PREVPRES.C                                       mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Image Display Facility Presentation Preview Window            บ
// บ                                                                               บ
// บ  NOTES:         This file contains the code used to create the Preview        บ
// บ                 Presentation Window.  The Presentation Viewer creates a       บ
// บ                 frame window that will draw thumbnail sketches of the         บ
// บ                 bitmaps in the presentation.                                  บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
 
#define INCL_WINSWITCHLIST
#define INCL_WINSYS
#define INCL_WINWINDOWMGR
#define INCL_WINMESSAGEMGR
#define INCL_WININPUT
#define INCL_WINDIALOGS
#define INCL_WINFRAMEMGR
#define INCL_WINPOINTERS
#define INCL_WINERRORS
#define INCL_WINSTDFILE
#define INCL_WINSHELLDATA
#define INCL_WINHELP
#define INCL_WINBUTTONS
#define INCL_WINMENUS
#define INCL_WINLISTBOXES
#define INCL_GPICONTROL
#define INCL_GPIPRIMITIVES
#define INCL_GPIBITMAPS
#define INCL_DOSPROCESS
#define INCL_DOSFILEMGR
#define INCL_DOSMEMMGR
#define INCL_DOSSEMAPHORES
#define INCL_DOSMODULEMGR

#include <os2.h>
#include <stdio.h>                        
#include <stdlib.h>
#include <string.h>
#include "showoff.h"
#include "structs.h"
#include "sherror.h"
#include "shcommon.h"
#include "showoff.fnc"


// --------------------------------------------------------------------------              
// Global Variables.
// --------------------------------------------------------------------------              
static BOOL bRegistered = FALSE;

// --------------------------------------------------------------------------              
// Function Prototypes specific to this file.
// --------------------------------------------------------------------------              
HBITMAP LoadBitmapForPreview(PSZ pszFilename);
MRESULT APIENTRY PreviewWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

// *******************************************************************************                      
// FUNCTION:     PreviewPresentation                                                                        
//                                                                                                      
// FUNCTION USE: Used to Create a Preview Window                                           
//                                                                                                      
// DESCRIPTION:  This function creates a new frame window called the 
//               Presentation Viewer that will display a thumbnail sized copy
//               of each bitmap in a selected Presentation.
//                                                                                                      
// PARAMETERS:   HWND     Handle of the main application window                                         
//               PSZ      Path and Filename of the Presentation File                                                 
//                                                                                                      
// RETURNS:      VOID                                                                                   
//                                                                                                      
// HISTORY:                                                                                             
//                                                                                                      
// *******************************************************************************                      
VOID PreviewPresentation(HWND hwndApp, PSZ pszPresentationFile)
{
   HAB           hab = WinQueryAnchorBlock(hwndApp);
   HWND          hwndPreviewFrame;
   HWND          hwndPreviewClient;
   HWND          hwndTitleBar;
   CHAR          szBuffer[CCHMAXPATH];
   RECTL         rectl;
   FILE          *hPresFile;
   PPRESENTATION pHeadNode;
   ULONG         ulFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU   | FCF_SIZEBORDER |
                                FCF_SHELLPOSITION | FCF_MINMAX;

   HSWITCH       hswitch;
   SWCNTRL       swctl;
   PID           pid;

   // --------------------------------------------------------------------------                          
   // Class and Titlebar Strings                                                                          
   // --------------------------------------------------------------------------                          
   PSZ     pszClassName = "WC_SHOWOFF";                                                                   

   // --------------------------------------------------------------------------                          
   // Open the Presentation File and make sure its valid.
   // --------------------------------------------------------------------------              
   if (!(hPresFile = fopen(pszPresentationFile, "r")))
    {
     return;
    }

   // --------------------------------------------------------------------------              
   // Build a list from the elements of the file
   // --------------------------------------------------------------------------              
   if ((pHeadNode = ParsePresentationFile(hPresFile))==NULL)
    {
     fclose(hPresFile);
     return;
    }

   // --------------------------------------------------------------------------              
   // Done with the file - dump it
   // --------------------------------------------------------------------------              
   fclose(hPresFile);

   // --------------------------------------------------------------------------              
   // Register the Presentation Viewer window class.
   // --------------------------------------------------------------------------              
   if(!bRegistered)
    {
     if(WinRegisterClass(hab,
                         "APREVIEW",
                         PreviewWndProc,
                         CS_SIZEREDRAW, 12 ))

     bRegistered = TRUE;
    }

   // --------------------------------------------------------------------------              
   // Create a new frame window (The Presentation Viewer)
   // --------------------------------------------------------------------------              
   if(bRegistered)
    {
     hwndPreviewFrame = WinCreateStdWindow(HWND_DESKTOP,
                                           NULLHANDLE,
                                           &ulFrameFlags,
                                           "APREVIEW",
                                           "",
                                           WS_VISIBLE,
                                           NULLHANDLE,
                                           ID_PREVIEW,
                                           &hwndPreviewClient);
    }

   // --------------------------------------------------------------------------              
   // Save the list pointers in the window data
   // --------------------------------------------------------------------------              
   WinSetWindowPtr(hwndPreviewClient, 0, pHeadNode);
   WinSetWindowPtr(hwndPreviewClient, 4, pHeadNode);

   // --------------------------------------------------------------------------              
   // Convert the Presentation File to UPPERCASE and store it in a buffer.                    
   // --------------------------------------------------------------------------              
   strupr(pszPresentationFile);                                                                   
   sprintf(szBuffer, "SHOWOFF - Previewing %s", pszPresentationFile);                                
                                                                                              
   // --------------------------------------------------------------------------              
   // Get the titlebar window handle for this dialog and store it in hwndHelp.                
   // --------------------------------------------------------------------------              
   hwndTitleBar = WinWindowFromID(hwndPreviewFrame, FID_TITLEBAR);                                        
                                                                                              
   // --------------------------------------------------------------------------              
   // Change the titlebar for this dialog to indicate that we are currently                   
   // previewing a presentation file.  The text will contain the path and 
   // filename of the presentation file.                                                                      
   // For Example:                                                                            
   //              SHOWOFF - Previewing X:\\DIRECTORY\\PRESFILE.URI                              
   // --------------------------------------------------------------------------              
   WinSetWindowText(hwndTitleBar, szBuffer);                     

   // --------------------------------------------------------------------------         
   // Add our program to the tasklist                                                    
   // --------------------------------------------------------------------------         
   WinQueryWindowProcess (hwndPreviewFrame, &pid, NULL );                                       
                                                                                         
   swctl.hwnd          = hwndPreviewFrame;                                                      
   swctl.hwndIcon      = (HWND) NULL;                                                    
   swctl.hprog         = (HPROGRAM) NULL;                                                
   swctl.idProcess     = pid;                                                            
   swctl.idSession     = (LONG) NULL;                                                    
   swctl.uchVisibility = SWL_VISIBLE;                                                    
   swctl.fbJump        = SWL_JUMPABLE;                                                   
                                                                                         
   sprintf(swctl.szSwtitle, szBuffer);                                                
   hswitch = WinCreateSwitchEntry (hab, &swctl);                                         


   // --------------------------------------------------------------------------              
   // Display the preview window and give it focus
   // --------------------------------------------------------------------------              
   WinShowWindow(hwndPreviewFrame, TRUE);
   WinSetFocus(HWND_DESKTOP, hwndPreviewClient);
}

// *******************************************************************************
// FUNCTION:     PreviewWndProc
//
// FUNCTION USE: The client window procedure for our Presentation Viewer Window
//
// DESCRIPTION:  This window procedure processes all messages received by the
//               client window of the Presentation Viewer.
//
// PARAMETERS:   HWND     client window handle
//               ULONG    window message
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  WinDefWindowProc for all unprocessed messages
//
// HISTORY:
//
// *******************************************************************************
MRESULT APIENTRY PreviewWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   HPS            hps;
   RECTL          rcl;
   RECTL          rclBmp;
   LONG           width;
   LONG           height;
   HDC            hdcMem;
   HPS            hpsMem;
   HBITMAP        hbmpMem;
   PSZ            pszTemp;
   LONG           index = 0;
   PPRESENTATION  pFirst;
   PPRESENTATION  pCurrent;

   // --------------------------------------------------------------------------              
   // On the paint message - clear the window and setup to start drawing
   // --------------------------------------------------------------------------              
   if (msg == WM_PAINT)
    {
      WinInvalidateRect(hwnd, NULL, FALSE);
      hps = WinBeginPaint(hwnd, NULLHANDLE, &rcl);

      if(hps != NULLHANDLE)
       {
        // --------------------------------------------------------------------------              
        // Clear the window and fill it with a pale gray color.
        // --------------------------------------------------------------------------              
        WinFillRect(hps, &rcl, CLR_PALEGRAY);

        // --------------------------------------------------------------------------              
        // Set current node to first node
        // --------------------------------------------------------------------------              
        pFirst = WinQueryWindowPtr(hwnd, 0);
        WinSetWindowPtr(hwnd, 4, pFirst);

        // --------------------------------------------------------------------------              
        // Start index at first bitmap position
        // --------------------------------------------------------------------------              
        WinSetWindowULong(hwnd, 8, 0);

        // --------------------------------------------------------------------------              
        // Post a message to draw a bitmap
        // --------------------------------------------------------------------------              
        WinPostMsg(hwnd, WM_USER, MPFROMLONG(0L), MPFROMLONG(0L));
        WinEndPaint(hps);
       } // endif
      return FALSE;
    }

   // --------------------------------------------------------------------------              
   // We will draw our bitmap on the WM_USER message
   // --------------------------------------------------------------------------              
   else if (msg == WM_USER)
    {
     // --------------------------------------------------------------------------              
     // Get the presentation space
     // --------------------------------------------------------------------------              
     hps = WinGetPS(hwnd);

     if (hps != NULLHANDLE)
      {
       // --------------------------------------------------------------------------              
       // The first and current node - no drawing if current is NULL
       // --------------------------------------------------------------------------              
       pFirst   = WinQueryWindowPtr(hwnd, 0);
       pCurrent = WinQueryWindowPtr(hwnd, 4);

       if (pCurrent)
        {
         // --------------------------------------------------------------------------              
         // Ensure that we have a valid bitmap filename which won't start with a 
         // slash or an asterisk.
         // --------------------------------------------------------------------------              
         if (pCurrent && pCurrent->pszBitmapPath[0] != '/')
          {
           if (pCurrent->pszBitmapPath[0] == '*')
            {
             return FALSE;
            }

           // --------------------------------------------------------------------------              
           // Get the position to draw the bitmap and compute the rectangle for drawing
           // --------------------------------------------------------------------------              
           index = WinQueryWindowULong(hwnd, 8);

           // --------------------------------------------------------------------------              
           // Get the current window rectangle.
           // --------------------------------------------------------------------------              
           WinQueryWindowRect(hwnd, &rcl);

           width          = (rcl.xRight - rcl.xLeft) / 4;
           height         = (rcl.yTop - rcl.yBottom) / 4;
           rclBmp.xLeft   = rcl.xLeft + (width * (index % 4));
           rclBmp.xRight  = rclBmp.xLeft + width;
           rclBmp.yTop    = rcl.yTop - (height * (index / 4));
           rclBmp.yBottom = rclBmp.yTop - height;

           // --------------------------------------------------------------------------              
           // Blow off any options at the end of the filename
           // --------------------------------------------------------------------------              
           pszTemp = strchr(pCurrent->pszBitmapPath, ' ' );

           if (pszTemp)
            {
             *pszTemp = '\0';
            }

           // --------------------------------------------------------------------------              
           // Get the bitmap into memory
           // --------------------------------------------------------------------------              
           hbmpMem = LoadBitmapForPreview(pCurrent->pszBitmapPath);

           if (hbmpMem) 
            {
             // --------------------------------------------------------------------------              
             // Draw the bitmap
             // --------------------------------------------------------------------------              
             WinDrawBitmap(hps,
                           hbmpMem,
                           NULL,
                           (PPOINTL)&rclBmp,
                           CLR_BLACK,
                           CLR_WHITE,
                           DBM_STRETCH | DBM_NORMAL );

             // --------------------------------------------------------------------------              
             // Draw a Border around each bitmap in the Presentation.
             // --------------------------------------------------------------------------              
             WinDrawBorder(hps, 
                           &rclBmp,
                           3, 
                           3, 
                           CLR_DARKRED,
                           CLR_DARKRED,
                           DB_PATCOPY);
            }

           else
            {
             DisplayMessages(NULLHANDLE, "no bitmap", MSG_INFO);
            }

           // --------------------------------------------------------------------------              
           // Increment the position index
           // --------------------------------------------------------------------------              
           index++;
          } // endif


         // --------------------------------------------------------------------------              
         // update for the next node
         // --------------------------------------------------------------------------              
         pCurrent = pCurrent->pNext;
         WinSetWindowPtr(hwnd, 4, pCurrent);
         WinSetWindowULong(hwnd, 8, index);

         // --------------------------------------------------------------------------              
         // draw next - unless all drawn
         // --------------------------------------------------------------------------              
         if (pCurrent != pFirst)
          {
           WinPostMsg(hwnd, WM_USER, MPFROMLONG(0L), MPFROMLONG(0L));
          }
        } // endif pCurrent

       // --------------------------------------------------------------------------              
       // Free our PS after we are done drawing.
       // --------------------------------------------------------------------------              
       WinReleasePS(hps);
      } // if (hps != NULLHANDLE)
     return FALSE;
    }

   // --------------------------------------------------------------------------              
   // Process the WM_CLOSE message
   // --------------------------------------------------------------------------              
   else if (msg == WM_CLOSE)
    {
     WinDestroyWindow(WinQueryWindow(hwnd, QW_PARENT));
     return FALSE;
    }

   // --------------------------------------------------------------------------              
   // Clear the bitmap list when window destroyed
   // --------------------------------------------------------------------------              
   else if (msg == WM_DESTROY)
    {
     WinSetWindowPtr(hwnd, 0, NULL);
     WinSetWindowPtr(hwnd, 4, NULL);
     FreePresentationMemory(WinQueryWindowPtr(hwnd, 0 ));
     return FALSE;
    }

   // --------------------------------------------------------------------------              
   // Fall down into the default window procedure for all other messages.
   // --------------------------------------------------------------------------              
   else
    {
     return WinDefWindowProc(hwnd, msg, mp1, mp2);
    } // end else
}

// *******************************************************************************               
// FUNCTION:     LoadBitmapForPreview                                                                      
//                                                                                               
// FUNCTION USE: Loads a Bitmap for Display                                                      
//                                                                                               
// DESCRIPTION:  This routine will load a bitmap from a bitmap file                              
//                                                                                               
// PARAMETERS:   PSZ        the file of the bitmap to display                                    
//                                                                                               
// RETURNS:      FALSE      Success                                                              
//               OTHERWISE  Error code defined in SHERROR.H                                      
//                                                                                               
//                                                                                               
// HISTORY:      mm/dd/yy Created this routine                                                   
//                                                                                               
// *******************************************************************************               
HBITMAP LoadBitmapForPreview(PSZ pszFilename)
{
  PBITMAPFILEHEADER   pbmpfilehdr;
  PBITMAPFILEHEADER2  pbmpfilehdr2;
  BITMAPINFOHEADER2   bmp2;
  APIRET              rc;
  ULONG               ulAction;
  PCH                 pvBuffer;
  SIZEL               sizel;
  HFILE               hBitmapFile;
  FILESTATUS3         filestatus3;
  HAB                 habRet;

  ULONG               ulOffset    = 14;
  PSZ                 pszDevData[9] = {0, "MEMORY", 0, 0, 0, 0, 0, 0, 0};

  HDC                 hdcMem;
  HPS                 hpsMem;
  HBITMAP             hbmMem;
  BOOL                b2X_Format;


  // --------------------------------------------------------------------------              
  // Get our anchor block handle.
  // --------------------------------------------------------------------------              
  habRet = WinQueryAnchorBlock(hwndFrame);
  WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP, SPTR_WAIT, FALSE));

  sizel.cx = 0;
  sizel.cy = 0;


  // --------------------------------------------------------------------------              
  // Open the bitmap file.
  // --------------------------------------------------------------------------              
  rc =  DosOpen(pszFilename,                // Valid Path and Filename
                &hBitmapFile,               // File handle
                &ulAction,                  // Action taken Returned 1=Existed 2=Created
                0,                          // Initial File Size
                0,                          // File Attributes
                OPEN_ACTION_OPEN_IF_EXISTS, // Just open (fail if NOEXIST)
                OPENMODE_FLAGS,             // File open mode flags
                0);                         // Extended Attributes


  if (rc)
   {
    return ERROR_OPENING_FILE;
   }


  // --------------------------------------------------------------------------              
  // Get the File Information.
  // --------------------------------------------------------------------------              
  rc = DosQueryFileInfo(hBitmapFile,          // File Handle
                        1,                    // Level Information
                        (PBYTE)&filestatus3,  // File Information Buffer
                        sizeof(filestatus3)); // File Information Buffer Size

  if (rc)
   {
    DosClose(hBitmapFile);
    return ERROR_OPENING_FILE;
   }


  // --------------------------------------------------------------------------              
  // Allocate memory for the bitmap.
  // --------------------------------------------------------------------------              
  rc = DosAllocMem((PVOID)&pvBuffer,
                   filestatus3.cbFile,
                   PAG_READ | PAG_WRITE | PAG_COMMIT);

  if (rc)
   {
    DosClose(hBitmapFile);
    return ERROR_ALLOC_MEM;
   }

  // --------------------------------------------------------------------------              
  // Read the bitmap into our allocated buffer.
  // --------------------------------------------------------------------------              
  rc = DosRead(hBitmapFile,
               (PVOID)pvBuffer,
               filestatus3.cbFile,
               &ulAction);

  if (rc)
   {
    DosClose(hBitmapFile);
    return ERROR_READING_FILE;
   }


  pbmpfilehdr = (PBITMAPFILEHEADER)pvBuffer;
  if (pbmpfilehdr->usType == BFT_BITMAPARRAY)
   {
    memmove(pbmpfilehdr, ulOffset + (PSZ)pbmpfilehdr, filestatus3.cbFile - ulOffset);
   }


  if ((pbmpfilehdr->usType !=  BFT_BMAP) && (pbmpfilehdr->usType !=  BFT_ICON) && (pbmpfilehdr->usType !=  BFT_POINTER))
   {
    DosClose(hBitmapFile);
    return ERROR_INVALID_BITMAP;
   }

  else
   {
    // --------------------------------------------------------------------------              
    // Get a memory device context
    // --------------------------------------------------------------------------              
    hdcMem  = DevOpenDC(habRet, OD_MEMORY, "*", 8L, (PDEVOPENDATA)pszDevData, 0);

    // --------------------------------------------------------------------------              
    // Get a normal PS and associate it
    // --------------------------------------------------------------------------              
    hpsMem  = GpiCreatePS(habRet,                                     // Anchor block handle
                          hdcMem,                                     // Memory Device Context
                          &sizel,                                     // PS page size
                          PU_PELS | GPIT_NORMAL | GPIA_ASSOC);        // Options

    // --------------------------------------------------------------------------              
    //  Is it a pre- 2.0 format bitmap?                                      
    // --------------------------------------------------------------------------              
    if (pbmpfilehdr->bmp.cbFix == sizeof (BITMAPINFOHEADER))     
     {                                                                        
      b2X_Format = FALSE;                                               
     }                                                                        

    else                                                                     
     {                                                                        
      pbmpfilehdr2 = (PBITMAPFILEHEADER2) pbmpfilehdr;                                    
      b2X_Format = TRUE;                                                
     }                                                                        
                                                                             
    // --------------------------------------------------------------------------              
    // Use the proper bitmap information                                    
    // --------------------------------------------------------------------------              
    if (b2X_Format)                                                      
     {                                                                        
       hbmMem = GpiCreateBitmap (hpsMem,                                
                                 &pbmpfilehdr2->bmp2,                              
                                 CBM_INIT,                                  
                                 pvBuffer + pbmpfilehdr2->offBits,                   
                                 (PBITMAPINFO2) &pbmpfilehdr2->bmp2);              
     }                                                                        

    else                                                                     
     {                                                                        
       hbmMem = GpiCreateBitmap (hpsMem,                                
                                 (PBITMAPINFOHEADER2) &pbmpfilehdr->bmp,           
                                 CBM_INIT,                                  
                                 pvBuffer + pbmpfilehdr->offBits,                    
                                 (PBITMAPINFO2) &pbmpfilehdr->bmp);                
     }                                                                        

    if (hdcMem && hpsMem && hbmMem)
     {
      GpiSetBitmap(hpsMem, hbmMem);

      memset(&bmp2, 0, sizeof(BITMAPINFOHEADER2));
      bmp2.cbFix = sizeof(BITMAPINFOHEADER2);
     }

    else
     {
      DisplayMessages(NULLHANDLE, "Error Creating Bitmap", MSG_INFO);
     }

   }


  // --------------------------------------------------------------------------              
  // Cleanup our Resources.
  // --------------------------------------------------------------------------              
  DosFreeMem(pvBuffer);
  DosClose(hBitmapFile);

  GpiDestroyPS(hpsMem);        
  DevCloseDC(hdcMem);

  // --------------------------------------------------------------------------              
  // Reset the Pointer.
  // --------------------------------------------------------------------------              
  WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, FALSE));
  return hbmMem;
}
