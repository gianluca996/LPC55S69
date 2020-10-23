/*************************  gsymrotate.c  **************************

   Draw a symbol rotated optionally using a rotation ancker different
   from symbol center

   void gsymrotate( GXT x, GYT y,
                     float angle,
                     PGSYMBOL sym,
                     SGINT xancher, SGINT yancher,
                     GMODE symflag)

   This function rotates a bitmap to any angle.
      sym            Symbol to write in an rotated fashion
      angle          Angle to rotate symbol in radians.
      x,y            The viewport destination point for the symbol ancker
                     (0,0 is upper left corner of viewport)
      xancher, yancher The ancher point for symbol rotation relative to symbol
                     connection point. (0,0) => rotation around connection point.
                     The symbol connection point is defined by the flags
      symflags       Define symbol output mode flags

        GALIGN_LEFT    Symbol connection point is aligned to the left edge
        GALIGN_RIGHT   Symbol connection point is aligned to the right edge
        GALIGN_HCENTER Symbol connection point is centered horizontally
        GALIGN_TOP     Symbol connection point is aligned to the top
        GALIGN_BOTTOM  Symbol connection point is aligned to the bottom
        GALIGN_VCENTER Symbol connection point is centered vertically
        GTRANSPERANT   Enable transperant mode for output
        GSYMCLR        Set exact area covered by symbol to viewport background
                       or foreground color depending on GINVERSE
                       (i.e. do an exact clear of a rotated symbol drawn earlier)
        GINVERSE       Enable inverse fill or inverse transperance

    Nine possible symbol connection alignment points
     ___
    *   |    GALIGN_TOP | GALIGN_LEFT
    |   |
    |___|
     ___
    | * |    GALIGN_TOP | GALIGN_HCENTER
    |   |    or just GALIGN_TOP
    |___|
     ___
    |   *    GALIGN_TOP | GALIGN_RIGHT
    |   |
    |___|

     ___
    |   |
    *   |    GALIGN_VCENTER | GALIGN_LEFT
    |___|    or just GALIGN_LEFT
     ___
    |   |
    | * |    GALIGN_VCENTER | GALIGN_HCENTER
    |___|    (or just 0 to use default)
     ___
    |   |
    |   *    GALIGN_VCENTER | GALIGN_RIGHT
    |___|    or just GALIGN_RIGHT

     ___
    |   |
    |   |
    *___|    GALIGN_BOTTOM | GALIGN_LEFT
     ___
    |   |
    |   |    GALIGN_BOTTOM | GALIGN_HCENTER
    |_*_|    or just GALIGN_BOTTOM
     ___
    |   |
    |   |
    |___*    GALIGN_BOTTOM | GALIGN_RIGHT


   Creation date:  13-11-2012

   Revision Purpose:
   Revision date:

   Version number: 1.0
   Copyright (c) RAMTEX Engineering Aps 2012

*********************************************************************/
#include <math.h>  /* Sinus and cosinus functions */
#include "gfixedpt.h"
#include "gi_disp.h"

#ifdef GSOFT_SYMBOLS
SGFIXP gi_fp_sin;  /* sin values as a fractional (16:16) number 65536==1.0 */
SGFIXP gi_fp_cos;  /* cosine values as a fractional (16:16) number 65536==1.0 */

/*
   Prepare sin and cos values and convert to fix point numbers
   for faster processing, i.e. for the rest of the rotated symbol processing
   avoid floating point calculations and use fast integer calculations instead
*/
void gi_setfpsincos( float angle )
   {
   gi_fp_sin = SGFLOAT_TO_FIXP(sin(angle));
   gi_fp_cos = SGFLOAT_TO_FIXP(cos(angle));

   /* Snap to axis when angle is close to ortogonal (give higher processing speed) */
   if (SGFIXP_ABS(gi_fp_sin) <= SGFIXP_SCALE/GDISPW)
      {
      gi_fp_sin = 0;
      gi_fp_cos = SGI_TO_FIXP((gi_fp_cos > 0) ? 1 : -1);
      }
   else
   if (SGFIXP_ABS(gi_fp_cos) <= SGFIXP_SCALE/GDISPH)
      {
      gi_fp_cos = 0;
      gi_fp_sin = SGI_TO_FIXP((gi_fp_sin > 0) ? 1 : -1);
      }
   }

