// 浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様� 
// �  FILENAME:      STRUCTS.H                                        mm/dd/yy     �
// �                                                                               �
// �  DESCRIPTION:   Important Structures                                          �
// �                                                                               �
// �  NOTES:         This file contains various low-level structures that are used �
// �                 by the application.                                           �
// �                                                                               �
// �  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        �
// �  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          �
// �                                                                               �
// �  REVISION DATES:  mm/dd/yy  Created this file                                 �
// �                                                                               �
// 藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�

// --------------------------------------------------------------------------
// Data Structure used for a simple circular linked list containing all 
// of the bitmap file strings in a presentation.
// --------------------------------------------------------------------------
typedef struct PRESENTATION                                                                 
{                                                                                           
  struct PRESENTATION *pPrev;                                                               
  struct PRESENTATION *pNext;                                                               
  PSZ    pszBitmapPath;                                                                         
} PRESENTATION, FAR *PPRESENTATION;                                                         

// --------------------------------------------------------------------------
// Function Prototypes.
// --------------------------------------------------------------------------
PPRESENTATION ParsePresentationFile(FILE *hFile);
VOID FreePresentationMemory(PPRESENTATION pHeadNode);
