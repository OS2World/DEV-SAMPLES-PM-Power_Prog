============================================================================================
Welcome to the OS/2 Warp Presentation Manager for Power Programmers.
============================================================================================

Section 1:  Installation
            ------------

We hope you find the enclosed sample programs helpful.  To install the samples
download the file POWERPM.ZIP to a directory on your hard drive and use PKUNZIP.EXE
to unpack this file.  For example:

   [X:\TEMP] PKUNZIP POWERPM.ZIP          // where X:\TEMP is a valid path and filename
                                          // on your fixed disk drive.


Once you unzip this file, you should get the following additional ZIP files.

    COMMON.ZIP    --> SHCOMMON source 
    CHAP02.ZIP    --> BUTTONS  source
    CHAP03.ZIP    --> PMED     source
    CHAP04.ZIP    --> CLKDRAW  source
    CHAP05.ZIP    --> HELPME   source
    CHAP06.ZIP    --> THREADS  source
    CHAP07.ZIP    --> DRAGEM   source 
    CHAP08.ZIP    --> PMEDIT   source
    CHAP09.ZIP    --> PMSTATS  source
    CHAP09W.ZIP   --> WINCHART source
    CHAP10.ZIP    --> SUBCLASS source
    CHAP11.ZIP    --> CARDFILE source
    CHAP12.ZIP    --> CHKREG   source
    CHAP13.ZIP    --> MATCH    source
    CHAP14.ZIP    --> TUTORIAL source
    CHAP15.ZIP    --> SHOWOFF  source
    CHAP16.ZIP    --> PRINTIT  source
    CHAP17.ZIP    --> PMSCREEN source
    INSTALL,EXE   --> Installation Utility

Run the INSTALL.EXE program to install the samples to a directory on your fixed disk.

The final directory structure should resemble this:

   X:\
    |
    |
    ------> X:\POWERPM
             |
             |
             ------> X:\POWERPM\COMMON 
                     X:\POWERPM\CHAP02 
                     X:\POWERPM\CHAP03 
                     X:\POWERPM\CHAP04 
                     X:\POWERPM\CHAP05 
                     X:\POWERPM\CHAP06 
                     X:\POWERPM\CHAP07 
                     X:\POWERPM\CHAP08 
                     X:\POWERPM\CHAP09
                      |
                      |
                      ------> X:\POWERPM\CHAP09\PM       
                              X:\POWERPM\CHAP09\WIN      
                     X:\POWERPM\CHAP10                   
                     X:\POWERPM\CHAP11                   
                     X:\POWERPM\CHAP12                   
                     X:\POWERPM\CHAP13                   
                     X:\POWERPM\CHAP14                   
                     X:\POWERPM\CHAP15                   
                     X:\POWERPM\CHAP16                   
                     X:\POWERPM\CHAP17                   

Finally, delete the temporary directory and enjoy the samples!!!

============================================================================================       


Section 2:  Description of Sample Programs            
            ------------------------------             

The following sample programs are included in the package:
 
Chapter 2:  BUTTONS -  a program launcher that makes use of the standard PM controls.
                       This chapter focuses on basic control concepts and shows some 
                       of the enhancements made to the basic control classes in OS/2 
                       Warp.

Chapter 3:  PMED    -  a fully functional text editor designed to demonstrate 
                       how to handle keyboard input and scrolling within a PM program.

Chapter 4:  CLKDRAW -  a graphical drawing program that demonstrates how to 
                       programmatically handle the pointing device, the PM clipboard 
                       and advanced GPI concepts. 
                       
Chapter 5:  HELPME  -  a template program that shows how to implement context
                       sensitive help using the Information Presentation Facility.

Chapter 6:  THREADS -  a program designed to demonstrate several effective 
                       multithreading techniques.

Chapter 7:  DRAGEM  -  a workplace shell compliant drag and drop file manager.

Chapter 8:  PMEDIT  -  a simple text editor that uses a multi-line entryfield as 
                       its edit window.  This program concentrates on teaching the 
                       effective use of dialog windows and menus and implements a 
                       button bar.

Chapter 9:  PMSTATS -  This sample program is a PM based DDE server sample program 
                       that is designed to illustrate how a PM application can 
                       effectively communicate with a Windows based DDE client 
                       application.  The source code for the sample Windows application
                       WINCHART is also provided.

Chapter 10: SUBCLASS - a program that demonstrates the power of subclassing a control.
                       This sample program subclasses the standard static control to 
                       create an editable static control by combining the power of 
                       the entryfield control with the simplicity of the static control.

Chapter 11: CARDFILE - a handy little address book program designed to show the 
                       practical use of the advanced OS/2 controls including, the 
                       notebook, value set, and slider.

Chapter 12: CHKREG   - This applet makes use of the powerful container control 
                       to create a simple check register program.

Chapter 13: MATCH    - If you have the multimedia Presentation Manager that comes
                       with OS/2 Warp installed, you'll appreciate this fun little
                       game.  MATCH is a simple match game that makes use of the 
                       animated buttons found in MMPM. 

Chapter 14: TUTOR    - This sample program focuses on the principles involved in
                       designing and developing your own control windows.  The 
                       tutor program is a simple button control implementation 
                       using half text and half button to create a control that 
                       is ideal for creating tutorial presentations.

Chapter 15: SHOWOFF  - This sample program is presentation program that allows 
                       the user to display and manipulate bitmap files.

Chapter 16: PRINTIT  - a simple utility that allows the user to print a text file.  
                       This program demonstrates the various concepts involved in 
                       printing from a Presentation Manager application.

Chapter 17: PMSCREEN - a screen capture utility that allows the user to capture the 
                       contents of the active window on the desktop to a bitmap.  The
                       bitmap can be copied to another program via the PM clipboard.
                       The code demonstrates how to use PM hooks, and makes use of 
                       several of the most powerful and least documented PM hooks;
                       including, the Input hook, FlushBuf hook, and Lockup hook.
