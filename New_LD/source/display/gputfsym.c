/************************* gputfsym.c ********************************

   Output font symbol at symbol index

   Note: if the font contains a code page then the symbol index is treated
   like a (wide) character and converted in accordance with the codepage settings.

   Creation date: 12-07-2011

   Revision date:         14-11-2012
   Revision Purpose:  Adapted for dynamic virtual fonts

   Version number: 1.1
   Copyright (c) RAMTEX Engineering Aps 2011-2012

*********************************************************************/

#include "gi_disp.h" /* gLCD prototypes */

#ifdef GSOFT_FONTS

#ifdef GVIRTUAL_FONTS_DYN
  #include <gvfont.h>
#endif

void gputfsym( GXT x, GYT y, SGUINT index, PGFONT pfont )
   {
   PGSYMBOL psym;
   gi_datacheck(); /* check internal data for errors */
   /* normalize to view-port */
   x += gcurvp->lt.x;
   y += gcurvp->lt.y;

   #ifdef GVIRTUAL_FONTS_DYN
   if (gvf_open(pfont))
   #else
   if( pfont == NULL )
   #endif
      {
      G_WARNING( "gputfsym: parameter, No font" );
      return;
      }

   /* Get pointer to symbol, use code page lookup if the font contains a codepage */
   if ((psym = gi_getsymbol((GWCHAR)index,pfont,gi_fpcodepage(pfont))) == NULL)
      {
      /* Warning already issued in gi_getsymbol(..) */
      return;
      }

   if( x < gcurvp->lt.x )
      {
      G_WARNING( "gputfsym: parameter, x<vp.left" );
      return;
      }
   if( x > gcurvp->rb.x )
      {
      G_WARNING( "gputfsym: parameter, x>vp.right" );
      return;
      }
   if( y < gcurvp->lt.y )
      {
      G_WARNING( "gputfsym: parameter, y<vp.top" );
      return;
      }
   if( y > gcurvp->rb.y )
      {
      G_WARNING( "gputfsym: parameter, y>vp.bottom" );
      return;
      }

   glcd_err = 0; /* Reset HW error flag */
   /* draw symbol in absolute cord */
   gi_putsymbol( x,y, gcurvp->rb.x, gcurvp->rb.y, psym,0,gi_fsymsize(pfont));
   ghw_updatehw();
   }

#ifdef GFUNC_VP

void gputfsym_vp( SGUCHAR vp, GXT x, GYT y, SGUINT index, PGFONT pfont )
   {
   GSETFUNCVP(vp, gputfsym(x,y,index,pfont) );
   }

#endif /* GFUNC_VP */
#endif /* GSOFT_FONTS */

