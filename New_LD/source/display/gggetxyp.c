/************************* gggetxyp.c ******************************

   Creation date: 980224

   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added

   Revision date:
   Revision Purpose:

   Version number: 2.1
   Copyright (c) RAMTEX Engineering Aps 1998-2004

********************************************************************/

#include "gi_disp.h" /* gLCD prototypes */

#ifdef GGRAPHICS

/********************************************************************
   Segment: Graphics
   Level: Graphics
   Return current graphics pixel xpos.
*/
GXT ggetxpos(void)
   {
   gi_datacheck(); /* check internal data for errors */
   return gcurvp->ppos.x-gcurvp->lt.x;
   }

/********************************************************************
   Segment: Graphics
   Level: Graphics
   Return current graphics pixel ypos.
*/
GYT ggetypos(void)
   {
   gi_datacheck(); /* check internal data for errors */
   return gcurvp->ppos.y-gcurvp->lt.y;
   }

#ifdef GFUNC_VP

GXT ggetxpos_vp( SGUCHAR vp )
   {
   GXT retp;
   GGETFUNCVP( vp, ggetxpos() );
   return retp;
   }

GYT ggetypos_vp( SGUCHAR vp )
   {
   GYT retp;
   GGETFUNCVP( vp, ggetypos() );
   return retp;
   }

#endif /* GFUNC_VP */

#endif /* GGRAPHICS */

