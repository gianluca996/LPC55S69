/************************* ggpixel.c *******************************

   Creation date: 980224

   Revision date:     03-01-26
   Revision Purpose:  Bit mask on GINVERSE mode

   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added

   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added

   Revision date:
   Revision Purpose:

   Version number: 2.3
   Copyright (c) RAMTEX Engineering Aps 1998-2004

*********************************************************************/

#include <display\gi_disp.h> /* gLCD prototypes */

#ifdef GGRAPHICS
/********************************************************************
   Segment: Graphics
   Level: Graphics
   Set pixel  == 0 = white, != 0 = black
   A pos. outside view-port is not drawn
*/
void gsetpixel( GXT xs, GYT ys, SGBOOL pixel )
   {
   gi_datacheck(); /* check internal data for errors */
   /* normalize to view-port */
   xs = xs + gcurvp->lt.x;
   ys = ys + gcurvp->lt.y;

   if( xs < gcurvp->lt.x )
      return;
   if( xs > gcurvp->rb.x )
      return;
   if( ys < gcurvp->lt.y )
      return;
   if( ys > gcurvp->rb.y )
      return;

   glcd_err = 0; /* Reset HW error flag */

   ghw_setpixel( xs,ys, (SGBOOL)(G_IS_INVERSE() ? !pixel : pixel));
   ghw_updatehw();
   }

/********************************************************************
   Segment: Graphics
   Level: Graphics
   Get pixel == 0 = white, != 0 = black
*/
SGBOOL ggetpixel( GXT xs, GYT ys )
   {
   SGBOOL pixel;
   gi_datacheck(); /* check internal data for errors */
   /* normalize to view-port */
   xs = xs + gcurvp->lt.x;
   ys = ys + gcurvp->lt.y;

   if( xs < gcurvp->lt.x )
      {
      G_WARNING( "ggetpixel: parameter, x<vp.left" );
      return 0;
      }
   if( xs > gcurvp->rb.x )
      {
      G_WARNING( "ggetpixel: parameter, x>vp.right" );
      return 0;
      }
   if( ys < gcurvp->lt.y )
      {
      G_WARNING( "ggetpixel: parameter, y<vp.top" );
      return 0;
      }
   if( ys > gcurvp->rb.y )
      {
      G_WARNING( "ggetpixel: parameter, y>vp.bottom" );
      return 0;
      }

   glcd_err = 0; /* Reset HW error flag */
   pixel = ghw_getpixel(xs,ys);
   return (G_IS_INVERSE() ? !pixel : pixel);
   }

#ifdef GFUNC_VP

void gsetpixel_vp( SGUCHAR vp, GXT xs, GYT ys, SGBOOL pixel )
   {
   GSETFUNCVP(vp, gsetpixel(xs,ys,pixel) );
   }

SGBOOL ggetpixel_vp( SGUCHAR vp, GXT xs, GYT ys )
   {
   SGBOOL retp;
   GGETFUNCVP(vp, ggetpixel(xs,ys) );
   return retp;
   }

#endif /* GFUNC_VP */

#endif /* GGRAPHICS */

