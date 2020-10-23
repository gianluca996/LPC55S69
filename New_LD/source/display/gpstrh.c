/************************* gpstrh.c ********************************

   Creation date: 03-05-03

   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added

   Revision date:
   Revision Purpose:

   Version number: 1.1
   Copyright (c) RAMTEX Engineering Aps 2003-2004

*********************************************************************/
#include "gi_disp.h" /* gLCD prototypes */

#if defined( GBASIC_TEXT ) || defined( GSOFT_FONTS)

extern SGUCHAR gi_strlines( PGCSTR sp );

SGUINT gpstrheight( PGCSTR str )
   {
   #ifdef GWIDECHAR
   GDATA_STRTYPE = 0;
   #endif
   return ((SGUINT) gi_strlines( str )) * ((SGUINT)ggetfh());
   }

#ifdef GFUNC_VP

SGUINT gpstrheight_vp( SGUCHAR vp, PGCSTR str )
   {
   #ifdef GWIDECHAR
   GDATA_STRTYPE = 0;
   #endif
   return ((SGUINT) gi_strlines( str )) * ((SGUINT)ggetfh_vp(vp));
   }

#endif /* GFUNC_VP */

#ifdef GWIDECHAR

SGUINT gpstrheightw( PGCWSTR str )
   {
   GDATA_STRTYPE = 1;
   return ((SGUINT) gi_strlines( (PGCSTR) str )) * ((SGUINT)ggetfh());
   }

#ifdef GFUNC_VP

SGUINT gpstrheightw_vp( SGUCHAR vp, PGCWSTR str )
   {
   GDATA_STRTYPE = 1;
   return ((SGUINT) gi_strlines( (PGCSTR) str )) * ((SGUINT)ggetfh_vp(vp));
   }

#endif /* GFUNC_VP */

#endif /* GWIDECHAR */
#endif /* GBASIC_TEXT || GSOFT_FONTS */

