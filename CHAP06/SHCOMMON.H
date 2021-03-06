// 浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�
// �  FILENAME:      SHCOMMON.H                                       mm/dd/yy     �
// �                                                                               �
// �  DESCRIPTION:   Header file for common functions.                             �
// �                                                                               �
// �  NOTES:         This file contains the various resource definitions used      �
// �                 by the COMMON code.                                           �
// �                                                                               �
// �  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        �
// �  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          �
// �                                                                               �
// �  REVISION DATES:  mm/dd/yy  Created this file                                 �
// �                                                                               �
// 藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�


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
#define TITLEBAR      "THREADS - Multiple Thread Sample Program"
