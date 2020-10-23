/************************** ghwinit.c *****************************

   Low-level driver functions for the KS07XX LCD display controller
   initialization and error handling.

   The KS07XX controller is assumed to be used with a LCD module.

   The following LCD module characteristics MUST be correctly
   defined in GDISPCFG.H:

      GDISPW  Display width in pixels
      GDISPH  Display height in pixels
      GBUFFER If defined most of the functions operates on
              a memory buffer instead of the LCD hardware.
              The memory buffer content is copied to the LCD
              display with ghw_updatehw().
              (Equal to an implementation of delayed write)


   Creation date:    070102
   Revision date:
   Revision date:    020127
   Revision Purpose: update of invalid-rectancle handling
   Revision date:    020330
   Revision Purpose: Unused driver functions removed after introduction
                     of GHWSYMRW.C
                     ghw_upddelay inserted.
   Revision date:    280403
   Revision Purpose: Line count error in ghw_puterr correctred
   Revision date:    120403
   Revision Purpose: Support for PGENERIC and intrinsic generic pointers
   Revision date:    121203
   Revision Purpose: KS0719 support updated, UC1606 support added
   Revision date:    170804
   Revision Purpose: S1D1571x support added  ,
   Revision date:    170804
   Revision Purpose: Modified to use ghw_set_xy(..)
                     NJU6677-NJU6679 support added
   Revision date:    030804
   Revision Purpose: Simulator update simplified
   Revision date:    010205
   Revision Purpose: Powerup delay introduced via ghw_cmdw(..).
                     (Required by some KS0724 and NJU6677 based displays)
   Revision date:    190505
   Revision Purpose: Support for KS0741 black & white mode added
   Revision date:    070705
   Revision Purpose: Support for GHW_NO_BUSY_POLL and GHW_EL43102 switches added
   Revision date:    170805
   Revision Purpose: PT6866 support added (OLED driver) ,
   Revision date:    140706
   Revision Purpose: UC1608 switch added
   Revision date:    100108
   Revision Purpose: Adjusted for simulator update
   Revision date:    160108
   Revision Purpose: UC1608 mode, MR bit set explitly
   Revision date:    270509
   Revision Purpose: UC1608 mode, setup commands corrected.

   Version number: 1.24
   Copyright (c) RAMTEX Engineering Aps 2003-2009

*********************************************************************/
#define GHWINIT_C

#include "gdisphw.h"  /* HW driver prototypes and types */
#include "ks07xx.h"   /* KS07XX controller specific definements */





#ifdef WIN32
#include <windows.h> /* Sleep function */
#endif

/*#define WR_RD_TEST */ /* Define to include write-readback test in ghw_init() in non-buffered mode*/

#if (defined( GHW_NO_LCD_READ_SUPPORT ) && !defined( GBUFFER ))
   #error GBUFFER must be defined when display interface has no read support
#endif

/* Define the width of the internal video RAM in the LCD controller */
#if (defined ( GHW_S1D1571x ) || defined( GHW_UC1608))
  #define GCTRLW 256
#elif defined ( GHW_KS0719 )
  #define GCTRLW 160
#elif defined( GHW_KS0759)
  #define GCTRLW 128
#elif defined ( GHW_KS0741 )
  #define GCTRLW 128
#elif defined ( GHW_EL43102 )
  #define GCTRLW 102
#else
  #define GCTRLW 132  /* NT750x, SED1565, KS0723, KS0713, UC1606, NJU667x, PT6866 */
#endif

#if (GDISPW > GCTRLW)
  #error Illegal configuration setting. Display width is larger than setting for internal video RAM
#endif


/* Check and clean up some definitions */
#ifndef GHW_NO_HDW_FONT
  #error Illegal configuration file. GHW_NO_HDW_FONT must be defined in GDISPCFG.H
#endif

#ifndef GHW_XOFFSET
  #ifdef GHW_MIRROR_HOR
     #define GHW_XOFFSET (GCTRLW-GDISPW)    /* Use a default x offset */
  #else
     #define GHW_XOFFSET 0
  #endif
#endif

/********************* Chip access definitions *********************/
#ifndef GHW_NOHDW
   #ifdef GHW_SINGLE_CHIP
      #include "bussim.h"
      #define  sgwrby(a,d) simwrby((a),(d))
      #define  sgrdby(a)   simrdby((a))
   #else
      /* Portable I/O functions + hardware port def */
      #include "sgio.h"
   #endif
#else
   #undef GHW_SINGLE_CHIP /* Ignore single chip mode */
#endif

/***********************************************************************/
/** All static LCD driver data is located here in this ghwinit module **/
/***********************************************************************/
#ifdef GBASIC_INIT_ERR

#ifdef GBUFFER
   #ifdef GHW_ALLOCATE_BUF
      /* <stdlib.h> is included via gdisphw.h */
      SGUCHAR *gbuf = NULL;           /* Graphic buffer pointer */
      static SGBOOL gbuf_owner = 0;   /* Identify pointer ownership */
   #else
      SGUCHAR gbuf[ GBUFSIZE ];       /* Graphic buffer */
   #endif
   GXT GFAST iltx,irbx;  /* "Dirty area" speed optimizers in buffered mode */
   GYT GFAST ilty,irby;
   SGBOOL  ghw_upddelay;
#endif

#ifdef GHW_INTERNAL_CONTRAST
static SGUCHAR ghw_contrast;      /* Current contrast value */
#endif

SGBOOL  glcd_err;                 /* Internal error */
#ifndef GNOCURSOR
GCURSOR ghw_cursor;               /* Current cursor state */
#endif

/* Symbol table entry with fixed sized symbols */

static struct
   {
   GSYMHEAD sh;        /* Symbol header */
   SGUCHAR b[8];       /* Symbol data, variable length = (cxpix/8+1)*cypix */
   }
