#ifndef KS07XX_H
#define KS07XX_H
/****************************** KS07XX.H *****************************

   Definitions specific for the KS07XX Graphic LCD controller.

   The KS07XX controller is assumed to be used with a LCD module.
   The LCD module characteristics (width, height etc) must be correctly
   defined in GDISPCFG.H

   This header should only be included by the low-level LCD drivers

   Creation date:

   Revision date      030520
   Revision Purpose:  GHW_STATE ypos is GYT type
   Revision date:     040819
   Revision Purpose:  NJU6677-NJU6679 support added
   Revision date:     190505
   Revision Purpose:  Support for KS0741 black & white mode added
   Revision date:     070705
   Revision Purpose:  Support for NJU6676 and EL43102 added
   Revision date:     170805
   Revision Purpose:  PT6866 support added (OLED driver)
   Revision date:     160506
   Revision Purpose:  SSD1303 support added (OLED driver)
   Revision date:     090108
   Revision Purpose:  S1D15719 support added (multichip driver)
   Revision date:     020209
   Revision Purpose:  UC1608 updated

   Version number: 1.8
   Copyright (c) RAMTEX Engineering Aps 2003-2009

***********************************************************************/

#include "gdisphw.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef GHW_S1D15719
  #define GCTRL_OFF        0xAE  /* Display off */
  #define GCTRL_ON         0xAF  /* Display on */
  /* The rest of the S1D15719 register definitions is local in ghwinit.c */
#else

/* Controller status bits */
#define GSTA_BUSY        0x80

#ifdef GHW_KS0741
#define GSTA_ON          0x40
#define GSTA_RESET       0x20
#elif defined(GHW_SSD1303)
#define GSTA_ON          0x40
#else
#define GSTA_ADC         0x40
#define GSTA_OFF         0x20
#define GSTA_RESET       0x10
#endif

/* Controller commands ( 1 byte ) */
#define GCTRL_OFF        0xAE  /* Display off */
#define GCTRL_ON         0xAF  /* Display on */
#define GCTRL_XADR_H     0x10  /* Set X adr | (132-0) MSB */
#define GCTRL_XADR_L     0x00  /* Set X adr | (132-0) LSB */

#ifdef GHW_NJU6677
/* NJU6677, NJU6678, NJU6679 */
#define GCTRL_DSTART_H   0x50  /* Set Initial display line (0)*/
#define GCTRL_DSTART_L   0x60  /* Set Initial display line (0)*/
#define GCTRL_YADR_H     0x40  /* Set Y adr | (15-0)*/
#define GCTRL_YADR_L     0xC0  /* Set Y adr | (15-0)*/

#define GCTRL_POWER      0x20  /* Power-on-off ctrl  | (0-1) */
#define GCTRL_DRIVER     0x22  /* Driver-on-off ctrl | (0-1) */
#define GCTRL_BOOST_LEV  0x30  /* Set boost level | (0-4) */
#define GCTRL_REG_RES    0xC0  /* Regulator resistor ration select | (0-7) (boost level) */

#else
/* Others */
#define GCTRL_YADR       0xB0  /* Set Y adr | (15-0)*/
#define GCTRL_DSTART     0x40  /* Set Initial display line (0)*/
#ifndef GHW_SSD1303
#define GCTRL_RESET      0xE2  /* Reset controller */
#endif

#define GCTRL_SHL        0xC0  /* Norm/reverse SHL | (0x00 or 0x08) */
#define GCTRL_NORM_REV   0xA6  /* Normal/Reverse display | (0-1) */

#ifndef GHW_PT6866
#define GCTRL_POWER      0x28  /* Power-Ctrl VC,VR,VF | (0-7) */
#define GCTRL_REG_RES    0x20  /* Regulator resistor ration select | (0-7) */
#endif

#endif

