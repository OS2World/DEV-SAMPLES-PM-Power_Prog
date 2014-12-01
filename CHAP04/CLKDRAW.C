// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      CLKDRAW.C                                        mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Main Source File for CLKDRAW/Chapter 04 Sample Program        บ
// บ                                                                               บ
// บ  NOTES:         This file contains the primary source code for the CLKDRAW    บ
// บ                 sample program.  CLKDRAW(Click Draw) is a simple drawing      บ
// บ                 utility that allows the user to draw simply by moving the     บ
// บ                 mouse pointer around the client window.  CLKDRAW also allows  บ
// บ                 the user to create "spirograph" like images by simply         บ
// บ                 double clicking the right mouse button.  The intent of this   บ 
// บ                 sample program is to demonstrate how to control the mouse     บ
// บ                 pointer, pointer position, and pointer resources.  This       บ
// บ                 code also introduces the clipboard and simple GPI drawing.    บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#define INCL_WIN
#define INCL_GPI
#define INCL_DOSFILEMGR

#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "shcommon.h"                    // Common function prototypes 
#include "clkdraw.h"                     // Resource Identifiers                    
#include "clkdraw.fnc"                   // Function Prototypes                     

// --------------------------------------------------------------------------
// Miscellaneous constant definitions used only in this source module.
// --------------------------------------------------------------------------
#define DOWN         TRUE
#define UP           FALSE
#define CLASSNAME    "ClkDrawClass"                               
#define STATUSCLASS  "StatusClass"

// --------------------------------------------------------------------------           
// For simplicity, let's make these globals, although in the real world                
// we would not!
// --------------------------------------------------------------------------           
HWND     hwndFrame;                        
HWND     hwndClientStatus;                 

// --------------------------------------------------------------------------              
// Global variables.                     
// --------------------------------------------------------------------------              
USHORT   usRotAngle         = 10;
SHORT    sColor             = CLR_BLUE;
BOOL     bUseWndCoordinates = TRUE;     
BOOL     bClear             = FALSE;         
LONG     lLineType          = LINETYPE_SOLID;
HBITMAP  hbmTrack           = NULLHANDLE;           
static   BOOL  bButton1;   

// --------------------------------------------------------------------------                            
// Function Prototypes                                                                                   
// --------------------------------------------------------------------------                            
extern BOOL      PutBitmapInClipboard(HBITMAP hbmClipboard);
extern HBITMAP   DuplicateBitmap (HBITMAP hbmSource);                           
extern SYSVALUES GetSysValues(VOID);              

POINTL NavigateButton(BOOL bUpDown, HWND hwndClient);    
HBITMAP GetBitmapFromTrack (RECTL prclTrack);
RECTL ProcessTrackingRectangle (HPOINTER hptrTrack, HWND hwndClientTrack);
BOOL ForcePointerVisible(HWND hwndStatic); 
BOOL SpiroGraphBox(HWND hwnd, POINTL ptlCurrent, USHORT usRotAngle, SHORT sDrawColor);
BOOL DrawLine(HPS hpsMemory, HPS hpsWindow, POINTL ptlPointerPos); 


