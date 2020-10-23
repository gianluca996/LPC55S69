/************************* gsympixrd.c ****************************

   Internal functions for read of individual symbol pixel data.

   To provide increased reading speed the symbol is "opened" in
   advance by a call to getsym_open(..) before the getsym_pixel(..)
   function is used to fetch the data.

   This is a generic, random location, pixel-by-pixel fetch in contrast
   to the faster, but target controller specific, storage unit streaming
   used during symbol write via the low-level drivers.
   Also support use of symbol data stored in virtual fonts.

   Creation date:  13-11-2012

   Revision Date:    27-01-2013
   Revision Purpose: GSYMCLR mode corrected

   Version number: 1.1
   Copyright (c) RAMTEX Engineering Aps 2012-2013

*********************************************************************/

#include "sgtypes.h"
#include "gfixedpt.h"
#include "gi_disp.h"

#ifdef GSOFT_SYMBOLS
#ifdef GVIRTUAL_FONTS
#include <gvfont.h>
#endif

#define ALPHABITS 4
#define ALPHAMAX  ((1<<ALPHABITS)-1)

/*
   Fetch symbol pixel
       if coordinate is not at exact pixel grid then use a
       weighted blend of 4 neighbour pixels to create the new logical
       pixel color
*/
SGUCHAR gi_getsym_pixel(GSYM_PROCESS *sp)
   {
   SGFIXP x,y;
   SGUINT color,alpha;
   SGUCHAR yidx, xidx, dy, calpha, acnt;
   GBUFINT sidx;
   register SGINT sx;
   register SGUCHAR by;

   /* Weighted blend of 4 pixels to create the averange pixel color */
   color = 0;
   alpha = 0;
   acnt = 0;
   for(yidx = 0, y = sp->yo; (yidx < 2) && (y < sp->symh); yidx++, y+=SGU_TO_FIXP(1))
      {
      if (y < 0)
         continue;
      if ((dy = (SGUCHAR)(((yidx == 0) ? ~y : y) >> (SGFIXP_BITS-ALPHABITS))&ALPHAMAX) == 0)
         continue;
      for(xidx = 0, x = sp->xo; (xidx < 2) && (x < sp->symw); xidx++, x+=SGU_TO_FIXP(1))
         {
         if (x < 0)
            continue;
         calpha = ((SGUINT)(((xidx == 0) ? ~x : x) >> (SGFIXP_BITS-ALPHABITS))&ALPHAMAX)*dy;
         if (calpha != 0)
            {
            /* Pixel is used */
            if (sp->fill)
               {
               sp->c = 0;
               sp->a = 1;
               return 0; /* Pixel clear used, no blending needed */
               }
            acnt++;
            /* Get pixel data */
            sx = SGFIXP_TO_U(x);
            sidx = (GBUFINT)(sx>>3) + (GBUFINT)sp->symbw*(SGFIXP_TO_U(y));
            #ifdef GVIRTUAL_FONTS
            if (sp->psymdat== NULL)
               by = gi_symv_by(sidx) ^ sp->invert;
            else
            #endif
               by = sp->psymdat[sidx] ^ sp->invert;
            if ((by = ((by >> (7 - (sx & 7))) & 0x1))!=0)
               color += calpha;   /* Accumulate 1 pixels */
            if (!(sp->transperant && (by == 0)))
               alpha += calpha;   /* Accumulate transperancy */
            }
         }
      }

   if (acnt == 0)
      {
      sp->a = 0;
      return 1;  /* Completely outside */
      }

   /* Set color and alpha with 50% rounding */
   sp->c = (color > (ALPHAMAX*ALPHAMAX)/2) ? 1 : 0;
   sp->a = (alpha > (ALPHAMAX*ALPHAMAX)/2) ? 1 : 0;
   return 0;
   }

/*
   void gi_getsym_open( PGSYMBOL psymbol );

   Init GSYM_PROCESS structure for pixel fetch / pixel blending.
   with gi_getsym_pixel(..).

   Includes support for pixel read of font symbols in virtual memory.
*/
SGUCHAR gi_getsym_open( GSYM_PROCESS *sp, PGSYMBOL psymbol, SGUINT symbytewidth, GMODE symflag)
   {
   if (sp == NULL)
      return 1;

   #ifdef GVIRTUAL_FONTS_DYN
   if (gvfsym_open(psymbol))
   #else
   if (psymbol == NULL)
   #endif
      return 1;

   G_POBJ_CLR(sp,GSYM_PROCESS);  /* Fast reset of structure elements to 0 */

   /* Set pixel processing flags */
   if (symflag & GINVERSE)
      sp->invert = 0xff;
   if (symflag & GSYMCLR)
      sp->fill = 0x1;   /* Fill takes precedene over blend and transperance */
   else
      {
      if (symflag & GTRANSPERANT)
         sp->transperant = 0x1;
      if ((symflag & GNOCOLORBLEND)==0)
         sp->blend = 0x1;
      }

   /* Set symbol info */
   sp->symbw = (symbytewidth != 0) ? symbytewidth : (gsymw(psymbol)+7)/8; /* width of symbol in bytes */
   sp->symw = SGU_TO_FIXP(gsymw(psymbol)); /* fixed point width, height (for simple comparation) */
   sp->symh = SGU_TO_FIXP(gsymh(psymbol));

   #ifdef GVIRTUAL_FONTS
   if (gissymbolv(psymbol))
      gi_symv_open( psymbol, sp->symbw, 0 ); /* Preset virtual symbol interface */
   else
   #endif
      sp->psymdat = (PGSYMBYTE) (&(((PGBWSYMBOL)psymbol)->b[0]));
   return 0;
   }

#endif /* GSOFT_SYMBOLS */