GCODE FCODE sysfontsym[0x80] =
   {
   #include "sfks07xx.sym"        /* System font symbol table */
   };

/* Default system font */
GCODE GFONT FCODE SYSFONT =
   {
   8,      /* width */
   8,      /* height */
   sizeof(sysfontsym[0])-sizeof(GSYMHEAD), /* number of data bytes in a symbol (including any alignment padding)*/
   (PGSYMBOL) sysfontsym,  /* pointer to array of SYMBOLS */
   0x80,   /* num symbols (includes download symbols) */
   NULL    /* SYSFONT must not use codepage */
   };

#ifdef GHW_PCSIM
/* PC simulator declaration */
extern SGUCHAR *KS07XX_buf;    /* KS07XX simulator module buffer */

/* Simulator functions */
void ghw_cmd_KS07XXsim( SGUCHAR cmd );
void ghw_dcmd_KS07XXsim( SGUCHAR cmd1, SGUCHAR cmd2 );
void ghw_init_KS07XXsim( SGUINT dispw, SGUINT disph, SGUINT ctrlw);
void ghw_wrupdate_KS07XXsim( SGUINT x, SGUINT y, SGUCHAR val );
void ghw_exit_KS07XXsim(void);
#endif

/**********************************************************************/
/** Low level KS07XX interface functions used only by ghw_xxx modules **/
/**********************************************************************/

/*
   Arrays with fast mask values used by the low-level graphic functions
   and view ports.
*/
GCODE SGUCHAR FCODE pixymsk[8] =
    {0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x80};

GCODE SGUCHAR FCODE startmask[8] =
    {0xff,0xfe,0xfc,0xf8,0xf0,0xe0,0xc0,0x80};
GCODE SGUCHAR FCODE stopmask[8] =
    {0x01,0x03,0x07,0x0f,0x1f,0x3f,0x7f,0xff};

/* Tempoary buffer for horizontal byte writing */
SGUCHAR GFAST ghw_tmpb[8];
SGUCHAR GFAST ghw_tmpb2[8];

static GXT GFAST g_xpos;  /* Tracking LCD coordinates */
static GYT GFAST g_ypos;

#ifndef GHW_NO_LCD_READ_SUPPORT
/*
   Read status register
   Internal ghw function
*/
SGUCHAR ghw_sta( void )
   {
   #ifndef GHW_NOHDW
   SGUCHAR sta;
   sta = sgrdby(GHWSTA);
   return sta; /* Put a break point here to monitor status returns */
   #else
   return (0);
   #endif
   }
#endif

/*
   Local status check functions for normal commands
   Internal ghw function

   (Note with a slow target processor system busy check may
    not be required because the LCD controller is fast enough.
    In such systems a speed optimization can be made by defining
    GHW_NO_BUSY_POLL in gdispcfg.h )
*/
#if (!defined( GHW_NOHDW ) && !defined( GHW_NO_BUSY_POLL ) && !defined( GHW_NO_LCD_READ_SUPPORT ))

/* Chip wait for not busy */
static void ghw_wait(void)
   {
   if ((sgrdby(GHWSTA) & GSTA_BUSY) != 0)
      {
      /* Controller was busy, execute wait loop */
      SGUINT timeout = 64;
      do {
         if ((sgrdby(GHWSTA) & GSTA_BUSY) == 0)
            return;
         }
      while (timeout-- != 0);
      glcd_err = 1;
      }
   }

#else
  #define ghw_wait()  /* Nothing */
#endif  /* !GHW_NOHDW && !GHW_NO_BUSY_POLL */


/*
   Send a command to KS07XX
   Wait for controller + data ready

   set ghw_err = 0 if Ok
   set ghw_err = 1 if Timeout error

   Internal ghw function
*/
void ghw_cmd( SGUCHAR cmd )
   {
   #ifdef GHW_PCSIM
   ghw_cmd_KS07XXsim( cmd );
   #endif

   #ifndef GHW_NOHDW
   ghw_wait();
   sgwrby(GHWCMD, cmd);
   #endif
   }

#ifndef GHW_NJU6677
/*
   Send a double byte command to KS07XX
   Wait for controller + data ready

   set ghw_err = 0 if Ok
   set ghw_err = 1 if Timeout error

   Internal ghw function
*/
void ghw_dcmd( SGUCHAR cmd1, SGUCHAR cmd2 )
   {
   #ifdef GHW_PCSIM
   ghw_dcmd_KS07XXsim( cmd1, cmd2 );
   #endif

   #ifndef GHW_NOHDW
   ghw_wait();
   sgwrby(GHWCMD, cmd1);
   ghw_wait();
   sgwrby(GHWCMD, cmd2);
   #endif
   }

#endif

/*
   Set the x,y position
   Wait for controller + data ready

   set ghw_err = 0 if Ok
   set ghw_err = 1 if Timeout error

   Internal ghw function
*/
void ghw_set_xypos(GXT x, GYT y)
   {
   #ifndef GHW_NOHDW
   if ((y & ~(0x7)) != (g_ypos & ~(0x7)))
      {
      /* Page has changed */
      g_ypos = y;
      #ifdef GHW_NJU6677
       y = ((y>>7) & 0xF) | GCTRL_YADR_H;
       ghw_wait();
       sgwrby(GHWCMD,y);  /* Set Y koordinate high */
       y = ((g_ypos>>3) & 0xF) | GCTRL_YADR_L;
      #else
       y = ((y>>3) & 0xF) | GCTRL_YADR;
      #endif
      ghw_wait();
      sgwrby(GHWCMD,y);  /* Set Y koordinate */
      }
   else
   #endif
      g_ypos = y;

   if (g_xpos != x)
      {
      g_xpos = x;
      #ifndef GHW_NOHDW
      x += GHW_XOFFSET;
      ghw_wait();
      sgwrby(GHWCMD,((x>>4) & 0xF) | GCTRL_XADR_H);
      ghw_wait();
      sgwrby(GHWCMD,(x & 0xF)      | GCTRL_XADR_L);
      #endif
      }
   }

