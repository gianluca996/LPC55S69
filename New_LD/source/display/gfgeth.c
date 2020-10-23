/************************* gfgeth.c ********************************

   Creation date: 980223

   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added
   Revision date:     16-11-07
   Revision Purpose:  Vfont adaption
   Revision date:     06-02-08
   Revision Purpose:  Support for additional character spacing
   Revision date:     14-11-12
   Revision Purpose:  Named dynamic virtual font support, font size optimization

   Revision date:
   Revision Purpose:

   Version number: 2.3
   Copyright (c) RAMTEX Engineering Aps 1998-2012

*********************************************************************/

#include "gi_disp.h" /* gLCD protoypes */

#if defined( GBASIC_TEXT ) || defined( GSOFT_FONTS )

#ifdef GVIRTUAL_FONTS_DYN
  #include "gvfont.h"
#endif

/********************************************************************
   Segment: Soft Fonts
   Level: Fonts
   return font height
*/
GYT gfgetfh( PGFONT fp )
   {
   #ifdef GVIRTUAL_FONTS_DYN
   if (gvf_open(fp))
      return SYSFONT.symheight;  /* NULL pointer or a named font, and open failed, use default height */
   #else
   if (fp == NULL)
      return SYSFONT.symheight;  /* default height */
   #endif
   return gi_fsymh(fp);
   }

GYT ggetfh(void)
   {
   gi_datacheck();    /* check internal data for errors */
   #ifndef GNOTXTSPACE
   return gcurvp->fsize.y + gcurvp->chln.y;
   #else
   return gcurvp->fsize.y;
   #endif
   }

GYT ggetfh_vp(SGUCHAR vp)
   {
   GCHECKVP(vp);
   #ifndef GNOTXTSPACE
   return GDATA_VIEWPORTS[vp].fsize.y + GDATA_VIEWPORTS[vp].chln.y;
   #else
   return GDATA_VIEWPORTS[vp].fsize.y;
   #endif
   }

#endif  /* GBASIC_TEXT */

