// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      STATWND.C                                        10/10/94     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Functions used for the MLE spy window on the spreadsheet      บ
// บ                                                                               บ
// บ  NOTES:         This file contains the source code for various routines       บ
// บ                 that are used for the spy window on the Spreadsheet dialog.   บ
// บ                 The spy window is a simple MLE window that we will use to     บ
// บ                 illustrate the communication between the DDE Client and DDE   บ
// บ                 server windows.                                               บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ          
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ          
// บ                                                                               บ          
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ          
// บ                                                                               บ          
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ          


#define INCL_WINSYS
#define INCL_WINMESSAGEMGR
#define INCL_WINMLE

#include <os2.h>
#include <stdio.h>                  
#include <string.h>                 
#include <stdlib.h>                 
#include "errors.h"


// *******************************************************************************                   
// FUNCTION:     SetDefaultFont                                                                      
//                                                                                                   
// FUNCTION USE: Used to set the default font for the editor to a monospaced font                    
//                                                                                                   
// DESCRIPTION:  Changes the font of the MLE window.  This code is from               
//               Chapter 8's PMEDIT sample program.
//                                                                                                   
// PARAMETERS:   HWND     window handle of editor window                                             
//                                                                                                   
// RETURNS:      VOID                                                                                
//                                                                                                   
// HISTORY:                                                                                          
//                                                                                                   
// *******************************************************************************                   
VOID SetDefaultFont(HWND hwndMLE)
{
  CHAR   szFontNameSize[25];

  strcpy(szFontNameSize, "8.Courier");
  WinSetPresParam(hwndMLE, PP_FONTNAMESIZE, sizeof(szFontNameSize) + 1, szFontNameSize);
  return;
}

// *******************************************************************************       
// FUNCTION:       SendTextToMLE                                         
//                                                                          
// FUNCTION USE:   Displays a text string in the MLE Spy Window
//
// DESCRIPTION:    This function is used to send text strings to the       
//                 MLE spy window.  It takes the formatted input string    
//                 and updates the text in the MLE field.                  
//                                                                         
// PARAMETERS:     PSZ    the string to be displayed                
// 
// RETURNS:        USHORT error code                                                    
//                                                                          
// HISTORY:                                                                
//                                                                          
// *******************************************************************************       
USHORT SendTextToMLE(PSZ pszInputString)
{
   ULONG ulTextLen;
   ULONG ulSizeofInput;
   PSZ   pszBuffer;

   // --------------------------------------------------------------------------    
   // Get the length of the input string                                           
   // --------------------------------------------------------------------------    
   ulSizeofInput = strlen(pszInputString);

   if (hwndMLE)
    {
      ulTextLen = WinQueryWindowTextLength(hwndMLE) + 1;

      // --------------------------------------------------------------------------    
      // Allocate enough room for our buffer                                            
      // --------------------------------------------------------------------------    
      pszBuffer = (PSZ)malloc(ulTextLen + ulSizeofInput);

      if (pszBuffer)
       {
        WinQueryWindowText(hwndMLE, ulTextLen + ulSizeofInput, pszBuffer);          

        // --------------------------------------------------------------------------    
        // Add a newline and the user passed string into our buffer                                       
        // --------------------------------------------------------------------------    
        strcat(pszBuffer, "\n");
        strcat(pszBuffer, pszInputString);

        // --------------------------------------------------------------------------    
        // Put the text in the buffer into the MLE                                        
        // --------------------------------------------------------------------------    
        WinSetWindowText(hwndMLE, pszBuffer);

        // --------------------------------------------------------------------------    
        // Free the memory for the buffer                                        
        // --------------------------------------------------------------------------    
        free(pszBuffer);
       }

      else
       {
        return ERROR_ALLOCMEM;
       }
    }

   else
    {
     return ERROR_INVALID_HWND;
    }

   return FALSE;
}