/*
  Fill line in destination rectangle with data from source symbol.
  The routine make a simplified flood fill to copy every pixel in the
  destination line from the source symbol. There are no guarantees that
  every pixel from the source is used, but every pixel in the destination
  is updated.
  cpypixline tries to go as far left and right in the destination as the source
  rectangle permits. While doing this, it copies the pixels from the source
  to the destination.
*/
static SGUCHAR cpypixline(GSYM_PROCESS *sp, SGINT xp, SGINT yp)
   {
   SGFIXP txo,tyo;
   SGINT x;
   SGUCHAR outside;
   outside = 1;
   txo = sp->xo;
   tyo = sp->yo;
   /* Copy to left */
   x = xp;
   for(;;)
      {
      if (x > gcurvp->rb.x)
         break;
      if (x >= gcurvp->lt.x)
         {
         if (gi_getsym_pixel(sp))
            {
            if (outside == 0)
               break; /* Symbol indside out */
            if (++outside > 2) /* Try one more time to catch symbol */
               break; /* Completely outside */
            }
         outside = 0;
         if (sp->a != 0) /* If not transperant print pixel */
            ghw_setpixel( (GXT)((SGUINT)x), (GYT)((SGUINT)yp), sp->c);
         }
      x++;                  /* Move destination right */
      sp->xo += gi_fp_cos;  /* Same move within source */
      sp->yo += gi_fp_sin;
      }

   /* Copy to right */
   x = xp-1;
   sp->xo = txo - gi_fp_cos;
   sp->yo = tyo - gi_fp_sin;
   for(;;)
      {
      if (x < gcurvp->lt.x)
         break;
      if (x <= gcurvp->rb.x )
         {
         if (gi_getsym_pixel(sp))
            { /* Outside symbol */
            if (outside == 0)
               break; /* Symbol inside out, skip at once */
            if (++outside > 2) /* Outside in, try to catch edge */
               break;
            }
         outside = 0;
         if (sp->a != 0) /* If not transperant print pixel  */
            ghw_setpixel( (GXT)((SGUINT)x), (GYT)((SGUINT)yp), sp->c);
         }
      x--;                  /* Move destination left */
      sp->xo -= gi_fp_cos;  /* Same move within source */
      sp->yo -= gi_fp_sin;
      }
   sp->xo = txo;   /* Restore for next line processing */
   sp->yo = tyo;
   return outside;
   }

/* Macros for packing / unpacking drawing endpoint info for connection points
   (packs info in two signed integers into one unsigned char) */
#define PK(x,y) (((((SGUCHAR)((x)+2))<<4)&0xf0) | ((((SGUCHAR)((y)+2)))&0xf)) /* Compile time packing */
#define UPKX(xy) (((SGINT)(((xy)>>4)&0xf))-2) /* unpack x info */
#define UPKY(xy) (((SGINT)((xy)&0xf))-2)      /* unpack y info */

/* The table describes relative distance from a connection point to the symbol corner
   which will be the highest and lowest point respectively for a rotation within
   the specific coordinate system quadrant. Simplifies drawing calculations below */
static GCODE SGUCHAR FCODE fpos[9*4*2] =
   {
   PK( 2, 0),PK( 0, 2), PK( 2, 2),PK( 0, 0), PK( 0, 2),PK( 2, 0), PK( 0, 0),PK( 2, 2),  /*  left, top       */
   PK( 1, 0),PK(-1, 2), PK( 1, 2),PK(-1, 0), PK(-1, 2),PK( 1, 0), PK(-1, 0),PK( 1, 2),  /*  hcenter, top    */
   PK( 0, 0),PK(-2, 2), PK( 0, 2),PK(-2, 0), PK(-2, 2),PK( 0, 0), PK(-2, 0),PK( 0, 2),  /*  right, top      */
   PK( 2,-1),PK( 0, 1), PK( 2, 1),PK( 0,-1), PK( 0, 1),PK( 2,-1), PK( 0,-1),PK( 2, 1),  /*  left, vcenter   */
   PK( 1,-1),PK(-1, 1), PK( 1, 1),PK(-1,-1), PK(-1, 1),PK( 1,-1), PK(-1,-1),PK( 1, 1),  /*  hcenter, vcenter*/
   PK( 0,-1),PK(-2, 1), PK( 0, 1),PK(-2,-1), PK(-2, 1),PK( 0,-1), PK(-2,-1),PK( 0, 1),  /*  right, vcenter  */
   PK( 2,-2),PK( 0, 0), PK( 2, 0),PK( 0,-2), PK( 0, 0),PK( 2,-2), PK( 0,-2),PK( 2, 0),  /*  left, bottom    */
   PK( 1,-2),PK(-1, 0), PK( 1, 0),PK(-1,-2), PK(-1, 0),PK( 1,-2), PK(-1,-2),PK( 1, 0),  /*  hcenter, bottom */
   PK( 0,-2),PK(-2, 0), PK( 0, 0),PK(-2,-2), PK(-2, 0),PK( 0,-2), PK(-2,-2),PK( 0, 0)   /*  right, bottom   */
   };


