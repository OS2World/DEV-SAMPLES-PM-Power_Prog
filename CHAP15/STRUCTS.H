// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป 
// บ  FILENAME:      STRUCTS.H                                        mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Important Structures                                          บ
// บ                                                                               บ
// บ  NOTES:         This file contains various low-level structures that are used บ
// บ                 by the application.                                           บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

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
