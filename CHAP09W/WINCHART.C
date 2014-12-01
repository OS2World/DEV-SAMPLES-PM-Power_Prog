// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      WINCHART.C                                       mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Main Source File for WINCHART                                 บ
// บ                                                                               บ
// บ  NOTES:         This file contains all of the functions and dialog procedures บ
// บ                 that compose the main WINCHART component.                     บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
#include <windows.h>
#include <dde.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "winchart.h"

// --------------------------------------------------------------------------                        
// Global variables.                                                                                 
// --------------------------------------------------------------------------                        
HANDLE   hInst;                   // Application instance handle
HWND     hwndFrame;               // Handle of the main window
UINT     cxClient;                // Window width
UINT     cyClient;                // Window height
WORD     DDE_FORMAT;              // Format for exchanged data
HWND     hwndDDEServer;           // window handle of DDE Server
HANDLE   hMem;                    // Shared memory handle

char     szApplication[64];       // Application Name String
char     szTopic      [64];       // Topic Name String

int      nChartData[12];          // Data points from DDE

COLORREF clrTable[ 6 ] = { RGB( 255,   0,   0 ),
                           RGB( 255, 255,   0 ),
                           RGB(   0, 255,   0 ),
                           RGB(   0, 255, 255 ),
                           RGB(   0,   0, 255 ),
                           RGB( 255,   0, 255 ), };

char     **szMonth[ ] = { "Jan","Feb","Mar","Apr","May","Jun",
                          "Jul","Aug","Sep","Oct","Nov","Dec" };

// --------------------------------------------------------------------------          
// Miscellaneous constant definitions used only in this source module.                 
// --------------------------------------------------------------------------          
#define APPNAME     "PM_Stats"
#define TOPIC       "ChartTopic"
#define APPLICATION "WINCHART"
#define CLASSNAME   "WinChartClass"
#define TITLEBAR    "WINCHART - Sample Charting Program"

// --------------------------------------------------------------------------                
// Function Prototypes                                                                       
// --------------------------------------------------------------------------                
LRESULT CALLBACK   ClientWndProc   (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);              
LRESULT CALLBACK   DrawChartDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT FAR PASCAL ProdInfoDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int PASCAL WinMain (HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
  MSG       msg;
  WNDCLASS  wc;
  UINT      x,y,cx,cy;
  HANDLE    hAccel;

  hInst = hThisInst;

  x  = GetSystemMetrics(SM_CXSCREEN) / 4;
  y  = GetSystemMetrics(SM_CYSCREEN) / 4;
  cx = GetSystemMetrics(SM_CXSCREEN) / 1.5;
  cy = GetSystemMetrics(SM_CYSCREEN) / 2;

  if (!hPrevInst)
   {
    wc.style         = 0;
    wc.lpfnWndProc   = ClientWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInst;
    wc.hIcon         = LoadIcon(hInst, APPLICATION);
    wc.hCursor       = LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = "MainMenu";
    wc.lpszClassName = CLASSNAME;

    if (!RegisterClass(&wc))
       return FALSE;
   }


  if (!(hwndFrame = CreateWindow(CLASSNAME,
                                 TITLEBAR,
                                 WS_OVERLAPPEDWINDOW,
                                 x,
                                 y,
                                 cx,
                                 cy,
                                 (HWND)0,
                                 (HMENU)0,
                                 hInst,
                                 (void *)0)))
                                 return FALSE;

  ShowWindow(hwndFrame, nCmdShow);
  UpdateWindow(hwndFrame);

  hAccel = LoadAccelerators(hInst, APPLICATION);

  while (GetMessage(&msg, NULL, NULL, NULL))
   {
    if (!TranslateAccelerator(hwndFrame, hAccel, &msg))
     {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
     }
   }
 return msg.wParam;
}


