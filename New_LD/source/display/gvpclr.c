/************************* gvpclr.c ********************************

   Creation date: 980220

   Revision date:      03-01-26
   Revision Purpose:   Bit mask on GINVERSE mode

   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added

   Revision date:     15-10-05
   Revision Purpose:  cpos.y update forced inside viewport bottom
                      (safer handling of errors where font height
                       exceeds viewport height)

   Revision date:     02-10-07
   Revision Purpose:  Added automatic cursor remove before clear

   Revision date:     16-11-07
   Revision Purpose:  Vfont adaption

   Version number: 2.5
   Copyright (c) RAMTEX Engineering Aps 1998-2007

*********************************************************************/

#include "gi_disp.h" /* gLCD prototypes */

#ifdef GVIEWPORT
/********************************************************************
   Segment: Viewport
   Level: Viewport
   Clear view-port and set the cursor position to the upper left corner,
   cleared to white or black depending on graphic mode.
*/
void gclrvp(void)
   {
   glcd_err = 0;   /* Reset HW error flag */
   gi_datacheck(); /* check internal data for errors */
   gi_cursor( 0 ); /* kill cursor before clear (needed for inversed cursor mode) */
   ghw_fill(
           gcurvp->lt.x,
           gcurvp->lt.y,
           gcurvp->rb.x,
           gcurvp->rb.y,
           (SGUINT)(G_IS_INVERSE() ? 0xFFFF : 0x0000)
           );

   ghw_updatehw();

   /* reset char position */
   gcurvp->cpos.x = gcurvp->lt.x;
   gcurvp->cpos.y = gcurvp->lt.y + gi_fsymh(gcurvp->pfont)-1;
   GLIMITU(gcurvp->cpos.x,GDISPW-1);
   GLIMITU(gcurvp->cpos.y,gcurvp->rb.y);
   gi_calcdatacheck(); /* correct VP to new settings */
   gi_cursor( 1 );     /* set cursor */

   #ifdef GGRAPHICS
   /* reset pixel position */
   gcurvp->ppos.x = gcurvp->lt.x;
   gcurvp->ppos.y = gcurvp->lt.y;
   #endif /* GGRAPHICS */

   gi_calcdatacheck(); /* correct VP to new settings */
   }

#ifdef GFUNC_VP

void gclrvp_vp( SGUCHAR vp )
   {
   GSETFUNCVP(vp, gclrvp() );
   }

#endif /* GFUNC_VP */
#endif

