#ifndef GDISPHW_H
#define GDISPHW_H
/************************ GDISPHW.H *************************

   Prototypes and definitions for hardware related functions
   and test messages

   Creation date: 980101

   Revision date:
   Revision Purpose:  Selective support of hardware text fonts
   Revision date:     02-01-23
   Revision Purpose:  symsize parameter added to gi_putsymbol(..)
   Revision date:     020123
   Revision Purpose:  update of invalid-rectancle handling
   Revision date:     020139
   Revision Purpose:  Vertical soft cursor support
   Revision date:     020319
   Revision Purpose:  ghw_invert added
   Revision date:     020326
   Revision Purpose:  ghw_rdsym ghw_wrsym added
   Revision date:     020728
   Revision Purpose:  ghw_setupdate & GHW_ALLOCATED_BUF and PSCREENS functions added
   Revision date:     120403
   Revision Purpose:  Support for PGENERIC i.e. intrinsic generic pointers
   Revision date:     171103
   Revision Purpose:  Use of GHW_BLK_SIZE macro added
   Revision date:     030804
   Revision Purpose:  Now includes gsimintf.h in simulator mode
   Revision date:     260405
   Revision Purpose:  Include of gi_fonts.h added to simlify user handling.
   Revision date:     190407
   Revision Purpose:  GMULTIBYTE_UTF8 support added
   Revision date:     191207
   Revision Purpose:  Use of WCHAR_MAX introduced
   Revision date:     21-07-08
   Revision Purpose:  Major redesign of gdispcfg.h file layout. Most compilation switch
                      checking moved to this module.
                      GNOTXTSPACE introduced.
   Revision date:     17-04-2009
   Revision Purpose:  Virtual font structures split in RAM dependent and constant
                      data structures
                      (to handle compilers using non-standard C conformant pointers)
   Revision date:     11-11-10
   Revision Purpose:  GPOS definition moved here from gi_disp.h to be "public"
   Revision date:     15-05-2012
   Revision Purpose:  New GSYM_SIZE(w,h) macro
                      gsymsize(..) gsymsize_vp(..) converted to macros defined here
   Revision date:     4-05-2012
   Revision Purpose:  GBUFPIXPSU default handling added (so storage unit size may be controlled via gdispcfg.h)
   Revision date:     14-11-12
   Revision Purpose:  GVIRTUAL_FONTS_DYN support added

   Version number: 4.5
   Copyright (c) RAMTEX Engineering Aps 1998-2013

*************************************************************/

#include <stdlib.h>   /* Compiler NULL definition, abs, malloc */
#include "sgtypes.h"  /* STIMGATE type definitions */

