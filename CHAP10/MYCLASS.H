// 浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�
// �  FILENAME:      MYCLASS.H                                        mm/dd/yy     �
// �                                                                               �
// �  DESCRIPTION:   Application interface to the editable static text control.    �
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