// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป     
// บ  FILENAME:      SHDSKBMP.C                                       mm/dd/yy     บ     
// บ                                                                               บ     
// บ  DESCRIPTION:   Code for changing the workplace desktop background bitmap     บ     
// บ                                                                               บ     
// บ  NOTES:         This file contains the code for WinChangeDesktopBitmap        บ     
// บ                 which is used to change the desktop background bitmap.        บ     
// บ                                                                               บ     
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ     
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ     
// บ                                                                               บ     
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ     
// บ                                                                               บ     
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ     

#define INCL_WPCLASS           
#define INCL_WINWINDOWMGR      
#define INCL_WINMESSAGEMGR     
#define INCL_WINDIALOGS        
#define INCL_WINPOINTERS       
#define INCL_WINSTDFILE        
#define INCL_WINSHELLDATA      
#define INCL_WINHELP           
#define INCL_DOSPROCESS        
#define INCL_DOSSESMGR         

#include <os2.h>                        
#include <stdlib.h>                     
#include <stdio.h>                      
#include <string.h>                     
#include "showoff.h"                    
#include "showoff.fnc"                  

// --------------------------------------------------------------------------                         
// Thread Function.
// --------------------------------------------------------------------------                         
static VOID EXPENTRY DesktopBackgroundThread(PUCHAR szDesktopBuffer);                                         

// *******************************************************************************
// FUNCTION:     WinChangeDesktopBitmap
//
// FUNCTION USE: API to change the workplace shell's desktop background bitmap
//
// DESCRIPTION:  This function is used to change the desktop bitmap.  The user 
//               can either pass a valid bitmap file to this function or if NULL
//               is passed, the function will call the standard file dialog to 
//               prompt the user for a bitmap file.  This function works by 
//               creating a setup string for WinCreateObject that looks like this
//               "OBJECTID=<WP_DESKTOP>;BACKGROUND=X:\\XXX\\XXX\\BITMAP.BMP"     
//                 
// PARAMETERS:   HWND     The Frame Window Handle
//               PUCHAR   The Path and Filename of a bitmap file 
//
// RETURNS:      VOID
//
// HISTORY:
//
// *******************************************************************************
BOOL WinChangeDesktopBitmap(HWND hwndFrame, PUCHAR szBitmap)
{
 static UCHAR szDesktopBuffer[CCHMAXPATH];     
 UCHAR        szPathAndFilename[160];
 FILEDLG      filedlg;
 APIRET       rc;
 TID          tid;


 if (szBitmap) 
  {
   strcpy (szPathAndFilename, szBitmap);                             
   strcpy (szDesktopBuffer, "\"OBJECTID=<WP_DESKTOP>;BACKGROUND=");            
   strncat(szDesktopBuffer, szPathAndFilename, sizeof(szPathAndFilename));     
  } 

 else
  {
   memset(&filedlg, 0, sizeof(filedlg));
   filedlg.cbSize          = sizeof(filedlg);
   filedlg.fl              = FDS_CENTER | FDS_OPEN_DIALOG;
   filedlg.pszTitle        = "Change Desktop Background Bitmap";

   strcpy(filedlg.szFullFile,"*.BMP");  // File Filter = BMP Files

   if (WinFileDlg(HWND_DESKTOP, hwndFrame, &filedlg) && filedlg.lReturn == DID_OK)
    {
     strcpy (szPathAndFilename, filedlg.szFullFile);
     strcpy (szDesktopBuffer, "\"OBJECTID=<WP_DESKTOP>;BACKGROUND=");
     strncat(szDesktopBuffer, szPathAndFilename, sizeof(szPathAndFilename));
    }

   // --------------------------------------------------------------------------                         
   // We need to handle the case where the user bails out of the standard
   // file dialog without selecting a bitmap file.  In this case, we simply
   // return FALSE, since no error condition actually occurred, the user 
   // simply changed their minds.... 
   // --------------------------------------------------------------------------                         
   else
    {
     return FALSE;
    }
  }

 // --------------------------------------------------------------------------                         
 // Dispatch our Desktop Background Thread to change the background.
 // --------------------------------------------------------------------------                         
 rc = DosCreateThread(&tid,
                      (PFNTHREAD)DesktopBackgroundThread,
                      (ULONG)szDesktopBuffer,
                      0,
                      8192);

 if (rc != NULLHANDLE)
  {                                                                              
   // --------------------------------------------------------------------------                         
   // If we got an error dispatching the worker thread, then we return 
   // return TRUE back to the caller to indicate that the thread did not 
   // get kicked off successfully.
   // --------------------------------------------------------------------------                         
   return TRUE;
  }                                                                              

// --------------------------------------------------------------------------                         
// Otherwise, if everything is fine and dandy we just return 
// FALSE for success. 
// --------------------------------------------------------------------------                         
return FALSE;
}

// *******************************************************************************
// FUNCTION:     DesktopBackgroundWorkerThread
//
// FUNCTION USE: Worker Thread for WinChangeDesktopBitmap
//
// DESCRIPTION:  This routine takes a single setup string as a parameter and  
//               uses it to call WinCreateObject to change the desktop background
//               bitmap.
//
// PARAMETERS:   PUCHAR        The Setup String
//
// RETURNS:      VOID
//
// HISTORY:
//
// *******************************************************************************
static VOID EXPENTRY DesktopBackgroundThread(PUCHAR szDesktopBuffer)                                         
{                                                                                           
  HAB   habThread;                                                                          
  HMQ   hmqThread;                                                                          
                                                                                            
  habThread = WinInitialize(0);                                                             
  hmqThread = WinCreateMsgQueue(habThread, 0);                                              
                                                                                            
  WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP, SPTR_WAIT, FALSE));          

  WinCreateObject("WPFolder",             // Workplace Object Class                                                                                         
                  "Change Desktop",       // Object Title
                  szDesktopBuffer,        // Setup String
                  "<WP_DESKTOP>",         // Location 
                  CO_UPDATEIFEXISTS);     // Flags

  WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, FALSE));         

  return;                                                                                   
}                                                                                            
