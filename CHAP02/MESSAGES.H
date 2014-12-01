// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป        
// บ  FILENAME:      MESSAGES.H                                       mm/dd/yy     บ        
// บ                                                                               บ        
// บ  DESCRIPTION:   Message Text Strings                                          บ        
// บ                                                                               บ        
// บ  NOTES:         None                                                          บ        
// บ                                                                               บ        
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ        
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ        
// บ                                                                               บ        
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ        
// บ                                                                               บ        
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ        

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
"; ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป\n"\    
"; บ  FILENAME:      BUTTONS.PRO                                                   บ\n"\      
"; บ                                                                               บ\n"\      
"; บ  DESCRIPTION:   BUTTONS profile information                                   บ\n"\      
"; บ                                                                               บ\n"\      
"; บ  NOTES:         This file contains the BUTTONS program list.  The BUTTONS     บ\n"\      
"; บ                 control panel contains eight configurable pushbuttons.        บ\n"\    
"; บ                 The actual program list contains nine entries.                บ\n"\    
"; บ                                                                               บ\n"\    
"; บ  RULES:         - The first program entry is a dummy entry used to validate   บ\n"\    
"; บ                   if this file is indeed in the correct format.  The dummy    บ\n"\    
"; บ                   entry must read BUTTONS                                     บ\n"\    
"; บ                                                                               บ\n"\            
"; บ                 - Comment lines begin with a semi colon and blank lines       บ\n"\    
"; บ                   are ignored.                                                บ\n"\    
"; บ                                                                               บ\n"\    
"; บ                 - An asterisk is used to mark the end of the program list     บ\n"\    
"; บ                                                                               บ\n"\    
"; บ  COPYRIGHTS:    Uri J. Stern and James S. Morrow                              บ\n"\            
"; บ                 OS/2 Warp Presentation Manager for Power Programmers          บ\n"\    
"; บ                                                                               บ\n"\ 
"; บ  PARAMETERS:    /F        = OS/2 Fullscreen session                           บ\n"\    
"; บ                 /f        = OS/2 Windowed session                             บ\n"\    
"; บ                 /D        = DOS  Fullscreen session                           บ\n"\    
"; บ                 /d        = DOS  Windowed session                             บ\n"\    
"; บ                 /W        = WinOS/2 Fullscreen session                        บ\n"\    
"; บ                 /w        = WinOS/2 Seamless session                          บ\n"\     
"; บ                 /%        = Specify Command line parameters to application    บ\n"\      
"; บ                                                                               บ\n"\ 
"; ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ"  
