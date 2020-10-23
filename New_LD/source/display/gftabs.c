/************************* gftabs.c ********************************

   Creation date: 980226

   Revision date:     03-05-06
   Revision Purpose:  GTABS array replaced with gdata.tab array
   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added
   Revision date:     04-07-12
   Revision Purpose:  size calculation adjusted

   Version number: 2.1
   Copyright (c) RAMTEX Engineering Aps 1998-2012

*********************************************************************/
#include "gi_disp.h"

#ifndef GCONSTTAB

/********************************************************************
   Segment: Tabs
   Level: Fonts
   Sets GDATA_TABS[] with tab-positions, spaced s.
   GTABS must be define by user
*/
void gsettabs( GXT s )
   {
   SGUCHAR n,i;
   n = sizeof(GDATA_TABS)/sizeof(GDATA_TABS[0]); /* tab array size */
   for( i=0; i<n; i++ )
      GDATA_TABS[i] = (GXT) s*(i+1);
   }

/********************************************************************
   Segment: Tabs
   Level: Fonts
   Sets GDATA_TABS[] with a tab at s, moving other tabs.
   GTABS must be define by user
*/
void gsettab( GXT s )
   {
   SGUCHAR n,i,j;
   n = sizeof(GDATA_TABS)/sizeof(GDATA_TABS[0]); /* tab array size */

   /* find insert pos */
   for( i=0; i<n; i++ )
      {
      if( GDATA_TABS[i] >= s )
         break;
      if (GDATA_TABS[i] == 0)
         break;
      }

   if( i >= n )
      return; /* Not room for a new tab */

   if( GDATA_TABS[i] == s ) /* don't set two equals */
      return;

   /* move tabs */
   for( j=n-1; j>i; j-- )
      GDATA_TABS[j] = GDATA_TABS[j-1];

   GDATA_TABS[i] = s;
   }

/********************************************************************
   Segment: Tabs
   Level: Fonts
   Clears GDATA_TABS[] to zeros.
   GTABS must be define by user
*/
void gclrtabs(void)
   {
   SGUCHAR n;
   n = sizeof(GDATA_TABS)/sizeof(GDATA_TABS[0]);
   while (n > 0)
      {
      GDATA_TABS[--n] = 0;
      }
   }

#endif /* GCONSTTAB */



