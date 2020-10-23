/************************* gvpvph.c ********************************

   Creation date: 980220

   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added

   Revision date:     20-07-2011
   Revision Purpose:  Return type for ggetvph_vp fixed

   Version number: 2.1
   Copyright (c) RAMTEX Engineering Aps 1998-2011

*********************************************************************/

#include "gi_disp.h" /* gLCD prototypes */

#ifdef GVIEWPORT

/********************************************************************
   Segment: Viewport
   Level: Viewport
   return viewport height
*/
GYT ggetvph(void)
   {
   gi_datacheck(); /* check internal data for errors */
   return (gcurvp->rb.y - gcurvp->lt.y) + 1;
   }

GYT ggetvph_vp(SGUCHAR vp)
   {
   GCHECKVP( vp );
   return GDATA_VIEWPORTS[vp].rb.y - GDATA_VIEWPORTS[vp].lt.y + 1;
   }

#endif

