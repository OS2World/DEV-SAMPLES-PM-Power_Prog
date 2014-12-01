// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      PMED.H                                           mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Main Header File for PMED/Chapter 03 Sample Program           บ
// บ                                                                               บ
// บ  NOTES:                                                                       บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#include <os2.h>
#include <stdlib.h>
#include <stdio.h>

// --------------------------------------------------------------------------
// definition of the Frame window ID for the main application.  This id is
// also used to identify resource associated with the frame window.
// --------------------------------------------------------------------------
#define APPLICATION_ID        100

// --------------------------------------------------------------------------
// definition of bitmaps used in the application.                             
// --------------------------------------------------------------------------
#define IDB_PRODINFO           99

// --------------------------------------------------------------------------
// definition of menu item ids.
// --------------------------------------------------------------------------
#define MID_FILE              100
#define MID_FILENEW           101
#define MID_FILEOPEN          102
#define MID_FILESAVE          103
#define MID_FILESAVEAS        104
#define MID_FILEEXIT          199
#define MID_HELP              200
#define MID_HELPGEN           201
#define MID_HELPUSING         202
#define MID_HELPKEYS          203
#define MID_HELPPROD          299

// --------------------------------------------------------------------------
// definition for the application help table
// --------------------------------------------------------------------------
#define APP_HELP_TABLE        1000

// --------------------------------------------------------------------------
// Help panel ids.
// NOTE: These values must match the .nameit definitions in pmed.inc.
// --------------------------------------------------------------------------
#define HID_GENERAL             10
#define HID_FILE_MENU          100
#define HID_FILE_NEW           101
#define HID_FILE_OPEN          102
#define HID_FILE_SAVE          103
#define HID_FILE_SAVEAS        104
#define HID_FILE_EXIT          199
#define HID_HELP_MENU          200
#define HID_HELP_GEN           201
#define HID_HELP_USE           202
#define HID_HELP_KEYS          203
#define HID_HELP_PROD          299
#define HID_KEYS_HELP         5000
#define HID_NOT_SAVED         5001

