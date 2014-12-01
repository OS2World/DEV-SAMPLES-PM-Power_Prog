// ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
// บ  FILENAME:      DRAGOBJ.C                                        mm/dd/yy     บ
// บ                                                                               บ
// บ  DESCRIPTION:   Object Thread Source for DRAGEM/Chapter 7 Sample Program      บ
// บ                                                                               บ
// บ  NOTES:         This program is a simple file manager which demonstrates      บ
// บ                 direct manipulation coding in PM applications.                บ
// บ                                                                               บ
// บ  PROGRAMMER:    Uri Joseph Stern and James Stan Morrow                        บ
// บ  COPYRIGHTS:    OS/2 Warp Presentation Manager for Power Programmers          บ
// บ                                                                               บ
// บ  REVISION DATES:  mm/dd/yy  Created this file                                 บ
// บ                                                                               บ
// ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#include "dragem.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "shcommon.h"

// --------------------------------------------------------------------------
// Miscellaneous global variables used only in this source module.
// --------------------------------------------------------------------------
static   BOOL        false = FALSE;
static   PDRAGINFO   gpDraginfo;
static   HWND        ghwndList;

// --------------------------------------------------------------------------
// Miscellaneous global variables defined outside this source file
// --------------------------------------------------------------------------
extern   ULONG       gulDragCount;
extern   PDRAGINFO   gpSrcDinfo;