// *******************************************************************************                         
// FUNCTION:     main                                                                                    
//                                                                                                      
// FUNCTION USE: main entry point
//
// DESCRIPTION:  This is the main function of the application.  This function                           
//               obtains an anchor block handle, creates the application message                               
//               queue and creates the frame and client windows.  This routine 
//               will subsequently enter the typical message loop which polls 
//               the message queue, getting and dispatching messages until 
//               WinGetMsg returns FALSE indicating a WM_QUIT message was    
//               received, thereby terminating the application.
// 
// PARAMETERS:   VOID     
//
// RETURNS:      int  
//                             
// HISTORY:                                  
//                                    
// *******************************************************************************
int main(VOID)
{
  HAB       hab;
  HMQ       hmq;                               
  QMSG      qmsg;
  BOOL      rc;
  SYSVALUES sysvals;
  RECTL     rcl;
  RGB2      rgb2;
  CHAR      szFont[30];

  LONG      lcx; 
  LONG      lcy;

  HWND      hwndClient;                                     
  HWND      hwndStatic;
  HWND      hwndStatus;

  ULONG     ulFrameFlags  = FCF_STANDARD;                           
  ULONG     ulStatusFlags = FCF_SIZEBORDER | FCF_TITLEBAR | FCF_ICON | FCF_SYSMENU | FCF_MINMAX;       

  // --------------------------------------------------------------------------              
  // Initialize our application with the Presentation Manager                                                                          
  // --------------------------------------------------------------------------              
  hab = WinInitialize(NULLHANDLE);
  if(!hab)
  {
    DosBeep(60, 100);
    exit(TRUE);
  }

  // --------------------------------------------------------------------------                  
  // Create our application message queue                                                    
  // --------------------------------------------------------------------------                  
  hmq = WinCreateMsgQueue(hab, NULLHANDLE);
  if(!hmq)
  {
    DosBeep(60, 100);
    WinTerminate(hab);
    exit(TRUE);
  }

  // --------------------------------------------------------------------------                  
  // Register our window class for our main window.  Note, that we use                      
  // the CS_CLIPCHILDREN class style here, so that we don't paint over our 
  // status window.
  // --------------------------------------------------------------------------                  
  rc = WinRegisterClass(hab,                              // Anchor Block Handle
                        CLASSNAME,                        // Window Class Name            
                        ClientWndProc,                    // Address of window procedure  
                        CS_SIZEREDRAW | CS_CLIPCHILDREN,  // Class styles 
                        0);                               // No extra window words        

  if (!rc)
  {
    DisplayMessages(NULLHANDLE, "Unable to Register Window Class", MSG_ERROR);
    exit(TRUE);
  }


  // --------------------------------------------------------------------------             
  // Create our main frame window.                     
  // --------------------------------------------------------------------------             
  hwndFrame = WinCreateStdWindow(HWND_DESKTOP,   // Desktop window is parent
                                 NULLHANDLE,     // Frame is invisble initially
                                 &ulFrameFlags,  // Frame Control Flags
                                 CLASSNAME,      // Window class name            
                                 TITLEBAR,       // Window Title Bar             
                                 WS_VISIBLE,     // Client style       
                                 NULLHANDLE,     // Resource is in the executable    
                                 ID_FRAME,       // Frame window identifier      
                                 &hwndClient);   // Client window handle         

  if (!hwndFrame)
  {
   DisplayMessages(NULLHANDLE, "Error Creating Frame Window", MSG_ERROR);
   exit(TRUE);
  }

  // --------------------------------------------------------------------------                   
  // Get our system values                                                                      
  // --------------------------------------------------------------------------                   
  sysvals = GetSysValues();            

  // --------------------------------------------------------------------------     
  // Register a private window class for our status window.                       
  // --------------------------------------------------------------------------     
  rc = WinRegisterClass(hab,             
                        STATUSCLASS,       
                        StatusWndProc,   
                        CS_SIZEREDRAW,   
                        0);              

  // --------------------------------------------------------------------------              
  // Create a status window, which is simply a window that will be used                 
  // to display the area that is tracked by the tracking rectangle.                    
  // --------------------------------------------------------------------------              
  hwndStatus = WinCreateStdWindow(hwndClient,         // Desktop window is parent        
                                  NULLHANDLE,         // Frame is invisble initially     
                                  &ulStatusFlags,     // Frame Control Flags             
                                  STATUSCLASS,        // Window class name               
                                  "Status Window",    // Window Title Bar                
                                  NULLHANDLE,         // Client style                    
                                  (HMODULE)NULL,      // Resource is in the executable   
                                  ID_STATUSWND,       // Frame window identifier         
                                  &hwndClientStatus); // Client window handle            


  // --------------------------------------------------------------------------            
  // Set the window position of our status window.  Basically, we will                                                       
  // position the window in the lower left corner of our client window.
  // --------------------------------------------------------------------------            
  WinSetWindowPos(hwndStatus,                       
                  NULLHANDLE,                    
                  0,          
                  0,          
                  sysvals.lcxFullScreen / 4,          
                  sysvals.lcyFullScreen / 3,          
                  SWP_MOVE | SWP_SIZE | SWP_SHOW); 

                                                                                                             
  // --------------------------------------------------------------------------     
  // Create a simple static text window to show the current pointer position        
  // whenever the user double clicks the second mouse button.  We also use
  // this text window to tell the user information regarding pointer 
  // visibility.  The window is initally created at 0,0 with a size of 0,0.
  // The real coordinates come from the processing of the WM_SIZE message for
  // the client window.
  // --------------------------------------------------------------------------     
  hwndStatic = WinCreateWindow (hwndClient,                   // Parent Window Handle            
                                WC_STATIC,                    // Class Name                      
                                "x = 0 y = 0",                // Window Text                     
                                WS_VISIBLE | SS_TEXT,         // Window Styles                   
                                1,                            // Initial X coordinate            
                                0,                            // Initial Y coordinate            
                                0,                            // Horizontal Length of Static Text     
                                0,                            // Vertical Width of Static Text       
                                NULLHANDLE,                   // Owner Window Handle             
                                HWND_BOTTOM,                  // Sibling Window                  
                                ID_STATIC,                    // Resource Identifier             
                                NULL,                         // Control Data             
                                NULL ) ;                      // Presentation Parameters         


  // --------------------------------------------------------------------------                
  // Set the font for the static text window to a courier 8 point font.                  
  // --------------------------------------------------------------------------                
  strcpy(szFont, "8.Courier");                                                    
  WinSetPresParam(hwndStatic, PP_FONTNAMESIZE, sizeof(szFont) + 1, szFont);        


  // --------------------------------------------------------------------------            
  // Set the color of the static text window to a pale gray                                
  // --------------------------------------------------------------------------            
  rgb2.bBlue     = 204;                                             
  rgb2.bGreen    = 204;                                             
  rgb2.bRed      = 204;                                             
  rgb2.fcOptions = 0;                                               
  WinSetPresParam(hwndStatic, PP_BACKGROUNDCOLOR, (ULONG)sizeof(RGB2), (PVOID)&rgb2);                                                                                                                                          


  // --------------------------------------------------------------------------      
  // Position our frame window.              
  // --------------------------------------------------------------------------      
  lcx = sysvals.lcxScreen / 8;                  
  lcy = sysvals.lcyScreen / 4;                  

  WinSetWindowPos(hwndFrame,                                
                  NULLHANDLE,                               
                  lcx,                                      
                  lcy,                                      
                  0 ,                                  
                  0 ,                                  
                  SWP_SHOW | SWP_MOVE);          


  // --------------------------------------------------------------------------
  // NOTE:  This is our standard PM message loop.  The purpose of this loop   
  //        is to get and dispatch messages from the application message queue   
  //        until WinGetMsg returns FALSE, indicating a WM_QUIT message.  In 
  //        which case we will pop out of the loop and destroy our frame window
  //        along with the application message queue then we will terminate our
  //        anchor block removing our ability to call the Presentation Manager.
  // --------------------------------------------------------------------------
  while(WinGetMsg(hab, &qmsg, (HWND)NULL, 0, 0 ))
    WinDispatchMsg(hab, &qmsg);

  // --------------------------------------------------------------------------                                 
  // Let's bail out, we are done!!!                                                                             
  // --------------------------------------------------------------------------                                 
  WinDestroyWindow(hwndFrame);
  WinDestroyMsgQueue(hmq);
  WinTerminate(hab);
  return(FALSE);
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
//               ULONG    window message
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  WinDefWindowProc for all unprocessed messages
//                             
// HISTORY:                                  
//                                    
// *******************************************************************************
MRESULT EXPENTRY ClientWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 HAB                   hab;
 HPS                   hpsWindow;
 SIZEL                 sizel;
 CHAR                  szCoordinates[CCHMAXPATH];
 SYSVALUES             sysvals;
 POINTERINFO           ptrinfo;
 RECTL                 rclTrack;
 HPOINTER              hptrTrack;
 LONG                  lVisible;
 APIRET                rc;

 HBITMAP               hbmTemp;    
 HWND                  hwndStatic;

 static HWND           hwndMenu;
 static HBITMAP        hbmShadow;
 static POINTL         ptlDraw[4];
 static POINTL         ptlCurrent;      
 static RECTL          rcl;
 static HPS            hpsMemory;     
 static HDC            hdcMemory;     


 switch(msg)
 {
  case WM_CREATE:
       // --------------------------------------------------------------------------               
       // First get our system values and initialize presentation space size
       // --------------------------------------------------------------------------               
       sysvals = GetSysValues();              
                                                                                                            
       sizel.cx = 0;                                                                                  
       sizel.cy = 0;                                                                                  

       // --------------------------------------------------------------------------                           
       // Get our anchor block handle                                                                          
       // --------------------------------------------------------------------------                           
       hab = WinQueryAnchorBlock(hwnd);

       // --------------------------------------------------------------------------               
       // Get a memory device context handle           
       // --------------------------------------------------------------------------               
       hdcMemory = DevOpenDC(hab,                         // anchor block handle                      
                             OD_MEMORY,                   // memory device context
                             "*",                         // device information token
                             0L,                          // number of items
                             NULL,                        // pointer to DEVOPENDATA structure
                             NULLHANDLE);                 // compatible hdc

       // --------------------------------------------------------------------------             
       // Create a memory Presentation space           
       // --------------------------------------------------------------------------               
       hpsMemory = GpiCreatePS(hab,                       // anchor block handle                                                                                        
                               hdcMemory,                 // device context handle
                               &sizel,                    // presentation space size
                               PU_PELS | GPIF_DEFAULT |   // Options 
                               GPIT_MICRO | GPIA_ASSOC);   

       // --------------------------------------------------------------------------
       // Create a 256 color bitmap for shadow drawing.  
       // --------------------------------------------------------------------------
       hbmShadow = CreateBitmap(hpsMemory, sysvals.lcxFullScreen, sysvals.lcyFullScreen, 256);          

       // --------------------------------------------------------------------------
       // If we got an error back from GpiCreateBitmap then we will destroy our
       // presentation space, and device context and show the user that an error 
       // has occurred.
       // --------------------------------------------------------------------------
       if (hbmShadow == GPI_ERROR)                                                 
        {                                                                 
         GpiDestroyPS (hpsMemory);                                        
         DevCloseDC (hdcMemory);                                          
         DisplayMessages(NULLHANDLE, "Error Creating Bitmap for Drawing", MSG_ERROR);                                                                   
        }                                                                 
                                                                       

       // --------------------------------------------------------------------------
       // If everything is cool
       // --------------------------------------------------------------------------
       else
        {
         ptlDraw[1].x = sysvals.lcxFullScreen;              
         ptlDraw[1].y = sysvals.lcyFullScreen;              

         GpiSetBitmap (hpsMemory, hbmShadow);                                        
                                                                       
         GpiBitBlt (hpsMemory,    //  Target presentation space handle         
                    NULLHANDLE,   //  Source presentation space handle      
                    2L,           //  Number of Points                      
                    ptlDraw,      //  Array of Points                       
                    ROP_ONE,      //  Mixing function        
                    BBO_OR);      //  Options                               
        }
       return FALSE;                                                             

  case WM_INITMENU:
       switch (SHORT1FROMMP (mp1)) 
        {
         case ID_EDITMENU:
              // --------------------------------------------------------------------------
              // Get the window handle of the menu
              // --------------------------------------------------------------------------
              hwndMenu = HWNDFROMMP(mp2);

              ToggleEditMenu(hwndMenu, IDM_EDITCUT, hbmTrack);
              ToggleEditMenu(hwndMenu, IDM_EDITCOPY, hbmTrack);
              ToggleEditMenu(hwndMenu, IDM_EDITPASTE, hbmTrack);
              return FALSE;
        }
       break;

  case WM_SIZE:
       // --------------------------------------------------------------------------         
       // On a size message get the rectangle coordinates of the client window                                                             
       // and then move and size our static text window.
       // --------------------------------------------------------------------------         
       WinQueryWindowRect(hwnd, &rcl);                                                                                
       WinSetWindowPos(WinWindowFromID(hwnd, ID_STATIC), NULLHANDLE, 1, rcl.yTop - 25, rcl.xRight, 20, SWP_MOVE | SWP_SIZE);   
       break;

  case WM_PAINT:                                                                       
       // --------------------------------------------------------------------------
       // Create our target presentation space
       // --------------------------------------------------------------------------
       hpsWindow = WinBeginPaint(hwnd, NULLHANDLE, &rcl);                                    

       // --------------------------------------------------------------------------                    
       // If this flag is set the user has cleared the screen using the                                
       // clear option.
       // --------------------------------------------------------------------------                    
       if (bClear == TRUE) 
        {
         // --------------------------------------------------------------------------          
         // Fill our PS quickly with the background color                                       
         // --------------------------------------------------------------------------          
         WinFillRect(hpsWindow, &rcl, CLR_BACKGROUND);         
         WinFillRect(hpsMemory, &rcl, CLR_BACKGROUND);         

         // --------------------------------------------------------------------------                
         // Erase our memory PS                                                                       
         // --------------------------------------------------------------------------                
         GpiErase(hpsMemory);
         bClear = FALSE; 
        }

       else
        {
         GpiErase(hpsWindow);
        }

       ptlDraw[0].x = rcl.xLeft;
       ptlDraw[0].y = rcl.yBottom;
       ptlDraw[1].x = rcl.xRight;
       ptlDraw[1].y = rcl.yTop;

       ptlDraw[2].x = rcl.xLeft;     
       ptlDraw[2].y = rcl.yBottom;   

       // --------------------------------------------------------------------------
       // Copy the bitmap in the Memory PS to the Window PS 
       // --------------------------------------------------------------------------
       GpiBitBlt (hpsWindow,     //  Target presentation space handle               
                  hpsMemory,     //  Source presentation space handle             
                  3L,            //  Number of Points                             
                  ptlDraw,       //  Array of Points                              
                  ROP_SRCCOPY,   //  Mixing function (source copy)                
                  BBO_OR);       //  Options                                      

       WinEndPaint(hpsWindow);
       return FALSE;

  case WM_BUTTON1DOWN:                                                                             
       ptlCurrent = NavigateButton(DOWN, hwnd);                                                       
       break;                                                        
                                                                                                 
  case WM_BUTTON1UP:                                                                               
       NavigateButton(UP, hwnd);             
       return FALSE;                                                                                  
                                                                                                 
  case WM_BUTTON2DBLCLK:                                                                   
       // --------------------------------------------------------------------------        
       // If the user double clicks the second mouse button, it means that    
       // they want to draw a spirograph.  The first thing we do, is obtain the 
       // current pointer coordinates.
       // --------------------------------------------------------------------------        
       WinQueryPointerPos(HWND_DESKTOP, &ptlCurrent);                                                

       // --------------------------------------------------------------------------           
       // Check to see which coordinate system the user has indicated they want to  
       // use for drawing.  If bUseWndCoordinates is set to TRUE, then we will use           
       // window coordinates.  In order to get window coordinates from screen                 
       // coordinates, we will have to convert them by using the WinMapWindowPoints
       // API, and passing the current pointer position in ptlCurrent.
       // --------------------------------------------------------------------------           
       if (bUseWndCoordinates == TRUE) 
        {
         WinMapWindowPoints(HWND_DESKTOP, hwnd, &ptlCurrent, 1);
        } 

       // --------------------------------------------------------------------------               
       // Get the window handle of the static window.                                              
       // --------------------------------------------------------------------------               
       hwndStatic = WinWindowFromID(hwnd, ID_STATIC);

       // --------------------------------------------------------------------------       
       // Update the static window with the current pointer coordinates                                    
       // that we will use to draw our spirograph.
       // --------------------------------------------------------------------------       
       sprintf(szCoordinates, "x = %ld y = %ld", ptlCurrent.x, ptlCurrent.y);
       WinSetWindowText(hwndStatic, szCoordinates);

       // --------------------------------------------------------------------------                
       // OK, this is where we make the call to draw the spirograph.  The appearance                                               
       // of the spirograph will change based on our rotation angle(usRotAngle) and
       // the color of the spirograph(sColor).  Both of these values are configured
       // through the Configure Spirographs dialog box.  For the purposes of this 
       // sample program, both of these variables are simply globals.
       // --------------------------------------------------------------------------                
       SpiroGraphBox(hwnd, ptlCurrent, usRotAngle, sColor);                                                              
       break;                                                                                   

  case WM_MOUSEMOVE:                                                                               
       // --------------------------------------------------------------------------                 
       // If the user is not pressing the mouse button 1 down, then we are                     
       // not in drawing mode, so we will break outta here and fall through to 
       // the default WM_MOUSEMOVE processing.
       // --------------------------------------------------------------------------                 
       if (!bButton1) 
        {
         break;                                                                                
        }

       else
        {
         hpsWindow = WinGetPS (hwnd);                                                               

         // --------------------------------------------------------------------------   
         // Do everything in duplication here to both the memory PS                      
         // and the window PS.
         // --------------------------------------------------------------------------   
         GpiSetColor(hpsMemory, sColor);          
         GpiSetColor(hpsWindow, sColor);       
                                                                                                 
         // --------------------------------------------------------------------------      
         // Everytime we pass through this message, move the line position to the          
         // current pointer position.
         // --------------------------------------------------------------------------      
         GpiMove(hpsMemory, &ptlCurrent);                                                       
         GpiMove(hpsWindow, &ptlCurrent);                                                       
                                                                                                 
         // --------------------------------------------------------------------------           
         // Obtain the current pointer position using the MOUSEMSG macro                        
         // --------------------------------------------------------------------------                                                                  
         ptlCurrent.x = MOUSEMSG(&msg)->x;                                                        
         ptlCurrent.y = MOUSEMSG(&msg)->y;                                                       

         hwndStatic = WinWindowFromID(hwnd, ID_STATIC);                          
         sprintf(szCoordinates, "x = %ld y = %ld", ptlCurrent.x, ptlCurrent.y);          
         WinSetWindowText(hwndStatic, szCoordinates);              

         // --------------------------------------------------------------------------  
         // Given the current pointer position lets draw our line               
         // --------------------------------------------------------------------------  
         DrawLine(hpsMemory, hpsWindow, ptlCurrent);

         WinReleasePS (hpsWindow);                                                                  
        }
       break;                                                           

  case WM_COMMAND:
       switch (COMMANDMSG(&msg)->cmd)
        {
         case IDM_PRODINFO:
              WinDlgBox(HWND_DESKTOP, hwnd, ProdInfoDlgProc, NULLHANDLE, IDD_PRODINFO, NULL); 
              return FALSE;

         case IDM_LINETYPE:
              WinDlgBox(HWND_DESKTOP, hwnd, LineTypeDlgProc, NULLHANDLE, IDD_LINETYPE, NULL);
              return FALSE;

         case IDM_CONFIGURE:
              WinDlgBox(HWND_DESKTOP, hwnd, ConfigureDlgProc, NULLHANDLE, IDD_CONFIGURE, NULL); 
              return FALSE;

         case IDM_COLOR:
              WinDlgBox(HWND_DESKTOP, hwnd, ColorDlgProc, NULLHANDLE, IDD_COLOR, NULL); 
              return FALSE;

         case IDM_HIDEMOUSE:        
              hwndStatic = WinWindowFromID(hwnd, ID_STATIC);                  

              // --------------------------------------------------------------------------                          
              // Check our pointer visibility level to determine if the pointer needs                            
              // to be restored or hidden.                                                                           
              // --------------------------------------------------------------------------                          
              lVisible = WinQuerySysValue(HWND_DESKTOP, SV_POINTERLEVEL);    

              // --------------------------------------------------------------------------                 
              // If the pointer visibility level is greater than zero, then our                           
              // pointer is currently not visible, so we will call our ForcePointerVisible           
              // routine to make is visible again.                                                          
              // --------------------------------------------------------------------------                 
              if (lVisible > 0) 
               {
                ForcePointerVisible(hwndStatic);
               }

              else
               {
                // --------------------------------------------------------------------------                
                // We must set the capture window here, otherwise as soon as the                                                  
                // pointer is moved outside the bounds of our window it will become           
                // visible again.  This is where we hide the pointer.                                                             
                // --------------------------------------------------------------------------                
                WinSetCapture (HWND_DESKTOP, hwnd);                                              
                WinShowPointer(HWND_DESKTOP, FALSE); 

                WinSetWindowText(hwndStatic, "Press F8 again to restore pointer visibility");              
               }
              return FALSE;

         case IDM_HELPGEN:          
         case IDM_HELPKEYS:         
              DisplayMessages(NULLHANDLE, "Sorry, no help is available.", MSG_INFO);
              return FALSE;

         case IDM_CLEAR:
              // --------------------------------------------------------------------------        
              // Make a copy of our bitmap so we can clear the window                  
              // --------------------------------------------------------------------------        
              // Reset the memory PS and repaint our client window
              if (hbmShadow) 
               {
                hbmTemp = DuplicateBitmap(hbmShadow);
                if (!hbmTemp) 
                 {
                  DisplayMessages(NULLHANDLE, "Error duplicating bitmap, or no bitmap to duplicate.", MSG_EXCLAMATION);
                 } 
               }

              // --------------------------------------------------------------------------        
              // Delete our shadow bitmap and set the handle to NULL                   
              // --------------------------------------------------------------------------        
              GpiDeleteBitmap(hbmShadow);
              hbmShadow = NULLHANDLE;

              // --------------------------------------------------------------------------             
              // Reset the presentation space with the NULL bitmap                                    
              // --------------------------------------------------------------------------             
              GpiSetBitmap(hpsMemory, hbmShadow);

              bClear = TRUE;

              // --------------------------------------------------------------------------             
              // Paint our client window so our drawing is cleared                                     
              // --------------------------------------------------------------------------             
              WinInvalidateRect(hwnd, NULL, FALSE);                

              // --------------------------------------------------------------------------     
              // Copy our temporary bitmap back into our shadow bitmap handle                              
              // --------------------------------------------------------------------------     
              hbmShadow = hbmTemp;

              // --------------------------------------------------------------------------         
              // Put our bitmap back into our memory presentation space                             
              // --------------------------------------------------------------------------         
              GpiSetBitmap(hpsMemory, hbmShadow);

              // --------------------------------------------------------------------------         
              // Delete our temporary bitmap and set the handle to NULL                        
              // --------------------------------------------------------------------------         
              GpiDeleteBitmap(hbmTemp);                                                           
              hbmTemp = NULLHANDLE;                                                               

              // --------------------------------------------------------------------------                
              // Set the bitmap to NULL so that we can repaint the status window                           
              // --------------------------------------------------------------------------                
              hbmTrack = NULLHANDLE;
              WinInvalidateRect(hwndClientStatus, NULL, TRUE);                 

              // --------------------------------------------------------------------------           
              // reset our static text window back to 0,0
              // --------------------------------------------------------------------------           
              hwndStatic = WinWindowFromID(hwnd, ID_STATIC);                      
              sprintf(szCoordinates, "x = %ld y = %ld", 0, 0);    
              WinSetWindowText(hwndStatic, szCoordinates);                              
              return FALSE;

         case IDM_TRACKBOX:
              // --------------------------------------------------------------------------                             
              // Load our special tracking pointer - it looks like a cross                                         
              // --------------------------------------------------------------------------                             
              hptrTrack = WinLoadPointer(HWND_DESKTOP,                   
                                         NULLHANDLE,                     
                                         IDP_TRACKRECT);                 
                                                                              
              // --------------------------------------------------------------------------               
              // Handle the tracking operation and retrieve the track rectangle 
              // --------------------------------------------------------------------------               
              rclTrack = ProcessTrackingRectangle(hptrTrack, HWND_DESKTOP);            

              if (hbmTrack != NULLHANDLE)                                   
               {
                GpiDeleteBitmap (hbmTrack);                                 
               }                                                               

              // --------------------------------------------------------------------------     
              // Get a bitmap of the area in our tracking rectangle coordinates                          
              // --------------------------------------------------------------------------     
              hbmTrack = GetBitmapFromTrack (rclTrack);                   
                                                                              
              if (hbmTrack == NULLHANDLE)                                  
               {
                DisplayMessages(NULLHANDLE, "Error Creating Bitmap", MSG_EXCLAMATION);         
               }

              WinInvalidateRect (hwndClientStatus, NULL, FALSE);                 
              return FALSE;                                                 


         case IDM_EDITCOPY:
              if (hbmTrack != NULLHANDLE) 
               {

                hbmTemp = DuplicateBitmap(hbmTrack);

                if (!hbmTemp) 
                 {
                  DisplayMessages(NULLHANDLE, "Error duplicating bitmap for copy operation", MSG_EXCLAMATION);
                 }

                else
                 {
                  PutBitmapInClipboard(hbmTemp);
                 }
               }
              return FALSE;

         case IDM_EDITCUT:
              if (hbmTrack != NULLHANDLE) 
               {
                PutBitmapInClipboard(hbmTrack);
                hbmTrack = NULLHANDLE;               
                WinInvalidateRect(hwndClientStatus, NULL, FALSE);      
               }
              return FALSE;

         case IDM_EDITPASTE:                                                      
              // --------------------------------------------------------------------------     
              // Handle the clipboard paste operation by opening the clipboard         
              // and getting the bitmap.  The GetBitmapFromClipboard function will
              // put the clipboard into our memory presentation space.
              // --------------------------------------------------------------------------     
              GetBitmapFromClipboard(hpsMemory);
              WinInvalidateRect (hwnd, NULL, FALSE) ;                         
              return FALSE;                                                      

         case IDM_HOTSPOT:
              // --------------------------------------------------------------------------        
              // For the current desktop pointer, this option will display the pointer's         
              // hotspot.  The first thing we do is get the window handle for the static           
              // text window and the pointer handle.                             
              // --------------------------------------------------------------------------        
              hwndStatic = WinWindowFromID(hwnd, ID_STATIC);                                       
              hptrTrack  = WinQueryPointer(HWND_DESKTOP);                                                
                                                                                                   
              // --------------------------------------------------------------------------     
              // Get the POINTERINFO structure for our pointer.  This structure contains       
              // the hotspot information.
              // --------------------------------------------------------------------------     
              WinQueryPointerInfo(hptrTrack,    // Pointer Handle                                  
                                  &ptrinfo);    // Pointer Information Structure                           
                                                                                                   
              // --------------------------------------------------------------------------                 
              // Format the szCoordinates string with the pointer hotspot information and                  
              // update our static text window.                                                            
              // --------------------------------------------------------------------------                 
              sprintf(szCoordinates,     
                      "The Pointer Hotspot for the current desktop pointer is at x = %ld y = %ld",
                      ptrinfo.xHotspot, ptrinfo.yHotspot);     

              WinSetWindowText(hwndStatic, szCoordinates);                                              
              return FALSE;

         case IDM_EXIT:
              WinPostMsg(hwnd, WM_QUIT, MPFROMLONG(NULL), MPFROMLONG(NULL));
              break;
        }
       break;

  case WM_DESTROY:                           
       GpiDestroyPS(hpsMemory);            
       DevCloseDC(hdcMemory);              
       GpiDeleteBitmap(hbmShadow);               
       return FALSE;                            
 }
 return WinDefWindowProc(hwnd, msg, mp1, mp2);
}


