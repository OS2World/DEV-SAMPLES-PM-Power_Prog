// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      PMSTATS.C                                        mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Primary Source File for PMSTATS DDE Server Code               บ
// บ                                                                               บ
// บ  NOTES:         This file contains the source code for the main PMSTATS       บ
// บ                 executable.  PMSTATS is a sample DDE server, which is         บ
// บ                 designed to communicate with a DDE client program running     บ
// บ                 under the WINOS2 environment.  This file initializes PM and   บ
// บ                 handles the primary frame and client windows.                 บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#define INCL_WINWINDOWMGR        
#define INCL_WINMESSAGEMGR       
#define INCL_WININPUT            
#define INCL_WINDIALOGS          
#define INCL_WINFRAMEMGR         
#define INCL_WINSYS              
#define INCL_WINDDE              
#define INCL_WINSHELLDATA        
#define INCL_WINHELP             
#define INCL_WINENTRYFIELDS   
#define INCL_WINMLE
#define INCL_WINLISTBOXES

#define INCL_GPICONTROL          
#define INCL_GPILCIDS            
#define INCL_DOSPROCESS          
#define INCL_DOSMEMMGR           

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>                         
#include "pmstats.h"
#include "errors.h"
#include "shcommon.h"
#include "pmstats.fnc"

// --------------------------------------------------------------------------    
// Miscellaneous constant definitions used only in this source module.           
// --------------------------------------------------------------------------    
#define APPNAME   "PM_Stats"    
#define CLASSNAME "PMStatsClass"
#define WKEYNAME  "WindowPos"
#define TOPIC     "ChartTopic"

// --------------------------------------------------------------------------              
// For simplicity make these global, although we would never really do this.          
// --------------------------------------------------------------------------              
HWND    hwndFrame;
HWND    hwndDDEClient;

