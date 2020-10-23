/************************* gvpfill.c *******************************

   Creation date: 980220

   Revision date:      25-07-02
   Revision Purpose:   Cursor is killed before fill

   Revision date:      03-01-26
   Revision Purpose:   Bit mask on GINVERSE mode

   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added

   Version number: 2.3
   Copyright (c) RAMTEX Engineering Aps 1998-2004

*********************************************************************/

#include "gi_disp.h" /* gLCD protoypes */

#ifdef GGRAPHICS
/********************************************************************
   Segment: View-port
   Level: View-port
   Fill area in view-port.
   The pattern word is used as a 2 byte pattern.
   The LSB byte of pattern ise used on even pixel lines and the MSB byte
   is used on odd pixel lines. This makes it easy to create a "gray" bit
   raster (for instance when pattern = 0x55aa or pattern = 0xaa55)
*/
void gfillvp( GXT xs, GYT ys, GXT xe, GYT ye, SGUINT f )
   {
   gi_datacheck(); /* check internal data for errors */

   gi_cursor( 0 );      /* kill cursor */

   /* normalize to view-port */
   xs = xs + gcurvp->lt.x;
   xe = xe + gcurvp->lt.x;
   ys = ys + gcurvp->lt.y;
   ye = ye + gcurvp->lt.y;

   /* limit values to view-port */
   LIMITTOVP( "gfillvp", xs,ys,xe,ye );

   glcd_err = 0; /* Reset HW error flag */
   ghw_fill(  xs,
              ys,
              xe,
              ye,
              (SGUINT)(G_IS_INVERSE() ? ~f : f)
              );

   ghw_updatehw();

   gi_cursor( 1 );      /* set cursor */

   gi_calcdatacheck(); /* correct VP to new settings */
   }

#ifdef GFUNC_VP

void gfillvp_vp( SGUCHAR vp, GXT xs, GYT ys, GXT xe, GYT ye, SGUINT f )
   {
   GSETFUNCVP(vp, gfillvp(xs,ys,xe,ye,f) );
   }

#endif /* GFUNC_VP */
#endif

