.* ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
.* บ  FILENAME:      THREADS.IPF                                       mm/dd/yy    บ
.* บ                                                                               บ
.* บ  DESCRIPTION:   IPF Source File for THREADS/Chapter 06 Sample Program         บ
.* บ                                                                               บ
.* บ  NOTES:         This program demonstartes several methods for implmenting     บ
.* บ                 long running operations in PM applications.                   บ
.* บ                                                                               บ
.* บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
.* บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
.* บ                                                                               บ
.* บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
.* บ                                                                               บ
.* ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
.*
.im threads.inc
:userdoc.
:title.Help for THREADS
:h1 res=&HID_GENHELP..Help for Threads
:p.PM applications which may obtain the input focus must be
programmed to process all messages in a relatively short
period of time.  This becomes a problem when the application
provides functionality which may take some time to complete.
This application provides examples of four methods which
may be used for performing this type of operation.
:p.The first method simply ignores the problem and performs
the operation in a straightforward manner.  Unfortunately,
some PM applications are actually programmed in this manner
and severely limit the user's ability to perform multiple
tasks.  To see this method in action, select the
:link reftype=hd res=&HID_MODESINGLE..Single thread:elink. item
under the :link reftype=hd res=&HID_MODE..Mode:elink. menu.
:p.The second method is similar to the first but occasionally
examines the message queue to retrieve and process any messages
which may be waiting.  This method is adequate for operations
which are totally processor bound and cannot block within an
API call.  To see this method in action, select the
:link reftype=hd res=&HID_MODEPEEK..Peek loop:elink. item under
the :link reftype=hd res=&HID_MODE..Mode:elink. menu.
:p.The third method spawns a new non-PM thread to perform the
operation.  This method allows the operation to be performed
in the same straightforward way as the first method, but
restricts or prevents the main thread's ability to participate
in the operation. To see this method in action, select the
:link reftype=hd res=&HID_MODEMULT..Multithread:elink. item under
the :link reftype=hd res=&HID_MODE..Mode:elink. menu.
:p.The fourth, and final, method also uses a secondary thread,
but creates an object window in the thread.  Since the object
window never receives the input focus, it may generally ignore
the rules about message processing time.  Messages may be
exchanged between the object window and the main application
window, allowing the primary thread to participate in the operation
as necessary.  To see this method in action, select the
:link reftype=hd res=&HID_MODEOBJ..Object window:elink. item
under the :link reftype=hd res=&HID_MODE..MODE:elink. menu.

:h2 res=&HID_FILE..Help for File
:p.Beneath the file menu are items which allow the user to
:link reftype=hd res=&HID_FILEINITIALIZE..initialize:elink. a
file, :link reftype=hd res=&HID_FILEDELETE..Delete:elink. a file,
and :link reftype=hd res=&HID_FILEEXIT..exit:elink. the application.

:h3 res=&HID_FILEINITIALIZE..Help for Initialize
:p.This menu causes a one megabyte file to written to a disk
storage device.  For demonstration purpoaes, the time required to
complete the operation has been extended by writing to each
individual byte of the file.  Various methods of performing this
operation may be observed by selecting one of the items under the
:link reftype=hd res=&HID_MODE..Mode:elink. menu.

:h3 res=&HID_FILEDELETE..Help for Delete
:p.This menu item may be used to delete a file and is intended to
remove the file created by the File :link reftype=hd res=&HID_FILEINITIALIZE..
Initialize:elink. menu item.

:h3 res=&HID_FILEEXIT..Help for Exit
:p.Selecting this menu item causes the application to be closed.

:h2 res=&HID_MODE..Help for Mode
:p.The selections under the Mode menu allow you to determine
the method used to initialize the file when the
:link reftype=hd res=&HID_FILEINITIALIZE..Initialize:elink. selection
is chosen from the File menu.  The method currently selected is
highlighted with a check mark.  The available methods are:
:dl tsize=25.
:dt.:link reftype=hd res=&HID_MODESINGLE..Single thread:elink.
:dd.Initialize the file in the main thread with no peek loop.
:dt.:link reftype=hd res=&HID_MODEPEEK..Peek loop:elink.
:dd.Initialize the file in the main thread using a peek loop.
:dt.:link reftype=hd res=&HID_MODEMULT..Multithread:elink.
:dd.Start a new thread to initialize the file.
:dt.:link reftype=hd res=&HID_MODEOBJ..Object window:elink.
:dd.Initialize the file using an object window in a separate
thread.
:edl.

:h3 res=&HID_MODESINGLE..Help for Single thread
:p.When this menu item is checked, the
application completely initializes a one megabyte file
during the processing of a single message.  This means that no
input is recognized until the file has been initialized and the
system appears to be "hung" throughout the operation.

:p.This method is not recommended and is actually considered to
be an error on the part of the programmer.

:h3 res=&HID_MODEPEEK..Help for Peek loop
:p.This method of initializing a file is similar to the
single thread method, but retrieves and process messages
from the queue at regular intervals.

:p.While this method is appropriate for long-running operations that
function totally within the processors memory, such as searches or
mathematical calculations on large arrays, operations which require
access to devices outside the system, and thus may have to wait for
those devices, may still cause apparent system hangs.

:h3 res=&HID_MODEMULT..Help for Multithread
:p.When this menu item is checked, file initialization occurs
in a secondary thread started expressly for the purpose of
performing the initialization operation.

:p.This method solves the I/O problem seen with the peek loop
method; but, under most circumstances, all data required for
the operation must be available when the operation is started.

:h3 res=&HID_MODEOBJ..Help for Object window
:p.When this menu item is checked, file initialization is
accomplished through a conversation between the application
window and an object window created in a secondary thread.
The object window is responsible for all I/O operations
and the application window is responsible for supplying
data to the operation in small chunks.

:p.This method resolves the I/O problem of the peek loop
method and, unlike the multithread method, allows the
application to format and provide the data on an as
needed basis as the operation progresses.

:h2 res=&HID_HELP..Help for Help
:p.The selections under this menu item provide access
to on-line help for the application.  The available
selections are:
:ul.
:li.:link reftype=hd res=&HID_HELPGENERAL..General help:elink.
:li.:link reftype=hd res=&HID_HELPUSING..Using help:elink.
:li.:link reftype=hd res=&HID_HELPPRODINFO..Product information:elink.
:eul.

:h3 res=&HID_HELPGENERAL..Help for General help
:p.This menu selection displays a panel which provides
general information for the application.

:h3 res=&HID_HELPUSING..Help for Using help
:p.This menu selection displays help for using the
help subsystem.

:h3 res=&HID_HELPPRODINFO..Help for Product information
:p.This menu item displays information about the product,
such as, version, authors, copyrights, etc.

:euserdoc.
