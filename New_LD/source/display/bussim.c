/***************************** bussim.c ************************************

   Generic template file for external bus simulator drivers.
   Implemented for serial (SPI) bus simulation using a parallel port.

   The functions below must be implemented as described in the comments.
   Use the proper I/O port instructions given by the actual processor
   architecture, the actual LCD connection method and the I/O access
   capabilities of the target C compiler. 

   The portable sgwrby(..) and sgrdby(..) syntax is used for illustration. 
   This syntax can be replaced by the compilers native syntax for I/O access.

   The functions in this module is called by the ghwinit.c module when the
   compiler switch GHW_SINGLE_CHIP is defined and GHW_NOHDW is undefined.

   The LCD controller does not support read back of the video memory in 
   serial mode. Serial bus mode will therefore require that GBUFFER is 
   defined, so the LCD driver functions still can do read-modify-write 
   operations on the display content. 
   A simrdby() function is not required for the same reason.

   The driver assumes that the address is either 0 or 1.

   Copyright (c) RAMTEX Engineering Aps 2002

****************************************************************************/
#ifndef GHW_NOHDW
#ifdef  GHW_SINGLE_CHIP

#include "bussim.h"
#include "sgio.h" /* or use the SG function syntax (*/
#include "fsl_spi.h"
#include "fsl_gpio.h"

#define EXAMPLE_SPI_MASTER 	SPI7 	//spi number
#define PORT_1 				1U
#define PORT_1_PIN_9 		9U		//active-low Reset pin
#define PORT_1_PIN_10 		10U		//active-low Chip Select pin

#define SRC_BUFFER_SIZE 	1
#define DEST_BUFFER_SIZE 	0

static uint8_t srcBuff[SRC_BUFFER_SIZE];
static volatile bool masterFinished = false;



static void masterCallback(SPI_Type *base, spi_master_handle_t *masterHandle, status_t status, void *userData) {
    masterFinished = true;
}



void simwrby(SGUCHAR adr, SGUCHAR dat) {

   masterFinished = false;

   spi_master_handle_t handle;
   spi_transfer_t xfer = {0};

   /* Init Buffer */
   for (uint16_t i= 0; i < SRC_BUFFER_SIZE; i++) { srcBuff[i] = i; }
  
   /* invio un comando */
   if (adr == 0x00) {
		GPIO_PinWrite(GPIO, PORT_1, PORT_1_PIN_10, 0);
		srcBuff[0]   	= (uint8_t)(dat);
		xfer.txData   	= srcBuff;
		xfer.rxData   	= 0;
		xfer.dataSize 	= 1;
		xfer.configFlags = kSPI_FrameAssert;
		SPI_MasterTransferCreateHandle(EXAMPLE_SPI_MASTER, &handle, masterCallback, NULL);
		SPI_MasterTransferNonBlocking(EXAMPLE_SPI_MASTER, &handle, &xfer);
		while (masterFinished != true){}
   }

   /* invio un dato */
   if (adr == 0x01) {
	    GPIO_PinWrite(GPIO, PORT_1, PORT_1_PIN_10, 1);
		srcBuff[0]   	= (uint8_t)(dat);
		xfer.txData   	= srcBuff;
		xfer.rxData   	= 0;
		xfer.dataSize 	= 1;
		xfer.configFlags = kSPI_FrameAssert;
		SPI_MasterTransferCreateHandle(EXAMPLE_SPI_MASTER, &handle, masterCallback, NULL);
		SPI_MasterTransferNonBlocking(EXAMPLE_SPI_MASTER, &handle, &xfer);
		while (masterFinished != true){}
   }
   
     
}
 


void sim_reset(void){
	int16_t cnt= 0x0000;
	GPIO_PinWrite(GPIO, PORT_1, PORT_1_PIN_9, 0);
	cnt = 0x0040;
	while (cnt>0){ cnt -= 1; }
	GPIO_PinWrite(GPIO, PORT_1, PORT_1_PIN_9, 1);
	cnt = 0x00FF;
	while (cnt>0){ cnt -= 1; }
}



#endif /* GHW_SINGLE_CHIP */
#endif /* GHW_NOHDW */