// *******************************************************************************                         
// FUNCTION:     SpiroGraphBox                                                                                    
//                                                                                                      
// FUNCTION USE: Draws SpiroGraph images when double clicked
//
// DESCRIPTION:  This function draws little images that look similar to something                      
//               that might have been created using a spirograph.  The purpose
//               of this function is to demonstrate how to obtain the current 
//               mouse pointer position and use it practically.  This function 
//               also demonstrates the powerful usage of GPI transforms to create
//               the spirographs.
// 
// PARAMETERS:   HWND     client window handle
//               POINTL   current window coordinates to draw transform
//               USHORT   rotation angle
//               SHORT    spirograph color
//
// RETURNS:      FALSE    success
//               TRUE     failure
//                             
// HISTORY:                                  
//                                    
// *******************************************************************************
BOOL SpiroGraphBox(HWND hwnd, POINTL ptlCurrent, USHORT usRotAngle, SHORT sDrawColor)
{
  HPS        hps;
  POINTL     ptlDraw;
  MATRIXLF   matrix;
  USHORT     usCounter;
  USHORT     usXValue;
  USHORT     usYValue;

  // --------------------------------------------------------------------------            
  // Get a cached PS for the window
  // --------------------------------------------------------------------------            
  hps = WinGetPS(hwnd);
             
  // --------------------------------------------------------------------------            
  // Set the color of the spirograph
  // --------------------------------------------------------------------------            
  GpiSetColor(hps, sDrawColor);

  // --------------------------------------------------------------------------            
  // The current pointer coordinates are passed into this function
  // as ptlCurrent.  We will use these pointer coordinates as the
  // anchor points for our box.
  // --------------------------------------------------------------------------            

  // --------------------------------------------------------------------------            
  // Query the current contents of the model transform
  // --------------------------------------------------------------------------            
  GpiQueryModelTransformMatrix(hps, 9L, &matrix);

  // --------------------------------------------------------------------------            
  // Setup our box coordinates based on the pointer position.
  // if they are too small use the default which is 30.
  // --------------------------------------------------------------------------            
  usXValue = ptlCurrent.x;
  usYValue = ptlCurrent.y;

  ptlDraw.x = ptlCurrent.x + (usXValue / 10);
  ptlDraw.y = ptlCurrent.y + (usYValue / 10);

  if (usXValue < 30 || usYValue < 30)
   {
    ptlDraw.x = ptlCurrent.x + 30;
    ptlDraw.y = ptlCurrent.y + 30;
   }

  // --------------------------------------------------------------------------            
  // The secret of creating the spirographs is simple.  We simply
  // start by drawing a series of boxes all the way around a circle
  // each time through the loop we will rotate an extra x degrees.
  // Where x is the value of the rotational angle that the user 
  // specifies via usRotAngle.  All the while, we will be replacing
  // our transform along the way with the newly calculated transform.
  // --------------------------------------------------------------------------            
  for (usCounter = 0; usCounter < 360; usCounter += usRotAngle)
  {
    GpiRotate(hps,                      // Handle to Presentation Space
              &matrix,                  // Transform matrix
              TRANSFORM_REPLACE,        // Transform options
              MAKEFIXED(usCounter, 0),  // Rotation angle
              &ptlCurrent);             // POINTL coordinates for center of rotation

    GpiSetModelTransformMatrix(hps, 9L, &matrix, TRANSFORM_REPLACE);

    GpiSetCurrentPosition(hps, &ptlCurrent);

    // --------------------------------------------------------------------------                         
    // Draw a normal box based on the ptlDraw coordinates that are obtained.
    // The logic that handles the box drawing is totally unaware that the 
    // transform will cause the box to be rotated.
    // --------------------------------------------------------------------------                         
    GpiBox(hps,                         // Handle to Presentation Space
           DRO_OUTLINE,                 // Outline and Fill control
           &ptlDraw,                    // POINTL coordinates for box
           0,                           // Horizontal corner rounding
           0);                          // Vertical corner rounding
  }

  WinReleasePS(hps);
  return FALSE;
}


