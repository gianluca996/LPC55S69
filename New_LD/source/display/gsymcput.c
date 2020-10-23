/************************* gsymcput.c ********************************

   Creation date: 980223

   Revision date:     02-01-23
   Revision Purpose:  symsize parameter added to gi_putsymbol(..)
   Revision date:     03-01-26
   Revision Purpose:  Bit mask on GINVERSE mode
   Revision date:     03-05-20
   Revision Purpose:  gi_putsymbol parameter types updated for large font support.
   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added
   Revision date:     25-01-08
   Revision Purpose:  Major redesign in alignment with new gputs gfputch implementation
   Revision date:     14-11-12
   Revision Purpose:  Named dynamic virtual font support, font size optimization

   Version number: 2.8
   Copyright (c) RAMTEX Engineering Aps 1998-2012

*********************************************************************/

#include "gi_disp.h" /* GLCD prototypes */

#if defined( GSOFT_SYMBOLS ) && defined( GBASIC_TEXT )

/* Define convinent short hand notations */
#define  ltx gcurvp->lt.x
#define  lty gcurvp->lt.y
#define  rbx gcurvp->rb.x
#define  rby gcurvp->rb.y
#define  cposx gcurvp->cpos.x
#define  cposy gcurvp->cpos.y

/* Support functions in gfputch.c externally referenced from this module */
#ifndef GNOTXTSPACE
void gi_clr_chsp(GYT h);
#endif

#ifdef GVIRTUAL_FONTS_DYN
  #include <gvfont.h>
#endif

/********************************************************************
   Segment: Software symbols
   Level: Fonts
   Draws a symbol in view-port at next char pos. and update char pos.
   Can be used to embed symbol in text lines.
*/
void gputcsym( PGSYMBOL psymbol )
   {
   GXT w;
   GYT h;

   #ifdef GVIRTUAL_FONTS_DYN
   if (gvfsym_open(psymbol))
   #else
   if (psymbol == NULL)
   #endif
      return;

   gi_put_prepare();
   w = gsymw(psymbol);
   h = gsymh(psymbol);

   #ifndef GNOTXTSPACE
   if ((SGUINT) cposx + w + gcurvp->chln.x > (SGUINT) rbx)
   #else
   if ((SGUINT) cposx + w > (SGUINT) rbx)
   #endif
      {
      if (G_IS_NOWRAP())
         {
         if (!G_IS_PARTIAL_CHAR())
            goto skip_symbol; /* no room for symbol */
         }
      else
         {
         /* Make a character wrap in advance */
         if (gi_process_newline(h) != 0)
            goto skip_symbol; /* a no-scroll condition reached, no room for character */
         }
      }

   gi_putsymbol( cposx,
                 (GYT)((cposy+1)-h),
                 rbx, rby,
                 psymbol,
                 (GYT)(( h <= cposy-lty ) ? 0 : h - ((cposy-lty)+1)), /* yoffset*/
                 0);


   #ifndef GNOTXTSPACE
   if ((SGUINT) cposx + w + gcurvp->chln.x > (SGUINT) rbx)
   #else
   if ((SGUINT) cposx + w > (SGUINT) rbx)
   #endif
      cposx = rbx;
   else
      {
      cposx = cposx + w;
      #ifndef GNOTXTSPACE
      if (gcurvp->chln.x > 0)
         gi_clr_chsp(h); /* Clear extra char spacing if needed (spacing is assumed to be equal to symbol)*/
      #endif
      }

   skip_symbol:
   gi_put_complete();
   }

#ifdef GFUNC_VP

void gputcsym_vp( SGUCHAR vp, PGSYMBOL psymbol )
   {
   GSETFUNCVP(vp, gputcsym(psymbol) );
   }

#endif /* GFUNC_VP */

#endif /* GSOFT_SYMBOLS && GBASIC_TEXT */

