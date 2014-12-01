// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      FILEOPS.C                                        mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Source for the File Operation functions for                   บ
// บ                 PRINTIT, the Chapter 16 Sample program                        บ
// บ                                                                               บ
// บ  NOTES:         This program demonstrates printing and font management.       บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <os2.h>
#include "fileops.h"

// *******************************************************************************
// FUNCTION:     OpenFile
//
// FUNCTION USE: Open file for reading by the application
//
// DESCRIPTION:  Allocates a structure to maintain current information about
//               the file, opens the file, and initializes the structure
//
// PARAMETERS:   PSZ      pointer to the file name to open
//
// RETURNS:      PAPPFILE pointer to file control structure - NULL on error
//
// *******************************************************************************
PAPPFILE OpenFile( char *pszFileName )
{
   PAPPFILE    pAppFile;

// --------------------------------------------------------------------------
// Allocate the file control structure and clear
// --------------------------------------------------------------------------
   pAppFile = (PAPPFILE)malloc( sizeof( APPFILE ) );
   if (pAppFile == NULL) {
      return NULL;
   } /* endif */
   memset( pAppFile, 0, sizeof( APPFILE ) );

// --------------------------------------------------------------------------
// Open the file and save the 'C' stream pointer
// --------------------------------------------------------------------------
   pAppFile->file = fopen( pszFileName, "rb" );
   if (pAppFile->file == NULL) {
      free( pAppFile );
      return NULL;
   } /* endif */

// --------------------------------------------------------------------------
// Get in the first buffer full of data and initialize the position info
// --------------------------------------------------------------------------
   pAppFile->cbBuffer = fread( pAppFile->achBuffer, 1, BUFFER_SIZE, pAppFile->file );
   pAppFile->nCurLine = pAppFile->nPrevLine = pAppFile->nFilePos = 0;

// --------------------------------------------------------------------------
// return pointer to control structure to the application
// --------------------------------------------------------------------------
   return pAppFile;
}

// *******************************************************************************
// FUNCTION:     CloseFile
//
// FUNCTION USE: Close a previously opened file
//
// DESCRIPTION:  Closes the file and deallocates the control structure
//
// PARAMETERS:   PAPPFILE pointer to the file control structure
//
// RETURNS:      void
//
// *******************************************************************************
void CloseFile( PAPPFILE pAppFile )
{
// --------------------------------------------------------------------------
// make sure a control structure was passed
// --------------------------------------------------------------------------
   if (pAppFile == NULL) {
      return;
   } /* endif */

// --------------------------------------------------------------------------
// Close the file
// --------------------------------------------------------------------------
   fclose( pAppFile->file );

// --------------------------------------------------------------------------
// Free the control structure
// --------------------------------------------------------------------------
   free( pAppFile );
}

