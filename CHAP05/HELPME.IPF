.* ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
.* บ  FILENAME:      HELPME.IPF                                       mm/dd/yy     บ
.* บ                                                                               บ
.* บ  DESCRIPTION:   IPF Source File for HELPME/Chapter 05 Sample Program          บ
.* บ                                                                               บ
.* บ  NOTES:         This program demonstartes the use of the OS/2 IPF facility.   บ
.* บ                                                                               บ
.* บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
.* บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
.* บ                                                                               บ
.* บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
.* บ                                                                               บ
.* ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
.*
#define  __IPFC__
#include "helpme.h"
:userdoc.
:title.Help for HELPME
.* --------------------------------------------------------------------------
.* General help for the application
.* --------------------------------------------------------------------------
:h1 res=HID_MAINEXTENDED.Help for HELPME Sample Program
:p.HELPME.EXE is a template of a PM application which
demonstrates the coding necessary to utilize OS/2&apos.s Information
Presentation Facility, IPF.
:p.You will find that many of the menu items do not actually perform
any useful application function, but try all the different means of
acquiring help. For instance, press the ALT key to highlight the file
menu, then press F1 to display the Help panel for that menu item.
Press ESC when you are ready to close the Help panel.  If you activate
the File New menu item, a message box will be displayed which
indicates that the function is not supported.  Press F1 or click the
Help button to get additional information.

.* --------------------------------------------------------------------------
.* Help for the primary menu items
.* --------------------------------------------------------------------------
:h2 res=HID_FILE.Help for File Menu
:i2 refid=5000.File
:p.The primary use of the file menu is the movement of files between
the application and the storage device.  The functions which may be
performed are&colon.
:dl.
:dt.:link reftype=hd res=HID_FILENEW.New:elink.
:dd.Begin work on a new, empty file.
:dt.:link reftype=hd res=HID_FILEOPEN.Open:elink.
:dd.Read an existing file from the storage device.
:dt.:link reftype=hd res=HID_FILESAVE.Save:elink.
:dd.Write application data to the current file.
:dt.:link reftype=hd res=HID_FILESAVEAS.Save as:elink.
:dd.Write application data to a new or different file.
:edl.
:p.The file menu also contains an :link reftype=hd
res=HID_EXIT.Exit:elink. item which ends the application when
selected.

.* --------------------------------------------------------------------------
.* Help for FILE menu items
.* --------------------------------------------------------------------------
:h3 res=HID_FILENEW.Help for New
:p.Use this menu item to clear the application&apos.s buffer in
preparation for working with a new file.  If the current file has been
changed you will be asked if you wish to save the changes before the
application contents are erased.

:h3 res=HID_FILEOPEN.Help for Open...
:p.Use this menu item to load a file from the storage device into the
application&apos.s buffer. :p.When the menu item is selected a dialog
box is raised which allows you to change storage devices and
directories, and to select a file by name.
:p.See also
:p.General Help for
:link reftype=hd res=HID_FILEOPENDLG.File Open Dialog:elink.

:h3 res=HID_FILESAVE.Help for Save
:p.Use this menu item to save the state of the
application&apos.s data buffer to the current file on the storage
device.  You may then continue working with the application buffer.

:h3 res=HID_FILESAVEAS.Help for Save as...
:p.Use this menu item to save the current state of the
application&apos.s data buffer to a new or different file. :p.When
this item is selected a dialog is raised where the name of the file
may be entered, or where an existing file can be selected. The
original file is not altered by this operation unless it selected from
the dialog box. :p.Following this operation, the file to which the
application buffer was saved becomes the current file. :p.See also
:p.General Help for
:link reftype=hd res=HID_FILESAVEASDLG.File Saveas Dialog:elink.
:p.:link reftype=hd refid='More Save As'.Additional Help:elink.
:note.This last link provides an example of a link to a panel which cannot
be directly accessed by the user.  The panel is not displayed in the
table of contents because it is marked with the "hide" attribute.

:h3 res=HID_EXIT.Help for Exit
:p.Select this menu item to close the application.

.* --------------------------------------------------------------------------
.* Help for SAMPLE menu items
.* --------------------------------------------------------------------------
:h2 res=HID_SAMPLE.Help for Sample Menu
:i2 refid=5000.Sample
:p.The Sample menu subitems access several common types of windows which
an application might employ.  While the function of the windows is minimal
the source code serves as a good example of how to code online help for each
of the window types.  Select the following links for a further description
of each window type.
:dl tsize=20.
:dt.:link reftype=hd res=HID_SAMPLECHLD.Child Window:elink.
:dd.Sample of a child window (a frame window whose parent is not the
desktop).
:dt.:link reftype=hd res=HID_SAMPLEDLG.Dialog...:elink.
:dd.Sample of a dialog window.
:dt.:link reftype=hd res=HID_SAMPLEMSG.Message Box...:elink.
:dd.Sample of a message box with a Help button.
:dt.:link reftype=hd res=HID_SAMPLETHD.Thread...:elink.
:dd.Sample of a window created in a separate thread.
:edl.

