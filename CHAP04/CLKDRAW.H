// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      CLKDRAW.H                                        mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Primary Include File for CLKDRAW                              บ
// บ                                                                               บ
// บ  NOTES:         This file contains all of the various defines for all of      บ
// บ                 window identifies and numeric string identifiers.             บ
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
// Help pushbutton identifier                                                           
// --------------------------------------------------------------------------           
#define DID_HELP          5

// --------------------------------------------------------------------------                    
// Window Identifiers                                                                          
// --------------------------------------------------------------------------                    
#define ID_FRAME        101    // Frame Window Identifier
#define ID_STATIC       102
#define ID_SPINBUTTON   103
#define ID_STATUSWND    104

// --------------------------------------------------------------------------                    
// Dialog Identifiers                                                                          
// --------------------------------------------------------------------------                    
#define IDD_CONFIGURE   120
#define IDD_COLOR       121
#define IDD_PRODINFO    122
#define IDD_LINETYPE    123

// --------------------------------------------------------------------------             
// Product Information Bitmap Identifier                                                  
// --------------------------------------------------------------------------             
#define IDB_PRODINFO    200 

// --------------------------------------------------------------------------              
// Tracking Pointer Identifier                                                                                       
// --------------------------------------------------------------------------              
#define IDP_TRACKRECT   210 

// --------------------------------------------------------------------------                  
// Our Action Bar Menu                                                                          
// --------------------------------------------------------------------------                  
#define ID_OPTIONSMENU  500    // Options SubMenu Identifier
#define IDM_CLEAR       501
#define IDM_CONFIGURE   502
#define IDM_COLOR       503
#define IDM_LINETYPE    504
#define IDM_HOTSPOT     505         
#define IDM_HIDEMOUSE   506         
#define IDM_EXIT        507

#define ID_EDITMENU     510    // Edit SubMenu Identifier 
#define IDM_TRACKBOX    511
#define IDM_EDITCUT     512
#define IDM_EDITCOPY    513
#define IDM_EDITPASTE   514

#define ID_HELPMENU     520    // Help SubMenu Identifier 
#define IDM_HELPGEN     522
#define IDM_HELPKEYS    523
#define IDM_PRODINFO    524

// --------------------------------------------------------------------------                  
// Radio Button Identifiers                                                                          
// --------------------------------------------------------------------------                  
#define IDR_WINCOOR     600
#define IDR_SCRCOOR     601

// --------------------------------------------------------------------------                           
// System Values Structure                                                                           
// --------------------------------------------------------------------------                           
typedef struct SYSVALUES
 {
  LONG  lcxScreen;
  LONG  lcyScreen;
  LONG  lcxFullScreen;     
  LONG  lcyFullScreen;     
  LONG  lcxPointer;
  LONG  lcyPointer;
  LONG  lcxDlgFrame;
  LONG  lcyDlgFrame;
 } SYSVALUES, FAR *PSYSVALUES;


// --------------------------------------------------------------------------                  
// Help text used for spirograph dialog box                                                                         
// --------------------------------------------------------------------------                  
#define HELP_TEXT     \                                                                                
"This dialog allows you to configure the rotational angle"\                 
" used to draw the spirographs.  The rotation angle will"\                 
" determine what the spirograph will look like.\n\n"\                 
"Also, you can select which coordinate system to use for"\                 
" drawing, by selecting one of the radiobuttons above."                  
