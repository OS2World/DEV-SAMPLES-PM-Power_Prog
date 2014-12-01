// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      FILE.C                                           mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Source code to handle File menu operations                    บ
// บ                                                                               บ
// บ  NOTES:         This file contains the source code required to handle the     บ
// บ                 standard File menu operations.                                บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#define INCL_WINDIALOGS
#define INCL_WINFRAMEMGR                                          
#define INCL_WINWINDOWMGR                                         
#define INCL_WINSWITCHLIST                                        
#define INCL_WINMLE                                               
#define INCL_WINSYS
#define INCL_WINSTDFILE                                           
#define INCL_WINBUTTONS
#define INCL_DOSMEMMGR                                            
                                                                  
#include <os2.h>                                                  
#include <string.h>                                               
#include <stdio.h>                                 
#include <bsememf.h>                                              
#include "pmedit.fnc"                          
#include "pmedit.h"                                             
#include "shcommon.h"

#define CHECKED         TRUE
#define UNCHECKED       FALSE

BOOL bNewFileExists = FALSE;
BOOL QuerySearchButtons(HWND hwnd, USHORT usButtonID);


// *******************************************************************************
// FUNCTION:     FileOpen       
//
// FUNCTION USE: Routine used to open a text file and place the contents in an MLE
//
// DESCRIPTION:  This function allows the user to select the file that they want
//               displayed in the edit window via the standard file dialog.  The 
//               The file is then opened and imported into the MLE edit window.
//
// PARAMETERS:   HWND     The window handle of the MLE window.
//
// RETURNS:      VOID   
//
// INTERNALS:    DisplayMessages
//               UpdateStatusWindow
//
// HISTORY:      mm-dd-yy
//
// *******************************************************************************
APIRET FileOpen(HWND hwndMLE, PSZ pszFileName)                                                                        
{                                                                                                 
 FILEDLG    filedlg;                                                                            
 FILESTATUS filestatus;                                                                          
 HFILE      hfile;                                                                                   
 ULONG      ulAction;                                                                                
 PVOID      pvBuf;                                                                                   
 USHORT     rc;
 IPT        iptStart;     // insertion point simply a long
 CHAR       szBuffer[CCHMAXPATH * 2];


 if (pszFileName) 
  {
   strcpy(szPathAndFilename, pszFileName);  
  } 

 else
  {
   memset (&filedlg, 0, sizeof(filedlg));
   filedlg.cbSize      = sizeof(filedlg);                                                           
   filedlg.pszTitle    = "PMEDIT - Open a Text File";                                                      
   filedlg.pszOKButton = "~Open";                                                             
   filedlg.fl          = FDS_CENTER | FDS_OPEN_DIALOG;  
   filedlg.x           = 0;                                                                              
   filedlg.y           = 0;                                                                              
                                                                                                  
   // Show the user the Standard File Dialog to select a file
   rc = WinFileDlg(HWND_DESKTOP, hwndFrame, &filedlg);
    
   if (rc && filedlg.lReturn == DID_OK) 
    {
     strcpy(szPathAndFilename, filedlg.szFullFile);  
    }
  
   else
    {
     return DID_CANCEL;
    }
  }

 rc = DosOpen(szPathAndFilename, 
              &hfile,
              &ulAction,
              0,
              FILE_NORMAL,
              FILE_OPEN,
              OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE,
              NULL);

 if (rc) 
  {
   return rc;
  }

 // Get the size of the file 
 rc = DosQueryFileInfo(hfile, 
                       FIL_STANDARD,
                       &filestatus,
                       sizeof(FILESTATUS));

 if (rc)                                                   
  {                                                             
   return rc;
  }                                                             

 // Allocate enough memory to hold the size of the file
 rc = DosAllocMem(&pvBuf,                  
                  filestatus.cbFileAlloc,                    
                  fALLOC);            

 if (rc)                                                                     
  {                                                                               
   return rc;
  }                                                                               

 // Read the file into our buffer
 rc = DosRead(hfile,                   
              pvBuf,   
              filestatus.cbFileAlloc,                   
              &ulAction);

 if (rc)                               
  {                                                                                           
   return rc; 
  }                                                                                           


 // --------------------------------------------------------------------------                  
 // If everything works, clear the MLE window before we                                         
 // stick another file in there.                                                                
 // --------------------------------------------------------------------------                  
 FileNew(hwndMLE, FALSE);                                                                              

 WinSendMsg(hwndMLE,
            MLM_SETIMPORTEXPORT, 
            MPFROMP((PBYTE)pvBuf),
            MPFROMSHORT(filestatus.cbFileAlloc));

 WinSendMsg(hwndMLE,                              // MLE window handle      
            MLM_IMPORT,                           // Import
            MPFROMP(&iptStart),                   // Beginning of insertion 
            MPFROMSHORT(filestatus.cbFileAlloc)); // Number of bytes in buffer                                                  
                                                                                                  
 WinSendMsg(hwndMLE,                        
            MLM_SETCHANGED,                                                                                 
            MPFROMSHORT(FALSE),
            NULL);

  // --------------------------------------------------------------------------                             
  // Update the titlebar with the path and filename of the file. 
  // --------------------------------------------------------------------------                             
  UpdateTitleBar(hwndFrame, szPathAndFilename);

  DosFreeMem(pvBuf);                                                                          
  DosClose(hfile);                                                                            

  // If everything is kosher when we get here, then 
  // we need to return FALSE indicating success.
  return FALSE;
} // End of FileOpen
                                                                                                  

