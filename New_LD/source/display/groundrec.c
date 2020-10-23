/************************* ggrect.c ********************************

   Draw rounded rectangle.
   Any morfing between an rectangle and a circle can be draw.
   If radius = 0 a retangle figure is drawn
   For a quadratic figure with r >= width/2 the figure
   will be a circle.

   With the arc skipflags a normal 90 degree corner can be selected
   instead for the specified corner:
      GCARC_LT   left-top corner
      GCARC_LB   left-bottom corner
      GCARC_RT   right top corner
      GCARC_RB   right bottom corner.

   Drawing types selected (or'ed with skipflags) :
     GLINE  Perimeter line with foreground color (default)
     GFILL  Inner area filled with background info
     GFRAME Combination of above

   Creation date: 22-11-2008

   Revision date:     07-05-2009
   Revision Purpose:  Viewport offset corrected
   Revision date:     16-01-2012
   Revision Purpose:  Corrected drawing bug with some arch skip combinations.

   Version number: 1.2
   Copyright (c) RAMTEX Engineering Aps 2008-2012

*********************************************************************/
#include "gi_disp.h"

#ifdef GGRAPHICS
/*
   Draw rounded rectangle

   The rounded rectangle is composed by 4 (0-4) arc areas LT,LB,RT,RB
   and 3 (0-3) rectangular areas: MIDT, LC, RC. If one or more of the
   arc areas are not used then the rectangular areas are optimized
   accordingly. Recangular areas of size zero is skipped (ex when the
   combined arcs form a regular circle)

      **********
     * *      * *
    *LT*      *RT*
    ****      ****
    *LC* MIDT *RC*
    ****      ****
    *LB*      *RB*
     * *      * *
      **********
*/
void groundrect( GXT ltx, GYT lty, GXT rbx, GYT rby, GXYT r, SGUCHAR skipflags)
   {
   GYT ycb,yce;
   GXT xb,xe;
   GYT yb,ye;
   SGBOOL coll;
   SGUINT pattern;
   SGUCHAR fill,line;

   gi_datacheck();

   /* normalize to view-port */
   ltx = ltx + gcurvp->lt.x;
   lty = lty + gcurvp->lt.y;
   rbx = rbx + gcurvp->lt.x;
   rby = rby + gcurvp->lt.y;

   /* limit values to view-port */
   LIMITTOVP( "groundrect",ltx,lty,rbx,rby );

   glcd_err = 0; /* Reset HW error flag */


   /* Check special cases */
   line = (skipflags & 0x1) ? 0 : 1; /* GLINE or GFRAME */
   fill = (skipflags & 0x2) ? 1 : 0; /* GFILL or GFRAME */
   if (!line && !fill)
      return;  /* Neither frame nor filled area -> no output, skip drawing */

   xb  = (rbx-ltx+1);
   yb = (rby-lty+1);
   if ((xb <= 2) || (yb <= 2) || ((skipflags & GCARC_ALL) == GCARC_ALL))
      r = 0; /* All corner arcs are to be skipped = rectangle */
   else
      {
      /* Check / limit radius (maximum is two half circles or one full circle */
      if ((GXYT)(2*r) > (GXYT)xb)
         r=(GXYT)(xb/2) + (r&1);
      if ((GXYT)(2*r) > (GXYT)yb)
         r=(GXYT)(yb/2) + (r&1);
      }

   /* Set colors */
   pattern = G_IS_INVERSE() ? 0xffff : 0x0000;
   coll = G_IS_INVERSE() ? 0 : 1;

   if (r <= 0)
      {
      /* Speed optimized for single rectangular area */
      if (line)
         {
         ghw_rectangle( ltx,lty,rbx,rby, coll );
         if ((rbx-ltx <= 1) || (rby-lty <= 1))  /* No inner area */
            return;
         ltx++;lty++;rbx--;rby--; /* Limit fill to inner area */
         }
      if (fill)
         ghw_fill(ltx,lty,rbx,rby,pattern);
      ghw_updatehw();
      return;
      }

   /* Set partial left areas */
   ycb = lty;
   yce = rby;
   xb = ltx;
   if ((skipflags & (GCARC_LT|GCARC_LB)) != (GCARC_LT|GCARC_LB))
      { /* Some arcs on left side */
      xe = ltx+r;
      if ((skipflags & GCARC_LT) == 0)
         {  /* Upper left arc */
         yb = lty;
         ye = (GYT)(lty+r);
         gi_carc(xe, ye, r, (SGUCHAR)(GCARC_LT | (skipflags & GFILL)));
         ycb = (GYT)(ye+1);
         }
      if ((skipflags & GCARC_LB) == 0)
         {  /* Lower left arc */
         ye = rby;
         yb = (GYT)(rby-r);
         gi_carc(xe, yb, r, (SGUCHAR)(GCARC_LB | (skipflags & GFILL)));
         yce = (GYT)(yb-1);
         }
      if (yce>=ycb)
         {
         /* Left rectangle */
         if (line)
            {
            if (skipflags & GCARC_LT)
               { /* Upper left line */
               ghw_rectangle(xb,ycb,xe,ycb, coll );
               ycb++;
               }
            if (skipflags & GCARC_LB)
               {  /* Lower left line */
               ghw_rectangle(xb,yce,xe,yce, coll );
               yce--;
               }
            if (yce>=ycb)
               {  /* left vertical line*/
               ghw_rectangle(xb,ycb,xb,yce, coll );
               xb++;
               }
            }
         if (fill)
            {  /* Fill left rectangle */
            if ((yce>=ycb) && (xe>=xb))
               ghw_fill(xb,ycb,xe,yce, pattern );
            }
         }
      xb = xe+1;
      }
   else
      {
      xb = ltx;
      if (line)
         {
         ghw_rectangle(xb,lty,xb,rby, coll );
         xb++;
         }
      }
   /* Midter rectangle */
   xe = ((skipflags & (GCARC_RT|GCARC_RB)) != (GCARC_RT|GCARC_RB)) ? (rbx-r)-1 : rbx;
   if (xe>=xb)
      {
      ycb = lty;
      yce = rby;
      if (line)
         {
         ghw_rectangle(xb,ycb,xe,ycb, coll );
         ycb++;
         ghw_rectangle(xb,yce,xe,yce, coll );
         yce--;
         }
      if (fill)
         {  /* Fill rectangle */
         if (yce>=ycb)
            ghw_fill(xb,ycb,xe,yce, pattern );
         }
      }

   /* Set partial right areas */
   ycb = lty;
   yce = rby;
   xe = rbx;
   if ((skipflags & (GCARC_RT|GCARC_RB)) != (GCARC_RT|GCARC_RB))
      { /* Some arcs on right side */
      xb = rbx-r;
      if ((skipflags & GCARC_RT) == 0)
         {  /* Upper right arc */
         yb = lty;
         ye = (GYT)(lty+r);
         gi_carc(xb, ye, r, (SGUCHAR)(GCARC_RT | (skipflags & GFILL)));
         ycb = (GYT)(ye+1);
         }
      if ((skipflags & GCARC_RB) == 0)
         {  /* Lower right arc */
         yb = (GYT)(rby-r);
         gi_carc(xb, yb, r, (SGUCHAR)(GCARC_RB | (skipflags & GFILL)));
         yce = (GYT)(yb-1);
         }
      if (yce>=ycb)
         {
         /* Left rectangle */
         if (line)
            {
            if (skipflags & GCARC_RT)
               { /* Upper left line */
               ghw_rectangle(xb,ycb,xe,ycb, coll );
               ycb++;
               }
            if (skipflags & GCARC_RB)
               {  /* Lower left line */
               ghw_rectangle(xb,yce,xe,yce, coll );
               yce--;
               }
            if (yce>=ycb)
               {  /* left vertical line*/
               ghw_rectangle(xe,ycb,xe,yce, coll );
               xe--;
               }
            }
         if (fill)
            {  /* Fill left rectangle */
            if ((yce>=ycb) && (xe>=xb))
               ghw_fill(xb,ycb,xe,yce, pattern );
            }
         }
	  }
   else
      {
      if (line)
         {
         ghw_rectangle(xe,ycb,xe,yce, coll );
         }
      }
   ghw_updatehw();
   }

#ifdef GFUNC_VP
void groundrect_vp( SGUCHAR vp, GXT ltx, GYT lty, GXT rbx, GYT rby, GXYT r, SGUCHAR skipflags)
   {
   GSETFUNCVP(vp, groundrect( ltx, lty, rbx, rby, r, skipflags));
   }
#endif /* GFUNC_VP */
#endif /* GGRAPHICS */


