// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      CLKDRAW.FNC                                      mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Global Function Prototype Definition File                     บ
// บ                                                                               บ
// บ  NOTES:         This file contains all of the global function prototype       บ
// บ                 definitions for the CLKDRAW executable.                       บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
int main(VOID);
MRESULT EXPENTRY ClientWndProc    (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY ConfigureDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY ProdInfoDlgProc  (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY ColorDlgProc     (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY LineTypeDlgProc  (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY StatusWndProc    (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

VOID SystemMenu(HWND hwnd);
HBITMAP CreateBitmap(HPS hpsMemory, LONG lcxSize, LONG lcySize,  SHORT sNumColors);    
HBITMAP CopyBitmap (HBITMAP hbmSource);                
HBITMAP GetBitmapFromClipboard(HPS hpsMemory);
VOID ToggleEditMenu(HWND hwndMenu, USHORT usID, HBITMAP hbmClip);
