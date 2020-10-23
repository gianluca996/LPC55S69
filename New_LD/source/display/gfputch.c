/**************************** gfputch.c ************************************

   This gfputch.c version is intended for use in designs without requirements
   for scrolling windows, or extra character or line spacing.

   The module provides a reduced feature set and produce a reduced memory footprint

   The feature set is eual to when these settings are used with the normal
   gputs version:
      Configuration switches:
          Define    GNOCURSOR
          Define    GNOTXTSPACE
      Mode settings:
          GNOSCROLL
          GNO_WRAP
          GPARTIAL_CHAR
      + no linkage to ghw_scroll() module.

   Use of '\n', '\r', '\t' is supported.

   Revision date:     11-11-10
   Revision Purpose:  Extraction of reduced feature set from the normal gfputch.c

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

#ifndef GHW_NO_HDW_FONT
/* Align cursor pos to hw char matrix */
void gi_xyposalign(void)
   {
   if (((SGUINT) cposx + GDISPW-1) <= rbx)
      /* Not at end of line */
      if ((cposx = (cposx / GDISPCW)*GDISPCW) < ltx)
         cposx += GDISPCW;

   cposy = ((cposy / GDISPCH)*GDISPCH) + GDISPCH-1;
   if (cposy < lty)
      cposy += GDISPCH;
   if (cposy > rby)
      cposy -= GDISPCH;
   }
#endif

SGUCHAR gi_process_newline(GYT lnsp)
   {
   if (lnsp == 0)
      lnsp = gfgetfh( gcurvp->pfont );

   if (((SGUINT) cposy+lnsp) <= ((SGUINT)rby))
      {
      ghw_updatehw();   /* Update hardware here to speed buffered mode */
      cposy = cposy + lnsp;    /* Inside vp area, just advance position */
      }
   else
      return 1;
   cposx = ltx;
   #ifndef GHW_NO_HDW_FONT
   if (gishwfont())
      gi_xyposalign();
   #endif
   return 0;
   }

/* Do checks, kill cursor and prepare for viewport relative calculations */
void gi_put_prepare(void)
   {
   gi_datacheck(); /* check internal data for errors */
   glcd_err = 0;   /* Reset HW error flag */

   gi_cursor( 0 ); /* kill cursor */

   #ifndef GHW_NO_HDW_FONT
   if (gishwfont())
      {
       /* Align cursor pos to hw char matrix */
      gi_xyposalign();
      }
   #endif
   }

/* Do checks, reposition cursor, update absolute viewport values */
void gi_put_complete(void)
   {
   /* update viewport cursor data */
   if (cposx > rbx) cposx = rbx;
   if (cposy > rby) cposy = rby;

   gi_cursor( cursor_on ); /* set cursor on if it was on */

   #ifdef GGRAPHICS
   gcurvp->ppos.x = cposx; /* update graphics pos also */
   gcurvp->ppos.y = cposy;
   #endif

   ghw_updatehw();
   gi_calcdatacheck(); /* correct VP to new settings */
   }

/* return aboslute step position  */
GXT gi_tabstep(GXT x)
   {
   #ifndef GCONSTTAB
   SGUCHAR i,n;
   #endif
   x = (GXT) (x - ltx);
   /* find next tab */
   #ifdef GCONSTTAB
   x = ((x/GTABSIZE)+1)*GTABSIZE+ltx;
   #else
   n = sizeof(GDATA_TABS)/sizeof(GXT);
   for( i=0; i<n; i++ )
      if( x < GDATA_TABS[i] )
         break;
   if( i < n ) /* Tab found */
      {
      x = GDATA_TABS[i]+ltx;
      if( x > rbx )
         x = rbx;
      }
   else /* tab not found */
      x = rbx;
   #endif
   return x;
   }

