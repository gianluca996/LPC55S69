/************************* gfsymw.c *******************************

   Creation date: 980223

   Revision date:     120403
   Revision Purpose:  Support for PGENERIC and intrinsic generic pointers

   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added

   Revision date:     3-11-04
   Revision Purpose:  gi_getsymbol (GBUFINT) cast added to assure correct
                      index calculations with all compilers when the symbol
                      is larger than 256 bytes.

   Revision date:     22-04-05
   Revision Purpose:  Increased codepage look-up speed by using a binary search
                      algoritm. Require that code page tables must be
                      sorted in increasing order (= library and IconEdit default).
                      If codepages are sorted in random order then use the slower
                      linear search method instead by defining USE_LINEAR_SEARCH
                      below
   Revision date:     16-11-07
   Revision Purpose:  Vfont adaption
   Revision date:     14-11-12
   Revision Purpose:  Dyn Vfont adaption

   Version number: 2.7
   Copyright (c) RAMTEX Engineering Aps 1998-2012

*********************************************************************/
#include "gi_disp.h" /* gLCD prototypes */

#ifndef GVIRTUAL_FONTS
/*#define USE_LINEAR_SEARCH*/ /* Codepage range lookup is slower when defined, but codepage element order become dont care */
#endif

#ifdef GSOFT_FONTS

#ifdef GVIRTUAL_FONTS
  #include <gvfont.h>
#endif

