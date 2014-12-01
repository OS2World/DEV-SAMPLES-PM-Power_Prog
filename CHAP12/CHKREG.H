// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      CHKREG.H                                         mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Main header File for CHKREG/Chapter 12 Sample Program         บ
// บ                                                                               บ
// บ  NOTES:         Definitions and Prototypes                                    บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#include <os2.h>


// --------------------------------------------------------------------------
// defines for INI file processing
// --------------------------------------------------------------------------
#define APPNAME         "Check Register"
#define KEYNAME         "Background Bitmap"
#define OPTION_NO       "0"
#define OPTION_YES      "1"
#define USEIT           FALSE
#define LOSEIT          TRUE


// --------------------------------------------------------------------------
// defined maximum field length
// --------------------------------------------------------------------------
#define  DATE_STR_LEN      10
#define  REF_STR_LEN       5
#define  AMT_MAX_DIGITS    7
#define  AMT_STR_LEN       12
#define  MEMO_STR_LEN      64


// --------------------------------------------------------------------------
// define for product information bitmap
// --------------------------------------------------------------------------
#define  IDB_PRODINFO       99

// --------------------------------------------------------------------------
// define the data structure used to communicate between the transaction
// edit dialog and the main application
// --------------------------------------------------------------------------
typedef struct __TRANSDLG__ {
   USHORT      cb;
   BOOL        bDeposit;
   char        date[ DATE_STR_LEN + 1 ];
   char        ref[ REF_STR_LEN + 1 ];
   char        amount[ AMT_STR_LEN + 1 ];
   char        memo[ MEMO_STR_LEN + 1 ];
} TRANSDLG, *PTRANSDLG;

// --------------------------------------------------------------------------
// define global variables
// --------------------------------------------------------------------------
extern   BOOL     false;
extern   BOOL     true;

// --------------------------------------------------------------------------
// prototype global functions
// --------------------------------------------------------------------------
MRESULT EXPENTRY TransactDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
BOOL IsValidAmount( char *szAmount );

// --------------------------------------------------------------------------
// define user messages
// --------------------------------------------------------------------------
#define  UM_NEXTTRAN             (WM_USER +  0)
#define  UM_OPENFILE             (WM_USER +  1)

// --------------------------------------------------------------------------
// define window/resource id's
// --------------------------------------------------------------------------
#define  RID_BASE                1000
#define  RID_CHKREG              (RID_BASE +  0)
#define  RID_FILEOPEN            (RID_BASE +  1)
#define  RID_FILESAVEAS          (RID_BASE +  2)
#define  RID_PRINT               (RID_BASE +  3)
#define  RID_TRANSACTION         (RID_BASE +  4)
#define  RID_TRN_DELETE          (RID_BASE +  5)
#define  CID_REGISTER            (RID_BASE +  6)
#define  CID_HELP_PB             (RID_BASE +  7)
#define  CID_TRN_DATE            (RID_BASE +  8)
#define  CID_TRN_REF             (RID_BASE +  9)
#define  CID_TRN_AMT             (RID_BASE + 10)
#define  CID_TRN_MEMO            (RID_BASE + 11)
#define  CID_TRN_WTH             (RID_BASE + 12)
#define  CID_TRN_DEP             (RID_BASE + 13)
#define  RID_MSG_BADFILE         (RID_BASE + 14)
#define  RID_MSG_NOFILE          (RID_BASE + 15)
#define  RID_MSG_FILEEXISTS      (RID_BASE + 16)
#define  RID_MSG_FILEIOERR       (RID_BASE + 17)
#define  RID_MSG_BADNAME         (RID_BASE + 18)
#define  RID_BITMAP              (RID_BASE + 19)   

#define  ICON_WITHDRAW           (RID_BASE + 20)
#define  ICON_DEPOSIT            (RID_BASE + 21)
#define  ICON_DEPOSITM           (RID_BASE + 22)
#define  ICON_WITHDRAWM          (RID_BASE + 23)

#define  BMP_WITHDRAWB           (RID_BASE + 24)
#define  BMP_DEPOSITB            (RID_BASE + 25)
#define  BMP_WITHDRAWM           (RID_BASE + 26)
#define  BMP_DEPOSITM            (RID_BASE + 27)
#define  ID_CNRBITMAP            (RID_BASE + 28)

#define  RID_PRODINFO            (RID_BASE + 30)

// --------------------------------------------------------------------------
// define menu item id's
// --------------------------------------------------------------------------
#define  MID_BASE                10000
#define  MID_FILE                (MID_BASE +  0)
#define  MID_FILENEW             (MID_BASE +  1)
#define  MID_FILEOPEN            (MID_BASE +  2)
#define  MID_FILESAVEAS          (MID_BASE +  3)
#define  MID_PRINT               (MID_BASE +  4)
#define  MID_EXIT                (MID_BASE +  5)
#define  MID_TRANS               (MID_BASE +  6)
#define  MID_TRN_ENTER           (MID_BASE +  7)
#define  MID_TRN_DELETE          (MID_BASE +  8)
#define  MID_HELP                (MID_BASE +  9)
#define  MID_PRODINFO            (MID_BASE + 14)
#define  MID_VIEW                (MID_BASE + 15)
#define  MID_VIEWICON            (MID_BASE + 16)
#define  MID_VIEWNAME            (MID_BASE + 17)
#define  MID_VIEWTEXT            (MID_BASE + 18)
#define  MID_VIEWDETAIL          (MID_BASE + 19)
#define  MID_VIEWTREE            (MID_BASE + 20)
#define  MID_VIEWMINI            (MID_BASE + 21)
#define  MID_VIEWFLOW            (MID_BASE + 22)
#define  MID_VIEWTYPE            (MID_BASE + 23)
#define  MID_VIEWICONS           (MID_BASE + 24)
#define  MID_VIEWBITMAPS         (MID_BASE + 25)
#define  MID_POPUP               (MID_BASE + 26)
#define  MID_POP_DELETE          (MID_BASE + 27)
#define  MID_BACKBMP             (MID_BASE + 28) 


// --------------------------------------------------------------------------              
// define radiobuttons and help pushbutton                                                                  
// --------------------------------------------------------------------------              
#define  DID_HELP                 99
#define  IDC_USEIT               100
#define  IDC_LOSEIT              101
