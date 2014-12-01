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
#define TITLEBAR        "Message Display Facility"
