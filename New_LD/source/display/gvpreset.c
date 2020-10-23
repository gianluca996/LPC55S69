/************************* gvpreset.c ******************************

   Creation date: 980220

   Revision date:    05-05-03
   Revision Purpose: Cursor position tracks font symheight.
                     gresetposvp() added.

   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added

   Revision date:     16-11-07
   Revision Purpose:  Vfont adaption

   Revision date:     05-02-08
   Revision Purpose:  Added support for inter character spacing

   Revision date:     13-06-08
   Revision Purpose:  Viewport reset code made an internal function and
                      shared with ginit()

   Revision date:
   Revision Purpose:

   Version number: 2.4
   Copyright (c) RAMTEX Engineering Aps 1998-2008

*********************************************************************/

#include "gi_disp.h" /* gLCD prototypes */


#ifdef GVIEWPORT

void gi_resetvp(SGUCHAR resettype);  /* Located in gvpinit.c */

/********************************************************************
   Segment: Viewport
   Level: View-port
   Reset the current view-port to full screen,
   set the cursor position to the upper left corner,
   set the pixel position to the upper left corner,
*/
void gresetposvp(void)
   {
   gi_datacheck();     /* check internal data for errors (assure valid defaults) */
   gi_cursor( 0 );     /* kill cursor (using old font size) */

   gi_resetvp(2);

   gi_cursor( 1 );     /* set cursor (using new font size) */
   }

/********************************************************************
   Segment: Viewport
   Level: View-port
   Reset the current view-port to full screen,
   set the cursor position to the upper left corner,
   set the pixel position to the upper left corner,
   set mode to GNORMAL,
   set font to SYSFONT,
   set code-page to NULL.
*/
void gresetvp(void)
   {
   gi_datacheck();     /* check internal data for errors (assure valid defaults) */
   gi_cursor( 0 );     /* kill cursor (using old font size) */

   gi_resetvp(1);

   gi_cursor( 1 );     /* set cursor (using new font size) */
   }

#ifdef GFUNC_VP

void gresetposvp_vp( SGUCHAR vp )
   {
   GSETFUNCVP(vp, gi_resetvp(2) );
   }

void gresetvp_vp( SGUCHAR vp )
   {
   GSETFUNCVP(vp, gi_resetvp(1) );
   }

#endif /* GFUNC_VP */
#endif

