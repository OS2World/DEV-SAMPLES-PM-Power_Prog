// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      TUTORIAL.H                                       mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Header file defining the interface to the TUTORIAL            บ
// บ                 control for the Chapter 14 sample program.                    บ
// บ                                                                               บ
// บ  NOTES:         None                                                          บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

// --------------------------------------------------------------------------
// Define a tutorial class name is a constant
// --------------------------------------------------------------------------
#define  WC_TUTORIAL    ("TUTORIAL")

// --------------------------------------------------------------------------
// Define the messages which can be sent to the control
// --------------------------------------------------------------------------
// TM_SETDATA:  set control data.  mp1 = pointer to TUTORCDATA
// --------------------------------------------------------------------------
#define  TM_SETDATA     WM_USER
// --------------------------------------------------------------------------
// TM_AUTOMATE: control automation. mp1 = FALSE - stop automation
//                                        TRUE  - start automation
// --------------------------------------------------------------------------
#define  TM_AUTOMATE    (TM_SETDATA + 1)
// --------------------------------------------------------------------------
// TM_QUERYDATA: query control data. mp1 = pointer to TUTORCDATA
// --------------------------------------------------------------------------
#define  TM_QUERYDATA   (TM_AUTOMATE + 1)
// --------------------------------------------------------------------------
// TM_QUERYSTATE: query automation state.
// --------------------------------------------------------------------------
#define  TM_QUERYSTATE  (TM_QUERYDATA + 1)

// --------------------------------------------------------------------------
// Define the control styles
// --------------------------------------------------------------------------
#define  TS_SPLITHORZ   0x00000001  // two panel window - split horizontally
#define  TS_SPLITVERT   0x00000002  // two panel window - split vertically
#define  TS_PANL1BITMAP 0x00000004  // panel one data is a bitmap handle
#define  TS_PANL2BITMAP 0x00000008  // panel two data is a bitmap handle
#define  TS_AUTO        0x00000010  // enable automation

// --------------------------------------------------------------------------
// Define the WM_CONTROL notification codes
// --------------------------------------------------------------------------
// TN_SETFOCUS: Control window received the focus - mp2 = control hwnd
// --------------------------------------------------------------------------
#define  TN_SETFOCUS    1
// --------------------------------------------------------------------------
// TN_KILLFOCUS: Control window lost the focus - mp2 = control hwnd
// --------------------------------------------------------------------------
#define  TN_KILLFOCUS   2
// --------------------------------------------------------------------------
// TN_CLICKED: Mouse clicked (or automate timer expired)
//             mp2 = direction - TA_BACKWARD - clicked in left half
//                               TA_FORWARD - clicked in right half
// --------------------------------------------------------------------------
#define  TN_CLICKED     3
// --------------------------------------------------------------------------
// TN_ENTER: Mouse double clicked or ENTER or NEWLINE key pressed
//           mp2 = control window handle
// --------------------------------------------------------------------------
#define  TN_ENTER       4

// --------------------------------------------------------------------------
// Direction indicator sent with the TN_CLICKED notification
// --------------------------------------------------------------------------
#define  TA_FORWARD     1
#define  TA_BACKWARD    2

// --------------------------------------------------------------------------
// Define the control data structure
// --------------------------------------------------------------------------
typedef struct __TUTORCDATA__ {
   USHORT      cb;            // size of structure
   PVOID       hWin1Data;     // data to display in panel 1 (left/top)
   PVOID       hWin2Data;     // data to display in panel 2 (right/bottom)
   ULONG       ulTimerMS;     // automation timer interval
} TUTORCDATA, *PTUTORCDATA;

// --------------------------------------------------------------------------
// Prototype for function to register the tutorial window class
// --------------------------------------------------------------------------
BOOL EXPENTRY RegisterTutorialControl(HAB hab);
