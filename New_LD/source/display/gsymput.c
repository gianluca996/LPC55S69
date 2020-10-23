/************************* gsymput.c ********************************

   Creation date: 980223

   Revision date:     02-01-23
   Revision Purpose:  symsize parameter added to gi_putsymbol(..)

   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added

   Revision date:     14-11-12
   Revision Purpose:  Dyn vfont support added

   Version number: 2.2
   Copyright (c) RAMTEX Engineering Aps 1998-2012

*********************************************************************/

#include "gi_disp.h" /* gLCD prototypes */

#ifdef GSOFT_SYMBOLS

#ifdef GVIRTUAL_FONTS_DYN
  #include <gvfont.h>
#endif

/********************************************************************
   Segment: Software symbols
   Level: Fonts
   Draws a symbol in view-port.
   Can be used to update icons and to show a large sign-on picture
*/
void gputsym( GXT x, GYT y, PGSYMBOL psym )
   {
   gi_datacheck(); /* check internal data for errors */
   /* normalize to view-port */
   x += gcurvp->lt.x;
   y += gcurvp->lt.y;

   #ifdef GVIRTUAL_FONTS_DYN
   if (gvfsym_open(psym))
   #else
   if( psym == NULL )
   #endif
      {
      G_WARNING( "gputsym: parameter, No symbol" );
      return;
      }
   if( x < gcurvp->lt.x )
      {
      G_WARNING( "gputsym: parameter, x<vp.left" );
      return;
      }
   if( x > gcurvp->rb.x )
      {
      G_WARNING( "gputsym: parameter, x>vp.right" );
      return;
      }
   if( y < gcurvp->lt.y )
      {
      G_WARNING( "gputsym: parameter, y<vp.top" );
      return;
      }
   if( y > gcurvp->rb.y )
      {
      G_WARNING( "gputsym: parameter, y>vp.bottom" );
      return;
      }
   glcd_err = 0; /* Reset HW error flag */
   /* draw symbol in absolute cord */
   gi_putsymbol( x,y, gcurvp->rb.x, gcurvp->rb.y, psym,0,0 );
   ghw_updatehw();
   }

#ifdef GFUNC_VP

void gputsym_vp( SGUCHAR vp, GXT x, GYT y, PGSYMBOL psym )
   {
   GSETFUNCVP(vp, gputsym(x,y,psym) );
   }

#endif /* GFUNC_VP */
#endif /* GSOFT_SYMBOLS */