// --------------------------------------------------------------------------
// Object window procedure
// --------------------------------------------------------------------------
MRESULT APIENTRY ObjectWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------
// Message processing procedures
// --------------------------------------------------------------------------
MRESULT dm_endconversation( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT dm_render( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT dm_rendercomplete( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT lazy_filllist( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT lazy_dirfromini( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT lazy_dorender( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT lazy_dodrop( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT lazy_inifromdir( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT lazy_discardobj( HWND hwnd, MPARAM mp1, MPARAM mp2 );

// --------------------------------------------------------------------------
// Miscellaneous procedures
// --------------------------------------------------------------------------
PDRAGTRANSFER MakeXfer( HWND hwnd, PDRAGITEM pdi,
                        USHORT usOp, PUSHORT pusToDo );
void KillXfer( PDRAGTRANSFER pDragXfer );
BOOL RenderFile ( PDRAGTRANSFER pdt );
static void BuildDriveMenu( HWND hwndFrame );

// *******************************************************************************
// FUNCTION:     ObjectThread
//
// FUNCTION USE: Main routine for the object window thread
//
// DESCRIPTION:  Initializes the PM environment for the thread, creates the
//               object window, and runs the message processing loop.
//
// PARAMETERS:   PVOID        required parameter - translates to application
//                            client window handle
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
void _Optlink ObjectThread( PVOID pArg )
{
   HAB      hab = NULLHANDLE;
   HMQ      hmq = NULLHANDLE;
   HWND     hwnd = NULLHANDLE;
   QMSG     qmsg;
   HWND     hwndMain = *(HWND *)pArg;
   char     szObjectClassName[] = "ObjectClass";


// --------------------------------------------------------------------------
// Single iteration loop - makes error handling easier
// --------------------------------------------------------------------------
   do {
// --------------------------------------------------------------------------
// First things first - set up the PM environment for the thread
// --------------------------------------------------------------------------
      hab = WinInitialize( 0 );
      if (hab == (HAB)NULLHANDLE) {
         // Oops - couldn't do it - bail out!
         DosBeep( 60, 100 );
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Next thing we need is a message queue
// --------------------------------------------------------------------------
      hmq = WinCreateMsgQueue( hab, 0 );
      if (hmq == (HMQ)NULLHANDLE) {
         // No message queue equals no PM - quit now
         DosBeep( 60, 100 );
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Now register the window class for object window
// --------------------------------------------------------------------------
      if (!WinRegisterClass( hab,
                             szObjectClassName,
                             ObjectWndProc,
                             0l, 0l )) {
         // failed!
         DisplayMessages(NULLHANDLE, "Unable to Register Object Class",
                         MSG_ERROR);
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Finally, now the window can be created
// --------------------------------------------------------------------------
      hwnd = WinCreateWindow( HWND_OBJECT,
                              szObjectClassName,
                              NULL,
                              0L, 0L, 0L, 0L, 0L,
                              NULLHANDLE,
                              HWND_BOTTOM,
                              RID_OBJECT,
                              NULL,
                              NULL );
      if (hwnd == (HWND)NULLHANDLE) {
         // no window - gotta leave.
         DisplayMessages(NULLHANDLE, "Error Creating Frame Window",
                         MSG_ERROR);
         break;
      } /* endif */

// --------------------------------------------------------------------------
// Save the main window (listbox) window handle
// --------------------------------------------------------------------------
      ghwndList = hwndMain;
// --------------------------------------------------------------------------
// All set, signal the application window with the object window handle
// --------------------------------------------------------------------------
      WinPostMsg( hwndMain, LAZY_SETOBJECTHWND, MPFROMHWND(hwnd), 0L );

// --------------------------------------------------------------------------
// NOTE:  This is our standard PM message loop.  The purpose of this loop
//        is to get and dispatch messages from the application message queue
//        until WinGetMsg returns FALSE, indicating a WM_QUIT message.  In
//        which case we will pop out of the loop and destroy our frame window
//        along with the application message queue then we will terminate our
//        anchor block removing our ability to call the Presentation Manager.
// --------------------------------------------------------------------------
      while (WinGetMsg( hab, &qmsg, NULLHANDLE, 0UL, 0UL))
         WinDispatchMsg( hab, &qmsg );

   } while ( false ); /* enddo */

// --------------------------------------------------------------------------
// Clean up on exit - releasing the message queue and PM environment
// --------------------------------------------------------------------------
   if (hmq != NULLHANDLE) WinDestroyMsgQueue( hmq );
   if (hab != NULLHANDLE) WinTerminate( hab );
}

// *******************************************************************************
// FUNCTION:     ObjWndProc
//
// FUNCTION USE: Procedure to receive messages for the object window.
//
// DESCRIPTION:  This procedure processes all messages arriving for the object
//               window.  In this implementation the window procedure's purpose
//               is to resolve the proper method to be called for the message.
//
// PARAMETERS:   HWND     object window handle
//               ULONG    window message
//               MPARAM   first message parameter
//               MPARAM   second message parameter
//
// RETURNS:      MRESULT  WinDefWindowProc for all unprocessed messages;
//                        otherwise, message dependent
//
// INTERNALS:    NONE
//
// *******************************************************************************
MRESULT APIENTRY ObjectWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   switch (msg) {
   case DM_ENDCONVERSATION: return dm_endconversation( hwnd, mp1, mp2 );
   case DM_RENDER:         return dm_render( hwnd, mp1, mp2 );
   case DM_RENDERCOMPLETE: return dm_rendercomplete( hwnd, mp1, mp2 );
   case LAZY_FILLLIST:     return lazy_filllist( hwnd, mp1, mp2 );
   case LAZY_DIRFROMINI:   return lazy_dirfromini( hwnd, mp1, mp2 );
   case LAZY_DODROP:       return lazy_dodrop( hwnd, mp1, mp2 );
   case LAZY_DORENDER:     return lazy_dorender( hwnd, mp1, mp2 );
   case LAZY_INIFROMDIR:   return lazy_inifromdir( hwnd, mp1, mp2 );
   case LAZY_DISCARDOBJ:   return lazy_discardobj( hwnd, mp1, mp2 );
   default:                return WinDefWindowProc( hwnd, msg, mp1, mp2 );
   } /* endswitch */
}

// *******************************************************************************
// FUNCTION:     dm_render
//
// FUNCTION USE: Worker function to process the DM_RENDER message
//
// DESCRIPTION:  This message is sent by the target application to invoke
//               source rendering for the file.  After checking to ensure
//               that the operation is appropriate, we forward another
//               message to ourselves and then return to the caller so
//               that the target may continue processing.
//
// PARAMETERS:   HWND     object window handle
//               MPARAM   pointer to DRAGTRANSFER structure
//               MPARAM   reserved
//
// RETURNS:      MRESULT  status of the rendering operation
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT dm_render( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PDRAGTRANSFER  pDT = (PDRAGTRANSFER)mp1;
   MRESULT        mrRetVal = (MRESULT)FALSE;
   ULONG          cRMF;
   PSZ            pszRMF;

   do {
// --------------------------------------------------------------------------
// forget it if the DRAGTRANSFER structure is null - the target messed up
// --------------------------------------------------------------------------
      if (pDT == NULL) break;

// --------------------------------------------------------------------------
// initialize result flags
// --------------------------------------------------------------------------
      pDT->fsReply = 0;

// --------------------------------------------------------------------------
// verify that a client window handle is given - else to whom do we
// send the DM_RENDERCOMPLETE message
// --------------------------------------------------------------------------
      if (pDT->hwndClient == NULLHANDLE) break;

// --------------------------------------------------------------------------
// verify that a DRAGITEM was passed
// --------------------------------------------------------------------------
      if (pDT->pditem == (PDRAGITEM)NULL) break;

// --------------------------------------------------------------------------
// verify that a destination name was given
// --------------------------------------------------------------------------
      if (pDT->hstrRenderToName == (HSTR)NULLHANDLE) break;

// --------------------------------------------------------------------------
// verify that an RMF was given
// --------------------------------------------------------------------------
      if (pDT->hstrSelectedRMF == (HSTR)NULLHANDLE) break;

// --------------------------------------------------------------------------
// verify that the RMF is valid
// --------------------------------------------------------------------------
      if ((cRMF = DrgQueryStrNameLen( pDT->hstrSelectedRMF )) == 0) break;

// --------------------------------------------------------------------------
// preset result flags in case of a recoverable RMF or operation error
// --------------------------------------------------------------------------
      pDT->fsReply = DMFL_RENDERRETRY;

// --------------------------------------------------------------------------
// Get the RMF string and be sure that a file transfer has been requested
// --------------------------------------------------------------------------
      if ((pszRMF = (PSZ)malloc(++cRMF)) == (PSZ)NULL) break;
      if (DrgQueryStrName(pDT->hstrSelectedRMF, cRMF, pszRMF) == 0) break;
      if (!strstr( pszRMF, "DRM_OS2FILE")) break;

// --------------------------------------------------------------------------
// verify that the operation is valid
// --------------------------------------------------------------------------
      if (pDT->usOperation != DO_COPY &&
          pDT->usOperation != DO_MOVE ) break;

// --------------------------------------------------------------------------
// all clear - post message to ourselves to start operation
// --------------------------------------------------------------------------
      WinPostMsg( hwnd, LAZY_DORENDER, mp1, mp2 );

// --------------------------------------------------------------------------
// Set valid return code - we're going to render
// --------------------------------------------------------------------------
      pDT->fsReply = 0;
      mrRetVal = (MRESULT)TRUE;

   } while ( false ); /* enddo */

// --------------------------------------------------------------------------
// release transfer structure if rendering won't occur
// --------------------------------------------------------------------------
   if( !mrRetVal ) DrgFreeDragtransfer( pDT );

// --------------------------------------------------------------------------
// return with status as determined above
// --------------------------------------------------------------------------
   return mrRetVal;
}

// *******************************************************************************
// FUNCTION:     lazy_dorender
//
// FUNCTION USE: Worker function to process the LAZY_DORENDER message.
//
// DESCRIPTION:  We post this message to ourselves after a DM_RENDER in
//               order to perform the actual file rendering.  The function
//               retrieves the source and target names, then copies or
//               moves the file.
//
// PARAMETERS:   HWND     object window handle
//               MPARAM   pointer to DRAGTRANSFER structure
//               MPARAM   reserved
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT lazy_dorender( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PDRAGTRANSFER     pDT = (PDRAGTRANSFER)PVOIDFROMMP(mp1);
   char              pszSource[ CCHMAXPATH ];
   char              pszTarget[ CCHMAXPATH ];
   char              pszSourceDir[ CCHMAXPATHCOMP ];
   char              pszSourceFile[ CCHMAXPATHCOMP ];
   ULONG             cString;
   USHORT            usRetVal = DMFL_RENDERFAIL;

   do {
// --------------------------------------------------------------------------
// Get the length of the target string and make sure it's not longer than
// the maximum allowed for a path.  Then get the string itself.
// --------------------------------------------------------------------------
      if ((cString = DrgQueryStrNameLen( pDT->hstrRenderToName )) >=
            CCHMAXPATH) break;
      if (!DrgQueryStrName( pDT->hstrRenderToName, ++cString, pszTarget))
         break;

// --------------------------------------------------------------------------
// Get the length of the source directory name and make sure it's not longer
// than the allowed portion of a file name.  Then get the string itself.
// --------------------------------------------------------------------------
      if ((cString = DrgQueryStrNameLen( pDT->pditem->hstrContainerName)) >=
            CCHMAXPATHCOMP ) break;
      if (!DrgQueryStrName( pDT->pditem->hstrContainerName, ++cString,
            pszSourceDir)) break;

// --------------------------------------------------------------------------
// Get the length of the source file name and make sure it's not longer
// than the allowed portion of a file name.  Then get the string itself.
// --------------------------------------------------------------------------
      if ((cString = DrgQueryStrNameLen( pDT->pditem->hstrSourceName)) >=
            CCHMAXPATHCOMP ) break;
      if (!DrgQueryStrName( pDT->pditem->hstrSourceName, ++cString,
            pszSourceFile)) break;

// --------------------------------------------------------------------------
// Make sure the combined length of the source directory and file names
// are not longer than the maximum allowed for a full path name.  Then
// combine the two to form the full path to the source file.
// --------------------------------------------------------------------------
      if (strlen(pszSourceFile) + strlen(pszSourceFile) + 1 > CCHMAXPATH)
         break;
      strcpy( pszSource, pszSourceDir );
      strcat( pszSource, pszSourceFile );

// --------------------------------------------------------------------------
// Copy the source file to the target file.
// --------------------------------------------------------------------------
      if (DosCopy( pszSource, pszTarget, DCPY_EXISTING) != NO_ERROR) break;
      if (pDT->usOperation == DO_MOVE) {
// --------------------------------------------------------------------------
// For move operations, delete the source file
// --------------------------------------------------------------------------
         if (DosDelete(pszSource) != NO_ERROR) break;
      } /* endif */

// --------------------------------------------------------------------------
// Indicate that the file has been rendered - but a retry is
// allowed if for some reason the target cannot complete its
// end of the bargain
// --------------------------------------------------------------------------
      usRetVal = DMFL_RENDEROK | DMFL_RENDERRETRY;

   } while ( false ); /* enddo */

// --------------------------------------------------------------------------
// Post the DM_RENDERCOMPLETE message back to the target window
// --------------------------------------------------------------------------
   DrgPostTransferMsg( pDT->hwndClient, DM_RENDERCOMPLETE, pDT,
                       (ULONG)usRetVal, 0L, TRUE );

// --------------------------------------------------------------------------
// Release access to the DRAGTRANSFER structure
// --------------------------------------------------------------------------
   DrgFreeDragtransfer( pDT );

// --------------------------------------------------------------------------
// return an appropriate value for the compiler's sake
// --------------------------------------------------------------------------
   return (MRESULT)0L;

}

// *******************************************************************************
// FUNCTION:     dm_endconversation
//
// FUNCTION USE: Worker function to process the DM_ENDCONVERSATION message
//
// DESCRIPTION:  Decrement the count of objects to be rendered and when
//               all objects are rendered relinquich access to the DRAGINFO
//               structure
//
// PARAMETERS:   HWND     object window handle
//               MPARAM   item id as a ULONG
//               MPARAM   flags
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT dm_endconversation( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND     hwndList;

// --------------------------------------------------------------------------
// count off one object drag complete
// --------------------------------------------------------------------------
   if( --gulDragCount == 0 ) {

// --------------------------------------------------------------------------
// Update the listbox - we must be the source window to get this message.
// --------------------------------------------------------------------------
   hwndList = gpSrcDinfo->hwndSource;
   WinSendMsg( hwndList, LM_DELETEALL, 0l, 0l );
   WinSendMsg( hwnd, LAZY_FILLLIST, MPFROMHWND(hwndList), MPFROMP( NULL ) );

// --------------------------------------------------------------------------
// free the drag info structure when all objects are done
// --------------------------------------------------------------------------
      DrgFreeDraginfo( gpSrcDinfo );
   }

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

/******************************************************************************
 *
 * Target Window Functions
 *
 *****************************************************************************/

// *******************************************************************************
// FUNCTION:     lazy_dodrop
//
// FUNCTION USE: Worker function to process the LAZY_DODROP message
//
// DESCRIPTION:  We post this message to ourselves when a DM_DROP
//               message is received to begin actual rendering of the
//               objects.  Each dragged item is scanned and source
//               rendering is attempted.  If the source agrees to render
//               we're done.  If not, we render the file ourseleves.
//
// PARAMETERS:   HWND     object window handle
//               MPARAM   window handle of the main application window
//               MPARAM   pointer to the DRAGINFO structure
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT lazy_dodrop( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND           hwndTarget = HWNDFROMMP(mp1);
   HWND           hwndRender;
   PDRAGINFO      pDraginfo = (PDRAGINFO)mp2;
   PDRAGITEM      pDragitem = (PDRAGITEM)NULL;
   USHORT         usDragitem = 0;
   PUSHORT        pusToDo;
   PDRAGTRANSFER  pDragXfer = (PDRAGTRANSFER)NULL;

// --------------------------------------------------------------------------
// save the draginfo structure pointer
// --------------------------------------------------------------------------
   gpDraginfo = pDraginfo;

// --------------------------------------------------------------------------
// allocate a place to save the number of files to be rendered and
// initialize to the count in the DRAGINFO structure
// --------------------------------------------------------------------------
   pusToDo = (PUSHORT)malloc( sizeof(USHORT) );
   *pusToDo = pDraginfo->cditem;

// --------------------------------------------------------------------------
// scan the dragged items
// --------------------------------------------------------------------------
   for( usDragitem = 0; usDragitem < pDraginfo-> cditem; usDragitem++ ) {
// --------------------------------------------------------------------------
// FOR THIS APPLICATION: if the source window is equal to the target window
// we're copying back to the same directory so no operation is necessary
// --------------------------------------------------------------------------
      if( hwndTarget == pDraginfo->hwndSource ) {
// --------------------------------------------------------------------------
// post the DM_ENDCONVERSATION so the source can free any resources.
// --------------------------------------------------------------------------
         WinPostMsg( pDraginfo->hwndSource, DM_ENDCONVERSATION,
                     MPFROMSHORT( usDragitem ),
                     MPFROMSHORT( DMFL_TARGETSUCCESSFUL ));
         --(*pusToDo);     // count one item done
         continue;
      } /* endif */

// --------------------------------------------------------------------------
// get the DRAGITEM structure for the current object
// --------------------------------------------------------------------------
      pDragitem = DrgQueryDragitemPtr( pDraginfo, (ULONG)usDragitem );

// --------------------------------------------------------------------------
// set up the DRAGTRANSFER structure
// --------------------------------------------------------------------------
      pDragXfer = MakeXfer( hwnd, pDragitem, pDraginfo->usOperation, pusToDo );

// --------------------------------------------------------------------------
// try source rendering first - does source need to prepare?  If so, send the
// DM_RENDERPREPARE message.  If the source has a problem with the message,
// abort transfer of the object.
// --------------------------------------------------------------------------
      hwndRender = pDraginfo->hwndSource;
      if( pDragitem->fsControl & DC_PREPARE ) {
         if(!DrgSendTransferMsg( pDraginfo->hwndSource, DM_RENDERPREPARE,
                                 MPFROMP( pDragXfer ), MPFROMLONG( 0l ))) {

// --------------------------------------------------------------------------
// release the transfer structure and count one item done
// --------------------------------------------------------------------------
            KillXfer( pDragXfer );
            --(*pusToDo);
            continue;
         } /* endif */

// --------------------------------------------------------------------------
// DM_RENDERPREPARE may have changed to the conversation window
// --------------------------------------------------------------------------
         hwndRender = pDragitem->hwndItem;
      } /* endif */

// --------------------------------------------------------------------------
// try to let the source do the rendering
// --------------------------------------------------------------------------
      if( DrgSendTransferMsg ( hwndRender, DM_RENDER,
                               MPFROMP( pDragXfer), MPFROMLONG( 0l ))) {
// --------------------------------------------------------------------------
// if the source agrees to handle the rendering - wait for DM_RENDERCOMPLETE
// --------------------------------------------------------------------------
         continue;
      } /* endif */

// --------------------------------------------------------------------------
// the source didn't agree - we could retry if the source allows it
// --------------------------------------------------------------------------
      if( pDragXfer->fsReply & DMFL_RENDERRETRY ) {
         ;/* no retries - just end it here */
      } /* endif */

// --------------------------------------------------------------------------
// otherwise, we try to render ourselves and send a DM_ENDCONVERSATION
// message to the source indicating our success or failure.  This allows the
// source to free its resources.
// --------------------------------------------------------------------------
      if( RenderFile( pDragXfer)) {
         WinPostMsg( hwndRender, DM_ENDCONVERSATION,
                     MPFROMSHORT(usDragitem),
                     MPFROMSHORT(DMFL_TARGETSUCCESSFUL));
      } else {
         WinPostMsg( hwndRender, DM_ENDCONVERSATION,
                     MPFROMSHORT(usDragitem),
                     MPFROMSHORT(DMFL_TARGETFAIL));
      } /* endif */

// --------------------------------------------------------------------------
// release the DRAGTRANSFER structure and count an item done
// --------------------------------------------------------------------------
      KillXfer( pDragXfer );
      --(*pusToDo );

   } /* endfor */

// --------------------------------------------------------------------------
// Rendering has at least been started on all items.  If the count has gone
// to zero, indicating that rendering of all items has been completed, free
// up the resources (the count, the DRAGINFO string handles, and the
// DRAGINFO).
// --------------------------------------------------------------------------
   if( *pusToDo == 0 ) {
      free( pusToDo );

// --------------------------------------------------------------------------
// Update the listbox - which was the target of the drop
// --------------------------------------------------------------------------
      WinSendMsg( hwndTarget, LM_DELETEALL, 0l, 0l );
      WinSendMsg( hwnd, LAZY_FILLLIST,
                  MPFROMHWND(hwndTarget), MPFROMP( NULL ) );

      DrgDeleteDraginfoStrHandles( pDraginfo );
      DrgFreeDraginfo( pDraginfo );
   } /* endif */

// --------------------------------------------------------------------------
// return an appropriate value to keep the compiler happy
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     dm_rendercomplete
//
// FUNCTION USE: Worker function to process the DM_RENDERCOMPLETE message
//
// DESCRIPTION:  This message is sent from the source window to the target
//               to indicate that it has finished rendering an object.  The
//               target window responds with DM_ENDCONVERSATION message.
//               After releasing the DRAGTRANSFER structure is check is made
//               to determine if all objects have been rendered.  If so,
//               the DRAGINFO is freed.
//
// PARAMETERS:   HWND     object window handle
//               MPARAM   pointer to DRAGTRANSFER structure
//               MPARAM   status flags
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT dm_rendercomplete( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   PUSHORT        pusToDo;
   PDRAGTRANSFER  pDragXfer = (PDRAGTRANSFER)mp1;
   PDRAGINFO      pDraginfo;

// --------------------------------------------------------------------------
// tell the source application we're done
// --------------------------------------------------------------------------
   WinPostMsg( pDragXfer->pditem->hwndItem, DM_ENDCONVERSATION,
               MPFROMLONG(pDragXfer->pditem->ulItemID),
               MPFROMSHORT(DMFL_TARGETSUCCESSFUL));

// --------------------------------------------------------------------------
// get the item count and DRAGINFO structure
// --------------------------------------------------------------------------
   pusToDo = (PUSHORT)pDragXfer->ulTargetInfo;
   pDraginfo = gpDraginfo;

// --------------------------------------------------------------------------
// release the DRAGTRANSFER structure
// --------------------------------------------------------------------------
   KillXfer( pDragXfer );

// --------------------------------------------------------------------------
// count off an item
// --------------------------------------------------------------------------
   if( --(*pusToDo) == 0 ) {

// --------------------------------------------------------------------------
// Update the listbox - which was the target of the drop
// --------------------------------------------------------------------------
      WinSendMsg( ghwndList, LM_DELETEALL, 0l, 0l );
      WinSendMsg( hwnd, LAZY_FILLLIST,
                  MPFROMHWND(ghwndList), MPFROMP( NULL ) );

// --------------------------------------------------------------------------
// all items done, free the counter, the DRAGINFO strings, and the DRAGINFO.
// --------------------------------------------------------------------------
      free( pusToDo );
      DrgDeleteDraginfoStrHandles( pDraginfo );
      DrgFreeDraginfo( pDraginfo );
   } /* endif */

// --------------------------------------------------------------------------
// return the appropriate reserved value
// --------------------------------------------------------------------------
   return (MRESULT)0L;
}

/******************************************************************************
 *
 * Utility Functions
 *
 *****************************************************************************/

// *******************************************************************************
// FUNCTION:     lazy_filllist
//
// FUNCTION USE: Worker function to process the LAZY_FILLLIST message
//
// DESCRIPTION:  This message is posted by the main application window to
//               tell the object window to  fill the list box.
//
// PARAMETERS:   HWND     object window handle
//               MPARAM   listbox window handle
//               MPARAM   pointer to directory string
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT lazy_filllist( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   HWND           hwndList = (HWND)mp1;
   PSZ            pszDirectory = (PSZ)mp2;
   char           szFile[ CCHMAXPATHCOMP * 2 ] = "";/* plenty of room? */
   char           szName[ CCHMAXPATHCOMP * 2 ] = "";
   HDIR           hSearch = 0xFFFFFFFF;
   FILEFINDBUF3   findBuf;
   ULONG          ulFound = 1;
   ULONG          rc;

   do {
// --------------------------------------------------------------------------
// make sure the directory name string contains a directory string
// --------------------------------------------------------------------------
      if (pszDirectory != (PSZ)NULL) {
         if (pszDirectory[0] == '[') {
            pszDirectory++;
            pszDirectory[ strlen(pszDirectory) - 1 ] = '\0';
         } /* endif */

// --------------------------------------------------------------------------
// change the current directory to the passed directory then free the string
// --------------------------------------------------------------------------
         DosSetCurrentDir( pszDirectory );
         DosFreeMem( (PVOID)mp2 );
      } /* endif */

// --------------------------------------------------------------------------
// set the initial file mask to find all files
// --------------------------------------------------------------------------
      strcpy( szFile, "*.*" );

// --------------------------------------------------------------------------
// get the first file entry in the directory
// --------------------------------------------------------------------------
      rc = DosFindFirst( szFile,
                         &hSearch,
                         FILE_ARCHIVED | FILE_DIRECTORY | FILE_READONLY,
                         &findBuf,
                         sizeof( FILEFINDBUF3 ),
                         &ulFound,
                         FIL_STANDARD );
      if (rc != NO_ERROR) {
         break;
      } /* endif */

// --------------------------------------------------------------------------
// loop until no more files are found
// --------------------------------------------------------------------------
      while (ulFound != 0) {

// --------------------------------------------------------------------------
// if the file is a directory put brackets around it
// --------------------------------------------------------------------------
         if (findBuf.attrFile & FILE_DIRECTORY) {
            strcpy( szName, "[" );
            strcat( szName, findBuf.achName );
            strcat( szName, "]" );
         } else {

// --------------------------------------------------------------------------
// if not a directory just use the straight file name
// --------------------------------------------------------------------------
            strcpy( szName, findBuf.achName );
         } /* endif */

// --------------------------------------------------------------------------
// add the file name to the listbox
// --------------------------------------------------------------------------
         WinSendMsg( hwndList, LM_INSERTITEM,
                     MPFROMLONG(LIT_SORTASCENDING), MPFROMP(szName));

// --------------------------------------------------------------------------
// get the next file in the directory
// --------------------------------------------------------------------------
         rc = DosFindNext( hSearch, &findBuf, sizeof( FILEFINDBUF3 ), &ulFound );
         if (rc != NO_ERROR) {
            break;
         } /* endif */

      } /* endwhile */

   } while ( false ); /* enddo */

// --------------------------------------------------------------------------
// release the search handle
// --------------------------------------------------------------------------
   if (hSearch != 0xFFFFFFFF) {
      DosFindClose( hSearch );
   } /* endif */

// --------------------------------------------------------------------------
// return an appropriate value to keep the compiler happy.
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     MakeXfer
//
// FUNCTION USE: Utility function to allocate and initialize a DRAGTRANSER
//               structure.
//
// DESCRIPTION:  Allocates and fills the DRAGTRANSFER structure, setting
//               the target name equal to the source name
//
// PARAMETERS:   HWND      object window handle
//               PDRAGITEM pointer to DRAGITEM to be rendered
//               USHORT    type of operation to perform
//               PUSHORT   pointer to count of item's left to render
//
// RETURNS:      PDRAGTRANSFER   pointer to the allocated structure
//
// INTERNALS:    NONE
//
// *******************************************************************************
static PDRAGTRANSFER MakeXfer( HWND hwnd, PDRAGITEM pdi,
                               USHORT usOp, PUSHORT pusToDo )
{
   PDRAGTRANSFER  pXfer;
   ULONG          len;
   PSZ            pszSource;
   PSZ            pszTarget;

// --------------------------------------------------------------------------
// Allocate the DRAGTRANSFER structure
// --------------------------------------------------------------------------
   pXfer = DrgAllocDragtransfer( 1 );

   if( pXfer != (PDRAGTRANSFER)NULL ) {

// --------------------------------------------------------------------------
// clear the structure then set the structure size element
// --------------------------------------------------------------------------
      memset( pXfer, 0, sizeof( DRAGTRANSFER ));
      pXfer->cb = sizeof( DRAGTRANSFER );

// --------------------------------------------------------------------------
// indicate that the target window is the current window, set the pointer
// to the DRAGITEM structure, and set the RMF string to the single RMF this
// application supports.
// --------------------------------------------------------------------------
      pXfer->hwndClient = hwnd;
      pXfer->pditem = pdi;
      pXfer->hstrSelectedRMF = DrgAddStrHandle( "<DRM_OS2FILE,DRF_UNKNOWN>" );

// --------------------------------------------------------------------------
// get the length of the source file name string, allocate space for the
// string and retrieve the string based on its handle.
// --------------------------------------------------------------------------
      len = DrgQueryStrNameLen( pdi->hstrSourceName ) + 1;
      pszSource = (PSZ)malloc(len);
      DrgQueryStrName( pdi->hstrSourceName, len, pszSource );

// --------------------------------------------------------------------------
// set the target name to the source name - in the current directory
// --------------------------------------------------------------------------
      pszTarget = _fullpath( NULL, pszSource, CCHMAXPATH );
      pXfer->hstrRenderToName = DrgAddStrHandle( pszTarget );

// --------------------------------------------------------------------------
// free up the memory allocated for the strings
// --------------------------------------------------------------------------
      free( pszSource );
      free( pszTarget );

// --------------------------------------------------------------------------
// save the pointer to the count of items to transfer and the operation code
// --------------------------------------------------------------------------
      pXfer->ulTargetInfo = (ULONG)pusToDo;
      pXfer->usOperation = usOp;
      } /* endif */

// --------------------------------------------------------------------------
// return a pointer to the allocated structure
// --------------------------------------------------------------------------
   return pXfer;
}

// *******************************************************************************
// FUNCTION:     KillXfer
//
// FUNCTION USE: Utility function to deallocate a DRAGTRANSER structure.
//
// DESCRIPTION:  Free the string handles and then the DRAGTRANSFER structure.
//
// PARAMETERS:   PDRAGTRANFER    pointer to the structure to be freed.
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void KillXfer( PDRAGTRANSFER pDragXfer )
{
// --------------------------------------------------------------------------
// free the RMF string handle
// --------------------------------------------------------------------------
   if (pDragXfer->hstrSelectedRMF != (HSTR)NULL) {
      DrgDeleteStrHandle( pDragXfer->hstrSelectedRMF );
   } /* endif */

// --------------------------------------------------------------------------
// free the target name string handle
// --------------------------------------------------------------------------
   if (pDragXfer->hstrRenderToName != (HSTR)NULL) {
      DrgDeleteStrHandle( pDragXfer->hstrRenderToName );
   } /* endif */

// --------------------------------------------------------------------------
// free the DRAGTRANSFER structure
// --------------------------------------------------------------------------
   DrgFreeDragtransfer( pDragXfer );
}

// *******************************************************************************
// FUNCTION:     RenderFile
//
// FUNCTION USE: Render a file based on the DRAGTRANSFER structure.
//
// DESCRIPTION:
//
// PARAMETERS:   PDRAGTRANFER    pointer to the DRAGTRANSFER for object to render
//
// RETURNS:      BOOL            TRUE if successful
//
// INTERNALS:    NONE
//
// *******************************************************************************
static BOOL RenderFile ( PDRAGTRANSFER pdt )
{
   PDRAGITEM pdi = pdt->pditem;
   BOOL     bResult = FALSE;
   USHORT   cchRMF;
   PSZ      pszRMF;
   PSZ      pszSource;
   PSZ      pszContainer;
   PSZ      pszSourcePath;
   PSZ      pszTargetPath;
   ULONG    len1;
   ULONG    len;

// --------------------------------------------------------------------------
// for target rendering, the native RMF must be supported - get the length
// then the string - check for FILE TRANSFER
// --------------------------------------------------------------------------
   cchRMF = DrgQueryNativeRMFLen( pdi );
   pszRMF = (PSZ)malloc( ++cchRMF );
   DrgQueryNativeRMF( pdi, cchRMF, pszRMF );
   if( strstr( pszRMF, "DRM_OS2FILE" ) != NULL ) {

// --------------------------------------------------------------------------
// The source must also have provided the directory and file name -
// retreive the length of these, then the strings into allocated memory
// --------------------------------------------------------------------------
      if (pdi->hstrContainerName != (HSTR)NULLHANDLE &&
          pdi->hstrSourceName != (HSTR)NULLHANDLE ) {
// --------------------------------------------------------------------------
// do the source directory name
// --------------------------------------------------------------------------
         len1 = DrgQueryStrNameLen( pdi->hstrContainerName ) + 1;
         pszContainer = (PSZ)malloc( len1 );
         DrgQueryStrName( pdi->hstrContainerName, len1, pszContainer );

// --------------------------------------------------------------------------
// do the source file name
// --------------------------------------------------------------------------
         len = DrgQueryStrNameLen( pdi->hstrSourceName ) + 1;
         pszSource = (PSZ)malloc( len );
         DrgQueryStrName( pdi->hstrSourceName, len, pszSource );

// --------------------------------------------------------------------------
// now combine the two and free the individual strings
// --------------------------------------------------------------------------
         len += len1;
         pszSourcePath = (PSZ)malloc( len );
         strcpy( pszSourcePath, pszContainer );
         strcat( pszSourcePath, pszSource );
         free( pszContainer );
         free( pszSource );

// --------------------------------------------------------------------------
// Now get the target file name
// --------------------------------------------------------------------------
         len = DrgQueryStrNameLen( pdt->hstrRenderToName ) + 1;
         pszTargetPath = malloc( len );
         DrgQueryStrName( pdt->hstrRenderToName, len, pszTargetPath );

// --------------------------------------------------------------------------
// Copy the source file to the target
// --------------------------------------------------------------------------
         if (DosCopy( pszSourcePath, pszTargetPath, DCPY_EXISTING)
                             == NO_ERROR) {

            if (pdt->usOperation == DO_MOVE) {
// --------------------------------------------------------------------------
// Delete the source for a move operation
// --------------------------------------------------------------------------
               if (DosDelete(pszSourcePath) == NO_ERROR) {
                  bResult = TRUE;
               } /* endif */
            } else {
               bResult = TRUE;
            } /* endif */
         } /* endif */

// --------------------------------------------------------------------------
// free the memory allocated for the file names
// --------------------------------------------------------------------------
         free( pszSourcePath );
         free( pszTargetPath );
      } /* endif */
   } /* endif */

// --------------------------------------------------------------------------
// return result to caller
// --------------------------------------------------------------------------
   return bResult;
}

// *******************************************************************************
// FUNCTION:     lazy_dirfromini
//
// FUNCTION USE: Worker function to process the LAZY_DIRFROMINI message
//
// DESCRIPTION:  Sets the current drive and directory based on the values in
//               a predefined keys in an initialization file
//
// PARAMETERS:   HWND     object window handle
//               MPARAM   reserved
//               MPARAM   reserved
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT lazy_dirfromini( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   ULONG    ulDiskNum;
   ULONG    ulDiskMap;
   char     szCurDir[ CCHMAXPATH];
   char     szIniDir[ CCHMAXPATH];
   ULONG    cbCurDir = CCHMAXPATH;
   HWND     hwndFrame;

// --------------------------------------------------------------------------
// get the current drive and directory to use as defaults in case the INI
// file keys do not exist
// --------------------------------------------------------------------------
   DosQueryCurrentDisk( &ulDiskNum, &ulDiskMap );
   DosQueryCurrentDir( ulDiskNum, szCurDir, &cbCurDir );

// --------------------------------------------------------------------------
// Get the disk number from the INI file
// --------------------------------------------------------------------------
   ulDiskNum = PrfQueryProfileInt( HINI_USERPROFILE, INIAPPNAME, INICURDSKKEY,
                                   ulDiskNum );

// --------------------------------------------------------------------------
// Get the directory name from the INI file - leave room for slash at front
// --------------------------------------------------------------------------
   PrfQueryProfileString( HINI_USERPROFILE, INIAPPNAME, INICURDIRKEY,
                          szCurDir, &szIniDir[1], CCHMAXPATH );

// --------------------------------------------------------------------------
// put leading slash on directory to ensure that path is from root
// --------------------------------------------------------------------------
   szIniDir[ 0 ] = '\\';

// --------------------------------------------------------------------------
// modify the current drive and directory
// --------------------------------------------------------------------------
   DosSetDefaultDisk( ulDiskNum );
   DosSetCurrentDir( szIniDir );

// --------------------------------------------------------------------------
// Build the drive selection menu for the main application.
// --------------------------------------------------------------------------
   hwndFrame = WinQueryWindow( HWNDFROMMP(mp1), QW_PARENT );
   BuildDriveMenu( hwndFrame );

// --------------------------------------------------------------------------
// return appropriate value to keep the compiler happy
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     lazy_initfromdir
//
// FUNCTION USE: Worker function to process the LAZY_INIFROMDIR message
//
// DESCRIPTION:  Save the current drive and directory to an INI file using
//               predefined keys.
//
// PARAMETERS:   HWND     object window handle
//               MPARAM   reserved
//               MPARAM   reserved
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT lazy_inifromdir( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
   ULONG    ulDiskNum;
   ULONG    ulDiskMap;
   char     szCurDir[CCHMAXPATH];
   char     szDisk[ 3 ];
   ULONG    cbCurDir = CCHMAXPATH;

// --------------------------------------------------------------------------
// Get the current disk number and convert to an ASCII string
// --------------------------------------------------------------------------
   DosQueryCurrentDisk( &ulDiskNum, &ulDiskMap );
   szDisk[ 0 ] = (CHAR)(ulDiskNum/10) + 0x30;
   szDisk[ 1 ] = (CHAR)(ulDiskNum % 10) + 0x30;
   szDisk[ 2 ] = '\0';

// --------------------------------------------------------------------------
// save the current disk number
// --------------------------------------------------------------------------
   PrfWriteProfileString( HINI_USERPROFILE, INIAPPNAME, INICURDSKKEY, szDisk );

// --------------------------------------------------------------------------
// Get the current working directory
// --------------------------------------------------------------------------
   DosQueryCurrentDir( ulDiskNum, szCurDir, &cbCurDir );

// --------------------------------------------------------------------------
// save the current directory to the INI file
// --------------------------------------------------------------------------
   PrfWriteProfileString( HINI_USERPROFILE, INIAPPNAME, INICURDIRKEY,
                          szCurDir );

// --------------------------------------------------------------------------
// return appropriate value to keep the compiler happy
// --------------------------------------------------------------------------
   return MRFROMLONG(0L);
}

// *******************************************************************************
// FUNCTION:     lazy_discardobj
//
// FUNCTION USE: Worker function to process the LAZY_DISCARDOBJ message
//
// DESCRIPTION:  Delete the files specified in the DRAGINFO in mp1
//
// PARAMETERS:   HWND     object window handle
//               MPARAM   pointer to a DRAGINFO structure
//               MPARAM   reserved
//
// RETURNS:      MRESULT  reserved value of zero
//
// INTERNALS:    NONE
//
// *******************************************************************************
static MRESULT lazy_discardobj( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
// --------------------------------------------------------------------------
// Note - we should already have access since we don't free on the drop
// --------------------------------------------------------------------------
   PDRAGINFO      pdi = (PDRAGINFO)PVOIDFROMMP(mp1);
   PDRAGITEM      pditem;
   HWND           hwndList;
   USHORT         usItem;
   PSZ            pszSource;
   PSZ            pszContainer;
   PSZ            pszSourcePath;
   ULONG          len1;
   ULONG          len;

   for (usItem = 0; usItem < pdi->cditem; usItem++) {
      pditem = DrgQueryDragitemPtr( pdi, (ULONG)usItem );
// --------------------------------------------------------------------------
// The source must also have provided the directory and file name -
// retreive the length of these, then the strings into allocated memory
// --------------------------------------------------------------------------
      if (pditem->hstrContainerName != (HSTR)NULLHANDLE &&
          pditem->hstrSourceName != (HSTR)NULLHANDLE ) {
// --------------------------------------------------------------------------
// do the source directory name
// --------------------------------------------------------------------------
         len1 = DrgQueryStrNameLen( pditem->hstrContainerName ) + 1;
         pszContainer = (PSZ)malloc( len1 );
         DrgQueryStrName( pditem->hstrContainerName, len1, pszContainer );

// --------------------------------------------------------------------------
// do the source file name
// --------------------------------------------------------------------------
         len = DrgQueryStrNameLen( pditem->hstrSourceName ) + 1;
         pszSource = (PSZ)malloc( len );
         DrgQueryStrName( pditem->hstrSourceName, len, pszSource );

// --------------------------------------------------------------------------
// now combine the two and free the individual strings
// --------------------------------------------------------------------------
         len += len1;
         pszSourcePath = (PSZ)malloc( len );
         strcpy( pszSourcePath, pszContainer );
         strcat( pszSourcePath, pszSource );
         free( pszContainer );
         free( pszSource );

// --------------------------------------------------------------------------
// Delete the file and release the path name memory
// --------------------------------------------------------------------------
         DosDelete(pszSourcePath);
         free(pszSourcePath);
      } /* endif */
   } /* endfor */

// --------------------------------------------------------------------------
// Update the listbox - since this is source rendering for discard object
// we know hwndSource is the listbox handle.
// --------------------------------------------------------------------------
   hwndList = pdi->hwndSource;
   WinSendMsg( hwndList, LM_DELETEALL, 0l, 0l );
   WinSendMsg( hwnd, LAZY_FILLLIST, MPFROMHWND(hwndList), MPFROMP( NULL ) );

// --------------------------------------------------------------------------
// release the DRAGINFO structure
// --------------------------------------------------------------------------
   DrgDeleteDraginfoStrHandles( pdi );
   DrgFreeDraginfo( pdi );

   return MRFROMLONG(0l);
}

// *******************************************************************************
// FUNCTION:     BuildDriveMenu
//
// FUNCTION USE: Fill in the drive map menu for the main application window.
//
// DESCRIPTION:  Query the current drive and drive map.  Create a menu item
//               for each entry in the drive map and check the one for the
//               current drive.
//
// PARAMETERS:   HWND     Frame window parent of the menu
//
// RETURNS:      void
//
// INTERNALS:    NONE
//
// *******************************************************************************
static void BuildDriveMenu( HWND hwndFrame )
{
   HWND     hwndMenu;
   ULONG    ulCurDrive;
   ULONG    ulDriveMap;
   MENUITEM mi;
   ULONG    drive;
   ULONG    ulMask;
   char     szMenuText[2] = { '\0', '\0' };

// --------------------------------------------------------------------------
// Get the handle of the main menu
// --------------------------------------------------------------------------
   hwndMenu = WinWindowFromID( hwndFrame, FID_MENU );

// --------------------------------------------------------------------------
// Get the handle of the DRIVE sub menu
// --------------------------------------------------------------------------
   WinSendMsg( hwndMenu, MM_QUERYITEM, MPFROM2SHORT( MID_DRIVE, TRUE ),
                                       MPFROMP(&mi));
   hwndMenu = mi.hwndSubMenu;

// --------------------------------------------------------------------------
// Get the drive map
// --------------------------------------------------------------------------
   DosQueryCurrentDisk( &ulCurDrive, &ulDriveMap );
   ulCurDrive--;

// --------------------------------------------------------------------------
// Fill in the menu item structure - the menu items goes at the end of the
// submenu and is a text menu.
// --------------------------------------------------------------------------
   mi.iPosition = MIT_END;
   mi.afStyle = MIS_TEXT;
   mi.hwndSubMenu = NULLHANDLE;
   mi.hItem = NULLHANDLE;

// --------------------------------------------------------------------------
// Scan the drive map
// --------------------------------------------------------------------------
   for( drive = 0; drive < 26; drive++ ) {
      ulMask = 1 << drive;
      if ( ulDriveMap & ulMask ) {

// --------------------------------------------------------------------------
// Set the menu text and ID based on the drive number.  Check the menu item
// for the current drive.
// --------------------------------------------------------------------------
         szMenuText[ 0 ] = 'A' + (UCHAR)drive;
         mi.afAttribute = (drive == ulCurDrive) ? MIA_CHECKED : 0;
         mi.id = MID_DRIVEA + drive;
         WinSendMsg( hwndMenu, MM_INSERTITEM, MPFROMP( &mi ),
                                              MPFROMP( szMenuText ));
      } /* endif */
   } /* endfor */
}
