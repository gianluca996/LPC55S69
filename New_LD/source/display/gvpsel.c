/************************* gvpsel.c ********************************

   Creation date: 980220

   Revision date: 24-04-2004
   Revision Purpose: vp limit in gi_selvp corrected

   Version number: 2.0
   Copyright (c) RAMTEX Engineering Aps 1998-2004

*********************************************************************/

#include "gi_disp.h" /* gLCD prototypes */

#ifdef GVIEWPORT

/********************************************************************
   Segment: View-port
   Level: View-port
   Returns current view-port
   GNUMVP in gdispcfg.h must
   be set to number of VP's to support.
   After ginit() view-port 0 is set as default.
*/
SGUCHAR ggetvpnum( void )
   {
   SGUCHAR i;
   gi_datacheck(); /* check internal data for errors */
   /* find current vp index */
   for( i=0; i<GNUMVP; i++ )
      {
      if( gcurvp == &GDATA_VIEWPORTS[i] )
         break;
      }
   return ( i<GNUMVP ) ? i : 0;
   }

/* VP select function without returning old vp (selects viewport faster)*/
void gi_selvp( SGUCHAR vp )
   {
   if (vp < GNUMVP)
      gcurvp = &GDATA_VIEWPORTS[ vp ];
   else
      {
      G_WARNING("Viewport number >= GNUMVP definition, Defaults to GNUMVP-1");
      gcurvp = &GDATA_VIEWPORTS[ GNUMVP-1 ];
      }
   }

/********************************************************************
   Segment: View-port
   Level: View-port
   Selects the view-port to become current, GNUMVP in gdispcfg.h must
   be set to number of VP's to support.
   After ginit() view-port 0 is set as default.
   Returns previous current view-port
*/
SGUCHAR gselvp( SGUCHAR vp /* view-port to select */ )
   {
   SGUCHAR vpold;
   vpold = ggetvpnum();   /* Get old viewport number (and do data check) */
   GCHECKVP(vp);
   gcurvp = &GDATA_VIEWPORTS[vp];
   return vpold;
   }

#endif