// --------------------------------------------------------------------------                   
// The default entryfield procedure(when subclassed).
// --------------------------------------------------------------------------                   
PFNWP   OldEntryFieldProc;


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
// INTERNALS:    NONE                                                                      
//                                                                                         
// *******************************************************************************         
int main(void)
{
   HAB      hab;
   BOOL     bSuccess;
   HMQ      hmq;
   QMSG     qmsg;
   HDC      hdcSize;                  
   HPS      hpsSize;                  
   ULONG    ulFrameFlags;       
   LONG     lxPelsPerMeter;      
   LONG     lyPelsPerMeter;      
   LONG     lxPelsPerInch;     
   LONG     lyPelsPerInch;     
   HWND     hwndClient;
   SWP      swp;                    
   ULONG    SwpLen = sizeof(SWP);     


   // --------------------------------------------------------------------------      
   // Initialize our application with the Presentation Manager                        
   // --------------------------------------------------------------------------      
   hab = WinInitialize(0);               
   if(!hab)                              
   {                                     
    DosBeep(60, 100);                   
    exit(TRUE);                 
   }                                     
                                         
   // --------------------------------------------------------------------------                                                    
   // Create our application message queue                                                    
   // --------------------------------------------------------------------------              
   hmq = WinCreateMsgQueue(hab, 0);     
   if(!hmq)                             
    {                                    
     DosBeep(60, 100);                  
     WinTerminate(hab);                 
     exit(TRUE);                
    }                                    

   // --------------------------------------------------------------------------                
   // Register our window class for our main window.                          
   // --------------------------------------------------------------------------                
   bSuccess = WinRegisterClass(hab,                     
                               CLASSNAME,                         
                               ClientWndProc,                         
                               CS_SIZEREDRAW,                         
                               0);                                    
                                                                                        
   if (!bSuccess)                                                                        
   {                                                                                    
     DisplayMessages(NULLHANDLE, "Error Registering Window Class", MSG_ERROR);                           
     exit(TRUE);                                                                
   }                                                                                    


   ulFrameFlags = FCF_TITLEBAR | FCF_TASKLIST | FCF_SIZEBORDER | FCF_MINMAX   |                                 
                  FCF_SYSMENU  | FCF_MENU     | FCF_ICON       | FCF_ACCELTABLE ;                                                            

   hwndFrame = WinCreateStdWindow(HWND_DESKTOP,       // Desktop window is parent              
                                  NULLHANDLE,         // Frame is invisble initially           
                                  &ulFrameFlags,      // Frame Control Flags                   
                                  CLASSNAME,          // Window class name                     
                                  TITLEBAR,           // Window Title Bar                      
                                  NULLHANDLE,         // Client style                           
                                  (HMODULE)NULL,      // Resource is in the executable          
                                  ID_FRAME,           // Frame window identifier                
                                  &hwndClient);       // Client window handle                   

   hpsSize = WinGetPS(HWND_DESKTOP);                                                                              
   hdcSize = GpiQueryDevice(hpsSize);                                                                             

   if ((hdcSize != NULLHANDLE) && (hdcSize != HDC_ERROR))                     
     {                                                                                                          
      DevQueryCaps(hdcSize, CAPS_HORIZONTAL_RESOLUTION, 1L, &lxPelsPerMeter);                                   
      DevQueryCaps(hdcSize, CAPS_VERTICAL_RESOLUTION,   1L, &lyPelsPerMeter);                                   
                                                                                                                
      lxPelsPerInch=(lxPelsPerMeter * 254) / 10000;                                                                 
      lyPelsPerInch=(lyPelsPerMeter * 254) / 10000;                                                                 
     }                                                                                                          
                                                                                                                
   WinSetWindowPos(hwndFrame,                                                                                   
                   NULLHANDLE,                                                                                  
                   lxPelsPerInch * 2,               // Initial X = 2 inches                                
                   lyPelsPerInch * 2,               // Initial Y = 2 inches                                
                   lxPelsPerInch * 7,               // Length  X = 7 inches                                
                   lyPelsPerInch * 5,               // Length  Y = 5 inches                                
                   SWP_ACTIVATE | SWP_SIZE | SWP_SHOW | SWP_MOVE);             
                                                                                                                
   while(WinGetMsg(hab, &qmsg, (HWND)NULL, 0, 0))   
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
// FUNCTION USE: PMSTATS Client Window Procedure                          
//
// DESCRIPTION:  This is the client window procedure for the main PMSTATS   
//               window.  It is used to process all of our standard messages.   
//
// PARAMETERS:   HWND     client window handle  
//               ULONG    the message to be processed
//               MPARAM   the first message parameter mp1
//               MPARAM   the second message parameter mp2
//
// RETURNS:      MRESULT
//
// INTERNALS:    NONE
//
// HISTORY:      mm-dd-yy
//
// *******************************************************************************
MRESULT EXPENTRY ClientWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   HPS        hps;
   RECTL      rcl;
   PSZ        pszResponse;
   PDDESTRUCT pddestruct;
   CHAR       szBuffer[100];

   switch (msg)
   {
      case WM_DDE_INITIATE:  
           // --------------------------------------------------------------------------       
           // On an initiate:
           //    mp1 = hwnd of DDE client requesting conversation 
           //    mp2 = points to DDEINIT structure
           // --------------------------------------------------------------------------       
           if (!(strcmp(((PDDEINIT)mp2)->pszAppName, APPNAME))) 
            {
             sprintf(szBuffer, "RECEIVED a WM_DDE_INITIATE message from hwnd = %8.8X", HWNDFROMMP(mp1));
             SendTextToMLE(szBuffer);
      
             hwndDDEClient = (HWND)mp1;
      
             // --------------------------------------------------------------------------         
             // Call WinDdeRespond to post an acknowledge the initiation request.
             // --------------------------------------------------------------------------         
             WinDdeRespond(hwndDDEClient,    // DDE Client Window Handle
                           hwnd,             // DDE Server's Window Handle
                           APPNAME,          // Application name of responding server
                           TOPIC,            // Topic that server will support
                           (PCONVCONTEXT)0); // Conversation Context
            }
           break;

      case WM_DDE_ACK:
           SendTextToMLE("Received an acknowledgement from DDE client window.");
           break;

      case WM_PAINT:
           hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);                        
           GpiErase(hps);                                                      
           WinEndPaint(hps);                 
           break;                                                           

      case WM_COMMAND:
           switch (SHORT1FROMMP(mp1))
            {
             case IDM_HELPGEN:   
             case IDM_HELPKEYS:  
                  DisplayMessages(NULLHANDLE, "Sorry, no help is available here.", MSG_INFO);
                  return FALSE;

             case IDM_PRODINFO:
                  WinDlgBox(HWND_DESKTOP, hwnd, ProdInfoDlgProc, NULLHANDLE, IDD_PRODINFO, NULL);
                  return FALSE;

             case IDM_ENTERSTATS:
                  WinSetWindowPos(hwndFrame, HWND_TOP, 0, 0, 0, 0, SWP_HIDE);
                  WinDlgBox(HWND_DESKTOP, hwnd, SpreadSheetDlgProc, NULLHANDLE, IDD_SPREADSHEET, NULL);
                  return FALSE;

            }
           break;

      default:

         return WinDefWindowProc(hwnd,msg,mp1,mp2);

   }

   return FALSE;

}

