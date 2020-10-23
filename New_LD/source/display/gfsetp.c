/************************* gfsetp.c ********************************

   Creation date: 980223

   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added

   Revision date:     13-06-06
   Revision Purpose:  gsetpos_vp parameter types changed to GXT,GYT

   Revision date:     26-03-07
   Revision Purpose:  Limit check fall back values corrected.

   Version number: 2.3
   Copyright (c) RAMTEX Engineering Aps 1998-2007

*********************************************************************/

#include "gi_disp.h" /* gLCD prototypes */

#if defined( GBASIC_TEXT ) || defined( GGRAPHICS )
/********************************************************************
   Segment: Viewport
   Level: Fonts
   Set character pos. in view-port in a pixel based fashion.
   Calculations and range limits are done relative to the current viewport
*/
void gsetpos( GXT xpos, GYT ypos )
   {
   gi_datacheck(); /* check internal data for errors */
   /* normalize to view-port */
   xpos = xpos + gcurvp->lt.x;
   ypos = ypos + gcurvp->lt.y;

   #if defined( GBASIC_TEXT ) && !defined( GHW_NO_HDW_FONT )
   if( gishwfont() )/* adjust to correct pos */
      {
      ghw_setcabspos( xpos, ypos );
      xpos = ghw_getcursorxpos(); /* back to abs pixel pos */
      ypos = ghw_getcursorypos();
      }
   #endif
   if( xpos < gcurvp->lt.x )
      {
      G_WARNING( "gsetpos: parameter, x<vp.left" );
      xpos = gcurvp->lt.x;
      }
   if( xpos > gcurvp->rb.x )
      {
      G_WARNING( "gsetpos: parameter, x>cp.right" );
      xpos = gcurvp->rb.x;
      }
   if( ypos < gcurvp->lt.y )
      {
      G_WARNING( "gsetpos: parameter, y<vp.top" );
      ypos = gcurvp->lt.y;
      }
   if( ypos > gcurvp->rb.y )
      {
      G_WARNING( "gsetpos: parameter, y>vp.bottom" );
      ypos = gcurvp->rb.y;
      }

   gi_cursor( 0 ); /* kill cursor */

   #ifdef GGRAPHICS
   gcurvp->ppos.x = xpos; /* update graphics pos also */
   gcurvp->ppos.y = ypos;
   #endif

   gcurvp->cpos.x = xpos;
   #ifndef GNOTXTSPACE
   if (gcurvp->chln.y != 0)
      {
      /* Compensate for extra line space */
      if (gcurvp->lt.y+gcurvp->chln.y > ypos)
         gcurvp->cpos.y = gcurvp->lt.y;
      else
         gcurvp->cpos.y = ypos - gcurvp->chln.y;
      }
   else
   #endif
      gcurvp->cpos.y = ypos;


   gi_calcdatacheck(); /* correct VP to new settings */
   gi_cursor( 1 ); /* set cursor */
   }


#ifdef GFUNC_VP

void gsetpos_vp( SGUCHAR vp, GXT xpos, GYT ypos )
   {
   GSETFUNCVP(vp, gsetpos(xpos,ypos) );
   }

#endif /* GFUNC_VP */

#endif /* GBASIC_TEXT */

