/************************* gvpset.c ********************************

   Creation date: 20-05-03

   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added

   Version number: 2
   Copyright (c) RAMTEX Engineering Aps 2003-2004

*********************************************************************/

#include "gi_disp.h" /* gLCD prototypes */

#ifdef GVIEWPORT

/********************************************************************
   Segment: Viewport
   Level: Viewport
   Get current view-port in abolute coordinates
   0,0 is upper left corner.
   0,0,0,0 is a view-port of 1 pixel.
   The view-port coordinates are limited to the LCD size (GDISPW-1,GDISPH-1).
   gselvp() should be called in advance to this function.
*/
void ggetvp(GXT *xs, GYT *ys, GXT *xe, GYT *ye )
   {
   gi_datacheck(); /* check internal data for errors */
   if (xs != NULL)
      *xs = gcurvp->lt.x;
   if (ys != NULL)
      *ys = gcurvp->lt.y;
   if (xe != NULL)
      *xe = gcurvp->rb.x;
   if (ye != NULL)
      *ye = gcurvp->rb.y;
   }

#ifdef GFUNC_VP

void ggetvp_vp( SGUCHAR vp, GXT *xs, GYT *ys, GXT *xe, GYT *ye )
   {
   GCHECKVP( vp );
   if (xs != NULL)
      *xs = GDATA_VIEWPORTS[vp].lt.x;
   if (ys != NULL)
      *ys = GDATA_VIEWPORTS[vp].lt.y;
   if (xe != NULL)
      *xe = GDATA_VIEWPORTS[vp].rb.x;
   if (ye != NULL)
      *ye = GDATA_VIEWPORTS[vp].rb.y;
   }

#endif /* GFUNC_VP */
#endif