:h3 res=HID_SAMPLECHLD.Help for Child Window
:p.Use this menu item to display a sample child window.  The PARENT window
is the main application window, not the desktop.

:h3 res=HID_SAMPLEDLG.Help for Dialog...
:p.Use this menu item to raise a typical dialog window.  This type of window
is handled automatically by IPF.  The only coding changes required occur in
the resource file when a HELPITEM and HELPSUBTABLE are added for the window.
:p.General Help for
:link reftype=hd res=HID_SAMPLEDLGEXT.Sample Dialog.:elink.

:h3 res=HID_SAMPLEMSG.Help for Message Box...
:p.Use this menu item to raise a message box with a Help pushbutton.  Note
that the message box is coded in the HELPTABLE as normal.  However, the
HELPSUBTABLE for the window is empty since we did not provide help for
the OK pushbutton.

:h3 res=HID_SAMPLETHD.Help for Thread
:p.Use this menu item to start an additional thread which creates a
frame window of its own.
:p.As coded in this example, the window raised by the second thread
acts as an additional instance of the application.

.* --------------------------------------------------------------------------
.* Help for Help menu items
.* --------------------------------------------------------------------------
:h2 res=HID_HELP.Help for Help Menu
:i2 refid=5000.Help
The Help menu subitems allow you to obtain online help of various
types.
:dl tsize=20.
:dt.:link reftype=hd res=HID_HELPINDEX.Help index:elink.
:dd.Use this menu item to display the index for the application&apos.s online help.
:dt.:link reftype=hd res=HID_GENHELP.General help:elink.
:dd.Use this menu item to display the applications general, or extended, help.
:dt.:link reftype=hd res=HID_USEHELP.Using help:elink.
:dd.Use this menu item to display help on how to obtain help.
:dt.:link reftype=hd res=HID_KEYSHELP.Keys help:elink.
:dd.Use this menu to display help on special uses of keys.
:dt.:link reftype=hd res=HID_ABOUT.Product information...:elink.
:dd.Use this menu to display specific information about the product.
:edl.

:h3 res=HID_HELPINDEX.Help for Help index
:p.Use this menu item to display the index panel for the application&apos.s
online help.  This panel allows you to view help pertaining to selected
topics.
:p.:link reftype=hd refid="HELP_INDEX".Programming Considerations:elink.

:h3 res=HID_GENHELP.Help for General help
:p.Use this menu item to display the window&apos.s general, or extended, help
panel.  This panel provides and overview of the function of the window.
:p.:link reftype=hd refid="GENERAL_HELP".Programming Considerations:elink.

:h3 res=HID_USEHELP name='USEHELP'.Help for Using help.
:p.Use this menu item to display an IPF defined panel which explains how to
use the help facility.
:p.:link reftype=hd refid="USING_HELP".Programming Considerations:elink.

:h3 res=HID_KEYSHELP.Help for Keys help
:p.Use this menu item to display a list of keys which have a special function
within the application.
:p.:link reftype=hd refid="KEYS_HELP".Programming Considerations:elink.

:h3 res=HID_ABOUT.Help for Product information
:p.Use this menu item to display general information about the products, such
as the formal product name, supplier, serial number, version number, etc.

.* --------------------------------------------------------------------------
.* Help for the child window
.* --------------------------------------------------------------------------
:h1 res=HID_SAMPLECHILD.Help for Child Window Example
:p.This window represents a child of the main application window.  In other
words, the PARENT window is the main application window, not the
desktop.  You will note that the title bar of both this window and
the main application window are in the "active" state.
:p.To see the menu activity at work, use the keyboard to select the File
New menu item from the main application window.  The help panel for the
main window menu item will be displayed. Then select the File New menu
item for the child window.  The help panel associated with the child window
will be displayed.

