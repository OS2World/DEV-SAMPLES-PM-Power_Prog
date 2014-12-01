// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      PMSTATS.FNC                                      mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Public Header File and Function Prototypes                    บ
// บ                                                                               บ
// บ  NOTES:         This file is global definitions and function prototypes       บ
// บ                 that can be called from the PMSTATS executable.               บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Converted file to new format                      บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ


// Main Function Prototype
int main(void);

// Window Procedures
MRESULT EXPENTRY ClientWndProc        (HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2);                            
MRESULT EXPENTRY ProdInfoDlgProc      (HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2);                            
MRESULT EXPENTRY SpreadSheetDlgProc   (HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2);                            
MRESULT EXPENTRY FilterEntryFieldProc (HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2);                            


VOID SetDefaultFont(HWND hwndMLE);                                                                  
BOOL EXPENTRY FillComboBoxes(HWND hwndCombo);

USHORT SendTextToMLE(PSZ pszInputString);     
PDDESTRUCT EXPENTRY BuildDDEDataStructure(USHORT, PSZ, PVOID, USHORT, USHORT);                      
