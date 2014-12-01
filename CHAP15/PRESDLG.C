// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป     
// บ  FILENAME:      PRESDLG.C                                        mm/dd/yy     บ     
// บ                                                                               บ     
// บ  DESCRIPTION:   Presentation File Routines                                    บ     
// บ                                                                               บ     
// บ  NOTES:         This file contains routines that create and modify the        บ     
// บ                 presentation file.  SHOWOFF Presentation files end in the     บ     
// บ                 extention .URI and are flat ASCII Files with tag format       บ     
// บ                 instructions.  This file contains routines to create and edit บ     
// บ                 a particular Presentation File.                               บ     
// บ                                                                               บ     
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ     
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ     
// บ                                                                               บ     
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ     
// บ                                                                               บ     
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ     
 
#define INCL_WINFRAMEMGR             
#define INCL_WINWINDOWMGR            
#define INCL_WINMESSAGEMGR           
#define INCL_WININPUT                
#define INCL_WINDIALOGS              
#define INCL_WINHELP
#define INCL_WINBUTTONS
#define INCL_WINENTRYFIELDS  
#define INCL_WINLISTBOXES
#define INCL_WINSTDFILE
#define INCL_WINSTDSPIN    
#define INCL_DOSFILEMGR              

#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <process.h>
#include "messages.h"
#include "shcommon.h"
#include "showoff.h"
#include "showoff.fnc"

// --------------------------------------------------------------------------    
// Function Prototypes used in this file
// --------------------------------------------------------------------------    
BOOL SavePresentationToDisk(HWND hwndListBox, PSZ pszPresentation, PSZ pszDefTime);
BOOL LoadPresentation(HWND hwndListBox, FILE *hFile);
BOOL DetermineMode (HWND hwnd, MPARAM mp2);

