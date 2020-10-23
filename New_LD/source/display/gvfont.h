#ifndef GVFONT_H
#define GVFONT_H

/*
   Virtual fonts are fonts which codepage data and symbol data
   tables are accessed via a generic device driver function.

   This enables memory consuming parts of a font to be stored
   in virtual memory devices like a serial eeprom, memory cards,
   a flashdisk etc.

   The library can distinguise between compiled-in fonts and
   virtual fonts at runtime. This enables the two font types
   to be mixed in a program in a way so it become transperant
   for the application program code.
*/

#include <gdisphw.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Macros for fetching little endian values (independent of any processor and compiler endians) */
#define  VF16(buf,idx) (((SGUINT)buf[(idx)])|(((SGUINT)buf[1+(idx)])<<8))
#define  VF24(buf,idx) (((SGULONG)buf[(idx)])|(((SGULONG)buf[1+(idx)])<<8)|(((SGULONG)buf[2+(idx)])<<16))
#define  VF32(buf,idx) (((SGULONG)buf[(idx)])|(((SGULONG)buf[1+(idx)])<<8)|(((SGULONG)buf[2+(idx)])<<16)|(((SGULONG)buf[3+(idx)])<<24))

#ifdef GVIRTUAL_FONTS
/* internal support functions */

/*
   Prepare for fast codepage and symbol data lookup (if needed)
   Init and preload a GSYMHEADV structure for the font related parameters
   (Default to first symbol in font)
*/
void gi_fontv_open(PGFONTV fp);

/*
   Return pointer to font range for virtual font
*/
PGCP_RANGE_V gi_fontv_cp( GWCHAR index );

/*
   Return pointer to font range for virtual font
   Initialze a GSYMHEADV structure and load (at least) symbol width information
*/
PGSYMBOL gi_fontv_sym( GWCHAR index );

/*
   Preset virtual symbol interface symbol
*/
void gi_symv_open( PGSYMBOL psymbol, SGUINT bw, GYT offset);

/*
   Return symbol byte for current symbol from virtual storage
   symdatidx = index for byte in symbol data array
   (Called by low-level drivers)
*/
SGUCHAR gi_symv_by(GBUFINT symdatidx);

#if defined( GVIRTUAL_FONTS_DYN)
/*
   Prepare use of named font (called by high-level functions)
   (Update font and location info in RAM (or font cashe) )
*/
SGUCHAR gvf_open( PGFONT pfont );

/*
   Prepare use of symbol in named font  (called by high-level functions)
   (Update font and symbol location info in RAM (or font cashe) )
*/
SGUCHAR gvfsym_open( PGSYMBOL psym );

/*
   Initialize virtual font cashe before use (called via ginit())
*/
void gvf_init( void );
#endif /* GVIRTUAL_FONTS_DYN */
#endif /* GVIRTUAL_FONTS */

#if defined( GVIRTUAL_FONTS_DYN) || defined( GVIRTUAL_FILES )

/* Internal function for search for directory block types in *.bin files */
typedef struct
   {
   SGULONG binidx;      /* Start of (next) blok header (must be initialized to 0 before first call of gi_dirblk_seek() )*/
   SGULONG binblksize;  /* Blok size (incl header) */
   SGULONG blkstart;    /* Start of (directory) block data */
   SGUINT  numelem;     /* Number of entries in directory */
   } G_SEEK_DIRBLK;

SGUCHAR gi_dirblk_seek(G_SEEK_DIRBLK *dir, GFONTDEVICE device_id, SGUINT dirtype);
#endif

#ifdef __cplusplus
}
#endif

#endif
