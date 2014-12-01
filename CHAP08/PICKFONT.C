// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป          
// บ  FILENAME:      PICKFONT.C                                       mm/dd/yy     บ          
// บ                                                                               บ          
// บ  DESCRIPTION:   Source File to handle Font Selection                          บ          
// บ                                                                               บ          
// บ  NOTES:         This file contains the source code for the PMEDIT program.    บ          
// บ                 PMEDIT is a simple text editor designed to illustrate dialog  บ          
// บ                 and menu concepts.                                            บ          
// บ                                                                               บ          
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ          
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ          
// บ                                                                               บ          
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ          
// บ                                                                               บ          
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ          

#define  INCL_WIN
#define  INCL_GPI

#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "pmedit.h"

static   BOOL     false = FALSE;

#define  ATTR_BOLD         0
#define  ATTR_ITALIC       1
#define  ATTR_UNDERSCORE   2
#define  ATTR_STRIKEOUT    3
#define  ATTR_OUTLINE      4


static PSZ pszfAttr[] = { ".Bold", ".Italic", ".Underscore", ".Strikeout", ".Outline" }; 

static PSZ GetCurrentFont( HWND hwnd );

// *******************************************************************************
// FUNCTION:     SelectFont
//
// FUNCTION USE: Used to allow the user to modify the current font.
//
// DESCRIPTION:  This function retrieves the current font for a particular window
//               the raises the Standard Font Dialog to allow the user to select
//               a new font.  If a new font is selected, the selection is set as
//               the current font for the window by modifying the PP_FONTNAMESIZE
//               presentation parameter.
//
// PARAMETERS:   HWND     window handle
//
// RETURNS:      VOID
//
// HISTORY:
//
// *******************************************************************************
BOOL SelectFont(HWND hwnd)
{
   HPS            hps;
   FONTDLG        fontdlg;
   char           szFamilyName[FACESIZE];
   USHORT         ptSize;
   ULONG          ulLength;
   PSZ            pszFullFaceName;
   PSZ            pszParse;
   int            i;
   FONTMETRICS    fm;
   BOOL           rc;

   // --------------------------------------------------------------------------       
   // Get the current presentation parameter 
   // --------------------------------------------------------------------------       
   pszFullFaceName = GetCurrentFont(hwnd);

   // --------------------------------------------------------------------------       
   // Get a presentation space for the window and query the metrics 
   // of the current default font - set based on presentation param. 
   // --------------------------------------------------------------------------       
   hps = WinGetPS(hwnd);
   GpiQueryFontMetrics(hps, sizeof(FONTMETRICS), &fm);

   // --------------------------------------------------------------------------       
   // initialize the FONTDLG structure 
   // --------------------------------------------------------------------------       
   memset((PVOID)&fontdlg, 0, sizeof(FONTDLG));
   szFamilyName[0] = '\0';

   fontdlg.cbSize                = sizeof(FONTDLG);
   fontdlg.hpsScreen             = hps;
   fontdlg.pszFamilyname         = szFamilyName;
   fontdlg.usFamilyBufLen        = FACESIZE;
   fontdlg.clrFore               = CLR_BLACK;
   fontdlg.clrBack               = CLR_WHITE;
   fontdlg.fl                    = FNTS_INITFROMFATTRS;
   fontdlg.fAttrs.usRecordLength = sizeof(FATTRS);
   fontdlg.fAttrs.lMatch         = fm.lMatch;

   // --------------------------------------------------------------------------       
   // parse the presentation parameter to get the size 
   // and attributes of the current font 
   // --------------------------------------------------------------------------       
   if(pszFullFaceName != NULL && strlen(pszFullFaceName) != 0) 
    {
      pszParse = strtok( pszFullFaceName, "." );

      if (fm.fsDefn & FM_DEFN_OUTLINE) 
       {
         fontdlg.fxPointSize = MAKEFIXED(atoi(pszParse), 0);
       } // endif 

      pszParse = strtok( NULL, "." );
      strcpy(fontdlg.fAttrs.szFacename, pszParse);

      while((pszParse = strtok(NULL, ".")) != NULL) 
       {
        for(i = 0;
            i <= ATTR_OUTLINE && strcmp(pszParse, &(pszfAttr[i][1]));
            i++ );

        switch (i) 
         {
          case ATTR_BOLD:
               fontdlg.fAttrs.fsSelection |= FATTR_SEL_BOLD;
               break;
         
          case ATTR_ITALIC:
               fontdlg.fAttrs.fsSelection |= FATTR_SEL_ITALIC;
               break;
              
          case ATTR_UNDERSCORE:
               fontdlg.fAttrs.fsSelection |= FATTR_SEL_UNDERSCORE;
               break;
          
          case ATTR_STRIKEOUT:
               fontdlg.fAttrs.fsSelection |= FATTR_SEL_STRIKEOUT;
               break;
          
          case ATTR_OUTLINE:
               fontdlg.fAttrs.fsSelection |= FATTR_SEL_OUTLINE;
               break;
           
          default:
               break;
         } // endswitch 
       } // endwhile 
      free( pszFullFaceName );
   } // endif 

   // --------------------------------------------------------------------------       
   // get the user's selection by calling the 
   // standard font dialog box.
   // --------------------------------------------------------------------------       
   WinFontDlg(HWND_DESKTOP, hwnd, &fontdlg);

   if (fontdlg.lReturn == DID_OK) 
    {
      // --------------------------------------------------------------------------       
      // build a new presentation parameter string 
      // --------------------------------------------------------------------------       
      ptSize = FIXEDINT(fontdlg.fxPointSize);

      ulLength = 6;        /* start with max length of number + . */
      ulLength += strlen(fontdlg.fAttrs.szFacename);
      if (fontdlg.flStyle & FATTR_SEL_BOLD)
         ulLength += strlen(pszfAttr[ATTR_BOLD]);

      if (fontdlg.flStyle & FATTR_SEL_ITALIC)
         ulLength += strlen(pszfAttr[ATTR_ITALIC]);

      if (fontdlg.flStyle & FATTR_SEL_UNDERSCORE)
         ulLength += strlen(pszfAttr[ATTR_UNDERSCORE]);

      if (fontdlg.flStyle & FATTR_SEL_STRIKEOUT)
         ulLength += strlen(pszfAttr[ATTR_STRIKEOUT]);

      if (fontdlg.flStyle & FATTR_SEL_OUTLINE)
         ulLength += strlen(pszfAttr[ATTR_OUTLINE]);

      ulLength++;

      pszFullFaceName = malloc(ulLength);

      sprintf( pszFullFaceName, "%d.%s", ptSize, fontdlg.fAttrs.szFacename );
      if (fontdlg.flStyle & FATTR_SEL_BOLD)
         strcat(pszFullFaceName, pszfAttr[ATTR_BOLD]);

      if (fontdlg.flStyle & FATTR_SEL_ITALIC)
         strcat(pszFullFaceName,pszfAttr[ATTR_ITALIC]);

      if (fontdlg.flStyle & FATTR_SEL_UNDERSCORE)
         strcat(pszFullFaceName,pszfAttr[ATTR_UNDERSCORE]);

      if (fontdlg.flStyle & FATTR_SEL_STRIKEOUT)
         strcat(pszFullFaceName,pszfAttr[ATTR_STRIKEOUT]);

      if (fontdlg.flStyle & FATTR_SEL_OUTLINE)
         strcat(pszFullFaceName,pszfAttr[ATTR_OUTLINE]);


      // --------------------------------------------------------------------------       
      // set the new current font 
      // --------------------------------------------------------------------------       
      rc = WinSetPresParam(hwnd, PP_FONTNAMESIZE, strlen(pszFullFaceName) + 1, pszFullFaceName); 

      // --------------------------------------------------------------------------       
      // write the current font to the OS2.INI file
      // --------------------------------------------------------------------------       
      PrfWriteProfileString(HINI_USER,                
                            APPNAME,                  
                            "User Font",                 
                            pszFullFaceName);     

      free(pszFullFaceName);
   } // endif 

   WinReleasePS(hps);
   return rc;
}

