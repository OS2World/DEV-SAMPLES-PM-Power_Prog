// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      SHOWOFF.H                                        mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Primary Include File for SHOWOFF                              บ
// บ                                                                               บ
// บ  NOTES:         This file contains all of the various defines for all of      บ
// บ                 window identifies and numeric string identifiers.             บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

// --------------------------------------------------------------------------
// Frame Window Handle is global for simplicity.
// --------------------------------------------------------------------------
HWND hwndFrame;

// --------------------------------------------------------------------------
// Window Identifiers
// --------------------------------------------------------------------------
#define ID_FRAME                  101
#define ID_PREVIEW                102
#define ID_FILENAME               103
#define ID_DISPBMP                104
#define ID_SECONDS                105
#define ID_BMPTOADD               106
#define ID_INFO                   107

// --------------------------------------------------------------------------
// Dialog Identifiers
// --------------------------------------------------------------------------
#define IDD_CONFIGURE             150
#define IDD_PRODINFO              151
#define IDD_CREATEPRES            152

// --------------------------------------------------------------------------
// Bitmap Identifiers
// --------------------------------------------------------------------------
#define IDB_PRODINFO              208            
#define IDB_LISTBOX               206            

// --------------------------------------------------------------------------
// Spinbutton and Entryfield Identifiers
// --------------------------------------------------------------------------
#define IDE_DEFTIME               301
#define IDE_BMPTIME               302
#define IDE_FILENAME              305

// --------------------------------------------------------------------------
// Listbox Identifiers
// --------------------------------------------------------------------------
#define IDL_PRESENTATION          401

// --------------------------------------------------------------------------
// CheckBox Identifiers
// --------------------------------------------------------------------------
#define IDC_SUPLOGO               0
#define IDC_ACTUALSIZE            1
#define IDC_SAVEWNDEXIT           2
#define IDC_END                   3

#define IDC_RESTART               510
#define IDC_FULLSCR               511

// --------------------------------------------------------------------------
// Pushbutton Identifiers
// --------------------------------------------------------------------------
#define DID_HELP                  550
#define DID_FIND                  551
#define DID_ADD                   552
#define DID_REMOVE                553
#define DID_CHANGE                554

// --------------------------------------------------------------------------
// Our Action Bar Menu
// --------------------------------------------------------------------------
#define ID_FILE_MENU              600
#define IDM_OPENBMP               601
#define IDM_CREATE                602
#define IDM_EDITPRES              603
#define IDM_PREVIEW               604
#define IDM_EXIT                  605

#define ID_EDIT_MENU              610
#define IDM_EDITCUT               611
#define IDM_EDITCOPY              612
#define IDM_EDITPASTE             613
#define IDM_EDITCLEAR             614

#define ID_PRES_MENU              620
#define IDM_STARTSHOW             621
#define IDM_PAUSE                 622
#define IDM_FFORWARD              623
#define IDM_REWIND                624
#define IDM_RESTART               625
#define IDM_STOPSHOW              626
                                  
#define ID_OPTIONS_MENU           630         
#define IDM_DESKTOP               631                           
#define IDM_LOCKUP                632          
#define IDM_FULLSCN               633          
#define IDM_SAVEWND               634            
#define IDM_CONFIGURE             635
#define IDM_DEVICEINF             636         

#define ID_HELP_MENU              640
#define IDM_HELPGEN               641
#define IDM_HELPKEYS              642
#define IDM_PRODINFO              643    

// --------------------------------------------------------------------------
// Help Panel Definitions
// --------------------------------------------------------------------------
#define PANEL_OPEN_BUTTON         20000
#define PANEL_KEYS_HELP           20408
#define PANEL_GENERAL_HELP        20409
#define PANEL_CREATE_PRES         20998
#define PANEL_EDIT_PRES           20999

// --------------------------------------------------------------------------
// Help Manager Definitions
// --------------------------------------------------------------------------
#define HELP_FILE                 "SHOWOFF.HLP"
#define HELP_GRAPHIC              1001                                    

// --------------------------------------------------------------------------
// Dos Open Flags
// --------------------------------------------------------------------------
#define OPEN_FILE       0x02          // replace existing file
#define CREATE_FILE     0x10          // create new file
#define OPEN_FLAGS      OPEN_FILE | CREATE_FILE
#define OPEN_MODE       OPEN_FLAGS_NOINHERIT     | OPEN_SHARE_DENYREADWRITE | OPEN_ACCESS_READWRITE
#define OPENMODE_FLAGS  OPEN_FLAGS_FAIL_ON_ERROR | OPEN_SHARE_DENYREADWRITE | OPEN_ACCESS_READONLY

// --------------------------------------------------------------------------
// These defines are used to navigate the direction of the presentation.
// --------------------------------------------------------------------------
#define PRES_MOVE_FORWARD   0xaaaa
#define PRES_MOVE_BACKWARD  0xbbbb
#define PRES_RESTART        0xcccc

// --------------------------------------------------------------------------
// These defines are used to query the status of checkbox buttons.
// --------------------------------------------------------------------------
#define UNCHECKED     FALSE
#define CHECKED       TRUE

// --------------------------------------------------------------------------
// File Extension Definitions
// --------------------------------------------------------------------------
#define EXT_PRESENTATION   "*.URI"
#define EXT_BITMAP         "*.BMP"

// --------------------------------------------------------------------------
// Various String Definitions.
// --------------------------------------------------------------------------
#define IDS_BLANK      ""
#define IDS_RESTART    " ~Automatically Restart Presentation "
#define IDS_FULLSCR    " ~Run this Presentation Full Screen "
#define IDS_DEFTIME    "Default Time Between Bitmaps"
#define IDS_SECONDS    "Seconds"
#define IDS_DISPLAY    "Display This Bitmap For"
#define IDS_BMPTOADD   "Bitmap File to Add"
#define IDS_BMPTOEDIT  "Bitmap File and Options"
#define IDS_BMPINPRES  "Bitmaps in Presentation"
#define IDS_PRESOPTS   "Presentation Options"
#define IDS_PRESENT    "Presentation"
#define IDS_NOTEXT     "You must specify a bitmap path and filename to add to the presentation."
#define IDS_NOSELECT   "You must first select an item in the Presentation Listbox to Remove."
#define IDS_SAVE_BTN   "~Save"
#define IDS_SAVEAS_BTN "~Save As..."
#define IDS_INFO1      " Make a selection from the listbox and edit it in the entryfield."
#define IDS_PAUSE_OFF  "Pause\tF9" 
#define IDS_PAUSE_ON   "Play\tF9"
#define BULLET         "" 

// --------------------------------------------------------------------------
// Application Name for Initialization Data
// --------------------------------------------------------------------------
#define APPNAME   "Showoff For OS/2"  

// --------------------------------------------------------------------------             
// These are the defines for the keynames in the OS2.INI files                            
// --------------------------------------------------------------------------             
#define KEY_LOGO         "S_SuppressProdInfo"                                              
#define KEY_STRETCH      "S_DisplayActualSize"                                             
#define KEY_WNDEXIT      "S_SaveWndPosOnExit"                                              