#ifndef GBUFFER
/*
   Write databyte to controller (at current position)
   Wait for controller + data ready

   set ghw_err = 0 if Ok
   set ghw_err = 1 if Timeout error

   Internal ghw function
*/
void ghw_wr( SGUCHAR val )
   {
   #ifndef GHW_NOHDW
   register GXT x;
   x = g_xpos + GHW_XOFFSET;
   ghw_wait();
   sgwrby(GHWCMD,(x & 0xF)      | GCTRL_XADR_L); /* Set X koordinate */
   ghw_wait();
   sgwrby(GHWCMD,((x>>4) & 0xF) | GCTRL_XADR_H);
   #ifdef GHW_KS0741
   ghw_wait();
   sgwrby(GHWWR,val);  /* Write first byte */
   #endif
   ghw_wait();
   sgwrby(GHWWR,val);                     /* Write data */
   #endif

   #ifdef GHW_PCSIM
   ghw_wrupdate_KS07XXsim(g_xpos,g_ypos,val);
   #endif
   }
#endif  /* GBUFFER */

/*
   Write databyte to controller (at current position) and increment
   internal xadr. Wait for controller + data ready

   set ghw_err = 0 if Ok
   set ghw_err = 1 if Timeout error

   Internal ghw function
*/
void ghw_auto_wr(SGUCHAR ydat)
   {
   #ifdef GHW_PCSIM
   ghw_wrupdate_KS07XXsim(g_xpos, g_ypos, ydat);
   #endif

   #ifndef GHW_NOHDW
   #ifdef GHW_KS0741
   ghw_wait();
   sgwrby(GHWWR,ydat);  /* Write first byte */
   #endif
   ghw_wait();
   sgwrby(GHWWR,ydat);  /* Write byte */
   #endif

   g_xpos++;
   }

#ifndef GBUFFER
/*
   Read databyte from controller (at current position) and keep
   position unchanged.
   Wait for controller + data ready

   set ghw_err = 0 if Ok
   set ghw_err = 1 if Timeout error

   Internal ghw function
*/
SGUCHAR ghw_rd( void )
   {
   #ifndef GHW_NOHDW
   SGUCHAR ret;
   register GXT x;
   x = g_xpos + GHW_XOFFSET;
   /* Reset colum pointer state*/
   ghw_wait();
   sgwrby(GHWCMD,(x & 0xF)        | GCTRL_XADR_L);  /* Set X koordinate */
   ghw_wait();
   sgwrby(GHWCMD,((x >> 4) & 0xF) | GCTRL_XADR_H);
   ghw_wait();
   ret = sgrdby(GHWRD);  /* Dummy read needed after internal pointer setting */
   #ifdef GHW_KS0741
   ghw_wait();
   ret = sgrdby(GHWRD);  /* Skip first of double byte */
   #endif
   ghw_wait();
   ret = sgrdby(GHWRD);  /* read data */

   /* Restore internal chip pointer */
   ghw_wait();
   sgwrby(GHWCMD,(x & 0xF)        | GCTRL_XADR_L);  /* Set X koordinate */
   ghw_wait();
   sgwrby(GHWCMD,((x >> 4) & 0xF) | GCTRL_XADR_H);
   return ret;
   #else
   #ifdef GHW_PCSIM
   return KS07XX_buf[(((SGUINT)g_ypos)/8)*GCTRLW + g_xpos];
   #else
   return 0;
   #endif
   #endif
   }

/* Perform required dummy reads after column position setting */
void ghw_auto_rd_start(void)
   {
   #ifndef GHW_NOHDW
   SGUCHAR ret = 0;       /* Dummy var to assure that code for sgrdby(..) is generated */
   ghw_wait();
   ret += sgrdby(GHWRD);  /* Dummy read needed after internal pointer setting */
   #endif
   }
/*
   Read databyte from controller at current pointer and
   inclement pointer
   Wait for controller + data ready

   set ghw_err = 0 if Ok
   set ghw_err = 1 if Timeout error

   Internal ghw function
*/
SGUCHAR ghw_auto_rd(void)
   {
   #ifndef GHW_NOHDW
   g_xpos++;
   #ifdef GHW_KS0741
   sgrdby(GHWRD); /* Skip first of double byte */
   #endif
   return sgrdby(GHWRD);

   #else

   #ifdef GHW_PCSIM
   return KS07XX_buf[(((SGUINT)g_ypos)/8)*GCTRLW + g_xpos++];
   #else
   return 0;
   #endif

   #endif
   }

#endif  /* GBUFFER */


/*
   Insert horizontal data in a buffer
   at the current Y sub-coordinate.
   bufptr points to a 8 bytes forming a 8x8 bit array
*/
void ghw_buf_xwr( SGUCHAR *bufptr, GYT y, SGUCHAR dat )
   {
   SGUCHAR xmask;
   SGUCHAR ymask;
   xmask = 0x80;
   ymask = pixymsk[y & 0x7];
   do
      {
      if ((dat & xmask) != 0)
         *bufptr |= ymask;
      else
         *bufptr &= ~ymask;
      bufptr++;
      }
   while ((xmask >>= 1) != 0);
   }


/***********************************************************************/
/**        KS07XX Initialization and error handling functions         **/
/***********************************************************************/

