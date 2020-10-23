/************************** ghwgscrl.c *****************************

   Scrolls the graphics on LCD x lines up.  The empty area in the
   bottom is cleared with a pattern. The fill pattern will be aligned
   to the background independent of the used coordinate parameters.

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


   Creation date:
   Revision date:    30-03-02
   Revision Purpose: x5 speed increase by use of the temporary buffer.
   Revision date:    17-08-03
   Revision Purpose: Scroll shift direction is now calculated correct
                     for all y positions
   Revision date:     170804
   Revision Purpose:  Modified to use ghw_set_xy(..)

   Version number: 1.2
   Copyright (c) RAMTEX Engineering Aps 2003

*********************************************************************/
/* <stdlib.h> is included via gdisphw.h */
#include "gdisphw.h"   /* HW driver prototypes and types */
#include "ks07xx.h"    /* KS07XX controller specific definements */

#if defined( GBASIC_TEXT ) || defined(GSOFT_FONTS) || defined(GGRAPHIC)

/*
   Scrolls the graphics on LCD x lines up.
   The empty area in the bottom is cleared

   lines  =  pixel lines to scroll
*/
void ghw_gscroll(GXT ltx, GYT lty, GXT rbx, GYT rby, GYT lines, SGUINT pattern)
   {
   GYT GFAST y,ys,ylim;
   GXT GFAST x,xc,xcb;
   SGUCHAR GFAST msk,mske,msklim,tidx;
   SGINT shift;
   SGUINT dat;
   #ifdef GBUFFER
   SGUCHAR *p;
   GBUF_CHECK();
   #endif

   glcd_err = 0;

   /* Force resoanable values */
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

   if (lines > rby - lty)
      {
      ghw_fill(ltx, lty, rbx, rby,pattern);   /* clear whole area */
      return;
      }

   /* Prepare patterns and masks */
   ylim = rby - lines;
   msk = startmask[lty & 0x7];
   mske  = stopmask[rby & 0x7];
   msklim = stopmask[ylim & 0x7];

   /* Set shift value (negative = shift left) */
   shift = (SGINT)(((SGUINT)lty + lines) & 0x7) - (SGINT)(lty & 0x7);

   ys = (lty+lines) / GDISPCH; /* First source row for scroll */
   rby /= GDISPCH;
   lty /= GDISPCH;
   ylim /= GDISPCH;


   /* Loop byte rows */
   for (y = lty; y <= rby; y++,ys++)
      {
      if (y == rby)
         msk &= mske;  /* Use stop mask */

      for (x = ltx; x <= rbx; )
         {
         /* Loop colums.
           Operations is done 8 bytes at a time via the ghw_tmp
           buffer, in order to take advantage of the LCD
           controllers auto increment functionality.
           This give a factor 5 speed increase compared to
           an implementation which complete one column at a time */
         if ((xcb = rbx-x) > 7) xcb = 7;
         #ifdef GBUFFER
         p = &gbuf[GINDEX(x,ys)];
         #else
         /* Read data to temp buffers */
         ghw_set_xypos(x,(GYT)(ys*GDISPCH));
         ghw_auto_rd_start();
         #endif
         xc = xcb;
         tidx = 0;
         do
            {
            #ifdef GBUFFER
            ghw_tmpb[tidx++] = *p++;
            #else
            ghw_tmpb[tidx++] = ghw_auto_rd();
            #endif
            }
         while (xc-- != 0);

         if (((shift > 0) && (ys != rby)) ||
             ((shift < 0) && (ys != lty)))
            {
            #ifdef GBUFFER
            p = &gbuf[GINDEX(x,((shift > 0) ? ((GBUFINT)ys+1) : ((GBUFINT)ys-1)))];
            #else
            ghw_set_xypos(x,(GYT)(((shift > 0) ? ((GBUFINT)ys+1) : ((GBUFINT)ys-1))*GDISPCH));
            ghw_auto_rd_start();
            #endif
            xc = xcb;
            tidx = 0;
            do
               {
               #ifdef GBUFFER
               ghw_tmpb2[tidx++] = *p++;
               #else
               ghw_tmpb2[tidx++] = ghw_auto_rd();
               #endif
               }
            while (xc-- != 0);
            }

         xc = xcb;
         tidx = 0;
         do
            {
            /* Get source data */
            if (y <= ylim)
               {
               if (shift != 0)
                  {
                  if (shift > 0)
                     {
                     dat = (SGUINT) ghw_tmpb[tidx];
                     if ((ys != rby))
                        dat |= ((SGUINT) ghw_tmpb2[tidx])*256;
                     dat >>= shift;
                     }
                  else
                     {
                     dat = (y == lty) ? 0 : (SGUINT) ghw_tmpb2[tidx];
                     dat |= ((SGUINT) ghw_tmpb[tidx]) *256;
                     dat <<= abs(shift);
                     dat /= 256;
                     }
                  }
               else
                  dat = (SGUINT) ghw_tmpb[tidx];

               if (y == ylim)
                  ghw_tmpb[tidx] = (((SGUCHAR) dat) & msklim) | (((SGUCHAR)pattern) & ~msklim);
               else
                  ghw_tmpb[tidx] = ((SGUCHAR) dat );
               }
            else
               ghw_tmpb[tidx] = (SGUCHAR) pattern; /* use pattern */
            tidx++;
            }
         while(xc-- != 0);

         if (msk != 0xff)
            {
            /* Fetch byte and mask */
            #ifdef GBUFFER
            p = &gbuf[GINDEX(x,y)];
            #else
            ghw_set_xypos(x,(GYT)(y*GDISPCH));
            ghw_auto_rd_start();
            #endif
            xc = xcb;
            tidx = 0;
            do
               {
               #ifdef GBUFFER
               ghw_tmpb[tidx] = (*p & ~msk) | (ghw_tmpb[tidx] & msk);
               p++;
               #else
               ghw_tmpb[tidx] = (ghw_auto_rd() & ~msk) | (ghw_tmpb[tidx] & msk);
               #endif
               tidx++;
               }
            while(xc-- != 0);
            }

         #ifdef GBUFFER
         p = &gbuf[GINDEX(x,y)];
         #else
         ghw_set_xypos(x,(GYT)(y*GDISPCH));
         #endif
         xc = xcb;
         tidx = 0;
         do
            {
            #ifdef GBUFFER
            *p++ = ghw_tmpb[tidx++];
            #else
            ghw_auto_wr(ghw_tmpb[tidx++]);
            #endif
            }
         while(xc-- != 0);
         x += xcb+1;
         }
      msk = 0xff;
      }
   #if (defined(GHW_PCSIM) && !defined(GHW_FAST_SIM_UPDATE))
   GSimFlush();
   #endif
   }
#endif /* GBASIC_TEXT */


