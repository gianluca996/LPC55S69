/************************** ghwpixel.c *****************************

   Low-level functions for graphic pixel set and clear
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

   Creation date:
   Revision date:     170804
   Revision Purpose:  Modified to use ghw_set_xy(..)
   Revision date:
   Revision Purpose:

   Version number: 1.0
   Copyright (c) RAMTEX Engineering Aps 2002

*********************************************************************/

#include "gdisphw.h"   /* HW driver prototypes and types */
#include "ks07xx.h"    /* KS07XX controller specific definements */

#ifdef GGRAPHICS

/*
   Set pixel  == 0 = white, != 0 = black
*/
void ghw_setpixel(GXT x, GYT y, SGBOOL pixel )
   {
   SGUCHAR msk;

   glcd_err = 0;

   /* Force resonable values */
   GLIMITU(y,GDISPH-1);
   GLIMITU(x,GDISPW-1);

   /* Calculate byte index */
   msk = pixymsk[y & 0x7];

   #ifdef GBUFFER
   GBUF_CHECK();

   if( pixel != 0 )
      gbuf[GINDEX(x,(y/8))] |= msk;
   else
      gbuf[GINDEX(x,(y/8))] &= ~msk;

   invalrect( x, y );

   #else

   ghw_set_xypos(x,y);  /* Initiate LCD controller address pointers*/
   if (pixel != 0)   /* Read data */
      msk |= ghw_rd();
   else
      msk = ~msk & ghw_rd();

   ghw_wr( msk );
   #endif
   }

/*
   Get pixel  == 0 = white, != 0 = black
*/
SGBOOL ghw_getpixel(GXT x, GYT y)
   {
   SGUCHAR msk;

   glcd_err = 0;

   /* Force resonable values */
   GLIMITU(y,GDISPH-1);
   GLIMITU(x,GDISPW-1);

   /* Calculate byte index */
   msk = pixymsk[y & 0x7];

   #ifdef GBUFFER

   #ifdef GHW_ALLOCATE_BUF
   if (gbuf == NULL)
      {
      glcd_err = 1;
      return 0;
      }
   #endif

   msk &= gbuf[GINDEX(x,(y/8))];

   #else

   ghw_set_xypos(x,y);  /* Initiate LCD controller address pointers*/
   msk &= ghw_rd();   /* Perform operation */

   #endif

   return (msk == 0) ? 0 : 1;
   }
#endif
