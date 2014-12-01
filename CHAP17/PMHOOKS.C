// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป       
// บ  FILENAME:      PMHOOKS.C                                        mm/dd/yy     บ       
// บ                                                                               บ       
// บ  DESCRIPTION:   DLL Source File for PMHOOKS Dynamic Link Library              บ       
// บ                                                                               บ       
// บ  NOTES:         This file contains the source code for the PMHOOKS.DLL        บ       
// บ                 dynamic link library that is used by the PMSCREEN executable. บ       
// บ                                                                               บ       
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ       
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ       
// บ                                                                               บ       
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ       
// บ                                                                               บ       
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ       
#define INCL_WIN
#define INCL_DOS
#define INCL_GPI

#include <os2.h>
#include <string.h>
#include <stdio.h>
#include "shcommon.h"
#include "pmhooks.h"
#include "gdata.h"

// --------------------------------------------------------------------------                                     
// Global variables.                                                                                              
// --------------------------------------------------------------------------                                     
HMODULE  hModule;
HWND     hwndClient;
HAB      hab;

// --------------------------------------------------------------------------                                           
// Address of Hooks Functions.                                                                                         
// --------------------------------------------------------------------------                                           
PFN      pfnInputHook;  
PFN      pfnLockupHook; 
PFN      pfnFlushBuf;   

// --------------------------------------------------------------------------          
// A simple function specific to this module that is used to return                  
// the path and filename of the reboot logfile on the user's boot drive.
// --------------------------------------------------------------------------          
PSZ  SubstituteBootDrive(PSZ pszFilename);                                   

// *******************************************************************************                    
// FUNCTION:     DLLInitRoutine                                                                       
//                                                                                                    
// FUNCTION USE: DLL initialization routine                                                           
//                                                                                                    
// DESCRIPTION:  This routine is responsible for setting the up the global                            
//               anchor block and Frame Window handles that are required                              
//               by the various hook procedures.  This routine is also                                
//               responsible for obtaining the address of the various hook                            
//               procedures.                                                                          
//                                                                                                    
//               Warning, this must be the first routine called in this module!                       
//                                                                                                    
// PARAMETERS:   HWND      window handle from application                                             
//                                                                                                    
// RETURNS:      TRUE      success                                                                            
//               FALSE     failure
//                                                                                                    
// HISTORY:      mm-dd-yy                                                                             
//                                                                                                    
// *******************************************************************************                    
BOOL EXPENTRY DLLInitRoutine(HWND hwnd)                                                               
{                                                                                                     
   BOOL  bValid;                                                                                      
   APIRET  rc;                                                                                        
                                                                                                      
   hab = WinQueryAnchorBlock(hwnd);                                                                   
   hwndClient = hwnd;                                                                                 
                                                                                                      
   // --------------------------------------------------------------------------         
   // Check to ensure our window handle is valid.  If our window handle is 
   // totally bogus, then we will return FALSE, indicating that an error
   // has occurred.
   // --------------------------------------------------------------------------         
   bValid = WinIsWindow(hab, hwndClient);                                                             
                                                                                                      
   if (bValid == FALSE)                                                                               
    {                                                                                                 
     // --------------------------------------------------------------------------                  
     // Sorry dude, got an error so return FALSE. 
     // --------------------------------------------------------------------------                  
     return FALSE;
    }                                                                                                 
                                                                                                      
   // --------------------------------------------------------------------------         
   // Load the DLL that contains the hook routines and obtain addresses                 
   // of the various hook procedures.
   // --------------------------------------------------------------------------         
   rc = DosLoadModule(NULL, 0, MODULE_HOOKS, &hModule);                                                       

   if (rc) 
    {
     return FALSE;
    }

   DosQueryProcAddr(hModule, NULLHANDLE, "InputHook",    &pfnInputHook);                                 
   DosQueryProcAddr(hModule, NULLHANDLE, "LockupHook",   &pfnLockupHook);                                
   DosQueryProcAddr(hModule, NULLHANDLE, "FlushBufHook", &pfnFlushBuf);                                  
   return TRUE;

}                                                                                                     

