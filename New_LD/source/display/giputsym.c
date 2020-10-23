
/*****************************************************************
   Internal function for symbols.
   writes a symbol at absolute coordinates, processes symbol header
   and viewport mode information.
   Absolute coordinates used.

   Revision date:     010205
   Revision Purpose:  Correction of overflow error when large symbol is
                      cropped near screen edges.

   Revision date:     300108
   Revision Purpose:  Support for transperant mode added

   Revision date:     06-02-08
   Revision Purpose:  Vfont adaption

   Revision date:     17-04-2009
   Revision Purpose:  Virtual font structure split in a RAM dependent and a constant
                      data structure
                      (to handle compilers using non-standard C conformant pointers)

   Version number: 2.7
   Copyright (c) RAMTEX Engineering Aps 1998-2009

***********************************************************************/

#include "gi_disp.h" /* gLCD prototypes */

#ifdef GSOFT_SYMBOLS

#ifdef GVIRTUAL_FONTS
#include <gvfont.h>
#endif

void gi_putsymbol(GXT xs,GYT ys,GXT xemax,GYT yemax,
            PGSYMBOL psymbol, GYT yoffset, SGUINT symsize)
   {
   SGUINT xe;
   SGUINT ye;
   SGUCHAR bw;
   SGUCHAR mode; /* hw mode flags */
   PGSYMBYTE f;

   xe = (SGUINT) gsymw(psymbol);
   ye = (SGUINT) gsymh(psymbol);
   if ((ye == 0) || (xe == 0))
      {
      /* Error: illegal symbol header data. Corrupted data or a ROM/RAM pointer problem detected */
      G_WARNING( "Illegal symbol header data detected. Zero sized symbol" );
      #ifdef GVIRTUAL_FONTS
      if (gissymbolv(psymbol) && (xe == 0))
         {
         G_WARNING("Check if virtual font has been opened correctly");
         }
      #endif
      return;
      }

   #ifdef GVIRTUAL_FONTS
   if (gissymbolv(psymbol))
      {
      #ifdef GDATACHECK
      register GYT type_id = (((PGSYMHEADV)psymbol)->type_id) & GVTYPEMASK;
      #ifdef GVIRTUAL_FONTS_DYN
      if ((type_id < 1) || (type_id > 3))
      #else
      if (type_id != 1)
      #endif
           { /* Error: format is unknown to this library or configuration */
           G_WARNING( "Unsupported extended font format. Illegal virtual font" );
           return;
           }
      if ((((PGSYMHEADV)psymbol)->psymh_v->numbits) != 0)
         { /* Error Color symbols are not supported by b&w libs */
         G_WARNING( "Color symbols are not supported by b&w libs" );
         return;
         }
      #endif
      f = (PGSYMBYTE) NULL; /* Signal use of virtual font to low-level driver*/
      }
   else
      #endif
      f = (PGSYMBYTE)&(((PGBWSYMBOL)psymbol)->b[0]);   /* first byte in symbol */
   mode = G_IS_INVERSE() ? GHW_INVERSE : 0;

   if (symsize == 0)
      bw = (SGUCHAR) ((xe+7)/8); /* width of symbol in bytes */
   else
      bw = (SGUCHAR) (symsize/ye);

   if (G_IS_TRANSPERANT())
       mode |= GHW_TRANSPERANT;

   if (yoffset != 0)
      {                          /* Only showing lower part of symbol */
      ys = ys + yoffset;
      ye = ye - yoffset;
      #ifdef GVIRTUAL_FONTS
      if (!gissymbolv(psymbol))
      #endif
         f=&f[(SGUINT)bw*yoffset];
      }

   xe = xe+((SGUINT) xs)-1;
   ye = ye+((SGUINT) ys)-1;

   /* truncate at max rect */
   if( xe > ((SGUINT) xemax) )
      xe = (SGUINT) xemax;
   if( ye > ((SGUINT) yemax) )
      ye = (SGUINT) yemax;

   glcd_err = 0; /* Reset HW error flag */

   #ifdef GVIRTUAL_FONTS
   if (gissymbolv(psymbol))
      gi_symv_open( psymbol, bw, yoffset ); /* Preset virtual symbol interface */
   #endif

   ghw_wrsym(xs, ys, (GXT) xe, (GYT) ye, f, bw, mode );
   }

#ifdef GVIRTUAL_FONTS

GXT gsymw(PGSYMBOL psymbol)
   {
   if (gissymbolv(psymbol))
      return psymbol->vsh.psymh_v->cxpix;
   else
      return psymbol->sh.cxpix;
   }

GYT gsymh(PGSYMBOL psymbol)
   {
   if (gissymbolv(psymbol))
      return psymbol->vsh.psymh_v->cypix;
   else
      return psymbol->sh.cypix;
   }

#endif /* GVIRTUAL_FONTS */

#endif /* GSOFT_SYMBOLS */


