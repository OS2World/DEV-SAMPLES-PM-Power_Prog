// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      BUTTONS.H                                        mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Primary Header File For BUTTONS/Chapter 02 Sample Program     บ
// บ                                                                               บ
// บ  NOTES:         This file contains the various resource definitions used      บ
// บ                 by the BUTTONS source code.                                   บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ


// --------------------------------------------------------------------------      
// Definition of the BUTTONS window class                                                    
// --------------------------------------------------------------------------      
#define CLASSNAME       "ButtonsClass"                       

// --------------------------------------------------------------------------
// Currently this program uses a default profile name  
// --------------------------------------------------------------------------
#define BUTTONSPRO      "BUTTONS.PRO"

// --------------------------------------------------------------------------      
// Define for static text                                                                                                         
// --------------------------------------------------------------------------      
#define ID_TEXT        -1                                                                                        

// --------------------------------------------------------------------------
// Dos Open Flags
// --------------------------------------------------------------------------
#define OPEN_FILE       0x02          // replace existing file                         
#define CREATE_FILE     0x10          // create new file                               
#define OPEN_FLAGS      OPEN_FILE | CREATE_FILE
#define OPEN_MODE       OPEN_FLAGS_NOINHERIT | OPEN_SHARE_DENYREADWRITE | OPEN_ACCESS_READWRITE  

// --------------------------------------------------------------------------
// This define corresponds to the application name in the OS2.INI
// --------------------------------------------------------------------------
#define APPNAME         "Buttons for OS/2"

// --------------------------------------------------------------------------
// These are the defines for the keynames in the OS2.INI files
// --------------------------------------------------------------------------
#define KEYCOMSPEC      "B_ComspecParm"
#define KEYCOMMAND      "B_CommandLine"
#define KEYPARAMS       "B_CommandParm"
#define KEYNAME1        "B_HideControls"
#define KEYNAME2        "B_RemoveSwitch"
#define KEYNAME3        "B_Confirmation"
#define KEYNAME4        "B_FuncButton"

// --------------------------------------------------------------------------
// Defines for significant file extensions
// --------------------------------------------------------------------------
#define EXT_PROFILE     "*.PRO"
#define EXT_ICON        "*.ICO"
#define EXT_EXECUTABLE  "*.EXE"

#define CMD_MSG         (COMMANDMSG(&msg)->cmd)

#define SIGNATURE_NAME  ("BUTTONS")     
#define MAX_EXEC        15                  

// --------------------------------------------------------------------------
// These defines are used for manipulating the OS2.INI file
// --------------------------------------------------------------------------
#define OPTION_YES      "1"
#define OPTION_NO       "0"

#define PRFSUCCESS      TRUE
#define CHECKED         TRUE              
#define UNCHECKED       FALSE             

// --------------------------------------------------------------------------
// NOTE: These is how the BUTTONS control panel is setup
// --------------------------------------------------------------------------
//  These are the buttons identifiers used to create the buttons control         
//  panel.  The first four buttons BID_ABOUT - BID_VIOCMD are known as the
//  function buttons, since they correspond to a particular OS/2 function.
//  The remaining eight buttons are known as the program buttons that the 
//  user can configure to launch a specified program.
//
//  Here is a sample BUTTONS matrix:
//
//
//                5  6  7  8
//
//                1  2  3  4     <- PROGRAM BUTTONS
//
//                *  *  *  *     <- FUNCTION BUTTONS (First four buttons)
//
//  WARNING:  These numbers are also used to index an array so they        
//            must remain defined as they are:                             
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Function Buttons       
// --------------------------------------------------------------------------
#define BID_ABOUT       0      // Product Information Dialog Box
#define BID_SHUTDOWN    1      // Invokes the system shutdown function
#define BID_LOCKUP      2      // Invokes the lockup function
#define BID_VIOCMD      3      // OS/2 Command Prompt

