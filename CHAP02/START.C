// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      START.C                                          mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Routines used in the session management of BUTTONS            บ
// บ                                                                               บ
// บ  NOTES:         This file contains the source code routines that are used     บ
// บ                 to start the programs represented by the BUTTONS.  The        บ
// บ                 main program start routine is called StartApplication and     บ
// บ                 uses the API WinStartApp.                                     บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
#define INCL_WINDIALOGS
#define INCL_WINPROGRAMLIST
#define INCL_DOSPROCESS
#define INCL_DOSSESMGR

#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <direct.h>
#include <process.h>
#include "buttons.h"
#include "buttons.fnc"


VOID ParsePathFromFileName (CHAR *pszPathAndFile, CHAR *pszPath, CHAR *pszFile);


// *******************************************************************************
// FUNCTION:     StartApplication
//
// FUNCTION USE: This function is used to start the program
//
// DESCRIPTION:  This function is used to kick off the programs represented
//               by the buttons.  This routine will populate a PROGDETAILS
//               structure and call WinStartApp to start the program.
//
// PARAMETERS:   PCH     The program path and filename to be started
//               PCH     The command line parameters to be passed
//               PCH     The program title
//
// RETURNS:      HAPP
//
// INTERNALS:    NONE
//
// HISTORY:
//
// *******************************************************************************
HAPP StartApplication(PCH szPgmName, PCH szComLine, PCH szProgTitle, ULONG ulPgmType)
{
 APIRET        rc;
 HWND          hwndNotify;
 CHAR          szDir[CCHMAXPATH];
 CHAR          szFile[CCHMAXPATH];
 ULONG         ulAppType;
 HAPP          happ;
 CHAR          szBuffer[250];
 PROGDETAILS   progdetails;


 // Take the full program path and filename
 // and parse it so that the string X:\DIRECTORY\FILE.TXT
 // becomes two strings X:\DIRECTORY and FILE.TXT
 ParsePathFromFileName(szPgmName, szDir, szFile);

 if (ulPgmType != 0)
  {
   ulAppType = ulPgmType;
  }

 else
  {
   // NOTE: Determine the application default type
   //       if the application is a Windows based
   //       application change the default Win session
   //       to be Enhanced Seamless as oppossed to a
   //       yucky WinOS/2 Fullscreen session, since
   //       seamless is groovy!
   rc = DosQueryAppType(szPgmName, &ulAppType);

   if ((ulAppType == FAPPTYP_WINDOWSREAL) ||
       (ulAppType == FAPPTYP_WINDOWSPROT) ||
       (ulAppType == FAPPTYP_WINDOWSPROT31))
    {
     ulAppType  = PROG_31_ENHSEAMLESSVDM;
    }
  }

 progdetails.Length                      = sizeof(PROGDETAILS);  // Length of structure
 progdetails.progt.progc                 = ulAppType;            // Application Program Type
 progdetails.progt.fbVisible             = SHE_VISIBLE;          // Visibility Indicator
 progdetails.pszTitle                    = szProgTitle;          // Program Title
 progdetails.pszExecutable               = szPgmName;            // Executable Path and Filename
 progdetails.pszParameters               = szComLine;            // Command Line Parameters
 progdetails.pszStartupDir               = szDir;                // Working Directory
 progdetails.pszIcon                     = NULL;                 // Program Icon
 progdetails.pszEnvironment              = NULL;                 // Environment String
 progdetails.swpInitial.x                = 0;                    // Initial x Window Position
 progdetails.swpInitial.y                = 0;                    // Initial y Window Position
 progdetails.swpInitial.cx               = 0;                    // Initial cx Window Size
 progdetails.swpInitial.cy               = 0;                    // Initial cy Window Size
 progdetails.swpInitial.hwndInsertBehind = HWND_TOP;             // Window Placement
 progdetails.swpInitial.fl               = SWP_SHOW;             // Initial Window Flags

 happ = WinStartApp(NULLHANDLE,              // Notification window handle
                    &progdetails,            // Program Details Structure
                    NULL,                    // Command Line Parameters for the started application
                    NULL,                    // Reserved
                    SAF_INSTALLEDCMDLINE);   // Start Application Flags

 return happ;
}



// *******************************************************************************
// FUNCTION:     ParsePathFromFileName
//
// FUNCTION USE: Takes a valid path and filename and separates the path  
//               string from the filename string.
//
// DESCRIPTION:  This function will take a valid path and filename and
//               return two individual strings; one containing the directory
//               path and the other returning the valid filename.
//
// PARAMETERS:   CHAR *     Complete Path and Filename   X:\DIRECTORY\FILE.TXT
//               CHAR *     on exit contains X:\DIRECTORY
//               CHAR *     on exit contains FILE.TXT
//
// RETURNS:      VOID
//
// INTERNALS:    NONE
//
// HISTORY:
//
// *******************************************************************************
VOID ParsePathFromFileName (CHAR *pszPathAndFile, CHAR *pszPath, CHAR *pszFile)
{
   USHORT  usCounter;

   for (usCounter = strlen(pszPathAndFile) - 1; usCounter > 0; usCounter--)
    {
     if (*(pszPathAndFile + usCounter) == '\\')
      break;
    }

   strcpy (pszFile, pszPathAndFile + usCounter + 1);
   strncpy (pszPath, pszPathAndFile, usCounter);
   pszPath[usCounter] = '\0';
   return;
}


