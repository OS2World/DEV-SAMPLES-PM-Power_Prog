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
// This banner is what gets written to the top of the presentation file
// whenever the user saves the program list information.
// --------------------------------------------------------------------------     
#define TEXT_HEADER     \
"; 浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�\n"\    
"; �                                                                               �\n"\      
"; �  DESCRIPTION:   SHOWOFF Presentation File                                     �\n"\      
"; �                                                                               �\n"\      
"; �  NOTES:         This file contains the information required by the SHOWOFF    �\n"\      
"; �                 presentation engine to display a series of bitmap files       �\n"\    
"; �                 files in a specified sequence.                                �\n"\    
"; �                                                                               �\n"\    
"; �  RULES:         - Comment lines in this file begin with a semi colon and      �\n"\    
"; �                   all blank lines are ignored.                                �\n"\    
"; �                                                                               �\n"\    
"; �                 - Each line in this file should contain the path and filename �\n"\    
"; �                   of a bitmap file to be displayed.                           �\n"\    
"; �                                                                               �\n"\    
"; �  OPTIONS:       - The first line of this file should contain a slash          �\n"\
"; �                   followed by a time value in seconds that each bitmap        �\n"\            
"; �                   should be displayed for.                                    �\n"\            
"; �                                                                               �\n"\            
"; �                   For example:                                                �\n"\            
"; �                      /5        each bitmap will be displayed for 5 seconds.   �\n"\            
"; �                                                                               �\n"\            
"; �                 - If the second line of this file contains a slash followed   �\n"\            
"; �                   by a dollar sign, the Presentation will be displayed        �\n"\            
"; �                   in Fullscreen mode.                                         �\n"\            
"; �                                                                               �\n"\            
"; �                 - If you want a specific bitmap to display for an alternate   �\n"\            
"; �                   amount of time, rather than the default time between        �\n"\            
"; �                   bitmaps, simply put a slash and the time value after the    �\n"\            
"; �                   path and filename of the bitmap.                            �\n"\            
"; �                                                                               �\n"\            
"; �                   For example:                                                �\n"\            
"; �                      C:\\SHOWOFF\\LOGO.BMP /10 will display the LOGO.BMP      �\n"\            
"; �                                                bitmap file for 10 seconds     �\n"\            
"; �                                                regardless of what the default �\n"\            
"; �                                                time value is.                 �\n"\            
"; �                                                                               �\n"\    
"; �  COPYRIGHTS:    Uri J. Stern and James S. Morrow                              �\n"\            
"; �                 OS/2 Warp Presentation Manager for Power Programmers          �\n"\    
"; �                                                                               �\n"\ 
"; 藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�"  
