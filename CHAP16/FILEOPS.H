// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      FILEOPS.H                                        mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   File Operations header for PRINTIT/Chapter 16 Sample Program  บ
// บ                                                                               บ
// บ  NOTES:         This program demonstartes printing and font management.       บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#include    <stdio.h>

// --------------------------------------------------------------------------
// Define constant for file buffer size
// --------------------------------------------------------------------------
#define  BUFFER_SIZE    4000

// --------------------------------------------------------------------------
// Define the file control structure type
// --------------------------------------------------------------------------
typedef struct _appfile_ 
 {
   FILE     *file;
   size_t   cbBuffer;
   size_t   nCurLine;
   size_t   nPrevLine;
   size_t   nFilePos;
   char     achBuffer[BUFFER_SIZE];
 } APPFILE, *PAPPFILE;

// --------------------------------------------------------------------------
// Define the application interface functions
// --------------------------------------------------------------------------
PAPPFILE OpenFile(char *pszFileName);
void CloseFile(PAPPFILE pAppFile);
size_t ReadFile(PAPPFILE pAppFile, char **ppchBuffer);
BOOL  IsEOF(PAPPFILE pAppFile);
