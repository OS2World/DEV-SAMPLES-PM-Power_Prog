// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      ERRORS.C                                         mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Header file that contains error information                   บ
// บ                                                                               บ
// บ  NOTES:         This file contains all of the error information defines       บ
// บ                 that can be returned from function calls within the PMSTATS   บ
// บ                 program.                                                      บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ          
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ          
// บ                                                                               บ          
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ          
// บ                                                                               บ          
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ          

// --------------------------------------------------------------------------               
// These are the possible error codes from our SendTextToMLE                                                       
// --------------------------------------------------------------------------               
#define ERROR_ALLOCMEM      0x100       
#define ERROR_INVALID_HWND  0x101

// --------------------------------------------------------------------------                
// Make this global for simplicity                           
// --------------------------------------------------------------------------                
HWND hwndMLE;
