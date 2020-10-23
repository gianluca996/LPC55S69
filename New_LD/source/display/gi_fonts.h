/**************************** gi_fonts.h **************************

   Extern definition of fonts and code pages which are supplied with
   the GLCD system

   Custom defined fonts may also be added here.

   Creation date: 980223

   Revision date:     030205
   Revision Purpose:  New fonts added
   Revision date:     041406
   Revision Purpose:  arial18, narrow10 added
   Revision date:     160507
   Revision Purpose:  ms58p, narrow20, uni_16x16 added

   Version number: 2.3
   Copyright (c) RAMTEX Engineering Aps 1998-2007

*******************************************************************/
#ifndef GIFONTS_H
#define GIFONTS_H

#include "gdisphw.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Font definitions */
extern GCODE GFONT FCODE font_big;	
extern GCODE GFONT FCODE ariel7x9;  //inserito Andrea Costa 28/07/2014
extern GCODE GFONT FCODE ariel9;
extern GCODE GFONT FCODE ariel18;
extern GCODE GFONT FCODE cp8859_9;
extern GCODE GFONT FCODE cp8859_14;
extern GCODE GFONT FCODE footnote;
extern GCODE GFONT FCODE rtmono8_8;
extern GCODE GFONT FCODE mono5_8;
extern GCODE GFONT FCODE ms58p;
extern GCODE GFONT FCODE msfont;
extern GCODE GFONT FCODE msfont78;
extern GCODE GFONT FCODE narrow10;
extern GCODE GFONT FCODE narrow20;
extern GCODE GFONT FCODE times9;
extern GCODE GFONT FCODE times13;
extern GCODE GFONT FCODE times16;
#if (defined(GMULTIBYTE) || defined(GWIDECHAR))
/* This font contains more than 256 symbols so it must be
   used with either GMULTIBYTE or GWIDECHAR defined */
extern GCODE GFONT FCODE uni_16x16;
#endif

#ifdef __cplusplus
}
#endif

#endif /* GIFONTS_H */

