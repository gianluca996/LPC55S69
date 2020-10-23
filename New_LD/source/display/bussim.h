#ifndef BUSSIM_H
#define BUSSIM_H
/***************************** bussim.c ************************************

   Definitions for the LCD bus simulator access functions.

   The definitions below should be modified to reflect the port layout in
   the target system and the given access function implementation.

   The defintions below assume the following hardware bit functionality of
   the "address" byte parameter to the access functions.

      A0 = RS  (Data/command select)

   RAMTEX Engineering Aps 2002

****************************************************************************/

#define  GHWCMD 0x00 /*Graphic LCD chip command write*/
#define  GHWSTA 0x03 /*Graphic LCD chip status read*/
#define  GHWWR  0x01 /*Graphic LCD chip data write*/
#define  GHWRD  0x03 /*Graphic LCD chip data read*/

/*************** Do not modify the definitions below ********************/
#include "gdisphw.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GBUFFER
   #error GBUFFER must be defined in serial mode so the library can do read-modify-write operations
#endif
/* Single chip mode -> access via user supplied access driver functions */

void simwrby( SGUCHAR adr, SGUCHAR dat);
#define simrdby( a ) 0 /* Read is not used in buffered single chip mode */
void sim_reset( void );

#ifdef __cplusplus
}
#endif

#endif /* BUSSIM_H */