// *******************************************************************************
// FUNCTION:     ClientWndProc
//
// FUNCTION USE: Typical client window procedure
//
// DESCRIPTION:  This window procedure processes all messages received by the
//               client window.
//
// PARAMETERS:   HWND     client window handle
//               UINT     window message
//               WPARAM   first message parameter
//               LPARAM   second message parameter
//
// RETURNS:      LRESULT  DefWindowProc for all unprocessed messages
//
// INTERNALS:    NONE
//
// HISTORY:
//
// *******************************************************************************
LRESULT CALLBACK ClientWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   FARPROC       farproc = (FARPROC)0;
   HDC           hdc;
   HPEN          hpen;
   HBRUSH        hbrush;
   PAINTSTRUCT   ps;
   int           nMonth;
   char          szpct[ 10 ];
   RECT          rect;
   LONG          xOrigin;
   LONG          yOrigin;
   LONG          xDelta;
   LONG          yDelta;
   LONG          xLeft;
   LONG          xRight;
   LONG          yTop;
   LONG          yBottom;

   switch (msg)
   {
      case WM_COMMAND:
      {
         switch(wParam)
         {
            case IDM_CHART:
                 farproc = MakeProcInstance((FARPROC)DrawChartDlgProc, hInst);
                 DialogBox(hInst, "IDD_CHART", HWND_DESKTOP, (DLGPROC)farproc);
                 FreeProcInstance(farproc);
                 break;

            case IDM_PRODINFO:
                 farproc = MakeProcInstance((FARPROC)ProdInfoDlgProc, hInst);
                 DialogBox(hInst, "IDD_PRODINFO", hwnd, (DLGPROC)farproc);
                 FreeProcInstance(farproc);
                 break;

            case IDM_EXIT: 
                 PostQuitMessage(FALSE);
                 break;

            default:
                 return DefWindowProc(hwnd, msg, wParam, lParam);
         }
         break;
      }


      case WM_DESTROY:
           PostQuitMessage(0);
           break;


      case WM_SIZE:
           cxClient = LOWORD(lParam);
           cyClient = HIWORD(lParam);
           InvalidateRect(hwnd, NULL, TRUE);
           break;

// -------------------------------------------------------------
// COLOR TABLE
//   Color    RGB Values
//    Red     = 255,   0,  0
//    Blue    =   0,   0,  255
//    Green   =   0, 255,  0
//    Yellow  = 255, 255,  0
//    Magenta = 255,   0,  200
// -------------------------------------------------------------

      case WM_PAINT:
           hdc = BeginPaint(hwnd, &ps);
           hpen = SelectObject (hdc, CreatePen(PS_SOLID, 1, 0L)) ;

           xOrigin = (LONG)cxClient / 10L;
           xDelta = ((LONG)cxClient - (xOrigin * 2L))/12L;
           yOrigin = (LONG)cyClient - ((LONG)cyClient / 10L);
           yDelta = ((LONG)cyClient / 10) * 8L;

           for (nMonth = 0; nMonth < 12; nMonth++) 
            {
              hbrush = CreateSolidBrush(clrTable[nMonth%6]);
              SelectObject(hdc, hbrush);
              xLeft = xOrigin;
              xRight = xOrigin + xDelta;
              yBottom = yOrigin;
              yTop = yOrigin - ((yDelta * nChartData[nMonth]))/100L;
              Rectangle (hdc, (int)xLeft, (int)yTop, (int)xRight, (int)yBottom );
              itoa( nChartData[nMonth], szpct, 10 );
              rect.left = (int)xLeft;
              rect.right = (int)xRight;
              rect.bottom = (int)(yOrigin - yDelta);
              rect.top = rect.bottom - 20;
              DrawText( hdc, szpct, strlen(szpct), &rect, DT_CENTER);
              rect.top = (int)yOrigin + 5;
              rect.bottom = rect.top + 20;
              DrawText( hdc, szMonth[nMonth], 3, &rect, DT_CENTER);
              xOrigin += xDelta;
            } /* endfor */

           EndPaint (hwnd, &ps) ;
           return FALSE;


      default:
        return DefWindowProc(hwnd,msg,wParam,lParam);
   }
 return FALSE;
}



