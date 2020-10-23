/************************** ghwsymwr.c *****************************

   Graphic symbol write functions for LCD display

   Write graphic symbol buffer using the general symbol format
   to LCD display.

   The byte ordering for a symbol is horizontal byte(s) containing the
   first pixel row at the lowest address followed by the byte(s) in
   the pixel row below etc. The symbol is left aligned in the byte buffer.

   All coordinates are absolute pixel coordinate.

   ---------

   The KS07XX controller is assumed to be used with a LCD module.

   The following LCD module characteristics MUST be correctly
   defined in GDISPCFG.H:

      GDISPW  Display width in pixels
      GDISPH  Display height in pixels
      GBUFFER If defined most of the functions operates on
              a memory buffer instead of the LCD hardware.
              The memory buffer content is complied to the LCD
              display with ghw_updatehw().
              (Equal to an implementation of delayed write)


   Creation date:
   Revision date:     041002
   Revision Purpose:  Made more generic by removing use of sg_union16
   Revision date:     120403
   Revision Purpose:  Support for PGENERIC and intrinsic generic pointers
   Revision date:     221203
   Revision Purpose:  Read and write modules splitted in two files
                      to optimeze memory consumption in most systems
   Revision date:     170804
   Revision Purpose:  Modified to use ghw_set_xy(..)
   Revision date:     300108
   Revision Purpose:  Support for transperant mode added.
   Revision date:     18-02-08
   Revision Purpose:  Support for vfonts added.

   Version number: 2.2
   Copyright (c) RAMTEX Engineering Aps 2000-2008

*********************************************************************/
/* <stdlib.h> is included via gdisphw.h */
#include "gdisphw.h"   /* HW driver prototypes and types */
#include "ks07xx.h"    /* LCD controller specific definements */

#ifdef GVIRTUAL_FONTS
#include <gvfont.h>
#endif

#ifdef GSOFT_SYMBOLS

/*
   Copy a graphic area from a buffer using the common symbol and font format
   to the LCD memory or the graphic buffer
*/
void ghw_wrsym(GXT ltx, GYT lty, GXT rbx, GYT rby, PGSYMBYTE src, GXT bw, SGUCHAR mode)
   {
   GYT ys;
   SGUCHAR GFAST msk;
   SGBOOL transperant;

   #ifdef GBUFFER
   GBUFINT gbufidx;
   GBUF_CHECK();
   #endif

   #ifdef GVIRTUAL_FONTS
   if (bw == 0)
      return;
   #else
   if ((src == NULL) || (bw == 0))
      return;
   #endif

   /* Force reasonable values */
   GLIMITU(ltx,GDISPW-1);
   GLIMITU(lty,GDISPH-1);
   GLIMITD(rby,lty);
   GLIMITU(rby,GDISPH-1);
   GLIMITD(rbx,ltx);
   GLIMITU(rbx,GDISPW-1);
   GLIMITD(bw,1);

   #ifdef GBUFFER
   invalrect( ltx, lty );
   invalrect( rbx, rby );
   #endif

   transperant = (mode & GHW_TRANSPERANT) ? 1 : 0;
   mode &= GHW_INVERSE; /* Turn mode into an inverse flag */
   msk = startmask[lty & 0x7];

   for (ys=0; lty<=rby; lty=(lty&~(0x7))+GDISPCH)
      {
      /* Vertical screen byte loop */
      GXT x,xs;
      GYT y;
      if ((lty & ~0x7) == (rby & ~0x7))
         msk &= stopmask[rby & 0x7];  /* Use stop mask on last row */

      for (x = 0, y = 0, xs=ltx; xs <= rbx; x++)
         {
         /* Horizontal screen column loop */
         GXT xc;
         SGUCHAR GFAST xmsk;

         #ifdef GBUFFER

         gbufidx = GINDEX(xs, (lty/8));

         #else

         SGUCHAR tidx;
         if ((msk != 0xff) || transperant)
            {
            /* Read data to be modified to tmp buffer to increase speed */
            ghw_set_xypos(xs,(GYT)(lty & ~(0x7)));
            ghw_auto_rd_start();
            if ((xc = rbx-xs) > 7) xc = 7;
            tidx = 0;
            do
               {
               ghw_tmpb[tidx++] = ghw_auto_rd();
               }
            while (xc-- != 0);
            }
         tidx = 0;

         #endif

         xmsk = 0x80;
         if ((xc = rbx-xs) > 7) xc = 7;
         do
            {
            /* Loop horizontal symbol byte rows while updating buffer */
            SGUCHAR GFAST dat;
            SGUCHAR GFAST ymsk;
            GYT GFAST yc;
            ymsk = pixymsk[lty & 0x7];
            dat = 0;
            y = ys;
            yc = lty;
            /* Scan and create vertical screen bytes */
            do
               {
               #ifdef GVIRTUAL_FONTS
               register GBUFINT idx;
               register SGUCHAR sdat;
               idx = ((GBUFINT)x)+((GBUFINT)y)*bw;
               if (src == NULL)
                   sdat = gi_symv_by(idx); /* Load symbol byte from virtual memory */
               else
                  sdat = src[idx];        /* Load symbol byte from normal memory */
               if ((sdat & xmsk) != 0)
                  dat |= ymsk;
               #else
               if ((src[((GBUFINT)x)+((GBUFINT)y)*bw] & xmsk) != 0)
                  dat |= ymsk;
               #endif
               y++;
               }
            while((((ymsk <<= 1) & 0xff) != 0) && (++yc <= rby));

            #ifdef GBUFFER
            if (transperant)
               {
               if (mode)
                  gbuf[gbufidx] &= ((dat ^ 0xff) | ~msk); /* Using white on transperant */
               else
                  gbuf[gbufidx] |= (dat & msk); /* Using black on transperant */
               }
            else
               {
               if (mode)
                  dat ^= 0xff;  /* Using white on black */
               if (msk != 0xff)
                  dat = (gbuf[gbufidx] & ~msk) | (dat & msk);
               gbuf[gbufidx] = dat;
               }
            gbufidx++;
            #else
            if (transperant)
               {
               if (mode)
                  ghw_tmpb[tidx] &= ((dat ^ 0xff) | ~msk); /* Using white on transperant */
               else
                  ghw_tmpb[tidx] |= (dat & msk); /* Using black on transperant */
               }
            else
               {
               if (mode)
                  dat ^= 0xff;  /* Using white on black */
               if (msk != 0xff)
                  dat = (ghw_tmpb[tidx] & ~msk) | (dat & msk);
               ghw_tmpb[tidx] = dat;
               }
            tidx++;
            #endif
            xmsk >>= 1;
            }
         while(xc-- != 0);

         #ifdef GBUFFER

         /* Update x position */
         if ((xc = rbx-xs) > 7)
            xs += 8;
         else
            xs += xc+1;

         #else

         /* Write tmp buffer data  to LCD memory or buffer */
         ghw_set_xypos(xs,(GYT)(lty & ~(0x7)));

         if ((xc = rbx-xs) > 7) xc = 7;
         tidx = 0;
         do
            {
            ghw_auto_wr( ghw_tmpb[tidx++] );
            xs++;
            }
         while (xc-- != 0);
         #endif
         }
      ys = y;
      msk = 0xff;
      }
   #if (defined(GHW_PCSIM) && !defined(GHW_FAST_SIM_UPDATE))
   GSimFlush();
   #endif
   }

#endif