/*
   Output character to screen,
   assuming that it is printable
   ('\n' and '\r' is handled at a higher level)

   Returns 0 if printed
   Returns symbol width if not printed (not room in vp, and ! GLINECUT)
*/
char gi_putch( GWCHAR val )
   {
   GXT w;
   if ((val == (GWCHAR)'\n') || (val == (GWCHAR)'\r'))
      return 0; /* No output, positions are handled at the level above */
   if (val == (GWCHAR)'\t')
      { /* Tabulator is handled like a single variable width symbol */
      if ((w = gi_tabstep(cposx)) != ltx)
         {
         GYT fh = gfgetfh( gcurvp->pfont );
         ghw_fill(cposx, (GYT)((cposy < lty+fh) ? lty : (cposy-(fh-1))),
                 w, cposy,(SGUINT)(G_IS_INVERSE() ? 0xffff : 0x0000));
         cposx = w; /* Move position to tab setting or end of viewport */
         }
      }
   else
      {
      #ifndef GHW_NO_HDW_FONT
      if( gishwfont() )
         {
         /* Assure absolute matrix positions is ok */
         ghw_setcabspos( cposx, cposy );
         cposx = ghw_getcursorxpos(); /* back to abs pixel pos */
         cposy = ghw_getcursorypos();
         if(((SGUINT) cposx + (GDISPCW-1)) > (SGUINT) rbx )
            return 1;   /* not room for symbol, skip to avoid viewport overrun */

         ghw_putch( (SGUCHAR) val );
         if ((SGUINT) cposx + GDISPCW > (SGUINT) rbx)
            cposx = rbx+1;
         else
            cposx += GDISPCW;
         }
      #endif
      #if (defined( GSOFT_FONTS ) && !defined (GHW_NO_HDW_FONT))
      else
      #endif
      #ifdef GSOFT_FONTS
         {
         PGSYMBOL psymbol; /* pointer to a symbol */
         GYT fh;
         fh = gfgetfh( gcurvp->pfont );

         if (cposx > rbx)
            {
            cposx = rbx+1;
            return 0; /* Line overflow by previous char, skip (waiting for \n or \r) */
            }

         psymbol = gi_getsymbol( val , gcurvp->pfont, gcurvp->codepagep);

         if( psymbol == NULL )
            {
            G_WARNING( "gputch: Character have undefined symbol" );
            return 0;
            }
         w = gsymw(psymbol);
         gi_putsymbol( cposx,
                       (GYT)((cposy+1)-fh),
                       rbx, rby,
                       psymbol,
                       (GYT)(( fh <= cposy-lty ) ? 0 : fh - ((cposy-lty)+1)), /* yoffset*/
                       gi_fsymsize(gcurvp->pfont));

         if ((SGUINT) cposx + w > (SGUINT) rbx)
            cposx = rbx;
         else
            {
            cposx = cposx + w;
            }
         }
      #endif /* GSOFT_FONTS */
      }
   return 0;

   }


/*
   gputchw  use these mode setting attributes
      GNORMAL,        Normal mode (not inversed, not aligned)
      GINVERSE        Inverse color (typical white on black)
      GTRANSPERANT    Symbol background color is transperant color

      gputch  maps to gputchw using an appropriate cast
*/
void gputchw( GWCHAR val )
   {
   gi_put_prepare();

   switch( val )
      {
      case ((GWCHAR)'\n'):
         /* Make new line processing */
         gi_process_newline(0);
         break;
      case ((GWCHAR)'\r'):
         cposx = ltx;
         #ifndef GHW_NO_HDW_FONT
         if (gishwfont())
            gi_xyposalign();
         #endif
         break;
      default:
         {
         if (gi_putch( val )) /* process character, incl '\t' */
            {
            /* end of viewport line reached, character put skipped */
            if (!G_IS_NOWRAP())
               {
               if (gi_process_newline(0)) /* Make new line processing */
                  break;  /* a no-scroll condition reached, no more characters needed */
               gi_putch( val ); /* Retry on new position */
               }
            }
         }
      }

   gi_put_complete();
   }

#ifdef GFUNC_VP

void gputchw_vp( SGUCHAR vp, GWCHAR val )
   {
   GSETFUNCVP(vp, gputchw(val) );
   }

#endif /* GFUNC_VP */

#endif /* GBASIC_TEXT */