// *******************************************************************************
// FUNCTION:     CreatePresentationDlgProc
//
// FUNCTION USE: Dialog Procedure used to create a presentation
//
// DESCRIPTION:  This dialog procedure is used to process the create
//               presentation dialog box.
//
// PARAMETERS:   HWND     client window handle
//               ULONG    window message
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT
//
// HISTORY:
//
// *******************************************************************************
MRESULT EXPENTRY CreatePresentationDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 static  HWND    hwndTemp;
 static  HWND    hwndListBox;
 static  HWND    hwndBitmapPath;
 static  HWND    hwndSpinDefTime;
 static  HWND    hwndSpinBmpTime;
 static  PSZ     pszPresFilename;
 static  BOOL    bEditMode;
 static  USHORT  usSelectedItem;

 PVOID   pvBuffer;
 CHAR    szBuffer[CCHMAXPATH];
 CHAR    szPresentation[CCHMAXPATH];
 USHORT  usDefaultTime;
 USHORT  usBitmapTime;
 FILEDLG filedlg;


 switch (msg)
  {
   case WM_INITDLG:
        // --------------------------------------------------------------------------
        // Center the Dialog Window.
        // --------------------------------------------------------------------------
        CenterDialog(hwnd);

        // --------------------------------------------------------------------------
        // Store the handles for the Presentation Listbox Window and the
        // Bitmap Path and Filename Entryfield Window.
        // --------------------------------------------------------------------------
        hwndListBox    = WinWindowFromID(hwnd, IDL_PRESENTATION);
        hwndBitmapPath = WinWindowFromID(hwnd, IDE_FILENAME);

        pszPresFilename = (PSZ)PVOIDFROMMP(mp2);      

        // --------------------------------------------------------------------------
        // We use a spinbutton to control the default time used to display a bitmap.
        // The upper limit of the default time is 60 sec, while the lower limit is
        // 1 sec.  Also, set the default value to 5 seconds and limit the text limit
        // of the spinbutton to 2 characters.
        // --------------------------------------------------------------------------
        hwndSpinDefTime = WinWindowFromID(hwnd, IDE_DEFTIME);
        WinSendMsg(hwndSpinDefTime, SPBM_SETLIMITS,       (MPARAM)60, (MPARAM)0);
        WinSendMsg(hwndSpinDefTime, SPBM_SETCURRENTVALUE, (MPARAM)5,  (MPARAM)NULL);
        WinSendMsg(hwndSpinDefTime, SPBM_SETTEXTLIMIT,    (MPARAM)2,  (MPARAM)NULL);

        // --------------------------------------------------------------------------
        // We also use a spinbutton to specify the time to display an individual
        // bitmap.  Note that this time value overrides the default time specified.
        // The upper limit of the default time is 60 sec, while the lower limit is
        // 1 sec.  Also, set the default value to 5 seconds and limit the text limit
        // of the spinbutton to 2 characters.
        // --------------------------------------------------------------------------
        hwndSpinBmpTime = WinWindowFromID(hwnd, IDE_BMPTIME);
        WinSendMsg(hwndSpinBmpTime, SPBM_SETLIMITS,       (MPARAM)60, (MPARAM)0);
        WinSendMsg(hwndSpinBmpTime, SPBM_SETCURRENTVALUE, (MPARAM)0,  (MPARAM)NULL);
        WinSendMsg(hwndSpinBmpTime, SPBM_SETTEXTLIMIT,    (MPARAM)2,  (MPARAM)NULL);


        // --------------------------------------------------------------------------               
        // Determine if we got here from the user selecting, the "Create                            
        // Presentation" menuitem or the "Edit Presentation" menuitem.                              
        // --------------------------------------------------------------------------               
        bEditMode = DetermineMode(hwnd, mp2);                                                       
                                                                                            
        if (bEditMode)                                                                              
         {                                                                                          
          hwndTemp = WinWindowFromID(hwnd, DID_OK);                                                 
          WinSetWindowText(hwndTemp, IDS_SAVE_BTN);                                                 

          // --------------------------------------------------------------------------               
          // If we are in edit mode remove the Bitmap Time value spinbutton, and the 
          // static text above the window.
          // --------------------------------------------------------------------------               
          hwndTemp = WinWindowFromID(hwnd, ID_DISPBMP);
          WinShowWindow(hwndTemp, FALSE); 

          hwndTemp = WinWindowFromID(hwnd, ID_BMPTOADD);
          WinSetWindowText(hwndTemp, IDS_BMPTOEDIT);                                                 

          hwndTemp = WinWindowFromID(hwnd, ID_SECONDS);
          WinShowWindow(hwndTemp, FALSE); 

          // --------------------------------------------------------------------------               
          // Remove the individual bitmap time spinbutton if we are in edit mode.
          // --------------------------------------------------------------------------               
          WinShowWindow(hwndSpinBmpTime, FALSE); 
         }                                                                                          

        else
         {
          hwndTemp = WinWindowFromID(hwnd, DID_CHANGE);
          WinShowWindow(hwndTemp, FALSE);

          hwndTemp = WinWindowFromID(hwnd, ID_INFO);
          WinShowWindow(hwndTemp, FALSE);
         }

        // --------------------------------------------------------------------------                 
        // Get the window handle for the help window out of the window words for                      
        // our frame window.                                                                          
        // --------------------------------------------------------------------------                 
        hwndTemp = WinQueryWindowULong(hwndFrame, QWL_USER);                                          

        // --------------------------------------------------------------------------                 
        // Remove the menuitems from the system menu that we don't really need.
        // --------------------------------------------------------------------------                 
        SetTheSysMenu(hwnd, NULL);
        return FALSE;

   case WM_CONTROL:
        if (SHORT1FROMMP (mp1) == IDL_PRESENTATION)
         {
          // --------------------------------------------------------------------------
          // If we are not in edit mode just return....
          // --------------------------------------------------------------------------
          if (!bEditMode) 
           {
            return FALSE;
           }

          usSelectedItem = (USHORT) WinSendDlgItemMsg(hwnd, SHORT1FROMMP(mp1), LM_QUERYSELECTION, 0L, 0L);
          WinSendDlgItemMsg(hwnd, SHORT1FROMMP(mp1), LM_QUERYITEMTEXT, MPFROM2SHORT(usSelectedItem, sizeof szBuffer), MPFROMP(szBuffer));

          switch (SHORT2FROMMP (mp1))
           {
            case LN_SELECT:
                 // --------------------------------------------------------------------------
                 // Convert the string passed in to uppercase
                 // --------------------------------------------------------------------------
                 strupr(szBuffer);
                 WinSetDlgItemText (hwnd, IDE_FILENAME, szBuffer);                             
           }
         }
        return FALSE;

   case WM_COMMAND:
        switch (COMMANDMSG(&msg)->cmd)
         {
          case DID_ADD:
               // --------------------------------------------------------------------------
               // Get the text from the Bitmap Path and Filename Entryfield.
               // --------------------------------------------------------------------------
               WinQueryDlgItemText(hwnd, IDE_FILENAME, sizeof(szBuffer), (PSZ)szBuffer);

               // --------------------------------------------------------------------------
               // If we have text in the Bitmap Path and Filename Entryfield, then copy
               // the text into the Presentation Listbox.
               // --------------------------------------------------------------------------
               if (szBuffer[0])
                {
                 // --------------------------------------------------------------------------
                 // Convert the path and filename to UPPERCASE characters.
                 // --------------------------------------------------------------------------
                 strupr(szBuffer);

                 // --------------------------------------------------------------------------
                 // Query the value in the spinbutton control.
                 // --------------------------------------------------------------------------
                 WinSendMsg(hwndSpinBmpTime, SPBM_QUERYVALUE, &pvBuffer, MPFROM2SHORT(0, 0));   
                 usBitmapTime = (USHORT)pvBuffer;                                           

                 // --------------------------------------------------------------------------
                 // If the time to display this bitmap is still set to zero which is the 
                 // default then we will only write the Bitmap Path and Filename to the 
                 // Presentation Listbox.
                 // --------------------------------------------------------------------------
                 if (usBitmapTime == 0) 
                  {
                   WinSendMsg(hwndListBox, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP(szBuffer));
                  }

                 else
                  {
                   // --------------------------------------------------------------------------
                   // If the time to display this bitmap is a non-zero value then we will 
                   // write the path and filename of the bitmap, along with a slash character
                   // followed by the time value in seconds that this bitmap should be 
                   // displayed.  For Example X:\\SHOWOFF\\DUMMY.BMP /12
                   // --------------------------------------------------------------------------
                   sprintf(szPresentation, "%s /%d", szBuffer, usBitmapTime);

                   WinSendMsg(hwndListBox, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP(szPresentation));
                  }

                 // --------------------------------------------------------------------------
                 // Clear the Bitmap path and filename entryfield.
                 // --------------------------------------------------------------------------
                 WinSetDlgItemText (hwnd, IDE_FILENAME, "");
                }

               // --------------------------------------------------------------------------
               // If there is no bitmap path and filename specified in the bitmap
               // entryfield post an error message to the user and set the input focus
               // to the entryfield.
               // --------------------------------------------------------------------------
               else
                {
                 DisplayMessages(NULLHANDLE, IDS_NOTEXT, MSG_EXCLAMATION);
                 WinSetFocus(HWND_DESKTOP, hwndBitmapPath);
                }
               return FALSE;

          case DID_REMOVE:
               // --------------------------------------------------------------------------
               // Get the selected item from the Presentation Listbox.
               // --------------------------------------------------------------------------
               usSelectedItem = WinQueryLboxSelectedItem (hwndListBox);  

               // --------------------------------------------------------------------------
               // If no item in the listbox is currently selected, post an error message
               // to the user telling him/her that they are a dummy, since they should 
               // have never clicked the remove pushbutton in the first place.
               // --------------------------------------------------------------------------
               if (usSelectedItem == LIT_NONE) 
                {
                 DisplayMessages(NULLHANDLE, IDS_NOSELECT, MSG_EXCLAMATION);
                }

               // --------------------------------------------------------------------------
               // Otherwise, simply delete the item from the listbox.
               // --------------------------------------------------------------------------
               else
                {
                 WinDeleteLboxItem(hwndListBox, usSelectedItem); 
                }
               return FALSE;

          case DID_FIND:
               memset(&filedlg, 0, sizeof(filedlg));                                                    
               filedlg.cbSize          = sizeof(filedlg);                                               
               filedlg.fl              = FDS_CENTER | FDS_OPEN_DIALOG;                 
               filedlg.pszTitle        = "SHOWOFF - Locate a Bitmap";                             
                                                                                                        
               strcpy(filedlg.szFullFile, EXT_BITMAP);                                                  
                                                                                                        
               if (WinFileDlg(HWND_DESKTOP, hwnd, &filedlg) && filedlg.lReturn == DID_OK)               
                {                                                                                       
                 // --------------------------------------------------------------------------
                 // Copy the file the user selected from the standard file dialog box into
                 // our string buffer.
                 // --------------------------------------------------------------------------
                 strcpy(szBuffer, filedlg.szFullFile);                                              

                 // --------------------------------------------------------------------------
                 // Convert the path and filename to UPPERCASE characters.
                 // --------------------------------------------------------------------------
                 strupr(szBuffer);

                 // --------------------------------------------------------------------------
                 // Put the path and filename the user selected via the Find pushbutton 
                 // into the Bitmap Path and Filename Entryfield.
                 // --------------------------------------------------------------------------
                 WinSetDlgItemText (hwnd, IDE_FILENAME, szBuffer); 
                }

               return FALSE;

          case DID_OK:
               memset(&filedlg, 0, sizeof(filedlg));                                                    
               filedlg.cbSize          = sizeof(filedlg);                                               
               filedlg.fl              = FDS_CENTER | FDS_SAVEAS_DIALOG;                 
               filedlg.pszTitle        = "SHOWOFF - Save Presentation";                             
                                                                                                        
               // --------------------------------------------------------------------------
               // If we are in edit mode then copy the whole filename to the standard
               // file dialog.
               // --------------------------------------------------------------------------
               if (bEditMode) 
                {
                 // --------------------------------------------------------------------------  
                 // Get the filename of the Presentation the user selected.                             
                 // --------------------------------------------------------------------------  
                 strupr(pszPresFilename);
                 strcpy(filedlg.szFullFile, pszPresFilename);                                                  
                }

               else
                {
                 strcpy(filedlg.szFullFile, EXT_PRESENTATION);                                                  
                }
                                                                                                        
               if (WinFileDlg(HWND_DESKTOP, hwnd, &filedlg) && filedlg.lReturn == DID_OK)               
                {                                                                                       
                 // --------------------------------------------------------------------------
                 // Query the value in the spinbutton control.
                 // --------------------------------------------------------------------------
                 WinSendMsg(hwndSpinDefTime, SPBM_QUERYVALUE, &pvBuffer, MPFROM2SHORT(0, 0));   
                 usDefaultTime = (USHORT)pvBuffer;                                           

                 // --------------------------------------------------------------------------
                 // Prepend a forward slash to the default time.
                 // --------------------------------------------------------------------------
                 sprintf(szBuffer, "/%d", usDefaultTime);

                 // --------------------------------------------------------------------------
                 // Save the presentation to disk.
                 // --------------------------------------------------------------------------
                 SavePresentationToDisk(hwndListBox, filedlg.szFullFile, szBuffer);
                }
               return FALSE;

          case DID_CHANGE:
               // --------------------------------------------------------------------------       
               // Get the text from the Bitmap Path and Filename Entryfield.                       
               // --------------------------------------------------------------------------       
               WinQueryDlgItemText(hwnd, IDE_FILENAME, sizeof(szBuffer), (PSZ)szBuffer);           
                                                                                                   
               // --------------------------------------------------------------------------       
               // If we have text in the Bitmap Path and Filename Entryfield, and the user         
               // clicks on the Change pushbutton, then we change that item.                       
               // --------------------------------------------------------------------------       
               if (szBuffer[0])                                                                    
                {                                                                                  
                 strupr(szBuffer);
                 WinSetLboxItemText(hwndListBox, usSelectedItem, szBuffer);
                }
               return FALSE;

          case DID_CANCEL:
               WinDismissDlg(hwnd, TRUE);
               return FALSE;

          case DID_HELP:
               if (hwndTemp) 
                {
                 // --------------------------------------------------------------------------       
                 // If we are in create mode then display the help panel for creating a 
                 // Presentation File.
                 // --------------------------------------------------------------------------       
                 if (!bEditMode) 
                  {
                   WinSendMsg(hwndTemp, HM_DISPLAY_HELP, MPFROMSHORT(PANEL_CREATE_PRES), 0L);                           
                  }

                 // --------------------------------------------------------------------------       
                 // If we are in edit mode then display the help panel for editing a 
                 // Presentation File.
                 // --------------------------------------------------------------------------       
                 else
                  {
                   WinSendMsg(hwndTemp, HM_DISPLAY_HELP, MPFROMSHORT(PANEL_EDIT_PRES), 0L);                          
                  }
                }
               return FALSE;
         }
        break;
  }
 return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

