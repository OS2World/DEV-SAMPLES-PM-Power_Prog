// 浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�
// �  FILENAME:      PMSCREEN.FNC                                     mm/dd/yy     �
// �                                                                               �
// �  DESCRIPTION:   Global Function Prototype Definition File                     �
// �                                                                               �
// �  NOTES:         This file contains all of the global function prototype       �
// �                 definitions for the PMSCREEN executable.                      �
// �                                                                               �
// �  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        �
// �  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          �
// �                                                                               �
// �  REVISION DATES:  mm/dd/yy  Created this file                                 �
// �                                                                               �
// 藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�

// --------------------------------------------------------------------------                                     
// Dialog Window Procedures                                                                                 
// --------------------------------------------------------------------------                                     
MRESULT EXPENTRY ClientWndProc       (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY ToggleHotkeyDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY ProdInfoDlgProc     (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY DrawBitmapDlgProc   (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);                  
MRESULT EXPENTRY GetBitmapResDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

// --------------------------------------------------------------------------             
// Functions from PMSCREEN.C                                                               
// --------------------------------------------------------------------------             
VOID DisableThreeFingerSalute(VOID);
BOOL CheckDisableState(VOID);

// --------------------------------------------------------------------------                                  
// Functions from CLIPBRD.C                                                                                   
// --------------------------------------------------------------------------                                  
BOOL    PutBitmapInClipboard(HBITMAP hbmClipboard);                                         
HBITMAP GetBitmapFromClipboard(VOID);                        
HBITMAP DuplicateBitmap (HBITMAP hbmSource);                            
VOID    ToggleEditMenu(HWND hwndMenu, USHORT usID, HBITMAP hbmClip);              
