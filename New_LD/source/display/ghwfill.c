/************************** ghwfill.c *****************************

   Fill box area with a pattern.

   The box area may have any pixel boundary, however the pattern is
   always aligned to the physical background, which makes patching
   of the background easier with when using multiple partial fills.

   The pattern word is used as a 2 character pattern.
   The LSB byte of pattern are used on even pixel lines and the MSB byte
   are used on odd pixel lines making it easy to make a "grey" bit raster
   (for instance when pat = 0x55aa or = 0xaa55)

   ---------

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
   Creation date:
   Revision date:
   Revision Purpose:

   Version number: 1.0
   Copyright (c) RAMTEX Engineering Aps 2002

*********************************************************************/
#include "gdisphw.h"   /* HW driver prototypes and types */
#include "ks07xx.h"    /* KS07XX controller specific definements */

#ifdef GVIEWPORT

/*
   Prepare ghw_tmpb buffer with a fill pattern
*/
static void ghw_set_pattern(SGUINT pattern)
   {
   GYT y;
   /* Use ghw_tmpb buffer to hold fill pattern */
   if ((pattern == 0x0) || (pattern == 0xffff))
      {
      /* Pixel symetric pattern, just make a fast store */
      for (y=0; y<sizeof(ghw_tmpb);)
         {
         ghw_tmpb[y++] = (SGUCHAR)(pattern & 0xff);
         ghw_tmpb[y++] = (SGUCHAR)(pattern / 256);
         }
      }
   else
      { /* Store as horizontal bytes*/
      for (y=0; y<sizeof(ghw_tmpb);)
         {
         ghw_buf_xwr(ghw_tmpb,y,(SGUCHAR)(pattern & 0xff));
         y++;
         ghw_buf_xwr(ghw_tmpb,y,(SGUCHAR)(pattern / 256));
         y++;
         }
      }
   }

void ghw_fill(GXT ltx, GYT lty, GXT rbx, GYT rby, SGUINT pattern)
   {
   GYT y;
   GXT GFAST x;
   SGUCHAR GFAST msk, mske, dat;
   #ifdef GBUFFER
   GBUFINT gbufidx;
   GBUF_CHECK();
   #endif

   glcd_err = 0;

   /* Force reasonable values */
   GLIMITU(ltx,GDISPW-1);
   GLIMITU(lty,GDISPH-1);
   GLIMITD(rby,lty);
   GLIMITU(rby,GDISPH-1);
   GLIMITD(rbx,ltx);
   GLIMITU(rbx,GDISPW-1);

   #ifdef GBUFFER
   invalrect( ltx, lty );
   invalrect( rbx, rby );
   #endif

   /* Prepare patterns and masks */
   ghw_set_pattern(pattern);
   msk = startmask[lty & 0x7];
   mske  = stopmask[rby & 0x7];
   rby /= GDISPCH;
   lty /= GDISPCH;

   /* Loop byte rows */
   for (y = lty; y <= rby; y++)
      {
      if (y == rby)
         msk &= mske;  /* Use stop mask on last row */

      #ifdef GBUFFER
      gbufidx = GINDEX(ltx,y);
      #else
      ghw_set_xypos(ltx,(GYT)(y*GDISPCH));
      #endif
      /* Loop bit collums */
      for (x = ltx; x <= rbx; x++ )
         {
         dat = ghw_tmpb[x & 0x7];
         #ifdef GBUFFER
         if (msk != 0xff)
            dat = (gbuf[gbufidx] & ~msk) | (dat & msk);
         gbuf[gbufidx++] = dat;
         #else
         if (msk != 0xff)
            dat = (ghw_rd() & ~msk) | (dat & msk);
         ghw_auto_wr(dat);
         #endif
         }
      msk = 0xff;
      }
   #if (defined(GHW_PCSIM) && !defined(GHW_FAST_SIM_UPDATE))
   GSimFlush();
   #endif
   }

#endif /* GBASIC_TEXT */