/*

   Output symbol rotated,
      Process anchor point offsets, anchor symbol connection point offset, a outmode settings

   gi_fp_sin, gi_fp_cos must have been preset before calling this function.
*/
void gi_symrotate( SGFIXP x, SGFIXP y, PGSYMBOL psymbol,
                   SGFIXP xanchor, SGFIXP yanchor,
                   GMODE symflag, SGUINT symbytewidth)
   {
   GSYM_PROCESS sp; /* Symbol processing structure */
   SGINT py,px,yend,displacement;
   SGFIXP sx, sy,xstep, xo, yo;
   SGUCHAR connectpt,updown;
   #ifdef GBUFFER
   GUPDATE update;
   #endif

   /* Prepare symbol processing structure */
   if (gi_getsym_open(&sp, psymbol, symbytewidth, symflag))
      return; /* Symbol pointer error */

   #ifdef GBUFFER
   /* Use completion of drawing before flush to screen */
   update = gsetupdate(GUPDATE_OFF);
   #endif

   if ((xanchor != 0) || (yanchor != 0))
      {
      /* Calculate connection point position change caused by rotation around anchor */
      x += (gi_fp_cos*xanchor + gi_fp_sin*yanchor)/SGFIXP_SCALE;
      y += (gi_fp_cos*yanchor - gi_fp_sin*xanchor)/SGFIXP_SCALE;
      }
 
   /* Set connection point position in symbol +
      index for lookup of farest corner position */
   switch (symflag & GALIGN_VCENTER)
      {
      case GALIGN_BOTTOM:
         connectpt = 6*4*2;
         yo = sp.symh-SGU_TO_FIXP(1);
         break;
      case GALIGN_TOP:
         connectpt = 0;
         yo = 0;
         break;
      default: /* GALIGN_VCENTER (or default) */
         connectpt = 3*4*2;
         yo = (sp.symh-SGU_TO_FIXP(1))/2;
         break;
      }

   switch (symflag & GALIGN_HCENTER)
      {
      case GALIGN_RIGHT:
         xo = sp.symw-SGU_TO_FIXP(1);
         connectpt += 2*4*2;
         break;
      case GALIGN_LEFT: /* Specifed anker point is relative to left edge*/
         xo = 0;
         break;
      default: /* GALIGN_HCENTER (or default) */
         connectpt += 1*4*2;
         xo = (sp.symw-SGU_TO_FIXP(1))/2;
         break;
      }
   /* Add kvadrant offset */
   connectpt += (gi_fp_sin >= 0) ? ((gi_fp_cos >= 0) ? 0:2) : ((gi_fp_cos >= 0) ? 6:4);

   /* Do drawing in two passes, from connection point and up, and from connection point down, */
   for (updown = 0; updown < 3; updown++)
      {
      /* Do lookup and calculate farest y points */
      sx  = (SGI_TO_FIXP(UPKX(fpos[connectpt]))*sp.symw)/SGI_TO_FIXP(2);
      sy  = (SGI_TO_FIXP(UPKY(fpos[connectpt++]))*sp.symh)/SGI_TO_FIXP(2);
      xanchor = (gi_fp_cos*sx + gi_fp_sin*sy)/SGFIXP_SCALE; /* Offset farest after rotation */
      yanchor = (gi_fp_cos*sy - gi_fp_sin*sx)/SGFIXP_SCALE;
      xstep = (yanchor == 0) ? xanchor : (xanchor*SGFIXP_SCALE) / yanchor;
      if (updown == 0)
         {
         yend = SGFIXP_TO_I(y+yanchor); /* Round down */
         xstep *=-1;
         }
      else
         yend = SGFIXP_TO_I(y+yanchor+SGFIXP_SCALE/2); /* Round up */
      py = SGFIXP_TO_I(y);
      sx = x;
      px = SGFIXP_TO_I(x);
      sp.xo = xo;
      sp.yo = yo;
      for(;;)
         {
         if (updown == 0)
            { /* Draw lines at connection point and up */
            if (py < gcurvp->lt.y)
               break; /* Moved outside viewport */
            if (py <= gcurvp->rb.y)
               { /* Symbol (part) inside viewport */
               if (cpypixline(&sp,px,py))
                  break; /* Outside symbol or drawing boundary */
               }
            if ( --py < yend)
               break;
            sp.xo += gi_fp_sin; /* Same move in source bitmap coordinates */
            sp.yo -= gi_fp_cos;
            }
         else
            { /* Draw lines down from connection point */
            if (py > gcurvp->rb.y)
               break; /* Moved outside viewport */
            if (updown == 1)
               updown++; /* skip center line which is already drawn, just move position */
            else
            if (py >= gcurvp->lt.y)
               { /* Symbol (part) inside viewport */
               if (cpypixline(&sp,px,py))
                  break; /* Outside symbol or drawing boundary */
               }
            if ( ++py >  yend)
               break; /* Last line reached */
            sp.xo -= gi_fp_sin;   /* Same move in source bitmap coordinates */
            sp.yo += gi_fp_cos;
            }
         /* Update line drawing start center*/
         sx += xstep;
         if ((displacement = SGFIXP_TO_I(sx)-px) != 0)
            {  /* Move x start towards farest symbol point */
            px += displacement;
            sp.xo += gi_fp_cos*displacement;
            sp.yo += gi_fp_sin*displacement;
            }
         }
      }

   #ifdef GBUFFER
   /* Flush changes to screen */
   gsetupdate(update);
   #endif
   }

