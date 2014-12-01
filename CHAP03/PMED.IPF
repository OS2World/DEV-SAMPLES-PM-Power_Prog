.* ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
.* บ  FILENAME:      PMED.IPF                                         mm/dd/yy     บ
.* บ                                                                               บ
.* บ  DESCRIPTION:   Help Source File for PMED/Chapter 03 Sample Program           บ
.* บ                                                                               บ
.* บ  NOTES:                                                                       บ
.* บ                                                                               บ
.* บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
.* บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
.* บ                                                                               บ
.* บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
.* บ                                                                               บ
.* ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

.im pmed.inc
:userdoc.
:title.Help for PMED

:h1 res=&HID_GENERAL..Help for PMED
:p.Welcome to PMED - a simple PM Text Editor.
:p.This program allows you to load, edit, and save simple
text files.  It is not intended for extensive use when
developing programs or text, but may prove to be very useful
for editing files like :hp1.config.sys:ehp1., whose lines
may be very long but cannot be split.  Many of the more
advanced editors cannot deal with this situation.
:p.For more information on the functions available from the
menus, double click one of the hypertext links below.
:p.:link reftype=hd res=100.FILE:elink.
:p.:link reftype=hd res=200.HELP:elink.
:p.This program was written as the sample program for
Chapter 3 of the book, :hp5.OS/2 WARP Presentation Manager
for Power Programmers:ehp5.

:h2 res=&HID_FILE_MENU..Help for File
:p.The items under the File Menu allow you to manipulate
the file being edited.  Click the hypertext for a more
detailed description of each item.
:dl tsize=20.
:dt.:link reftype=hd res=&HID_FILE_NEW..New:elink.:dd.Start editing a new, empty file.
:dt.:link reftype=hd res=&HID_FILE_OPEN..Open&per.&per.&per.:elink.:dd.Open an existing file.
:dt.:link reftype=hd res=&HID_FILE_SAVE..Save:elink.:dd.Save the contents back to the file.
:dt.:link reftype=hd res=&HID_FILE_SAVEAS..Save as&per.&per.&per.:elink.:dd.Save the contents to a different file.
:dt.:link reftype=hd res=&HID_FILE_EXIT..End:elink.:dd.Close the editor.
:edl.

:h3 res=&HID_FILE_NEW..Help for New
:p.The :hp2.NEW:ehp2. function provides a new, empty file.
:p.If the file currently being edited has changed since it was opened,
NEW gives the user the opportunity to save the file before the editor's
buffers are erased.

:h3 res=&HID_FILE_OPEN..Help for Open...
:p.The :hp2.OPEN:ehp2. function loads the contents of a file into
the editor's buffers for modification.
:p.If the file being edited when OPEN is selected has been modified,
the user is given an opportunity to save the current file before the
OPEN operation proceeds.

:h3 res=&HID_FILE_SAVE..Help for Save
:p.The :hp2.SAVE:ehp2. function writes the contents of the editor's
buffers back to the currently open file.

:h3 res=&HID_FILE_SAVEAS..Help for Save as...
:p.The :hp2.SAVE AS:ehp2. function allows the user to save the
contents of the editor's buffer to a different file from the currently
open file.
:p.Following the operation, the new file becomes the currently open
file.

:h3 res=&HID_FILE_EXIT..Help for Exit
:p.The :hp2.EXIT:ehp2. functions terminates the editor.  If the current
file has been modified, the user is allowed to save the file before
the program terminates.
:note.The :hp2.Close:ehp2. function of the system menu performs the same
operation; however, closing the application from the window list does not allow
the user to save the file.

:h2 res=&HID_HELP_MENU..Help for Help
:p.The items under the Help Menu provide the user with information
on how the editor operates.  Click the hypertext for a more detailed
description of each item.
:dl tsize=20.
:dt.:link reftype=hd res=&HID_HELP_GEN..General help:elink.:dd.Help for PMED
:dt.:link reftype=hd res=&HID_HELP_USE..Using help:elink.:dd.Help for the Help Subsystem
:dt.:link reftype=hd res=&HID_HELP_KEYS..Keys help:elink.:dd.Key assignments for PMED
:dt.:link reftype=hd res=&HID_HELP_PROD..Product information:elink.:dd.About PMED
:edl.

