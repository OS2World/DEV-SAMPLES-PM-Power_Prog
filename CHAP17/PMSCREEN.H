// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป             
// บ  FILENAME:      PMSCREEN.H                                       mm/dd/yy     บ             
// บ                                                                               บ             
// บ  DESCRIPTION:   Primary Header File For PMSCREEN/Chapter 17 Sample Program    บ             
// บ                                                                               บ             
// บ  NOTES:         This file contains the various resource definitions used      บ             
// บ                 by the PMSCREEN source code.                                  บ             
// บ                                                                               บ             
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ             
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ             
// บ                                                                               บ             
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ             
// บ                                                                               บ             
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ             

// --------------------------------------------------------------------------                            
// Global definitions                                                                                    
// --------------------------------------------------------------------------                            
#define VALUE          (SHORT1FROMMP (mp1))                                                              

// --------------------------------------------------------------------------                       
// Identifiers for various resource Bitmaps
// --------------------------------------------------------------------------                       
#define IDB_PRODINFO    998         
#define IDB_COVER       999                                      

// --------------------------------------------------------------------------           
// Window Identifiers                                                                   
// --------------------------------------------------------------------------           
#define ID_FRAME        101               
#define ID_WIDTH        102
#define ID_HEIGHT       103
#define ID_CPLANE       104
#define ID_BPPPEL       105
#define ID_SCRWIDTH     106
#define ID_SCRHEIGHT    107
#define ID_NUMCOLORS    108

// --------------------------------------------------------------------------                 
// Dialog Window Identifiers                                                                  
// --------------------------------------------------------------------------                 
#define IDD_HOTKEY      402    
#define IDD_PRODINFO    405
#define IDD_DRAWBMP     406
#define IDD_BMPRES      407
#define IDD_THANKS      408

// --------------------------------------------------------------------------                  
// The Options Menu                                                                            
// --------------------------------------------------------------------------                  
#define ID_FILEMENU     500               
#define IDM_HKREBOOT    501      
#define IDM_EXIT        502            
#define IDM_CAPTURE     503
#define IDM_DRAWBMP     504
#define IDM_BMPRES      505

// --------------------------------------------------------------------------                   
// The Edit Menu                                                                             
// --------------------------------------------------------------------------                   
#define ID_EDITMENU     510
#define IDM_EDITCUT     511
#define IDM_EDITCOPY    512
#define IDM_EDITPASTE   513
#define IDM_EDITCLEAR   514

// --------------------------------------------------------------------------      
// The Help Menu                                                                   
// --------------------------------------------------------------------------      
#define ID_HELPMENU     520                 
#define IDM_HELPGEN     521                 
#define IDM_HELPKEYS    522                 
#define IDM_PRODINFO    523                 
#define IDM_THANKS      524

// --------------------------------------------------------------------------                    
// Pushbutton Identifiers                                                                        
// --------------------------------------------------------------------------                    
#define DID_DISABLE      60
#define DID_HELP         61

// --------------------------------------------------------------------------                
// Data structure needed for Disabling reboot hotkey.                                                                                 
// --------------------------------------------------------------------------                
struct parm
 {
  USHORT KeyState;
  BYTE   MakeCode;
  BYTE   BreakCode;
  USHORT KeyID;
 } Parm_Packet;

// --------------------------------------------------------------------------               
// Help text                                                
// --------------------------------------------------------------------------               
#define HELP_TEXT     \                                                                     
"To capture a window simply give that window input focus"\                                 
" and press the Print Screen key.  Once the window is"\                                  
" captured, you can use the clipboard cut and copy options to put the bitmap"\                                       
" into your favorite program.\n\n"\                                  
"To demonstrate the Lockup hook, just select the Lockup now option from the"\                                  
" Desktop menu.\n\n"\                                  
"To demonstrate the FlushBuf hook, warm boot your machine using the Ctrl-Alt-Del"\                                  
" reboot hotkey.  The root directory of the boot drive will contain a file named"\                                  
" REBOOT.LOG, indicating when the machine was rebooted."                                     
