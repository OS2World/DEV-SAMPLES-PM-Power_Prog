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
// This banner is what gets written to the top of the presentation file
// whenever the user saves the program list information.
// --------------------------------------------------------------------------     
#define TEXT_HEADER     \
"; ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป\n"\    
"; บ                                                                               บ\n"\      
"; บ  DESCRIPTION:   SHOWOFF Presentation File                                     บ\n"\      
"; บ                                                                               บ\n"\      
"; บ  NOTES:         This file contains the information required by the SHOWOFF    บ\n"\      
"; บ                 presentation engine to display a series of bitmap files       บ\n"\    
"; บ                 files in a specified sequence.                                บ\n"\    
"; บ                                                                               บ\n"\    
"; บ  RULES:         - Comment lines in this file begin with a semi colon and      บ\n"\    
"; บ                   all blank lines are ignored.                                บ\n"\    
"; บ                                                                               บ\n"\    
"; บ                 - Each line in this file should contain the path and filename บ\n"\    
"; บ                   of a bitmap file to be displayed.                           บ\n"\    
"; บ                                                                               บ\n"\    
"; บ  OPTIONS:       - The first line of this file should contain a slash          บ\n"\
"; บ                   followed by a time value in seconds that each bitmap        บ\n"\            
"; บ                   should be displayed for.                                    บ\n"\            
"; บ                                                                               บ\n"\            
"; บ                   For example:                                                บ\n"\            
"; บ                      /5        each bitmap will be displayed for 5 seconds.   บ\n"\            
"; บ                                                                               บ\n"\            
"; บ                 - If the second line of this file contains a slash followed   บ\n"\            
"; บ                   by a dollar sign, the Presentation will be displayed        บ\n"\            
"; บ                   in Fullscreen mode.                                         บ\n"\            
"; บ                                                                               บ\n"\            
"; บ                 - If you want a specific bitmap to display for an alternate   บ\n"\            
"; บ                   amount of time, rather than the default time between        บ\n"\            
"; บ                   bitmaps, simply put a slash and the time value after the    บ\n"\            
"; บ                   path and filename of the bitmap.                            บ\n"\            
"; บ                                                                               บ\n"\            
"; บ                   For example:                                                บ\n"\            
"; บ                      C:\\SHOWOFF\\LOGO.BMP /10 will display the LOGO.BMP      บ\n"\            
"; บ                                                bitmap file for 10 seconds     บ\n"\            
"; บ                                                regardless of what the default บ\n"\            
"; บ                                                time value is.                 บ\n"\            
"; บ                                                                               บ\n"\    
"; บ  COPYRIGHTS:    Uri J. Stern and James S. Morrow                              บ\n"\            
"; บ                 OS/2 Warp Presentation Manager for Power Programmers          บ\n"\    
"; บ                                                                               บ\n"\ 
"; ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ"  
