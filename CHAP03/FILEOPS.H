// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      FILEOPS.H                                        mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Header File for the PMED File Operations                      บ
// บ                                                                               บ
// บ  NOTES:         None                                                          บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

// --------------------------------------------------------------------------
// definition of the input buffer size
// --------------------------------------------------------------------------
#define  BUFFER_SIZE    4096

// --------------------------------------------------------------------------
// definition of the file control structure
// --------------------------------------------------------------------------
typedef struct _appfile_ {
   FILE     *file;
   size_t   cbBuffer;
   size_t   nCurLine;
   size_t   nPrevLine;
   size_t   nFilePos;
   char     achBuffer[ BUFFER_SIZE ];
} APPFILE, *PAPPFILE;

// --------------------------------------------------------------------------
// Function prototypes
// --------------------------------------------------------------------------
PAPPFILE OpenFile( char *pszFileName );
size_t WriteFile( PAPPFILE pAppFile, char *pszText, unsigned long fRewind );
void CloseFile( PAPPFILE pAppFile );
size_t ReadFile( PAPPFILE pAppFile, char **ppchBuffer );
unsigned long IsEOF( PAPPFILE pAppFile );
