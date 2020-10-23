/************************* gslen.c *********************************

   Creation date: 980224

   Revision date:     20-04-03
   Revision Purpose:  gpstrlen and gpwordlen combined, using a macro frontend
                      to gi_strlen. Support for multi-byte C strings added.

   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added

   Revision date:     05-02-08
   Revision Purpose:  Added support for inter character spacing

   Version number: 2.3
   Copyright (c) RAMTEX Engineering Aps 1998-2008

*********************************************************************/

#include "gi_disp.h" /* gLCD prototypes */

/********************************************************************
   Segment: SoftFonts
   Level: Strings
   Return string length in pixels with current font
   if wordmode = 0 special chars \n, \r and \t terminates string
   if wordmode = 1 word length is from start of string to first space, \n, \t or \r.
*/
#if defined( GBASIC_TEXT ) || defined( GSOFT_FONTS )

SGUINT gi_strlen( PGCSTR sp, SGUCHAR mode )
   {
   SGUINT len;
   SGUINT lenmax;
   GSTRINGPTR str;
   #ifndef GNOTXTSPACE
   SGBOOL firstch;
   firstch = 1;
   #endif

   if( sp == NULL )
      return 0;
   str.s = (PGCSTR) sp;

   gi_datacheck(); /* check internal data for errors */


   for(len = 0,lenmax = 0;;)
      {
      GWCHAR c;
      c =  GETCHAR(str);
      if (c == 0)
         break;
      switch( c )
         {
         case ((GWCHAR)'\n'):
         case ((GWCHAR)'\r'):
            {
            if (mode < 2)
               return len;
            if (len > lenmax)
               lenmax = len;
            len = 0;
            #ifndef GNOTXTSPACE
            firstch = 1;
            #endif
            break;
            }
         case ((GWCHAR)'\t'):
            {
            if (mode < 2)
               return len;
            /* Should never be used, but count tab as a default character, just in case */
            len += GDISPCW;
            break;
            }
         case ((GWCHAR)' '):
            {
            if (mode == 1)
               return len;
            }
         default:
            {
            #ifndef GHW_NO_HDW_FONT
            if( gishwfont() )
               len += GDISPCW;
            #endif
            #if (defined( GSOFT_FONTS ) && !defined(GHW_NO_HDW_FONT))
            else
            #endif
            #ifdef GSOFT_FONTS
               {
               PGSYMBOL ps;
               if ((ps = gi_getsymbol(c, gcurvp->pfont, gcurvp->codepagep)) != NULL)
                  {
                  len = len + gsymw(ps); /* = GDISPCW ifndef GGSOFT_FONT */
                  #ifndef GNOTXTSPACE
                  if (firstch)
                     firstch = 0;
                  else
                     /* If not first character then add inter character space */
                     len = len + gcurvp->chln.x;
                  #endif
                  }
               }
            #endif
            }
         }
      GINCPTR(str);
      }
   return ((mode == 2) && (lenmax > len)) ? lenmax : len;
   }

SGUINT gpstrlen( PGCSTR str )
   {
   #ifdef GWIDECHAR
   GDATA_STRTYPE = 0;
   #endif
   return gi_strlen( str, 0);
   }

SGUINT gpwordlen( PGCSTR str )
   {
   #ifdef GWIDECHAR
   GDATA_STRTYPE = 0;
   #endif
   return gi_strlen( str, 1);
   }

SGUINT gpstrwidth( PGCSTR str )
   {
   #ifdef GWIDECHAR
   GDATA_STRTYPE = 0;
   #endif
   return gi_strlen( str, 2);
   }

#ifdef GFUNC_VP

SGUINT gpstrlen_vp( SGUCHAR vp, PGCSTR str )
   {
   SGUINT retp;
   #ifdef GWIDECHAR
   GDATA_STRTYPE = 0;
   #endif
   GGETFUNCVP(vp, gi_strlen(str,0) );
   return retp;
   }

SGUINT gpwordlen_vp( SGUCHAR vp, PGCSTR str )
   {
   SGUINT retp;
   #ifdef GWIDECHAR
   GDATA_STRTYPE = 0;
   #endif
   GGETFUNCVP(vp, gi_strlen(str,1) );
   return retp;
   }

SGUINT gpstrwidth_vp( SGUCHAR vp, PGCSTR str )
   {
   SGUINT retp;
   #ifdef GWIDECHAR
   GDATA_STRTYPE = 0;
   #endif
   GGETFUNCVP(vp, gi_strlen(str,2) );
   return retp;
   }

#endif /* GFUNC_VP */


/* Wide char functions */

#ifdef GWIDECHAR

SGUINT gpstrlenw( PGCWSTR str )
   {
   GDATA_STRTYPE = 1;
   return gi_strlen( (PGCSTR)str, 0);
   }

SGUINT gpwordlenw( PGCWSTR str )
   {
   GDATA_STRTYPE = 1;
   return gi_strlen( (PGCSTR)str, 1);
   }

SGUINT gpstrwidthw( PGCWSTR str )
   {
   GDATA_STRTYPE = 1;
   return gi_strlen( (PGCSTR) str, 2);
   }


#ifdef GFUNC_VP

SGUINT gpstrlenw_vp( SGUCHAR vp, PGCWSTR str )
   {
   SGUINT retp;
   GDATA_STRTYPE = 1;
   GGETFUNCVP(vp, gi_strlen( (PGCSTR) str,0) );
   return retp;
   }

SGUINT gpwordlenw_vp( SGUCHAR vp, PGCWSTR str )
   {
   SGUINT retp;
   GDATA_STRTYPE = 1;
   GGETFUNCVP(vp, gi_strlen( (PGCSTR) str,1) );
   return retp;
   }

SGUINT gpstrwidthw_vp( SGUCHAR vp, PGCWSTR str )
   {
   SGUINT retp;
   GDATA_STRTYPE = 1;
   GGETFUNCVP(vp, gi_strlen( (PGCSTR) str,2) );
   return retp;
   }

#endif /* GFUNC_VP */
#endif /* GWIDECHAR */

#endif /* GBASIC_TEXT || GSOFT_FONTS */
