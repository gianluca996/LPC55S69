/********************************** gsimintf.h ***********************

   Interface functions for socket connections to the general LCD
   WIN32 simulator program.

   These generic LCD simulator functions are usually only activated
   via the simulator C module which emulates the functionality of a
   specific LCD controller chip.

   Copyright RAMTEX International ApS 2001-2002

   Version 1

*********************************************************************/
#ifndef GSIMINTF_H
#define GSIMINTF_H

#ifdef __cplusplus
extern "C" {
#endif

unsigned char GSimInit(unsigned short w, unsigned short h);
void GSimClose(void);
unsigned char GSimGClr(void);
unsigned char GSimError(void);
unsigned char GSimPutsClr(void);
unsigned char GSimPuts(const char *str);
unsigned char GSimWrBit(unsigned short x, unsigned short y, unsigned char val);
unsigned char GSimFlush(void);
unsigned short GSimKbHit(void);
unsigned short GSimKbGet(void); 
unsigned long GSimTimestamp(void);
unsigned char GSimTouchGet(unsigned char *eventp, unsigned char *levelp, unsigned short *xp, unsigned short *yp);


#ifdef __cplusplus
}
#endif

#endif