// *******************************************************************************                         
// FUNCTION:     SetInputHook                                                                             
//                                                                                                         
// FUNCTION USE: The Wrapper routine to set the Input Hook Procedure                                      
//                                                                                                         
// DESCRIPTION:  This routine registers the Input Hook.                                                   
//                                                                                                         
// PARAMETERS:   VOID                                                                                      
//                                                                                                         
// RETURNS:      BOOL   The result of the WinSetHook                                                       
//                                                                                                         
// HISTORY:      mm-dd-yy                                                                                  
//                                                                                                         
// *******************************************************************************                         
BOOL EXPENTRY SetInputHook(VOID)
{
 BOOL rc;

 rc = WinSetHook(hab,                          // Handle Anchor Block  
                 NULLHANDLE,                   // Hook System MsgQueue 
                 HK_INPUT,                     // Hook Type            
                 pfnInputHook,                 // Hook Handler         
                 hModule);                     // DLL Needed for Hook  

 return rc;
}

// *******************************************************************************            
// FUNCTION:     ReleaseInputHook                                                            
//                                                                                            
// FUNCTION USE: The Wrapper routine to release the Input Hook                               
//                                                                                            
// DESCRIPTION:  This routine releases the Input Hook and frees the                          
//               module handle.                                                               
//                                                                                            
// PARAMETERS:   VOID                                                                         
//                                                                                            
// RETURNS:      VOID                                                                         
//                                                                                            
// HISTORY:      mm-dd-yy                                                                     
//                                                                                            
// *******************************************************************************            
BOOL EXPENTRY ReleaseInputHook(VOID)
{
  BOOL rc;

  rc = WinReleaseHook(hab,                     // Handle Anchor Block  
                      NULLHANDLE,              // Hook System MsgQueue 
                      HK_INPUT,                // Hook Type            
                      pfnInputHook,            // Hook Handler         
                      hModule);                // DLL Needed for Hook  
   
  // --------------------------------------------------------------------------         
  // If the input hook can be released, then we need
  // to re-enable the default Print Screen functionality
  // and unload the module.
  // --------------------------------------------------------------------------         
  if (rc) 
   {
    // --------------------------------------------------------------------------          
    // Enable the default functionality of the Print Screen key.
    // --------------------------------------------------------------------------          
    DisablePrintScreenKey(FALSE);           
    DosFreeModule(hModule);
   }

  // --------------------------------------------------------------------------          
  // Return the returned value of WinReleaseHook back to the caller
  // --------------------------------------------------------------------------          
  return rc;
}

// *******************************************************************************                   
// FUNCTION:     InputHook                                                                        
//                                                                                                   
// FUNCTION USE: The Input Hook Procedure                                                     
//                                                                                                   
// DESCRIPTION:  This routine demonstrates the use of the Input Hook.                         
//               This hook is designed to filter posted messages.  We use the                   
//               input hook here to capture the WM_CHAR message that occurs as                    
//               a result of the user pressing the Print Screen key.                                 
//                                                                                                   
// PARAMETERS:   HAB      Anchor block handle                                                        
//               PQMSG    Pointer to QMSG structure containing the message.
//               ULONG    We don't use this parameter.
//                                                                                                   
// RETURNS:      BOOL                                                                                
//                                                                                                   
// HISTORY:      mm-dd-yy                                                                            
//                                                                                                   
// *******************************************************************************                   
BOOL EXPENTRY InputHook(HAB hab, PQMSG pqmsg, ULONG fs)
{
   CHAR  szEnabledState[2];
   BOOL  bValid;

   // --------------------------------------------------------------------------                 
   // Filter WM_CHAR messages looking for Print Screen key, virtual key.  Also                 
   // we need to check KC_VIRTUALKEY flag 
   // --------------------------------------------------------------------------                 
   if ((pqmsg->msg == WM_CHAR) && (CHAR3FROMMP(pqmsg->mp2) == VK_PRINTSCRN) &&                                            
      !(SHORT1FROMMP(pqmsg->mp1) & KC_KEYUP)  &&  (SHORT1FROMMP(pqmsg->mp1) & KC_VIRTUALKEY))                             
    {                                                           
     // --------------------------------------------------------------------------                                 
     // If the user has pressed the Print Screen key, Beep                                                         
     // --------------------------------------------------------------------------                                 
     DosBeep (500,500);

     // --------------------------------------------------------------------------                   
     // Query the OS2.INI file to check whether the Print Screen key is      
     // currently enabled.  
     // --------------------------------------------------------------------------                   
     PrfQueryProfileString (HINI_USER,
                            INI_APPNAME,  
                            INI_KEYNAME,
                            INI_DEFAULT,
                            (PVOID) &szEnabledState,
                            2L);

     // --------------------------------------------------------------------------                  
     // If the Print Screen Function is Enabled, then we need to disable it by                 
     // calling the DisablePrintScreenKey function.             
     // --------------------------------------------------------------------------                  
     if (*szEnabledState == '1')      
      {                              
       DisablePrintScreenKey(TRUE);  
      } 

     // --------------------------------------------------------------------------            
     // Now that we have disabled the default behavior of the Print Screen key,                                    
     // we send a message back over to our window so that we can get a bitmap 
     // of the active window and display it.
     // --------------------------------------------------------------------------            
     WinSendMsg(hwndClient, WM_UPDATEBITMAP, NULL, NULL); 
    }
   return FALSE;
}

