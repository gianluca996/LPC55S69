/************************* gvpmode.c *******************************

   Creation date: 980220

   Revision date:    02-01-26
   Revision Purpose: Return all mode bits
   Revision date:    03-02-03
   Revision Purpose: Type consistence. Handle extended GMODE definitions
   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added
   Revision date:
   Revision Purpose:

   Version number: 2.3
   Copyright (c) RAMTEX Engineering Aps 1998-2004

*********************************************************************/

#include "gi_disp.h" /* gLCD prototypes */

#ifdef GVIEWPORT
/********************************************************************
   Segment: Viewport
   Level: Viewport
   Set new display mode.
   Return old display mode.
   GMODE defined in gdisp.h
*/

GMODE gi_setmode( GMODE mode )
   {
   GMODE oldmode;
   gi_datacheck();     /* check internal data for errors */
   oldmode = gcurvp->mode;
   #ifdef GWORDWRAP
   /* Make compatible with old versions where GWORDWRAP was defined in gdispcfg.h */
   mode |= GWORD_WRAP;
   #endif
   gcurvp->mode = mode;
   gi_calcdatacheck(); /* correct VP to new settings */
   return oldmode;
   }

#ifdef GFUNC_VP
GMODE gi_setmode_vp( SGUCHAR vp, GMODE mode )
   {
   GMODE retp;
   GGETFUNCVP(vp, gi_setmode(mode) );
   return retp;
   }
#endif /* GFUNC_VP */

#endif

