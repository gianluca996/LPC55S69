/************************* ggline.c ********************************

   Creation date: 980224

   Revision date:    03-01-26
   Revision Purpose: Bit mask on GINVERSE mode
   Revision date:    03-02-11
   Revision Purpose: Type conversion from unsigned char to signed int
                     avoided in gi_line(). if tests used instead to
                     be more generic.
   Revision date:     13-08-04
   Revision Purpose:  Named viewport function _vp added

   Revision date:
   Revision Purpose:

   Version number: 2.3
   Copyright (c) RAMTEX Engineering Aps 1998-2004

*********************************************************************/
#include "gi_disp.h" /* LCD prototypes */

#ifdef GGRAPHICS

/*******************************************************************
   Segment: Graphics
   Level: Graphics
   Sets current graphics pixel point in view-port
   Default current point is define as 0,0
*/
void gmoveto( GXT xs, GYT ys )
   {
   gi_datacheck(); /* check internal data for errors */

   /* normalize to view-port */
   gcurvp->ppos.x = xs + gcurvp->lt.x;
   gcurvp->ppos.y = ys + gcurvp->lt.y;

   gi_calcdatacheck(); /* correct VP to new settings */
   }

/********************************************************************
   Segment: Graphics
   Level: Graphics
   Draw line from current point to this point and set new current point
   Default current point is define as 0,0
*/
void glineto( GXT xe, GYT ye )
   {
   GXT xs; /* Pos. in view-port */
   GYT ys;

   gi_datacheck(); /* check internal data for errors */

   /* normalize to view-port */
   xe = xe + gcurvp->lt.x;
   ye = ye + gcurvp->lt.y;

   xs = gcurvp->ppos.x;
   ys = gcurvp->ppos.y;
   
   
   gcurvp->ppos.x = xe;
   gcurvp->ppos.y = ye;

   glcd_err = 0; /* Reset HW error flag */
   if( (xs == xe) || (ys == ye) )  /* accelerated line drawing */
      {
      if( xs < gcurvp->lt.x ) /* limit to VP */
         xs = gcurvp->lt.x;
      if( xe > gcurvp->rb.x ) /* limit to VP */
         xe = gcurvp->rb.x;
      if( ys < gcurvp->lt.y ) /* limit to VP */
         ys = gcurvp->lt.y;
      if( ye > gcurvp->rb.y ) /* limit to VP */
         ye = gcurvp->rb.y;
      if( xs > xe )
         {
         GXT t;
         t = xs;
         xs = xe;
         xe = t;
         }
      if( ys > ye )
         {
         GYT t;
         t = ys;
         ys = ye;
         ye = t;
         }
      ghw_rectangle( xs, ys , xe, ye, (SGBOOL)(G_IS_INVERSE() ?  0 : 1));
      }
   else
      gi_line( xs, ys , xe, ye );

   ghw_updatehw();
   gi_calcdatacheck(); /* correct VP to new settings */
   }

/********************************************************************/
/* internal functions */
void gi_line( GXT x, GYT y, GXT x2, GYT y2 )
   {
   SGINT wid,deltax1,deltax2,diaginc,loop;
   SGINT hei,deltay1,deltay2,nondiag;
   SGINT d, temp;
   SGINT x1,y1;

   /* Make safe difference calculation and conversion to signed int. */
   x1 = (SGINT) ((SGUINT) x);
   y1 = (SGINT) ((SGUINT) y);

   if (x2 >= x)
      wid = (SGINT)((SGUINT) (x2 - x));
   else
      {
      wid = (SGINT)((SGUINT) (x - x2));
      wid *= -1;
      }

   if (y2 >= y)
      hei = (SGINT) ((SGUINT) (y2 - y));
   else
      {
      hei = (SGINT) ((SGUINT) (y - y2));
      hei *= -1;
      }

   if( wid < 0 )
      {
      wid = -wid;
      deltax1 = -1;
      }
   else
      deltax1 = 1;

   if( hei < 0 )
      {
      hei = -hei;
      deltay1 = -1;
      }
   else
      deltay1 = 1;

   if( wid < hei )
      {
      /* swap wid and hei */
      temp = wid;
      wid = hei;
      hei = temp;
      deltax2 = 0;
      deltay2 = deltay1;
      }
   else
      {
      deltax2 = deltax1;
      deltay2 = 0;
      }

   nondiag = hei * 2;
   d = nondiag - wid;
   diaginc = d - wid;

   for( loop=0; loop <= wid; loop++ )
      {
      x = (GXT) x1;
      y = (GYT) y1;
      if( !(x < gcurvp->lt.x ||  /* if outside do not draw */
         x > gcurvp->rb.x ||
         y < gcurvp->lt.y ||
         y > gcurvp->rb.y) )
         ghw_setpixel( x,y, (SGBOOL) (G_IS_INVERSE() ? 0 : 1) );

      if( d < 0 )
         {
         x1 = x1 + deltax2;
         y1 = y1 + deltay2;
         d = d + nondiag;
         }
      else
         {
         x1 = x1 + deltax1;
         y1 = y1 + deltay1;
         d = d + diaginc;
         }
      }
   }

#ifdef GFUNC_VP

void gmoveto_vp( SGUCHAR vp, GXT xs, GYT ys )
   {
   GSETFUNCVP(vp, gmoveto(xs,ys) );
   }

void glineto_vp( SGUCHAR vp, GXT xe, GYT ye )
   {
   GSETFUNCVP(vp, glineto(xe,ye) );
   }

#endif /* GFUNC_VP */



#endif /* GGRAPHICS */

