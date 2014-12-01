// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      DRAGEM.H                                         mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Main header File for DRAGEM/Chapter 7 Sample Program          บ
// บ                                                                               บ
// บ  NOTES:         None                                                          บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#define  INCL_PM
#define  INCL_DOS
#define  INCL_DOSERRORS
#include <os2.h>


// --------------------------------------------------------------------------
// define identifier for Product Information Bitmap
// --------------------------------------------------------------------------
#define IDB_PRODINFO     99

// --------------------------------------------------------------------------
// define window IDs
// --------------------------------------------------------------------------
#define  RID_LAZY       1000
#define  RID_OBJECT     1001
#define  DLGID_PRODINFO 1002

// --------------------------------------------------------------------------
// Define menu item identifiers
// --------------------------------------------------------------------------
#define  MID_LAZYDRAG   2000
#define  MID_PICKUP     2001
#define  MID_DROP       2002
#define  MID_DROPCOPY   2003
#define  MID_DROPMOVE   2004
#define  MID_CANCEL     2005

#define  MID_DRIVE      3000
#define  MID_DRIVEA     3001
#define  MID_DRIVEZ     3026

#define  MID_HELP       4000
#define  MID_PRODINFO   4001

// --------------------------------------------------------------------------
// Define application defined messages
// --------------------------------------------------------------------------
#define  LAZY_SETOBJECTHWND   (WM_USER)
#define  LAZY_FILLLIST        (WM_USER+1)
#define  LAZY_DORENDER        (WM_USER+2)
#define  LAZY_DODROP          (WM_USER+3)
#define  LAZY_DIRFROMINI      (WM_USER+4)
#define  LAZY_INIFROMDIR      (WM_USER+5)
#define  LAZY_DISCARDOBJ      (WM_USER+6)

// --------------------------------------------------------------------------
// Define constant strings used in INI file processing
// --------------------------------------------------------------------------
#define INIAPPNAME   "DRAGEM"
#define INIFRMPOSKEY "FRAMEPOS"
#define INILBXPOSKEY "LISBOXPOS"
#define INICURDSKKEY "CURRENT DISK"
#define INICURDIRKEY "CURRENT DIR"

// --------------------------------------------------------------------------
// Define the object window thread function
// --------------------------------------------------------------------------
void _Optlink ObjectThread( PVOID pArg );
