/************************* gsymget.c *******************************

   Creation date: 980223

   Revision date:     03-01-26
   Revision Purpose:  Bit mask on GINVERSE mode
   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added
   Revision date:     7-01-05
   Revision Purpose:  Pointer changed from PGSYMBOL to a GSYMBOL* type
                      (PGSYMBOL is now equal to 'const GSYMBOL *')
   Revision date:     15-05-12
   Revision Purpose:  Optimized to use the GSYM_SIZE(w,h) macro
                      The old gsymsize(..) gsymsize_vp(..) is converted to macros.
                      Use of the old gsymsize_vp(..) is deprecated.
   Revision date:
   Revision Purpose:

   Version number: 2.3
   Copyright (c) RAMTEX Engineering Aps 1998-2012

*********************************************************************/
#include "gi_disp.h" /* gLCD prototypes */

#ifdef GSOFT_SYMBOLS

/*******************************************************************
   Segment: Software symbols
   Level: Graphics
   Get box area into symbol buffer.
   The area defined by the input parameters is read from
   the display into sym. The area allocated for symbol must
   be large enough to hold all the data + the GSYMHEAD struct.
   The GSYMHEAD is updated to reflect the actual size
   (which may have been limited by view-port settings or the display size)
   Display information fetched with ggetsym can be written back with
   gfillsym or gputsym.
   See also gsymsize()

   Note: This functions requires a compiler with standard C
   conformant pointer handling. Ex that the same function pointer
   parameter can point both to objects located in ROM and to objects
   located in RAM
*/
void ggetsym(GXT xs, GYT ys, GXT xe, GYT ye, GSYMBOL * psym, GBUFINT size )
   {
   GXT w;
   GYT h;
   gi_datacheck(); /* check internal data for errors */
   /* normalize to view-port */
   xs = xs + gcurvp->lt.x;
   ys = ys + gcurvp->lt.y;
   xe = xe + gcurvp->lt.x;
   ye = ye + gcurvp->lt.y;

   /* limit values to view-port */
   LIMITTOVP( "ggetsym",xs,ys,xe,ye );
   if( psym == NULL )
      {
      G_WARNING( "ggetsym: parameter, psym == NULL" );
      return;
      }

   glcd_err = 0; /* Reset HW error flag */
   h = (ye-ys)+1;
   w = (xe-xs)+1;
   if( size < GSYM_SIZE(w,h) )
      {
      G_WARNING( "ggetsym: parameter, buffer too small" );
      return;
      }

   /* save w,h */
   psym->sh.cypix = h;
   psym->sh.cxpix = w;
   w = (w+7)/8; /* Number of storage bytes pr row */
   ghw_rdsym(xs, ys, xe, ye, (PGUCHAR)(&(((PGBWSYMBOL)((void*)psym))->b[0])), w, (SGUCHAR) G_IS_INVERSE() );
   }

#ifdef GFUNC_VP

void ggetsym_vp( SGUCHAR vp, GXT xs, GYT ys, GXT xe, GYT ye, GSYMBOL * psym, GBUFINT size )
   {
   GSETFUNCVP(vp, ggetsym(xs,ys,xe,ye,psym,size) );
   }

#endif /* GFUNC_VP */
#endif /* GSOFT_SYMBOLS */