#if (defined( WR_RD_TEST ) && !defined(GBUFFER))
/*
   Make write-readback test on controller memory.

   This test will fail if some databus and control signals is not connected correctly.

   This test will fail if bus mode selection in the configuration settings
   does not match the actual bus configuration for the hardware (display and processor
   8080/6800 bus type settings, address offsets, etc).

   This test may fail if illegal GDISPW, GDISPH, GHW_XOFFSET, GHW_YOFFSET
   configuration settings cause overrun of the on-chip video RAM.

   This test can be exectuted correctly with only logic power on the display module.
   No high-level voltages are nessesary for the test to run (although nothing then can
   be shown on the display)

   Return 0 if no error,
   Return != 0 if some readback error is detected (the bit pattern may give information
   about connector pins in error)

   NOTE:
   This function test should not be enabled in serial mode. In serial mode
   the controller does not provide read-back facility and this test will always
   fail.
*/
#include <stdio.h>
static SGUCHAR ghw_wr_rd_test(void)
   {
   #ifndef GHW_NOHDW
   int i;
   SGUCHAR msk,result;
   ghw_set_xypos(0,0);
   /*printf("\n");*/

   /* Fill pattern to video buffer */
   for (i = 0, msk = 1; i < 8; i++)
      {
      ghw_auto_wr(msk);
      /*printf("0x%02x ", (unsigned int) msk);*/
      ghw_auto_wr(~msk);
      /*printf(" 0x%02x\n", (unsigned int) (~msk & 0xff));*/
      msk <<= 1;
      }
   /*printf("\n");*/

   ghw_set_xypos(0,0);
   ghw_auto_rd_start();

   /* Read pattern back while compare */
   for (i=0, msk=1, result=0; i < 8; i++)
      {
      SGUCHAR val;
      val = ghw_auto_rd();
      result |= (val ^ msk);
      /*printf("0x%02x ",  (unsigned short) val);*/
      val = ghw_auto_rd();
      /*printf(" 0x%02x\n", (unsigned short) val );*/
      result |= (val ^ (~msk));
      msk <<= 1;
      }
   return result;  /* 0 = Nul errors */
   #else
   return 0; /* 0 = Nul errors */
   #endif
   }
#endif /* WR_RD_TEST */

/*
   Fast set or clear of LCD module RAM buffer
       dat  = data byte

   Internal ghw function
*/
static void ghw_bufset( SGUCHAR dat )
   {
   GYT y;
   #ifdef GBUFFER
   GBUFINT cnt = 0;
   #endif
   g_ypos = 0xff;  /* Force update */
   g_xpos = 0xff;
   for (y = 0; y < GDISPH; y += 8)
      {
      ghw_set_xypos(0,y);
      /* Clear using X autoincrement */
      while (g_xpos<GDISPW)
         {
         ghw_auto_wr(dat);  /* Set LCD buffer */
         #ifdef GBUFFER
         gbuf[cnt++] = dat; /* Set ram buffer as well */
         #endif
         }
      }
   }

#ifndef GHW_PT6866
/*
   Wait here for LCD power to stabilize
   If this condition is violated then the rest of the initialization may fail.
   The required time depends on the actual display and power circuit.
   Here multiple writes is used as delay.

   For some displays the for(;;) loop can be optimized away (commented out)
*/
static void ghw_cmdw( SGUCHAR cmd )
   {
   #ifdef WIN32
   Sleep(15);
   ghw_cmd( cmd );
   #else
   SGUINT tmp;
   for (tmp = 0; tmp < 255; tmp++)
      ghw_cmd( cmd );
   #endif
   }
#endif /* GHW_PT6866 */

/*
   Initialize display, clear ram  (low-level)
   Clears glcd_err status before init

   Return 0 if no error,
   Return != 0 if some error
*/

