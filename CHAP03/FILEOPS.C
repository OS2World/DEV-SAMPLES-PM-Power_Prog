// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      FILEOPS.C                                        mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Functions used by PMED for file access.                       บ
// บ                                                                               บ
// บ  NOTES:         This file contains all of the routines required to handle     บ
// บ                 file operations for the PMED editor.                          บ
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
#include <io.h>
#include "fileops.h"


// *******************************************************************************
// FUNCTION:     OpenFile
//
// FUNCTION USE: Opens a file for access by the editor.
//
// DESCRIPTION:  After allocating memory for the file control structure
//               the function attempts to open file in read mode in case
//               the file already exists.  If this fails, the file is
//               opened in write mode to create the file.  The initial
//               buffer full of data is then read from the file and the
//               file control block pointers are initialized.
//
// PARAMETERS:   PSZ       pointer to name of file to open
//
// RETURNS:      PAPPFILE  pointer to file control structure
//
// INTERNALS:    NONE
//
// *******************************************************************************
PAPPFILE OpenFile( char *pszFileName )
{
   PAPPFILE    pAppFile;

// --------------------------------------------------------------------------
// Allocate the file control structure and initialize
// --------------------------------------------------------------------------
   pAppFile = (PAPPFILE)malloc( sizeof( APPFILE ) );
   if (pAppFile == NULL) {
      return NULL;
   } /* endif */
   memset( pAppFile, 0, sizeof( APPFILE ) );

// --------------------------------------------------------------------------
// Attempt to open the file in read mode.  If the file does not exist this
// call will fail, so open the file in write mode.
// --------------------------------------------------------------------------
   pAppFile->file = fopen( pszFileName, "r+b" );
   if (pAppFile->file == NULL) {
      pAppFile->file = fopen( pszFileName, "w+b" );
      if (pAppFile->file == NULL) {
         free( pAppFile );
         return NULL;
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// Read in the initial buffer full of data and initialize file position
// elements of the file control structure.
// --------------------------------------------------------------------------
   pAppFile->cbBuffer = fread( pAppFile->achBuffer, 1,
                               BUFFER_SIZE, pAppFile->file );
   pAppFile->nCurLine = pAppFile->nPrevLine = pAppFile->nFilePos = 0;

// --------------------------------------------------------------------------
// return the pointer to the file control structure
// --------------------------------------------------------------------------
   return pAppFile;
}

// *******************************************************************************
// FUNCTION:     CloseFile
//
// FUNCTION USE: Closes a file.
//
// DESCRIPTION:  If a valid file control structure is given, close the
//               file indicated in the structure.
//
// PARAMETERS:   PAPPFILE  pointer to file control structure
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
void CloseFile( PAPPFILE pAppFile )
{
// --------------------------------------------------------------------------
// Validate the file control structure
// --------------------------------------------------------------------------
   if (pAppFile == NULL) {
      return;
   } /* endif */

// --------------------------------------------------------------------------
// Close the file and free the memory allocated for the control structure.
// --------------------------------------------------------------------------
   fclose( pAppFile->file );
   free( pAppFile );

// --------------------------------------------------------------------------
// Done, return to the caller
// --------------------------------------------------------------------------
   return;
}

// *******************************************************************************
// FUNCTION:     ReadFile
//
// FUNCTION USE: Reads a line of text from the file.
//
// DESCRIPTION:  Scan the cached file buffer for end of line characters,
//               refilling the buffer as necessary.  Return pointer to
//               the current position and number of characters in the
//               the line back to the caller.
//
// PARAMETERS:   PAPPFILE  pointer to file control structure
//               char**    pointer to location to store pointer to start
//                         of the line.
//
// RETURNS:      size_t    number of characters in the line.
//
// INTERNALS:    NONE
//
// *******************************************************************************
size_t ReadFile( PAPPFILE pAppFile, char **ppchBuffer )
{
   size_t   nChars = 0;
   char     *pchBuffer;

// --------------------------------------------------------------------------
// set returned buffer to next location in file buffer
// --------------------------------------------------------------------------
   pchBuffer = *ppchBuffer = &(pAppFile->achBuffer[pAppFile->nCurLine]);
   pAppFile->nPrevLine = pAppFile->nCurLine;

// --------------------------------------------------------------------------
// determine the number of characters - scan buffer until end of buffer
// is reached or a "new line" character is found.
// --------------------------------------------------------------------------
   while (pAppFile->nCurLine < pAppFile->cbBuffer) {

      if (*pchBuffer == '\n' || *pchBuffer == '\r') {
// --------------------------------------------------------------------------
// found end of line, but if next character is the other part of a CR/LF
// sequence then we need to bump the buffer pointer past the character
// --------------------------------------------------------------------------
         pAppFile->nCurLine++;
         if (pAppFile->nCurLine < pAppFile->cbBuffer) {

// --------------------------------------------------------------------------
// check for new line followed by return
// --------------------------------------------------------------------------
            if (*(pchBuffer++) == '\n' && *pchBuffer == '\r') {
               pAppFile->nCurLine++;
               pchBuffer++;

// --------------------------------------------------------------------------
// check for return followed by new line
// --------------------------------------------------------------------------
            } else if (*pchBuffer == '\n') {
               pAppFile->nCurLine++;
               pchBuffer++;
            } /* endif */

// --------------------------------------------------------------------------
// end of line found - send back the number of characters read
// --------------------------------------------------------------------------
            return nChars;
         } /* endif */
      } /* endif */

// --------------------------------------------------------------------------
// regular character - bump start of next line, output buffer pointer, and
// number of characters
// --------------------------------------------------------------------------
      pAppFile->nCurLine++;
      pchBuffer++;
      nChars++;
   } /* endwhile */

// --------------------------------------------------------------------------
// oops - must have hit the end of the buffer, if the line encompasses the
// entire buffer, just send back the whole buffer.
// --------------------------------------------------------------------------
   if (pAppFile->nPrevLine == 0) {
      return pAppFile->cbBuffer;
   } /* endif */

// --------------------------------------------------------------------------
// If at end of file - return what was left in buffer as a line - CR/LF at
// end of file is assumed.
// --------------------------------------------------------------------------
   if (feof(pAppFile->file)) {
      return nChars;
   } /* endif */

// --------------------------------------------------------------------------
// time to read more data from the file -> first move the current line
// contents to the beginning of the buffer.
// --------------------------------------------------------------------------
   pAppFile->cbBuffer -= pAppFile->nPrevLine;
   strncpy( pAppFile->achBuffer, &(pAppFile->achBuffer[pAppFile->nPrevLine]),
            pAppFile->cbBuffer );

// --------------------------------------------------------------------------
// fill the remainder of the buffer from the file
// --------------------------------------------------------------------------
   pAppFile->cbBuffer += fread( &(pAppFile->achBuffer[pAppFile->cbBuffer]), 1,
                         BUFFER_SIZE - pAppFile->cbBuffer, pAppFile->file );

// --------------------------------------------------------------------------
// go back and restart the scan from the beginning of the line and return
// the result.  Note: recursion should never be more than one level.
// --------------------------------------------------------------------------
   pAppFile->nCurLine = 0;
   return ReadFile( pAppFile, ppchBuffer );
}

// *******************************************************************************
// FUNCTION:     WriteFile
//
// FUNCTION USE: Writes a line of text back to the file.
//
// DESCRIPTION:  If rewind is specified, close the file and reopen in
//               write mode to clear the old contents of the file, then
//               write the data back to the file.
//
// PARAMETERS:   PAPPFILE        pointer to file control structure
//               char*           pointer to text to write
//               unsigned long   rewind flag
//
// RETURNS:      size_t    number of characters written.
//
// INTERNALS:    NONE
//
// *******************************************************************************
size_t WriteFile( PAPPFILE pAppFile, char *pszText, unsigned long fRewind )
{
   size_t   nChars;
   int      filenum;

// --------------------------------------------------------------------------
// If a request is made to rewind, then reset the file size back to zero.
// --------------------------------------------------------------------------
   if (fRewind) {
      fseek( pAppFile->file, 0, SEEK_SET );
      filenum = fileno(pAppFile->file);
      chsize( filenum, 0 );
   } /* endif */

// --------------------------------------------------------------------------
// if valid text was passed, get the number of characters then write the
// text to the file and append a CRLF.
// --------------------------------------------------------------------------
   if( pszText ) {
      nChars = strlen( pszText );
      if (nChars) {
         fwrite( pszText, 1, nChars, pAppFile->file );
      } /* endif */
      fwrite( "\r\n", 1, 2, pAppFile->file );
   } /* endif */

// --------------------------------------------------------------------------
// done - return the length of the text
// --------------------------------------------------------------------------
   return nChars;
}

// *******************************************************************************
// FUNCTION:     IsEof
//
// FUNCTION USE: Indicates when the end of file has been reached.
//
// DESCRIPTION:  Returns TRUE if the end of the file has been read
//               and all characters have been scanned from the
//               internal buffer.
//
// PARAMETERS:   PAPPFILE        pointer to file control structure
//
// RETURNS:      unsigned long   TRUE if end of file
//
// INTERNALS:    NONE
//
// *******************************************************************************
unsigned long IsEOF( PAPPFILE pAppFile )
{
   return (feof( pAppFile->file ) && pAppFile->nCurLine == pAppFile->cbBuffer );
}
