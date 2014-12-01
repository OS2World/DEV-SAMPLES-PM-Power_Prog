// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      SHOWOFF.FNC                                      mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Global Function Prototype Definition File                     บ
// บ                                                                               บ
// บ  NOTES:         This file contains all of the global function prototype       บ
// บ                 definitions for the SHOWOFF executable.                       บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

int main(int argc, char *argv[]);
MRESULT EXPENTRY ClientWndProc             (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY ProdInfoDlgProc           (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY ConfigureUserDlgProc      (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY CreatePresentationDlgProc (HWND, ULONG, MPARAM, MPARAM);
HBITMAP EXPENTRY PaintBitmap(HWND hwnd);

HWND    UpdateMenu(HWND hwndOwner, BOOL bStartShow);
ULONG   LoadBitmap(PSZ pszFilename);
BOOL    WinChangeDesktopBitmap(HWND hwndFrame, PUCHAR szBitmap);
VOID    PreviewPresentation( HWND hwndOwner, PSZ pszBitmapFile );
VOID    QueryDeviceInfoStatus(HWND hwndFrame);

BOOL    PutBitmapInClipboard(HBITMAP hbmClipboard);
HBITMAP GetBitmapFromClipboard(HPS hpsSource);
HBITMAP DuplicateBitmap (HBITMAP hbmSource, HPS hpsSource);
VOID    ToggleEditMenu(HWND hwndMenu, USHORT usID, HBITMAP hbmClip); 
