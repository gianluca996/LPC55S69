#ifndef GVPAPP_H
#define GVPAPP_H

#ifdef GEXTMODE
/*
   The application programmer can append application specific data to each
   library viewport by defining GEXTMODE in gdispcfg.h
   The GVPAPP data structure defined below is then added to each viewport.
   When GEXTMODE is defined the GVPAPP structure must contain at least one
   data member.

   You can get a pointer to the application specific data when you have the
   viewport number, or get a the viewport number if you have a (valid) pointer 
   to the GVPAPP structure:
      PGVPAPP ggetapp( void )          Get pointer to GVPAPP for current viewport
      PGVPAPP ggetapp_vp( SGUCHAR vp ) Get pointer to GVPAPP for named viewport
      SGUCHAR ggetvpapp( PGVPAPP app ) Get viewport number for PGVPAPP

   The grapich library itself does not use any data members of the GVPAPP
   structure. It only refer to the structure via a pointer. You may
   therefore define the structure content as you like to fit your application.

   Note:
   The ginit() function does not modify or use any of the GVPAPP data members.
   The GVPAPP structures can therefore be initialized for the first time
   either before or after the initial ginit() call.

   The initialization of all viewport GVPAPP's can be done like this:

   #include <gdisp.h>
   //...
   void my_vpapp_init(void)
      {
      SGUCHAR i;
      PGVPAPP app;
      for (i = 0; i < GNUMVP; i++)
        {
        app = ggetappvp(i); // Get pointer to GVPAPP stucture for viewport number
        // Do first-time initialization of the GVPAPP structure members
        // via the app pointer
        }
     }


   Note:
   As data members defined here in GVPAPP are added to all viewports then care 
   should be taken not to overuse the feature. 
   For instance just adding a generic pointer to application specific data 
   structures may be the right way to optimize RAM memory consumption in some 
   applications.
   In other systems using a union of application specific data structures may be 
   the best solution to optimize the RAM memory consumption.
*/

#include <gdisp.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
   Viewport extention structure.
   The data elements defined here are only used by the application layers
*/
typedef struct _GVPAPP
   {
   /* Application specific viewport data members are inserted here  */
   void *buf;  /* Pointer to application specific data (just an example) */
   } GVPAPP;

#ifdef __cplusplus
}
#endif

#endif  /* GVPEXT */
#endif  /* GVPAPP_H */