// *******************************************************************************
// FUNCTION:     FileNew       
//
// FUNCTION USE: Routine used to clear the MLE text
//
// DESCRIPTION:  This routine is used to create a new file 
//
// PARAMETERS:   HWND     The window handle of the MLE window.
//
// RETURNS:      VOID   
//
// HISTORY:      mm-dd-yy
//
// *******************************************************************************
VOID FileNew(HWND hwndMLE, BOOL bUpdateTitle)                                                                         
{                                                                                                 
 USHORT usAnswer;                                                                         
 APIRET rc;
                     
 // --------------------------------------------------------------------------                                                 
 // Before we clear all of the text in the MLE, let's check to see if any                                                                                 
 // unsaved text exists in the MLE.  If unsaved text exists, prompt the user
 // to save it.
 // --------------------------------------------------------------------------                                                 
 if (WinSendMsg(hwndMLE, MLM_QUERYCHANGED, NULL, NULL))                                   
  {                                                                                              
   usAnswer = DisplayMessages(NULLHANDLE, "Do you want to save changes?", MSG_WARNING);                                   
                                                                                                  
   // --------------------------------------------------------------------------            
   // If the user wants to save the text in the edit window, then call the       
   // FileSave routine to save the file.
   // --------------------------------------------------------------------------            
   if (usAnswer == MBID_YES)                                                         
    {
     bNewFileExists = TRUE;
     rc = FileSave(hwndMLE);                                                                       

     // --------------------------------------------------------------------------             
     // If the user cancels out of the save dialog box, then just return.                  
     // --------------------------------------------------------------------------             
     if (rc == DID_CANCEL) 
      {
       return;
      } 
    }
  }

 // --------------------------------------------------------------------------                  
 // Disable the MLE control's redraw capability, so that we do not see the 
 // text being deleted.
 // --------------------------------------------------------------------------                  
 WinSendMsg(hwndMLE, MLM_DISABLEREFRESH, NULL, NULL);                                           

 // --------------------------------------------------------------------------             
 // Delete all of the text in the MLE control.                                                    
 // --------------------------------------------------------------------------             
 WinSendMsg(hwndMLE, MLM_DELETE, (LINE)0, (MPARAM) WinSendMsg(hwndMLE, MLM_QUERYTEXTLENGTH, 0, 0));                       

 // --------------------------------------------------------------------------                       
 // Reset the changed flag.                                                       
 // --------------------------------------------------------------------------                       
 WinSendMsg(hwndMLE, MLM_SETCHANGED, MPFROMSHORT((BOOL)FALSE), NULL);                           

 // --------------------------------------------------------------------------                     
 // Enable the MLE control's redraw capability.                        
 // --------------------------------------------------------------------------                     
 WinSendMsg(hwndMLE, MLM_ENABLEREFRESH, NULL, NULL);                                            

 // --------------------------------------------------------------------------                    
 // If the bUpdateTitle flag is set, it means that the caller has asked                                                    
 // that we clear all of the update controls to indicate that there is no 
 // file in the editor.  The first thing we need to do is clear the global 
 // path and filename string, then we update the titlebar control with the 
 // default titlebar text and reset the status window to read "Untitled".
 // --------------------------------------------------------------------------                    
 if (bUpdateTitle) 
  {
   szPathAndFilename[0] = 0;                                                                             
   UpdateTitleBar(hwndFrame,   TITLEBAR); 
   UpdateStatusWindow(ID_STATIC, "Untitled");                                                                                                   
  }
 return;
}                    
                                                                                                  