.* --------------------------------------------------------------------------
.* Help for the child window primary menu items
.* --------------------------------------------------------------------------
:h2 res=HID_CFILE.Help for File Menu
:p.Insert help for the child window file menu here.
:dl.
:dt.:link reftype=hd res=HID_CFILENEW.New:elink.
:dd.Begin work on a new, empty file.
:dt.:link reftype=hd res=HID_CFILEOPEN.Open:elink.
:dd.Read an exiting file from the storage device.
:dt.:link reftype=hd res=HID_CFILESAVE.Save:elink.
:dd.Write current contents to the file.
:dt.:link reftype=hd res=HID_CFILESAVEAS.Save as:elink.
:dd.Write current contents to a different or new file.
:edl.
:p.The file menu also contains an :link reftype=hd res=HID_CEXIT.Exit:elink.
item.  Selecting this item closes the child window.

.* --------------------------------------------------------------------------
.* Help for child window FILE menu items
.* --------------------------------------------------------------------------
:h3 res=HID_CFILENEW.Help for New
:p.Insert help for the child window file new menu item here.

:h3 res=HID_CFILEOPEN.Help for Open...
:p.Insert help for the child window file open menu item here.

:h3 res=HID_CFILESAVE.Help for Save
:p.Insert help for the child window file save menu item here.

:h3 res=HID_CFILESAVEAS.Help for Save as...
:p.Insert help for the child window file saveas menu item here.

:h3 res=HID_CEXIT.Help for Exit
:p.Selecting this menu item closes the child window.

.* --------------------------------------------------------------------------
.* Help for child window Help menu items
.* --------------------------------------------------------------------------
:h2 res=HID_CHELP.Help for Help Menu
Insert help for the help menu here.
:dl tsize=20.
:dt.:link reftype=hd res=HID_CHELPINDEX.Help index:elink.
:dd.Use this menu item to display the index for the application&apos.s online help.
:dt.:link reftype=hd res=HID_CGENHELP.General help:elink.
:dd.Use this menu item to display the child window&apos.s general, or extended, help.
:dt.:link reftype=hd res=HID_CUSEHELP.Using help:elink.
:dd.Use this menu item to display help on how to obtain help.
:dt.:link reftype=hd res=HID_CKEYSHELP.Keys help:elink.
:dd.Use this menu to display help on special uses of keys.
:edl.

:h3 res=HID_CHELPINDEX.Help for Help index
:p.Use this menu item to display the index panel for the application&apos.s
online help.  This panel allows you to view help pertaining to selected
topics.
:note.The Help Index is the same for both the child window and the main
window, since there is only one index for the entire help library.

:h3 res=HID_CGENHELP.Help for General help
:p.Use this menu item to display the window&apos.s general, or extended, help
panel.  This panel provides and overview of the function of the window, in this
instance the child window.

:h3 res=HID_CUSEHELP.Help for Using help.
:p.Use this menu item to display an IPF defined panel which explains how to
use the help facility.
:note.The Using Help panel is the same for both the child window and the main
window.

:h3 res=HID_CKEYSHELP.Help for Keys help
:p.Use this menu item to display a list of keys which have a special function
within the application.

.* --------------------------------------------------------------------------
.* Help for the File Open Dialog
.* --------------------------------------------------------------------------
:h1 res=HID_FILEOPENDLG.Help for File open dialog
:p.The fields and buttons contained in the dialog are&colon.
:sl compact.
:li.:link reftype=hd res=HID_OPEN_FILENAME.Open Filename:elink.
:li.:link reftype=hd res=HID_OPEN_FILTER.Type of File:elink.
:li.:link reftype=hd res=HID_OPEN_DRIVE.Drive:elink.
:li.:link reftype=hd res=HID_OPEN_FILES.File:elink.
:li.:link reftype=hd res=HID_OPEN_DIR.Directory:elink.
:li.:link reftype=hd res=HID_OPEN_BUTTONS.OK/Cancel/Help:elink.
:esl.

:h2 res=HID_OPEN_FILENAME.Help for Open Filename.
:p.The :hp1.Open Filename:ehp1. field may be used to enter the name
of the file to be opened.  You may enter a full path name, a path name
relative to the current directory, or the name of a file in the
current directory.
:p.You may also use the "?" and "*" wildcard characters in the file name
to filter the files displayed in the :hp1.File:ehp1. list.  If a drive or
directory (the backslash and wildcard character(s) must also be entered for
directories) the lists will be updated to show the new information.
:p.If you elect to choose the path and filename from the list boxes, the
selected name will be displayed in this field.

:h2 res=HID_OPEN_DRIVE.Help for Drive.
:p.The :hp1.Drive:ehp1. drop-down provides a list of the available
storage devices.  When the field is selected you may use the up and down
arrow keys to scroll through the drives.  Click the arrow at the right of
the field to "drop down" a list of available drives.