SGBOOL ghw_init(void)
   {
   #ifdef GBUFFER
   iltx = 0;
   ilty = 0;
   irbx = GDISPW-1;
   irby = GDISPH-1;
   ghw_upddelay = 0;
   #endif

   #ifdef GHW_PCSIM
   /* Tell simulator about the visual LCD screen organization */
   ghw_init_KS07XXsim( GDISPW, GDISPH, GCTRLW );
   #endif

   glcd_err = 0;
   ghw_io_init();                 /* Perform target specific initialization */
   
  //for(;;)
  // {
  // //sgwrby(GHWCMD, 0x00); // Write command
  // simwrby(GHWCMD, 0x00); // Write command
  // //SM1_SendBlock(masterDevData,GHWCMD, sizeof(GHWCMD));
  // //sgwrby(GHWWR, 0xaa); // Write data
  // simwrby(GHWWR, 0xaa); // Write data 
  // //SM1_SendBlock(masterDevData,GHWWR, sizeof(GHWWR));
  // //sgrdby(GHWSTA); // Read status
  // //sgrdby(GHWRD); // Read data
  // }
   
   #if (defined( GHW_ALLOCATE_BUF) && defined( GBUFFER ))
   if (gbuf == NULL)
      {
      /* Allocate graphic ram buffer */
      if ((gbuf = calloc(ghw_gbufsize(),1)) == NULL)
         glcd_err = 1;
      else
         gbuf_owner = 1;
      }
   #endif

   if (glcd_err)
      return 1;                   /* Some lowlevel io error detected */

   #ifndef GHW_NOHDW

   /* Test loop for oscilloscope check of bus signal sequences */
   /*
   for(;;)
      {
      SGUCHAR sta;
      sta = sgrdby(GHWSTA);
      sta = sgrdby(GHWRD);
      sgwrby(GHWCMD,0);
      sgwrby(GHWWR,0x00);
      }
   */

   sgwrby(GHWCMD, GCTRL_RESET);   /* soft reset */

   #ifdef GHW_UC1608
   /* For some UC1608 versions the display must be turned on before
      the RESET status flag will be cleared */
   ghw_cmdw(GCTRL_ON);             /* Display On */
   #endif

   #ifndef GHW_NO_LCD_READ_SUPPORT
   /* Wait for soft reset to complete */
   #ifdef GHW_NO_BUSY_POLL
   if ((ghw_sta() & GSTA_RESET) != 0)
   #else
   if ((ghw_sta() & (GSTA_BUSY | GSTA_RESET)) != 0)
   #endif
      {
      /* Controller was in reset (or busy) state, execute wait loop */
      SGUINT timeout = 1000;
      do {
         if (--timeout == 0)
            {
            glcd_err = 1;
            return 1; /* Some lowlevel io error detected */
            }
         }
      #ifdef GHW_NO_BUSY_POLL
      while ((ghw_sta() & GSTA_RESET) != 0);
      #else
      while ((ghw_sta() & (GSTA_BUSY | GSTA_RESET)) != 0);
      #endif
      }
   #endif /* GHW_NO_LCD_READ_SUPPORT */

   #if ( defined( GHW_KS0719 ) || (defined GHW_KS0741) )

   /* GHW_KS0719 or KS0741 */
   ghw_dcmd(GCTRL_DSTART,0);      /* LCD start line = 0 */

   /* Set partial display size. Only needed if it should be different from
      reset default. Please note that in some KS0719 manuals there is an
      error in the description of the partial size parameter. */
   /* ghw_dcmd(GCTRL_DUTY,105); */ /* LCD partial duty ratio (1/105)*/

   ghw_cmd(GCTRL_OSC_ON);         /* Turn oscillator on */
   ghw_cmd(GCTRL_DC_DC_STEPUP|2); /* DC-DC stepup | (0-3)*/
   ghw_cmd(GCTRL_LCD_BIAS | 4);   /* LCD bias ratio | (0-7) */
   ghw_cmd(GCTRL_REG_RES  | 7);   /* Regulator resistor ratio select | (0-7) */

   ghw_cont_set(30);              /* Set default contrast level (0-99) */ 
   ghw_cmdw(GCTRL_POWER | 0x4);   /* Turn stepup generator on */
   ghw_cmdw(GCTRL_POWER | 0x6);   /* Turn resistor ladder on */
   ghw_cmdw(GCTRL_POWER | 0x7);   /* Turn buffers on */

   /* End GHW_KS0719 or KS0741 */

   #elif defined( GHW_NT750x )

   /* GHW_NT750x */
   ghw_cmd(GCTRL_DSTART);         /* LCD start line = 0 */
   ghw_cmd(GCTRL_LCD_BIAS | 2);   /* LCD bias ratio | (2-3) */
   ghw_cmd(GCTRL_NORM_REV | 0);   /* True / inverse display | (0-1) */

   ghw_cont_set(60);              /* Set default contrast level (0-99) */
   ghw_cmdw(GCTRL_POWER | 0x4);   /* Turn stepup generator on */
   ghw_cmdw(GCTRL_POWER | 0x6);   /* Turn resistor ladder on */
   ghw_cmdw(GCTRL_POWER | 0x7);   /* Turn buffers on */

   /* End GHW_NT750x */

   #elif defined( GHW_UC1606 )

   /* GHW_UC1606 */
   ghw_cmd(GCTRL_DSTART);         /* LCD start line = 0 */
   ghw_cmd(GCTRL_LCD_BIAS | 3);   /* LCD bias ratio | (2-3) */
   ghw_cmd(GCTRL_REG_RES  | 0x1); /* Regulator resistor ratio select | (0-7) */
   ghw_cmd(GCTRL_NORM_REV | 0);   /* True / inverse display | (0-1) */

   ghw_cont_set(50);              /* Set default contrast level (0-99) */
   ghw_cmdw(GCTRL_POWER | 0x4);   /* Turn stepup generator on */
   ghw_cmdw(GCTRL_POWER | 0x6);   /* Turn resistor ladder on */
   ghw_cmdw(GCTRL_POWER | 0x7);   /* Turn buffers on */

   /* End GHW_UC1606 */
   #elif defined( GHW_UC1608 )

   /* Most configuration is based on reset defaults */
   ghw_cmd( 0x88 );  /* Address increment control */
   ghw_cmd( 0xee );  /* Reset cursor mode */
   #if (GDISPH > 96)
   ghw_cmd( GCTRL_MUXRATE | 4 ); /* Mux rate 128 */
   #else
   ghw_cmd( GCTRL_MUXRATE | 0 ); /* Mux rate 96 */
   #endif
   ghw_cont_set(50);              /* Set default contrast level (0-99) */

   /* End GHW_UC1608 */
   #elif defined( GHW_NJU6677 )

   /* Start NJU6677, NJU6678, NJU6679 */
   ghw_cont_set(50);               /* Set default contrast level (0-99) */
                                   /* Only active with internal voltage generator */
   ghw_cmd(GCTRL_POWER | 0);       /* 0 = external LCD power, 1=  contrast regulation is used */
   ghw_cmd(GCTRL_LCD_BIAS  | 8);   /* 1/12 bias (reset default) */
   ghw_cmdw(GCTRL_BOOST_LEV | 4);  /* 6 x boost (reset default) */

   ghw_cmd(GCTRL_DRIVER | 1);      /* Turn driver hardware on */
   ghw_cmd(GCTRL_DSTART_H | 0);
   ghw_cmd(GCTRL_DSTART_L | 0);
   ghw_cmd(GCTRL_NORM_REV | 0);    /* True / inverse display | (0-1) */
   /* End NJU6677, NJU6678, NJU6679 */

   #elif defined(GHW_PT6866)

   /* Start PT6866 OLED controller */
   ghw_dcmd(GCTRL_CURRENT,0x12);      /* Current mode, 0x12= full, 0x10 = half */
   ghw_dcmd(GCTRL_PRE_CHA,(8*16)|8);  /* Precharge & zero select, Pre-charge mode, zero mode (1-15)<<4 | (1-15) */
   ghw_dcmd(GCTRL_OLED_BIAS, 0x88);   /* Bias 0x88 = Normal, 0xA1 = Low bias current */
   ghw_dcmd(GCTRL_DOFFSET, 0);        /* Display offset | (0-63) */
   ghw_dcmd(GCTRL_LOW_PWR, 0);        /* Low power, 0 = Normal, 5 = Low power */
   ghw_dcmd(GCTRL_MUX_RATIO,63);      /* Multplexer ration, (0-63)  */
   ghw_dcmd(GCTRL_FR_FREQ, 0x42);     /* Frame frequency */
                                      /*  0x41  (fosc/(4*df)) */
                                      /*  0x42  (fosc/(6*df)) */
                                      /*  0x43  (fosc/(8*df)) */

   ghw_cmd(GCTRL_DSTART | 0);         /* Start line = 0 */
   ghw_cmd(GCTRL_NORM_REV | 0);       /* True / inverse display | (0-1) */
   ghw_cmd(GCTRL_ICON_MODE | 0);      /* Set Icon Mode on/off | (0-1) */

   ghw_cont_set(50);                  /* Set default contrast level (0-99) */
   /* End PT6866 OLED controller */

   #else

   /* others */

   #ifdef GHW_EL43102
   /* Extra registers for EL43102, enable if setting should be different from reset default */
   /*ghw_dcmd(GCTRL_CL_FREQ,    0x03); */ /* Set CL frequency ration */
   /*ghw_dcmd(GCTRL_DUTY_RATIO, 0x05); */ /* Set duty ration mode 1/43 */
   /*ghw_dcmd(GCTRL_BIAS_SELECT,0x0A); */ /* Set LCD bias select 1/8 */
   #endif

   /* others */
  
  
   ghw_cmd(GCTRL_DSTART);         /* LCD start line = 0 */
  // ghw_cmd(GCTRL_LCD_BIAS | 3);   /* LCD bias ratio | (2-3) */			//commentato AR 11/07/2017
   ghw_cmd(GCTRL_LCD_BIAS );								//inserito AR 11/07/2017

   ghw_cmd(GCTRL_NORM_REV | 0);   /* True / inverse display | (0-1) */
   //ghw_cmd(GCTRL_REG_RES  | 0x07); /* Regulator resistor ratio select | (0-7) */

   //ghw_cmd(GCTRL_REG_RES  | 0x03); /* Regulator resistor ratio select | (0-7) */		//commentato AR 11/07/2017
   ghw_cmd(GCTRL_REG_RES  | 0x04); /* Regulator resistor ratio select | (0-7) */		//inserito AR 11/07/2017

   ghw_cont_set(70);              /* Set default contrast level (0-99) */
   //ghw_cont_set(50);              /* Set default contrast level (0-99) */
   ghw_cmdw(GCTRL_POWER | 0x4);   /* Turn stepup generator on */
   ghw_cmdw(GCTRL_POWER | 0x6);   /* Turn resistor ladder on */
   ghw_cmdw(GCTRL_POWER | 0x7);   /* Turn buffers on */
   /* End others */

   #endif

   #ifdef GHW_NJU6677
     /* NJU6677, NJU6678, NJU6679 */

     #ifdef GHW_MIRROR_HOR
     ghw_cmd(GCTRL_ADC | 1);       /* Inverse horizontal direction */
     #else
     ghw_cmd(GCTRL_ADC | 0);       /* Normal horizontal direction */
     #endif

     /* End NJU6677, NJU6678, NJU6679 */
   #elif defined( GHW_UC1608 )

    /* UC1608 has both mirror flags in same command */
   #if    defined( GHW_MIRROR_HOR ) &&  defined( GHW_MIRROR_VER )
   ghw_cmd(GCTRL_LCDMAP | 0xc);
   #elif  defined( GHW_MIRROR_HOR ) && !defined( GHW_MIRROR_VER )
   ghw_cmd(GCTRL_LCDMAP | 0x4);
   #elif !defined( GHW_MIRROR_HOR ) &&  defined( GHW_MIRROR_VER )
   ghw_cmd(GCTRL_LCDMAP | 0x8);
   #else
   ghw_cmd(GCTRL_LCDMAP | 0x0);
   #endif
   /* End UC1608 */

   #else  /* Others */

     #ifdef GHW_MIRROR_HOR
   	   ghw_cmd(GCTRL_SHL | 8);        /* Inverse horizontal direction */
     #else
   	   ghw_cmd(GCTRL_SHL | 0);        /* Normal horizontal direction */

     #endif

     #ifdef GHW_MIRROR_VER
     ghw_cmd(GCTRL_ADC | 1);       /* Inverse vertical direction */
     #else
    // ghw_cmd(GCTRL_ADC | 1);
     ghw_cmd(GCTRL_ADC | 0);       /* Normal vertical direction */
     #endif

     /* End Others */
   #endif

   #ifdef GHW_NJU6676
   /* Extra register in NJU6676 compared to KS07xx default */
   ghw_cmd(GCTRL_DRIVER | 1);     /* Turn driver hardware on */
   #endif

   #endif  /* GHW_NOHDW */

   if (glcd_err)
      return 1;                   /* Some initial error detected */

   #if (defined( WR_RD_TEST ) && !defined(GBUFFER))
   /*
      NOTE:
      The call of ghw_wr_rd_test() should be commented out in serial mode.
      In serial mode the display controller  does not provide read-back facility
      and this test will always fail.
   */
   if (ghw_wr_rd_test() != ((SGUCHAR) 0))
      {
      /* Controller memory write-readback error detected
      (Check the cable or power connections to the display) */
      G_WARNING("Hardware interface error\nCheck display connections\n");  /* Test Warning message output */
      glcd_err = 1;
      return 1;
      }
   #endif

   ghw_cmd(GCTRL_E_ON | 0);       /* Clear entire display On, (0 = normal, 1 = on) */
   ghw_bufset( 0x00);            /* Clear graphic area */
   ghw_cmd(GCTRL_ON);             /* Display On */

   ghw_set_xypos(0,0);

   #ifndef GNOCURSOR
   ghw_cursor = GCURSIZE1;                 /* Cursor is off initially */
   /* ghw_cursor = GCURSIZE1 | GCURON; */  /* Uncomment to set cursor on initially */
   #endif

   ghw_updatehw();  /* Flush to display hdw or simulator */
   return (glcd_err != 0) ? 1 : 0;
   }

