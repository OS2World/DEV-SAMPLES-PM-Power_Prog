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


// --------------------------------------------------------------------------                          
// Function Prototypes.                                                                                
// --------------------------------------------------------------------------                          
VOID CenterDialog(HWND hwnd);                                               
USHORT DisplayMessages(ULONG ulMessageID, PCH pchText, USHORT usMsgType);   
VOID HideControls(HWND hwndFrame);                                          
VOID SetTheSysMenu(HWND hwnd, PCH szCloseItem);                             

// --------------------------------------------------------------------------                     
// Window Identifier for Message Box in DisplayMessages                         
// --------------------------------------------------------------------------                     
#define ID_MESSAGEBOX   999      

// --------------------------------------------------------------------------     
// Defines for types of message boxes used in DisplayMessages                           
// --------------------------------------------------------------------------     
#define MSG_ERROR       0x30   // Fatal Error Messages
#define MSG_INFO        0x40   // Information Only Messages
#define MSG_WARNING     0x50   // Warning messages that require yes or no response
#define MSG_EXCLAMATION 0x60   // Application errors

// --------------------------------------------------------------------------                 
// These Fields are to be modified by the user
// --------------------------------------------------------------------------                 
#define TITLEBAR        "PMSCREEN - Screen Capture Utility"               