:h2 res=HID_OPEN_FILTER.Help for Type of File.
:p.The :hp1.Type of File:ehp1. drop-down list allows you to select the type
of files which are shown in the :hp1.File:ehp1. list.  When this field is
selected you may use the up and down arrow keys to scroll through the file
types which the application supports.  Click the arrow at the right of the
field with the mouse to "drop down" a list of the file types supported by
the application.

:h2 res=HID_OPEN_DIR.Help for Directory.
:p.The :hp1.Directory:ehp1. list provides a hieracrchy of the directories above
and immediately below the current directory.  Selecting one of these directories
and pressing the enter key causes the :hp1.File:ehp1. list to be updated with a
list of files in the selected directory.

:h2 res=HID_OPEN_FILES.Help for Files.
:p.The :hp1.File:ehp1. list shows the files in the currently selected :hp1.Drive:ehp1.
and :hp1.Directory:ehp1.  This list only includes files which match the current
entries in the :hp1.Open Filename:ehp1. and :hp1.Type of File:ehp1. fields.
Selecting one of the files places the name in the :hp1.Open Filename:ehp1. field.
If you double-click with the mouse, the selected file is opened.

:h2 res=HID_OPEN_BUTTONS.Help for Buttons.
:p.The File Open Dialog contains 3 buttons.
:p.If the :hp1.OK:ehp1. button is "clicked" with a filename in the :hp1.Open
Filename:ehp1. field that file is opened.  If the :hp1.Open Filename:ehp1. field
contains a partial filename, the dialog is updated to reflect the files
which match the partial filename.
:p."Clicking" the :hp1.Cancel:ehp1. button terminates the dialog without
modifying the current file.
:p."Click" the :hp1.Help:ehp1. button to display help about the
currently selected field.

.* --------------------------------------------------------------------------
.* Help for the File Saveas Dialog
.* --------------------------------------------------------------------------
:h1 res=HID_FILESAVEASDLG.Help for File Save as dialog
:p.The fields and buttons contained in the dialog are&colon.
:sl compact.
:li.:link reftype=hd res=HID_SAVEAS_FILENAME.Open Filename:elink.
:li.:link reftype=hd res=HID_SAVEAS_FILTER.Type of File:elink.
:li.:link reftype=hd res=HID_SAVEAS_DRIVE.Drive:elink.
:li.:link reftype=hd res=HID_SAVEAS_FILES.File:elink.
:li.:link reftype=hd res=HID_SAVEAS_DIR.Directory:elink.
:li.:link reftype=hd res=HID_SAVEAS_BUTTONS.OK/Cancel/Help:elink.
:esl.

:h2 res=HID_SAVEAS_FILENAME.Help for Save As Filename.
:p.The :hp1.Save As Filename:ehp1. field may be used to enter the name
of the file to be created or overwritten.  You may enter a full path name,
a path name relative to the current directory, or the name of a file in the
current directory.
:p.You may also use the "?" and "*" wildcard characters in the file name
to filter the files displayed in the :hp1.File:ehp1. list.  If a drive or
directory (the backslash and wildcard character(s) must also be entered for
directories) the lists will be updated to show the new information.
:p.If you elect to choose the path and filename from the list boxes, the
selected name will be displayed in this field.

:h2 res=HID_SAVEAS_DRIVE.Help for Drive.
:p.The :hp1.Drive:ehp1. drop-down provides a list of the available
storage devices.  When the field is selected you may use the up and down
arrow keys to scroll through the drives.  Click the arrow at the right of
the field to "drop down" a list of available drives.

:h2 res=HID_SAVEAS_FILTER.Help for Save File as Type.
:p.The :hp1.Save File as Type:ehp1. drop-down list allows you to select the type
of files which are shown in the :hp1.File:ehp1. list as well as the type of the
file where the application data is saved.  When this field is
selected you may use the up and down arrow keys to scroll through the file
types which the application supports.  Click the arrow at the right of the
field with the mouse to "drop down" a list of the file types supported by
the application.

:h2 res=HID_SAVEAS_DIR.Help for Directory.
:p.The :hp1.Directory:ehp1. list provides a hieracrchy of the directories above
and immediately below the current directory.  Selecting one of these directories
and pressing the enter key causes the :hp1.File:ehp1. list to be updated with a
list of files in the selected directory.

:h2 res=HID_SAVEAS_FILES.Help for Files.
:p.The :hp1.File:ehp1. list shows the files in the currently selected :hp1.Drive:ehp1.
and :hp1.Directory:ehp1.  This list only includes files which match the current
entries in the :hp1.Open Filename:ehp1. and :hp1.Type of File:ehp1. fields.
:note.This list is for reference only, you may not select a file from the list.

:h2 res=HID_SAVEAS_BUTTONS.Help for OK.
:p.Insert help for the file saveas dialog buttons here.

