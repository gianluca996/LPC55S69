/************************* gfsel.c *********************************

   Creation date: 980223

   Revision date:     02-01-23
   Revision Purpose:  Support for soft cursors change.
   Revision date:     14-04-03
   Revision Purpose:  Support for extended codepage.
   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added
   Revision date:     10-02-05
   Revision Purpose:  cursor y positions adjustment added so a font
                      is visible inside the viewport
   Revision date:     15-10-05
   Revision Purpose:  Warning added if font height exceeds viewport height
   Revision date:     16-11-07
   Revision Purpose:  Vfont adaption
   Revision date:     16-11-07
   Revision Purpose:  Vfont adaption
   Revision date:     14-11-12
   Revision Purpose:  Dynamic Vfont adaption
   Version number: 4.8
   Copyright (c) RAMTEX Engineering Aps 1998-2012

*********************************************************************/

#include "gi_disp.h" /* glcd prototypes */

#if defined( GSOFT_FONTS ) || defined( GBASIC_TEXT )

#ifdef GVIRTUAL_FONTS
  #include <gvfont.h>
#endif

/********************************************************************
   Segment: SoftFonts
   Level: Fonts
   Set current font (incl. default character h,w)
   pass a pointer from a <font>.c file.
   Returns previous font
*/
PGFONT gselfont( PGFONT pfont )
   {
   PGFONT pf;
   GYT h;
   #ifndef GNOCURSOR
   GCURSOR c;
   #endif

   #ifndef GNOCURSOR
   glcd_err = 0; /* Reset HW error flag */
   gi_datacheck(); /* check internal data for errors */

   c = GDATA_CURSOR;
   #ifndef GHW_NO_HDW_FONT
   if( gishwfont() )
       ghw_setcursor( (GCURSOR)(GDATA_CURSOR & ~GCURON) );
   #endif
   #if  !defined( GHW_NO_HDW_FONT ) && defined( GSOFT_FONTS )
   else
   #endif
   #ifdef GSOFT_FONTS
      {
      gi_cursor( 0 ); /* Remove cursor of old font size */
      }
   #endif
   #endif /* GNOCURSOR */
   pf = gcurvp->pfont; /* Prepare return of old font */

   #ifdef GVIRTUAL_FONTS_DYN
   if (gvf_open(pfont))
   #else
   if (pfont == NULL)
   #endif
      {
      G_WARNING( "gselfont: Parameter, No font selected" );
      gcurvp->pfont = &SYSFONT;
      gcurvp->codepagep = SYSFONT.pcodepage;
      gcurvp->fsize.x = SYSFONT.symwidth;
      gcurvp->fsize.y = SYSFONT.symheight;
      }
   else
      {
      gcurvp->pfont = pfont;
      gcurvp->codepagep = gi_fpcodepage(pfont);
      gcurvp->fsize.x = gi_fsymw(pfont);
      gcurvp->fsize.y = gi_fsymh(pfont);
      }

   /* Move cpos to legal value */
   #ifndef GHW_NO_HDW_FONT
   if( gishwfont() )
      {
      h = GDISPCH;  /* default height */
      #ifndef GNOTXTSPACE
      /* Hardware fonts does not support extra character or line spacing */
      gcurvp->chln.x = 0;
      gcurvp->chln.y = 0;
      #endif
      }
   #endif
   #if  !defined( GHW_NO_HDW_FONT ) && defined( GSOFT_FONTS )
   else
   #endif
   #if  defined( GSOFT_FONTS )
      {
      #if (defined( GVIRTUAL_FONTS) && !defined( GNOTXTSPACE ))
      /* Check if font spacing is used by font */
      if (gisfontv(pfont))
         {
         /* Use font specific extended spacing settings */
         gcurvp->chln.x = gi_chsp(pfont);
         gcurvp->chln.y = gi_lnsp(pfont);
         }
      #endif
      h = gcurvp->fsize.y;
      #ifndef GNOTXTSPACE
      /* Limit line and character spacing to resonable values */
      gi_limit_check();
      #endif
      }
   #endif

   if( gcurvp->cpos.y < gcurvp->lt.y+(h-1))
      {
      gcurvp->cpos.y = gcurvp->lt.y+(h-1); /* Assure font is visible */
      if (gcurvp->cpos.y > gcurvp->rb.y)
         gcurvp->cpos.y = gcurvp->rb.y;    /* Must not exceed buttom */
      }

   #ifdef GGRAPHICS
   /* update graphics pos also */
   gcurvp->ppos.x = (gcurvp->cpos.x > gcurvp->rb.x) ? gcurvp->rb.x : gcurvp->cpos.x;
   gcurvp->ppos.y = gcurvp->cpos.y;
   #endif

   gi_calcdatacheck(); /* correct VP to new settings */

   #ifndef GNOCURSOR
   #ifndef GHW_NO_HDW_FONT
   if( gishwfont() )
       ghw_setcursor( c );
   #endif
   #if  !defined( GHW_NO_HDW_FONT ) && defined( GSOFT_FONTS )
   else
   #endif
   #ifdef GSOFT_FONTS
      {
      GDATA_CURSOR = c;
      gi_cursor( 1 ); /* Restore cursor with new font size */
      }
   #endif
   #endif /* GNOCURSOR */
   return pf;
   }

PGFONT ggetfont( void )
   {
   gi_datacheck(); /* check internal data for errors */
   return gcurvp->pfont;
   }

#ifdef GFUNC_VP

PGFONT gselfont_vp( SGUCHAR vp, PGFONT pfont )
   {
   PGFONT retp;
   GGETFUNCVP(vp, gselfont(pfont) );
   return retp;
   }

#endif /* GFUNC_VP */

PGFONT ggetfont_vp( SGUCHAR vp )
   {
   if ( vp >= GNUMVP)
      {
      G_WARNING("Parameter error vp >= GNUMVP");
      vp = GNUMVP-1;
      }
   return GDATA_VIEWPORTS[vp].pfont;
   }


#ifdef GVIRTUAL_FONTS_DYN
/* Get font codepage */
PGCODEPAGE gi_fpcodepage( PGFONT pfont )
   {
   /* (ROM) located code page stub.
      Signals that code page is located with the font data (can not be used for codepage override) */
   static GCODE GCODEPAGEV FCODE vfontdyn_cpstup =
      {
      0,     /* = 0 = identifier for extended codepage structure */
      3,     /* Type ID (3 = code page info located in dynamically loaded font structure) */
      0,0,0
      };

   if (gisfontv(pfont))
      {
      if (gisfontv_named(pfont))
         {
         if (gvf_curfont->cprnum == 0)
            return NULL; /* No codepage in GFONTVDYN data */
         return (PGCODEPAGE)(&vfontdyn_cpstup);
         }
      else
         return (PGCODEPAGE)(((PGFONTV)(pfont))->pcodepage);
      }
   else
      return pfont->pcodepage;
   }
#endif /* GVIRTUAL_FONTS_DYN */

#endif /* GSOFT_FONTS || GBASIC_TEXT */


