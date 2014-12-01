.* ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป          
.* บ  FILENAME:      SHOWOFF.IPF                                      mm/dd/yy     บ          
.* บ                                                                               บ          
.* บ  DESCRIPTION:   Help Source File for SHOWOFF/Chapter 15 Sample Program        บ          
.* บ                                                                               บ          
.* บ  NOTES:         This file contains all the help text for the program.         บ          
.* บ                                                                               บ          
.* บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ          
.* บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ          
.* บ                                                                               บ          
.* บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ          
.* บ                                                                               บ          
.* ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ          
:userdoc.
.*
.* =================================================================================
.* Help for the help button in the Open Standard File Dialog
.* =================================================================================
:h1 res=20000.Help for Open
:p.
This standard file dialog is used to select a bitmap file to be :hp2.Opened:ehp2. for 
display.  To use the standard file dialog box simply:
:ol compact.
:li.Select the down arrow to the right of the :hp2.Drive:ehp2. list to
display all the drives on your system.
:li.Select a drive from the :hp2.Drive:ehp2. list.
:li.Select a directory from the :hp2.Directory:ehp2. list.
:li.Select a bitmap file from the :hp2.File:ehp2. list or type in the path and 
filename of the bitmap file that you would like to view in the "Open filename"                                                   
entryfield.
:eol.
:p.
.*
.* =================================================================================
.* Help for the Configuring User Preferences Dialog Box
.* =================================================================================
.*
:h1 res=20817.Configuring User Preferences Dialog
:p.
This dialog allows the user to configure a set of global options
for :hp2.SHOWOFF:ehp2. depending on user preference.  You can  
decide whether or not to configure the following options:
:p.
:hp2.Suppress Logo on Startup -:ehp2. this option if set will not display
the Product Information Dialog Box, when SHOWOFF is started.
:p.
:hp2.Display Bitmap as Actual Size -:ehp2. this option will cause the 
individual bitmaps to be displayed in their actual size, rather than 
having the bitmap stretched to fit the size of the window.          
:p.
:hp2.Save Window Position on Exit -:ehp2. this option if set will    
automatically save the window position coordinates each time the     
application is exited.                                              

.* =================================================================================
.* Help for Creating a Presentation 
.* =================================================================================
:h1 res=20998.Creating A Presentation File
:p.
This dialog box allows the user to link a series of bitmap files 
together to form a Presentation.  To use this dialog box simply enter  
enter the path and filename of a valid bitmap file in the 
"Bitmap File to Add" entryfield, or use the "Find" pushbutton 
to display the standard file dialog, which will allow you to 
locate a bitmap.  Once you have entered a bitmap file in the 
entryfield, click the "Add" pushbutton to add the bitmap to 
the "Bitmaps in Presentation" listbox.  The "Remove" pushbutton will 
allow you to remove the selected bitmap file from the 
"Bitmaps in Presentation" listbox.
:p.
The :hp2.Presentation Options:ehp2. groupbox contains two options 
that determine how a presentation will be displayed.  
:p.
:artwork align=center name='options.bmp'.    
:p.
The "Automatically Restart Presentation" checkbox will restart the presentation over 
again, once the last bitmap in the presentation has been displayed.
The "Run this Presentation Full Screen" checkbox if selected will 
display each of the bitmaps in "Full Screen" mode.  Full Screen mode 
is actually a maximized window with the frame controls removed, not an OS/2 
Fullscreen session.  Full Screen mode is helpful for viewing Presentations, since 
it allows you to just view the bitmaps themselves.
:p.
The "Default Time Between Bitmaps" spinbutton allows you to configure the amount 
of time each bitmap in the Presentation should be displayed.  The default value 
is 5 seconds.  The value can be anywhere from 0 seconds to 60 seconds.  If the value
is set to 0, the Bitmap files in a Presentation will not automatically advance to 
the next bitmap.  The user must use the menuitems from the Display Submenu to manually
navigate through the bitmap files in a Presentation.  The "Display This Bitmap For"
spinbutton allows you to configure the amount of time to display a specific bitmap.
Any value specified here will override the default time value.  
:p.
The "Find" pushbutton will display the Standard File Dialog, so that you can select
a Bitmap file to Add.  The "Add" pushbutton will copy the Bitmap specified in the 
"Bitmap File To Add" entryfield to the "Bitmaps in Presentation" listbox.  The 
"Remove" pushbutton will remove a Bitmap file from the listbox.


