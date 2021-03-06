// 浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�
// �  FILENAME:      FILEOPS.H                                        mm/dd/yy     �
// �                                                                               �
// �  DESCRIPTION:   Header File for the PMED File Operations                      �
// �                                                                               �
// �  NOTES:         None                                                          �
// �                                                                               �
// �  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        �
// �  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          �
// �                                                                               �
// �  REVISION DATES:  mm/dd/yy  Created this file                                 �
// �                                                                               �
// 藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�

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