/*
   Return last error state. Called from applications to
   check for LCD HW or internal errors.
   The error state is reset by ghw_init and all high_level
   LCD functions.

   Return == 0 : No errors
   Return != 0 : Some errors
*/
SGUCHAR ghw_err(void)
   {
   #if (defined(_WIN32) && defined( GHW_PCSIM))
   if (GSimError())
      return 1;
   #endif
   return (glcd_err == 0) ? 0 : 1;
   }

void ghw_exit(void)
   {
   #if (defined( GHW_ALLOCATE_BUF) && defined( GBUFFER ))
   if (gbuf != NULL)
      {
      ghw_cmd(GCTRL_OFF);  /* Blank display */
      if (gbuf_owner != 0)
         {
         /* Buffer is allocated by ginit, so release graphic buffer here */
         free(gbuf);
         gbuf_owner = 0;
         }
      gbuf = NULL;
      }
   #else
   ghw_cmd(GCTRL_OFF);  /* Blank display */
   #endif
   ghw_io_exit();       /* Release any LCD hardware resources, if required */
   #ifdef GHW_PCSIM
   ghw_exit_KS07XXsim();/* Release simulator resources */
   #endif
   }

/*
   Display a (fatal) error message.
   The LCD display module is always cleared and initialized to
   the system font in advance.
   The error message is automatically centered on the screen
   and any \n characters in the string is processed.

   str = ASCII string to write at display center
*/
void ghw_puterr( PGCSTR str)
   {
   PGCSTR idx;
   SGUCHAR xcnt,i;
   PGSYMBYTE psym;
   GYT y,ytmp,h;
   GXT x;
   SGBOOL centering = 1;
   #ifdef GBUFFER
   GBUFINT gbufidx;
   #endif

   if (ghw_init() != 0)  /* (Re-) initialize display */
      return;            /* Some initialization error */

   idx=str;
   /* Count number of xcnt in string */
   if (idx == NULL)
      return;
   x = 0;
   for (x = 0, y = 1; *idx != 0; idx++)
      {
      if (*idx == '\n')
         {
         y++;
         x = 0;
         }
      else
         {
         x++;
         if (x >= (GDISPW/SYSFONT.symwidth))
            {
            y++;
            x = 0;         /* Some text longer than line, start on new line */
            centering = 0; /* and skip horizontal centering */
            }
         }
      }

   /* Set start line */
   y = ((y > ((GDISPH/8)-1)) ? 0 : ((GDISPH/8-1)-y)/2) * 8;
   idx=str;
   do
      {
      xcnt=0;  /* Set start x position so line is centered */
      while ((idx[xcnt]!=0) && (xcnt < GDISPW/8) &&
            ((idx[xcnt]!='\n') || (centering == 0)) )
         {
         xcnt++;
         }

      /* Set start for centered message */
      if ((GDISPW > xcnt*SYSFONT.symwidth) && centering)
         x = ((GDISPW-xcnt*SYSFONT.symwidth)/2) & ~(0x7);
      else
         x = 0;
      h = (SYSFONT.symheight > 8) ? 8 : SYSFONT.symheight;

      while (xcnt-- > 0)
         {
         /* Point to graphic content for character symbol */
         psym = &(sysfontsym[(*idx == '\n') ? ' ' : ((*idx) & 0x7f)].b[0]);

         /* Clear tempoary buffer and make it default updated */
         for (i = 0; i < sizeof(ghw_tmpb); i++)
            ghw_tmpb[i] = 0;
         ytmp = y;

         #ifdef GBUFFER
         gbufidx = GINDEX(x,(y/8));
         invalrect(x,ytmp);
         invalrect(x+7,ytmp);
         #endif

         /* Place symbol data in temp buffer and on display */
         for (i = 0; i < h; i++, ytmp++)
            {
            #ifdef GBUFFER
            ghw_buf_xwr(&(gbuf[gbufidx]),ytmp,*psym++);
            #else
            ghw_buf_xwr(ghw_tmpb, ytmp, *psym++);
            #endif
            }

         #ifndef GBUFFER
         /* Flush tmp buffer */
         ghw_set_xypos(x,y);
         for (i = 0; i < SYSFONT.symwidth; i++)
            {
            ghw_auto_wr(ghw_tmpb[i]);
            }
         #endif
         idx++;
         if ((x+=8) > GDISPW)
            break; /* Line overrun, continue on next line */
         }
      if (centering && (*idx == '\n'))
         idx++;
      y+=8;
      }
   while ((*idx != 0) && (y < GDISPH));

   ghw_updatehw();  /* Flush to display hdw or simulator */
   }