// *******************************************************************************
// FUNCTION:     SavePresentationToDisk 
//
// FUNCTION USE: Writes the data from the create/edit Presentation Dialog to 
//               a Presentation File on disk.
//
// DESCRIPTION:  This function will create a Presentation File based on the 
//               data the user entered in the various controls in the 
//               Create/Edit Presentation Dialog.
//
// PARAMETERS:   HWND     The window handle of the Listbox control containing 
//                        the bitmaps in the Presentation.
//               PSZ      The path and filename of the Presentation File to 
//                        to save to disk.        
//               PSZ      The default time to display each bitmap.
//
// RETURNS:      FALSE  
//
// HISTORY:
//
// *******************************************************************************
BOOL SavePresentationToDisk(HWND hwndListBox, PSZ pszPresentation, PSZ pszDefTime)
{
 HFILE   hfFile;
 ULONG   ulAction;
 ULONG   ulWritten;

 USHORT  usNumberOfItems;
 USHORT  usCounter;
 USHORT  usCheckState;

 ULONG   ulMaxChar;
 CHAR    szBuffer[CCHMAXPATH];
 HWND    hwndParent;


 // --------------------------------------------------------------------------
 // Get the window handle of the dialog box.
 // --------------------------------------------------------------------------
 hwndParent = WinQueryWindow(hwndListBox, QW_PARENT);

 // --------------------------------------------------------------------------
 // Open the Presentation File to be written to disk.
 // --------------------------------------------------------------------------
 DosOpen (pszPresentation,   // Profile Name
          &hfFile,           // File Handle
          &ulAction,         // Action returned
          0L,                // Initial FileSize
          FILE_ARCHIVED,     // File Attributes
          OPEN_FLAGS,        // Open Flags
          OPEN_MODE,         // Open Mode
          0L);               // Extended Attribute Info

 // --------------------------------------------------------------------------
 // Write our banner in the beginning of each Presentation File.
 // --------------------------------------------------------------------------
 DosWrite(hfFile, TEXT_HEADER, strlen(TEXT_HEADER), &ulWritten);
 DosWrite (hfFile, "\r\n", 2, &ulWritten);

 // --------------------------------------------------------------------------
 // Write our default time     
 // --------------------------------------------------------------------------
 DosWrite (hfFile, pszDefTime, strlen(pszDefTime), &ulWritten);           
 DosWrite (hfFile, "\r\n", 2, &ulWritten);

 // --------------------------------------------------------------------------
 // Check to see if the option to run this presentation in fullscreen mode
 // is selected.  If this button is checked, then we need to write a 
 // slash followed by a Dollar sign to the Presentation File.
 // --------------------------------------------------------------------------
 usCheckState = WinQueryButtonCheckstate(hwndParent, IDC_FULLSCR);      
                                                                        
 if (usCheckState == CHECKED)                                           
  {                                                                     
   DosWrite (hfFile, "/$", 2, &ulWritten);                              
   DosWrite (hfFile, "\r\n", 2, &ulWritten);
  }                                                                     

 // --------------------------------------------------------------------------
 // Obtain the total number of items in the Presentation Listbox.
 // --------------------------------------------------------------------------
 usNumberOfItems = WinQueryLboxCount(hwndListBox);

 for (usCounter = 0; usCounter < usNumberOfItems; usCounter++)           
  {
   // --------------------------------------------------------------------------
   // Query the length of each item string in the listbox.
   // --------------------------------------------------------------------------
   ulMaxChar = WinQueryLboxItemTextLength(hwndListBox, usCounter);                                        

   // --------------------------------------------------------------------------
   // Now query the actually text string.
   // --------------------------------------------------------------------------
   WinQueryLboxItemText(hwndListBox,      // Listbox handle
                        usCounter,        // Item Index
                        szBuffer,         // Store the text string buffer
                        ulMaxChar + 1);   // Account for NULL character

   // --------------------------------------------------------------------------
   // Write the string to our Presentation File.
   // --------------------------------------------------------------------------
   DosWrite(hfFile, szBuffer, strlen(szBuffer), &ulWritten);
   DosWrite (hfFile, "\r\n", 2, &ulWritten);
  }

 usCheckState = WinQueryButtonCheckstate(hwndParent, IDC_RESTART);                               

 if (usCheckState == UNCHECKED) 
  {                                                                                       
   DosWrite (hfFile, "*", 1, &ulWritten);                                    
  }                                                                                       

 // --------------------------------------------------------------------------
 // Close the Presentation File and Return.
 // --------------------------------------------------------------------------
 DosClose(hfFile);
 return FALSE;
}

