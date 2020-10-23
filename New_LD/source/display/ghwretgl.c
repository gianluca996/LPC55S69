/************************** ghwretgl.c *****************************

   Low-level function for drawing rectangles or straight vertical or
   horizontal lines.

   Absolute coordinates are used.

   The KS07XX controller is assumed to be used with a LCD module.
   The following LCD module characteristics MUST be correctly
   defined in GDISPCFG.H:

      GDISPW  Display width in pixels
      GDISPH  Display height in pixels
      GBUFFER If defined most of the functions operates on
              a memory buffer instead of the LCD hardware.
              The memory buffer content is copied to the LCD
              display with ghw_updatehw().
              (Equal to an implementation of delayed write)

   Revision date:     170804
   Revision Purpose:  Modified to use ghw_set_xy(..)
   Revision date:     061005
   Revision Purpose:  GBUFFER mode speed optimized
   Revision date:     240206
   Revision Purpose:  Correction, x declared as GXT, not GYT

   Version number: 1.1
   Copyright (c) RAMTEX Engineering Aps 2002-5

*********************************************************************/

#include "gdisphw.h"   /* HW driver prototypes and types */
#include "ks07xx.h"    /* KS07XX controller specific definements */

#ifdef GGRAPHICS



/*
   Draw vertical line
*/
static void ghw_linev(GXT xb, GYT yb, GYT ye, SGBOOL pixel )
   {
   SGUINT GFAST y;
   SGUCHAR val,stpmsk;

   #ifdef GBUFFER
   GBUFINT gbufidx;
   GBUF_CHECK();
   invalx( xb );
   invaly( yb );
   invaly( ye );
   gbufidx = GINDEX(xb,(yb/8));
   #endif

   val = startmask[yb & 0x7];
   stpmsk = stopmask[ye & 0x7];
   ye &= ~(0x7);

   for (y = yb & ~(0x7); y <= ye; y+=GDISPCH)
      {
      if (y == ye)
         val &= stpmsk;
      #ifdef GBUFFER

      /* Write destination */
      if( pixel )
        gbuf[gbufidx] = (val != 0xff) ? val | gbuf[gbufidx] : val;
      else
        gbuf[gbufidx] = (val != 0xff) ? ~val & gbuf[gbufidx] : ~val;
      gbufidx += GDISPW;

      #else
      ghw_set_xypos(xb,(GYT)y);
      if( pixel )
        ghw_auto_wr((SGUCHAR)((val != 0xff) ?  val | ghw_rd() : val));
      else
        ghw_auto_wr((SGUCHAR)((val != 0xff) ? ~val & ghw_rd() : ~val));

      #endif

      val = 0xff;
      }
   #ifdef GBUFFER
   if (ghw_upddelay == 0)
      ghw_updatehw();
   #endif
   }

/*
   Draw horisontal line
*/
static void ghw_lineh(GXT xb, GYT yb, GXT xe, SGBOOL pixel )
   {
   GXT GFAST x;
   SGUCHAR GFAST msk;

   #ifdef GBUFFER
   GBUFINT gbufidx;
   invalx( xb );
   invalx( xe );
   invaly( yb );
   gbufidx = GINDEX(xb,(yb/8));
   #else
   ghw_set_xypos(xb,yb);
   #endif

   msk = pixymsk[yb & 0x7];
   if( !pixel )
      msk = ~msk;

   for (x = xb; x <= xe; x++ )
      {
      #ifdef GBUFFER
      if( pixel )
        gbuf[gbufidx++] |= msk;
      else
        gbuf[gbufidx++] &= msk;

      #else

      /* Update value */
      if( pixel )
          ghw_auto_wr((SGUCHAR)(msk | ghw_rd()));
      else
          ghw_auto_wr((SGUCHAR)(msk & ghw_rd()));
      #endif
      }
   #ifdef GBUFFER
   if (ghw_upddelay == 0)
      ghw_updatehw();
   #endif
   }

/*
   Provides accelerated line drawing for horizontal/vertical lines.

   If left-top and right-bottom is on a single vertical or horizontal
   line a single line is drawn.

   All coordinates are absolute coordinates.
*/
void ghw_rectangle(GXT ltx, GYT lty, GXT rbx, GYT rby, SGBOOL pixel)
   {
   glcd_err = 0;

   /* Force resonable values */
   GLIMITU(ltx,GDISPW-1);
   GLIMITU(lty,GDISPH-1);
   GLIMITD(rby,lty);
   GLIMITU(rby,GDISPH-1);
   GLIMITD(rbx,ltx);
   GLIMITU(rbx,GDISPW-1);
   GBUF_CHECK();

   if (ltx != rbx)
      ghw_lineh(ltx, lty, rbx, pixel );      /* Draw horisontal line */

   if (lty != rby)
      {
      ghw_linev(ltx, lty, rby, pixel );      /* Draw vertical line */
      if (ltx != rbx)
         {                                   /* It is box coordinates */
         ghw_lineh(ltx, rby, rbx, pixel );   /* Draw bottom horizontal line */
         ghw_linev(rbx, lty, rby, pixel );   /* Draw right vertical line */
         }
      }
   #if (defined(GHW_PCSIM) && !defined(GHW_FAST_SIM_UPDATE))
   GSimFlush();
   #endif
   }
#endif /* GGRAPHICS */