// *******************************************************************************                            
// FUNCTION:     FileSave                                                                                      
//                                                                                                            
// FUNCTION USE: Routine used to save a file to disk                                                        
//                                                                                                            
// DESCRIPTION:  This is the routine responsible for saving the                                                   
//               file that is currently in the MLE window, when the 
//               user selects the Save option from the file menu.   
//               If the file in the MLE does not have a file name   
//               associated with it then we need to call the routine
//               for FileSaveAs.                                  
//                                                                                                            
// PARAMETERS:   HWND     The window handle of the MLE window.                                                
//                                                                                                            
// RETURNS:      VOID                                                                                         
//                                                                                                            
// HISTORY:      mm-dd-yy                                                                                     
//                                                                                                            
// *******************************************************************************                            
APIRET FileSave(HWND hwndMLE)                                                                 
{                                                                                          
   HFILE  hfile;                                                                            
   ULONG  ulAction;                                                                         
   USHORT rc;

   ULONG ulWrite;          
   PVOID pvBuf;            
   ULONG ulFileLen;        
   ULONG ulOffset;         
   ULONG cbExport;         
   CHAR  szBuffer[CCHMAXPATH];
  

   // --------------------------------------------------------------------------             
   // The szPathAndFilename string contains the file to be saved.  If we do 
   // not have a valid path and filename, we will call the FileSaveAs routine
   // to bring up the standard file dialog box so that the user can provide 
   // a filename.
   // --------------------------------------------------------------------------             
   if ((szPathAndFilename[0] == 0) || (bNewFileExists))                                                                
   {                                                                                       
    rc = FileSaveAs(hwndMLE);                                                                     

    if (rc == DID_CANCEL) 
     {
      return DID_CANCEL;
     }

    else
     {
      return FALSE;                                                                              
     }
   }                                                                                       
                                                                                           
   else
    {
     // --------------------------------------------------------------------------           
     // Open the file so that we can save the changes.
     // --------------------------------------------------------------------------           
     rc = DosOpen(szPathAndFilename,                  
                  &hfile,
                  &ulAction,
                  0,
                  FILE_NORMAL,
                  FILE_OPEN | FILE_CREATE,
                  OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYNONE, 
                  NULL);

     if (rc) 
      {
       sprintf(szBuffer, "Error Saving File, DosOpen failed with rc = %d", rc);
       DisplayMessages(NULLHANDLE, szBuffer, MSG_EXCLAMATION);
       return rc;
      }                                                                                       
                                                                                           
     // --------------------------------------------------------------------------                                                                                                 
     // Get the size of the text in the edit window and allocate enough 
     // storage for the file.
     // --------------------------------------------------------------------------           
     ulFileLen = (ULONG) WinSendMsg(hwndMLE, MLM_QUERYTEXTLENGTH, NULL, NULL);       
                                                                                  
     rc = DosAllocMem((PPVOID)&pvBuf,                          
                      ulFileLen + 1,
                      fALLOC);


     if (rc) 
      {                                                                              
       sprintf(szBuffer, "Error Saving File, DosAllocMem failed with rc = %d", rc);           
       DisplayMessages(NULLHANDLE, szBuffer, MSG_EXCLAMATION);                            
       return rc;                                                                         
      }                                                                              
                                                                                  
     cbExport = ulFileLen;                                                          
     WinSendMsg(hwndMLE, MLM_SETIMPORTEXPORT, MPFROMP((PBYTE)pvBuf), MPFROMLONG(cbExport));                                                                    
                                                                                
     ulOffset = 0;                                                                  
     WinSendMsg(hwndMLE, MLM_EXPORT, MPFROMP(&ulOffset), MPFROMLONG(&cbExport));    
                                                                                  
                                                                                
     // --------------------------------------------------------------------------        
     // Write the file to Disk.                                                             
     // --------------------------------------------------------------------------        
     rc = DosWrite(hfile,                               
                   pvBuf,
                   ulFileLen,
                   &ulWrite);

     if (rc)                                                                  
      {                                                                            
       sprintf(szBuffer, "Error Saving File, DosWrite failed with rc = %d", rc);                   
       DisplayMessages(NULLHANDLE, szBuffer, MSG_EXCLAMATION);                                        
       return rc;                                                                                     
      }                                                                            


     else
      {
        // --------------------------------------------------------------------------                    
        // Put a message in the status window, indicating to the user that the                                                                        
        // file has been saved.
        // --------------------------------------------------------------------------                    
        UpdateStatusWindow(ID_STATIC, "File has been saved");              

        // --------------------------------------------------------------------------              
        // Update the titlebar with the path and filename of the file.                             
        // --------------------------------------------------------------------------              
        UpdateTitleBar(hwndFrame, szPathAndFilename);                                              

        // --------------------------------------------------------------------------                    
        // Reset the changed flag.                                                                       
        // --------------------------------------------------------------------------                    
        WinSendMsg(hwndMLE, MLM_SETCHANGED, MPFROMSHORT((BOOL)FALSE), NULL);           
                                                                                  
        // --------------------------------------------------------------------------               
        // Free the memory and close the file.                                                                  
        // --------------------------------------------------------------------------               
        DosFreeMem(pvBuf);                                                             
        DosClose(hfile);                                                                        
      }
    }             
}


