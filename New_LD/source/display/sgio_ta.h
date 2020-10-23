/***********************************************************************
 *
 * This file contains I/O port definition for the target system and is
 * included by sgio.h
 *
 * This file is normally generated automatically by the SGSETUP tool using
 * the sgio_pc.h header file as a template. The definitions here correspond
 * to I/O port definitions in sgio_pc.h but are here using the I/O register
 * definition syntax for the target compiler.
 *
 * The example definitions here assume that the LCD I/O registers are
 * memory mapped at fixed addresses, and that the register SELECT line on
 * the display is connected to address bus bit 0 signal.
 *
 * Modify these definitions so they fit the actual target system and the
 * I/O register definition syntax used by the actual target compiler.
 *
 * Copyright (c) RAMTEX International 2000
 * Version 1.0
 *
 **********************************************************************/
#define GHWWR  (* (SGUCHAR volatile *) ( 0x0001 ))
#define GHWRD  (* (SGUCHAR volatile *) ( 0x0001 ))
#define GHWSTA (* (SGUCHAR volatile *) ( 0x0000 ))
#define GHWCMD (* (SGUCHAR volatile *) ( 0x0000 ))
