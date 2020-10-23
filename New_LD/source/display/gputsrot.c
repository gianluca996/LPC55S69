/**************************** gputsrot.c *****************************

   Functions for rotated write of text strings.

   One or more text lines defines a "text frame" area. This area is
   virtually rotated as a whole. The connection point for the rotation
   anchor is defined by the normal alignment mode settings.

   The viewport mode alignment setting therefore influences both where
   the acher point is connected to the text frame and the alignment of text
   within the frame.

   Effect of anchor point alignment on text output start position in text frame
      GALIGN_LEFT      Left align text, anchor at left edge of text frame
      GALIGN_HCENTER   Center text lines, anchor at horizontal center of
                       text frame
      GALIGN_RIGHT     Right align text, anchor at right frame edge
      (default)        Normal, Ancher is left corner of first text symbol.

      GALIGN_TOP       Top adjust text, Ancher at Top edge
      GALIGN_VCENTER   Center text vertically, anchor at vertical center
                       of text frame
      GALIGN_BOTTOM    Bottom align text, anchor at bottom frame edge
      (default)        Normal, Ancher is lower corner of first text symbol.

   Creation date: 13-11-2012

   Revision Purpose:
   Revision date:

   Version number: 1.0
   Copyright (c) RAMTEX Engineering Aps 2012

***********************************************************************/
#include "gfixedpt.h" /* Fixed point functions (macros) */
#include "gi_disp.h"
#include <stdio.h>

#if defined( GBASIC_TEXT ) && defined( GSOFT_SYMBOLS )

/*
   Output of rotated text string.
   This internal function common for char, widechar and multibyte char strings
*/

static void gi_puts_rotate( GSTRINGPTR *sm, float angle)
   {
   SGFIXP xb, yb, x, y, dy, h, w;
   SGFIXP sw;
   SGINT line;
   GSTRINGPTR cp;
   GWCHAR val;
   #ifdef GBUFFER
   GUPDATE update;
   #endif

   #ifndef GHW_NO_HDW_FONT
   if (gishwfont())
      return; /* No support for hardware fonts. Use gputs() instead */
   #endif
   if( sm->s == NULL )
      return;
   cp.s = sm->s;
   if  ((val = GETCHAR(cp)) != 0)
      {
      #ifdef GBUFFER
      update = gsetupdate(GUPDATE_OFF);  /* Enable drawing completion before flush */
      #endif

      gi_setfpsincos(angle);  /* Prepare for fixed point calculations */

      xb = SGU_TO_FIXP( gcurvp->cpos.x);
      yb = SGU_TO_FIXP( gcurvp->cpos.y);
      line = 0;
      w = dy = x = y = 0;
      h = SGU_TO_FIXP(ggetfh()); /* Includes line spacing settings */
      do
         {
         if ((line == 0) || (val == (GWCHAR)'\n') || (val == (GWCHAR)'\r'))
            {
            if ((line == 0) || (val == (GWCHAR)'\n'))
               line++;

            if ((val == (GWCHAR)'\n') || (val == (GWCHAR)'\r'))
               GINCPTR(cp);

            #ifdef GS_ALIGN
            if ((gcurvp->mode & GALIGN_VCENTER)!=0)
               {
               switch (gcurvp->mode & GALIGN_VCENTER)
                  {
                  case GALIGN_TOP:
                     dy = h*line-1;
                     break;
                  case GALIGN_BOTTOM:
                     dy = (gi_strlines( cp.s )-1)*(-h);
                     break;
                  case GALIGN_VCENTER:
                     dy = (gi_strlines( cp.s )*(-h))/2+(h-SGU_TO_FIXP(1));
                     break;
                  default:
                     if (line <= 1)
                        dy = (gi_strlines( cp.s )*(-h))/2+h;
                     else
                        dy+=h;
                     break;
                  }
               }
            else
            #endif
               dy = h*(line-1);

            #ifdef GS_ALIGN
            if ((gcurvp->mode & GALIGN_HCENTER)!=0)
               {
               switch (gcurvp->mode & GALIGN_HCENTER)
                  {
                  case GALIGN_RIGHT:
                     w = SGU_TO_FIXP(gi_strlen( cp.s, 0))*(-1);
                     break;
                  case GALIGN_HCENTER:
                     w = SGU_TO_FIXP(gi_strlen( cp.s, 0)+1)/(-2);
                     break;
                  default:
                     w = 0;
                     break;
                  }
               }
            else
            #endif
               w = 0;
            x = xb+(gi_fp_cos*w  + gi_fp_sin*dy)/SGFIXP_SCALE;
            y = yb+(gi_fp_cos*dy - gi_fp_sin*w)/SGFIXP_SCALE;
            if ((val == (GWCHAR)'\n') || (val == (GWCHAR)'\r'))
               continue;
            }

         if (val == (GWCHAR)'\t')
            val = (GWCHAR)' ';      /* Potentially fetch full multibyte char */

         //w += gi_putchw_rotate( val, x, y );
         sw = gi_putchw_rotate( val, x, y );
                 w += sw;
         x = xb+(gi_fp_cos*w  + gi_fp_sin*dy)/SGFIXP_SCALE;
         y = yb+(gi_fp_cos*dy - gi_fp_sin*w)/SGFIXP_SCALE;

         GINCPTR(cp);
         }
      while ((val = GETCHAR(cp)) != 0);

      /* Update viewport coordinates with text end point */
      /* Force end coordinate inside viewport (and screen), just in case */
      if (x < SGU_TO_FIXP(gcurvp->lt.x))
         gcurvp->cpos.x = gcurvp->lt.x;
      else
      if (x > SGU_TO_FIXP(gcurvp->rb.x))
         gcurvp->cpos.x = gcurvp->rb.x;
      else
         gcurvp->cpos.x = (GXT)SGFIXP_TO_U(x);

      if (y < SGU_TO_FIXP(gcurvp->lt.y))
         gcurvp->cpos.y = gcurvp->lt.y;
      else
      if (y > SGU_TO_FIXP(gcurvp->rb.y))
         gcurvp->cpos.y = gcurvp->rb.y;
      else
         gcurvp->cpos.y = (GYT)SGFIXP_TO_U(y);
      #ifdef GGRAPHICS
      gcurvp->ppos.x = gcurvp->cpos.x; /* update graphics pos also */
      gcurvp->ppos.y = gcurvp->cpos.y;
      #endif

      gsetupdate(update);  /* Enable drawing completion before flush */

      ghw_updatehw();
      gi_calcdatacheck(); /* correct VP to new settings */
      }
   }

void gputsrot( PGCSTR str, float angle)
   {
   GSTRINGPTR sm;
   sm.s = str;
   #ifdef GWIDECHAR
   GDATA_STRTYPE = 0;
   #endif
   gi_puts_rotate( &sm, angle );
   }

#ifdef GWIDECHAR
void gputswrot( PGCWSTR str, float angle)
   {
   GSTRINGPTR sm;
   sm.ws = str;
   GDATA_STRTYPE = 1;
   gi_puts_rotate( &sm, angle );
   }
#endif

#ifdef GFUNC_VP

void gputsrot_vp( SGUCHAR vp, PGCSTR str, float angle )
   {
   GSETFUNCVP(vp, gputsrot( str, angle ));
   }

#ifdef GWIDECHAR
void gputswrot_vp( SGUCHAR vp, PGCWSTR str, float angle )
   {
   GSETFUNCVP(vp, gputswrot( str, angle ));
   }
#endif

#endif /* GFUNC_VP */

#endif /* GBASIC_TEXT && GSOFT_SYMBOLS  */