#ifndef GNOCURSOR
/*
   Replace cursor type data (there is no HW cursor support in KS07XX)
*/
void ghw_setcursor( GCURSOR type)
   {
   ghw_cursor = type;
   }
#endif

/*
   Turn display off
   (Minimize power consumption)
*/
void ghw_dispoff(void)
   {
   GBUF_CHECK();
   ghw_cmd(GCTRL_OFF);
   }

/*
   Turn display on
*/
void ghw_dispon(void)
   {
   GBUF_CHECK();
   ghw_cmd(GCTRL_ON);
   }

#ifdef GHW_NJU6677
/*  Set NJU6677, NJU6678, NJU6679  partial display mode  */
void ghw_partial_display(SGUCHAR strt1, SGUCHAR len1, SGUCHAR strt2, SGUCHAR len2)
   {
   /* Validate parameters */
   if ((strt1 > 15) ||
       ((len1 + len2) > 16) ||
       (((strt1 + len1) >= strt2) && (len2 != 0)))
      {
      glcd_err |= 1;
      return;                  /* Some paramter error detected */
      }

   ghw_cmd(GCTRL_SUB_BEGIN);
   ghw_cmd((SGUCHAR)(GCTRL_PD_1STRT | (strt1 & 0xf)));
   ghw_cmd((SGUCHAR)(GCTRL_PD_1LEN  | (len1  & 0xf)));
   ghw_cmd((SGUCHAR)(GCTRL_PD_1STRT | (strt2 & 0xf)));
   ghw_cmd((SGUCHAR)(GCTRL_PD_1LEN  | (len2  & 0xf)));
   ghw_cmd(GCTRL_PD_ON);
   ghw_cmd(GCTRL_SUB_END);
   }
