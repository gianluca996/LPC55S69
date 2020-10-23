
#ifndef GDISPCFG_H
#define GDISPCFG_H
/******************* gdispcfg.h *****************************

   CONFIGURATION FILE FOR THE GRAPHIC LCD LIBRARY
   This header file define the driver feature set used by your
   application and the abstract LCD hardware properties.

   As a programmer you set these definements in order to adjust
   the LCD driver code to the needs of your application.
   In many cases the definements in this file is used to remove
   modify or replace sections of the underlying library source code.

   This header is included via the Graphic LCD library files.
   It should not be included directly by the user application.

   Revision date: 011111
   Revision Purpose: Windows simulator cfg added.
   Revision date: 030528
   Revision Purpose: GMULTIBYTE, GWIDECHAR added
   Revision data: 080306
   Revision Purpose: Configuration header simplified

   Version number: 2.1
   Copyright (c) RAMTEX Engineering Aps 2008

************************************************************/

/* Size of display module in pixels */
  #define GDISPW  128     /* Width */
  #define GDISPH  64      /* Height */

/* LCD controller type (selects initialization for internal video RAM) */
/* #define GHW_SED1565 */
/* #define GHW_S1D1571x */  /* 256 bit wide ram */
/* #define GHW_KS0719 */    /* + SDD1820, SDD1821, SSD1850, SSD1851*/
   #define GHW_KS07xx       /* The default command set, KS0713, KS0717, KS0723, ST7565, ST7567, SPLC50x etc */
/* #define GHW_NT750x */
/* #define GHW_UC1606 */
/* #define GHW_UC1608 */
/* #define GHW_NJU6677 */   /* GHW_NJU6677, GHW_NJU6678, GHW_NJU6679 */
/* #define GHW_NJU6676 */
/* #define GHW_KS0741 */    /* + SSD1852, S6B0741, NT7506, NT7508 */
/* #define GHW_EL43102 */
/* #define GHW_PT6866 */    /* OLED driver */

/* char bit width (height) in HW LCD (do not modify with this LCD controller ) */
  #define GDISPCH 8
  #define GDISPCW 8

/* Define number of view-ports supported,
   See the function SGUCHAR gselvp( SGUCHAR s );
   At least 1 view-port must be defined */
//#define GNUMVP 5
#define GNUMVP 1
 

/* Feature optimization compilation keywords */
#define GBASIC_INIT_ERR           /* Enable Basic initalization and error handling */
#define GBASIC_TEXT               /* Enable Basic text */   

#define GVIEWPORT                 /* Enable high-level (viewport) functions */
#define GGRAPHICS                 /* Enable Graphics */                                                                             
#define GSOFT_SYMBOLS             /* Enable Software symbols */
#define GSOFT_FONTS               /* Enable Soft fonts */                                              //commentato Andrea Costa 8/9/2014
/*#define GVIRTUAL_FONTS*/        /* Enable virtual font support (static lookup) */
/*#define GVIRTUAL_FONTS_DYN*/    /* Enable named virtual font support (dynamic lookup) */
                                           //commentato Andrea Costa 8/9/2014        
#define GS_ALIGN                  /* Enable extended string alignment */                               //commentato Andrea Costa 8/9/2014
/*#define GMULTIBYTE */           /* Enable multibyte support */
/*#define GMULTIBYTE_UTF8 */      /* Enable UTF-8 multibyte support */
/*#define GWIDECHAR */            /* Enable wide-char support */
/*#define GFUNC_VP */             /* Enable named viewport functions xxx_vp()*/
/*#define GSCREENS*/              /* Enable screens */
/*#define GEXTMODE*/              /* Enable application specific viewport data extentions */
                                  /* (viewport data extensions are defined in gvpapp.h) */
#define GNOCURSOR                 /* Turn visual cursor handling off or on*/                               //commentato Andrea Costa 8/9/2014  
                                  /* Define for max speed, undefine to have cursor support */
//#define GNOTXTSPACE               /* Turn extra character or line space handling off and on */             //commentato Andrea Costa 8/9/2014 
                                  /* Define for max speed, undefine to have line and character spacing */
/*#define GVIRTUAL_FILES*/        /* Enable virtual "file" support (dynamic lookup) */
/*#define GVIRTUAL_FILES_STATIC*/ /* Enable static virtual "file" support (fast lookup) */

/* Tabulator definitions */
//#define GCONSTTAB            /* Tab table contain constants (undefine to use variable tabs) */           //commentato Andrea Costa 8/9/2014

#ifdef  GCONSTTAB                                                                                        //commentato Andrea Costa 8/9/2014
 /* Define const tab increments in number of pixel */
 #define GTABSIZE  (GDISPW/6)                                                                            //commentato Andrea Costa 8/9/2014
#else                                                                                                    //commentato Andrea Costa 8/9/2014
 /* variable tab tabel is used */
 #define GMAXTABS 10           /* Max. number of tab positions (defines tabulator tabel size) */         //commentato Andrea Costa 8/9/2014
#endif                                                                                                   //commentato Andrea Costa 8/9/2014

/* Define value for switch between normal and multi-byte string chars
   If the char is above this value then this char plus the next char is
   used to form a 16 bit wide char (ex in the range 0x8000 to 0xffff) */
#define G_MULTIBYTE_LIMIT 0x80                                                                           //commentato Andrea Costa 8/9/2014

/* Select buffered implementation (speed optimization with
   external display RAM buffer or use direct operation on
   module RAM. Define or undefine GBUFFER */