// *******************************************************************************
// FUNCTION:     ReadFile
//
// FUNCTION USE: Extract a line of text from the file
//
// DESCRIPTION:  Scans the file buffer for a newline character.  If the buffer
//               is exhausted before the character is found, moves the current
//               portion of the line back to the beginning of the buffer and
//               reads more data from the file before continuing the scan.
//
// PARAMETERS:   PAPPFILE pointer to the file control structure
//               char**   pointer to location to store pointer to first
//                        character in the line
//
// RETURNS:      size_t   number of characters in the line (excluding the
//                        newline character
//
// *******************************************************************************
size_t ReadFile( PAPPFILE pAppFile, char **ppchBuffer )
{
   size_t   nChars = 0;
   char     *pchBuffer;

// --------------------------------------------------------------------------
// start of line is current position - set application pointer, then
// find number of characters and update control to start of next line
// --------------------------------------------------------------------------
   pchBuffer = *ppchBuffer = &(pAppFile->achBuffer[pAppFile->nCurLine]);
   pAppFile->nPrevLine = pAppFile->nCurLine;

// --------------------------------------------------------------------------
// parse the line till end of buffer or end of line
// --------------------------------------------------------------------------
   while (pAppFile->nCurLine < pAppFile->cbBuffer) {

// --------------------------------------------------------------------------
// if current character is end of line check for two character EOL
// --------------------------------------------------------------------------
      if (*pchBuffer == '\n' || *pchBuffer == '\r') {
         pAppFile->nCurLine++;

// --------------------------------------------------------------------------
// can't check if at the end of buffer - we'll have to read more and try
// again; other wise look for the partner to the current character
// --------------------------------------------------------------------------
         if (pAppFile->nCurLine < pAppFile->cbBuffer) {

// --------------------------------------------------------------------------
// first check for \n\r
// --------------------------------------------------------------------------
            if (*(pchBuffer++) == '\n' && *pchBuffer == '\r') {
               pAppFile->nCurLine++;
               pchBuffer++;

// --------------------------------------------------------------------------
// and if it wasn't that check for\r\n
// --------------------------------------------------------------------------
            } else if (*pchBuffer == '\n') {
               pAppFile->nCurLine++;
               pchBuffer++;
            } /* endif */

// --------------------------------------------------------------------------
// buffer position now set to start of next line - send back the number
// of characters.
// --------------------------------------------------------------------------
            return nChars;
         } /* endif */
      } /* endif */

// --------------------------------------------------------------------------
// this must have been a regular character - count one done and proceed.
// Note that end of buffer when checking EOL comes through here, but all
// the numbers are redone later anyway.
// --------------------------------------------------------------------------
      pAppFile->nCurLine++;
      pchBuffer++;
      nChars++;
   } /* endwhile */

// --------------------------------------------------------------------------
// oops - must have hit the end of the buffer.  If it turns out that a single
// line spans the whole buffer - just return what we have as a complete line.
// --------------------------------------------------------------------------
   if (pAppFile->nPrevLine == 0) {
      return pAppFile->cbBuffer;
   } /* endif */

// --------------------------------------------------------------------------
// Likewise, just return the current buffer and line if EOF is reached.
// --------------------------------------------------------------------------
   if (feof(pAppFile->file)) {
      return nChars;
   } /* endif */

// --------------------------------------------------------------------------
// If we get here - we're ready to read some more from the file.  Copy the
// current line back to the beginning of the buffer first.
// --------------------------------------------------------------------------
   pAppFile->cbBuffer -= pAppFile->nPrevLine;
   strncpy( pAppFile->achBuffer, &(pAppFile->achBuffer[pAppFile->nPrevLine]),
            pAppFile->cbBuffer );

// --------------------------------------------------------------------------
// Now, fill in the rest of the buffer from the file
// --------------------------------------------------------------------------
   pAppFile->cbBuffer += fread( &(pAppFile->achBuffer[pAppFile->cbBuffer]), 1,
                         BUFFER_SIZE - pAppFile->cbBuffer, pAppFile->file );

// --------------------------------------------------------------------------
// set scan back to the beginning of the line
// --------------------------------------------------------------------------
   pAppFile->nCurLine = 0;

// --------------------------------------------------------------------------
// Call ourselves recursively to rescan the line from the beginning
// --------------------------------------------------------------------------
   return ReadFile( pAppFile, ppchBuffer );
}

// *******************************************************************************
// FUNCTION:     IsEOF
//
// FUNCTION USE: Check for end of file
//
// DESCRIPTION:  Determine if all data as been read from the file and parsed
//               from the buffer.
//
// PARAMETERS:   PAPPFILE pointer to the file control structure
//
// RETURNS:      BOOl     TRUE if end of file reached
//
// *******************************************************************************
BOOL  IsEOF( PAPPFILE pAppFile )
{
// --------------------------------------------------------------------------
// At end of file if the file itself is exhausted AND the buffer is exhausted
// --------------------------------------------------------------------------
   return (feof( pAppFile->file ) && pAppFile->nCurLine == pAppFile->cbBuffer );
}