// *******************************************************************************                       
// FUNCTION:     SetLockupHook                                                                         
//                                                                                                       
// FUNCTION USE: The Wrapper routine to set the Lockup Hook Procedure                              
//                                                                                                       
// DESCRIPTION:  This routine registers the Lockup Hook.                                           
//                                                                                                       
// PARAMETERS:   VOID                                                                                    
//                                                                                                       
// RETURNS:      BOOL   The result of the WinSetHook                                                     
//                                                                                                       
// HISTORY:      mm-dd-yy                                                                                
//                                                                                                       
// *******************************************************************************                       
BOOL EXPENTRY SetLockupHook(VOID)
{
 BOOL bReturn;

 bReturn =  WinSetHook(hab,                         // Handle Anchor Block  
                       NULLHANDLE,                  // Hook System MsgQueue 
                       HK_LOCKUP,                   // Hook Type            
                       (PFN)LockupHook,             // Hook Handler         
                       hModule);                    // DLL Needed for Hook  
 return bReturn;
}


// *******************************************************************************                 
// FUNCTION:     ReleaseLockupHook                                                               
//                                                                                                 
// FUNCTION USE: The Wrapper routine to release the Lockup Hook                  
//                                                                                                 
// DESCRIPTION:  This routine releases the Lockup Hook and frees the                         
//               module handle.                                                                    
//                                                                                                 
// PARAMETERS:   VOID                                                                              
//                                                                                                 
// RETURNS:      VOID                                                                              
//                                                                                                 
// HISTORY:      mm-dd-yy                                                                          
//                                                                                                 
// *******************************************************************************                 
VOID EXPENTRY ReleaseLockupHook(VOID)
{
   WinReleaseHook(hab,                     // Handle Anchor Block  
                  NULLHANDLE,              // Hook System MsgQueue 
                  HK_LOCKUP,               // Hook Type            
                  pfnLockupHook,           // Hook Handler         
                  hModule);                // DLL Needed for Hook  

   DosFreeModule(hModule);
}

// *******************************************************************************              
// FUNCTION:     LockupHook                                                                   
//                                                                                              
// FUNCTION USE: The Lockup Hook Procedure                                                
//                                                                                              
// DESCRIPTION:  In line with the KISS (Keep It Simple Stupid) philosophy, we             
//               are going to keep this simple.  This hook allows the developer
//               to modify what happens when the user selects the workplace shell
//               lockup option.  For the purpose of this sample program, we are 
//               going to Hide the Shell's Lockup window, and post a WM_LOCKUP 
//               message back to our application's client window.  The WM_LOCKUP 
//               message is user defined, and merely displays the Thank You  
//               dialog box.
//
// PARAMETERS:   HAB      Anchor block handle                                                   
//               HWND     Lockup Window Handle
//                                                                                              
// RETURNS:      BOOL                                                                           
//                                                                                              
// HISTORY:      mm-dd-yy                                                                       
//                                                                                              
// *******************************************************************************              
BOOL EXPENTRY LockupHook(HAB hab, HWND hwndLockup)
{
  // --------------------------------------------------------------------------                         
  // Post our lockup message to display the Thank You dialog box.                                                                         
  // --------------------------------------------------------------------------                         
  WinPostMsg(hwndClient, WM_LOCKUP, NULL, NULL);           

  // --------------------------------------------------------------------------                    
  // Hide our lockup dialog box.                                                                   
  // --------------------------------------------------------------------------                    
  WinSetWindowPos(hwndLockup,                                                                             
                  NULLHANDLE,                                                                            
                  0, 
                  0,   
                  0,   
                  0,   
                  SWP_HIDE | SWP_MOVE | SWP_SIZE);    

   WinInvalidateRect(hwndClient, NULL, FALSE); 
   return TRUE;
}