// *******************************************************************************        
// FUNCTION:     ForcePointerVisible                                                            
//                                                                                        
// FUNCTION USE: Shows or hides the mouse pointer                              
//                                                                                        
// DESCRIPTION:  This function is used to show or hide the mouse pointer while it        
//               is over the client window.  It first validates that a mouse              
//               is currently installed and then continues to alter the pointer           
//               visibility usage count until the specified mouse pointer is              
//               either made visible or invisible.                                        
//                                                                                        
// PARAMETERS:   HWND     static window handle                                        
//                                                                                        
// RETURNS:      TRUE     success                                                                
//               FALSE    no pointing device installed
//                                                                                        
// HISTORY:                                                                               
//                                                                                        
// *******************************************************************************        
BOOL ForcePointerVisible(HWND hwndStatic) 
{                                                
  LONG     lSysVal;                                
  USHORT   usCounter;

  // --------------------------------------------------------------------------                    
  // Do we have a pointing device installed.                                                       
  // --------------------------------------------------------------------------                    
  lSysVal = WinQuerySysValue(HWND_DESKTOP, SV_MOUSEPRESENT);

  if (!lSysVal) 
   {
    // --------------------------------------------------------------------------                    
    // If no pointing device is installed show the user a message and                                
    // return FALSE back to the caller.
    // --------------------------------------------------------------------------                    
    DosBeep(1000, 1000);
    DisplayMessages(NULLHANDLE, "No pointing device installed", MSG_EXCLAMATION);
    return FALSE;
   } 

  // --------------------------------------------------------------------------              
  // Check the pointer visibility level.                                                     
  // --------------------------------------------------------------------------              
  lSysVal = WinQuerySysValue(HWND_DESKTOP, SV_POINTERLEVEL);

  // --------------------------------------------------------------------------              
  // Decrement our pointer usage count until we're back to zero.                        
  // --------------------------------------------------------------------------              
  for (usCounter = 0; usCounter < lSysVal; usCounter++)
   {
    WinShowPointer(HWND_DESKTOP, TRUE);    
   } 

  // --------------------------------------------------------------------------              
  // Update the static text window to tell the user the pointer is visible,      
  // although they should have figured it out.....
  // --------------------------------------------------------------------------              
  WinSetWindowText(hwndStatic, "Pointer Visibility Restored");           
  return TRUE;
}


