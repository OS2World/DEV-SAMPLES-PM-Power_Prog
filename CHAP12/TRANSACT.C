// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      TRANSACT.C                                       mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Enter transaction dialog for CHKREG/Chapter 12 Sample         บ
// บ                                                                               บ
// บ  NOTES:         This file contains the code for managing the transaction      บ
// บ                 entry dialog.                                                 บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#define  INCL_WINDIALOGS
#define  INCL_WINENTRYFIELDS
#define  INCL_WINWINDOWMGR
#define  INCL_WINBUTTONS

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "chkreg.h"
#include "shcommon.h"

// --------------------------------------------------------------------------
// local function prototypes
// --------------------------------------------------------------------------
static MRESULT wm_command( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT wm_initdlg( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static MRESULT ProcessOK( HWND hwnd, MPARAM mp1, MPARAM mp2 );
static BOOL IsValidDate( char *date);

// *******************************************************************************
// FUNCTION:     TransactDlgProc
//
// FUNCTION USE: Dialog window procedure
//
// DESCRIPTION:  This routine is the dialog procedure for the ENTER TRANSACTION
//               dialog.  The routine dispatches processed messages to worker
//               functions and unprocessed messages to the default dialog window
//               procedure.
//
// PARAMETERS:   HWND     dialog window handle
//               ULONG    window message
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  WinDefDlgProc for all unprocessed messages;
//                        otherwise, message dependent
//
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT EXPENTRY TransactDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
 switch (msg) 
  {
   case WM_INITDLG:
        return wm_initdlg (hwnd, mp1, mp2); 

   case WM_COMMAND:              
        return wm_command (hwnd, mp1, mp2);         

   default:                      
        return WinDefDlgProc (hwnd, msg, mp1, mp2);      
  } /* endswitch */
}

// *******************************************************************************
// FUNCTION:     wm_command
//
// FUNCTION USE: Worker function to process the WM_COMMAND message
//
// DESCRIPTION:  Calls the worker function for the OK button, routes other
//               commands to the default dialog procedure.
//
// PARAMETERS:   HWND     dialog window handle
//               MPARAM   command code
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  return code from worker function
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wm_command( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
 switch (SHORT1FROMMP(mp1)) 
  {
   case DID_OK:            
        return ProcessOK (hwnd, mp1, mp2);            

   case DID_HELP:
        DisplayMessages (NULLHANDLE, "Sorry, I cannot offer you any help!", MSG_INFO);
        return FALSE;

   default:                
        return WinDefDlgProc (hwnd, WM_COMMAND, mp1, mp2);          
  } /* endswitch */
}

// *******************************************************************************
// FUNCTION:     wm_initdlg
//
// FUNCTION USE: Worker function to process the WM_INITDLG message
//
// DESCRIPTION:  save the dialog communication structure pointer and
//               sets limits on the input fields.
//
// PARAMETERS:   HWND     client window handle
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  WinDefDlgProc for all unprocessed messages;
//                        otherwise, message dependent
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT wm_initdlg( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PTRANSDLG      pTransDlg = (PTRANSDLG)mp2;
   DATETIME       datetime;
   CHAR           szBuffer[50];
   APIRET         rc;


// --------------------------------------------------------------------------        
// Remove the system menuitems that are not needed by this dialog                                                                  
// --------------------------------------------------------------------------        
   SetTheSysMenu(hwnd, NULL);

// -------------------------------------------------------------------------- 
// get today's date 
// --------------------------------------------------------------------------
   rc = DosGetDateTime(&datetime);                                                            
   if (rc)                                                                               
    {                                                                                         
     WinAlarm(HWND_DESKTOP, WA_ERROR);                                                        
     DisplayMessages(NULLHANDLE, "Error getting today's date", MSG_EXCLAMATION);
    }                                                                                         
                                                                                              
// -------------------------------------------------------------------------- 
// format the date string and plug it into the transaction date entryfield       
// --------------------------------------------------------------------------
   sprintf(szBuffer, "%02d/%02d/%04d", datetime.month, datetime.day, datetime.year);        
   WinSetDlgItemText (hwnd, CID_TRN_DATE, szBuffer);            

// -------------------------------------------------------------------------- 
// save parameter pointer in the window data
// --------------------------------------------------------------------------
   WinSetWindowULong( hwnd, QWL_USER, (ULONG)pTransDlg );

// --------------------------------------------------------------------------
// establish field limits
// --------------------------------------------------------------------------
   WinSendDlgItemMsg( hwnd, CID_TRN_DATE, EM_SETTEXTLIMIT,
                      MPFROMLONG(DATE_STR_LEN), MPFROMP(NULL));
   WinSendDlgItemMsg( hwnd, CID_TRN_REF, EM_SETTEXTLIMIT,
                      MPFROMLONG(REF_STR_LEN), MPFROMP(NULL));
   WinSendDlgItemMsg( hwnd, CID_TRN_AMT, EM_SETTEXTLIMIT,
                      MPFROMLONG(AMT_STR_LEN), MPFROMP(NULL));
   WinSendDlgItemMsg( hwnd, CID_TRN_MEMO, EM_SETTEXTLIMIT,
                      MPFROMLONG(MEMO_STR_LEN), MPFROMP(NULL));

// --------------------------------------------------------------------------
// indicate no change in focus
// --------------------------------------------------------------------------
   return MRFROMLONG(FALSE);
}

// *******************************************************************************
// FUNCTION:     ProcessOK
//
// FUNCTION USE: Worker function to process OK pushbutton clicks
//
// DESCRIPTION:  Retrieves the dialog entries and verifies the date and amount
//               fields.  If there are no errors, the dialog window is dismissed,
//               otherwise, focus is returned to the field in error.
//
// PARAMETERS:   HWND     dialog window handle
//               MPARAM   command code
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  return code from worker function
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT ProcessOK( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   TRANSDLG       TransDlg;
   PTRANSDLG      pTransDlg;
   BOOL           bVerified = FALSE;
   ULONG          ulIDError;

   do {
// --------------------------------------------------------------------------
// Initialize a temporary communications structure
// --------------------------------------------------------------------------
      TransDlg.cb = sizeof(TransDlg);

// --------------------------------------------------------------------------
// first get the field data into the temporary buffer
// --------------------------------------------------------------------------
      TransDlg.bDeposit = (BOOL)WinSendDlgItemMsg( hwnd, CID_TRN_WTH, BM_QUERYCHECKINDEX, (MPARAM)0, (MPARAM)0); 
      WinQueryDlgItemText( hwnd, CID_TRN_DATE, DATE_STR_LEN + 1, TransDlg.date );
      WinQueryDlgItemText( hwnd, CID_TRN_REF,  REF_STR_LEN + 1,  TransDlg.ref );
      WinQueryDlgItemText( hwnd, CID_TRN_AMT,  AMT_STR_LEN + 1,  TransDlg.amount );
      WinQueryDlgItemText( hwnd, CID_TRN_MEMO, MEMO_STR_LEN + 1, TransDlg.memo );

// --------------------------------------------------------------------------
// verify the date field - should be MM/DD/YYYY
// --------------------------------------------------------------------------
      if (!IsValidDate(TransDlg.date)) {
         DisplayMessages( NULLHANDLE, "Invalid Date", MSG_ERROR );
         ulIDError = CID_TRN_DATE;
         break;
      } /* endif */

// --------------------------------------------------------------------------
// verify the amount field
// --------------------------------------------------------------------------
      if (!IsValidAmount(TransDlg.amount)) {
         DisplayMessages( NULLHANDLE, "Invalid Amount", MSG_ERROR );
         ulIDError = CID_TRN_AMT;
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Indicate that all fields were valid
// --------------------------------------------------------------------------
      bVerified = TRUE;

   } while ( false ); /* enddo */

   if (bVerified) {
// --------------------------------------------------------------------------
// If fields were valid, get the real communications structure and copy
// the temporary structure into the real structure.  Dismiss the dialog.
// --------------------------------------------------------------------------
      pTransDlg = (PTRANSDLG)WinQueryWindowULong( hwnd, QWL_USER );
      *pTransDlg = TransDlg;
      WinDismissDlg( hwnd, DID_OK );
   } else {

// --------------------------------------------------------------------------
// Invalid data - set focus back to the invalid field
// --------------------------------------------------------------------------
      WinSetFocus( HWND_DESKTOP, WinWindowFromID( hwnd, ulIDError ) );
   } /* endif */

// --------------------------------------------------------------------------
// return reserved value
// --------------------------------------------------------------------------
   return (MRESULT)0;
}

// *******************************************************************************
// FUNCTION:     IsValidDate
//
// FUNCTION USE: Utility function to verify the date entered.
//
// DESCRIPTION:  Verifies the syntax of the date and the validity of the actual
//               date.
//
// PARAMETERS:   char*    date string
//
// RETURNS:      BOOL     TRUE if date is valid
//
// INTERNALS:    NONE
//
// *******************************************************************************
static BOOL IsValidDate( char *szDate )
{
   USHORT   usMonth = 0;
   USHORT   usDay = 0;
   USHORT   usYear = 0;
   USHORT   usMonthLen = 0;
   USHORT   usDayLen = 0;
   USHORT   usYearLen = 0;
   char     *szTemp = szDate;

   do {
// --------------------------------------------------------------------------
// parse out the month */
// --------------------------------------------------------------------------
      while (usMonthLen < 2 && isdigit( *szTemp )) {
         usMonth = (usMonth * 10U) + (USHORT)(*szTemp - '0');
         usMonthLen++;
         szTemp++;
      } /* endwhile */

// --------------------------------------------------------------------------
// error if no month string or the string is too long
// --------------------------------------------------------------------------
      if (usMonthLen == 0 || *szTemp != '/') {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// move past the separator and parse out the day string
// --------------------------------------------------------------------------
      szTemp++;
      while (usDayLen < 2 && isdigit( *szTemp )) {
         usDay = (usDay * 10U) + (USHORT)(*szTemp - '0');
         usDayLen++;
         szTemp++;
      } /* endwhile */

// --------------------------------------------------------------------------
// error if no day string or the string is too long
// --------------------------------------------------------------------------
      if (usDayLen == 0 || *szTemp != '/') {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// move past the separator and parse out the year string
// --------------------------------------------------------------------------
      szTemp++;
      while (usYearLen < 4 && isdigit( *szTemp)) {
         usYear = (usYear * 10U) + (USHORT)(*szTemp - '0');
         usYearLen++;
         szTemp++;
      } /* endwhile */

// --------------------------------------------------------------------------
// error if string is too short or too long - must have all four digits
// to allow for dates rolling into and past the year 2000
// --------------------------------------------------------------------------
      if (usYearLen != 4 || *szTemp) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// make sure the month and day are no zero
// --------------------------------------------------------------------------
      /* validate the numeric values */
      if (usMonth == 0 || usDay == 0) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// check the day range for the month of February
// --------------------------------------------------------------------------
      if (usMonth == 2) {
         if (usYear % 4 == 0 && usYear % 100 != 0) {
// --------------------------------------------------------------------------
// leap year limit is 29
// --------------------------------------------------------------------------
            if (usDay > 29) {
               break;
            } /* endif */
         } else {
// --------------------------------------------------------------------------
// non-leap year limit is 28
// --------------------------------------------------------------------------
            if (usDay > 28) {
               break;
            } /* endif */
         } /* endif */
      } else {
// --------------------------------------------------------------------------
// adjust for months over 7 such that odd months have 31 days and even 30
// --------------------------------------------------------------------------
         if (usMonth > 7) {
            usMonth--;
         } /* endif */

// --------------------------------------------------------------------------
// verify the day range
// --------------------------------------------------------------------------
         if (usMonth % 2 == 0) {
            if (usDay > 30) {
               break;
            } /* endif */
         } else {
            if (usDay > 31) {
               break;
            } /* endif */
         } /* endif */
      } /* endif */

// --------------------------------------------------------------------------
// made it to here - date verified OK
// --------------------------------------------------------------------------
      return TRUE;

   } while ( false ); /* enddo */

// --------------------------------------------------------------------------
// error somewhere along the way
// --------------------------------------------------------------------------
   return FALSE;
}

// *******************************************************************************
// FUNCTION:     IsValidAmount
//
// FUNCTION USE: Utility function to verify the amount entered.
//
// DESCRIPTION:  Verifies the syntax of the amount and the validity of the actual
//               amount.
//
// PARAMETERS:   char*    amount string
//
// RETURNS:      BOOL     TRUE if amount is valid
//
// INTERNALS:    NONE
//
// *******************************************************************************
BOOL IsValidAmount( char *szAmount )
{
   char     *szDecimal;
   char     *szTemp;
   char     szOutput[ AMT_STR_LEN + 1];
   USHORT   usChars = 0;

   do {
// --------------------------------------------------------------------------
// make sure the first character is a digit.  Cents must be entered as 0.cc
// --------------------------------------------------------------------------
      if (!isdigit(*szAmount)) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// determine the number of digits to the end of the string or the decimal
// point.
// --------------------------------------------------------------------------
      szDecimal = szAmount;
      while (*szDecimal != '.' && *szDecimal != '\0') {
         if (isdigit(*szDecimal)) {
            usChars++;
         } /* endif */
         szDecimal++;
      } /* endwhile */

// --------------------------------------------------------------------------
// make sure there are not too many digits for the limits we have set
// --------------------------------------------------------------------------
      if (usChars > AMT_MAX_DIGITS) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// move the whole amount to the temporary output string
// --------------------------------------------------------------------------
      szDecimal = szAmount;
      szTemp = szOutput;
      *(szTemp++) = *(szDecimal++);
      usChars--;

// --------------------------------------------------------------------------
// valid characters can be either digits, commas or the decimal point
// --------------------------------------------------------------------------
      while (*szDecimal != '.' && *szDecimal != '\0') {
// --------------------------------------------------------------------------
// commas can only be placed at intervals of 3 digits from the decimal point
// --------------------------------------------------------------------------
         if (*szDecimal == ',') {
            if (usChars == 0 || usChars % 3 != 0) {
               break;
            } /* endif */
         } else {
// --------------------------------------------------------------------------
// not a comma - make sure its a digit
// --------------------------------------------------------------------------
            if (!isdigit(*szDecimal)) {
               break;
            } /* endif */

// --------------------------------------------------------------------------
// move the character to the output string, inserting a comma if necessary
// --------------------------------------------------------------------------
            if (usChars != 0 && usChars % 3 == 0) {
               *(szTemp++) = ',';
            } /* endif */
            *(szTemp++) = *szDecimal;
            usChars--;
         } /* endif */

         szDecimal++;
      } /* endwhile */

// --------------------------------------------------------------------------
// make sure we're now at the decimal point or end of string
// --------------------------------------------------------------------------
      if (*szDecimal != '.' && *szDecimal != '\0') {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// move the decimal point to the output string
// --------------------------------------------------------------------------
      *(szTemp++) = '.';

// --------------------------------------------------------------------------
// make sure there are two digits after the decimal point - inserting
// 0's if necessary
// --------------------------------------------------------------------------
      if (*szDecimal != '\0') {
         szDecimal++;
         if (!isdigit(*(szDecimal)) || !isdigit(*(szDecimal + 1))) {
            break;
         } /* endif */
         *(szTemp++) = *(szDecimal++);
         *(szTemp++) = *(szDecimal++);
      } else {
         *(szTemp++) = '0';
         *(szTemp++) = '0';
      } /* endif */

// --------------------------------------------------------------------------
// we must now be at the end of the string
// --------------------------------------------------------------------------
      if (*szDecimal != '\0') {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// terminate the output string and copy the output back to the input
// --------------------------------------------------------------------------
      /* fill in the terminating zero for the output string */
      *szTemp = '\0';
      strcpy( szAmount, szOutput );

// --------------------------------------------------------------------------
// indicate that the amount was verified
// --------------------------------------------------------------------------
      return TRUE;
   } while ( false  ); /* enddo */

// --------------------------------------------------------------------------
// indicate that an invalid string was encountered.
// --------------------------------------------------------------------------
   return FALSE;
}