// *******************************************************************************              
// FUNCTION:     ProdInfoDlgProc                                                                
//                                                                                              
// FUNCTION USE: Dialog procedure used to display product information dialog                    
//                                                                                              
// DESCRIPTION:  This dialog procedure is used for the product information                      
//               dialog box.                                                                    
//                                                                                              
// PARAMETERS:   HWND     Window handle                                                         
//               ULONG    Message to be processed                                               
//               MP1      First message parameter mp1                                           
//               MP2      Second message parameter mp2                                          
//                                                                                              
// RETURNS:      MRESULT                                                                        
//                                                                                              
// INTERNALS:    NONE                                                                           
//                                                                                              
// HISTORY:      mm-dd-yy                                                                       
//                                                                                              
// *******************************************************************************              
MRESULT EXPENTRY ProdInfoDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 switch (msg)
  {
   case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
         {
          case DID_OK:
               WinDismissDlg(hwnd,TRUE);
               return FALSE;
         }
  }
 return WinDefDlgProc(hwnd,msg,mp1,mp2);
}


// *******************************************************************************              
// FUNCTION:     SpreadSheetDlgProc                                                             
//                                                                                              
// FUNCTION USE: Dialog procedure used to obtain input from the user                            
//                                                                                              
// DESCRIPTION:  This dialog procedure handles the spreadsheet dialog, which                    
//               is simply the dialog box that is used to collect the data                      
//               from the user.
//                                                                                              
// PARAMETERS:   HWND     Window handle                                                         
//               ULONG    Message to be processed                                               
//               MP1      First message parameter mp1                                           
//               MP2      Second message parameter mp2                                          
//                                                                                              
// RETURNS:      MRESULT                                                                        
//                                                                                              
// INTERNALS:    NONE                                                                           
//                                                                                              
// HISTORY:      mm-dd-yy                                                                       
//                                                                                              
// *******************************************************************************              
MRESULT EXPENTRY SpreadSheetDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   PDDESTRUCT pddestruct;
   PSZ        pszSendString;
   CHAR       szTextFromEF[5];
   BOOL       bSuccess;
   USHORT     usCounter;
   USHORT     usNumber;
   LONG       cbText;
   LONG       lReturn;
   CHAR       szBuffer[50];
   DATETIME   datetime;
   APIRET     rc;

   HWND       hwndTemp;
   HWND       hwndStatic;
   HWND       hwndEntryfield;
   HWND       hwndCombo;
   HWND       hwndClient;

   switch (msg)
   {
    case WM_INITDLG:
         // --------------------------------------------------------------------------                   
         // Set the system menu                                                                          
         // --------------------------------------------------------------------------                   
         SetTheSysMenu(hwnd, NULLHANDLE);

         // --------------------------------------------------------------------------                   
         // Get the window handle of the combobox and populate it with our                             
         // OS/2 product line.
         // --------------------------------------------------------------------------                   
         hwndCombo  = WinWindowFromID(hwnd, IDC_PRODUCTS);
         FillComboBoxes(hwndCombo);

         // --------------------------------------------------------------------------                   
         // Get the window handle of the MLE control and set the font of the                     
         // MLE window.  The MLE window is the spy window that shows the DDE 
         // communication.
         // --------------------------------------------------------------------------                   
         hwndMLE = WinWindowFromID(hwnd, MLE_SPYWINDOW);
         SetDefaultFont(hwndMLE);


         // --------------------------------------------------------------------------                   
         // Obtain current date and time
         // --------------------------------------------------------------------------                   
         rc = DosGetDateTime(&datetime);     
         if (rc)                                                                          
          {                                                                                    
           WinAlarm(HWND_DESKTOP, WA_ERROR);                      
           SendTextToMLE("CLOCK-ERROR: Error Querying Date Information");  
          }                                                                                    

         else                                                                                       
          {
           // --------------------------------------------------------------------------                   
           // Convert year to proper format and display date information
           // in our static window.
           // --------------------------------------------------------------------------                   
           datetime.year %= 100;           

           // --------------------------------------------------------------------------                        
           // Dump the date information in the static text window.                                           
           // --------------------------------------------------------------------------                        
           hwndStatic = WinWindowFromID(hwnd, ID_STATIC);                                       
           sprintf(szBuffer, "%02d-%02d-%02d", datetime.month, datetime.day, datetime.year);        
           WinSetWindowText(hwndStatic, szBuffer);                                               

           // --------------------------------------------------------------------------                             
           // Tell the Spy Window we got the date information just so we have some                                
           // data in the MLE initially.
           // --------------------------------------------------------------------------                             
           SendTextToMLE("Accessed Date Information");   
          }

         // --------------------------------------------------------------------------                             
         // Loop through all of the entryfield identifiers and convert them
         // to window handles, then subclass each of the entryfield windows.
         // --------------------------------------------------------------------------                             
         for (usCounter = IDE_MONTH01; usCounter < IDE_MONTH13; usCounter++)                  
         {                                                                
          // --------------------------------------------------------------------------     
          // Since we are dealing with percentages, limit the size of the              
          // entryfield text to 3 characters.                                               
          // --------------------------------------------------------------------------     
          WinSendDlgItemMsg (hwnd, usCounter,  EM_SETTEXTLIMIT, MPFROM2SHORT (3, 0), NULL);     

          hwndTemp = WinWindowFromID(hwnd, usCounter); 
          OldEntryFieldProc = WinSubclassWindow(hwndTemp, FilterEntryFieldProc);           
         }
         break;

    case WM_COMMAND:
         switch (SHORT1FROMMP(mp1))
         {
          case DID_OK:
               cbText = 0;

               // --------------------------------------------------------------------------                        
               // For each of the entryfields get the length of the text plus one                               
               // for the separator/terminator.
               // --------------------------------------------------------------------------                        
               for (usCounter = IDE_MONTH01; usCounter < IDE_MONTH13; usCounter++) 
                {
                  cbText += WinQueryDlgItemTextLength(hwnd, (ULONG)usCounter);
                  cbText++;     // room for separator - terminator 
                } 

               // --------------------------------------------------------------------------             
               // Allocate storage for the string of data to be exchanged
               // --------------------------------------------------------------------------             
               pszSendString = malloc(cbText + 1);
               pszSendString[0] = '\0';

               for (usCounter = IDE_MONTH01; usCounter < IDE_MONTH13; usCounter++) 
                {
                  lReturn = WinQueryDlgItemText(hwnd, usCounter, sizeof(szTextFromEF), szTextFromEF);

                  // --------------------------------------------------------------------------             
                  // Don't trust the DDE client to validate the data.  Since we are 
                  // dealing with percentages, if the user does not enter any data in
                  // one of the month entryfields it is the same as the user entering 
                  // a zero, so plug a zero into the entryfield for the user so it  
                  // can be passed on to the DDE client.
                  // --------------------------------------------------------------------------             
                  if (lReturn == 0) 
                   {
                    hwndEntryfield = WinWindowFromID(hwnd, usCounter);

                    strcpy(szTextFromEF, "0"); 
                    WinSetDlgItemText(hwnd, usCounter, szTextFromEF);      
                   }


                  // --------------------------------------------------------------------------             
                  // OK, since our sample program deals with satisfaction percentages
                  // we can only have the user enter a number up to 100 percent.  So 
                  // we will validate the number the user entered in the entryfield.
                  // If the number is above 100, sound the error tone, put an error message 
                  // in the status window, and finally reset the focus to the entryfield
                  // that is in error.
                  // --------------------------------------------------------------------------             
                  usNumber = atoi(szTextFromEF);
                  if (usNumber > 100) 
                   {
                    WinAlarm(HWND_DESKTOP, WA_ERROR);           
                    SendTextToMLE("ERROR: Number in Entryfield must be percentage up to 100%"); 
                    hwndEntryfield = WinWindowFromID(hwnd, usCounter);
                    WinSetFocus(HWND_DESKTOP, hwndEntryfield);
                    return FALSE;
                   } 


                  strcat(pszSendString, szTextFromEF );

                  // --------------------------------------------------------------------------             
                  // pass one big comma delimited string over to the 
                  // DDE client containing all of the data
                  // --------------------------------------------------------------------------             
                  if(usCounter != IDE_MONTH12) 
                   {
                    strcat(pszSendString, ",");
                   } 
                } 

               if (pszSendString[0])
                {
                 // --------------------------------------------------------------------------           
                 // Ok, we now have our string containing all of the data to be exchanged       
                 // (pszSendString), allocate our shared memory object and place the                   
                 // data to be exchanged in a DDESTRUCT structure.
                 // --------------------------------------------------------------------------           
                 pddestruct = BuildDDEDataStructure(DDEFMT_TEXT, 
                                                    TOPIC,
                                                    (PVOID)pszSendString,
                                                    sizeof(pszSendString),
                                                    DDE_FACK);

                 // --------------------------------------------------------------------------   
                 // Get the client window handle, since it is our DDE Server window.
                 // In reality, if we were writing a real application we would probably
                 // use an object window.
                 // --------------------------------------------------------------------------   
                 hwndClient = WinWindowFromID(hwndFrame, FID_CLIENT);

                 // --------------------------------------------------------------------------   
                 // Post a WM_DDE_DATA message over to the DDE client so they get our data.
                 // --------------------------------------------------------------------------   
                 bSuccess = WinDdePostMsg(hwndDDEClient,  // DDE Client Window Handle 
                                          hwndClient,     // DDE Server Window Handle
                                          WM_DDE_DATA,    // DDE message to be posted
                                          pddestruct,     // Pointer to DDE structure
                                          DDEPM_RETRY);   // Options

                 // --------------------------------------------------------------------------   
                 // If everything works, update our spy window
                 // --------------------------------------------------------------------------   
                 if (bSuccess == TRUE)   // No Error
                  {
                   SendTextToMLE("WM_DDE_DATA - message posted to DDE Client window...");
                  }

                 // --------------------------------------------------------------------------    
                 // If we get an error, sound the error alarm and show the error to the user.  
                 // --------------------------------------------------------------------------    
                 else 
                  {
                   WinAlarm(HWND_DESKTOP, WA_ERROR);  
                   SendTextToMLE("DDEERROR: Error Posting WM_DDE_DATA message to DDE client"); 
                  }
                }


               // --------------------------------------------------------------------------    
               // Free the memory for the string
               // --------------------------------------------------------------------------    
               free(pszSendString);
               return FALSE;
            

          case DID_CANCEL:
               hwndMLE = NULLHANDLE;
               WinDismissDlg(hwnd,TRUE);
               WinSetWindowPos(hwndFrame, HWND_TOP, 0, 0, 0, 0, SWP_SHOW); 
               return FALSE;

         } //switch WM_COMMAND
   } // switch msg

 return WinDefDlgProc(hwnd, msg, mp1, mp2);
}


