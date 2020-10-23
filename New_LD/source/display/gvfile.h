#ifndef GETVFILE_H
#define GETVFILE_H
/************************ GVFILE.H *************************

   VIRTUAL FONT "FILE" INTERFACE

   Revision date:
   Revision Purpose:

   Version number: 1.0
   Copyright (c) RAMTEX International ApS 2012
***********************************************************/
#include <gdisphw.h>  /* glcd types incl getvmem.h types */

#ifdef __cplusplus
extern "C" {
#endif


#if defined( GVIRTUAL_FILES ) || defined( GVIRTUAL_FILES_STATIC )
typedef struct _GV_FILE    /* Virtual "file" stucture (RAM object) */
   {
   GXT struct_id;          /* Extended structure ID (= 0) */
   GYT type_id;            /* Virtual file structure type (id = 0x40 for virtual files ) */
   GFONTDEVICE device_id;  /* Virtual font device identifer (passed to device loader) */
   GFONTBUFIDX length;     /* Byte length of binary block */
   GFONTBUFIDX blockidx;   /* Storage index for base of binary block */
   GFONTBUFIDX rdidx;      /* Block read index */
   } GV_FILE;

#define GV_FILE_ID    0x40 /* type_id parameter */

//typedef GV_FILE G_FILE;    /* G_FILE name reserved for future extensions */

/* "file" block read */
GFONTBUFIDX gvfread( GV_FILE *fp, GFONTBUFIDX rdindex, SGUCHAR *buf, GFONTBUFIDX loadsize); /* gfread.c */
/* "file stream" functions */
SGINT gvfgetch( GV_FILE *fp);  /* gfgetch.c */
void gvfungetch( GV_FILE *fp); /* gfungetch.c*/
SGUCHAR gvfeof( GV_FILE *fp);  /* gfeof.c */
void gvfseek( GV_FILE *fp, GFONTBUFIDX index); /* gfseek.c */
GFONTBUFIDX gvfsize( GV_FILE *fp); /* gfsize.c */
GFONTBUFIDX gvftell( GV_FILE *fp); /* gftell.c */
void gvfclose(GV_FILE *fp);    /* gfclose.c */
#define GVFILE_IS_OPEN(fp) (((fp) == NULL) ? 0 : ((fp)->type_id == GV_FILE_ID))
#else
#define gvfread( fp, rdindex, buf, loadsize) 0
#define gvfclose(fp) { /* Nothing */ }
#define gvfeof( fp ) 1
#define gvfgetch( fp) -1
#define gvfungetch(fp) { /* Nothing */ }
#define gvfseek( fp, index) { /* Nothing */ }
#define gvfsize(fp) 0
#define gvftell(fp) 0
#define GVFILE_IS_OPEN(gvf) 0

#endif /* GVIRTUAL_FILES || GVIRTUAL_FILES_STATIC */

#ifdef GVIRTUAL_FILES_STATIC
typedef struct _GV_FILE_DESC  /* Static (fast) "file" lookup descriptor (ROM object) */
   {
   GXT         struct_id;  /* Extended structure ID (= 0) */
   GYT         type_id;    /* Virtual file structure type (id = 0x50 for static lookup descriptions) */
   GFONTDEVICE device_id;  /* Virtual font device identifer (passed to device loader) */
   GFONTBUFIDX length;     /* Byte length of binary block */
   GFONTBUFIDX blockidx;   /* Storage index for base of binary block */
   } GV_FILE_DESC;

typedef GCODE GV_FILE_DESC PFCODE * PGV_FILE_DESC;

#define GV_FILE_STATIC_ID 0x50 /* type_id parameter */

/* Open VF read-only "file" via a static location descriptor (fast) */
SGUCHAR gvfopen_desc( GV_FILE *fp, PGV_FILE_DESC filedesc);
#else
#define gvfopen_desc( fp, filedesc) 1
#endif

#ifdef GVIRTUAL_FILES
/* Open VF read-only "file" via a "name" key (flexible) */
SGUCHAR gvfopenrd( GV_FILE *fbin, PGCSTR binname, GFONTDEVICE device_id);
#else
#define gvfopenrd( fbin, binname, device_id) 1
#endif

#ifdef __cplusplus
}
#endif

#endif

