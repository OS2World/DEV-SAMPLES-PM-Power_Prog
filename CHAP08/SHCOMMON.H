// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      SHCOMMON.H                                       mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Header file for common functions.                             บ
// บ                                                                               บ
// บ  NOTES:         This file contains the various resource definitions used      บ
// บ                 by the COMMON code.                                           บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ


VOID CenterDialog(HWND hwnd);                                               
USHORT DisplayMessages(ULONG ulMessageID, PCH pchText, USHORT usMsgType);   
VOID HideControls(HWND hwndFrame);                                          
VOID SetTheSysMenu(HWND hwnd, PCH szCloseItem);                             

#define ID_MESSAGEBOX   999      //  Used for DisplayMessages

#define MSG_ERROR       0x30
#define MSG_INFO        0x40
#define MSG_WARNING     0x50
#define MSG_EXCLAMATION 0x60


// These Fields are to be modified by the user
#define TITLEBAR        "PMEDIT - Text Editor"            