#define GCTRL_ADC        0xA0  /* Set Alternative DireCtion | (0-1) */
#define GCTRL_E_ON       0xA4  /* Normal display / Entire display on  | (0-1) */


#ifdef GHW_NT7501

  #define GCTRL_LCD_BIAS 0xA2  /* Set bias | (2-3) */
  #define GCTRL_REF_VOLT 0x80  /* Set reference voltage | (0-63) */

#elif (defined(GHW_KS0719) || defined(GHW_KS0741))

  #define GCTRL_OSC_ON   0xAB  /* Turn oscillator on */
  #define GCTRL_LCD_BIAS 0x50  /* Set bias | (0-7) */
  #define GCTRL_INI_LINE 0x40  /* Double command 2:byte (0-128)*/
  #define GCTRL_INI_COM0 0x44  /* Double command 2:byte (0-128)*/
  #define GCTRL_DUTY     0x48  /* Double command 2:byte (0-128)*/
  #define GCTRL_NLINEINV 0x4C  /* Double command 2:byte (0-32) */
  #define GCTRL_DC_DC_STEPUP 0x64  /* DC-DC_stepup (0-3) */
  #define GCTRL_REF_VOLT 0x81  /* Set reference voltage, next byte = (0-63) */

#elif (defined(GHW_UC1606) || defined(GHW_UC1608))

#define GCTRL_REF_VOLT    0x81  /* Set reference voltage, next byte = (0-63) */
#define GCTRL_LCD_BIAS    0xE8  /* Set bias | (0-3) */
#ifdef GHW_UC1608
#define GCTRL_MUXRATE     0x20  /* Mux rate | (MR = 0,0x4) */
#define GCTRL_LCDMAP      0xC0  /* X,Y mapping */
#endif

#elif defined(GHW_NJU6677)

#define GCTRL_LCD_BIAS   0xB0  /* Set bias | (0-9) */

/* Sub instructions (1 byte) */
#define GCTRL_SUB_BEGIN  0x70  /* Enter sub instruction mode */
#define GCTRL_SUB_END    0x71  /* Exit sub instruction mode */

#define GCTRL_PD_1STRT   0x00  /* Partial data, 1st block start | (0-15) */
#define GCTRL_PD_1LEN    0x20  /* Partial data, 1st block len   | (0-32) */
#define GCTRL_PD_2STRT   0xC0  /* Partial data, 2st block start | (0-15) */
#define GCTRL_PD_2LEN    0xE0  /* Partial data, 2st block len   | (0-32) */
#define GCTRL_PD_ON      0x40  /* Partial data setting activate */

#define GCTRL_INV_H      0x50  /* Partial inverse start high | (0-3) */
#define GCTRL_INV_L      0x60  /* Partial inverse start low  | (0-16) */
#define GCTRL_INV_ON     0x70  /* Partial inverse setting activate */

#define GCTRL_EVR_H      0x80  /* Contrast setting high | (0-3) */
#define GCTRL_EVR_L      0x90  /* Contrast setting low  | (0-16) */
#define GCTRL_EVR_ON     0xA0  /* Contrast setting activate */

#elif defined(GHW_PT6866)
/* OLED controller */
#define GCTRL_MUX_RATIO  0xA8  /* Multiplexer ration (double command) */
#define GCTRL_FR_FREQ    0xAA  /* Frame frequecy (double command) */
#define GCTRL_ICON_MODE  0xD0  /* Set Icon Mode on/off | (0-1) */
#define GCTRL_CURRENT    0xDA  /* OLED current mode, 0x12= full, 0x10 = half */
#define GCTRL_PRE_CHA    0xD9  /* OLED precharge & zero select, Pre-charge mode, zero mode (1-15)<<4 | (1-15) */
#define GCTRL_OLED_BIAS  0xAB  /* OLED bias 0x88 = Normal, 0xA1 = Low bias current */
#define GCTRL_DOFFSET    0xD3  /* OLED display offset , (0-63) */
#define GCTRL_LOW_PWR    0xD3  /* OLED low power, 0 = Normal, 5 = Low power */