// *******************************************************************************                            
// FUNCTION:     FileSaveAs                                                                                    
//                                                                                                            
// FUNCTION USE: Routine used to save a file to disk                                                        
//                                                                                                            
// DESCRIPTION:  This routine is used to save the text in the                                               
//               edit window to a file.  The user specifies the filename
//               via the use of the standard file dialog box.  The 
//               routine will call the FileSave routine to perform much of 
//               the real work associated with actually saving the file to disk.
//                                                                                                            
// PARAMETERS:   HWND     The window handle of the MLE window.                                                
//                                                                                                            
// RETURNS:      VOID                                                                                         
//                                                                                                            
// HISTORY:      mm-dd-yy                                                                                     
//                                                                                                            
// *******************************************************************************                            
APIRET FileSaveAs(HWND hwndMLE)                                                                                   
{                                                                                                              
 HFILE   hfile;                                                                                                
 ULONG   ulAction;                                                                                             
 USHORT  usAnswer;                                                                                      
 APIRET  rc;
 CHAR    szBuffer[CCHMAXPATH];   


 bNewFileExists = FALSE;

 // --------------------------------------------------------------------------              
 // Call the standard file dialog routine so that the user can specify                      
 // the path and filename for the file to be saved.
 // --------------------------------------------------------------------------              
 rc = FileDialogProc("PMEDIT - Saving a File", "~Save", DLG_SAVEAS);                                                                                       
 if (rc == FALSE) 
  {
   DisplayMessages(NULLHANDLE, "Error calling standard file dialog.", MSG_EXCLAMATION);                                                                                                           
   return FALSE;
  }

 else if (rc == DID_CANCEL)
  {
   return DID_CANCEL;
  } 
                                                                                                             
 // --------------------------------------------------------------------------               
 // We call DosOpen to see if the file currently exists in this directory.  
 // If the file that the user is trying to save already exists, then call                                                 
 // the DisplayMessages routine to ask the user whether or not they want to
 // overwrite the file.  If the answer is yes, then we need to save the 
 // the file, but if the answer is no then we need to get a totally new                                            
 // filename to save the file as, so we will call this routine again.                                                                                                                                                                                                        
 // --------------------------------------------------------------------------               
 rc = DosOpen(szPathAndFilename,                                           
              &hfile,                                             
              &ulAction,                                                                                  
              0,                                                                                          
              FILE_NORMAL,                                                                                
              FILE_OPEN | FILE_CREATE,                                                                    
              OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYNONE,                                                
              NULL);                                                                                      

 if (rc) 
  {                                                                                                         
   sprintf(szBuffer, "Error Saving File As, DosOpen failed with rc = %d", rc);           
   DisplayMessages(NULLHANDLE, szBuffer, MSG_EXCLAMATION);                            
   return rc;                                                                         
  }                                                                                                         

 else                                                                                                      
  {
   DosClose(hfile);                                                                                       
  }                                                                                                         

 if (ulAction == FILE_EXISTED)                                                                             
  {                                                                                                         
   // The file already exists.  Do you want to overwrite it???
   usAnswer = DisplayMessages (IDMSG_FILEALREADYEXISTS, NULLHANDLE, MSG_WARNING);                                   
                                                                                                               
   if (usAnswer == MBID_YES)                                                                       
    {
     sprintf(szBuffer, "Saving File As %s", szPathAndFilename);                          
     UpdateStatusWindow(ID_STATIC, szBuffer);                                            
                                                                                         
     // Call the FileSave Function with the filename to perform the save                 
     FileSave(hwndMLE);                                                                  
     return FALSE;
    }

   else
    {
     // If the user selected the no pushbutton call this routine again.                 
     FileSaveAs(hwndMLE);                                                               
    }
  }                                                                                                        

 sprintf(szBuffer, "Saving File As %s", szPathAndFilename);                                              
 UpdateStatusWindow(ID_STATIC, szBuffer);                                                

 // Call the FileSave Function with the filename to perform the save
 FileSave(hwndMLE);                                                                                             
 return;
} // End of FileSaveAs()                                                                               

