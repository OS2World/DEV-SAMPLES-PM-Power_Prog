.* ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
.* บ  FILENAME:      CARDFILE.IPF                                     mm/dd/yy     บ
.* บ                                                                               บ
.* บ  DESCRIPTION:   Help Source File for CARDFILE/Chapter 11 Sample Program       บ
.* บ                                                                               บ
.* บ  NOTES:         This file contains all the help text for the program.         บ
.* บ                                                                               บ
.* บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
.* บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
.* บ                                                                               บ
.* บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
.* บ                                                                               บ
.* ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
.im cardfile.iph
:userdoc.
.* --------------------------------------------------------------------------
.* General Help Panel for the application
.* --------------------------------------------------------------------------
:h1 res=&APPLICATION_ID..General Help for CARDFILE
:p.CARDFILE.EXE is the sample program for Chapter 11 of the book
&odq.OS/2 Warp Presentation Manager for Power Programmers&cdq.
The source code for the program demonstrates the usage of the notebook,
value set, and slider controls.
:p.The CARDFILE program simulates a card based address and phone directory
such as is common in many offices.  Additional features include the ability
to maintain birthdays and anniversary dates along with general notes.  A type
icon may be associated with each card and a rating may be given.
:p.The primary functions of the application are accessed via the action bar menu: 
:dl tsize=10.
:dt.:link reftype=hd res=&MID_FILE..FILE:elink.
:dd.provides the functions for maintaining multiple databases.
:dt.:link reftype=hd res=&MID_CARD..CARD:elink.
:dd.provides functions for maintaining the cards and the data on them.
:dt.:link reftype=hd res=&MID_HELP..HELP:elink.
:dd.provides direct access to the online help for the application.
:edl.

.* --------------------------------------------------------------------------
.* Help for the FILE menu and sub-menus
.* --------------------------------------------------------------------------
:h1 res=&MID_FILE..Help for FILE Menu
:p.The File menu contains items which are used to open and save files.  The
menu items available are:
:dl tsize=10.
:dt.:link reftype=hd res=&MID_FILENEW..New:elink.
:dd.Empty the card file and start fresh.
:dt.:link reftype=hd res=&MID_FILEOPEN..Open:elink.
:dd.Load cards from a disk file into the card file.
:dt.:link reftype=hd res=&MID_FILESAVE..Save:elink.
:dd.Save the cards in the card file to the currently open disk file.
:dt.:link reftype=hd res=&MID_FILESAVEAS..Save as:elink.
:dd.Select a file name, then save the cards in the card file to the selected
file.
:dt.:link reftype=hd res=&MID_FILEEXIT..Exit:elink.
:dd.Exit the CARDFILE application.
:edl.

.* --------------------------------------------------------------------------
.* Help for the FILE NEW menu item
.* --------------------------------------------------------------------------
:h2 res=&MID_FILENEW..Help for NEW Menu Item
:p.The New menu item causes the card file to be emptied so that work on a
new file may be started.  No disk file is created by this command.

:p.If the current contents of the card file have been modified, a dialog will
be presented which allows the current contents to be saved before the card
file is cleared.  If the option to save is selected, the Save As dialog is
raised to allow a disk file name to be selected.

.* --------------------------------------------------------------------------
.* Help for the FILE OPEN menu item
.* --------------------------------------------------------------------------
:h2 res=&MID_FILEOPEN..Help for OPEN Menu Item
:p.The Open menu item raises the Open dialog so that a disk file name may
be selected.  The contents of the disk file are then loaded into the card
file.

:p.If the current contents of the card file have been modified, a dialog will
be presented which allows the current contents to be saved before the card
file is cleared.  If the option to save is selected, the Save As dialog is
raised to allow a disk file name to be selected.

.* --------------------------------------------------------------------------
.* Help for the FILE SAVE menu item
.* --------------------------------------------------------------------------
:h2 res=&MID_FILESAVE..Help for SAVE Menu Item
:p.The Save menu item stores the current contents of the card file back to
the currently open disk file.  This menu item is disabled when a disk file
has not been opened.

.* --------------------------------------------------------------------------
.* Help for the FILE SAVE AS menu item
.* --------------------------------------------------------------------------
:h2 res=&MID_FILESAVEAS..Help for SAVE AS Menu Item
:p.The Save as menu item raises the Save As dialog so that a file name may
be selected.  If a name is selected, the contents of the card file are stored
to the selected disk file.

.* --------------------------------------------------------------------------
.* Help for the FILE EXIT menu item
.* --------------------------------------------------------------------------
:h2 res=&MID_FILEEXIT..Help for EXIT Menu Item
:p.The Exit menu item closes the application.

:p.If the current contents of the card file have been modified, a dialog will
be presented which allows the current contents to be saved before the card
file is cleared.  If the option to save is selected, the Save As dialog is
raised to allow a disk file name to be selected.