#define GCTRL_REF_VOLT   0x81  /* Set reference voltage, next byte = (0-255) */

#elif (defined(GHW_SSD1303) || defined(GHW_SSD1305) || defined(GHW_SSD1306))
/* OLED controller */
#define GCTRL_MUX_RATIO  0xA8  /* Multiplexer ration (double command) */
#define GCTRL_DC_DC      0xAD  /* DC-DC converter on off (por = on) */
#define GCTRL_DOFFSET    0xD3  /* OLED display offset , (0-63) */
#define GCTRL_OSC_DIV    0xd5  /* Divide ratio, Osc freq (double command) */
#define GCTRL_COLOR_MODE 0xd8  /* "Color" mode (double command) POR = monocrome*/
#define GCTRL_PRE_CHA    0xD9  /* OLED precharge & zero select, Pre-charge mode, zero mode (1-15)<<4 | (1-15) */
#define GCTRL_COM_CFG    0xDA  /* Com scan seq mapping */

#define GCTRL_REF_VOLT   0x81  /* Set reference voltage, next byte = (0-255) */
#define GCTRL_CB_BRIGHT  0x82  /* Set brightness for color banks, next byte = (0-255) */

#define GCTRL_AREA_LUT   0x91  /* Set current drive pulse width of bank 0, Colour A, B and C. next byte = (0-63) */
                               /* next1:   pulsew (0-63) default 49 */
                               /* next2-4: pulsew color A,B,C (0-63), default 63 */
#define GCTRL_CB_COL_P1  0x92  /* Set bank colour of for bank 1-16 (Page 0), next1-4 bytes = (0-255) */
#define GCTRL_CB_COL_P2  0x93  /* Set bank colour of for bank 17-32 (Page 1), next1-4 bytes = (0-255) */

/* Additional OLED SSD1305 registers (currently not used) */
#define GCTRL_MADR_MODE  0x20  /* Memory addressing mode, next byte: 0x00 Horizontal */
                                                                  /* 0x01 Vertical */
                                                                  /* 0x02 Page (default) */
#define GCTRL_COL_STAEND 0x21  /* Column start and end addresses */
                                 /* next1 byte: Start address (0-131) default 0 */
                                 /* next2 byte: End address (0-131) default 131 */
#define GCTRL_PG_STAEND  0x22  /* Page start and end addresses */
                                 /* next1 byte: Start page (0-7) default 0 */
                                 /* next2 byte: End page (0-7) default 7 */
#define GCTRL_DIMMODE    0xAB  /* Dim mode next1 0-15, contrast next2 0-255, bright next3 0-255 */

#ifdef GHW_SSD1306

#define GCTRL_CHARGEPUMP 0x8d  /*  */

#endif

#else  /* others KS07xx */



  #define GCTRL_REF_VOLT 0x81  /* Set reference voltage, next byte = (0-63) */
  #define GCTRL_LCD_BIAS 0xA2  /* Set bias | (2-3) */

/* Extra command for NJU6676 in addition to KS07xx default */
#ifdef GHW_NJU6676
#define GCTRL_DRIVER     0xE6  /* Driver-on-off ctrl | (0-1) */
#endif

/* Extra double command registers for EL43102 in addition to KS07xx default */
#ifdef GHW_EL43102
#define  GCTRL_CL_FREQ     0x82 /* Set CL frequency ration */
#define  GCTRL_DUTY_RATIO  0x84 /* Set duty ration mode */
#define  GCTRL_BIAS_SELECT 0x86 /* Set LCD bias select */
#endif

#endif

#endif  /* GHW_S1D15719 */

/* Controller commands (2 byte) */
#define GCTRL_INDICATOR  0xAC  /* Set static indicator mode (0=off,1=On)
                                  next byte : (0=Off 1,2= Blinking 3=On) */