// *******************************************************************************
// FUNCTION:     DrawChartDlgProc
//
// FUNCTION USE: DDE Client Window Procedure
//
// DESCRIPTION:  This window procedure handles the message processing for the
//               DDE client window that will draw our chart data.
//
// PARAMETERS:   HWND     client window handle
//               UINT     window message
//               WPARAM   first message parameter
//               LPARAM   second message parameter
//
// RETURNS:      LRESULT  DefWindowProc for all unprocessed messages
//
// INTERNALS:    NONE
//
// HISTORY:
//
// *******************************************************************************
LRESULT CALLBACK DrawChartDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   DDEACK          ddeack;
   DDEDATA FAR     *pdde;
   HANDLE          hDDEData;
   static ATOM     aItem = 0;
   static ATOM     aAppName;
   static ATOM     aTopic;

   int             rc;
   int             nMonth;
   char FAR        *pszToken;
   WORD   wAck;

   switch (msg)
   {
      case WM_INITDIALOG:
           aAppName = GlobalAddAtom(APPNAME);
           aTopic   = GlobalAddAtom(TOPIC);
           break;

      case WM_DDE_ACK:
           if ((HWND)wParam == hwnd)
            break;

           hwndDDEServer = (HWND)wParam;

           SetDlgItemText(hwnd, ID_STATUSLINE, "Received WM_DDE_INITIATE");

           GlobalGetAtomName((ATOM)LOWORD(lParam), szApplication, sizeof(szApplication));
           SetDlgItemText(hwnd, ID_APPNAME, szApplication);

           GlobalGetAtomName((ATOM)HIWORD(lParam), szTopic, sizeof(szTopic));
           SetDlgItemText(hwnd, ID_TOPICNAME, szTopic);

           // Check to ensure we have the right Application and Topic
           rc = strcmp(APPNAME, szApplication);
           if (rc == 0)
            {
             SetDlgItemText(hwnd, ID_DATA, "Match");
            } /* endif */

           else
            {
             SetDlgItemText(hwnd, ID_DATA, "No Match");
            }

           // Allocate our shared DDE memory.
           if (!hMem)
            {
             hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_DDESHARE, (DWORD)1024UL);
            }

           if (!hMem)
            {
             MessageBox(hwnd,
                        "Error Allocating Memory for DDE conversation.",
                        "WINCHART - Error Display Facility",
                         MB_OK | MB_ICONSTOP);

             EndDialog(hwnd, FALSE);
             return FALSE;
            }
           break;

      case WM_DDE_DATA:
           hDDEData = (HANDLE)LOWORD(lParam);
           aItem = HIWORD(lParam);

           pdde = (DDEDATA FAR *)GlobalLock(hDDEData);
           GlobalUnlock(hDDEData);

           // Received Data from PM, show the data
           SetDlgItemText(hwnd, ID_DATA, pdde->Value);

           // Parse out the fields in the string
           for (nMonth = 0; nMonth < 12; nMonth++) 
            {
              if (nMonth) 
               {
                pszToken = strtok(NULL, ",");                                                                                                                                        
               } 

              else
               {
                pszToken = strtok(pdde->Value, ",");
               } /* endif */

              if (pszToken == NULL) 
               {
                break;
               } 

              nChartData[ nMonth ] = atoi( pszToken );
            } 

           // Now send an acknowledgement back to the PM side
           ddeack.bAppReturnCode = 0;
           ddeack.reserved       = 0;
           ddeack.fBusy          = FALSE;
           ddeack.fAck           = FALSE;

           wAck =  *(WORD *) &ddeack;

           SetDlgItemText(hwnd, ID_STATUSLINE, "Posting WM_DDE_ACK");                               

           // Post a WM_DDE_ACK message back to DDE Server
           PostMessage(wParam,
                       WM_DDE_ACK,        
                       hwnd,
                       MAKELONG(wAck, aItem));

           break;

      case WM_COMMAND:
      {
         switch(wParam)
         {
            case DID_CANCEL:
                 EndDialog(hwnd,FALSE);
                 break;

            case DID_OK:
                 if (aAppName && aTopic)
                  {
                   SendMessage(HWND_BROADCAST,
                               WM_DDE_INITIATE,
                              (WORD)hwnd,
                               MAKELONG(aAppName, aTopic));
                  }
                 break;

            default:
               return (LRESULT)FALSE;
         }
      }

      default:
         return (LRESULT) FALSE;
   }
   return (LRESULT)TRUE;
}



// *******************************************************************************
// FUNCTION:     ProdInfoDlgProc
//
// FUNCTION USE: Simple Product Information Dialog Box
//
// DESCRIPTION:  This dialog procedure handles the message processing for the
//               Product Information Dialog Window.
//
// PARAMETERS:   HWND     client window handle
//               UINT     window message
//               WPARAM   first message parameter
//               LPARAM   second message parameter
//
// RETURNS:      LRESULT  
//
// INTERNALS:    NONE
//
// HISTORY:
//
// *******************************************************************************
LRESULT FAR PASCAL ProdInfoDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
 switch (msg)
  {
    case WM_INITDIALOG:
         return TRUE;

    case WM_COMMAND:
    {
     switch (wParam)
      {
       case DID_OK:
            EndDialog(hwnd, TRUE);
            break;

       case DID_CANCEL:
            EndDialog(hwnd, FALSE);
            break;

       default:
            return FALSE;
            break;
      }
    }

    default:
     return FALSE;
  }
}