// *******************************************************************************
// FUNCTION:     LoadPresentation
//
// FUNCTION USE: Loads a Presentation File from disk and inserts all of the        
//               information into the proper dialog controls.
//
// DESCRIPTION:  This function will load the Presentation information from the 
//               Presentation File into the various dialog controls in the 
//               Edit Presentation Dialog Box.
//
// PARAMETERS:   HWND     The window handle of the Listbox control to insert
//                        the bitmap information.
//               FILE     The file handle of the Presentation File.
//
// RETURNS:      FALSE  
//
// HISTORY:
//
// *******************************************************************************
BOOL LoadPresentation(HWND hwndListBox, FILE *hFile)
{
  USHORT        usLineLength;
  ULONG         ulDefTime;
  CHAR          szLine[CCHMAXPATH];
  PSZ           pszString;
  HWND          hwndParent;
  HWND          hwndSpinDefTime;
  CHAR          *szTime;          

  // --------------------------------------------------------------------------
  // Get the window handle of the dialog box and the Default Time Spinbutton.
  // --------------------------------------------------------------------------
  hwndParent = WinQueryWindow(hwndListBox, QW_PARENT);
  hwndSpinDefTime = WinWindowFromID(hwndParent, IDE_DEFTIME);                                 

  // --------------------------------------------------------------------------
  // Check the restart button when we begin.  If we find an asterisk in the
  // Presentation, it means that the user does not want to restart the 
  // presentation.  If that is the case then we will stop the presentation, 
  // thus uncheck the button.
  // --------------------------------------------------------------------------
  WinCheckButton(hwndParent, IDC_RESTART, CHECKED);

  while (fgets(szLine, sizeof(szLine), hFile))
   {
    // --------------------------------------------------------------------------
    // The format of the Presentation File allows for comment lines.  A comment
    // line begins with a semicolon.  If we detect a comment line let's skip
    // over it.
    // --------------------------------------------------------------------------
    if (szLine[0] != ';')
     {
      // --------------------------------------------------------------------------
      // At this point szLine should contain a valid path and filename string
      // for the bitmap along with any option information.  Let's determine how
      // long the length of each line string is so that we can determine if we
      // need to remove the carriage return or line feed at the end of the line.
      // --------------------------------------------------------------------------
      usLineLength = strlen(szLine);

      if (usLineLength)
       {
        // --------------------------------------------------------------------------
        // If the last character on each line is a carriage return or line feed
        // character, then convert it to a NULL character for our processing.
        // --------------------------------------------------------------------------
        if ((szLine[usLineLength - 1] == '\n') || (szLine[usLineLength - 1] == '\r'))
         {
          szLine[usLineLength - 1] = '\0';
         }
       }

      // --------------------------------------------------------------------------
      // Here we check to see if the line actually has anything on it.  We will
      // ignore the blank lines.
      // --------------------------------------------------------------------------
      if (szLine[0])
       {
        // --------------------------------------------------------------------------
        // If the Run Presentation FullScreen option is set in the Presentation 
        // file, then make sure we check the checkbox.
        // --------------------------------------------------------------------------
        if ((szLine[0] == '/') && (szLine[1] == '$'))
         {
          WinCheckButton(hwndParent, IDC_FULLSCR, CHECKED);
         }

        else if ((szLine[0] == '/') && (szLine[1] != '$')) 
         {
          // --------------------------------------------------------------------------
          // Find the slash and get everything after it, since it is the default
          // time value.  Convert the time string to an integer and insert the 
          // value into the spinbutton control.
          // --------------------------------------------------------------------------
          szTime = strtok(szLine, "/");  
          ulDefTime = atoi(szTime);

          WinSendMsg(hwndSpinDefTime, SPBM_SETCURRENTVALUE, (MPARAM)ulDefTime,  (MPARAM)NULL);          
         }

        // --------------------------------------------------------------------------
        // By default at the top of this routine, we check the restart pushbutton
        // which means that the user does want to restart the presentation.  If
        // we find the asterisk, then we will uncheck the button since the user 
        // does not want to automatically restart the presentation.  The asterisk
        // means that the presentation will stop after the last bitmap file is 
        // displayed.
        // --------------------------------------------------------------------------
        else if (!strcmp("*", szLine))                 
         {
          WinCheckButton(hwndParent, IDC_RESTART, UNCHECKED);
         }

        else
         {
          // --------------------------------------------------------------------------
          // Allocate storage and copy szLine into pszString.
          // --------------------------------------------------------------------------
          pszString = malloc((strlen(szLine) + 1));
          strcpy(pszString, szLine);

          // --------------------------------------------------------------------------
          // Insert each item into the listbox.
          // --------------------------------------------------------------------------
          WinSendMsg(hwndListBox, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP(pszString));
         }
       }
     }
   }
 return(FALSE);
}

