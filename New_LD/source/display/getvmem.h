/*
   Prototype for interface function for virtual (external) memory

   The getvmem(..) function must be implemented by the library user
   for the specific type of storage device.
*/
#ifndef GETVMEM_H
#define GETVMEM_H

#include <sgtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
   Define optimized type used as index in virtual memory
   Use SGUINT if sizeof external memory is < 65K
   Use SGULONG if sizeof external memory is >= 65K
*/
typedef SGULONG GFONTBUFIDX;
/*typedef SGUINT GFONTBUFIDX;*/

/*
   GFONTDEVICE defined here is an application specifc optimized type
   used for identifing the specific external device where the virtual font
   buffer is located.

   A GFONTDEVICE type parameter is incorporated in each virtual font
   structure as the device_id parameter. It is passed as a parameter in the
   getvmem(..) call.
   The parameter type and its value is transperant (don't care) for the
   graphic library.

   Typical use for GFONTDEVICE could be
   -  the physical address for the memory chip /CS
   -  the SPI address for a memory device on a SPI bus
   -  an index into an array of device access information
   -  a const pointer to a { filename, FILE * } structure
   etc

   A SGUCHAR type is used as default to get a minimum font memory footprint.
   For example if the vf_device parameter is not used by the specific  getvmem(..)
   function implementation
*/
typedef SGUCHAR GFONTDEVICE;     /* Default type, used for ex. as index or SPI address */
/*typedef SGULONG GFONTDEVICE;*/ /* used for ex processor chip select address */

/*
   Get data from virtual memory

   buf      = destination pointer to RAM storage location
   vf_device= device identifier (ex address) for virtual font.
              Identifiy which virtual font memory device to use if a system contains
              more than one virtual font image storage locations. Can be ignored
              if an application only contains one virtual font storage location.
   index    = source index for data in virtual font to be loaded to RAM
   numbytes = number of bytes to load to buffer
*/
void getvmem(GFONTDEVICE vf_device, SGUCHAR *buf, GFONTBUFIDX index, SGUINT numbytes);

/******* Init / exit functions (Optional, not used by library itself) ************/

/* Optionally called before first call of getvmem
   Return 0   if no errror
   Return !=0 if error */
int getvmem_open( GFONTDEVICE vf_device );

/* Optionally called before exiting program */
void getvmem_close( GFONTDEVICE vf_device );

#ifdef __cplusplus
}
#endif

#endif


