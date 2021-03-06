// 浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�
// �  FILENAME:      ERRORS.C                                         mm/dd/yy     �
// �                                                                               �
// �  DESCRIPTION:   Header file that contains error information                   �
// �                                                                               �
// �  NOTES:         This file contains all of the error information defines       �
// �                 that can be returned from function calls within the PMSTATS   �
// �                 program.                                                      �
// �                                                                               �
// �  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        �          
// �  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          �          
// �                                                                               �          
// �  REVISION DATES:  mm/dd/yy  Created this file                                 �          
// �                                                                               �          
// 藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�          

// --------------------------------------------------------------------------               
// These are the possible error codes from our SendTextToMLE                                                       
// --------------------------------------------------------------------------               
#define ERROR_ALLOCMEM      0x100       
#define ERROR_INVALID_HWND  0x101

// --------------------------------------------------------------------------                
// Make this global for simplicity                           
// --------------------------------------------------------------------------                
HWND hwndMLE;