// *******************************************************************************            
// FUNCTION:     FileDialogProc                                                               
//                                                                                            
// FUNCTION USE: Routine used to call the standard file dialog box                          
//                                                                                            
// DESCRIPTION:  This routine simply calls the standard file dialog box                   
//               for file manipulation.                    
//                                                                                            
// PARAMETERS:   PSZ      The text string for the dialog's title bar.                                
//               PSZ      The text string for the DID_OK pushbutton.
//               USHORT   The type of standard file dialog (open or save as)
//                                                                                            
// RETURNS:      FALSE      0 - An error occurred.                                                 
//               DID_OK     1 - Everything works
//               DID_CANCEL 2 - Everything works, but user cancelled 
//                                                                                            
// HISTORY:      mm-dd-yy                                                                     
//                                                                                            
// *******************************************************************************            
USHORT FileDialogProc(PSZ szTitleText, PSZ szButtonText, USHORT usDlgType)                                                                           
{                                                                                                
 FILEDLG filedlg;
 BOOL    rc;
 ULONG   ulFileDlgFlags = 0;

 switch (usDlgType) 
  {
   // --------------------------------------------------------------------------             
   // The caller has asked for an Open File Dialog.                                       
   // --------------------------------------------------------------------------             
   case DLG_OPEN:
        ulFileDlgFlags = FDS_CENTER | FDS_OPEN_DIALOG;                                                   
        break;
      
   // --------------------------------------------------------------------------             
   // The caller has asked for the Save As File Dialog.                                       
   // --------------------------------------------------------------------------             
   case DLG_SAVEAS:
        ulFileDlgFlags = FDS_CENTER | FDS_SAVEAS_DIALOG | FDS_ENABLEFILELB;
        break;

  }

 // --------------------------------------------------------------------------              
 // Populate the file dialog structure.                                      
 // --------------------------------------------------------------------------              
 filedlg.cbSize                = sizeof(FILEDLG);            // Size of FILEDLG structure.                           
 filedlg.fl                    = ulFileDlgFlags;             // File Dialog Flags                                          
 filedlg.ulUser                = 0;                          // User defined field.                                  
 filedlg.lReturn               = 0;                          // Return code when SFD is dismissed                   
 filedlg.lSRC                  = 0;                          // System return code.                                  
 filedlg.pszTitle              = szTitleText;                // String to display in title bar.                      
 filedlg.pszOKButton           = szButtonText;               // String to display in OK button.                   
 filedlg.pfnDlgProc            = NULL;                       // Entry point to custom dialog proc.                   
 filedlg.pszIType              = NULL;                       // Pointer to string containing EA type filter.                       
 filedlg.papszITypeList        = NULL;                       // Pointer to table of pointers that                   
                                                             //   point to null terminated type strings.                    
 filedlg.pszIDrive             = NULL;                       // Pointer to string containing initial drive.                     
 filedlg.papszIDriveList       = NULL;                       // Pointer to table of pointers that                   
                                                             //   point to null terminated drive strings.                   
 filedlg.hMod                  = (HMODULE)NULLHANDLE;        // Custom File Dialog template.                   

 strcpy(filedlg.szFullFile, szPathAndFilename);              // Initial or selected fully                    

 filedlg.papszFQFilename = NULL;                             // Pointer to table of pointers that point to                  
                                                             // NULL terminated fully qualified filename strings.                 
 filedlg.ulFQFCount            = 0;                          // Number of files selected                   
 filedlg.usDlgId               = 0;                          // Custom dialog identifier.                   
 filedlg.x                     = 0;                          // X coordinate of the dialog                   
 filedlg.y                     = 0;                          // Y coordinate of the dialog                   
 filedlg.sEAType               = 0;                          // Selected file's EA Type.                   
                                                                                                 

 // --------------------------------------------------------------------------                 
 // Show the standard file dialog box                                                                                                
 // --------------------------------------------------------------------------                 
 rc = WinFileDlg(HWND_DESKTOP, HWND_DESKTOP, &filedlg);                                

 // --------------------------------------------------------------------------          
 // WinFileDlg will return NULLHANDLE on error.
 // --------------------------------------------------------------------------          
 if (rc == NULLHANDLE) 
  {
   return FALSE;
  } 

 // --------------------------------------------------------------------------              
 // WinFileDlg returned OK.                                             
 // --------------------------------------------------------------------------              
 else if (rc == TRUE) 
  {
   // --------------------------------------------------------------------------                         
   // Check to see what button the user pressed.  If the user pressed the OK     
   // button we will copy the filename selected by the dialog into our global
   // path and filename string, so that we can update the titlebar with the 
   // filename string.
   // --------------------------------------------------------------------------                         
   if (filedlg.lReturn == DID_OK)     
    {
     // --------------------------------------------------------------------------            
     // Copy path and filename returned global string                   
     // --------------------------------------------------------------------------            
     strcpy(szPathAndFilename, filedlg.szFullFile);                      
     return DID_OK;
    }

   else if (filedlg.lReturn == DID_CANCEL)                  
    {
     return DID_CANCEL;
    }
  }
 return FALSE;
} // End of FileDialogProc
                                                                                                 