.* --------------------------------------------------------------------------
.* Help for the CARD menu and sub-menus
.* --------------------------------------------------------------------------
:h1 res=&MID_CARD..Help for CARD Menu
:p.The Card menu contains items which allow used to manipulate the cards in
the card file.  The items contained in the menu are:
:dl tsize=10.
:dt.:link reftype=hd res=&MID_CARDCREATE..Add:elink.
:dd.Adds a new card to the card file.
:dt.:link reftype=hd res=&MID_CARDEDIT..Edit:elink.
:dd.Modifies the data on an existing card.
:dt.:link reftype=hd res=&MID_CARDREMOVE..Delete:elink.
:dd.Removes a card from the card file.
:edl.

.* --------------------------------------------------------------------------
.* Help for the CARD ADD menu item
.* --------------------------------------------------------------------------
:h2 res=&MID_CARDCREATE..Help for ADD Menu Item
:p.The Add menu item creates a new card.  When the menu item is selected the
:link reftype=hd res=&DID_EDITCARD..Add/Edit Card:elink. dialog is presented.
When the dialog has been filled, the data is used to create a new card in
the card file.  The card is placed into the file in alphabetical order based
on last name, first name and initial, and the company name.  The card file
is then positioned so that the new card is visible at the top of the card
file.

.* --------------------------------------------------------------------------
.* Help for the CARD EDIT menu item
.* --------------------------------------------------------------------------
:h2 res=&MID_CARDEDIT..Help for EDIT Menu Item
:p.The Edit menu item raises the :link reftype=hd res=&DID_EDITCARD..Add/Edit
Card:elink. dialog filled with data from the card visible at the top of the
cardfile.  Modifications to the dialog data are stored back to the card when
the dialog is closed.  If the last name, first name and initial, or company
name are changed, the card may be moved to a new location in the card file,
but will remain as the card visible at the top of the card file.

:p.This menu item is disabled when the card at the top of the card file is a
the tab card for an letter of the alphabet for which no cards are present.

.* --------------------------------------------------------------------------
.* Help for the CARD DELETE menu item
.* --------------------------------------------------------------------------
:h2 res=&MID_CARDREMOVE..Help for DELETE Menu Item
:p.The Delete menu item removes the card visible at the top of the card file.
A confirmation dialog is presented before the card is actually removed.

:p.This menu item is disabled when the card at the top of the card file is a
the tab card for an letter of the alphabet for which no cards are present.

.* --------------------------------------------------------------------------
.* Help for the HELP menu and sub-menus
.* --------------------------------------------------------------------------
:h1 res=&MID_HELP..Help for HELP Menu
:p.The Help menu contains items which are used to access on-line assistance
for the application.  The menu items are:
:dl tsize=15.
:dt.:link reftype=hd res=&MID_HELPGENERAL..General help:elink.
:dd.Raises a help panel which provides an overview of the application.
:dt.:link reftype=hd res=&MID_HELPUSING..Using help:elink.
:dd.Raises a help panel which explains the use of the on-line help subsystem.
:dt.:link reftype=hd res=&MID_HELPPROD..Production information:elink.
:dd.Raises a dialog which describes attributes of the application, such
as the title, author(s), and version.
:edl.

.* --------------------------------------------------------------------------
.* Help for the HELP GENERAL HELP menu item
.* --------------------------------------------------------------------------
:h2 res=&MID_HELPGENERAL..Help for GENERAL HELP Menu Item
:p.The General help menu item raises the general help panel for the application,
providing an overview of the application.

:p.This panel may also be accessed by pressing the F1 key during normal
operation.

.* --------------------------------------------------------------------------
.* Help for the HELP USING HELP menu item
.* --------------------------------------------------------------------------
:h2 res=&MID_HELPUSING..Help for USING HELP Menu Item
:p.The Using help menu item raises an OS/2 supplied panel which explains the operation
of the help subsystem.

.* --------------------------------------------------------------------------
.* Help for the HELP PRODUCT INFORMATION menu item
.* --------------------------------------------------------------------------
:h2 res=&MID_HELPPROD..Help for PRODUCT INFORMATION Menu Item
:p.The Product information menu item raises a dialog which presents
information about the product.

.* --------------------------------------------------------------------------
.* Help the card add/edit dialog box
.* --------------------------------------------------------------------------
:h1 res=&DID_EDITCARD..Help for ADD(EDIT) Card Dialog
:p.The Add/Edit Card dialog is used to initialize and modify the data
displayed on a card.  The dialog provides the following entry fields
and controls:

:p.:link reftype=hd res=&DID_EDITNAMEI..First Name and Initial:elink.
:p.:link reftype=hd res=&DID_EDITNAMEL..Last Name:elink.
:p.:link reftype=hd res=&DID_EDITCOMP..Company:elink.
:p.:link reftype=hd res=&DID_EDITADDR1..Address Line 1:elink.
:p.:link reftype=hd res=&DID_EDITADDR2..Address Line 2:elink.
:p.:link reftype=hd res=&DID_EDITCITY..City:elink.
:p.:link reftype=hd res=&DID_EDITSTATE..State:elink.
:p.:link reftype=hd res=&DID_EDITZIP..Zip:elink.
:p.:link reftype=hd res=&DID_EDITPHONE..Telephone &numsign.:elink.
:p.:link reftype=hd res=&DID_EDITDOB..Date of Birth:elink.
:p.:link reftype=hd res=&DID_EDITANN..Anniversary Date:elink.
:p.:link reftype=hd res=&DID_EDITMLE..Notes:elink.
:p.:link reftype=hd res=&DID_EDITTYPE..Card Type:elink.
:p.:link reftype=hd res=&DID_EDITRATING..Rating:elink.
:p.:link reftype=hd res=&DID_EDITOK..Ok:elink.
:p.:link reftype=hd res=&DID_EDITCAN..Cancel:elink.

.* --------------------------------------------------------------------------
.* Help for the first name and initial entry field
.* --------------------------------------------------------------------------
:h2 res=&DID_EDITNAMEI..Help for First Name and Initial
:p.The entry field is typically used for entering the first name and
initial(s) of an individual.

.* --------------------------------------------------------------------------
.* Help for the last name entry field
.* --------------------------------------------------------------------------
:h2 res=&DID_EDITNAMEL..Help for Last Name
:p.This entry field is typically used for entering the last name of an
individual.  The first letter of the contents of this field is used
to determine the alpha tab under which the card is placed.

.* --------------------------------------------------------------------------
.* Help for the company entry field
.* --------------------------------------------------------------------------
:h2 res=&DID_EDITCOMP..Help for Company
:p.This entry field is typically used for entering the company which employs
an individual.  When adding a card for a company for which there is no
contact individual, the company name should be entered in the Last Name field
for proper sorting.

.* --------------------------------------------------------------------------
.* Help for the address line 1 entry field
.* --------------------------------------------------------------------------
:h2 res=&DID_EDITADDR1..Help for Address Line 1
:p.This entry field is used for entering an individual's or company's address.

.* --------------------------------------------------------------------------
.* Help for the address line 2 entry field
.* --------------------------------------------------------------------------
:h2 res=&DID_EDITADDR2..Help for Address Line 2
:p.This entry field is used for entering an additional line of an
individual's or company's address.

.* --------------------------------------------------------------------------
.* Help for the city entry field
.* --------------------------------------------------------------------------
:h2 res=&DID_EDITCITY..Help for City
:p.This entry field is used for entering the city portion of the address.

.* --------------------------------------------------------------------------
.* Help for the state entry field
.* --------------------------------------------------------------------------
:h2 res=&DID_EDITSTATE..Help for State
:p.This entry field is used for entering the state portion of the address.

.* --------------------------------------------------------------------------
.* Help for the zip code entry field
.* --------------------------------------------------------------------------
:h2 res=&DID_EDITZIP..Help for Zip
:p.This entry field is used for entering the postal zip code portion of the
address.

.* --------------------------------------------------------------------------
.* Help for the telephone number entry field
.* --------------------------------------------------------------------------
:h2 res=&DID_EDITPHONE..Help for Telephone &numsign.
:p.This entry field is used to record the individual's or company's telephone
number.

.* --------------------------------------------------------------------------
.* Help for the birthdate entry field
.* --------------------------------------------------------------------------
:h2 res=&DID_EDITDOB..Help for Date of Birth
:p.This field is typically used for entering a date of birth,
but may also be used for other significant dates.

.* --------------------------------------------------------------------------
.* Help for the anniversary entry field
.* --------------------------------------------------------------------------
:h2 res=&DID_EDITANN..Help for Anniversary Date
:p.This field is typically used for entering an anniversary date.  This
might be a wedding anniversary, an employment start anniversary, or any
other significant date.

.* --------------------------------------------------------------------------
.* Help for the notes multi-line edit field
.* --------------------------------------------------------------------------
:h2 res=&DID_EDITMLE..Help for Notes
:p.This field allows free form entry of any additional desired information.

.* --------------------------------------------------------------------------
.* Help for the card type value set
.* --------------------------------------------------------------------------
:h2 res=&DID_EDITTYPE..Help for Card Type
:p.This value set allows selection of one of ten icons to be displayed on the
card, reflecting the type of card, such as business contact, sports contact,
or phone number.

.* --------------------------------------------------------------------------
.* Help for the rating slider
.* --------------------------------------------------------------------------
:h2 res=&DID_EDITRATING..Help for Rating
:p.This slider allows rating of the company or individual.  The rating might
reflect the importance of a customer, a credit rating, or any other desired
ratable characteristic.

.* --------------------------------------------------------------------------
.* Help for the OK button
.* --------------------------------------------------------------------------
:h2 res=&DID_EDITOK..Help for Ok
:p.Clicking the Ok button causes the dialog to close and the contents to be
used for creating or updating a card in the card file.

.* --------------------------------------------------------------------------
.* Help for the Cancel button
.* --------------------------------------------------------------------------
:h2 res=&DID_EDITCAN..Help for Cancel
:p.Clicking the Cancel button causes the dialog to close without modifying
the contents of the card file.
:euserdoc.
