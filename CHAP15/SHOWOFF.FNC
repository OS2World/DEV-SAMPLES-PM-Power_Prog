// 浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�
// �  FILENAME:      SHOWOFF.FNC                                      mm/dd/yy     �
// �                                                                               �
// �  DESCRIPTION:   Global Function Prototype Definition File                     �
// �                                                                               �
// �  NOTES:         This file contains all of the global function prototype       �
// �                 definitions for the SHOWOFF executable.                       �
// �                                                                               �
// �  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        �
// �  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          �
// �                                                                               �
// �  REVISION DATES:  mm/dd/yy  Created this file                                 �
// �                                                                               �
// 藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�

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