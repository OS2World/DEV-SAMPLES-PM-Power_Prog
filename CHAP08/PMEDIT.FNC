// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      PMEDIT.FNC                                       mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Global Function Prototype Definition File                     บ
// บ                                                                               บ
// บ  NOTES:         This file contains all of the global function prototype       บ
// บ                 definitions for the PMEDIT executable.                        บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

// --------------------------------------------------------------------------                 
// Window and Dialog Procedures 
// --------------------------------------------------------------------------                 
MRESULT EXPENTRY ClientWndProc     (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY ProdInfoDlgProc   (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY FileSearchDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

// --------------------------------------------------------------------------            
// Various other function Prototypes                                                         
// --------------------------------------------------------------------------            
BOOL   SetDefaultFont(HWND hwnd);
VOID   ReplaceTheSystemMenu(HWND hwndFrame);
VOID   UpdateStatusWindow(USHORT usStaticID, PCH pchBuffer);
VOID   UpdateTitleBar(HWND hwnd, PCH pchBuffer);                
VOID   ToggleEditMenu(HWND hwndMenu, USHORT usID);                      
APIRET FileOpen(HWND hwndMLE, PSZ pszFileName);                    
VOID   FileNew(HWND hwndMLE, BOOL bUpdateTitle);                                        
APIRET FileSave(HWND hwndMLE);                                       
APIRET FileSaveAs(HWND hwndMLE);                                     
BOOL   SelectFont(HWND hwnd);                                
USHORT FileDialogProc(PSZ szTitleText, PSZ szButtonText, USHORT usDlgType);
