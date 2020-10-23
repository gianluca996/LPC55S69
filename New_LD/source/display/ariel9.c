/****************************   ariel9.c **********************

   ariel9 font table and code page structure definitions.
   This file has been auto-generated with the IconEdit tool.

   Copyright(c) RAMTEX 1998-2009

*****************************************************************/
#include "gdisphw.h"

/* Code page entry (one codepage range element) */
static struct
   {
   GCPHEAD chp;
   GCP_RANGE cpr[1];     /* Adjust this index if more codepage segments are added */
   }
GCODE FCODE ariel9cp =
   {
   #include "ariel9.cp" /* Codepage table */
   };

/* Symbol table entry with fixed sized symbols */
static struct
   {
   GSYMHEAD sh;         /* Symbol header */
   SGUCHAR b[9];       /* Symbol data, "variable length" */
   }
GCODE FCODE ariel9sym[97] =
   {
   #include "ariel9.sym" /* Include symbols */
   };

/* Font structure */
GCODE GFONT FCODE ariel9 =
   {
   6,       /* width */
   9,       /* height */
   sizeof(ariel9sym[0])-sizeof(GSYMHEAD),  /* number of bytes in a symbol (including any alignment padding)*/
   (PGSYMBOL)ariel9sym,                    /* pointer to array of SYMBOLS */
   97,                                     /* num symbols */   ///INSERITO UN ELEMENTO IN PIU' NEL FILE ariel9.sym__Alessandro Rinaldi 13/05/2016
   (PGCODEPAGE)&ariel9cp
   };