// *******************************************************************************                      
// FUNCTION:     SetFlushBufHook                                                                    
//                                                                                                      
// FUNCTION USE: The Wrapper routine to set the Flush Buffer Hook Procedure                                
//                                                                                                      
// DESCRIPTION:  This routine registers the Flush Buffer Hook.                                  
//                                                                                                      
// PARAMETERS:   VOID                                                                                   
//                                                                                                      
// RETURNS:      BOOL   The result of the WinSetHook                                                                                   
//                                                                                                      
// HISTORY:      mm-dd-yy                                                                               
//                                                                                                      
// *******************************************************************************                      
BOOL EXPENTRY SetFlushBufHook(VOID)                                                
{                                                                               
  BOOL rc;

  rc = WinSetHook(hab,                     // Handle Anchor Block               
                  NULLHANDLE,              // Hook System MsgQueue              
                  HK_FLUSHBUF,             // Hook Type                         
                  (PFN)FlushBufHook,       // Hook Handler                      
                  hModule);                // DLL Needed for Hook               

  return rc;
}                                                                               
                                                                                
// *******************************************************************************                    
// FUNCTION:     ReleaseFlushBufHook                                                                         
//                                                                                                    
// FUNCTION USE: The Wrapper routine to release the Flush Buffer Hook Procedure                                                      
//                                                                                                    
// DESCRIPTION:  This routine releases the Flush Buffer Hook and frees the             
//               module handle.                                                                       
//                                                                                                    
// PARAMETERS:   VOID                                                           
//                                                                                                    
// RETURNS:      VOID                                                                                 
//                                                                                                    
// HISTORY:      mm-dd-yy                                                                             
//                                                                                                    
// *******************************************************************************                    
VOID EXPENTRY ReleaseFlushBufHook(VOID)                                            
{                                                                               
   WinReleaseHook(hab,                     // Handle Anchor Block               
                  NULLHANDLE,              // Hook System MsgQueue              
                  HK_FLUSHBUF,             // Hook Type                         
                  pfnFlushBuf,             // Hook Handler                      
                  hModule);                // DLL Needed for Hook               
                                                                                
   DosFreeModule(hModule);                                                         
}                                                                               