// *******************************************************************************
// FUNCTION:     GetCurrentFont
//
// FUNCTION USE: Used to obtain the current font selected for a window
//
// DESCRIPTION:  This function retrieves the PP_FONTNAMESIZE parameter into an
//               allocated buffer.  If the original buffer is too short to hold
//               the returned string a new string is allocated.  The address of
//               the allocated string is returned to the caller.  The caller is
//               responsible for freeing the memory.
//
// PARAMETERS:   HWND     window handle
//
// RETURNS:      PSZ      pointer to buffer containing presentaion parameter
//
// HISTORY:
//
// *******************************************************************************
static PSZ GetCurrentFont(HWND hwnd)
{
   PSZ      pszPresParam;
   ULONG    ulLength;
   ULONG    ulLengthReturned;


   // --------------------------------------------------------------------------       
   // set length and allocate initial string 
   // --------------------------------------------------------------------------       
   ulLengthReturned = ulLength = FACESIZE;
   pszPresParam = malloc(ulLength);

   // --------------------------------------------------------------------------       
   // loop until length returned less than buffer length 
   // means we got the whole thing 
   // --------------------------------------------------------------------------       
   while(ulLengthReturned == ulLength) 
    {
     // --------------------------------------------------------------------------       
     // get PP_FONTNAMESIZE presentation parameter 
     // --------------------------------------------------------------------------       
     ulLengthReturned = WinQueryPresParam(hwnd, PP_FONTNAMESIZE, 0, NULL, ulLength, pszPresParam, 0);    

     // --------------------------------------------------------------------------       
     // truncated string ?? 
     // --------------------------------------------------------------------------       
     if (ulLengthReturned == ulLength) 
      {
       // --------------------------------------------------------------------------       
       // free current buffer 
       // --------------------------------------------------------------------------       
       free(pszPresParam);

       // --------------------------------------------------------------------------       
       // reallocate a buffer twice as large 
       // --------------------------------------------------------------------------       
       ulLengthReturned = ulLength *= 2;
       pszPresParam = malloc(ulLength);
      } // endif 
    } // endwhile 

   // --------------------------------------------------------------------------       
   // send back pointer to returned presentation parameter 
   // --------------------------------------------------------------------------       
   return pszPresParam;
}