.* =================================================================================
.* Help for Editing a Presentation
.* =================================================================================
:h1 res=20999.Editing A Presentation File
:p.
This dialog box allows the user to edit an existing presentation.
You will notice that this dialog is a slightly modified version of     
the "Create a Presentation" dialog box.  The filename of the Presentation                      
File that you have selected to edit appears in the titlebar of the 
dialog window.
:p.
The "Bitmaps in Presentation" listbox will display the path and filenames
of all the bitmaps in the selected presentation along with the amount of time
in seconds that a particular bitmap should be displayed.  
:p.
For Example:
:p.
:artwork align=center name='listbox.bmp'.   
:p. 
The above sample indicates that the bitmap file "WINLOGO.BMP" will be displayed
for 3 seconds, while the bitmap file "FANTASIA.BMP" will be displayed for 4 seconds.
:p.
If you would like to modify a particular bitmap or time value simply select the 
appropriate line in the listbox.  Once an entry is selected, the bitmap path and 
filename, along with any time value specified will be displayed in the "Bitmap File and Options"
entryfield.  
:p.
:artwork align=center name='entryfld.bmp'.   
:p.
You can modify the bitmap filename or time value in the entryfield if you desire.  The   
above example changes the time this bitmap will be displayed to 6 seconds.  
Once you have made the changes you wish to make, simply press the 
"Change" pushbutton to make the appropriate changes to the Presentation.
The modified entry will replace the original entry in the listbox.  Once you are 
finished editing the Presentation, you can press the "Save" pushbutton to save the 
Presentation File back to the filename that you edited, or you may save the Presentation
to another Presentation File.  You can use the "Add" or "Remove" pushbuttons to add or 
delete entries from the Presentation.

:p.                                                                     
For a detailed explanation of each dialog control used in this dialog, select          
below&colon.                                                            
:ul compact.                                                            
:li.:link res=20998 reftype=hd.Creating a Presentation:elink.                    
:eul.                                                                   

.* =================================================================================
.* Help for the General Help Menuitem
.* =================================================================================
:h1 res=20409.SHOWOFF - General Help
:p.
.*
:artwork align=center name='help.bmp'.
.*
:p.
:hp2.SHOWOFF:ehp2. is the sample program for Chapter 15.  This program 
is a complete Bitmap Viewer that demonstrates the concepts discussed 
in the corresponding text.  The program provides insight into the 
Graphical Programming Interface (GPI) API, and also delves into 
programming topics, such as, drawing bitmaps, the clipboard, displaying 
a bitmap on the OS/2 desktop, and the palette manager.
:p.
:hp2.SHOWOFF:ehp2. allows a user to display bitmap files designed for OS/2 
or Windows.  The user may view individual bitmap files, or combine a series 
of bitmap files to create a "Presentation File".  A "Presentation File" is 
simply an ASCII text file with some simple tags.  The program provides a
method for creating and editing a Presentation File. 
:p. 
The File Submenu contains options for Displaying an individual Bitmap file or  
Creating, Editing, and Previewing a Presentation.  The Create Presentation 
Dialog Box contains different control windows that guide you through entering
the path and filenames of the Bitmaps you want to see in a Presentation along 
with options for how the Bitmap files are to be displayed.  The Edit Presentation
Dialog Box allows you to edit an already created Presentation.  The Presentation 
File created by the application is a simple ASCII file that ends in a .URI
extension.  When a Presentation is created by the Create Presentation Dialog Box
an explanation of the tags is written to the Presentation File along with the 
Bitmap Path and Filenames and any specified options.  If you prefer, you can 
use a text editor like the OS/2 system editor or EPM to modify your Presentation
Files, or you can use the Edit Presentation menuitem.
:p.
The File Submenu also contains a Preview Presentation menuitem that will allow
you to "Preview" a Presentation that you created.  This allows you to view all 
of the Bitmaps in a Presentation so that you can get an idea of what the Bitmaps
in the Presentation look like.                          
:p.
The Display Submenu allows you to control the actual Presentation.  The Start 
Presentation menuitem will invoke the standard file dialog to allow you to select
a Presentation File that you want to display.  The rest of the menuitems in the 
Display Submenu work like the buttons on your VCR.  You can use the Forward, 
Rewind, Pause, Restart or Stop options to navigate your way through a Presentation.
:p.
The Edit Submenu should look extremely familiar to you by now...  It allows you 
to Cut and Copy to the PM Clipboard or Paste from the OS/2 Clipboard/
:p. 
The :hp2.SHOWOFF:ehp2. application is a simple yet effective utility that you 
can use to create computerized Presentations that can make a great learning 
tool or tutorial.  We hope you enjoy the it!!!

