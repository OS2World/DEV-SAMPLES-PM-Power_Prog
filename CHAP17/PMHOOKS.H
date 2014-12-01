// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป             
// บ  FILENAME:      PMHOOKS.H                                        mm/dd/yy     บ             
// บ                                                                               บ             
// บ  DESCRIPTION:   Primary Header File For PMHOOKS Dynamic Link Library          บ             
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
// OS2.INI profile information needed to disable or enable the                                                                                   
// Print Screen function.
// --------------------------------------------------------------------------                                  
#define INI_APPNAME  "PM_ControlPanel"
#define INI_KEYNAME  "PrintScreen"
#define INI_DEFAULT  "1"

// --------------------------------------------------------------------------                                  
// Flags for DosOpen
// --------------------------------------------------------------------------                                  
#define OPEN_FILE       0x02          // replace existing file                                      
#define CREATE_FILE     0x10          // create new file                                            
#define OPEN_FLAGS      OPEN_FILE | CREATE_FILE                                                     
#define OPEN_MODE       OPEN_FLAGS_NOINHERIT | OPEN_SHARE_DENYREADWRITE | OPEN_ACCESS_READWRITE     

// --------------------------------------------------------------------------                                  
// The DLL Initialization Routine for PMHOOKS
// --------------------------------------------------------------------------                                  
BOOL  EXPENTRY DLLInitRoutine(HAB);

// --------------------------------------------------------------------------               
// Wrapper functions for setting our various hooks.                                       
// --------------------------------------------------------------------------               
BOOL  EXPENTRY SetInputHook(VOID);
BOOL  EXPENTRY SetLockupHook(VOID);                                  
BOOL  EXPENTRY SetFlushBufHook(VOID);                                    

// --------------------------------------------------------------------------                
// Wrapper functions for removing our various hooks.                                          
// --------------------------------------------------------------------------                
VOID EXPENTRY ReleaseLockupHook(VOID);                  
VOID EXPENTRY ReleaseFlushBufHook(VOID);                
BOOL EXPENTRY ReleaseInputHook(VOID);                   

// --------------------------------------------------------------------------                         
// Our actual hook procedures.                                                                     
// --------------------------------------------------------------------------                         
BOOL  EXPENTRY InputHook  (HAB, PQMSG, ULONG);
BOOL  EXPENTRY LockupHook (HAB, HWND);
BOOL  EXPENTRY FlushBufHook (HAB);           

// --------------------------------------------------------------------------                      
// Miscellaneous Functions from PMHOOKS.C                                                                        
// --------------------------------------------------------------------------                      
BOOL    DisablePrintScreenKey(BOOL bState);                                                        
HBITMAP PrintScreenToBitmap(VOID);                                                                 