/*********************************************************************
   Finds a pointer to char
   Must not be called if it is a hardware font
*/
PGSYMBOL gi_getsymbol( GWCHAR c, PGFONT pfont, PGCODEPAGE codepagep)
   {
   PGSYMBYTE ps;
   GBUFINT s;
   PGCP_RANGE crpb;
   #ifdef USE_LINEAR_SEARCH
   GWCHAR cnt;
   #endif
   #if !defined( USE_LINEAR_SEARCH ) || defined( GVIRTUAL_FONTS )
   GWCHAR numelem, min, half;
   PGCP_RANGE crp;
   #endif

   #ifdef GVIRTUAL_FONTS
   PGCP_RANGE_V crpv;
   GWCHAR def_wch;

   if (gisfontv(pfont))
      {
      /* Prepare virtual font driver for codepage and symbol lookup */
      #ifdef GDATACHECK
      /* Check for mixed font design error */
      if ((((PGFONTV)pfont)->type_id != 1)
      #ifdef GVIRTUAL_FONTS_DYN
            && (((PGFONTV)pfont)->type_id != 2)
            && (((PGFONTV)pfont)->type_id != 3)
      #endif
            )
         { /* Error: format is unknown to this library */
         G_WARNING( "Unsupported extended font format. Illegal virtual font" );
         return NULL;
         }
      #endif
      gi_fontv_open((PGFONTV) pfont);
      if( codepagep != NULL )
         {
         if (gisfontcpv(codepagep))
            {
            /* Extended codepage in virtual memory */
            #ifdef GDATACHECK
            #ifdef GVIRTUAL_FONTS_DYN
            if (giscpv_named(codepagep) && !gisfontv_named(pfont))
               {
               /* Named code page can only be used with a named font
                  (code page lookup info is inside dynamic loaded font structure) */
               return NULL;
               }
            #endif
            #endif

            /* Do fast codepage lookup in virtual memory */
            for(;;)
               {
               /* Sub range = full range */
               #ifdef GVIRTUAL_FONTS_DYN
               if (giscpv_named(codepagep))
                  { /* Load cp info from loaded font data (code page pointer is just a flag) */
                  if ((numelem = gvf_curfont->cprnum) == 0)
                     {
                     codepagep = NULL; /* Font has no codepage */
                     break;
                     }
                  }
               else
               #endif
                  numelem = ((PGCODEPAGEV)codepagep)->cprnum;
               min = 0;
               for(;;)
                  {
                  half = numelem >> 1;   /* half = mid point in sub range */
                  crpv = gi_fontv_cp(min+half); /* Next element to check */
                  if ((c >= crpv->min) && (c <= crpv->max))
                     {
                     /* Found */
                     c = crpv->idx + (c - crpv->min); /* Convert to symbol index */
                     goto c_converted; /* fast skip of all loops */
                     }
                  else
                     {
                     if (half != 0)
                        {
                        /* define next sub range */
                        if (c < crpv->min)
                           numelem = half;        /* too high, continue in lower half */
                        else
                           {
                           min = min+half;        /* too low, continue in upper half */
                           numelem = numelem - half;
                           }
                        if (numelem != 0)        /* subrange exist ? */
                           continue;
                        }

                     /* Not found */
                     #ifdef GVIRTUAL_FONTS_DYN
                     if (giscpv_named(codepagep))
                        /* Load cp info from loaded font data (code page pointer is just a flag) */
                        def_wch = gvf_curfont->def_wch;
                     else
                     #endif
                        def_wch = ((PGCODEPAGEV)codepagep)->def_wch;
                     if (c == def_wch)
                        {
                        /* Default character does not exist (either) */
                        G_WARNING("Illegal codepage default character detected");
                        return NULL;
                        }
                     else
                        {
                        /* Use default character instead */
                        c = def_wch;
                        break; /* Search once more in full range, skip inner loop */
                        }
                     }
                  }
               }
            }
         }
      }
   /* We end here if no codepage is used, or if codepage is not in virtual memory */
   #endif

   if( codepagep != NULL )
      {
      /* Using codepage, convert character to an index via lookup */
      crpb = &codepagep->cpr[0]; /* Codepage element array base */

      if (codepagep->cph.cprnum == 0)
         {
         G_WARNING("Illegal codepage header detected");
         return NULL;
         }

      #ifdef USE_LINEAR_SEARCH
      /* Old linear search method */
      for(;;)
         {
         /* Check ranges */
         for (cnt = 0; cnt < codepagep->cph.cprnum; cnt++)
            {
            if ((c >= crpb->min) && (c <= crpb->max))
               {
               /* Found */
               c = crpb->idx + (c - crpb->min); /* found, return symbol index */
               goto c_converted;                /* fast skip of all loops */
               }
            crpb++;
            }

         /* Not found*/
         if (c == codepagep->cph.def_wch)
            {
            /* Default character does not exist (either) */
            G_WARNING("Illegal codepage default character detected");
            return NULL;
            }

         /* Use default character instead, and try one more */
         c = codepagep->cph.def_wch;
         crpb = &codepagep->cpr[0];
         }

      #else

      /* Use fast lookup
         Much faster with large codepages but require that codepage elements are
         arranged in increasing order (= default ordering for all standard font
         code pages, and for the fonts codepages created with IconEdit) */
      for(;;)
         {
         /* Sub range = full range */
         numelem = codepagep->cph.cprnum;
         min = 0;
         for(;;)
            {
            half = numelem >> 1;   /* half = mid point in sub range */
            crp = &crpb[(SGUINT)min+half]; /* Next element to check */
            if ((c >= crp->min) && (c <= crp->max))
               {
               /* Found */
               c = crp->idx + (c - crp->min); /* Convert to symbol index */
               goto c_converted; /* fast skip of all loops */
               }
            else
               {
               if (half != 0)
                  {
                  /* define next sub range */
                  if (c < crp->min)
                     numelem = half;        /* too high, continue in lower half */
                  else
                     {
                     min = min+half;        /* too low, continue in upper half */
                     numelem = numelem - half;
                     }

                  if (numelem != 0)        /* subrange exist ? */
                     continue;
                  }

               /* Not found */
               if (c == codepagep->cph.def_wch)
                  {
                  /* Default character does not exist (either) */
                  G_WARNING("Illegal codepage default character detected");
                  return NULL;
                  }
               else
                  {
                  /* Use default character instead */
                  c = codepagep->cph.def_wch;
                  break; /* Search once more in full range, skip inner loop */
                  }
               }
            }
         }
      #endif /* USE_LINEAR_SEARCH */
      }
   else
   if ( c > gi_fnumsym(pfont))
      {
      G_WARNING( "gfsymv.c: symbol index larger than font table" );
      return NULL;
      }

   c_converted:
   #ifdef GVIRTUAL_FONTS
   if (gisfontv(pfont))
      return gi_fontv_sym(c); /* Do GFONTV lookup */
   /* Do GFONT lookup */
   #endif

   /* Is a standard font (linear symbol array in linear memory) */
   s = ((GBUFINT) sizeof(GSYMHEAD)) /* Sizeof symbol header */
      + ((GBUFINT) pfont->symsize); /* size of symbol data */
   ps = (PGSYMBYTE) (pfont->psymbols);
   return (PGSYMBOL)(&ps[s*c]);
   }


/********************************************************************
   Segment: Software symbols
   Level: Fonts
   Finds a chars width
   return 0 if symbol not found

   ggetsymw( c ) is mapped to this function
   When GSOFT_FONT is not defined then #define ggetsymw(c) GDISPCW
*/
GXT gi_getsymw( GWCHAR c )
   {
   PGSYMBOL ps;
   #ifdef GDATACHECK
   if (gcurvp == NULL)
      return GDISPCW;
   if (gcurvp->pfont == NULL)
       return GDISPCW;
   #endif

   #ifndef GHW_NO_HDW_FONT
   if( gishwfont() )
      return GDISPCW; /* width of symbol HW fonts*/
   #endif

   ps = gi_getsymbol(c, gcurvp->pfont, gcurvp->codepagep);
   if( ps == NULL )
      return 0;
   return gsymw(ps); /* Width of SW font */
   }

#ifdef GFUNC_VP

GXT gi_getsymw_vp( SGUCHAR vp, GWCHAR c )
   {
   GXT retp;
   GGETFUNCVP(vp, gi_getsymw(c) );
   return retp;
   }

#endif /* GFUNC_VP */


#endif /* GSOFT_FONTS */

