// 浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�
// �  FILENAME:      PMSTATS.FNC                                      mm/dd/yy     �
// �                                                                               �
// �  DESCRIPTION:   Public Header File and Function Prototypes                    �
// �                                                                               �
// �  NOTES:         This file is global definitions and function prototypes       �
// �                 that can be called from the PMSTATS executable.               �
// �                                                                               �
// �  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        �
// �  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          �
// �                                                                               �
// �  REVISION DATES:  mm/dd/yy  Converted file to new format                      �
// �                                                                               �
// 藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�


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
