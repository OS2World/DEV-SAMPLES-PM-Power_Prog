// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป             
// บ  FILENAME:      BUTTONS.FNC                                      mm/dd/yy     บ             
// บ                                                                               บ             
// บ  DESCRIPTION:   Global Function Prototype Definition File                     บ             
// บ                                                                               บ             
// บ  NOTES:         This file contains all of the global function prototype       บ             
// บ                 definitions for the executable.                               บ             
// บ                                                                               บ             
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ             
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ             
// บ                                                                               บ             
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ             
// บ                                                                               บ             
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ             

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