// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      CARDFILE.H                                       mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Main header File for CARDFILE/Chapter 11 Sample Program       บ
// บ                                                                               บ
// บ  NOTES:         None                                                          บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#define  INCL_PM
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// --------------------------------------------------------------------------
// Presentation Parameters
// --------------------------------------------------------------------------
#define PP_PROGTITLE PRESPARAMS PP_FOREGROUNDCOLORINDEX, CLR_RED PRESPARAMS PP_FONTNAMESIZE, "28.Tms Rmn"

// --------------------------------------------------------------------------
// define product information bitmap ID
// --------------------------------------------------------------------------
#define IDB_PRODINFO           99

// --------------------------------------------------------------------------
// define window IDs
// --------------------------------------------------------------------------
#define  APPLICATION_ID       100
#define  NOTEBOOK_ID          101
#define  DID_CARD             102
#define  DID_EDITCARD         103
#define  DID_DELCARDMSG       104
#define  OBJWND_ID            105
#define  PRODINFO_ID          106
#define  ID_SAVING            300
#define  ID_LOADING           301
#define  CHANGED_ID           200

// --------------------------------------------------------------------------
// Define menu item identifiers
// --------------------------------------------------------------------------
#define  MID_FILE             110
#define  MID_FILENEW          111
#define  MID_FILEOPEN         112
#define  MID_FILESAVE         113
#define  MID_FILESAVEAS       114
#define  MID_FILEEXIT         115
#define  MID_CARD             120
#define  MID_CARDCREATE       121
#define  MID_CARDEDIT         122
#define  MID_CARDREMOVE       123
#define  MID_HELP             130
#define  MID_HELPGENERAL      131
#define  MID_HELPUSING        132
#define  MID_HELPKEYS         133
#define  MID_HELPPROD         134

// --------------------------------------------------------------------------
// define card edit dialog controls
// --------------------------------------------------------------------------
#define  DID_EDITNAMEI        202
#define  DID_EDITNAMEL        204
#define  DID_EDITCOMP         206
#define  DID_EDITADDR1        208
#define  DID_EDITADDR2        210
#define  DID_EDITCITY         212
#define  DID_EDITSTATE        214
#define  DID_EDITZIP          216
#define  DID_EDITPHONE        218
#define  DID_EDITDOB          220
#define  DID_EDITANN          222
#define  DID_EDITTYPE         223
#define  DID_EDITRATING       226
#define  DID_HELP             229
#define  DID_EDITMLE          230
#define  DID_EDITOK           231
#define  DID_EDITCAN          232

// --------------------------------------------------------------------------
// Define card display (notebook page window) dialog controls
// --------------------------------------------------------------------------
#define  DIT_LASTNAME         1000
#define  DIT_NAMEINIT         1001
#define  DIT_COMPANY          1002
#define  DIT_ADDR1            1003
#define  DIT_ADDR2            1004
#define  DIT_CITY             1005
#define  DIT_STATE            1006
#define  DIT_ZIP              1007
#define  DIT_TELNO            1008
#define  DIT_DOB              1009
#define  DIT_ANN              1010
#define  DIT_ICON             1020
#define  DIT_INFO             1030
#define  DIT_RATING           1040

// --------------------------------------------------------------------------
// Define the data changed dialog controls
// --------------------------------------------------------------------------
#define DID_SAVECHANGES             203
#define DID_DISCARDCHANGES          205

// --------------------------------------------------------------------------
// Define application defined messages
// --------------------------------------------------------------------------
#define  UM_OPEN              (WM_USER + 0)
#define  UM_CLOSE             (UM_OPEN + 1)
#define  UM_WRITE             (UM_CLOSE + 1)
#define  UM_READ              (UM_WRITE + 1)
#define  UM_READY             (UM_READ + 1)
#define  UM_DONE              (UM_READY + 1)
#define  UM_OBJHWND           (UM_DONE + 1)

// --------------------------------------------------------------------------
// Define valueset icon IDs
// --------------------------------------------------------------------------
#define  ID_ICON1             10001
#define  ID_ICON2             10002
#define  ID_ICON3             10003
#define  ID_ICON4             10004
#define  ID_ICON5             10005
#define  ID_ICON6             10006
#define  ID_ICON7             10007
#define  ID_ICON8             10008
#define  ID_ICON9             10009
#define  ID_ICON10            10010


// --------------------------------------------------------------------------
// Define file operation status codes
// --------------------------------------------------------------------------
#define        OPSTAT_NOERROR          0L
#define        OPSTAT_FILEOPENERROR    1L
#define        OPSTAT_FILEWRITEERROR   2L
#define        OPSTAT_FILEREADERROR    3L

// --------------------------------------------------------------------------
// Define file operation type codes
// --------------------------------------------------------------------------
#define        OPTYPE_READ             0L
#define        OPTYPE_WRITE            1L

// --------------------------------------------------------------------------
// definition of the file operation control structure
// --------------------------------------------------------------------------
typedef struct __file_op_control__ {
   USHORT      cb;
   HWND        hwndClient;
   HWND        hwndServer;
   FILE        *file;
   PSZ         pszFilename;
   PSZ         pszMode;
   PVOID       buffer;
   ULONG       cbBuffer;
   ULONG       ulUser;
   ULONG       ulOp;          // Operation code - read or write
} FILEOPCTL, *PFILEOPCTL;

// --------------------------------------------------------------------------
// Define card data flags
// --------------------------------------------------------------------------
#define  CARD_CREATED      0x0001

// --------------------------------------------------------------------------
// definition of the internal structure representing a card
// --------------------------------------------------------------------------
typedef struct __card_data__ {
   USHORT      cb;
   USHORT      fs;
   PSZ         pszNameL;
   PSZ         pszNameI;
   PSZ         pszCompany;
   PSZ         pszAddr1;
   PSZ         pszAddr2;
   PSZ         pszCity;
   PSZ         pszState;
   PSZ         pszZip;
   PSZ         pszTelephone;
   PSZ         pszDob;
   PSZ         pszAnn;
   PSZ         pszNotes;
   USHORT      usType;
   USHORT      usRating;
} CARDDATA, *PCARDDATA;

// --------------------------------------------------------------------------
// Define the object window creation function
// --------------------------------------------------------------------------
void CreateIOObject( HWND hwnd );