// *******************************************************************************                               
// FUNCTION:     FileSearchDlgProc                                                                          
//                                                                                                               
// FUNCTION USE: Dialog procedure used for searching                                   
//                                                                                                               
// DESCRIPTION:  This dialog procedure is used to provide a text based search
//               and retrieval facility to the user.  This simple dialog box 
//               provides two entryfields to the user, one for entering the 
//               search text and one used to enter a replacement string.
//               The search can be case sensitive and the replace option contains
//               a replace all option.  The search is done by using the 
//               MLE_SEARCHDATA structure for the MLE control(edit window).
//                                                                                                               
// PARAMETERS:   HWND     Window handle                                                                          
//               ULONG    Message to be processed                                                                
//               MP1      First message parameter mp1                                                            
//               MP2      Second message parameter mp2                                                           
//                                                                                                               
// RETURNS:      MRESULT                                                                                         
//                                                                                                               
// HISTORY:      mm-dd-yy                                                                                        
//                                                                                                               
// *******************************************************************************                               
MRESULT EXPENTRY FileSearchDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)                                        
{                                                                                                               
 MRESULT         rc;                                                                                                       
 BOOL            bCheckState;
 ULONG           ulFlags;
 MLE_SEARCHDATA  mlesrch;                                                                                         

 static CHAR szSearch[CCHMAXPATH];                                                                                              
 static CHAR szReplace[CCHMAXPATH];
                                                                                                                     
 switch (msg)                                                                                                    
  {                                                                                                          
   case WM_INITDLG:                                                                                           
        WinSendDlgItemMsg (hwnd, IDE_SEARCHTEXT, EM_SETTEXTLIMIT, MPFROM2SHORT (80, 0), NULL);    
        if (szSearch[0]) 
         {
          WinSetDlgItemText (hwnd, IDE_SEARCHTEXT, szSearch);                                    
         }

        SetTheSysMenu(hwnd, "~Close Search Facility\tAlt+F4");          
        break;                                                                                                      

   case WM_COMMAND:                                                                                           
        switch (COMMANDMSG(&msg)->cmd)                                                                        
         {                                                                                                
          case DID_FIND:                                                                                      
               WinQueryWindowText(WinWindowFromID(hwnd, IDE_SEARCHTEXT),  sizeof(szSearch), szSearch);             
                                                                                                                     
               mlesrch.cb         = sizeof(mlesrch);                                                                      
               mlesrch.pchFind    = szSearch;                                                                        
               mlesrch.pchReplace = 0;                                                                         
               mlesrch.cchFind    = 0;                                                                               
               mlesrch.cchReplace = 0;                                                                            
               mlesrch.iptStart   = -1;      // start at the position of the cursor                                                                       
               mlesrch.iptStop    = -1;      // stop at the end of the file                                                                        
                                                                                                                     

               // if the user has checked the case sensitive checkbox
               // then make sure we add the case sensitive flag...
               bCheckState = QuerySearchButtons(hwnd, IDC_CASECHECK);
               if (bCheckState == CHECKED) 
                {
                 ulFlags = MLFSEARCH_SELECTMATCH | MLFSEARCH_CASESENSITIVE;
                }

               else
                {
                 ulFlags = MLFSEARCH_SELECTMATCH;
                }

               rc = WinSendMsg (hwndMLE, MLM_SEARCH, MPFROMLONG(ulFlags), MPFROMP(&mlesrch));            

               if (!rc) // If we cannot find a match
                {
                 DisplayMessages(NULLHANDLE, "Search text not found.", MSG_EXCLAMATION);
                }
               return FALSE;

          case DID_REPLACE:                                                                                          
               WinQueryWindowText(WinWindowFromID(hwnd, IDE_SEARCHTEXT),  sizeof(szSearch), szSearch);            
               WinQueryWindowText(WinWindowFromID(hwnd, IDE_REPLACETEXT), sizeof(szReplace), szReplace);          
                                                                                                                  
               mlesrch.cb         = sizeof(mlesrch);                                                              
               mlesrch.pchFind    = szSearch;                                                                     
               mlesrch.pchReplace = szReplace;                                                                    
               mlesrch.cchFind    = strlen(szSearch);                                                             
               mlesrch.cchReplace = strlen(szReplace);                                                            
               mlesrch.iptStart   = -1;      // start at the position of the cursor                               
               mlesrch.iptStop    = -1;      // stop at the end of the file                                       
                                                                                                                  
               rc = WinSendMsg (hwndMLE, MLM_SEARCH, MPFROMLONG(MLFSEARCH_CHANGEALL), MPFROMP(&mlesrch)); 
                                                                                                                  
               if (!rc) // If we cannot replace the text                                               
                {                                                                                                 
                 DisplayMessages(NULLHANDLE, "Error Replacing Text", MSG_EXCLAMATION);                                  
                }                                                                                                 
               return FALSE;                                                                                      

          case DID_OK:                                                                                        
               WinDismissDlg (hwnd, TRUE) ;                                                                       
               break;                                                                                         
         }                                                                                                
        break ;                                                                                               
  }                                                                                                          
 return WinDefDlgProc (hwnd, msg, mp1, mp2) ;                                                                    
}                                                                                                               
                                                                                                                   
