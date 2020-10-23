/************************* gmbcpyw.c *********************************

   Creation date: 20-04-07

   Convert and copy between multibyte strings and wide char strings
   Similar to strcpy(..) except that format conversion takes place.
   If neither GMULTIBYTE nor GWIDECHAR is defined the functions behave
   similar to strcpy()

   The destination string must be able to hold the converted string.
   A multi-byte string may be 3 times larger than widechar string.

   The needed destination string length can be calculated in advance
   with the functions
      SGUINT gmstrlenw( PGWCSTR str )   needed length to hold multi-byte string
      SGUINT gmsstrlen( PGCSTR str )    needed length to hold widechar string

   Revision date:
   Revision Purpose:

   Version number: 1
   Copyright (c) RAMTEX Engineering Aps 2007

*********************************************************************/

#include "gi_disp.h" /* gLCD prototypes */

/*
   Save widechar value as a multibyte string value
   Returns the number of bytes used for storing the value.
*/
static SGUCHAR gi_wctomb( PGSTR str, GWCHAR wc )
   {
   #ifdef GMULTIBYTE
   #ifdef GMULTIBYTE_UTF8
   /* UTF-8 encoding */
   if (wc < 0x80)
      {
      *str = (SGUCHAR) wc;
      return 1;
      }
   else
   if (wc < 0x800)
      {
      *str = (((SGUCHAR)(wc >> 6)) & 0x1F)|0xc0;
      str++;
      *str = (((SGUCHAR)wc) & 0x3F)|0x80;
      return 2;
      }
   else
      {
      *str = (((SGUCHAR)(wc >> 12)) & 0x0F)|0xe0;
      str++;
      *str = (((SGUCHAR)(wc >> 6)) & 0x3F)|0x80;
      str++;
      *str = (((SGUCHAR)wc) & 0x3F)|0x80;
      return 3;
      }
   #else
   /* Simple multi-byte encoding */
   if ((wc >= G_MULTIBYTE_LIMIT) && (wc < G_MULTIBYTE_LIMIT*256))   /* Is character legal for encoding */
      wc = G_MALFORMED_RETURNCHAR;

   if (wc < G_MULTIBYTE_LIMIT)   /* Is character the first byte of a 2 byte value ?*/
      {
      *str = (SGUCHAR) wc;
      return 1;
      }
   else
      {
      *str = (SGUCHAR) (wc >> 8) & 0xff;
      str++;
      *str = ((SGUCHAR)wc);
      return 2;
      }
   #endif

   #else
   /* No multibyte support, just mark illegal values and do simple type conversion */
   #ifdef GWIDECHAR
   if (wc > 0xff)   /* Is character legal for encoding */
      wc = G_MALFORMED_RETURNCHAR;
   #endif
   *str = (SGUCHAR) wc;
   return 1;
   #endif
   }

/*
   Save widechar value as a multibyte string value
   Returns the number of bytes used for storing the value
*/
SGUCHAR gwctomb( PGSTR str, GWCHAR wc )
   {
   if (str == NULL)
      return 0;
   return gi_wctomb( str, wc );
   }

/*
   Copy a wide char string to a multibyte character string
*/
PGSTR gwstrcpymb( PGSTR dstr, PGCWSTR sstr )
   {
   if (( sstr != NULL ) && ( dstr != NULL ))
      {
      while (*sstr != 0)
         {
         /* Convert and let destination point to next byte */
         dstr = &dstr[gi_wctomb( dstr, *sstr )];
         sstr++;
         }
      *dstr = 0;
      }
   return dstr;
   }

/*
   Copy a multibyte character string to a wide char string
*/
PGWSTR gmbstrcpyw( PGWSTR dstr, PGCSTR sstr )
   {
   if (( sstr != NULL ) && ( dstr != NULL ))
      {
      while (*sstr != 0)
         {
         /* Convert and let destination point to next byte */
         #ifdef GMULTIBYTE
         *dstr = ggetmbc(&sstr);
         #else
         *dstr = (GWCHAR) (*sstr);
         sstr++;
         #endif
         dstr++;
         }
      *dstr = 0;
      }
   return dstr;
   }


