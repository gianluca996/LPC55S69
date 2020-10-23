/***************************** ariel7x9.c ************************

   ariel7x9 font table structure definitions.
   This file has been auto-generated with the IconEdit tool.

   Copyright RAMTEX 1998-2003

*****************************************************************/
#include "gdisphw.h"

/* Code page entry (one codepage range element) */
static struct
   {
   GCPHEAD chp;
   GCP_RANGE cpr[1];     /* Adjust this index if more codepage segments are added */
   }
GCODE FCODE ariel7x9cp =
   {
   #include "ariel7x9.cp" /* Symbol table */
   };

typedef struct          /* Structure used for automatic word alignment */
   {
   SGUCHAR b[9];       /* Symbol data, "variable length" array */
   } GSYMDAT;

/* Symbol table entry with fixed sized symbols */
static struct
   {
   GSYMHEAD sh;        /* Symbol header */
   SGUCHAR b[9];       /* Symbol data, "variable length" */
   }
GCODE FCODE ariel7x9sym[96] =
   {
   #include "ariel7x9.SYM" /* Include symbols */
   };

/* Font structure */
GCODE GFONT FCODE ariel7x9 =
   {
   6,       /* width */
   9,       /* height */
   sizeof(GSYMDAT),    /* number of bytes in a symbol  (must include any alignment padding)*/
   (PGSYMBOL)ariel7x9sym,/* pointer to array of SYMBOLS */
   96,      /* num symbols */
   (PGCODEPAGE)&ariel7x9cp
   };

