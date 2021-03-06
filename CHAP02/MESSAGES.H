// 浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�        
// �  FILENAME:      MESSAGES.H                                       mm/dd/yy     �        
// �                                                                               �        
// �  DESCRIPTION:   Message Text Strings                                          �        
// �                                                                               �        
// �  NOTES:         None                                                          �        
// �                                                                               �        
// �  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        �        
// �  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          �        
// �                                                                               �        
// �  REVISION DATES:  mm/dd/yy  Created this file                                 �        
// �                                                                               �        
// 藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�        

// --------------------------------------------------------------------------     
// These are the Informational Messages   
// --------------------------------------------------------------------------     
#define INFO_NOHELP                100

// --------------------------------------------------------------------------     
// These are Questions that the program will prompt the user to answer
// --------------------------------------------------------------------------     
#define QUESTION_SHUTDOWN          200         
#define QUESTION_SAVEWINPOS        201
#define QUESTION_EXIT              202

// --------------------------------------------------------------------------     
// These are Error Messages that will be displayed to the user
// --------------------------------------------------------------------------     
#define ERROR_INVALID_PROFILE      301
#define ERROR_OPENING_PROFILE      303
#define ERROR_ALREADY_RUNNING      304
#define ERROR_CHANGING_ICON        305
#define ERROR_STARTING_PROGRAM     306
#define ERROR_PROGRAM_NOT_RUNNING  307
#define ERROR_WRITING_PROFILE      308
#define ERROR_POPULATING_LISTBOX   309


// --------------------------------------------------------------------------     
// This banner is what gets written to the top of the profile(BUTTONS.PRO)
// whenever the user saves the program list information.
// --------------------------------------------------------------------------     
#define TEXT_HEADER     \
"; 浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�\n"\    
"; �  FILENAME:      BUTTONS.PRO                                                   �\n"\      
"; �                                                                               �\n"\      
"; �  DESCRIPTION:   BUTTONS profile information                                   �\n"\      
"; �                                                                               �\n"\      
"; �  NOTES:         This file contains the BUTTONS program list.  The BUTTONS     �\n"\      
"; �                 control panel contains eight configurable pushbuttons.        �\n"\    
"; �                 The actual program list contains nine entries.                �\n"\    
"; �                                                                               �\n"\    
"; �  RULES:         - The first program entry is a dummy entry used to validate   �\n"\    
"; �                   if this file is indeed in the correct format.  The dummy    �\n"\    
"; �                   entry must read BUTTONS                                     �\n"\    
"; �                                                                               �\n"\            
"; �                 - Comment lines begin with a semi colon and blank lines       �\n"\    
"; �                   are ignored.                                                �\n"\    
"; �                                                                               �\n"\    
"; �                 - An asterisk is used to mark the end of the program list     �\n"\    
"; �                                                                               �\n"\    
"; �  COPYRIGHTS:    Uri J. Stern and James S. Morrow                              �\n"\            
"; �                 OS/2 Warp Presentation Manager for Power Programmers          �\n"\    
"; �                                                                               �\n"\ 
"; �  PARAMETERS:    /F        = OS/2 Fullscreen session                           �\n"\    
"; �                 /f        = OS/2 Windowed session                             �\n"\    
"; �                 /D        = DOS  Fullscreen session                           �\n"\    
"; �                 /d        = DOS  Windowed session                             �\n"\    
"; �                 /W        = WinOS/2 Fullscreen session                        �\n"\    
"; �                 /w        = WinOS/2 Seamless session                          �\n"\     
"; �                 /%        = Specify Command line parameters to application    �\n"\      
"; �                                                                               �\n"\ 
"; 藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�"  