// *******************************************************************************             
// FUNCTION:     BuildDDEDataStructure                                                         
//                                                                                             
// FUNCTION USE: Used to allocate DDE memory object required to transfer data                  
//                                                                                             
// DESCRIPTION:  This function allocates the shared memory object required for                
//               the data exchange and returns a pointer to a DDESTRUCT structure
//               that contains the information to be exchanged.
//
//                                                                                             
// PARAMETERS:   USHORT   DDE format                                                           
//               PSZ      Item Name                                                            
//               PVOID    The data                                     
//               USHORT   The size of the data                                         
//               USHORT   The acknowledgement element of DDESTRUCT structure
//                                                                                             
// RETURNS:      Pointer to DDESTRUCT Structure                                                                    
//                                                                                             
// HISTORY:                                                                                    
//                                                                                             
// *******************************************************************************             
PDDESTRUCT EXPENTRY BuildDDEDataStructure(USHORT usFormat,           
                                          PSZ    pszItemName,
                                          PVOID  pvData,       
                                          USHORT cbData,       
                                          USHORT fsStatus)     
{
 APIRET      rc;                         
 ULONG       ulObjSize  = 0;              
 PVOID       pvBuffer;                    
 PCHAR       szBuffer   = NULL;            
 PDDESTRUCT  pddestruct = NULL;                 


 // --------------------------------------------------------------------------                        
 // Set the size of the object to the size of the data to be exchanged                                                                                                                
 // --------------------------------------------------------------------------                        
 ulObjSize = cbData;

 // --------------------------------------------------------------------------                        
 // Allocate enough shared memory for the data and ensure that the memory                                                                                                                   
 // object is giveable.
 // --------------------------------------------------------------------------                        
 rc = DosAllocSharedMem(&pvBuffer, 
                        NULL,
                        ulObjSize,
                        PAG_COMMIT | PAG_READ | PAG_WRITE | OBJ_GIVEABLE);

 if (rc)          
  {
   DisplayMessages(NULLHANDLE, "Error Allocating Shared Memory for DDE Data", MSG_ERROR);
   return (PDDESTRUCT)NULL;
  }

 // --------------------------------------------------------------------------            
 // Populate the DDESTRUCT 
 // --------------------------------------------------------------------------            
 pddestruct                 = (PDDESTRUCT)pvBuffer;
 pddestruct->usFormat       = usFormat;
 pddestruct->offszItemName  = (USHORT)sizeof(DDESTRUCT);

 // --------------------------------------------------------------------------            
 // Copy the item name into the structure
 // --------------------------------------------------------------------------            
 strcpy(DDES_PSZITEMNAME(pddestruct), pszItemName);

 pddestruct->cbData         = cbData;
 pddestruct->offabData      = pddestruct->offszItemName + strlen(pszItemName) + 1; 
 pddestruct->fsStatus       = fsStatus;

 // --------------------------------------------------------------------------            
 // This routine only supports the text format.  Anything else, report                  
 // an error back to the user.                                                           
 // --------------------------------------------------------------------------            
 if (usFormat == DDEFMT_TEXT)
  {
   if (pvData == NULL || cbData == 0)
    {
     // --------------------------------------------------------------------------             
     // This will only happen if the caller of the routine for whatever
     // reason passed us no data.  Return NULL back to the caller.
     // --------------------------------------------------------------------------             
     DisplayMessages(NULLHANDLE, "No data to exchange", MSG_ERROR);     
     return (PDDESTRUCT)NULL; 
    }

   else
    {
     // --------------------------------------------------------------------------                      
     // Copy the data string into the DDESTRUCT structure so that it can be                           
     // transferred to the DDE Client.
     // --------------------------------------------------------------------------                      
     szBuffer = pvData;
     strcpy(DDES_PABDATA(pddestruct), szBuffer);
    }
  }

 // --------------------------------------------------------------------------                  
 // For all other format types, report an error.
 // --------------------------------------------------------------------------                  
 else
  {
   DisplayMessages(NULLHANDLE, "This routine currently only supports the DDEFMT_TEXT format", MSG_EXCLAMATION);     
  }

 // --------------------------------------------------------------------------     
 // If everything is fine and dandy, return a pointer to our newly created 
 // DDESTRUCT structure.
 // --------------------------------------------------------------------------     
 return pddestruct;
}