// *******************************************************************************                
// FUNCTION:     FlushBufHook                                                                     
//                                                                                                
// FUNCTION USE: The Flush Buffer Hook Procedure                                                  
//                                                                                                
// DESCRIPTION:  This routine demonstrates the use of the Flush Buffer Hook.                         
//               This hook is designed to filter the Ctrl-Alt-Del hotkey sequence                  
//               to allow the developer the ability to perform a task prior to                             
//               rebooting.  This procedure will create a file in the root 
//               directory of the boot drive called REBOOT.LOG, that will 
//               contain the date and time the machine was rebooted.
//                                                                                                
// PARAMETERS:   HAB      Anchor block handle                                                     
//                                                                                                
// RETURNS:      BOOL                                                                             
//                                                                                                
// HISTORY:      mm-dd-yy                                                                         
//                                                                                                
// *******************************************************************************                
BOOL EXPENTRY FlushBufHook(HAB hab)
{
 HFILE           hfFile;                 
 ULONG           ulAction;               
 ULONG           ulWritten;              
 ULONG           ulLength;               
 CHAR            szDateTime[CCHMAXPATH];
 DATETIME        datetime;
 PSZ             pszLogFile;
 APIRET          rc;

 // --------------------------------------------------------------------------                       
 // This is our log file.  The question mark character "?" will be replaced                                           
 // by the boot drive in the SubstituteBootDrive routine, so that the file 
 // reboot.log will be placed in the boot drive.
 // --------------------------------------------------------------------------                       
 CHAR            *szLogFile[] = {"?:\\REBOOT.LOG"};                      

 // --------------------------------------------------------------------------                       
 // The days of the week are stored in the szDayName string.                                                         
 // --------------------------------------------------------------------------                       
 static CHAR     *szDayName [] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };               

 // --------------------------------------------------------------------------                       
 // This is the format string.                                                                      
 // --------------------------------------------------------------------------                       
 static CHAR     szFormat   [] = "The machine was rebooted on %s %d-%d-%d at %d:%d:%d %cM";                                          

 // --------------------------------------------------------------------------             
 // Get the current date and time.                                                        
 // --------------------------------------------------------------------------             
 DosGetDateTime (&datetime);            

 // --------------------------------------------------------------------------             
 // Format our date.                                                                       
 // --------------------------------------------------------------------------             
 datetime.year %= 100;                  

 // --------------------------------------------------------------------------             
 // Get the boot drive and return the string path and filename containing              
 // the reboot log file.
 // --------------------------------------------------------------------------             
 pszLogFile = SubstituteBootDrive(szLogFile[0]);          

 // --------------------------------------------------------------------------         
 // Create the reboot log file.                                                      
 // --------------------------------------------------------------------------         
 DosOpen (pszLogFile,        // File Name                  
          &hfFile,           // File Handle                   
          &ulAction,         // Action returned               
          0L,                // Initial FileSize              
          FILE_ARCHIVED,     // File Attributes               
          OPEN_FLAGS,        // Open Flags                    
          OPEN_MODE,         // Open Mode                     
          0L);               // Extended Attribute Info       
                                                                   
 // --------------------------------------------------------------------------                   
 // Fill in our format string with the correct date and time.                                    
 // --------------------------------------------------------------------------                   
 sprintf (szDateTime,                                    
          szFormat,                                
          szDayName [datetime.weekday],                
          datetime.month,                              
          datetime.day,                                
          datetime.year,                          
          (datetime.hours + 11) % 12 + 1,          
          datetime.minutes,     
          datetime.seconds,      
          datetime.hours / 12 ? 'P' : 'A');           

 // --------------------------------------------------------------------------               
 // Write the string to our file.                       .                                
 // --------------------------------------------------------------------------               
 DosWrite (hfFile, szDateTime, strlen(szDateTime), &ulWritten);            
 DosWrite (hfFile, "\r\n", 2, &ulWritten);                                  

 // --------------------------------------------------------------------------             
 // Close the file and alert the user that this was a success by beeping        
 // with an obnoxious tone.
 // --------------------------------------------------------------------------             
 DosClose(hfFile);                                                                           
 DosBeep(1000, 1000);

 return TRUE;
}


