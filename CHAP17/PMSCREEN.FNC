// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      PMSCREEN.FNC                                     mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Global Function Prototype Definition File                     บ
// บ                                                                               บ
// บ  NOTES:         This file contains all of the global function prototype       บ
// บ                 definitions for the PMSCREEN executable.                      บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

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