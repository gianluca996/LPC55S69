/************************* gvpvpw.c ********************************

   Creation date: 980220

   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added

   Revision date:
   Revision Purpose:

   Version number: 2.1
   Copyright (c) RAMTEX Engineering Aps 1998-2004

*********************************************************************/

#include "gi_disp.h" /* gLCD prototypes */

#ifdef GVIEWPORT

/********************************************************************
   Segment: View-port
   Level: View-port
   return view-port width
*/
GXT ggetvpw(void)
   {
   gi_datacheck(); /* check internal data for errors */
   return (gcurvp->rb.x - gcurvp->lt.x) + 1;
   }

GXT ggetvpw_vp(SGUCHAR vp)
   {
   GCHECKVP( vp );
   return GDATA_VIEWPORTS[vp].rb.x - GDATA_VIEWPORTS[vp].lt.x + 1;
   }

#endif