// *******************************************************************************                      
// FUNCTION:     PrintScreenToBitmap                                                         
//                                                                                                      
// FUNCTION USE: Capture the active window and returns a bitmap handle
//                                                                                                      
// DESCRIPTION:  This routine is the little piece of magic that is used to                              
//               replace the print screen functionality.  The function is 
//               called within the context of the InputHook procedure.  
//
// PARAMETERS:   VOID     Nothing required                                                                                     
//                                                                                                      
// RETURNS:      HBITMAP  Bitmap handle                                                               
//                                                                                                      
// HISTORY:      mm-dd-yy                                                                               
//                                                                                                      
// *******************************************************************************                      
HBITMAP PrintScreenToBitmap(VOID)
 {
  BITMAPINFOHEADER2 bmp;
  HBITMAP           hbmWindow;
  HDC               hdcMemory;
  HPS               hpsMemory;
  HPS               hpsScreen;
  LONG              alBitmapFormats[2];
  POINTL            aptl[3];
  SIZEL             sizl;
  HWND              hwndActive;
  RECTL             rclActive;
  APIRET            rc;

  // --------------------------------------------------------------------------              
  // Get a memory device context
  // --------------------------------------------------------------------------              
  hdcMemory = DevOpenDC (hab, OD_MEMORY, "*", 0L, NULL, NULLHANDLE);

  // --------------------------------------------------------------------------              
  // Initialize presentation space page size to 0
  // --------------------------------------------------------------------------              
  sizl.cx = 0;
  sizl.cy = 0;

  // --------------------------------------------------------------------------              
  // Get a memory presentation space
  // --------------------------------------------------------------------------              
  hpsMemory = GpiCreatePS (hab,                         // anchor block handle
                           hdcMemory,                   // device context handle
                           &sizl,                       // PS page size
                           PU_PELS    | GPIF_DEFAULT |  // Options
                           GPIT_MICRO | GPIA_ASSOC);


  // --------------------------------------------------------------------------              
  // Capture the active window, 
  // --------------------------------------------------------------------------              
  hwndActive = WinQueryActiveWindow(HWND_DESKTOP);

  // --------------------------------------------------------------------------              
  // Get a presentation space handle for the active window.
  // --------------------------------------------------------------------------              
  hpsScreen  = WinGetPS(hwndActive);

  // --------------------------------------------------------------------------              
  // Get the rectangle coordinates for the active window.
  // --------------------------------------------------------------------------              
  WinQueryWindowRect(hwndActive, &rclActive);

  rc = GpiQueryDeviceBitmapFormats (hpsMemory, 2L, alBitmapFormats);
  if (!rc) 
   {
    DisplayMessages(NULLHANDLE, "error querying device bitmap formats", MSG_INFO);
   } 


  // --------------------------------------------------------------------------              
  // Populate the bitmap info header structure
  // --------------------------------------------------------------------------              
  memset (&bmp,0, sizeof(BITMAPINFOHEADER2));               
  bmp.cbFix     = sizeof(BITMAPINFOHEADER2);               
  bmp.cPlanes   = alBitmapFormats[0];
  bmp.cBitCount = alBitmapFormats[1];
  bmp.cx        = rclActive.xRight;
  bmp.cy        = rclActive.yTop;

  // --------------------------------------------------------------------------              
  // Now, let's create our bitmap 
  // --------------------------------------------------------------------------              
  hbmWindow = GpiCreateBitmap (hpsMemory, &bmp, 0L, NULL, NULL);

  if (hbmWindow != GPI_ERROR)
   {
    // --------------------------------------------------------------------------              
    // Set our bitmap into our memory presentation space.
    // --------------------------------------------------------------------------              
    GpiSetBitmap (hpsMemory, hbmWindow);

    aptl[0].x = 0;
    aptl[0].y = 0;
    aptl[1].x = rclActive.xRight;
    aptl[1].y = rclActive.yTop;
    aptl[2].x = 0;
    aptl[2].y = 0;

    // --------------------------------------------------------------------------              
    // Bit Block Transfer the contents of our Screen presentation space to 
    // our memory presentation space.
    // --------------------------------------------------------------------------              
    WinLockVisRegions (hwndActive, TRUE);

    GpiBitBlt (hpsMemory,     //  Target presentation space handle       
               hpsScreen,     //  Source presentation space handle       
               3L,            //  Number of Points                       
               aptl,          //  Array of Points                        
               ROP_SRCCOPY,   //  Mixing function (source copy)          
               BBO_IGNORE);   //  Options                                

    WinLockVisRegions (hwndActive, FALSE);

    // --------------------------------------------------------------------------              
    // Cleanup our graphics engine resources
    // --------------------------------------------------------------------------              
    WinReleasePS (hpsScreen);
    GpiDestroyPS (hpsMemory);
    DevCloseDC (hdcMemory);

    // --------------------------------------------------------------------------              
    // If we have a valid bitmap return it
    // --------------------------------------------------------------------------              
    return hbmWindow;
   }

  else
   {
    // --------------------------------------------------------------------------              
    // If we got an error creating the bitmap return GPI_ERROR
    // --------------------------------------------------------------------------              
    DisplayMessages(NULLHANDLE, "Error GpiCreateBitmap", MSG_INFO);         
    return GPI_ERROR;
   }
}