#define GBUFFER /* Extern buffer for data manipulation, fast */


/* If GHW_ALLOCATE_BUF is defined the graphich buffer is allocated using malloc.
   instead of using a (faster) static buffer */
#ifdef GBUFFER
  /* #define GHW_ALLOCATE_BUF */ /* Allocate buffer on heap */
#endif

/* If GWARNING is defined, illegal runtime values will cause
   issue of a display message and stop of the system.
   The soft error handler function G_WARNING(str) defined in
   gdisphw.h is used for message output.
   If undefined parameters will be forced within a legal range
   and used afterwards. */
#define GWARNING                                                                                        //commentato Andrea Costa 8/9/2014

/* If GERROR is defined, states and situations which may result
   in a fatal runtime state will cause a display message to be
   issued and the system stopped. The soft error handler function
   G_ERROR(str) defined in gdisphw.h is used for message output.
   If undefined the situation is ignored or an exit is performed. */
#define GERROR                                                                                        //commentato Andrea Costa 8/9/2014

/* If GDATACHECK is defined the internal data is checked
   for errors. Maybe some faulty part of the main code overwrites
   the internal data of the LCD driver, such and error will be
   catched with this define. Undefine for max speed. */
#define GDATACHECK                                                                                    //commentato Andrea Costa 8/9/2014

/* Variable used for X and W */
#if (GDISPW <= 255)                                                                         //commentato Andrea Costa 8/9/2014
#define GXT   unsigned char                                                                 //commentato Andrea Costa 8/9/2014
#else                                                                                       //commentato Andrea Costa 8/9/2014
#define GXT   unsigned short                                                                //commentato Andrea Costa 8/9/2014
#endif                                                                                      //commentato Andrea Costa 8/9/2014
/* Variable used for Y and H */
#if (GDISPH <= 255)                                                                         //commentato Andrea Costa 8/9/2014
#define GYT   unsigned char                                                                 //commentato Andrea Costa 8/9/2014
#else                                                                                       //commentato Andrea Costa 8/9/2014
#define GYT   unsigned short                                                                //commentato Andrea Costa 8/9/2014
#endif                                                                                      //commentato Andrea Costa 8/9/2014

/* Define integer optimized for buffer indexing and buffer size values */
#define GBUFINT SGUINT

#ifdef GHW_PCSIM                                                                            //commentato Andrea Costa 8/9/2014
   /* Simulator mode only switches */
   /* Define to minimize a console application when the LCD simulator is used */
   #define GHW_MINIMIZE_CONSOLE                                                             //commentato Andrea Costa 8/9/2014
   /* Define to limit simulator updates to the highlevel functions.
      The simulator operations is faster when defined */           
   #define GHW_FAST_SIM_UPDATE                                                              //commentato Andrea Costa 8/9/2014
#endif                                                                                      //commentato Andrea Costa 8/9/2014

#ifndef GHW_PCSIM
   /* Memory type qualifiers only for target compilation mode (dont care in PC simulation mode) */
   /* The following definements allow you to optimize the parameter passing and to use target compiler specific
      memory allocation keywords (default will be used for PC simulator mode) */

   /* Keyword used for fast variables optimization (critical params)*/
   #define GFAST  /* nothing */
   /* Keyword used for very fast variables optimization (critical params)*/

 #define GVFAST /* nothing */                                                              //commentato Andrea Costa 8/9/2014
   /* Type qualifier used on pointer (parameter) to strings when the object is not
      modified by function (part of C strong prototyping) */
   #define GCONSTP const                                                                   //commentato Andrea Costa 8/9/2014
   /* type qualifier used for fixed data (graphic tables etc) */
   #define GCODE  const
   /* Memory type qualifier used for fixed data (if GCODE setting is not enough) */
   #define FCODE  /* nothing */
   /* Memory type qualifier used for pointer to fixed data (if GCODE var * is not enough) */
   #define PFCODE /* nothing */                                                                  //commentato Andrea Costa 8/9/2014
   /* Keyword used for generic pointers to data strings, if generic pointers is not default */
   #define PGENERIC /* nothing */
#endif

/****************** LOW-LEVEL DRIVER CONFIGURATIONS *******************/

/* Adapt library to scan layout selected by LCD module vendor */
/*#define GHW_MIRROR_VER */  /* Mirror the display vertically */
#define GHW_MIRROR_HOR       /* Mirror the display horizontally */                            //commentato Andrea Costa 8/9/2014
#define GHW_XOFFSET  0       /* Set display start offset in on-chip video ram */              //commentato Andrea Costa 8/9/2014

/* Must be defined if the LCD controller chip does not support hardware
   fonts or download font.
   When defined SYSFONT is a soft-font and code for hard font support
   in COMMON modules are optimized away.*/
#define GHW_NO_HDW_FONT

/* Define to turn off busy poll (speed increase with slow systems) */
/* #define GHW_NO_BUSY_POLL */

/* When defined the contrast regulation on LCD controller chip is enabled */
#define GHW_INTERNAL_CONTRAST                                                         //commentato Andrea Costa 8/9/2014 

/* Define to turn off busy poll (speed increase with slow systems) */
/* Also define if the display controller does not provide a BUSY status bit (ex ST7567)*/
/* #define GHW_NO_BUSY_POLL */

#define GHW_USING_VBYTE /* This controller uses vertical bytes (do not modify) */
/* End of gdispcfg.h */
#endif
