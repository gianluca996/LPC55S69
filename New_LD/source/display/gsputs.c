/**************************** gsputs.c ************************************

   This gsputs.c version is intended for use in designs without requirements
   for scrolling windows, auto alignment, or extra character or line spacing.

   The module provides a reduced feature set and produce a reduced memory footprint

   The text output features supported:
      Use of '\n', '\r', '\t'
      Use of GVPCLR_xx auto clear modes
      Use of GNORMAL, GINVERSE, GTRANSPERANT modes

   Text output features not supported compared to the normal gputs.c gfputch.c:
      No word wrap control     (equal to GNO_WRAP  mode setting always used)
      No full char wrap        (equal to GPARTIAL_CHAR or GLINECUT mode setting always used)
      No partial lines         (equal to GPARTIAL_LINE mode setting not used)
      No scroll control        (equal to GNOSCROLL mode switch always used)
      No auto alignment        (equal to GS_ALIGN switch always undefined, or GALIGN_xx modes setting not used)
      No extra spacing         (equal to GNOTXTSPACE switch always defined)

   Revision date:     11-11-10
   Revision Purpose:  Extraction of reduced feature set from the normal gsputs.c

   Version number: 1.0
   Copyright (c) RAMTEX Engineering Aps 2010

***************************************************************************/
#include "gi_disp.h"

#ifdef GBASIC_TEXT

/* Define convinent short hand notations */
#define  ltx gcurvp->lt.x
#define  lty gcurvp->lt.y
#define  rbx gcurvp->rb.x
#define  rby gcurvp->rb.y
#define  cposx gcurvp->cpos.x
#define  cposy gcurvp->cpos.y

/*
   Put string
*/
static void gi_puts( GSTRINGPTR cp )
   {
   GYT fh;
   GYT vph;
   SGBOOL transmode;
   GWCHAR val;

   if( cp.s == NULL )
      return;

   gi_put_prepare();
   fh = gfgetfh( gcurvp->pfont );
   vph = (rby-lty)+1;
   transmode = (G_IS_TRANSPERANT() && !G_IS_INVERSE()) ? 1 : 0;

   if (vph < fh)
      {
      G_WARNING( "gputch: Viewport height too small for character" );
      return; /* Viewport too small for font, skip output */
      }

   #ifndef GHW_NO_HDW_FONT
   if (gishwfont())
      gi_xyposalign();
   #endif

   /* Clear viewport above string ? */
   if (G_IS_VPCLR_UP() && (cposy >= fh) && !transmode)
      ghw_fill(ltx,lty,rbx,cposy-fh,(SGUINT)(G_IS_INVERSE() ? 0xffff : 0x0000));

   do
      {
      /* Clear viewport to the left of the string segment ? */
      if (G_IS_VPCLR_LEFT() && (cposx > ltx) && !transmode)
         ghw_fill(ltx, (GYT)((cposy < lty+fh) ? lty : cposy-(fh-1)),
                  (GXT)(cposx-1), (GYT)cposy,
                  (SGUINT)(G_IS_INVERSE() ? 0xffff : 0x0000));

      for(;;)
         {
         val = GETCHAR(cp);
         if (val == 0)
            break;
         GINCPTR(cp);
         if ((val == (GWCHAR)'\n') || (val == (GWCHAR)'\r'))
            break;
         if (cposx > rbx)
            continue; /* Line is full, waiting for \n \r or \0 */
         gi_putch( val );
         }

      if (val == (GWCHAR)'\r')
         {
         cposx = ltx;
         #ifndef GHW_NO_HDW_FONT
         if (gishwfont())
            gi_xyposalign();
         #endif
         }

      /* Clear viewport to the left of the string segment ? */
      if (G_IS_VPCLR_RIGHT() && (cposx <= rbx) && !transmode)
         ghw_fill((GXT) cposx,(GYT)((cposy < lty+fh) ? lty : cposy-(fh-1)),
                     rbx,cposy,(SGUINT)(G_IS_INVERSE() ? 0xffff : 0x0000));

      if (val == '\n')
         {
         if (gi_process_newline(fh)) /* Make new line processing */
            break;  /* a no-scroll condition reached, no more characters needed */
         }
      }
   while (val != 0);
   /* Clear viewport from line to bottom ? */
   if (G_IS_VPCLR_DOWN() && (cposy < rby) && !transmode)
      ghw_fill(ltx, cposy+1 ,rbx, rby,(SGUINT)(G_IS_INVERSE() ? 0xffff : 0x0000));

   gi_put_complete();
   }


void gputs( PGCSTR str )
   {
   GSTRINGPTR cp;
   cp.s = str;
   #ifdef GWIDECHAR
   GDATA_STRTYPE = 0;
   #endif
   gi_puts( cp );
   }

#ifdef GWIDECHAR
void gputsw( PGCWSTR str )
   {
   GSTRINGPTR cp;
   cp.ws = str;
   GDATA_STRTYPE = 1;
   gi_puts( cp );
   }
#endif

#ifdef GFUNC_VP

void gputs_vp( SGUCHAR vp, PGCSTR str )
   {
   GSETFUNCVP(vp, gputs( str ));
   }

#ifdef GWIDECHAR
void gputsw_vp( SGUCHAR vp, PGCWSTR str )
   {
   GSETFUNCVP(vp, gputsw( str ));
   }
#endif

#endif /* GFUNC_VP */

#endif /* GBASIC_TEXT */