// *******************************************************************************        
// FUNCTION:     ConfigureDlgProc                                                          
//                                                                                        
// FUNCTION USE: Allows the user to configure options                          
//                                                                                        
// DESCRIPTION:  This function is used to allow the user the opportunity to              
//               configure their preferences like spirograph          
//               rotational angle.
//                                                                                        
// PARAMETERS:   HWND     window handle 
//               ULONG    message to be processed
//               MPARAM   first message parameter mp1
//               MPARAM   second message parameter mp2
//                                                                           
//
// RETURNS:      MRESULT                                                                    
//                                                                                        
// HISTORY:                                                                               
//                                                                                        
// *******************************************************************************        
MRESULT EXPENTRY ConfigureDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   static  HWND    hwndSpin;
   static  HWND    hwndStatic;

   PVOID           pvBuffer;
   CHAR            szBuffer[25];

   switch (msg)
   {
      case WM_INITDLG:
           // --------------------------------------------------------------------------               
           // Remove the items not needed in the system menu and check the                         
           // appropriate radiobutton, representing the coordinate system that 
           // the user has selected.  The coordinate system is controlled through
           // the use of the bUseWndCoordinates global variable.  The default 
           // coordinate system for drawing is window coordinates rather than 
           // screen coordinates.
           // --------------------------------------------------------------------------               
           SetTheSysMenu(hwnd, NULLHANDLE);
           WinCheckButton(hwnd, bUseWndCoordinates ? IDR_WINCOOR : IDR_SCRCOOR, TRUE);          

           // --------------------------------------------------------------------------     
           // We use a spinbutton to control the rotation angle of the spirographs.  
           // The upper limit of the rotation angle is 99, while the lower limit is 1.       
           // Also, set the default value to the default rotation angle, which is 10.      
           // Finally set the text limit of the spinbutton to 2 characters.              
           // --------------------------------------------------------------------------     
           // Set the SpinButton Values
           hwndSpin = WinWindowFromID(hwnd, ID_SPINBUTTON);                                           
           WinSendMsg(hwndSpin, SPBM_SETLIMITS,       (MPARAM)99, (MPARAM)1);
           WinSendMsg(hwndSpin, SPBM_SETCURRENTVALUE, (MPARAM)10, (MPARAM)NULL); 
           WinSendMsg(hwndSpin, SPBM_SETTEXTLIMIT,    (MPARAM)2,  (MPARAM)NULL); 
           return FALSE;

      case WM_CONTROL:                                
           switch (SHORT1FROMMP (mp1))                
           {                                          
            case IDR_WINCOOR:                            
                 bUseWndCoordinates = TRUE;  // If True use window coordinates   
                 return FALSE;                        
                                                      
            case IDR_SCRCOOR:                             
                 bUseWndCoordinates = FALSE; // If False use screen coordinates                   
                 break;                               
           }                                           

      case WM_COMMAND:
           switch (COMMANDMSG(&msg)->cmd)
           {
            case DID_HELP:
                 DisplayMessages(NULLHANDLE, HELP_TEXT, MSG_INFO);
                 return FALSE;

            case DID_OK:
                 WinSendMsg(hwndSpin, SPBM_QUERYVALUE, &pvBuffer, MPFROM2SHORT(0, 0)); 
                 usRotAngle = (ULONG)pvBuffer;
                 WinDismissDlg(hwnd,TRUE);
                 return FALSE;
           }
   }
 return WinDefDlgProc(hwnd, msg, mp1, mp2);
}                                                                              