:h3 res=&HID_HELP_GEN..Help for General help
:p.The :hp2.General Help:ehp2. function presents a help panel that gives
an overview of the editor and contains hypertext links to other
useful information.

:h3 res=&HID_HELP_USE..Help for Using help
:p.The :hp2.Using Help:ehp2. function presents a help panel that
explains how to obtain help and use the OS/2 Help Subsystem.

:h3 res=&HID_HELP_KEYS..Help for Keys help
:p.The :hp2.Keys Help:ehp2. function presents a help panel that
describes the key assignments used by the editor.

:h3 res=&HID_HELP_PROD..Help for Product information...
:p.The :hp2.Product Information:ehp2. function provides general identifying
information about PMED.

:h1 res=&HID_NOT_SAVED..Help for Warning (File Not Saved).
:p.This dialog is presented when you attempt to close the
application or change files without saving a file which
you have modified.  The pushbuttons provide you with three
options:
:dl.
:dt.:hp2.Save as&per.&per.&per.:ehp2.
:dd.Clicking this button will raise a dialog box where
you may specify the name of the file in which to save the
current contents of the editor's buffers.
:dt.:hp2.Discard:ehp2.
:dd.Clicking on this button will cause the current contents
of the editor's buffers to be deleted without the changes
being saved.
:dt.:hp2.Cancel:ehp2.
:dd.When enabled, clicking this button will
cancel the requested operation and allow you to continue
to work with the current contents of the editor's buffers.
:p.This button is disabled, when a request has been made to
close the operation.
:edl.
:note.If the application is closed from the Window List, this
dialog will not be presented and any changes to the editor's
buffers will be automatically discard.

:h1 res=&HID_KEYS_HELP..PMED Keys Help
:dl tsize=25.
:dt.:hp2.PAGE UP:ehp2.:dd.Move the input cursor up one page.
:dt.:hp2.CTRL+PAGE UP:ehp2.:dd.Scroll one page left.
:dt.:hp2.PAGE DOWN:ehp2.:dd.Move the input cursor down one page.
:dt.:hp2.CTRL+PAGE DOWN:ehp2.:dd.Scroll one page right.
:dt.:hp2.HOME:ehp2.:dd.Move the input cursor to the beginning of the current line.
:dt.:hp2.CTRL+HOME:ehp2.:dd.Move the input cursor to the first character of the file.
:dt.:hp2.END:ehp2.:dd.Move the input cursor beyond the end of the current line.
:dt.:hp2.CTRL+END:ehp2.:dd.Move the input cursor beyond the last character in the file.
:dt.:hp2.ENTER:ehp2.:dd.Insert a new, blank line in the file.
:dt.:hp2.NEWLINE:ehp2.:dd.Insert a new, blank line in the file.
:dt.:hp2.CTRL+BACKSPACE:ehp2.:dd.Remove the current line from the file.
:dt.:hp2.BACKSPACE:ehp2.:dd.Delete the character to the
left of the input cursor.
:dt.:hp2.DELETE:ehp2.:dd.Delete the character under the
input cursor in overstrike mode or the character to the
to the right of the cursor in insert mode.
:dt.:hp2.INSERT:ehp2.:dd.Toggle between insert and
overstrike mode.
:dt.:hp2.Up Arrow:ehp2.:dd.Move the input cursor up one
line, scrolling if necessary to keep the cursor line in
the display area.
:dt.:hp2.Down Arrow:ehp2.:dd.Move the cursor down by one
line, scrolling if necessary to keep the cursor line in
the display area.
:dt.:hp2.Left Arrow:ehp2.:dd.Move the cursor left by one
column, scrolling if necessary to keep the cursor column
in the display.
:dt.:hp2.Right Arrow:ehp2.:dd.Move the cursor to the right by
one column, scrolling if necessary to keep the cursor
column in the display.
:edl.

:euserdoc.
