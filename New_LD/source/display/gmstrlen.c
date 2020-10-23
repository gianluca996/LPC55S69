/************************* gmslen.c *********************************

   Creation date: 13-04-2004

   gmsstrlen() reports the total number of symbol characters in a string
   excluding the terminating 0.
   Similar to strlen(..), but a multibyte character is only counted as one symbol.
   Useful when calculating visual character positions with monospace fonts.
   (Returns the visual length of a multibyte string)

   gmstrlen() reports the total number of bytes (characters) in a string
   excluding the terminating 0.
   Similar to strlen(..) but accepts a string with multibyte characters where
   ex. the second byte may be 0 (depends on the multibyte encoding).
   Useful when space for a mulitbyte string should be allocated or text strings
   should be appended to the end of a multibyte string.
   (Returns the memory allocation length of a multibyte string - 1 )

   Revision date:     090207
   Revision Purpose:  Use of gi_sizeofmb(..) to detect number of bytes
                      occupied by (multibyte) character.
   Revision date:     140207
                      gmsstrlen count error corrected.
                      Functionality and description now matches.

   Version number: 2.3
   Copyright (c) RAMTEX Engineering Aps 1998-2007

*********************************************************************/

#include "gi_disp.h" /* gLCD prototypes */

/********************************************************************
   Segment: SoftFonts
   Level: Strings
   Return string length in number of character (logic size) if charcnt = 0
   or in number of bytes (storage size) if charcnt = 1
*/

static SGUINT gm_strlen( PGCSTR str , SGUCHAR charcnt )
   {
   SGUINT len;

   if( str == NULL )
      return 0;

   len = 0;

   while( *str != '\0')
      {
      #ifdef GMULTIBYTE
      SGUCHAR size;
      size = gi_sizeofmb( str ); /* Get number of bytes occupied by the (multi-byte) character */
      if (charcnt)
         len = len + (SGUINT)size;      /* Count storage size */
      else
         len++;                  /* Count logical size in number of (multibyte) characters */

      while (size-- > 0)         /* Step to next (multibyte) character */
         str++;
      #else
      str++;
      len++;
      #endif
      }

   #ifndef GMULTIBYTE
   charcnt++; /* Just to silence a not-used warning */
   #endif

   return len;
   }

/* Number of storage bytes in string (physical length) */
SGUINT gmstrlen( PGCSTR str )
   {
   return gm_strlen(str,1);
   }

/* Number of symbol characters in string (logical length) */
SGUINT gmsstrlen( PGCSTR str )
   {
   return gm_strlen(str,0);
   }

#ifdef GWIDECHAR

/* Return multibyte size of wc */
SGUCHAR gwcmbsize( GWCHAR wc )
   {
   #ifdef GMULTIBYTE
   #ifdef GMULTIBYTE_UTF8
   if (wc < 0x80)   /* Is character plain ASCII */
      return 1;
   else
   if (wc < 0x800)
      return 2;
   else
      return 3;     /* UTF-8 may use 6 bytes but 16 bit widechars are limited to 3 UTF-8 bytes */
   #else
   if (wc >= G_MULTIBYTE_LIMIT)   /* Is character the first byte of a 2 byte value ?*/
      return 2;
   else
      return 1;
   #endif
   #else
   return 1;
   #endif
   }

/* Number of storage bytes needed to hold the data in a wide char string
   as a multi-byte string. (excl any terminating \0) */
SGUINT gmstrlenw( PGCWSTR str )
   {
   SGUINT size;
   size = 0;
   if (str != NULL)
      {
      while (*str != 0)
         size = size + gwcmbsize(*str++);
      }
   return size;
   }

#endif /* GWIDECHAR */

