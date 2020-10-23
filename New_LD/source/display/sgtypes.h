/*---------------------------------------------------------------------------*
 * Generic template for sgtypes.h
 *
 * This is a reduced and simplified version of the SGTYPES.H file generated with
 * the SGSETUP tool. Modify this file to fit the target C compiler:
 * 
 * This header file contains:
 * -    Definitions of fixed sized data types. 
 * -    Definitions of unions for fast type conversions (during I/O access). 
 *
 * Note: The compiler / linker must use byte alignment of structures, when the
 * sg_unions are used for fast type conversion during I/O access.
 * 
 * This file is included via SGIO.H or directly in the source code.
 * 
 * Copyright (c) RAMTEX International 2001
 * Version 1.0
 * 
 * A description of the SG syntax can be found at: 
 *     www.ramtex.dk/standard/sgsyntax.htm
 *
 *--------------------------------------------------------------------------*/
#ifndef SGTYPESH
#define SGTYPESH

#ifndef SGPCMODE
 #if (defined (__MSDOS__) || defined (MSDOS) || defined(_WIN32))
   #define SGPCMODE
 #else
   #define SGTAMODE
 #endif
#endif

#ifdef SGPCMODE
   /* This part is used when compiling with PC compilers
      These definitions should not need modifications */
   #ifndef NULL
   #define NULL ((void *) 0x0)
   #endif

   #define sgfnc /* nothing */ 
   #ifdef _WIN32
   #define far   /* nothing */
   #endif

   #ifndef SGUCHAR
   #define SGUCHAR unsigned char   /* 8 bit */
   #endif
   #ifndef SGCHAR
   #define SGCHAR signed char      /* 8 bit */
   #endif
   #ifndef SGUINT
   #define SGUINT unsigned short   /* 16 bit */
   #endif
   #ifndef SGINT
   #define SGINT signed short      /* 16 bit */
   #endif
   #ifndef SGULONG
   #define SGULONG unsigned long   /* 32 bit */
   #endif
   #ifndef SGLONG
   #define SGLONG signed long      /* 32 bit */
   #endif
   #ifndef SGBOOL
   #define SGBOOL unsigned char    /* (1 bit) 0/1 */
   #endif

   /* The PC compiler use <LOW>..<HIGH> representation */
   typedef union    /* 8 / 16 bit IO conversion union */
      {
      SGUINT w;            /* unsigned int */
      SGINT i;             /* int */
      struct
         {                       
         SGUCHAR b0;       /* LSB byte */
         SGUCHAR b1;       /* MSB byte */
         }b;
      } sg_union16;

   typedef union   /* 8 / 16 / 32 bit IO conversion union */
      {
      SGLONG l;                    
      SGULONG ul;          
      struct
         {                       
         SGUINT w0;        /* LSB word */
         SGUINT w1;        /* MSB word */
         }w;
      struct
         {
         SGUCHAR b0;       /* LSB byte */
         SGUCHAR b1;
         SGUCHAR b2;
         SGUCHAR b3;       /* MSB byte (exp) */
         }b;
      } sg_union32;

#else
   /* This part is used when compiling with target compilers.

      The SG fixed size integer definitions below makes library source code
      portable across compilers.

      Modify these definitions to fit the integer size of the target C
      compiler if necessary.
   */
   #ifndef NULL
   #define NULL 0
   #endif

   #ifndef SGUCHAR
   #define SGUCHAR unsigned char  /* 8 bit */
   #endif
   #ifndef SGCHAR
   #define SGCHAR  signed char    /* 8 bit */
   #endif
   #ifndef SGUINT
   #define SGUINT  unsigned short /* 16 bit */
   #endif
   #ifndef SGINT
   #define SGINT   signed short   /* 16 bit */
   #endif
   #ifndef SGULONG
   #define SGULONG unsigned long  /* 32 bit */ /*  Cosmic 6808 v4.1 error */
   #endif
   #ifndef SGLONG
   #define SGLONG  signed long    /* 32 bit */
   #endif
   #ifndef SGBOOL
   #define SGBOOL  char           /* (1 bit) 0/1. Optionally use data bit */
   #endif                         /* if supported by compiler and processor*/

   /* The sg_unions are for fast conversion between integer types, for
      instance during I/O access.
      Byte alignment of data must be used by the compiler, linker.

      The union definitions below assumes that the Target compiler uses a
      <HIGH>..<LOW> byte ordering of data types. 
      Rearrange the ordering to fit the target compiler endian if necessary */

   typedef union    /* 8 / 16 bit IO conversion union */
      {
      SGUINT w;            
      SGINT i;             
      struct
         {                       
         SGUCHAR b1;       /* MSB byte */
         SGUCHAR b0;       /* LSB byte */
         }b;
      } sg_union16;

   typedef union   /* 8 / 16 / 32 bit IO conversion union */
      {
      SGULONG ul;          
      SGLONG l;                    
      struct
         {                       
         SGUINT w1;        /* MSB word */
         SGUINT w0;        /* LSB word */
         }w;
      struct
         {                       
         SGUCHAR b3;       /* MSB byte (exp) */
         SGUCHAR b2;
         SGUCHAR b1;
         SGUCHAR b0;       /* LSB byte */
         }b;
      } sg_union32;

#endif
#endif
