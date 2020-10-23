/************************* gvpset.c ********************************

   Creation date: 980220

   Revision date:     20-03-02
   Revision Purpose:  limit cpos adjustment to display size

   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added

   Revision date:     02-10-07
   Revision Purpose:  Added automatic cursor remove before size change

   Revision date:     16-11-07
   Revision Purpose:  Vfont adaption

   Version number: 2.4
   Copyright (c) RAMTEX Engineering Aps 1998-2007

*********************************************************************/

#include "gi_disp.h" /* gLCD prototypes */

#ifdef GVIEWPORT

/********************************************************************
   Segment: Viewport
   Level: Viewport
   Set current view-port in graphic pixels coordinates
   0,0 is upper left corner.
   0,0,0,0 is a view-port of 1 pixel.
   The view-port coordinates is limited to the LCD size (GDISPW-1,GDISPH-1).
   gselvp() should be called in advance to this function.
*/
void gsetvp(GXT xs, GYT ys, GXT xe, GYT ye )
   {
   if( xs > GDISPW-1 )
      {
      G_WARNING( "gsetvp: parameter, xs>GDISPW-1" );
      xs = GDISPW-1;
      }
   if( xe > GDISPW-1 )
      {
      G_WARNING( "gsetvp: parameter, xe>GDISPW-1" );
      xe = GDISPW-1;
      }
   if( ys > GDISPH-1 )
      {
      G_WARNING( "gsetvp: parameter, ys>GDISPH-1" );
      ys = GDISPH-1;
      }
   if( ye > GDISPH-1 )
      {
      G_WARNING( "gsetvp: parameter,ye>GDISPH-1" );
      ye = GDISPH-1;
      }

   if( xs > xe )
      {
      G_WARNING( "gsetvp: parameter, xs>xe" );
      xs = xe;
      }
    if( ys > ye )
      {
      G_WARNING( "gsetvp: parameter, ys>ye" );
      ys = ye;
      }

   gi_datacheck(); /* check internal data for errors */
   gi_cursor( 0 ); /* kill cursor before viewport size change
                     (cursor may be outside new viewport) */
   gcurvp->lt.x = xs;
   gcurvp->lt.y = ys;
   gcurvp->rb.x = xe;
   gcurvp->rb.y = ye;

   /* adjust ppos & cpos */
   gcurvp->cpos.x = xs;
   gcurvp->cpos.y = ys + ((gcurvp->pfont != NULL) ? gi_fsymh(gcurvp->pfont)-1 : 8-1);
   GLIMITU(gcurvp->cpos.y, gcurvp->rb.y);

   #ifndef GNOTXTSPACE
   gi_limit_check();   /* Limit spacing */
   #endif

   #ifdef GGRAPHICS
   gcurvp->ppos.x = xs;
   gcurvp->ppos.y = ys;
   #endif

   gi_calcdatacheck(); /* correct VP to new settings */
   gi_cursor( 1 );     /* set cursor */
   }

#ifdef GFUNC_VP

void gsetvp_vp( SGUCHAR vp, GXT xs, GYT ys, GXT xe, GYT ye )
   {
   GSETFUNCVP(vp, gsetvp(xs,ys,xe,ye) );
   }

#endif /* GFUNC_VP */
#endif


