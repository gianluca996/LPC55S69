/************************** ghwsymrd.c *****************************

   Graphic symbol read functions for LCD display

   Read graphic area from the display to a GLCD buffer using the general
   symbol format.

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
   Revision date:     240206
   Revision Purpose:  SGUCHAR changed to GXT

   Version number: 1.21
   Copyright (c) RAMTEX Engineering Aps 2000-2006

*********************************************************************/
/* <stdlib.h> is included via gdisphw.h */
#include "gdisphw.h"   /* HW driver prototypes and types */
#include "ks07xx.h"    /* LCD controller specific definements */

#ifdef GSOFT_SYMBOLS

/*
   Copy a graphic area from the display to a buffer organized with the
   common symbol and font format.
*/
void ghw_rdsym(GXT ltx, GYT lty, GXT rbx, GYT rby, PGUCHAR dest, GXT bw, SGCHAR mode)
   {
   GYT y,ys,yc;
   GXT x,xs;
   SGUCHAR xmsk,ymsk,dat,tidx;
   #ifdef GBUFFER
   GBUFINT gbufidx;
   GBUF_CHECK();
   #endif

   if (dest == NULL)
      return;

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
   if (mode != 0)
      mode = (SGCHAR)((SGUCHAR)0xff);  /* Display is using inverse mode */

   for (ys=0,y=0; lty<=rby; lty=(lty&~(0x7))+GDISPCH)
      {
      /* Vertical screen byte loop */
      #ifdef GBUFFER
      gbufidx = GINDEX(ltx,(lty/8));
      #else
      ghw_set_xypos(ltx,(GYT)(lty & ~(0x7)));
      ghw_auto_rd_start();
      #endif
      x = 0;
      for (tidx = 0, xs=ltx; xs <= rbx; xs++)
         {
         /* Horizontal screen column loop */
         #ifdef GBUFFER
         ghw_tmpb[tidx++] = gbuf[gbufidx++] ^ mode;
         #else
         ghw_tmpb[tidx++] = ghw_auto_rd() ^ mode;
         #endif
         if (((tidx & 0x7) == 0) || (xs == rbx))
            {
            ymsk = pixymsk[lty & 0x7];
            y = ys;
            yc = lty;
            /* Loop vertical symbol byte row */
            do
               {
               GXT xc = xs;
               tidx = dat = 0;
               xmsk = 0x80;
               /* Scan and create horizontal byte */
               do
                  {
                  if ((ghw_tmpb[tidx++] & ymsk) != 0)
                     dat |= xmsk;
                  }
               while(((xmsk >>= 1) != 0) && (xc <= rbx));
               dest[(GBUFINT)x+(GBUFINT)y*bw] = dat;
               y++;
               }
            while((((ymsk <<= 1) & 0xff) != 0) && (++yc <= rby));
            tidx = 0;
            x++;
            }
         }
      ys = y;
      }
   }

#endif