// *******************************************************************************                         
// FUNCTION:     ProcessTrackingRectangle                                                                         
//                                                                                                      
// FUNCTION USE: Used for the Capture Rectangle Menu Option     
//
// DESCRIPTION:  This function is used to create the tracking rectangle that 
//               will be used to create the capture rectangle that is used to 
//               capture a portion of any window on the desktop to the capture 
//               window.  Once there is data in the capture window the user can 
//               copy the contents of the capture window to the PM Clipboard.                       
// 
// PARAMETERS:   HPOINTER  the pointer to be used for the tracking rectangle
//               HWND      the window that can be tracked
//
// RETURNS:      RECTL     the final track rectangle
//                             
// HISTORY:                                  
//                                    
// *******************************************************************************
RECTL ProcessTrackingRectangle (HPOINTER hptrTrack, HWND hwndClientTracking)
{
  RECTL       rclTemp;
  POINTL      ptlTemp;
  TRACKINFO   trackinfo;
  SYSVALUES   sysvals;
  HWND        hwndClient;
  HWND        hwndStatic;


  // --------------------------------------------------------------------------           
  // Get our client window handle so that we can get our static text                 
  // window handle.
  // --------------------------------------------------------------------------           
  hwndClient = WinWindowFromID(hwndFrame, FID_CLIENT);            
  hwndStatic = WinWindowFromID(hwndClient, ID_STATIC);            

  // --------------------------------------------------------------------------                 
  // Tell the user we are starting the track operation.                                
  // --------------------------------------------------------------------------                 
  WinSetWindowText(hwndStatic, "Starting Track Operation");                              

  // --------------------------------------------------------------------------                   
  // Set our pointer for tracking                        
  // --------------------------------------------------------------------------                   
  if (hptrTrack) 
   {
    WinSetPointer(HWND_DESKTOP, hptrTrack);
   } 

  // --------------------------------------------------------------------------                        
  // Get our system values that we need to fill in our TRACKINFO structure                   
  // --------------------------------------------------------------------------                        
  sysvals = GetSysValues();

  // --------------------------------------------------------------------------         
  // Setup our track info structure                                                     
  // --------------------------------------------------------------------------         
  trackinfo.cxBorder            = sysvals.lcxDlgFrame;      // Horizontal tracking rectangle border
  trackinfo.cyBorder            = sysvals.lcyDlgFrame;      // Vertical tracking rectangle border
  trackinfo.cxGrid              = 0;                        // Horizontal Grid 
  trackinfo.cyGrid              = 0;                        // Vertical Grid
  trackinfo.cxKeyboard          = 4;                        // X pixel movement by keyboard
  trackinfo.cyKeyboard          = 4;                        // y pixel movement by keyboard
  trackinfo.rclBoundary.xLeft   = 0;                        // Tracking rectangle boundary
  trackinfo.rclBoundary.yBottom = 0;                        // Bounding rectangle
  trackinfo.rclBoundary.xRight  = sysvals.lcxScreen;
  trackinfo.rclBoundary.yTop    = sysvals.lcyScreen;
  trackinfo.ptlMinTrackSize.x   = 1;                        // Minimum rectangle width
  trackinfo.ptlMinTrackSize.y   = 1;                        // Minimum rectangle height
  trackinfo.ptlMaxTrackSize.x   = sysvals.lcxScreen;        // Maximum rectangle width
  trackinfo.ptlMaxTrackSize.y   = sysvals.lcyScreen;        // Maximum rectangle height

  // --------------------------------------------------------------------------                           
  // Obtain current pointer coordinates                                                                                      
  // --------------------------------------------------------------------------                           
  WinQueryPointerPos(HWND_DESKTOP, &ptlTemp);               

  // --------------------------------------------------------------------------      
  // Setup our track info structure
  // --------------------------------------------------------------------------      
  trackinfo.rclTrack.xLeft      = ptlTemp.x - sysvals.lcxPointer; 
  trackinfo.rclTrack.yBottom    = ptlTemp.y - sysvals.lcyPointer; 
  trackinfo.rclTrack.xRight     = ptlTemp.x + sysvals.lcxPointer; 
  trackinfo.rclTrack.yTop       = ptlTemp.y + sysvals.lcyPointer; 
  trackinfo.fs                  = TF_MOVE | TF_STANDARD | TF_SETPOINTERPOS;     

  // --------------------------------------------------------------------------                      
  // Call WinTrackRect to handle the move operation of the track rectangle                         
  // --------------------------------------------------------------------------                      
  WinTrackRect(hwndClientTracking,   // Track over entire screen
               NULLHANDLE,           // Presentation Space Handle
               &trackinfo);          // Track Information

  // --------------------------------------------------------------------------      
  // Change our tracking flags
  // --------------------------------------------------------------------------      
  trackinfo.fs = TF_RIGHT | TF_BOTTOM | TF_STANDARD | TF_SETPOINTERPOS;

  // --------------------------------------------------------------------------             
  // Call WinTrackRect to handle the sizing operation of the track rectangle                                                    
  // --------------------------------------------------------------------------             
  WinTrackRect(hwndClientTracking,   // Track over entire screen
               NULLHANDLE,           // Presentation Space Handle
               &trackinfo);          // Track Information

  // --------------------------------------------------------------------------                         
  // Get our tracking rectangle                                                               
  // --------------------------------------------------------------------------                         
  rclTemp = trackinfo.rclTrack;      

  // --------------------------------------------------------------------------                         
  // Tell the user that the tracking operation is complete                                             
  // --------------------------------------------------------------------------                         
  WinSetWindowText(hwndStatic, "Tracking Complete");                              

  // --------------------------------------------------------------------------           
  // Return our rectangle                                                               
  // --------------------------------------------------------------------------           
  return rclTemp;
}