// *******************************************************************************
// FUNCTION:     FilterEntryFieldProc
//
// FUNCTION USE: used to restrict input in an entryfield to numeric data only
//
// DESCRIPTION:  This is a suclass procedure used to only allow the spreadsheet 
//               entryfields to contain numbers only.
//
// PARAMETERS:   HWND     Window handle for the subclassed window
//               ULONG    Message to be modified          
//               MP1      First message parameter mp1     
//               MP2      Second message parameter mp2    
//
// RETURNS:      MRESULT
//
// HISTORY:
//
// *******************************************************************************
MRESULT EXPENTRY FilterEntryFieldProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  // --------------------------------------------------------------------------                         
  // Filter WM_CHAR messages only                                                                  
  // --------------------------------------------------------------------------                         
  if (msg == WM_CHAR) 
   {
    if (!(CHARMSG(&msg)->fs & KC_VIRTUALKEY))
      if (CHARMSG(&msg)->fs & KC_CHAR)

        // --------------------------------------------------------------------------                         
        // Check if we have a number.  If we do, handle this normally.                                 
        // --------------------------------------------------------------------------                         
        if (isdigit(CHARMSG(&msg)->chr)) 
         {
           return ((*OldEntryFieldProc)(hwnd, msg, mp1, mp2));
         }

        // --------------------------------------------------------------------------                         
        // If we do not have a number, give the user an obnoxious beep to let                              
        // them know that something is wrong.
        // --------------------------------------------------------------------------                         
        else
         {
          WinAlarm(HWND_DESKTOP, WA_ERROR); 
          return FALSE;
         }
  } /* endif WM_CHAR */
  return (*OldEntryFieldProc)(hwnd, msg, mp1, mp2);
}


