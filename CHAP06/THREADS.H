// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      THREADS.H                                        mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Main Header file for THREADS/Chapter 06 Sample Program        บ
// บ                                                                               บ
// บ  NOTES:         This program demonstartes several methods for implmenting     บ
// บ                 long running operations in PM applications.                   บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#define  INCL_PM
#include <os2.h>

// --------------------------------------------------------------------------
// Window IDs for windows created by the application.
// --------------------------------------------------------------------------
#define APPLICATION_ID        100
#define SELECTFILE_ID         101
#define PROGRESS_ID           102
#define ID_PRODINFO           103
#define IDB_PRODINFO          105

// --------------------------------------------------------------------------
// Menu Item IDs
// --------------------------------------------------------------------------
#define MID_FILE              1000
#define MID_FILEINITIALIZE    1001
#define MID_FILEDELETE        1002
#define MID_FILEEXIT          1003

#define MID_MODE              1020
#define MID_MODESINGLE        1021
#define MID_MODEPEEK          1022
#define MID_MODEMULTI         1023
#define MID_MODEOBJ           1024

#define MID_HELP              1040
#define MID_HELPGENERAL       1041
#define MID_HELPUSING         1042
#define MID_HELPPRODINFO      1043

// --------------------------------------------------------------------------
// Control Window IDs for the sample dialog window
// --------------------------------------------------------------------------
#define PROGRESS_ID           102
#define DID_PROGRESS          2000
#define DID_FILENAME          2001

// --------------------------------------------------------------------------
// Help panel IDs
// --------------------------------------------------------------------------
#define HID_GENHELP           100
#define HID_FILE              200
#define HID_FILEINITIALIZE    201
#define HID_FILEDELETE        202
#define HID_FILEEXIT          203
#define HID_MODE              300
#define HID_MODESINGLE        301
#define HID_MODEPEEK          302
#define HID_MODEMULT          303
#define HID_MODEOBJ           304
#define HID_HELP              400
#define HID_HELPGENERAL       401
#define HID_HELPUSING         402
#define HID_HELPPRODINFO      403


// --------------------------------------------------------------------------
// Presentation Parameters
// --------------------------------------------------------------------------
#define PP_PROGTITLE PRESPARAMS PP_FOREGROUNDCOLORINDEX, CLR_RED PRESPARAMS PP_FONTNAMESIZE, "28.Tms Rmn"
