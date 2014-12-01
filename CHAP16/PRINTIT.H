// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      PRINTIT.H                                        mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Main Header file for PRINTIT/Chapter 16 Sample Program        บ
// บ                                                                               บ
// บ  NOTES:         This program demonstartes printing and font management.       บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#define INCL_SPL
#define INCL_SPLDOSPRINT
#define INCL_SPLERRORS
#define INCL_DEV
#include <os2.h>
#include "fileops.h"
#include "shcommon.h"

// --------------------------------------------------------------------------
// Window IDs for windows created by the application.
// --------------------------------------------------------------------------
#define IDB_PRODINFO        55
#define RID_APPWND         100
#define SELPRT_ID          200
#define SELFORM_ID         300

#define MID_FILE           1000
#define MID_OPEN           1001
#define MID_PRINT          1002
#define MID_QUEUE          1003

#define MID_OPTIONS        1010
#define MID_FONT           1011
#define MID_MARGINS        1012

#define MID_HELPMENU       1020
#define MID_HELPGEN        1021
#define MID_HELPKEYS       1022
#define MID_PRODINFO       1023

#define CID_PRINTERLB      2000
#define CID_SETUP          2001

#define CID_MAR_LEFT       3000
#define CID_MAR_RIGHT      3001
#define CID_MAR_TOP        3002
#define CID_MAR_BOTTOM     3003
#define CID_ONEUP          3005
#define CID_MEASURE        3007

// --------------------------------------------------------------------------
// Global variables used in multiple source modules
// --------------------------------------------------------------------------
extern PPRQINFO3   gpQueueBuf;
extern ULONG       gnQueue;
extern ULONG       gcQueues;

// --------------------------------------------------------------------------
// Prototype inter-module functions
// --------------------------------------------------------------------------
BOOL  SelectPrinter(HWND hwnd);
BOOL  SetPageFormat(HWND hwnd, PRECTL prectlMargins);