#endif

#ifdef GHW_INTERNAL_CONTRAST
/*
   Set contrast (Normalized value range [0 : 99] )
   Returns the old value.
*/
SGUCHAR ghw_cont_set(SGUCHAR contrast)
   {
   SGUCHAR tmp;
   GLIMITU(contrast,99);
   tmp = ghw_contrast;
   ghw_contrast = contrast;

   #if (defined( GHW_ALLOCATE_BUF) && defined( GBUFFER ))
   if (gbuf == NULL) {glcd_err = 1; return contrast;}
   #endif

   #ifdef GHW_NJU6677
   /*  NJU6677, NJU6678, NJU6679 */
   contrast = (contrast+1) * 2 + 55;  /* Scale so 99 => 255 and 0 => 57 */
   ghw_cmd(GCTRL_SUB_BEGIN);
   ghw_cmd((SGUCHAR)(GCTRL_EVR_H | ((contrast >> 4) & 0xf)) );
   ghw_cmd((SGUCHAR)(GCTRL_EVR_L | (contrast & 0xf)) );
   ghw_cmd(GCTRL_EVR_ON);
   ghw_cmd(GCTRL_SUB_END);

   #elif defined( GHW_NT7501 )

   ghw_cmd(GCTRL_REF_VOLT | ((SGUCHAR)((((SGUINT) contrast) *8) / 25)));

   #elif (defined(GHW_PT6866) || defined( GHW_UC1608))

   ghw_dcmd(GCTRL_REF_VOLT , (SGUCHAR)((((SGUINT) contrast) *255) / 99));

   #else  /* Others */

   ghw_dcmd(GCTRL_REF_VOLT , (SGUCHAR)((((SGUINT) contrast) *16) / 25));

   #endif
   return tmp;
   }

/*
   Change contrast (Normalized value range [-99 : +99] )
   Returns the old value.
*/
SGUCHAR ghw_cont_change(SGCHAR contrast_diff)
   {
   SGINT tmp;
   tmp =  (SGINT) ((SGUINT) ghw_contrast);
   tmp = tmp + (SGINT) contrast_diff;
   GLIMITU(tmp,99);
   GLIMITD(tmp,0);
   return ghw_cont_set((SGUCHAR)tmp);
   }
#endif /* GHW_INTERNAL_CONTRAST */

#if (defined( GHW_ALLOCATE_BUF) && defined( GBUFFER ))
/*
  Size of buffer requied to save the whole screen state
*/
SGUINT ghw_gbufsize( void )
   {
   return GBUFSIZE + sizeof(GHW_STATE);
   }

#ifdef GSCREENS
/*
   Check if screen buf owns the screen ressources.
*/
SGUCHAR ghw_is_owner( SGUCHAR *buf )
   {
   return (((void *)buf == (void *)gbuf) && (gbuf != NULL)) ? 1 : 0;
   }

/*
  Save the current state to the screen buffer
*/
SGUCHAR *ghw_save_state( SGUCHAR *buf )
   {
   GHW_STATE *ps;
   if (!ghw_is_owner(buf))
      return NULL;
   ps = (GHW_STATE *)(&gbuf[GBUFSIZE]);
   ps->upddelay = (ghw_upddelay != 0);
   #ifndef GNOCURSOR
   ps->cursor = ghw_cursor;
   #endif
   ps->xpos = g_xpos;
   ps->ypos = g_ypos;
   return gbuf;
   }

/*
   Set state to buf.
   If buffer has not been initiated by to a screen before, only
   the pointer is updated. Otherwise the buffer
*/
void ghw_set_state(SGUCHAR *buf, SGUCHAR doinit)
   {
   if (gbuf != NULL)
      {
      /* The LCD controller has been initiated before */
      if (gbuf_owner != 0)
         {
         /* Buffer was allocated by ginit, free it so screen can be used instead*/
         free(gbuf);
         gbuf_owner = 0;
         gbuf = NULL;
         }
      }

   if (doinit != 0)
      {
      /* First screen initialization, just set buffer pointer and
         leave rest of initialization to a later call of ghw_init() */
      gbuf = buf;
      gbuf_owner = 0;
      }
   else
      {
      if ((gbuf = buf) != NULL)
         {
         GHW_STATE *ps;
         ps = (GHW_STATE *) &buf[GBUFSIZE];
         #ifndef GNOCURSOR
         ghw_cursor = ps->cursor;
         #endif

         ghw_upddelay = 0;        /* Force update of whole screen */
         iltx = 0;
         ilty = 0;
         irbx = GDISPW-1;
         irby = GDISPH-1;
         ghw_updatehw();

         ghw_set_xypos(ps->xpos, ps->ypos);  /* Restore positions */
         ghw_upddelay = (ps->upddelay != 0) ? 1 : 0;
         }
      }
   }
#endif  /* GSCREENS */
#endif  /* GHW_ALLOCATE_BUF */
#endif