.* --------------------------------------------------------------------------
.* Help for example dialog window
.* --------------------------------------------------------------------------
:h1 res=HID_SAMPLEDLGEXT.Help for Dialog Window Example
:p.Insert help for the sample dialog here.
:p.The dialog includes the following fields:
:p.:link reftype=hd res=HID_CMDENTRY.Command:elink.
:p.:link reftype=hd res=HID_PARAMETERS.Parameters:elink.
:p.Push buttons defined for the dialog box are:
:p.:link reftype=hd res=HID_DLGBUTTONS.OK/Cancel/Help:elink.

:h2 res=HID_CMDENTRY.Help for Command
.* Note that this panel is only available from the table of
.* contents or from the link in the :h1. panel above.
:p.Insert help for the command file here.
The defined commands are:
:p.:link reftype=hd refid="COMMAND_COMMAND1".COMMAND1:elink.
:p.:link reftype=hd refid="COMMAND_COMMAND2".COMMAND2:elink.

:h3 id="COMMAND_COMMAND1".Help for COMMAND1
:icmd.COMMAND1
:p.Insert help for COMMAND1 here.:p.This example shows how the IPF source
file is coded for an entry field which is used to enter a command.

:h3 id="COMMAND_COMMAND2".Help for COMMAND2
:icmd.COMMAND2
:p.Insert help for COMMAND2 here.:p.This example shows how the IPF source
file is code for an entry field which is used to enter a command.

:h2 res=HID_PARAMETERS.Help for Parameters.
:p.Enter the parameters for the selected command in this field.
:p.See the help text for the desired command to determine what parameters
are valid.

:h2 res=HID_DLGBUTTONS.Help for OK
:p.Insert help for the dialog&apos.s buttons here.

.* --------------------------------------------------------------------------
.* Help for the sample message box
.* --------------------------------------------------------------------------
:h1 res=HID_SAMPLEMSGGEN.Help for Sample Message Box
:p.This message box is presented for the purpose of illustrating
the application coding necessary to enable IPF Help support for
a message box.

.* --------------------------------------------------------------------------
.* Help for additional windows
.* --------------------------------------------------------------------------
:h1 res=HID_UNSUPPORTED.Help for unsupported function.
:p.This application is primary intended to demostrate IPF
programming and the function you have selected has not been
implemented.
:p.Select OK to continue with the application.

:h2 res=HID_DLGOK.Help for OK
:p.Select the OK button to dismiss the message box and
continue with the application.

.* --------------------------------------------------------------------------
.* Miscellaneous panels
.* --------------------------------------------------------------------------
:h1 name='More Save As' hide.Additional Help for Save as...
Insert additional help for Save As here.

:h1.Programming Guidelines
:p.The following sections provide guidelines for programming help
functions.

:h2 name="HELP_INDEX".Help Index Menu Item Programming
:p.When the Help Index menu item is selected, the application should
send the HM_HELP_INDEX message to the IPF help instance associated
with the current thread of execution.

:h2 name="GENERAL_HELP".General Help Menu Item Programming
:p.When the General Help menu item is selected, the application should
send the HM_GENERAL_HELP message to the IPF help instance associated
with the current thread of execution.

:h2 name="USING_HELP".Using Help Menu Item Programming
:p.When the Using Help menu item is selected, the application should
send the HM_DISPLAY_HELP message to the IPF help instance associated
with the current thread of execution.  Parameters mp1 and mp2 should
be set to zero values in order to allow IPF to display its own
predefined Using Help panel.

:h2 name="KEYS_HELP".Keys Help Menu Item Programming.
:p.When the Keys Help menu item is selected, the application should
send the HM_KEYS_HELP messsage to the IPF help instancd associated
with the current thread of execution.  IPF will respond with an
HM_QUERYKEYSHELP message.  The application should then return the
id of a panel that describes any special key assignments used by the
application.

:h1 res=HID_KEYSUSAGE.Keys help
:p.This panel is used to describe interaction with the application using
the keyboard.  This application has no special keys other than the
:hp1.F1:ehp1. key which is used to display context sensitive help panels.

:h1 name="MAINMENU" hide.Primary Menu Help
:i1 id=5000.Primary Menu Items
:p.The items on the applications main menu are:
:dl tsize=20.
:dt.:link reftype=hd res=HID_FILE.File:elink.
:dd.Manage application files
:dt.:link reftype=hd res=HID_SAMPLE.Sample:elink.
:dd.View sample window implementations
:dt.:link reftype=hd res=HID_HELP.Help:elink.
:dd.Obtain additional information about program elements.
:edl.
:euserdoc.
