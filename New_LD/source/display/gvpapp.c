/************************* gvpapp.c ********************************

   Creation date: 04-07-04

   Revision date:     13-08-04
   Revision Purpose:  ggetappvp renamed to ggetapp_vp for compatibility
                      with named viewport functions

   Version number: 2.1
   Copyright (c) RAMTEX Engineering Aps 2004

*********************************************************************/

#include "gi_disp.h" /* LCD prototypes */

#ifdef GEXTMODE  /* and defined GVIEWPORT */

/* Get application data pointer for current vp */
PGVPAPP ggetapp( void )
   {
   gi_datacheck();  /* check internal data for errors (correct illegal gcurvp) */
   return &(gcurvp->vpapp);
   }

/* Get vp number for viewport where application data is placed */
SGUCHAR ggetvpapp( PGVPAPP app )
   {
   SGUCHAR i;
   /* find current vp index */
   for( i=0; i<GNUMVP; i++ )
      {
      if( app == &GDATA_VIEWPORTS[i].vpapp )
         break;
      }
   if( i<GNUMVP )
      return i;  /* Return viewport which app points to */
   else
      {
      /* The warning message here can be commented out if it is legal for
         the application to use fallback to a default viewport */
      G_WARNING( "ggetvpapp: parameter, Illegal PGVPAPP" );

      return 0;  /* Use viewport 0 as the default viewport, or to signal lookup failure */
      }
   }

/* Get application data pointer for named vp */
PGVPAPP ggetapp_vp( SGUCHAR vp )
   {
   GCHECKVP( vp );
   return &(GDATA_VIEWPORTS[vp].vpapp);
   }

#endif

