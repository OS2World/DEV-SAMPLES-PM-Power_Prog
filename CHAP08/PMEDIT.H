// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป                     
// บ  FILENAME:      PMEDIT.H                                         mm/dd/yy     บ                     
// บ                                                                               บ                     
// บ  DESCRIPTION:   Primary Header File For PMEDIT/Chapter 08 Sample Program      บ                     
// บ                                                                               บ                     
// บ  NOTES:         This file contains the various resource definitions used      บ                     
// บ                 by the PMEDIT source code.                                    บ                     
// บ                                                                               บ                     
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ                     
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ                     
// บ                                                                               บ                     
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ                     
// บ                                                                               บ                     
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ                     

// --------------------------------------------------------------------------   
// Various definitions used by the program.
// --------------------------------------------------------------------------   
#define APPNAME "PMEditor"                                                               
#define COMMAND (COMMANDMSG(&msg)->cmd)                                                  
#define BOOKTITLE "OS/2 Warp Presentation Manager for Power Programmers"                 
#define PREFIX  "PMEDIT - "
#define VALUE          (SHORT1FROMMP (mp1))

// --------------------------------------------------------------------------                      
// This define is used by the recall file array.                                                
// --------------------------------------------------------------------------                      
#define MAX_EXEC        5                                  

// --------------------------------------------------------------------------             
// Some global variables.                                                                 
// --------------------------------------------------------------------------             
static PSZ pszRecall[MAX_EXEC];    
CHAR   szPathAndFilename[CCHMAXPATH];
USHORT usFilePos;

// --------------------------------------------------------------------------                         
// Make these global for simplicity.                                                                             
// --------------------------------------------------------------------------                         
HWND     hwndFrame;        
HWND     hwndMLE;
                                    
// --------------------------------------------------------------------------              
// Used for standard dialog code.                                                       
// --------------------------------------------------------------------------              
#define DLG_OPEN            7        // Used to detect Open Dialog     
#define DLG_SAVEAS          8        // Used to detect SaveAs Dialog   

// --------------------------------------------------------------------------                        
// This section identifies Window Identifiers    50-70       
// --------------------------------------------------------------------------                        
#define ID_FRAME           51        // Main Window Identifier         
#define ID_MLEWINDOW       52        // MLE Window Identifier          
#define ID_PRODINFO        54        // About Dialog Identifier        
#define ID_SEARCH          55        // Search Dialog Identifier       
#define ID_STATIC          56        // Static Window Identifier
#define ID_FILENAME        57
#define ID_CONFIGURE       58
#define FID_SYSMENUPOP     61        // System Menu Popup Window ID    

// --------------------------------------------------------------------------                        
// This section identifies Pushbuttons           70-80       
// --------------------------------------------------------------------------                        
#define DID_FIND           71  
#define DID_REPLACE        72
#define DID_HELP           73        

// --------------------------------------------------------------------------                        
// This section identifies EntryFields           80-90       
// --------------------------------------------------------------------------                        
#define IDE_SEARCHTEXT     81
#define IDE_REPLACETEXT    82

// --------------------------------------------------------------------------                        
// This section identifies Checkboxes            90-94       
// --------------------------------------------------------------------------                        
#define IDC_CASECHECK      91                                     
#define IDC_REPLACEALL     92
#define IDC_WORDWRAP       93
#define IDC_WINPOS         94
#define IDC_CAPITALS       95

// --------------------------------------------------------------------------                        
// This section identifies Bitmaps               95-100      
// --------------------------------------------------------------------------                        
#define IDB_PRODINFO       100       // Bitmap for Product Information 

// --------------------------------------------------------------------------                        
// This section identifies Menuitem resources    100-500     
// --------------------------------------------------------------------------                        
#define ID_FILEMENU       100        // File Menu                      
#define IDM_FILENEW       101
#define IDM_FILEOPEN      102
#define IDM_FILESAVE      103
#define IDM_FILESAVEAS    104
#define IDM_FILESEARCH    105
#define IDM_EXIT          106
#define IDM_SAVEWIN       107

#define IDM_FILERECALL    108
#define IDM_RECALL        109
#define IDM_PREVFILES     110 

#define ID_EDITMENU       200        // Edit Menu                      
#define IDM_EDITCUT       201
#define IDM_EDITCOPY      202
#define IDM_EDITPASTE     203
#define IDM_EDITCLEAR     204
#define IDM_EDITSELALL    205

#define ID_OPTIONSMENU    300        // Options Menu                    
#define IDM_CHANGEFONT    301
#define IDM_COLORS        302
#define IDM_TEXTCOLORS    303                                                                                 

#define IDM_DEFAULT       350                               
#define IDM_WHITE         351
#define IDM_RED           352
#define IDM_BLUE          353 
#define IDM_YELLOW        354
#define IDM_CYAN          355

#define IDM_CONFIGURE     380

#define ID_HELPMENU       400        // Help Menu                      
#define IDM_HELPINDEX     401
#define IDM_HELPGEN       402
#define IDM_HELPKEYS      403
#define IDM_PRODINFO      405

#define ID_POPUPMENU      406

#define IDM_TXT_BLACK     409                  
#define IDM_TXT_WHITE     410                                
#define IDM_TXT_RED       411                                
#define IDM_TXT_BLUE      412                                
#define IDM_TXT_GREEN     413   
#define IDM_TXT_YELLOW    414                             

#define IDM_SETTINGS      418
#define IDM_DISMISS       420

// --------------------------------------------------------------------------      
// These are the Icon Identifiers used by the button bar                      
// --------------------------------------------------------------------------      
#define IDI_NEW           600
#define IDI_EDIT          601
#define IDI_SAVE          602
#define IDI_SAVEAS        603
#define IDI_SEARCH        604
#define IDI_CUT           605
#define IDI_COPY          606        
#define IDI_PASTE         607
#define IDI_FONT          608 
#define IDI_HELP          609

// --------------------------------------------------------------------------                        
// These values are radiobutton identifiers used for WordWrap
// --------------------------------------------------------------------------                        
#define IDR_WORDON        0
#define IDR_WORDOFF       1

// --------------------------------------------------------------------------                        
// This section identifies Stringtable resources 900-999     
// --------------------------------------------------------------------------                        
#define IDMSG_FILEALREADYEXISTS   907
#define IDMSG_HELP                908

// --------------------------------------------------------------------------                      
// Help text                                                                                       
// --------------------------------------------------------------------------                      
#define HELP_TEXT     \                                                                            
"PMEDIT is a pretty simple text editor that demonstrates"\                                         
" several PM concepts.  You will notice that if you press"\                                            
" and hold the right mouse button on any of the buttons in the button bar"\                     
" you will get a description of what the button does in the status window."                                                                
