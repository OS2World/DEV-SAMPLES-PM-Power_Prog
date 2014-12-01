#if !defined __IPFC__
// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      HELPME.H                                         mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Main Header file for HELPME/Chapter 05 Sample Program         บ
// บ                                                                               บ
// บ  NOTES:         This program demonstartes the use of the OS/2 IPF facility.   บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#include <os2.h>

// --------------------------------------------------------------------------
// Presentation Parameters                         
// --------------------------------------------------------------------------
#define PP_PROGTITLE PRESPARAMS PP_FOREGROUNDCOLORINDEX, CLR_RED PRESPARAMS PP_FONTNAMESIZE, "28.Tms Rmn"

// --------------------------------------------------------------------------
// Product Information Bitmap                         
// --------------------------------------------------------------------------
#define IDB_PRODINFO       99

// --------------------------------------------------------------------------
// Window IDs for windows created by the application.
// --------------------------------------------------------------------------
#endif
#define  RID_HELPME        16
#define  RID_UNSUPPORTED   17
#define  RID_FILEOPEN      18
#define  RID_FILESAVEAS    19
#define  RID_SAMPLECHLD    20
#define  RID_SAMPLEDLG     21
#define  RID_SAMPLEMSG     22
#define  RID_SAMPLETHD     23
#define  RID_SAMPLECHLD2   24
#define  RID_PRODINFO      25

#if !defined __IPFC__
// --------------------------------------------------------------------------
// Control Window IDs for the sample dialog window
// --------------------------------------------------------------------------
#endif
#define  DID_CMD_ENTRY     101
#define  DID_HELP          102
#define  DID_PARAMETERS    103

#if !defined __IPFC__
// --------------------------------------------------------------------------
// Menu Item IDs
// --------------------------------------------------------------------------
#endif
#define  MID_BASE       1000
#define  MID_FILE       (MID_BASE + 0)
#define  MID_FILENEW    (MID_BASE + 1)
#define  MID_FILEOPEN   (MID_BASE + 2)
#define  MID_FILESAVE   (MID_BASE + 3)
#define  MID_FILESAVEAS (MID_BASE + 4)
#define  MID_EXIT       (MID_BASE + 5)
#define  MID_HELP       (MID_BASE + 6)
#define  MID_HELPINDEX  (MID_BASE + 7)
#define  MID_GENHELP    (MID_BASE + 8)
#define  MID_USEHELP    (MID_BASE + 9)
#define  MID_KEYSHELP   (MID_BASE + 10)
#define  MID_PRODINFO   (MID_BASE + 11)
#define  MID_SAMPLE     (MID_BASE + 12)
#define  MID_SAMPLECHLD (MID_BASE + 13)
#define  MID_SAMPLEDLG  (MID_BASE + 14)
#define  MID_SAMPLEMSG  (MID_BASE + 15)
#define  MID_SAMPLETHD  (MID_BASE + 16)

#if !defined __IPFC__
// --------------------------------------------------------------------------
// Help Subtable IDs
// --------------------------------------------------------------------------
#endif
#define  HSTID_BASE        1000
#define  HSTID_MAIN        (HSTID_BASE + 0)
#define  HSTID_UNSUPPORTED (HSTID_BASE + 1)
#define  HSTID_OPEN_DLG    (HSTID_BASE + 2)
#define  HSTID_SAVEAS_DLG  (HSTID_BASE + 3)
#define  HSTID_SAMPLECHLD  (HSTID_BASE + 4)
#define  HSTID_SAMPLEDLG   (HSTID_BASE + 5)
#define  HSTID_EMPTY       (HSTID_BASE + 6)

#if !defined __IPFC__
// --------------------------------------------------------------------------
// Help Panels IDs (Explicitly defined since IPFC does not evaluate
// expressions.
// --------------------------------------------------------------------------
#endif
#define  HID_BASE             1000
#define  HID_MAINEXTENDED     1001
#define  HID_FILE             1002
#define  HID_FILENEW          1003
#define  HID_FILEOPEN         1004
#define  HID_FILESAVE         1005
#define  HID_FILESAVEAS       1006
#define  HID_EXIT             1007
#define  HID_SAMPLECHLD       1008
#define  HID_SAMPLEDLG        1009
#define  HID_SAMPLEMSG        1010
#define  HID_SAMPLETHD        1011
#define  HID_HELP             1012
#define  HID_HELPINDEX        1013
#define  HID_GENHELP          1014
#define  HID_USEHELP          1015
#define  HID_KEYSHELP         1016
#define  HID_ABOUT            1017
#define  HID_UNSUPPORTED      1018
#define  HID_FILEOPENDLG      1019
#define  HID_FILESAVEASDLG    1020
#define  HID_SAMPLE           1021
#define  HID_OPEN_FILENAME    1022
#define  HID_OPEN_DRIVE       1023
#define  HID_OPEN_FILTER      1024
#define  HID_OPEN_DIR         1025
#define  HID_OPEN_FILES       1026
#define  HID_OPEN_BUTTONS     1027
#define  HID_SAVEAS_FILENAME  1028
#define  HID_SAVEAS_DRIVE     1029
#define  HID_SAVEAS_FILTER    1030
#define  HID_SAVEAS_DIR       1031
#define  HID_SAVEAS_FILES     1032
#define  HID_SAVEAS_BUTTONS   1033
#define  HID_SAMPLECHILD      1034
#define  HID_CFILE            1035
#define  HID_CFILENEW         1036
#define  HID_CFILEOPEN        1037
#define  HID_CFILESAVE        1038
#define  HID_CFILESAVEAS      1039
#define  HID_CEXIT            1040
#define  HID_CHELP            1041
#define  HID_CHELPINDEX       1042
#define  HID_CGENHELP         1043
#define  HID_CUSEHELP         1044
#define  HID_CKEYSHELP        1045
#define  HID_SAMPLEDLGEXT     1046
#define  HID_CMDENTRY         1047
#define  HID_PARAMETERS       1048
#define  HID_DLGBUTTONS       1049
#define  HID_SAMPLEMSGGEN     1050
#define  HID_DLGOK            1051
#define  HID_KEYSUSAGE        1052
