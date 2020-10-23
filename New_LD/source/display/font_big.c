/*
 * font_big.c
 *
 *  Created on: Oct 6, 2014
 *      Author: alvaro.patacchiola
 */
#include "gdisphw.h"

/* Code page entry (one codepage range element) */
static struct
   {
   GCPHEAD chp;
   GCP_RANGE cpr[1];     /* Adjust this index if more codepage segments are added */
   }
GCODE FCODE font_bigcp =
   {
   #include "font_big.cp" /* Symbol table */
   };

typedef struct          /* Structure used for automatic word alignment */
   {
   SGUCHAR b[52];       /* Symbol data, "variable length" array */
   } GSYMDAT;

/* Symbol table entry with fixed sized symbols */
static struct
   {
   GSYMHEAD sh;        /* Symbol header */
   SGUCHAR b[52];       /* Symbol data, "variable length" */
   }
GCODE FCODE font_bigsym[15] =
   {
   #include "font_big.sym" /* Include symbols */
   };

/* Font structure */
GCODE GFONT FCODE font_big =
   {
   13,       /* width */
   26,       /* height */
   sizeof(GSYMDAT),    /* number of bytes in a symbol  (must include any alignment padding)*/
   (PGSYMBOL)font_bigsym,/* pointer to array of SYMBOLS */
   15,      /* num symbols */
   (PGCODEPAGE)&font_bigcp
   };