// *******************************************************************************          
// FUNCTION:     FillComboBoxes                                                             
//                                                                                          
// FUNCTION USE: Routine used to update insert items into a combobox control                
//                                                                                          
// DESCRIPTION:  This routine is used to fill the Product combobox that is in the           
//               Spreadsheet dialog.  Functionally, this combobox does absolutely           
//               nothing, but is used to spruce up the dialog.                              
//                                                                                          
// PARAMETERS:   HWND  Window handle of the combobox.                                    
//                                                                                          
// RETURNS:      BOOL  Currently, we always return TRUE                                  
//                                                                                          
// HISTORY:                                                                                 
//                                                                                          
// *******************************************************************************          
BOOL EXPENTRY FillComboBoxes(HWND hwndCombo)
{
 USHORT usCounter;
 APIRET rc;

 CHAR   *szProducts[] = {"OS/2 Warp",
                         "OS/2 Warp with WIN-OS/2",
                         "OS/2 Warp Connect",
                         "OS/2 2.11",
                         "OS/2 2.1",
                         "OS/2 2.0",
                         "OS/2 1.3",
                         "OS/2 1.2",
                         "OS/2 1.1",
                         "OS/2 1.0",
                        };

 // --------------------------------------------------------------------------               
 // We do this on purpose.  This routine demonstrates the old and ugly way                 
 // of populating a listbox(combobox in this case).  We used to have to                     
 // sit and loop, inserting each individual item one by one, into the 
 // listbox.  The new and improved listbox code in the OS/2 Warp window 
 // manager has a message called LM_INSERTMULTITEMS which makes this a lot
 // easier.  For the documentation of this message read Chapter 2, or check
 // out the source file BUTTONS.C in the Chapter 2 sample program for the 
 // usage of the new message.
 // --------------------------------------------------------------------------               
 for(usCounter = 0; usCounter < 10; usCounter ++)
  {
     WinEnableWindowUpdate(hwndCombo, FALSE);
     WinSendMsg(hwndCombo, LM_INSERTITEM, MPFROMSHORT(LIT_NONE), MPFROMP(szProducts[usCounter]));

     WinEnableWindowUpdate(hwndCombo, TRUE);
     WinSendMsg(hwndCombo, CBM_HILITE, MPFROMSHORT(TRUE), NULL);
  }
 return TRUE;
}