// --------------------------------------------------------------------------
// Program Buttons
// --------------------------------------------------------------------------
#define BID_PROGRAM1    4      // Button program 1  - User Configurable
#define BID_PROGRAM2    5      // Button program 2  - User Configurable 
#define BID_PROGRAM3    6      // Button program 3  - User Configurable 
#define BID_PROGRAM4    7      // Button program 4  - User Configurable 
#define BID_PROGRAM5    8      // Button program 5  - User Configurable 
#define BID_PROGRAM6    9      // Button program 6  - User Configurable 
#define BID_PROGRAM7    10     // Button program 7  - User Configurable 
#define BID_PROGRAM8    11     // Button program 8  - User Configurable 
#define BID_END         12     // Used to mark the end of program buttons 

// --------------------------------------------------------------------------
// Window Identifiers 
// --------------------------------------------------------------------------
#define ID_MAINWND      101    //  Window Identifier
#define ID_DIALOGMENU   102    //  Window Identifier
#define ID_STATIC       103    //  Window Identifier

// --------------------------------------------------------------------------
// Dialog Window Identifiers 
// --------------------------------------------------------------------------
#define IDD_ABOUT       402    
#define IDD_CONFIGURE   405    
#define IDD_SETUP       406    
#define IDD_PROGLIST    407
#define IDD_COMMAND     408
#define IDD_COMSPEC     409

// --------------------------------------------------------------------------
// Bitmap Identifier used to display product information
// --------------------------------------------------------------------------
#define IDB_PRODINFO    999    

#define ID_OPTIONSMENU  700
#define IDM_CMDBUTTON   701      
#define IDM_PROGLIST    702
#define IDM_HIDECTLS    703
#define IDM_SAVEWIN     704
#define IDM_EXIT        705    //  Exit  Menu Item Identifier

#define ID_CONFIGMENU   710    //  File SubMenu Identifier
#define IDM_SETUP       711
#define IDM_CONFIGURE   712
#define IDM_COMSPEC     713

#define ID_HELPMENU     720                    
#define IDM_HELPGEN     721                    
#define IDM_HELPKEYS    722                    
#define IDM_ABOUT       723    //  About Menu Item Identifier


// --------------------------------------------------------------------------
// Icon Identifiers
// --------------------------------------------------------------------------
// NOTE: DO NOT MODIFY ANY OF THESE DEFINES!!!           
//       These values are referenced in the window text of the       
//       WinCreateWindow function when the button windows are created.
//       If these values change, we need to change the WinCreateWindow              
//       call to use the update identifier.
// --------------------------------------------------------------------------
#define IDI_SHUTDOWN    602    
#define IDI_LOCKUP      603
#define IDI_VIOCMD      604
#define IDI_LEFT        605
#define IDI_RIGHT       606

// --------------------------------------------------------------------------
// Radio Buttons 
// --------------------------------------------------------------------------
#define IDR_DEFAULT     800
#define IDR_PM          801
#define IDR_OS2WINDOW   802
#define IDR_OS2FSCREEN  803
#define IDR_DOSWINDOW   804
#define IDR_DOSFSCREEN  805
#define IDR_WINWINDOW   806
#define IDR_WINFSCREEN  807

// --------------------------------------------------------------------------
// Check Buttons 
// --------------------------------------------------------------------------
#define IDC_FRAMECTLS   0
#define IDC_WINLIST     1
#define IDC_CONFIRM     2
#define IDC_COMLINE     3
#define IDC_LAST        4

// --------------------------------------------------------------------------
// Entryfields 
// --------------------------------------------------------------------------
#define IDE_EXECUTABLE  823
#define IDE_PARAMETERS  824

// --------------------------------------------------------------------------
// ComboBoxes and ListBoxes 
// --------------------------------------------------------------------------
#define IDL_PROGLIST    830
#define IDL_COMLIST     831

// --------------------------------------------------------------------------
// Pushbuttons 
// --------------------------------------------------------------------------
#define DID_HELP        900
#define DID_LEFT        901
#define DID_RIGHT       902
#define DID_FIND        903
#define DID_ICON        904
#define DID_SAVE        905
#define DID_FRAME       906
#define DID_RUN         907
#define DID_DEFAULT     908
#define DID_KILL        909
