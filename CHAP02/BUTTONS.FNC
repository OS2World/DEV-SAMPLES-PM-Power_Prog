// 浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�             
// �  FILENAME:      BUTTONS.FNC                                      mm/dd/yy     �             
// �                                                                               �             
// �  DESCRIPTION:   Global Function Prototype Definition File                     �             
// �                                                                               �             
// �  NOTES:         This file contains all of the global function prototype       �             
// �                 definitions for the executable.                               �             
// �                                                                               �             
// �  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        �             
// �  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          �             
// �                                                                               �             
// �  REVISION DATES:  mm/dd/yy  Created this file                                 �             
// �                                                                               �             
// 藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�             

// --------------------------------------------------------------------------           
// Window Procedures                                                                    
// --------------------------------------------------------------------------           
MRESULT EXPENTRY ClientWndProc           (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY ProdInfoDlgProc         (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY ConfigureDlgProc        (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY SetupDlgProc            (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY ViewProgramListDlgProc  (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY CommandButtonDlgProc    (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY ConfigureComSpecDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

// --------------------------------------------------------------------------                 
// Icon Functions                                                                             
// --------------------------------------------------------------------------                 
BOOL UpdateIcon(HWND hwndDlg, ULONG ulIconID, PSZ pszIcon);
BOOL ChangeIcon(HWND hwndDlg, ULONG ulIconID, PSZ pszExecutable, PSZ pszIcon);

// --------------------------------------------------------------------------           
// This function is used to start the programs                                       
// --------------------------------------------------------------------------           
HAPP StartApplication(PCH szPgmName, PCH szComLine, PCH szProgTitle, ULONG ulPgmType);
