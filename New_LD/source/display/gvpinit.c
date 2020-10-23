/************************* gvpinit.c *******************************

   Creation date: 980220

   Revision date:     02-01-23
   Revision Purpose:  symsize parameter added to gi_putsymbol(..)
   Revision date:     02-01-27
   Revision Purpose:  Support for soft cursors change.
   Revision date:     03-01-26
   Revision Purpose:  Bit mask on GINVERSE mode
   Revision date:     03-06-06
   Revision Purpose:  Color support added
   Revision date:     05-02-08
   Revision Purpose:  Extra character and line spacing control added
   Revision date:     13-06-08
   Revision Purpose:  Viewport reset code made an internal function and
                      shared with gresetvp() gresetposvp()
   Revision date:     01-07-10
   Revision Purpose:  gcurvpnum introduced to optimize viewport handling speed.
   Revision date:    14-11-12
   Revision Purpose:  Named vitural font support added

   Version number: 2.3
   Copyright (c) RAMTEX Engineering Aps 1998-2012

*********************************************************************/

#include "gi_disp.h" /* gLCD prototypes */

#ifdef GVIEWPORT

GCOMMON_DATA gdata;  /* Collection of data structures used by the library */
GVP GFAST *gcurvp;   /* pointer to current view-port struct */

#ifdef GVIRTUAL_FONTS
 #include <gvfont.h>
#endif

/*********************************************************************
   Segment: Viewport
   Level: Viewport
   Checks the vp struct for erroneous values, maybe some faulty part
   of the main code overwrites the internal data of the LCD driver.
   This function catch such errors.
   If GDATACHECK is defined many LCD functions calls this.
   Returns 1 if no error.
*/
#ifdef GDATACHECK

void gi_datacheck(void)
   {
   SGUCHAR c;
   /* Check pointers */
   if ( gcurvp == NULL )
      {
      G_WARNING( "GLCD was not initialized, Initialization will be done" );
      ginit();
      }
   if (GDATA_CURVPNUM >= GNUMVP)
      {
      G_WARNING("Current viewport number >= GNUMVP definition, Defaults to GNUMVP-1");
      GDATA_CURVPNUM = GNUMVP-1;
      gcurvp = &GDATA_VIEWPORTS[ GDATA_CURVPNUM ];
      }
   if (gcurvp->pfont == NULL)
      {
      G_WARNING( "GLCD was not initialized, Initialization will be done" );
      ginit();
      if (gcurvp == NULL)
         return; /* (Re-) initialization failed */
      }

   #ifdef GHW_NO_HDW_FONT
   if (gishwfont())
      {
      G_WARNING( "This display controller does not support hardware fonts" );
      }
   #endif


   /* check VP for illegal values */
   if( gcurvp->lt.x >= GDISPW ||
       gcurvp->lt.y >= GDISPH ||
       gcurvp->rb.x >= GDISPW ||
       gcurvp->rb.y >= GDISPH ||
       gcurvp->cpos.x < gcurvp->lt.x ||
       gcurvp->cpos.y < gcurvp->lt.y ||
       ((SGUINT) gcurvp->cpos.x > ((SGUINT)gcurvp->rb.x+1)) || /* cpos.x = rb.x+1 is ok, used internally as put stop condition */
       gcurvp->cpos.y > gcurvp->rb.y ||
       #ifndef GNOTXTSPACE
       (gcurvp->rb.x - gcurvp->lt.x) < gcurvp->chln.x ||
       (gcurvp->rb.y - gcurvp->lt.y) < gcurvp->chln.y ||
       #endif
       /*
       Its legal to have ppos outside view-port and LCD
       */
       (!(gcurvp->mode < GMODELAST)) )
       {
       G_WARNING( "Coordinate range error or missing initialization" );
       }

   c = gcurvp->gi_check;
   gi_calcdatacheck();
   if( c != gcurvp->gi_check)
      {
      c = c;           /* for easy set of break point */
      G_WARNING( "Internal GLCD data error, check sum" );
      }
   }

/*********************************************************************
   Segment: Viewport
   Level: Viewport
   Calculates the current vp struct check value.
   Returns the new check value.
*/
void gi_calcdatacheck(void)
   {
   SGUCHAR i,s;
   s = (SGUCHAR)((GVP*)&(gcurvp->gi_check) - gcurvp);
   gcurvp->gi_check = 0;
   for( i=0; i<s; i++)
      {
      gcurvp->gi_check += ((SGUCHAR*)gcurvp)[i];
      }
   }