/* Internal functions and types used only by other ghw_xxx functions */

#ifdef GBASIC_INIT_ERR
extern GCODE SGUCHAR FCODE pixymsk[8];     /* Bit mask values */
SGUCHAR ghw_sta( void ); /* For testing of status flags */
void ghw_cmd( SGUCHAR cmd );
void ghw_set_xypos(GXT x, GYT y);
void ghw_auto_wr(SGUCHAR ydat);
#ifndef GBUFFER
void ghw_wr( SGUCHAR val );
#endif

#endif /* GBASIC_INIT_ERR */

extern SGUCHAR GFAST ghw_tmpb[8];
extern SGUCHAR GFAST ghw_tmpb2[8];
extern GCODE SGUCHAR FCODE startmask[8];
extern GCODE SGUCHAR FCODE stopmask[8];

#ifndef GBUFFER
SGUCHAR ghw_rd( void );
SGUCHAR ghw_auto_rd(void);
void ghw_auto_rd_start(void);
#endif

#ifdef GHW_USING_VBYTE
void ghw_buf_xwr( SGUCHAR *bufptr, GYT y, SGUCHAR dat );
#endif

/*
   Internal data types used only by ghw_xxx functions
   The data types are located in ghw_init
*/

extern SGBOOL glcd_err;        /* Internal hdw error */

#define  GBUFSIZE (((GDISPH+(GDISPCH-1))/GDISPCH) * GDISPW)

#ifdef GBUFFER
   extern SGBOOL ghw_upddelay;

   /* "Dirty area" buffer controls for ghw_update speed optimization */
   extern GXT GFAST iltx,irbx;
   extern GYT GFAST ilty,irby;

   #define invalx( irx ) { \
      GXT rirx; \
      rirx = (GXT)(irx); \
      if(  irbx < iltx ) iltx = irbx = rirx; \
      else if( rirx < iltx ) iltx = rirx; \
      else if( rirx > irbx ) irbx = rirx; \
      }

   #define invaly( iry ) { \
      GYT riry; \
      riry = (GYT)(iry); \
      if( irby < ilty) ilty = irby = riry; \
      else if( riry < ilty ) ilty = riry; \
      else if( riry > irby ) irby = riry; \
      }

   #define invalrect( irx, iry ) { \
      invalx( irx ); \
      invaly( iry ); \
      }

   #ifdef GHW_ALLOCATE_BUF
      extern   SGUCHAR *gbuf;                  /* Graphic buffer pointer */
      #define GBUF_CHECK()  {if (gbuf == NULL) {glcd_err=1;return;}}
   #else
      extern   SGUCHAR gbuf[GBUFSIZE];         /* Graphic buffer */
      #define GBUF_CHECK()  { /* Nothing */ }
   #endif


   /* Structure to save the low-level state information */
   typedef struct _GHW_STATE
      {
      SGUCHAR upddelay;  /* Store for ghw_update */
      #ifndef GNOCURSOR
      GCURSOR cursor;    /* Store for ghw_cursor */
      #endif
      GXT xpos;          /* Store for local xpos tracking */
      GYT ypos;          /* Store for local ypos tracking */
      } GHW_STATE;

   #ifdef GHW_USING_VBYTE
     #define  GINDEX(x,y) ((GBUFINT)(x) + ((GBUFINT)(y))*GDISPW)
   #else
     #define  GINDEX(x,y) (((GBUFINT)(GDISPW/GDISPCW - 1) - (x)) * GDISPH + ((GBUFINT)(y)))
   #endif

#else  /* GBUFFER */

   #ifdef GHW_ALLOCATE_BUF
     #undef GHW_ALLOCATE_BUF /* Allocation must only be active in buffered mode */
   #endif
   #define GBUF_CHECK()  { /* Nothing */ }

#endif /* GBUFFER */

#ifdef __cplusplus
}
#endif


#endif /* KS07XX_H */