// *******************************************************************************                
// FUNCTION:     QuerySearchButtons                                                                
//                                                                                                
// FUNCTION USE: Used to determine whether a radiobutton or checkbox is checked.       
//                                                                                                
// DESCRIPTION:  This routine is used to determine whether or not a button 
//               is checked.  This routine is a simple wrapper function for the 
//               WinQueryButtonCheckstate macro defined in PMWIN.H.  The function
//               will return CHECKED or TRUE if the button is checked.
//                                                                                                
// PARAMETERS:   HWND     Window handle                                                           
//               USHORT   The identifier of the button                                                 
//                                                                                                
// RETURNS:      The Checkstate of the button.                                                 
//               ----------------------------
//               CHECKED   = TRUE                             
//               UNCHECKED = FALSE
//                                                                                                
// HISTORY:      mm-dd-yy                                                                         
//                                                                                                
// *******************************************************************************                
BOOL QuerySearchButtons(HWND hwnd, USHORT usButtonID) 
{                                                     
 USHORT usCheckState;                                 

 usCheckState = WinQueryButtonCheckstate(hwnd, usButtonID);

 switch (usCheckState)
  {
   case UNCHECKED:  // Unchecked
        return UNCHECKED;

   case CHECKED:    // Checked
        return CHECKED;
  }
} 