#ifndef GNOTXTSPACE
void gi_limit_check(void)
   {
   GXT w;
   GYT h;
   #ifndef GHW_NO_HDW_FONT
   if( gishwfont() )
      return;
   #endif
   h = gcurvp->fsize.y;
   w = gcurvp->fsize.x;

   /* Limit to viewport size */
   if (w >= (gcurvp->rb.x-gcurvp->lt.x)+1)
      w = (gcurvp->rb.x-gcurvp->lt.x)+1;
   if (h >= (gcurvp->rb.y-gcurvp->lt.y)+1)
      w =  (gcurvp->rb.y-gcurvp->lt.y)+1;

   /* Limit to font size */
   if (gcurvp->chln.x >= w)
      {
      G_WARNING( "Char spacing exceeds font or viewport width, limiting char spacing" );
      gcurvp->chln.x = w-1;
      }
   if (gcurvp->chln.y >= h)
      {
      G_WARNING( "Line spacing exceeds font or viewport height, limiting line spacing" );
      gcurvp->chln.y = h-1;
      }
   }
#endif /* GNOTXTSPACE */

#endif /* GDATACHECK */

/*
   Internal function clears current viewport
   Resettype = 0: Clear all
   Resettype = 1: Clear all, except color setting (gresetvp)
   Resettype = 2: Clear all, except color and font data setting (gresetposvp)
*/
void gi_resetvp(SGUCHAR resettype)
   {
   /* Set full screen coordiantes */
   gcurvp->lt.x = 0;
   gcurvp->lt.y = 0;
   gcurvp->rb.x = GDISPW-1;
   gcurvp->rb.y = GDISPH-1;

   #ifdef GGRAPHICS
   /* reset pixel pos */
   gcurvp->ppos.x = 0;
   gcurvp->ppos.y = 0;
   #endif /* GGRAPHICS */

   gcurvp->cpos.x = 0;
   if (resettype <= 1)
      {
      /*gcurvp->cpos.y = SYSFONT.symheight-1;*/
      gcurvp->cpos.y = 8-1; /* default for SYSFONT */

      /* reset mode */
      #ifdef GWORDWRAP
      /* Make compatible with old versions where GWORDWRAP was defined in gdispcfg.h */
      gcurvp->mode = GWORD_WRAP;
      #else
      gcurvp->mode = GNORMAL;
      #endif

      /* reset font */
      gcurvp->pfont = &SYSFONT;
      /* preset code-page */
      gcurvp->codepagep = SYSFONT.pcodepage;
      gcurvp->fsize.x = SYSFONT.symwidth;
      gcurvp->fsize.y = SYSFONT.symheight;

      #ifndef GNOTXTSPACE
      /* default is no extra character & line spacing */
      gcurvp->chln.x = 0;
      gcurvp->chln.y = 0;
      #endif
      #ifdef GHW_USING_COLOR
      if (resettype == 0)
         {
         gcurvp->foreground = GHW_PALETTE_FOREGROUND;
         gcurvp->background = GHW_PALETTE_BACKGROUND;
         }
      #endif
      }
   else
      {
      gcurvp->cpos.y = ((gcurvp->pfont == NULL) ? SYSFONT.symheight : gcurvp->fsize.y)-1;
      #ifndef GNOTXTSPACE
      gi_limit_check();   /* Limit spacing */
      #endif
      }
   gi_calcdatacheck();    /* correct VP checksum to new settings */
   }

/*
   Init high-level GCOMMON_DATA part
*/
void gi_common_init(void)
   {
   SGUCHAR i;

   #ifndef GNOCURSOR
   /* set cursor */
   if ( ghw_err() == 0)
      {
      ghw_setcursor( GCURSIZE2 );
      }
   #endif /* GNOCURSOR */

   /* Reset vp structures */
   for( i=0; i<GNUMVP; i++ )
      {
      gcurvp = &GDATA_VIEWPORTS[i];
      gi_resetvp(0);
      }

   /* select current vp = 0 */
   GDATA_CURVPNUM = 0;
   gcurvp = &GDATA_VIEWPORTS[0];

   #ifndef GCONSTTAB
   /* Clear variable tabulator table */
   for( i=0; i<GMAXTABS; i++ )
      GDATA_TABS[i] =  0;
   #endif

   #ifdef GWIDECHAR
   GDATA_STRTYPE = 0;
   #endif

   #ifndef GNOCURSOR
   GDATA_CURSORON = 0;
   #endif
   }

/*
    Library and low-level init
*/
SGUCHAR ginit(void)
   {
   SGUCHAR err;
   if ((err = ghw_init()))   /* init hw */
      {
      G_WARNING("ginit()->ghw_init() : Hardware initialization failure ");
      }
   else
      gi_common_init();

   #ifdef GVIRTUAL_FONTS_DYN
   /* Clear virtual font load structures */
   gvf_init();
   #endif
   return err;          /* Return != 0 if some error has occured */
   }

#endif /* GVIEWPORT */


