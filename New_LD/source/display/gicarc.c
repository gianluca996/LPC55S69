/************************* gicarc.c ********************************

   Internal function common for gcarc.c and groundrec.c

   Creation date:     05-12-2008
   Revision date:     06-03-2009
   Revision Purpose:  Use of floating point calculations replaced with
                      signed long calculations to support compilers without
                      floating point math.
   Revision date:     26-01-2010
   Revision Purpose:  Extra casts inserted in next point calculations to force
                      use of signed integer promotion rules indpendent of
                      compiler defaults.

   Version number: 1.2
   Copyright (c) RAMTEX Engineering Aps 2008-2010

*********************************************************************/
#include "gi_disp.h"

#ifdef GGRAPHICS

/* Convenient short hand macros for image coordinate mirroring */
#define  ADD(xy1,xy2) (((GXYT)(xy1))+((GXYT)(xy2)))
#define  SUB(xy1,xy2) (((GXYT)(xy1))-((GXYT)(xy2)))
#define  MIRX( e1, e2) ((mx!= 0) ? SUB(e1,e2) : ADD(e1,e2))
#define  MIRY( e1, e2) ((my!= 0) ? SUB(e1,e2) : ADD(e1,e2))
#define  SWPX( e1, e2) ((mx!= 0) ? (e1) : (e2))


void gi_carc(GXT xc, GYT yc, GXYT r, SGUCHAR arctype)
   {
   GXYT x,x1;
   GXYT y,y1;
   SGUCHAR mx,my,fill,line;
   SGBOOL colf,coll;
   SGLONG p;

   line = ((arctype & 0x1)!=0) ? 0 : 1; /* GLINE or GFRAME */
   fill = ((arctype & 0x2)!=0) ? 1 : 0; /* GFILL or GFRAME */
   coll = G_IS_INVERSE() ? 0 : 1;
   colf = G_IS_INVERSE() ? 1 : 0;

   /* Process one or more arc types */
   for(;;)
      {
      if ((arctype & GCARC_LT)!=0)
         {
         arctype &= ~GCARC_LT;
         my=1;
         mx=1;
         }
      else
      if ((arctype & GCARC_LB)!=0)
         {
         arctype &= ~GCARC_LB;
         my=0;
         mx=1;
         }
      else
      if ((arctype & GCARC_RT)!=0)
         {
         arctype &= ~GCARC_RT;
         my=1;
         mx=0;
         }
      else
      if ((arctype & GCARC_RB)!=0)
         {
         arctype &= ~GCARC_RB;
         my=0;
         mx=0;
         }
      else
         return;

      /* Start on new arc */
      x = 0;
      y = r;
      p = (SGLONG)((SGINT)1 - (SGINT)r);
      x1 = x;
      y1 = y;

      /* Calculate a 45 degree angle, and mirror the rest */
      for(;;)
         {
         if (line)
            {
            if (y != x)
               {
               ghw_setpixel((GXT)MIRX(xc,x),(GYT)MIRY(yc,y),coll);
               if (fill && (y != y1))
                  {
                  /* Fill using horizontal lines, so check on vertical move */
                  ghw_rectangle((GXT)SWPX(SUB(xc,x-1),xc),
                                (GYT)MIRY(yc,y),
                                (GXT)SWPX(xc,ADD(xc,x-1)),
                                (GYT)MIRY(yc,y),colf);
                  y1=y;
                  }
               }
            ghw_setpixel((GXT)MIRX(xc,y),(GYT)MIRY(yc,x), coll);
            if (fill)
               ghw_rectangle((GXT)SWPX(SUB(xc,y-1),xc),
                             (GYT)MIRY(yc,x),
                             (GXT)SWPX(xc,ADD(xc,y-1)),
                             (GYT)MIRY(yc,x),colf);
            if (x+1 >= y)
               break;
            }
         else
            {
            ghw_rectangle((GXT)SWPX(SUB(xc,y),xc),
                          (GYT)MIRY(yc,x),
                          (GXT)SWPX(xc,ADD(xc,y)),
                          (GYT)MIRY(yc,x),colf);
            if (y != y1)
               {
               finalize:
               ghw_rectangle((GXT)SWPX(SUB(xc,x1),xc),
                          (GYT)MIRY(yc,y1),
                          (GXT)SWPX(xc,ADD(xc,x1)),
                          (GYT)MIRY(yc,y1),colf);
               y1=y;
               }
            x1=x;
            if (x+1 >= y)
               {
               if (x!=y)
                  {
                  x=y;
                  goto finalize;
                  }
               break;
               }
            }

         /* Calculate next arc point */
         if( p < 0 )
            {
            x = x + 1;
            p = p + ((SGLONG)2)*((SGLONG)x) + 1;
            }
         else
            {
            x = x + 1;
            y = y - 1;
            p = p + ((SGLONG)2)*((SGLONG)((SGINT)x - (SGINT)y)) + 1;
            }
         }
      }
   }

#endif /* GGRAPHICS */


