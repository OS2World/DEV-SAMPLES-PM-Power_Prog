// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป          
// บ  FILENAME:      PRESENT.C                                        mm/dd/yy     บ          
// บ                                                                               บ          
// บ  DESCRIPTION:   Presentation File Routines                                    บ          
// บ                                                                               บ          
// บ  NOTES:         This file contains various functions that are used for the    บ          
// บ                 processing of the presentations.                              บ          
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
#include <stdlib.h>                 
#include <stdio.h>                  
#include <string.h>                 
#include "showoff.h"                
#include "structs.h"
#include "showoff.fnc"              

// *******************************************************************************
// FUNCTION:     ParsePresentationFile
//
// FUNCTION USE: Loads a Bitmap for Display
//
// DESCRIPTION:  This function will parse the .URI file selected by
//               the user.  Each valid line contains a string pointing
//               to a bitmap file.  The bitmap file strings are placed
//               in a circular linked list.  If this function is
//               successful, a pointer to the head node of the list
//               is returned.
//                
//               The function will allocate and create a PRESENTATION structure
//               based on the lines in a presentation file.
//
// PARAMETERS:   FILE       *hfile pointer to file handle
//
// RETURNS:      PPRESENTATION  pointer to PRESENTATION structure
//               FALSE          error
//
// HISTORY:
//   mm-dd-yy                                                
//
// *******************************************************************************
PPRESENTATION ParsePresentationFile(FILE *hFile)
{
  PPRESENTATION pFirst = (PPRESENTATION)NULL;
  PPRESENTATION pCurrent;
  USHORT        usLineLength;
  CHAR          szLine[CCHMAXPATH];

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
        pCurrent = calloc(1, sizeof(PRESENTATION));
        pCurrent->pszBitmapPath = malloc((strlen(szLine) + 1));
        strcpy(pCurrent->pszBitmapPath, szLine);

        if (pFirst)
         {
          pCurrent->pNext = pFirst;
          pCurrent->pPrev = pFirst->pPrev;
          pCurrent->pPrev->pNext = pCurrent;
          pFirst->pPrev = pCurrent;

          if (!pFirst->pNext)
           {
            pFirst->pNext = pCurrent;
           }
         }

        else
         {
          pFirst = pCurrent;
          pFirst->pPrev = pFirst;
         }
       }
     }
   }
 return(pFirst);
}

// *******************************************************************************
// FUNCTION:     FreePresentationMemory
//
// FUNCTION USE: Function used to free memory
//
// DESCRIPTION:  This function will free the memory that is associated with
//               our PRESENTATION structure and reset the pointers.
//
// PARAMETERS:   PPRESENTATION pointer to the presentation structure
//
// RETURNS:      VOID
//
// HISTORY:
//
// *******************************************************************************
VOID FreePresentationMemory(PPRESENTATION pHeadNode)
{
 PPRESENTATION pFreeNode;

 while (pHeadNode)
  {
   if (pHeadNode->pNext == pHeadNode)
    {
     free(pHeadNode->pszBitmapPath);
     free(pHeadNode);
     pHeadNode = (PPRESENTATION)NULL;
    }

   else
    {
     pFreeNode = pHeadNode->pNext;
     pHeadNode->pNext = pFreeNode->pNext;
     free(pFreeNode->pszBitmapPath);
     free(pFreeNode);
    }
  }
 return;
}
