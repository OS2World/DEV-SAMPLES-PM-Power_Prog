// 浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�
// �  FILENAME:      PMEDIT.FNC                                       mm/dd/yy     �
// �                                                                               �
// �  DESCRIPTION:   Global Function Prototype Definition File                     �
// �                                                                               �
// �  NOTES:         This file contains all of the global function prototype       �
// �                 definitions for the PMEDIT executable.                        �
// �                                                                               �
// �  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        �
// �  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          �
// �                                                                               �
// �  REVISION DATES:  mm/dd/yy  Created this file                                 �
// �                                                                               �
// 藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�

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