// *******************************************************************************                   
// FUNCTION:     SubstituteBootDrive                                                                 
//                                                                                                   
// FUNCTION USE: Routine used to query the boot drive and insert it into a string.                   
//                                                                                                   
// DESCRIPTION:  This routine takes the path and filename of the reboot logfile                      
//               including the substitution character which is a question mark                     
//               and returns a complete path and filename including the boot                         
//               drive.  This routine is slightly modified from the version that              .                                                               
//               appears in the Chapter 2 sample program.
//                                                                                                   
// PARAMETERS:   PSZ   The path and filename containing the reboot logfile                            
//                     with the substitute character "?" replacing the 
//                     boot drive.  For example ?:\\REBOOT.LOG
//                                                                                                   
// RETURNS:      PSZ   The complete path and filename including the boot drive.                    
//                                                                                                   
// HISTORY:                                                                                          
//                                                                                                   
// *******************************************************************************                   
PSZ SubstituteBootDrive(PSZ pszFilename)                                                         
{                                                                                                
   static CHAR szDriveLetter = '\0';                                                             
   ULONG       ulDrive;                                                                          
   PSZ         pszCompletePath;                                                                  

   // --------------------------------------------------------------------------                               
   // Extract the boot drive letter.                                                                           
   // --------------------------------------------------------------------------                               
   if (!szDriveLetter)                                                                           
   {                                                                                             
    // --------------------------------------------------------------------------                      
    // Get the boot drive, but don't bother to check return codes, since we                        
    // are rebooting we don't really care.
    // --------------------------------------------------------------------------                      
    DosQuerySysInfo (QSV_BOOT_DRIVE,                                                       
                     QSV_BOOT_DRIVE,                                                       
                     &ulDrive,                                                             
                     sizeof(ulDrive));                                                     
                                                                                                 
    // --------------------------------------------------------------------------         
    // Convert the drive number to its equivalent drive letter.        
    // --------------------------------------------------------------------------         
    szDriveLetter = (CHAR)('A' + ulDrive - 1);                                               
   }                                                                                             
                                                                                                 
   // --------------------------------------------------------------------------           
   // If the string starts with "?:", substitute the boot drive letter!                          
   // --------------------------------------------------------------------------    
   pszCompletePath = strstr(pszFilename,"?:");                                                   

   // --------------------------------------------------------------------------             
   // Return our complete path with the boot drive.                             
   // --------------------------------------------------------------------------             
   if (pszCompletePath)                                                                          
    {                                                                                            
     *pszCompletePath = szDriveLetter;                                                          
     return pszCompletePath;                                                                    
    }                                                                                            
                                                                                                 
   else                                                                                          
    {                                                                                            
     return NULL;                                                                                
    }                                                                                            
}                                                                                                

// *******************************************************************************                      
// FUNCTION:     DisablePrintScreenKey                                                       
//                                                                                                      
// FUNCTION USE: Toggles the enable state of the Print Screen key.       
//                                                                                                      
// DESCRIPTION:  Despite its name, this function will either disable or enable                          
//               the default Print Screen functionality.  The function is 
//               called within the context of the InputHook procedure.  The 
//               function works by setting the appropriate system value to 
//               either enable or disable the Print Screen functionality.
//
// PARAMETERS:   BOOL     The caller specifies whether we should enable or                                     
//                        disable the default Print Screen functionality.
//                        If this value is TRUE, this function will disable
//                        the Print Screen key.  If this value is FALSE, 
//                        this function will enable the Print Screen key.
//                                                                                                      
// RETURNS:      BOOL     TRUE indicates that this function modified                                
//                        the PrintScreen system value.  FALSE indicates
//                        an error occurred and the PrintScreen system 
//                        value could not be modified.
//                                                                                                      
// HISTORY:      mm-dd-yy                                                                               
//                                                                                                      
// *******************************************************************************                      
BOOL DisablePrintScreenKey(BOOL bState)
{
 BOOL rc;

 // --------------------------------------------------------------------------                
 // This conditional will disable the default 
 // function of the Print Screen key.
 // --------------------------------------------------------------------------                
 if (bState)                             
  {
   rc = WinSetSysValue(HWND_DESKTOP,     // Desktop window handle
                       SV_PRINTSCREEN,   // System value identifier
                       FALSE);           // System value - FALSE disables
  }

 // --------------------------------------------------------------------------                
 // This conditional will enable the default 
 // function of the Print Screen key.
 // --------------------------------------------------------------------------                
 else
  {
   rc = WinSetSysValue(HWND_DESKTOP,     // Desktop window handle           
                       SV_PRINTSCREEN,   // System value identifier         
                       TRUE);            // System value - TRUE enables   
  }

 // --------------------------------------------------------------------------                
 // If either call to WinSetSysValue returns a TRUE, then the 
 // enable state has changed, so we will broadcast a message 
 // to tell others that the state has been toggled by this call.
 // --------------------------------------------------------------------------                
 if (rc) 
  {
   WinBroadcastMsg(HWND_DESKTOP,
                   WM_SYSVALUECHANGED,
                   (MPARAM) SV_PRINTSCREEN,
                   (MPARAM) SV_PRINTSCREEN,
                   BMSG_POST | BMSG_FRAMEONLY);

   // --------------------------------------------------------------------------                
   // If this function returns TRUE, then the enable state of the 
   // Print Screen key has been toggled by this function.
   // --------------------------------------------------------------------------                
   return TRUE;
  }

 else
  {
   // --------------------------------------------------------------------------                
   // A FALSE return value tells the caller that the enable state
   // of the Print Screen key could not be modified by this function.
   // This is highly unlikely....
   // --------------------------------------------------------------------------                
   return FALSE;
  }
}