// *******************************************************************************
// FUNCTION:     DetermineMode   
//
// FUNCTION USE: Used to determine if the user has selected the menuitem to 
//               create a presentation or edit a presentation.
//
// DESCRIPTION:  In a nutshell we use the same dialog box for creating and     
//               editing a Presentation File.  Anything else would be ugly....
//               This routine is used to determine how we got to this dialog
//               procedure.  In other words, we have two distinct modes, a 
//               create mode which comes from the user selecting the "Create 
//               Presentation" menuitem, and a edit mode which comes from 
//               the user selecting the "Edit Presentation" menuitem.  
//
//               We can determine which of these two modes we originated from 
//               by checking mp2.  The mp2 message parameter will contain the
//               path and filename of the Presentation File to be edited if 
//               we are in edit mode.  This is due to the fact that the filename
//               string is passed as the last parameter to WinDlgBox when the 
//               dialog box is created.  Thus if we are in edit mode this 
//               function will return TRUE, otherwise the function will return 
//               FALSE.  This routine is needed so we know which controls to 
//               remove/change depending on whether we are editing or creating 
//               a presentation.
//
// PARAMETERS:   HWND     The window handle of the Dialog Box.              
//               MPARAM   The second message parameter (mp2).      
//
// RETURNS:      TRUE     if we are in edit mode
//               FALSE    if we are in create mode
//
// HISTORY:
//
// *******************************************************************************
BOOL DetermineMode (HWND hwnd, MPARAM mp2)
{
 BOOL  bEditMode; 
 HWND  hwndTitleBar;
 HWND  hwndListBox;
 FILE  *hfPresentation;    
 PSZ   pszPresFilename;
 CHAR  szBuffer[CCHMAXPATH];

 // --------------------------------------------------------------------------          
 // Store the handle for the Presentation Listbox Window.
 // --------------------------------------------------------------------------          
 hwndListBox = WinWindowFromID(hwnd, IDL_PRESENTATION);                              

 // --------------------------------------------------------------------------          
 // Ok, there are two ways that we can display this dialog box.  The first
 // way is if the user has asked us to "create a presentation".  The second
 // way is if the user has asked us to "edit a presentation".  If we are in 
 // edit mode the user has selected a presentation file to edit via the 
 // standard file dialog.  Once this dialog procedure is called the path 
 // and filename of the presentation file is passed in the pCreateParms 
 // parameter in the call to WinLoadDlg.  Thus, if we are in edit mode we 
 // should have the presentation name stored in mp2.  The goal is to 
 // reuse the dialog box, since the data is essentially the same.
 // --------------------------------------------------------------------------    
 if (mp2) 
  {
   // --------------------------------------------------------------------------    
   // Since we are in edit mode set the bEditMode flag to TRUE.
   // --------------------------------------------------------------------------    
   bEditMode = TRUE;

   // --------------------------------------------------------------------------    
   // Determine whether we are in create or edit mode                                                  
   // --------------------------------------------------------------------------    
   pszPresFilename = (PSZ)PVOIDFROMMP(mp2);                                                         

   // --------------------------------------------------------------------------    
   // Convert the Presentation File to UPPERCASE and store it in a buffer.
   // --------------------------------------------------------------------------    
   strupr(pszPresFilename);
   sprintf(szBuffer, "SHOWOFF - Editing %s", pszPresFilename);

   // --------------------------------------------------------------------------    
   // Get the titlebar window handle for this dialog and store it in hwndHelp.
   // --------------------------------------------------------------------------    
   hwndTitleBar = WinWindowFromID(hwnd, FID_TITLEBAR);

   // --------------------------------------------------------------------------    
   // Change the titlebar for this dialog to indicate that we are currently
   // editing a file.  The text will contain the path and filename of the 
   // presentation file.
   // For Example:
   //              SHOWOFF - Editing X:\\DIRECTORY\\PRESFILE.URI
   // --------------------------------------------------------------------------    
   WinSetWindowText(hwndTitleBar, szBuffer);

   // --------------------------------------------------------------------------      
   // Open the Presentation File.                                                     
   // --------------------------------------------------------------------------      
   hfPresentation = fopen(pszPresFilename, "r");                                             
                                                                            
   if (!hfPresentation)                                                                   
    {                                                                                 
     DisplayMessages(NULLHANDLE, "Error Loading Presentation File", MSG_ERROR);       
     return FALSE;                                                                          
    }                                                                                 
                                                                            
   // --------------------------------------------------------------------------      
   // Let's load the presentation into our dialog controls.
   // --------------------------------------------------------------------------      
   else                                                                               
    {                                                                                 
     LoadPresentation(hwndListBox, hfPresentation);                                   
    }
  }

 else
  {
   // --------------------------------------------------------------------------    
   // Since we are in create mode set the bEditMode flag to FALSE.
   // --------------------------------------------------------------------------    
   bEditMode = FALSE;
  }

 // --------------------------------------------------------------------------    
 // Close the file out.
 // --------------------------------------------------------------------------    
 fclose(hfPresentation);
 return bEditMode;
}