#if defined( GBASIC_TEXT ) && defined( GSOFT_FONTS )

/*
   Internal function for write of character symbol to position using a
   rotated angle
   The anker position is the lower left corner of the character symbol.
   The current font and viewport is used

   Returns width of symbol
*/
SGFIXP gi_putchw_rotate( GWCHAR ch, SGFIXP x, SGFIXP y )
   {
   GMODE mode;
   PGSYMBOL psymbol;
   /* Output symbol using rotated mode, with anker in lower left symbol corner */
   mode = (GMODE) ((((unsigned int) gcurvp->mode) & (GSYMCLR|GINVERSE|GTRANSPERANT))|(GALIGN_BOTTOM|GALIGN_LEFT));
   /* Get symbol for character */
   psymbol = gi_getsymbol( ch, gcurvp->pfont, gcurvp->codepagep);
   if (psymbol != NULL)
      {
      gi_symrotate(x, y, psymbol,0,0,mode, (SGUINT)(gi_fsymsize(gcurvp->pfont)/gi_fsymh(gcurvp->pfont)));
          /* return width */
      #ifdef GNOTXTSPACE
      return SGU_TO_FIXP(gsymw(psymbol));
      #else
      return SGU_TO_FIXP(gsymw(psymbol) + gcurvp->chln.x);
      #endif
      }
   return (SGFIXP) 0;
   }
#endif

void gputsymrot( SGINT x, SGINT y, float angle, PGSYMBOL psymbol,
                 SGINT xanchor, SGINT yanchor, GMODE symflag)
   {
   gi_setfpsincos(angle); /* Prepare for fixed point calculations */
   gi_symrotate(SGU_TO_FIXP(x+(SGINT)((SGUINT)gcurvp->lt.x)),
                SGU_TO_FIXP(y+(SGINT)((SGUINT)gcurvp->lt.y)),
                psymbol,SGU_TO_FIXP(xanchor),SGU_TO_FIXP(yanchor),symflag,0);
   ghw_updatehw();
   }

#ifdef GFUNC_VP

void gputsymrot_vp( SGUCHAR vp, SGINT x, SGINT y, float angle, PGSYMBOL psymbol,
                    SGINT xanchor, SGINT yanchor, GMODE symflag)
   {
   gi_setfpsincos(angle); /* Prepare for fixed point calculations */
   GSETFUNCVP(vp, gi_symrotate(SGU_TO_FIXP(x+(SGINT)((SGUINT)gcurvp->lt.x)),
                               SGU_TO_FIXP(y+(SGINT)((SGUINT)gcurvp->lt.y)),
                  psymbol,xanchor,yanchor,symflag,0));
   ghw_updatehw();
   }

#endif

#endif /* GSOFT_SYMBOLS */

