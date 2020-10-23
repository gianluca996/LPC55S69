/************************* gmstrcpy.c *********************************

   Creation date: 13-04-2004

   Copy or concatenate multibyte strings.
   Similar to strcpy(..) and strcat(..) except that they accept
   multibyte characters where the second character is allowed to be 0
   (depends on the multibyte encoding)

   Revision date:     090207
   Revision Purpose:  Use of gi_sizeofmb(..) to detect number of byte
                      occupied by (multibyte) character.

   Version number: 2.2
   Copyright (c) RAMTEX Engineering Aps 2004-2007

*********************************************************************/

#include "gi_disp.h" /* gLCD prototypes */

/*
   Copy a multibyte character string
*/
PGSTR gmstrcpy( PGSTR dstr, PGCSTR sstr )
   {
   SGUINT i;
   if (( sstr != NULL ) && ( dstr != NULL ))
      {
      i=0;
      do
         {
         #if (defined( GMULTIBYTE ) && !defined( GMULTIBYTE_UTF8 ))
         /* Prevent that the second byte of a multi-byte character stops copying if it is \0 */
         SGUCHAR size;
         size = gi_sizeofmb( sstr ); /* Get number of bytes occupied by the (multi-byte) character */
         while (size-- > 0)
            dstr[i++] = *sstr++;
         #else
         /* UTF-8 strings can be copied like normal strings */
         dstr[i++] = *sstr++;
         #endif
         }
      while( *sstr != '\0' );
      dstr[i] = 0;
      }
   return dstr;
   }

/*
   Append a multibyte character string to a string.
   
   Note: This functions requires a compiler with standard C
   conformant pointer handling. Ex that the same function pointer
   parameter can point both to objects located in ROM and to objects 
   located in RAM  
  
*/
PGSTR gmstrcat( PGSTR dstr, PGCSTR sstr )
   {
   if ( dstr == NULL )
      return NULL;
   gmstrcpy( &dstr[gmstrlen((PGCSTR)dstr)], sstr );
   return dstr;
   }

