/************************* gstrln.c ********************************

   Creation date: 03-05-03

   Revision date:
   Revision Purpose:

   Version number: 1.0
   Copyright (c) RAMTEX Engineering Aps 2003

*********************************************************************/
#include "gi_disp.h" /* gLCD prototypes */

/*
   Returns the number of text lines in a multi-line string
   Returns 0 if str = 0
   Returns 1 if *str = 0
*/
SGUCHAR gi_strlines( PGCSTR sp )
   {
   SGUCHAR lh = 1;
   GSTRINGPTR str;

   if (sp == NULL)
      return 0;

   for (str.s = sp;;)
      {
      GWCHAR c;
      c =  GETCTRLCHAR(str);
      if (c == 0)
         break;
      if (c == (GWCHAR) '\n')
         {
         lh++;
         if (lh >= 255)
            break;       /* Prevent overflow */
         }
      GINCPTR(str);
      }
   return lh;
   }

SGUCHAR gstrlines( PGCSTR str )
   {
   #if defined( GWIDECHAR ) && defined ( GVIEWPORT )
   GDATA_STRTYPE = 0;
   #endif
   return gi_strlines( str );
   }

#ifdef GWIDECHAR

SGUCHAR gstrlinesw( PGCWSTR str )
   {
   #ifdef GVIEWPORT
   GDATA_STRTYPE = 1;
   #endif
   return gi_strlines( (PGCSTR) str );
   }

#endif