#ifdef __cplusplus
extern "C" {
#endif

/* Avoid double definitions. gdispcfg.h must only be included here */

#include "gdispcfg.h" /* Configurations file for LCD driver */

/********* Clear up any conflicting configuration switch settings ********/

/*
    Verify soft switch setup and assure valid combinations

    Application level features
      GVIEWPORT      Enable high-level functions and features (viewports)
      GGRAPHICS      Graphic drawing features (primary switch)
      GSOFT_SYMBOLS  Graphic symbol drawing, like gputsym() (primary switch)
      GSOFT_FONTS    Enable soft font handling (either with text or softsymbols)
      GBASIC_TEXT    Enable text handling (either with soft font or hard fonts)

      GCONSTTAB       No tabulator table generated if defined
      GNOCURSOR       No cursor handling genrated if defined
      GNOTXTSPACE     No extra character or line space handling enabled

      These text string handling features can be used without viewports
      GMULTIBYTE
      GMULTIBYTE_UTF8
      GWIDECHAR
      GBASIC_INITERR

   If GWARNING is defined in gdispcfg.h then diagnostic error messages
   is shown here at compile time if seriously conflicting switches are detected,
   else the conflicting switch setting is just corrected silently
*/

/* Validate GSCREENS */
#ifdef GSCREENS
  #ifndef GVIEWPORT
    #ifdef GWARNING
       #error GVIEWPORT must be defined when GSCREENS is enabled in gdispcfg.h
    #endif
    #define GVIEWPORT
  #endif
  #ifndef GBUFFER
    #define GBUFFER
  #endif
  #ifndef GHW_ALLOCATE_BUF
    #define GHW_ALLOCATE_BUF
  #endif
#endif

/* Validate GGRAPHICS */
#ifdef GGRAPHICS
   #ifndef GVIEWPORT
     #ifdef GWARNING
        #error GVIEWPORT must be defined when GGRAPHICS is enabled in gdispcfg.h
     #endif
     #define GVIEWPORT
   #endif
#endif

/* Validate GSOFT_SYMBOLS */
#ifdef GSOFT_SYMBOLS
   #ifndef GVIEWPORT
     #ifdef GWARNING
        #error GVIEWPORT must be defined when GSOFT_SYMBOLS is enabled in gdispcfg.h
     #endif
     #define GVIEWPORT
   #endif
#endif

/* Validate GVIRTUAL_FONTS, GVIRTUAL_FONTS_DYN */
#ifdef GVIRTUAL_FONTS_DYN
   #ifndef GVIRTUAL_FONTS
      /* GVIRTUAL_FONTS must also be defined when GVIRTUAL_FONTS_DYN is enabled in gdispcfg.h */
      #define GVIRTUAL_FONTS
   #endif
#endif
#ifdef GVIRTUAL_FONTS
  #ifndef GSOFT_FONTS
    #ifdef GWARNING
       #error GSOFT_FONTS must be defined when GVIRTUAL_FONTS is enabled in gdispcfg.h
    #endif
    #define GSOFT_FONTS
  #endif
#endif

/* Validate GSOFT_FONTS */
#ifdef GSOFT_FONTS
   #ifndef GSOFT_SYMBOLS
     #ifdef GWARNING
        #error GSOFT_SYMBOLS must be defined when GSOFT_FONTS is enabled in gdispcfg.h
     #endif
     #define GSOFT_SYMBOLS
     #ifndef GVIEWPORT
       #ifdef GWARNING
          #error GVIEWPORT must be defined when GSOFT_FONTS is enabled in gdispcfg.h
       #endif
       #define GVIEWPORT
     #endif
   #endif
#endif

/* Validate GBASIC_TEXT */
#ifdef GBASIC_TEXT
   #ifndef GVIEWPORT
     #ifdef GWARNING
        #error GVIEWPORT must be defined when GBASIC_TEXT is enabled in gdispcfg.h
     #endif
     #define GVIEWPORT
   #endif
   #ifdef GHW_NO_HDW_FONT
      /* No hardware font so soft font & soft symbols MUST be used */
      #ifndef GSOFT_FONTS
         #ifdef GWARNING
            #error GSOFT_FONTS must be defined when GBASIC_TEXT is enabled in gdispcfg.h and there is no hardware fonts
         #endif
         #define GSOFT_FONTS
      #endif
      #ifndef GSOFT_SYMBOLS
        #ifdef GWARNING
           #error GSOFT_SYMBOLS must be defined when GBASIC_TEXT is enabled in gdispcfg.h and there is no hardware fonts
        #endif
        #define GSOFT_SYMBOLS
      #endif
   #endif
   #ifndef GSOFT_FONTS
      /* Only hardware font is used, no pixel level spacing */
      #ifndef GNOTXTSPACE
         #define GNOTXTSPACE
      #endif
   #endif
   #ifndef GS_ALIGN
      #ifndef GNOTXTSPACE
         #define GNOTXTSPACE
      #endif
   #endif
#else
   /* No text handling, so no need for tabs, cursor control and text alignment */
   #ifndef GCONSTTAB
      #define GCONSTTAB
   #endif
   #ifndef GNOCURSOR
      #define GNOCURSOR
   #endif
   #ifdef GS_ALIGN
      #undef GS_ALIGN
   #endif
   #ifndef GNOTXTSPACE
      #define GNOTXTSPACE
   #endif
#endif

/* Validate GVIEWPORT */
#ifdef GVIEWPORT
   #ifndef GNUMVP
    #ifdef GWARNING
       #error Missing GNUMVP setting in gdispcfg.h
    #endif
    #define GNUMVP 1  /* Assure that at least one view port is defined */
   #else
    #if ((GNUMVP <= 0) || (GNUMVP > 255))
     #ifdef GWARNING
        #error Illegal GNUMVP setting in gdispcfg.h. Must be in range {255:1}
     #endif
     #undef GNUMVP
     #define GNUMVP 1  /* Assure that at least one view port is defined */
    #endif
   #endif
#else
   /* No high level features used */
   #ifdef GNUMVP
     #undef GNUMVP
   #endif
   #define GNUMVP 1  /* Just to avoid compilation errors */
   #ifdef GFUNC_VP
      #undef GFUNC_VP
   #endif
   #ifdef GEXTMODE
      #undef GEXTMODE
   #endif
#endif

#ifndef GBASIC_INIT_ERR
  /* No back light and contrast available if no init */
 #ifdef GHW_INTERNAL_BACKLIGHT
  #undef GHW_INTERNAL_BACKLIGHT
 #endif
 #ifdef GHW_INTERNAL_CONTRAST
  #undef GHW_INTERNAL_CONTRAST
 #endif
#endif

/* Insert default hw character width and height if not allready defined in gdispcfg.h */
#ifndef GDISPCW
  #define GDISPCW 8
#endif
#ifndef GDISPCH
  #define GDISPCH 8
#endif
#ifndef GBUFPIXPSU
  #define GBUFPIXPSU 8  /* Pixels pr storage unit */
#endif

/* Optimized type able to hold both X and Y coordinate types */
#ifndef GXYT
  #if ((GDISPW >= 256) || (GDISPH >= 256))
     #define GXYT SGUINT
  #else
     #define GXYT SGUCHAR
  #endif
#endif

/* Clean up memory type qualifiers.
   Change to defaults in PC simulator mode */
#ifdef GHW_PCSIM
  /* Set defaults for target mode only swirches */
  #ifndef   _WIN32
    #define _WIN32  /* _WIN32 is used by the library */
  #endif
  /* Remove existing definitions (compatibility with old configurations ) */
   #ifdef   GFAST
     #undef GFAST
   #endif
   #ifdef   GVFAST
     #undef GVFAST
   #endif
   #ifdef GCONSTP
     #undef GCONSTP
   #endif
   #ifdef   GCODE
     #undef GCODE
   #endif
   #ifdef   FCODE
     #undef FCODE
   #endif
   #ifdef   PFCODE
     #undef PFCODE
   #endif
   #ifdef   PGENERIC
     #undef PGENERIC
   #endif

   #define GFAST    /* nothing */
   #define GVFAST   /* nothing */
   #define GCONSTP  const
   #define GCODE    const
   #define FCODE    /* nothing */
   #define PFCODE   /* nothing */
   #define PGENERIC /* nothing */
#else
 /* Undef simulator mode only switches */
 #ifdef GHW_MINIMIZE_CONSOLE
  #undef GHW_MINIMIZE_CONSOLE
 #endif
 #ifdef GHW_FAST_SIM_UPDATE
  #undef GHW_FAST_SIM_UPDATE
 #endif
#endif

/* Clean up incomplete multibyte configuration settings */
#ifdef GMULTIBYTE_UTF8
  #ifndef GMULTIBYTE
    #define GMULTIBYTE
  #endif
#endif

/* Define generic pointers */
#ifndef PGENERIC
  #define PGENERIC /* nothing */
#endif
/* Define pointer to fixed data or fonts in code memory
  (only used if ROM memory type qualifier is different from const) */
#ifndef PFCODE
  #define PFCODE /* nothing */
#endif

/* Define default const type qualifier used on string pointer types */
#ifndef GCONSTP
  #define GCONSTP const
#endif

/* Define internal (wide) character index types (must be unsigned) */
#ifdef  GWCHAR
 #undef GWCHAR
#endif
#ifdef PGCWCHAR
 #undef PGCWCHAR
#endif

/* Variable used for X and W */
#ifndef GXT
  #if (GDISPW <= 254)
   #define GXT   SGUCHAR
  #else
   #define GXT   SGUINT
  #endif
#endif

/* Variable used for Y and H */
#ifndef GYT
  #if (GDISPH <= 254)
   #define GYT   SGUCHAR
  #else
   #define GYT   SGUINT
  #endif
#endif

/************ End gdispcfg.h clean up ******************/

typedef GCONSTP char PGENERIC * PGCSTR;            /* Pointer to constant string (may be in ROM or RAM) */
typedef char PGENERIC * PGSTR;                     /* Pointer to variable string (must be RAM) */
typedef GCONSTP unsigned char PGENERIC * PGCUCHAR; /* Pointer to constant buffer (may be in ROM or RAM) */
typedef unsigned char PGENERIC * PGUCHAR;          /* Pointer to variable buffer (must be RAM) */

#if (defined(GMULTIBYTE) || defined(GWIDECHAR))
 #ifndef G_MALFORMED_RETURNCHAR
   /* Define default value to use as replacement char in case of multi-byte encoding / decoding errors */
   /* (convinient during multibyte text debugging) */
   #define  G_MALFORMED_RETURNCHAR '?'
 #endif

 #ifdef GWIDECHAR

   #ifdef __cplusplus
     }
   #endif
   #include <wchar.h>      /* Get compiler definition of wchar_t and WCHAR_MAX */

   #if defined( _MSC_VER ) || defined(__BORLANDC__)
     /* Fix C99 standard conformance error in MSVC 6.0 and Borland 5 */
     #ifdef swprintf
       #undef swprintf
     #endif
     #define   swprintf   _snwprintf
   #endif

   #ifdef __cplusplus
     extern "C" {
   #endif
   #define GWCHAR wchar_t  /* Use wide char */
 #else
   #define GWCHAR SGUINT   /* Use local wide char definition */
 #endif

 /* WCHAR_MAX should be defined in <wchar.h>, if not used make a substitute */
 #ifndef WCHAR_MAX
   #define WCHAR_MAX 0xffff /* Max wide char limit */
 #endif

 #ifdef GMULTIBYTE
   #ifndef G_MULTIBYTE_LIMIT
     #define G_MULTIBYTE_LIMIT 0x80  /* Shift multi-byte state if larger or equal */
   #endif
  #endif
#else  /* GMULTIBYTE ||  GWIDECHAR */
  #define GWCHAR SGUCHAR   /* Use narrow char */
  #ifndef WCHAR_MAX
    #define WCHAR_MAX 0xff /* Max "wide char" limit */
  #endif
#endif /* GMULTIBYTE ||  GWIDECHAR */

typedef GWCHAR PGENERIC * PGWSTR;
typedef GCONSTP GWCHAR PGENERIC * PGCWSTR;

#ifdef GHW_PCSIM
   #if _WIN32
      void simputs( SGINT sgstream, const char *chp );
      void simprintf( SGINT sgstream, const char *fmt, ...);
      #pragma warning ( disable : 4761 ) /*  remove : "integral size mismatch in argument : conversion supplied" */
      #pragma warning ( disable : 4244 ) /* remove : "converting from 'unsigned int' to 'unsigned char'" caused by integer */
        #if _MSC_VER >= 1400
         #pragma warning ( disable : 4996 ) /* remove : "function or variable may be unsafe" for pre C99 C syntax */
      #endif
      #define _CRT_SECURE_NO_WARNINGS  /* Remove not secure warnings in C++ Express */
   #else
      #error Compiler not defined
   #endif

   #ifdef GWARNING
      #define G_WARNING( str ) simputs(0,"WARNING " str " in " __FILE__ )
   #else
      #define G_WARNING( str ) /* nothing */
   #endif
   #ifdef GERROR
      #define G_ERROR( str ) simputs(-1,"ERROR " str " in " __FILE__ )
   #else
      #define G_ERROR( str )   /* nothing */
   #endif

#else /* PC-mode or Target-mode */
      #ifdef _WIN32
         /* 1 byte alignment.  For backward compatibility. Can be removed with the newer full standard conforming compilers */
         #pragma warning ( disable : 4761 ) /* remove : integral size mismatch in argument : conversion supplied */
         #pragma warning ( disable : 4103 ) /* used #pragma pack to change alignment */
         #pragma pack(1)
         #define _CRT_SECURE_NO_WARNINGS  /* Remove not secure warnings in C++ Express */
      #endif

      #ifdef GWARNING
         #ifdef SGWINMSG
         #define G_WARNING( str ) sgprintf(0,"WARNING " str " in " __FILE__ )
         #else
            #ifdef _CONSOLE
               #ifdef __cplusplus
               }
               #endif
               #include <stdio.h>
               #ifdef __cplusplus
               extern "C" {
               #endif
               #define G_WARNING( str ) printf("\nWARNING " str " in " __FILE__ )
            #else
               #define G_WARNING( str ) /* nothing (defined by user) */
            #endif
         #endif
      #else
         #define G_WARNING( str ) /* nothing (defined by user)  */
      #endif

      #ifdef GERROR
         #ifdef SGWINMSG
         #define G_ERROR( str ) sgprintf(-1,"ERROR " str " in " __FILE__ )
         #else
            #ifdef _CONSOLE
               #ifdef __cplusplus
               }
               #endif
               #include <stdio.h>
               #ifdef __cplusplus
               extern "C" {
               #endif
               #define G_ERROR( str ) printf("\nERROR " str " in " __FILE__ )
            #else
               #define G_ERROR( str ) /* nothing  (defined by user) */
            #endif
         #endif
      #else
         #define G_ERROR( str ) /* nothing  (defined by user) */
   #endif
#endif

/*********************************************************************
Font definitions
*/

/*********************************************************************
   Segment: Basic initialization and error handling
   Level: HWDriver
   Layout of symbols header
*/
typedef struct _GSYMHEAD /* Symbol header */
   {
   GXT cxpix;       /* Symbol size in no X pixels */
   GYT cypix;       /* Symbol size in no Y pixels */
   } GSYMHEAD;

typedef GCODE GSYMHEAD PFCODE * PGSYMHEAD;

/*
   Layout of (old) b&w font symbols
*/
typedef struct _GBWSYMBOL /* One table entry */
   {
   GSYMHEAD sh;     /* Symbol header */
   SGUCHAR  b[3];   /* Symbol data, variable length = (cxpix/8+1)*cypix */
   } GBWSYMBOL;

typedef GCODE GBWSYMBOL PFCODE * PGBWSYMBOL;

#if defined( GVIRTUAL_FONTS ) || defined( GVIRTUAL_FILES ) || defined( GVIRTUAL_FILES_STATIC )
/* Include definition for the GFONTBUFIDX type */
#ifdef __cplusplus
}
#endif
#include <getvmem.h>      /* GFONTBUFIDX type */
#ifdef __cplusplus
extern "C" {
#endif
#endif /* GVIRTUAL_FONTS || GVIRTUAL_FILES */

#ifdef GVIRTUAL_FONTS

typedef GCODE void PFCODE * PGVOIDC;

typedef struct _GSYMHEADV_V /* Virtual symbol header (variable part) */
   {
   GYT numbits;            /* Number of bits pr pixel (0=b&w mode, >=1 -> color mode) */
   GXT cxpix;              /* Symbol size in no X pixels */
   GYT cypix;              /* Symbol size in no Y pixels */
   GFONTBUFIDX bidx;       /* Symbol data index in virtual storage */
   GBUFINT symsize;        /* Virtual storage size for font symbol */
   GFONTDEVICE device_id;  /* Virtual font device identifer */
   PGVOIDC symbol_font;    /* Storage for PGFONT pointer for font assosiated with virtual font symbol */
   } GSYMHEADV_V;

typedef GSYMHEADV_V * PGSYMHEADV_V;

typedef struct _GSYMHEADV  /* Virtual symbol header (constant part) */
   {
   GXT id0;                /* virtual symbol identifier = 0 */
   GYT id1;                /* virtual symbol identifier = 0 */
   GXT type_id;            /* extended symbol type id (1 = virtual font symbol) */
   PGSYMHEADV_V psymh_v;   /* pointer to variable part of vf symbol header */
   } GSYMHEADV;

typedef GCODE GSYMHEADV PFCODE * PGSYMHEADV;

#endif

/* Generic symbol header */
typedef union _GSYMBOL /* One table entry */
   {
   GSYMHEAD sh;     /* B&W symbol header */
   #ifdef GVIRTUAL_FONTS
   GSYMHEADV vsh;   /* Virtual symbol header */
   #endif
   } GSYMBOL;

typedef GCODE GSYMBOL PFCODE * PGSYMBOL;
typedef GCODE SGUCHAR PFCODE * PGSYMBYTE; /* Pointer to graphic symbol data */

/*********************************************************************
   Segment: Basic initialization and error handling
   Level: HWDriver
   Codepage, defines a codepage.
*/
typedef struct _GCP_RANGE
   {
   GWCHAR min;  /* Minimum value included in range */
   GWCHAR max;  /* Maximum value included in range */
   GWCHAR idx;  /* Index in symbol table for the first value */
   } GCP_RANGE;

typedef GCODE GCP_RANGE PFCODE * PGCP_RANGE;

typedef struct _GCPHEAD /* codepage header */
   {
   GWCHAR cprnum;    /* Number of GCP_RANGE elements ( >=1) */
   GWCHAR def_wch;   /* Default character used when not found in codepage */
   } GCPHEAD;

typedef GCODE GCPHEAD PFCODE * PGCPHEAD;

typedef struct _GCODEPAGE
   {
   GCPHEAD cph;
   GCP_RANGE cpr[1]; /* Dynamic length. Must contain cprnum elements. Minimum is 1 element */
   } GCODEPAGE;

typedef GCODE GCODEPAGE PFCODE * PGCODEPAGE;

/*********************************************************************
   Segment: Basic initialization and error handling
   Level: HWDriver
   Font struct, defines a font with an array of symbols
*/
typedef struct _GFONT
   {
   GXT        symwidth;   /* default width */
   GYT        symheight;  /* default height */
   SGUINT     symsize;    /* number of bytes in a symbol */
   PGSYMBOL   psymbols;   /* pointer to array of GSYMBOL's (may be NULL) */
   SGUINT     numsym;     /* number of symbols in psymbols (if pcodepage == NULL) */
   PGCODEPAGE pcodepage;  /* pointer to default codepage for font (may be NULL) */
   } GFONT;

typedef GCODE GFONT PFCODE * PGFONT;

#ifdef GVIRTUAL_FONTS
/*********************** Virtual font extentions *****************/

typedef GCP_RANGE  * PGCP_RANGE_V; /* Pointer to codepage range element (variable) */

typedef struct _GCPHEADV /* codepage header */
   {
   GWCHAR gcpv_id;          /* = 0 = identifier for extended codepage structure */
   GWCHAR type_id;          /* Extended code page structure type (id = 1 for virtual fonts) */
   GWCHAR cprnum;           /* Number of GCP_RANGE elements ( >=1) */
   GWCHAR def_wch;          /* Default character used when not found in codepage */
   GFONTBUFIDX si_codepage; /* Storage index to base of array of GCP_RANGE elements */
   } GCODEPAGEV;

typedef GCODE GCODEPAGEV PFCODE * PGCODEPAGEV;

typedef struct _GFONTV
   {
   GXT         gfontv_id;  /* GFONT default width = 0 -> id for extended font structure */
   GYT         type_id;    /* Extended font structure type (id = 1 for virtual fonts) */
   SGUCHAR     numbits;    /* Number of bits pr pixel (0=b&w mode, >=1 -> color mode (not supported by this library)) */
   SGUCHAR     reserved;   /* Reserved fill parameter. Type assure structure word alignment) */
   GXT         symwidth;   /* default width */
   GYT         symheight;  /* default height */
   SGUCHAR     chsp;       /* Text character spacing (0=normal) */
   SGUCHAR     lnsp;       /* Text text line spacing (0=normal) */
   SGUINT      numsym;     /* number of symbols in psymbols (if pcodepage == NULL) */
   GBUFINT     symsize;    /* number of bytes in a symbol */
   PGCODEPAGEV pcodepage;  /* pointer to default codepage for font (may be 0 if no codepage is used) */
   void *      extention;  /* Font extention (reserved for compatibility) (=NULL)*/
   GFONTBUFIDX si_symbols; /* storage index to base of array of GSYMBOL's */
   GFONTDEVICE device_id;  /* Virtual font device identifer
                             (application driver specific type defined in getvmem.h)*/
   } GFONTV;

typedef GCODE GFONTV PFCODE * PGFONTV;

#ifdef GVIRTUAL_FONTS_DYN

typedef struct _GFONTV_DYN  /* Dynamic (flexible) font lookup descriptor (ROM part) */
   {
   GXT         gfontv_id;  /* GFONT default width = 0 -> id for extended font structure */
   GYT         type_id;    /* Extended font structure type (id = 3 for virtual named fonts (using dynamic font info load to dedicated buffer) */
   GFONTDEVICE device_id;  /* Virtual font device identifer (passed to device loader) */
   PGCSTR      fontname;   /* Pointer to font name string in ROM (used for font location search in virtual image) */
   } GFONTV_DYN;

typedef GCODE GFONTV_DYN PFCODE * PGFONTV_DYN;

typedef struct _GFONTVDYN   /* Dynamic virtual font (RAM part) */
   {
   GXT         gfontv_id;   /* GFONT default width = 0 -> id for extended font structure */
   GYT         type_id;     /* Extended font structure type (id = 2,3 for dynamic virtual fonts) */
   SGUCHAR     numbits;     /* Symbol number of bits pr pixel (0=b&w mode, >=1 -> color mode) */
   SGUCHAR     reserved;    /* Load management flags (0 for ROM storage) */
   GXT         symwidth;    /* Symbol default width */
   GYT         symheight;   /* Symbol default height */
   SGUCHAR     chsp;        /* Text character pixel spacing (0=normal) */
   SGUCHAR     lnsp;        /* Text line pixel spacing (0=normal) */
   SGUINT      numsym;      /* number of symbols in psymbols (if pcodepage == NULL) */
   GBUFINT     symsize;     /* number of bytes in a symbol */
   void *      extention;   /* Font parameter extention (reserved for compatibility) (=NULL) */
   GFONTBUFIDX si_symbols;  /* storage index in virtual memory to base of array of GSYMBOL's */
   GFONTDEVICE device_id;   /* Virtual font device identifer */
   /* Dyn font codepage info (cprnum = 0 -> no codepage) */
   GWCHAR      cprnum;      /* Number of GCP_RANGE elements ( >=1 if codepages are used) */
   GWCHAR      def_wch;     /* Default character used when not found in codepage */
   GFONTBUFIDX si_codepage; /* Storage index to base of array of GCP_RANGE elements */
   } GFONTVDYN;

typedef struct _GFONTV_DYN_BUF  /* Dynamic (flexible) lookup font descriptor (ROM part) with dedicated font buffer (RAM part) */
   {
   GXT         gfontv_id;   /* GFONT default width = 0 -> id for extended font structure */
   GYT         type_id;     /* Extended font structure type (id = 2 for virtual named fonts (using dynamic font info load in shared cashe) */
   GFONTDEVICE device_id;   /* Virtual font device identifer (passed to device loader) */
   PGCSTR      fontname;    /* Pointer to font name string in ROM (used for font location search in virtual image) */
   GFONTVDYN   *fontbuf;    /* Ponter to dynamic loadable font part */
   } GFONTV_DYN_BUF;

typedef GCODE GFONTV_DYN_BUF PFCODE * PGFONTV_DYN_BUF;

#endif /* GVIRTUAL_FONTS_DYN */
#endif /* GVIRTUAL_FONTS */

#ifdef GVIRTUAL_FONTS_DYN
void gfontv_clr( PGFONT *pfont);  /* gfontv.c */
#else
#define gfontv_clr( pfont)        /* nothing */
#endif


/*** Font type classification and font element access macros ***/
#ifdef GVIRTUAL_FONTS
  #ifdef GVIRTUAL_FONTS_DYN
    #define gisfontv_named(pfont)   (((((PGFONTV)(pfont))->type_id   & 0x2)!=0) ? 1 : 0)
    #define gissymbolv_named(psym)  (((((PGSYMHEADV)(psym))->type_id & 0x2)!=0) ? 1 : 0)
    #define giscpv_named(pcp)       (((((PGCODEPAGEV)(pcp))->type_id & 0x2)!=0) ? 1 : 0)
    PGCODEPAGE gi_fpcodepage( PGFONT pcodepage);  /* gfsel.c */
  #endif  /* GVIRTUAL_FONTS_DYN */

  #define gisfontv(pfont)      (((PGFONT)(pfont)==NULL) ? 0 : (((PGFONT)(pfont))->symwidth == 0))
  #define gisfontcpv(pcp)      ((((PGCODEPAGE)(pcp))->cph.cprnum == 0) ? 1 : 0)
  #define gissymbolv(psym)     ((((PGSYMBOL)(psym))->sh.cypix == 0) ? 1 : 0)

  GXT gsymw(PGSYMBOL psymbol);
  GYT gsymh(PGSYMBOL psymbol);

  /* (Internal) access macros for font structure elements */
  #ifdef GVIRTUAL_FONTS_DYN
    extern GFONTVDYN *gvf_curfont; /* Pointer to RAM image part for current font */
    #define gi_fsymh(pfont)      ((gisfontv(pfont)==0) ? (((PGFONT) (pfont))->symheight) : ((gisfontv_named(pfont)==0) ? (((PGFONTV)(pfont))->symheight) : (gvf_curfont->symheight)))
    #define gi_fsymw(pfont)      ((gisfontv(pfont)==0) ? (((PGFONT) (pfont))->symwidth)  : ((gisfontv_named(pfont)==0) ? (((PGFONTV)(pfont))->symwidth)  : (gvf_curfont->symwidth)))
    #define gi_fsymsize(pfont)   ((gisfontv(pfont)==0) ? (((PGFONT) (pfont))->symsize)   : ((gisfontv_named(pfont)==0) ? (((PGFONTV)(pfont))->symsize)   : (gvf_curfont->symsize)))
    #define gi_fnumsym(pfont)    ((gisfontv(pfont)==0) ? (((PGFONT) (pfont))->numsym)    : ((gisfontv_named(pfont)==0) ? (((PGFONTV)(pfont))->numsym)    : (gvf_curfont->numsym)))
    #define gvfdevice(pfont)     ((gisfontv(pfont)==0) ? ((GFONTDEVICE)0)                : ((gisfontv_named(pfont)==0) ? (((PGFONTV)(pfont))->device_id) : (gvf_curfont->device_id)))
    #define gi_chsp(pfont)       ((gisfontv_named(pfont)==0) ? (((PGFONTV)(pfont))->chsp) : (gvf_curfont->chsp))
    #define gi_lnsp(pfont)       ((gisfontv_named(pfont)==0) ? (((PGFONTV)(pfont))->lnsp) : (gvf_curfont->lnsp))
  #else /* GVIRTUAL_FONTS_DYN */
    #define gi_fsymh(pfont)      (gisfontv(pfont) ? (((PGFONTV)(pfont))->symheight) : (((PGFONT)(pfont))->symheight))
    #define gi_fsymw(pfont)      (gisfontv(pfont) ? (((PGFONTV)(pfont))->symwidth)  : (((PGFONT)(pfont))->symwidth))
    #define gi_fsymsize(pfont)   (gisfontv(pfont) ? (((PGFONTV)(pfont))->symsize)   : (((PGFONT)(pfont))->symsize))
    #define gi_fnumsym(pfont)    (gisfontv(pfont) ? (((PGFONTV)(pfont))->numsym)    : (((PGFONT)(pfont))->numsym))
    #define gi_fpcodepage(pfont) (gisfontv(pfont) ? (PGCODEPAGE)(((PGFONTV)(pfont))->pcodepage) : (PGCODEPAGE)(((PGFONT)(pfont))->pcodepage))
    #define gvfdevice(pfont)     (gisfontv(pfont) ? (((PGFONTV)(pfont))->device_id) : ((GFONTDEVICE)0))
    #define gi_chsp(pfont)       (((PGFONTV)pfont)->chsp)
    #define gi_lnsp(pfont)       (((PGFONTV)pfont)->lnsp)
  #endif  /* GVIRTUAL_FONTS_DYN */

  #define GVTYPEMASK 0x1F
#else
  /* Only default GFONT structures used */
  #define gi_fsymh(pfont)      ((pfont)->symheight)
  #define gi_fsymw(pfont)      ((pfont)->symwidth)
  #define gi_fpcodepage(pfont) ((pfont)->pcodepage)
  #define gi_fsymsize(pfont)   ((pfont)->symsize)
  #define gi_fnumsym(pfont)    ((pfont)->numsym)
  #define gvfdevice(pfont)      ((GFONTDEVICE)0)

  #define gsymw(psymbol) ((psymbol)->sh.cxpix) /* Width of a symbol */
  #define gsymh(psymbol) ((psymbol)->sh.cypix) /* Height of a symbol */

  /* (Internal) access macros for font structure elements */
  #define gi_fsymh(pfont)      ((pfont)->symheight)
  #define gi_fsymw(pfont)      ((pfont)->symwidth)
  #define gi_fpcodepage(pfont) ((pfont)->pcodepage)
  #define gi_fsymsize(pfont)   ((pfont)->symsize)
  #define gi_fnumsym(pfont)    ((pfont)->numsym)
  #define gvfdevice(pfont)     ((GFONTDEVICE)0)
#endif

/* Public access macros for font size info */
#define gfontsize(pfont) (((void*)(pfont)== NULL) ? 0 : gi_fnumsym(pfont))   /* Number of symbols in font */
#define gfsymsize(pfont) (((void*)(pfont)== NULL) ? 0 : gi_fsymsize(pfont))  /* Datasize in number of bytes of each symbol in font */

/* Calculate storage size needed for holding a symbol, ex when fetched with ggetsym(..) */
#define GSYM_SIZE(w,h) ((GBUFINT)(((((GBUFINT)(w))+7)/8)*((GBUFINT)(h))+sizeof(GSYMHEAD)))
/* Old function converted to a macro to be generic */
#define gsymsize(xs,ys,xe,ye) GSYM_SIZE(((xe)-(xs))+1,((ye)-(ys))+1)
/* Old named viewport function converted to macro. Just for backward compatibility, deprecated in new designs */
#define gsymsize_vp(vp,xs,ys,xe,ye) gsymsize(xs,ys,xe,ye)

typedef enum _GCURSOR
   {
   GCURSIZE1 = 0x00,
   GCURSIZE2,
   GCURSIZE3,
   GCURSIZE4,
   GCURSIZE5,
   GCURSIZE6,
   GCURSIZE7,
   GCURSIZE8,
   /* and one of these */
   GCURON = 0x10,
   GCURVERTICAL = 0x20,
   GCURBLINK = 0x40,
   GCURBLOCK = 0x80
   } GCURSOR;

#define GHW_INVERSE      0x80
#define GHW_TRANSPERANT  0x40
#define GHW_PALETTEMASK  (GHW_TRANSPERANT-1)

/**** Low-level functions called directly from application layer ****/

#ifdef GBASIC_INIT_ERR

extern GCODE GFONT FCODE SYSFONT; /* instantiated in ghwinit.c */

extern SGBOOL glcd_err; /* instantiated in ghwinit.c */
SGBOOL ghw_init(void);
void ghw_puterr( PGCSTR str );
SGUCHAR ghw_err(void);
void ghw_exit(void);
void ghw_dispoff(void);
void ghw_dispon(void);

void ghw_io_init(void);         /* Located in ghwioinit.c */
void ghw_io_exit(void);
#else

#define ghw_init() 1
#define ghw_puterr( str ) { /* Nothing */ }
#define ghw_err() 1
#define ghw_dispoff() { /* Nothing */ }
#define ghw_dispon()  { /* Nothing */ }

#define ghw_io_init();  { /* Nothing */ }  /* Located in ghwioinit.c */
#define ghw_io_exit();  { /* Nothing */ }

#endif /* GBASIC_INIT_ERR */


#if (!defined( GHW_NO_HDW_FONT ) && defined(GBASIC_TEXT))
SGBOOL ghw_loadsym( PGSYMBOL psymtab, SGUCHAR nosym, SGUCHAR index);
#else
#define ghw_loadsym( psymtab, nosym, offset)  1 /* Return error flag */
#endif

#ifdef GHW_INTERNAL_CONTRAST
/* The controller uses contrast regulation on chip */
SGUCHAR ghw_cont_set(SGUCHAR contrast);
SGUCHAR ghw_cont_change(SGCHAR contrast_diff);
#else
#define ghw_cont_set(a)    { /* Nothing */ }
#define ghw_cont_change(a) { /* Nothing */ }
#endif

#ifdef GBASIC_INIT_ERR
void ghw_rdblk( GXT ltx, GYT lty, GXT rbx, GYT rby, SGUCHAR *dest, GBUFINT bufsize );
void ghw_wrblk( GXT ltx, GYT lty, GXT rbx, GYT rby, SGUCHAR *src );
void ghw_restoreblk(SGUCHAR *src);
GBUFINT ghw_blksize( GXT ltx, GYT lty, GXT rbx, GYT rby );
#else
#define ghw_rdblk( xs, ys, xe, ye, dest, size ) { /* Nothing */}
#define ghw_wrblk( xs, ys, xe, ye, src  ) { /* Nothing */}
#define ghw_restoreblk( src ) { /* Nothing */}
#define ghw_blksize( xs, ys, xe, ye ) 0
#endif

/*** Functions to be accessed only via highlevel functions *****/

/* gfcursor, gfsel, gvpinit */
#ifndef GNOCURSOR
extern GCURSOR ghw_cursor; /* instantiated in ghwinit.c */
void ghw_setcursor( GCURSOR type ); /* */
#endif

/* gvpinv.c gfcursor.c */
#if (!defined( GNOCURSOR ) && defined (GSOFT_FONTS )) || defined (GGRAPHICS)
void ghw_invert(GXT ltx, GYT lty, GXT rbx, GYT rby);
#endif

/* gfputch.c gvpscroll.c */
#if defined( GBASIC_TEXT ) || defined(GSOFT_FONTS) || defined(GGRAPHIC)
void ghw_gscroll( GXT ltx, GYT lty, GXT rbx, GYT rby, GYT lines, SGUINT pattern );
#endif

#ifdef GVIEWPORT
void ghw_fill( GXT ltx, GYT lty, GXT rbx, GYT rby, SGUINT pattern );

typedef struct _GPOS
   {
   GXT x;
   GYT y;
   } GPOS;

typedef GPOS PGENERIC * PGPOS;
#endif

/* gfputch.c gfsetp.c */
#if !defined( GHW_NO_HDW_FONT )
void ghw_putch( SGUCHAR val );
GXT ghw_getcursorxpos(void);
GYT ghw_getcursorypos(void);
void ghw_setcabspos( GXT xpos, GYT ypos );
#endif

#if !defined( GHW_NO_HDW_FONT ) && (defined( GBASIC_TEXT ) || defined(GSOFT_FONTS) || defined(GGRAPHIC))
void ghw_tscroll( GXT ltx, GYT lty, GXT rbx, GYT rby );
#else
#define ghw_tscroll(xs,ys,xe,ye ) { /* Nothing */ }
#endif

#ifdef GSOFT_SYMBOLS
/* gsymget.c */
void ghw_rdsym(GXT ltx, GYT lty, GXT rbx, GYT rby, PGUCHAR dest, GXT bw, SGCHAR mode);
/* giputsym.c */
void ghw_wrsym(GXT ltx, GYT lty, GXT rbx, GYT rby, PGSYMBYTE src, GXT bw, SGUCHAR mode);
#endif

#ifdef GGRAPHICS
/* ggline.c ggpixel.c gcircle.c */
void ghw_rectangle( GXT ltx, GYT lty, GXT rbx, GYT rby, SGBOOL pixel );
void ghw_setpixel( GXT x, GYT y, SGBOOL pixel );
/* ggpixel.c */
SGBOOL ghw_getpixel( GXT x, GYT y );
#endif

/* ghwicon.c (NOTE: this feature is not supported by all display controllers) */
void ghw_seticon(GXT x, GYT y, SGBOOL pixel );

/* Add simulator definitions */
#if (defined(_WIN32) && defined( GHW_PCSIM))
   /* Stop C declarations */
   #ifdef __cplusplus
     }
   #endif
   /* Add windows simulator interface definitions */
   #include <gsimintf.h>
   /* Restart C declarations */
   #ifdef __cplusplus
     extern "C" {
   #endif
#endif

#if (defined(_WIN32) && defined(IOTESTER_USB))
   /* Stop C declarations */
   #ifdef __cplusplus
     }
   #endif
   #include <iotester.h>  /* iot_sync() prototype */
   /* Restart C declarations */
   #ifdef __cplusplus
     extern "C" {
   #endif
#endif

typedef enum _GUPDATE
   {
   GUPDATE_OFF,  /* Activate delayed update */
   GUPDATE_ON    /* De-activate delayed update, draw pending content */
   } GUPDATE;

#ifdef GBUFFER
  /* Buffered mode */
  void ghw_updatehw(void);
  GUPDATE ghw_setupdate( GUPDATE update );
#else
  #if (defined(_WIN32) && defined( GHW_PCSIM))
     /* Map LCD updates to Simulator flush only */
     #define ghw_updatehw() GSimFlush()
     #define ghw_setupdate(a) GSimFlush()
  #elif (defined(_WIN32) && defined(IOTESTER_USB))
     #define ghw_updatehw()   iot_sync(IOT_SYNC);
     #define ghw_setupdate(a) { /* Nothing */ }
  #else
     /* Neither buffer, nor simulator */
     #define ghw_updatehw() { /* Nothing */ }
     #define ghw_setupdate(a) { /* Nothing */ }
  #endif
#endif

/* Calculation of buffer byte size for ghw_rdblk (at compile time) */
#if ( defined( GHW_USING_VBYTE ) && !defined( GHW_ROTATED )) || \
    (!defined( GHW_USING_VBYTE ) &&  defined( GHW_ROTATED ))
 #define  GHW_BLK_SIZE(ltx,lty,rbx,rby) \
    (((GBUFINT)(rbx) - (ltx) + 1) * ((GBUFINT)(((rby)&~(0x7)) - ((lty)&~(0x7)))/8 + 1) + \
     ((GBUFINT) 2*sizeof(GXT)) + ((GBUFINT)2*sizeof(GYT)) + 1)
#else
#define  GHW_BLK_SIZE(ltx,lty,rbx,rby) \
      ((((GBUFINT)(rbx)/GBUFPIXPSU - (ltx)/GBUFPIXPSU + 1) * ((GBUFINT)((rby) - (lty)) + 1) + 1) * (GBUFPIXPSU/8) + \
       ((GBUFINT) 2*sizeof(GXT)) + ((GBUFINT) 2*sizeof(GYT)))
#endif

#if (defined( GHW_ALLOCATE_BUF ) && defined( GBUFFER ))
  GBUFINT ghw_gbufsize( void );

  #ifdef GSCREENS
    SGUCHAR ghw_is_owner( SGUCHAR *buf );
    SGUCHAR *ghw_save_state( SGUCHAR *buf );
    void ghw_set_state(SGUCHAR *buf, SGUCHAR doinit);
  #else
    #define ghw_is_owner( a ) 0
    #define ghw_save_state(a) NULL
    #define ghw_set_state(a,b) { /* Nothing */ }
  #endif /* GSCREENS */

#else
  #define ghw_gbufsize() 0
  #define ghw_is_owner( a ) 0
  #define ghw_save_state(a) NULL
  #define ghw_set_state(a,b) { /* Nothing */ }
#endif

/* Internal range check / correction macros */
#define GLIMITU(x,u) \
   { \
   if ((x) > (u)) (x)=(u);\
   }

#define GLIMITD(x,d) \
   { \
   if ((x) < (d)) (x)=(d);\
   }

/* Fast / low overhead clear of RAM based object type */
#define G_OBJ_CLR( obj ) \
   {  \
   register unsigned char *p = (unsigned char*)((void *)(&(obj)));  \
   register unsigned int count = sizeof(obj);  \
   while (count-- > 0) *p++ = 0; \
   }

#define G_POBJ_CLR( pobj, objtype ) \
   {  \
   register unsigned char *p = (unsigned char*)((void *)(pobj));  \
   register unsigned int count = sizeof(objtype);  \
   while (count-- > 0) *p++ = 0; \
   }

#ifdef __cplusplus
}
#endif

/* Include prototypes for standard fonts */
#include "gi_fonts.h"

#endif /* GDISPHW_H */

