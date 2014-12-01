// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      MYCLASS.H                                        mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Application interface to the editable static text control.    บ
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
// Define the class styles
// --------------------------------------------------------------------------
#define  MC_LEFT     ES_LEFT
#define  MC_CENTER   ES_CENTER
#define  MC_RIGHT    ES_RIGHT
#define  MC_STATIC   ES_READONLY
#define  MC_AUTOSIZE ES_AUTOSIZE

// --------------------------------------------------------------------------
// Define messages which can be sent to the control
// --------------------------------------------------------------------------
#define  MC_ALLOWEDIT   EM_SETREADONLY

// --------------------------------------------------------------------------
// Prototype the function used to register the class
// --------------------------------------------------------------------------
void RegisterMyClass( HAB hab );
