/***********************************************************************
 *
 * STIMGATE I/O port definition for the target system.
 *
 *   Target processor CPU family :
 *   Target processor device     :
 *   Target compiler             : PCMODE
 *
 * This file is included by SGIO.H to preprocess the sgxxx() port functions
 * when the C source file is compiled for execution in the PC.
 *
 * The file can be generated with the SGSETUP program.
 * SGSETUP use this file for back-annotation and to generate a corresponding
 * file: SGIO_TA.H for the target C-compiler.
 * NOTE : Do not modify this file directly, unless you are absolutely sure
 * of what you are doing.
 *
 * V3.05  STIMGATE Copyright (c) RAMTEX International 1997-2002
 *
 **********************************************************************/

 SGPORTELEM( WR, MM, BY, 0x0000, 0, GHWCMD ), /*Graphic LCD chip command write*/
 SGPORTELEM( RD, MM, BY, 0x0000, 0, GHWSTA ), /*Graphic LCD chip status read*/
 SGPORTELEM( WR, MM, BY, 0x0001, 0, GHWWR  ), /*Graphic LCD chip data write*/
 SGPORTELEM( RD, MM, BY, 0x0001, 0, GHWRD  ), /*Graphic LCD chip data read*/