// *******************************************************************************                     
// FUNCTION:     StatusWndProc                                                                      
//                                                                                                     
// FUNCTION USE: Window Procedure used by the Status window                                              
//                                                                                                     
// DESCRIPTION:  This procedure handles the message processing for the status                          
//               window.  The status window is used for displaying the selected                 
//               rectangle captured via the tracking rectangle.                                     
//                                                                                                     
// PARAMETERS:   HWND     window handle                                                                
//               ULONG    message to be processed                                                      
//               MPARAM   first message parameter mp1                                                  
//               MPARAM   second message parameter mp2                                                 
//                                                                                                     
//                                                                                                     
// RETURNS:      MRESULT                                                                               
//                                                                                                     
// HISTORY:                                                                                            
//                                                                                                     
// *******************************************************************************                     
MRESULT EXPENTRY StatusWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 HPS    hps;
 LONG   lcxSize;              
 LONG   lcySize;
 RECTL  rclClientStatus;

 switch(msg)
 {
  case WM_CREATE:                    
       // --------------------------------------------------------------------------             
       // We use this special routine to fixup the system menu menuitems.                        
       // --------------------------------------------------------------------------             
       SystemMenu(hwnd);
       return FALSE;

  case WM_CLOSE:
       // --------------------------------------------------------------------------               
       // If we get a close message, ignore the close and just minimize the window.                
       // --------------------------------------------------------------------------               
       WinSetWindowPos(WinQueryWindow(hwnd, QW_PARENT),                                                     
                       NULLHANDLE,                                                      
                       0,                                                             
                       0,                                                             
                       0,                                     
                       0,                                     
                       SWP_MINIMIZE);                  
       return FALSE;

  case WM_PAINT:                                                                 
       hps = WinBeginPaint(hwnd, NULLHANDLE, NULL );                            
       GpiErase(hps);                                                            

       if (hbmTrack != NULLHANDLE)                                       
        {                                                       
         // --------------------------------------------------------------------------                   
         // Get the rectangle coordinates for this client window.                                                                        
         // --------------------------------------------------------------------------                   
         WinQueryWindowRect (hwnd, &rclClientStatus);                 

         // --------------------------------------------------------------------------    
         // For emphasis, we fill the rest of the status window with a pale gray color
         // --------------------------------------------------------------------------    
         WinFillRect(hps, &rclClientStatus, CLR_PALEGRAY);  

         // --------------------------------------------------------------------------       
         // Draw our bitmap.   
         // --------------------------------------------------------------------------       
         WinDrawBitmap (hps,    
                        hbmTrack,
                        NULL,
                        (PPOINTL)&rclClientStatus,     
                        CLR_NEUTRAL,              
                        CLR_BACKGROUND,              
                        DBM_NORMAL);                           
        }                                                       
       WinEndPaint (hps) ;                                          
       return FALSE;                                                   
 }
 return WinDefWindowProc(hwnd, msg, mp1, mp2);
}


// *******************************************************************************        
// FUNCTION:     ColorDlgProc                                                          
//                                                                                        
// FUNCTION USE: Dialog Procedure used for changing draw colors                
//                                                                                        
// DESCRIPTION:  This dialog procedure uses radiobuttons to allow the user to            
//               select the foreground color that will be used for drawing 
//                                                                                        
// PARAMETERS:   HWND     client window handle                                    
//               ULONG    window message                                                                                                 
//               MPARAM   first message parameter                 
//               MPARAM   second message parameter                                                                                  
// 
// RETURNS:      MRESULT                                                                     
//                                                                                        
// HISTORY:                                                                               
//                                                                                        
// *******************************************************************************        
MRESULT EXPENTRY ColorDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 switch (msg)
 {
    case WM_INITDLG:
         SetTheSysMenu(hwnd, NULLHANDLE);
         WinCheckButton(hwnd, sColor, TRUE);          
         return FALSE;

    case WM_CONTROL:
         switch VALUE              
         {
          case CLR_BLUE:
          case CLR_RED:        
          case CLR_PINK:       
          case CLR_GREEN:      
          case CLR_CYAN:       
          case CLR_YELLOW:     
          case CLR_DARKBLUE:   
          case CLR_DARKRED:    
          case CLR_DARKPINK:   
          case CLR_DARKGREEN:  
          case CLR_DARKCYAN:   
          case CLR_BROWN:      
               sColor = VALUE;                                 
               return FALSE;                                               
         }

    case WM_COMMAND:
         switch (COMMANDMSG(&msg)->cmd)
         {
          case DID_OK:
               WinDismissDlg(hwnd, TRUE);
               return FALSE;

          case DID_HELP:
               DisplayMessages(NULLHANDLE,                
                               "Select the radiobutton that represents the drawing color you want to use, and then select OK.\n",
                               MSG_INFO);
               return FALSE;
         }
 }
 return WinDefDlgProc(hwnd, msg, mp1, mp2);
}                                                                              

