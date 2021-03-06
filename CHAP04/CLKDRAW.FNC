// 浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�
// �  FILENAME:      CLKDRAW.FNC                                      mm/dd/yy     �
// �                                                                               �
// �  DESCRIPTION:   Global Function Prototype Definition File                     �
// �                                                                               �
// �  NOTES:         This file contains all of the global function prototype       �
// �                 definitions for the CLKDRAW executable.                       �
// �                                                                               �
// �  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        �
// �  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          �
// �                                                                               �
// �  REVISION DATES:  mm/dd/yy  Created this file                                 �
// �                                                                               �
// 藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�
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