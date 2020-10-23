/************************* gvpscrol.c ******************************

   Controls the use of view-ports

   Creation date: 980223

   Revision date:      02-01-27
   Revision Purpose:   Support for soft cursors change.
   Revision date:      03-01-26
   Revision Purpose:   Bit mask on GINVERSE mode
   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added
   Revision date:     16-11-07
   Revision Purpose:  Vfont adaption

   Version number: 2.3
   Copyright (c) RAMTEX Engineering Aps 1998-2005

*********************************************************************/

#include "gi_disp.h" /* gLCD prototypes */

#if (defined( GBASIC_TEXT ) || defined( GSOFT_FONTS ) || defined( GGRAPHICS ))

/********************************************************************
   Segment: Viewport
   Level: Fonts
   Scroll graphics and soft-fonts in view-port a character line up.
   The number of pixel lines depend on the current font height
   If HW FONT is selected the appropriate ghw_gscroll/ghw_tscroll
   function is called.
*/
void gscrollcvp(void)
   {
   glcd_err = 0;   /* Reset HW error flag */
   gi_datacheck(); /* check internal data for errors */

   gi_cursor( 0 ); /* kill cursor */

   #ifndef GHW_NO_HDW_FONT
   /* Scroll text fonts */
   ghw_tscroll(
      gcurvp->lt.x,
      gcurvp->lt.y,
      gcurvp->rb.x,
      gcurvp->rb.y);
   #endif

   #if defined(GSOFT_FONTS) || defined(GGRAPHIC)
   /* Scroll graphic symbols */
   ghw_gscroll(
            gcurvp->lt.x,
            gcurvp->lt.y,
            gcurvp->rb.x,
            gcurvp->rb.y,
            (GYT)gi_fsymh(gcurvp->pfont),
            (SGUINT)(G_IS_INVERSE() ? 0xFFFF : 0x0000));
   #endif

   gi_cursor( 1 ); /* set cursor */
   ghw_updatehw();
   }

#ifdef GFUNC_VP

void gscrollcvp_vp( SGUCHAR vp )
   {
   GSETFUNCVP(vp, gscrollcvp() );
   }

#endif /* GFUNC_VP */

#endif /* GBASIC_TEXT || GSOFT_FONT */