.* =================================================================================
.* Help for the Keys Help Menuitem
.* =================================================================================
:h1 res=20408.SHOWOFF - Keys Help
:p.
The following are the key assignments for :hp2.SHOWOFF:ehp2.&colon.
:dl compact break=all.
:dt.:hp2.F2 = :ehp2.Open Bitmap
:dd.Allows you to select and display a Bitmap file.
:p.
:dt.:hp2.F3 = :ehp2.Exit SHOWOFF
:dd.Exits the application.
:p.
:dt.:hp2.F5 = :ehp2.Create Presentation
:dd.Allows you to select a series of bitmap files to create a 
Presentation File.  The dialog box allows you to define which 
bitmaps shall compose the Presentation, in addition to configuring
options that control how the Presentation is displayed.
:p.
:dt.:hp2.F6 = :ehp2.Edit Presentation
:dd.Allows you to edit/change the Presentation File.  The dialog 
box allows you to change the bitmaps that compose a Presentation,  
along with the Presentation options, and the amount of time that 
each bitmap will be displayed.
:p.
:dt.:hp2.F7 = :ehp2.Preview Presentation
:dd.Allows you to view the bitmap files that compose a 
Presentation.  This option creates another frame window called the 
"Presentation Viewer" that will display "thumbnail" representations 
of each bitmap in the selected Presentation.
:p.
:dt.:hp2.F8 = :ehp2.Start Presentation
:dd.Allows you to view a Presentation.  This option displays a 
a series of bitmaps that you selected for an amount of time that 
you can configure.
:p.
:dt.:hp2.F9 = :ehp2.Pause 
:dd.This option stops on the current bitmap in the Presentation.  Thus, the 
Presentation does not automatically advance to the next bitmap.  This option 
is similar in function to the "Pause" function of a VCR.  Once this menuitem 
is selected, the Presentation is paused and the menuitem text is changed to 
read "Play".  Selecting the Play menuitem will resume the Presentation.
:p.
:dt.:hp2.F10 = :ehp2.Forward
:dd.This option automatically advances to the next image in
the Presentation.
:p.
:dt.:hp2.F11 = :ehp2.Rewind
:dd.This option automatically moves to the previous image in
the Presentation.
:p.
:dt.:hp2.F12 = :ehp2.Restart Presentation
:dd.This option restarts the Presentation by automatically
returning to the first image in the presentation while the
current presentation is running.
:p.
:dt.:hp2.Space Bar = :ehp2.Stop Presentation
:dd.This option stops the currently executing Presentation and
clears the Presentation window.
:p.
:dt.:hp2.Del = :ehp2.Clear Image
:dd.This option clears the presentation window if an image is
opened for display or editing purposes.
:p.
:dt.:hp2.Ctrl+D = :ehp2.Change Desktop Background
:dd.This option allows the user the simplicity of changing the
OS/2 Desktop Background without opening the Desktop settings notebook.
:p.
:dt.:hp2.Ctrl+L = :ehp2.Lockup Keybaord and Mouse
:dd.This option calls the OS/2 function lockup to lockup your
Keyboard and Mouse.  You must already have a lockup password
enabled for lockup to occur immediately.  Do not forget your
password...  Do NOT use this option if you are using the SHOWOFF
program on another machine that you do not know the password for.
:p.
:dt.:hp2.Ctrl+F = :ehp2.Hide Frame Controls
:dd.This option allows a Presentation to be displayed in the
Presentation window without the clutter of frame window controls like 
the Application Menu and Title Bar.  All accelerator keys are still available.
Use Ctrl+F as a toggle to restore the frame controls.  This option
can be especially useful if you are using :hp2.SHOWOFF :ehp2.in
conjunction with a screen capture program by maximizing the image
and removing the frame controls; thereby, allowing the screen
capture program to get full screen images.  Try it with the PMSCREEN 
screen capture program that you will see in Chapter 17.  You can also
use your left mouse button to double click anywhere in the client area
to toggle the frame controls.  When creating a Presentation, you can 
automatically have a Presentation display in Full Screen mode.
:p.
:dt.:hp2.Ctrl+S = :ehp2.Save Window Size and Position
:dd.This option allows the user to save the current window size
and position, so that the next time :hp2.SHOWOFF :ehp2.is started
it will restore to the screen coordinates that you specify rather
than the application default.
:p.
:dt.:hp2.Ctrl+U = :ehp2.Configure User Preferences
:dd.This option will display the User Preferences Dialog Box which 
allows you to configure various global options that will be used       
by the application.
:p.
:dt.:hp2.Ctrl+R = :ehp2.Display Current Screen Resolution
:dd.This option will display the current screen resolution.          
:edl.      
:p.        
To jump to the General Help window select :link reftype=hd
res=20409.General Help:elink..
:euserdoc.