// *******************************************************************************        
// FUNCTION:     ProdInfoDlgProc                                                       
//                                                                                        
// FUNCTION USE: Dialog Procedure used to display Product Information Dialog   
//                                                                                        
// DESCRIPTION:  This dialog procedure is used to process the simple product             
//               information dialog box.                                     
//                                                                                        
// PARAMETERS:   HWND     client window handle                                    
//               ULONG    window message                                                                                                 
//               MPARAM   first message parameter                 
//               MPARAM   second message parameter                                                                                  
// 
// RETURNS:      MRESULT                                                                     
//                                                                                        
// HISTORY:                                                                               
//                                                                                        
// *******************************************************************************        
MRESULT EXPENTRY ProdInfoDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 switch (msg)
  {
   case WM_INITDLG:
        SetTheSysMenu(hwnd, NULL);
        return FALSE;

   case WM_COMMAND:
        switch (COMMANDMSG(&msg)->cmd)
         {
          case DID_OK:
          case DID_CANCEL:
               WinDismissDlg(hwnd, TRUE);
               return FALSE;
         }
        break;
  }
 return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
}


// *******************************************************************************          
// FUNCTION:     DrawLine                                                                   
//                                                                                          
// FUNCTION USE: Actually draws a line based on pointer coordinates                     
//                                                                                          
// DESCRIPTION:  This function uses the GpiLine API to draw a line.  The function          
//               also sets the line width and the line type (based on a global           
//               variable).  The lines are drawn on two different presentation 
//               spaces.
//                                                                                          
// PARAMETERS:   HPS      memory presentation space handle.                                 
//               HPS      window presentation space handle.                               
//               POINTL   pointer position used to draw line.                          
//                                                                                          
// RETURNS:      FALSE    failure.                                                          
//               TRUE     success.
//                                                                                          
// HISTORY:                                                                                 
//                                                                                          
// *******************************************************************************          
BOOL DrawLine(HPS hpsMemory, HPS hpsWindow, POINTL ptlCurrent) 
{
 BOOL rc;

 // --------------------------------------------------------------------------                               
 // Set the line width to thick(it's a little easier to see, but not                                                                                        
 // too thick.
 // --------------------------------------------------------------------------                               
 GpiSetLineWidth(hpsMemory, LINEWIDTH_THICK);                                   
 GpiSetLineWidth(hpsWindow, LINEWIDTH_THICK);                                   
                                                                                     
 // --------------------------------------------------------------------------                               
 // Set the type of line to whatever the user selected in the LineType                                                                                        
 // dialog box.  The lLineType variable is a global variable that is 
 // set by the LineType dialog.  
 // --------------------------------------------------------------------------                               
 GpiSetLineType(hpsMemory, lLineType);                                             
 GpiSetLineType(hpsWindow, lLineType);                                             
                                                                                     
 // --------------------------------------------------------------------------                                 
 // Draw our line in both the memory and screen presentation spaces.  If                                    
 // we encounter an error return FALSE (GPI_ERROR is FALSE) to the caller.
 // Otherwise this call will return TRUE if successful.
 // --------------------------------------------------------------------------                                 
 rc = GpiLine (hpsMemory, &ptlCurrent);                                              
 if (rc == GPI_ERROR)
  {
   return FALSE;
  }

 rc = GpiLine (hpsWindow, &ptlCurrent);                                              
 if (rc == GPI_ERROR) 
  {
   return FALSE;
  } 

 return TRUE;
}

// *******************************************************************************        
// FUNCTION:     LineTypeDlgProc                                                          
//                                                                                        
// FUNCTION USE: Dialog Procedure used for changing draw colors                
//                                                                                        
// DESCRIPTION:  This dialog procedure uses radiobuttons to allow the user to            
//               select the foreground color that will be used for drawing 
//                                                                                        
// PARAMETERS:   HWND     client window handle                                    
//               ULONG    window message                                                                                                 
//               MPARAM   first message parameter                 
//               MPARAM   second message parameter                                                                                  
// 
// RETURNS:      MRESULT                                                                     
//                                                                                        
// HISTORY:                                                                               
//                                                                                        
// *******************************************************************************        
MRESULT EXPENTRY LineTypeDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
      case WM_INITDLG:
           SetTheSysMenu(hwnd, NULLHANDLE);
           WinCheckButton(hwnd, lLineType, TRUE);          
           return FALSE;

      case WM_CONTROL:
           switch VALUE              
           {
            case LINETYPE_DOT:          
            case LINETYPE_SHORTDASH:            
            case LINETYPE_DASHDOT:             
            case LINETYPE_DOUBLEDOT:          
            case LINETYPE_LONGDASH:           
            case LINETYPE_DASHDOUBLEDOT:    
            case LINETYPE_SOLID:          
            case LINETYPE_INVISIBLE:       
                 lLineType = VALUE;                                 
                 return FALSE;                                               
           }

      case WM_COMMAND:
           switch (COMMANDMSG(&msg)->cmd)
           {
            case DID_OK:
                 WinDismissDlg(hwnd, TRUE);
                 return FALSE;

            case DID_HELP:
                 DisplayMessages(NULLHANDLE,                
                                 "Select the radiobutton that represents the line type you want to use, and then select OK.\n",
                                 MSG_INFO);
                 return FALSE;                                                              
           }
   }
 return WinDefDlgProc(hwnd, msg, mp1, mp2);
}                                                                              

// *******************************************************************************                 
// FUNCTION:     NavigateButton                                                                   
//                                                                                                 
// FUNCTION USE: Used during mouse movement processing                                           
//                                                                                                 
// DESCRIPTION:  This routine is used to handle what happens when                       
//               the user presses the first mouse button for drawing.  If the                         
//               user presses the button down, we set the capture window 
//               for drawing.  If the button is up we reset the capture window.
//                                                                                                 
// PARAMETERS:   BOOL     button press (up or down)                                                        
//               HWND     client window handle                                              
//                                                                                                 
// RETURNS:      POINTL   on a down press the function returns a valid                                                                          
//                        POINTL structure containing the current window 
//                        coordinates.
//
//               NULL     otherwise
//                                                                                                 
// HISTORY:                                                                                        
//                                                                                                 
// *******************************************************************************                 
POINTL NavigateButton(BOOL bDirection, HWND hwndClient)
{
 POINTL ptlCurrent;


 // --------------------------------------------------------------------------    
 // If the direction is down, then set our capture window to the client        
 // window, and get the current pointer position.  We need to ensure that 
 // the pointer position is in window coordinates, so we will need to call
 // WinMapWindowPoints to convert from screen coordinates to window 
 // coordinates.  We will return the window coordinates back to the caller.
 // --------------------------------------------------------------------------    
 if (bDirection == DOWN)
  {
   WinSetCapture(HWND_DESKTOP, hwndClient);
   WinQueryPointerPos(HWND_DESKTOP, &ptlCurrent);                  
   WinMapWindowPoints(HWND_DESKTOP, hwndClient, &ptlCurrent, 1);   

   bButton1 = DOWN;
   return ptlCurrent;
  }

 // --------------------------------------------------------------------------    
 // If the direction is up, then reset the capture window and return.             
 // --------------------------------------------------------------------------    
 else
  {
   WinSetCapture (HWND_DESKTOP, NULLHANDLE);
   bButton1 = UP;
   return;
  }
}
