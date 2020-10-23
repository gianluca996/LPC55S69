/*
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    USB_Log.c
 * @brief   Application entry point.
 */


#include "usb_host_config.h"
#include "usb_host.h"
#include "fsl_device_registers.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#if ((defined USB_HOST_CONFIG_KHCI) && (USB_HOST_CONFIG_KHCI))
#define CONTROLLER_ID kUSB_ControllerKhci0
#endif /* USB_HOST_CONFIG_KHCI */
#if ((defined USB_HOST_CONFIG_EHCI) && (USB_HOST_CONFIG_EHCI))
#define CONTROLLER_ID kUSB_ControllerEhci0
#endif /* USB_HOST_CONFIG_EHCI */
#if ((defined USB_HOST_CONFIG_OHCI) && (USB_HOST_CONFIG_OHCI))
#define CONTROLLER_ID kUSB_ControllerOhci0
#endif /* USB_HOST_CONFIG_OHCI */
#if ((defined USB_HOST_CONFIG_IP3516HS) && (USB_HOST_CONFIG_IP3516HS))
#define CONTROLLER_ID kUSB_ControllerIp3516Hs0
#endif /* USB_HOST_CONFIG_IP3516HS */

#if defined(__GIC_PRIO_BITS)
#define USB_HOST_INTERRUPT_PRIORITY (25U)
#else
#define USB_HOST_INTERRUPT_PRIORITY (3U)
#endif



#include <stdio.h>
#include "board.h"

#include "fsl_uart.h"
#include "fsl_port.h"
#include "fsl_flash.h"
#include "fsl_pit.h"

#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK22D5.h"
#include "fsl_debug_console.h"
//---------------------------------------------------------------------------------------------

//#include "usb_host_config.h"
//#include "usb_host.h"
//#include "fsl_device_registers.h"
#include "usb_host_msd.h"
//#include "board.h"
#include "host_msd_fatfs.h"
#include "fsl_common.h"
#if (defined(FSL_FEATURE_SOC_SYSMPU_COUNT) && (FSL_FEATURE_SOC_SYSMPU_COUNT > 0U))
#include "fsl_sysmpu.h"
#endif /* FSL_FEATURE_SOC_SYSMPU_COUNT */
//#include "app.h"
//#include "board.h"

#if ((!USB_HOST_CONFIG_KHCI) && (!USB_HOST_CONFIG_EHCI) && (!USB_HOST_CONFIG_OHCI) && (!USB_HOST_CONFIG_IP3516HS))
#error Please enable USB_HOST_CONFIG_KHCI, USB_HOST_CONFIG_EHCI, USB_HOST_CONFIG_OHCI, or USB_HOST_CONFIG_IP3516HS in file usb_host_config.
#endif

//#include "pin_mux.h"
#include "fsl_gpio.h"
#include <stdbool.h>
//#include "clock_config.h"
/* TODO: insert other include files here. */

volatile uint32_t i = 0;

 //-------------------------------------------------------------------------------------------
 // Timer Configuration

 #define DEMO_PIT_BASEADDR PIT
 #define DEMO_PIT_CHANNEL  kPIT_Chnl_0
 #define PIT_LED_HANDLER   PIT0_IRQHandler
 #define PIT_IRQ_ID        PIT0_IRQn
 /* Get source clock for PIT driver */
 #define PIT_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk)
 #define LED_INIT()       LED_RED_INIT(LOGIC_LED_ON)
 #define LED_TOGGLE()     LED_RED_TOGGLE()

 volatile bool pitIsrFlag = false;

//-------------------------------------------------------------------------------------------

#define BOARD_DEBUG_UART_BAUDRATE 115200

#define DEMO_UART UART1
#define DEMO_UART_CLKSRC kCLOCK_BusClk
#define DEMO_UART_CLK_FREQ CLOCK_GetFreq(kCLOCK_BusClk)
#define DEMO_UART_IRQn UART1_RX_TX_IRQn
#define DEMO_UART_IRQHandler UART1_RX_TX_IRQHandler

/*! @brief Ring buffer size (Unit: Byte). */
#define DEMO_RING_BUFFER_SIZE 500

volatile uint16_t Dati_Presenti; /*  */

uint8_t g_tipString[] =
    "Uart functional API interrupt example\r\nBoard receives characters then sends them out\r\nNow please input:\r\n";


 uint8_t demoRingBuffer[DEMO_RING_BUFFER_SIZE];
 volatile uint16_t txIndex; /* Index of the data to send out. */
 volatile uint16_t rxIndex; /* Index of the memory to save new arrived data. */

//--------------------------------------------------------------------------------------------


static usb_status_t USB_HostEvent(usb_device_handle deviceHandle,
                                  usb_host_configuration_handle configurationHandle,
                                  uint32_t eventCode);

/* TODO: insert other definitions and declarations here. */

static void USB_HostApplicationInit(void);

extern void USB_HostClockInit(void);
extern void USB_HostIsrEnable(void);
extern void USB_HostTaskFn(void *param);
void BOARD_InitHardware(void);

/*! @brief USB host msd fatfs instance global variable */

extern usb_host_msd_fatfs_instance_t g_MsdFatfsInstance;
usb_host_handle g_HostHandle;

#if defined(USB_HOST_CONFIG_KHCI) && (USB_HOST_CONFIG_KHCI > 0U)
void USB0_IRQHandler(void)
{
    USB_HostKhciIsrFunction(g_HostHandle);
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
    exception return operation might vector to incorrect interrupt */
    __DSB();
}
#endif /* USB_HOST_CONFIG_KHCI */

void USB_HostClockInit(void)
{
#if defined(USB_HOST_CONFIG_KHCI) && (USB_HOST_CONFIG_KHCI > 0U)
    SystemCoreClockUpdate();
    CLOCK_EnableUsbfs0Clock(kCLOCK_UsbSrcPll0, CLOCK_GetFreq(kCLOCK_PllFllSelClk));
#endif
}

void USB_HostIsrEnable(void)
{
    uint8_t irqNumber;
#if defined(USB_HOST_CONFIG_KHCI) && (USB_HOST_CONFIG_KHCI > 0U)
    uint8_t usbHOSTKhciIrq[] = USB_IRQS;
    irqNumber = usbHOSTKhciIrq[CONTROLLER_ID - kUSB_ControllerKhci0];
#endif /* USB_HOST_CONFIG_KHCI */

/* Install isr, set priority, and enable IRQ. */
#if defined(__GIC_PRIO_BITS)
    GIC_SetPriority((IRQn_Type)irqNumber, USB_HOST_INTERRUPT_PRIORITY);
#else
    NVIC_SetPriority((IRQn_Type)irqNumber, USB_HOST_INTERRUPT_PRIORITY);
#endif
    EnableIRQ((IRQn_Type)irqNumber);
}

void USB_HostTaskFn(void *param)
{
#if defined(USB_HOST_CONFIG_KHCI) && (USB_HOST_CONFIG_KHCI > 0U)
    USB_HostKhciTaskFunction(param);
#endif
}

#if ((defined USB_HOST_CONFIG_COMPLIANCE_TEST) && (USB_HOST_CONFIG_COMPLIANCE_TEST))
extern usb_status_t USB_HostTestEvent(usb_device_handle deviceHandle,
                                      usb_host_configuration_handle configurationHandle,
                                      uint32_t eventCode);
#endif

static usb_status_t USB_HostEvent(usb_device_handle deviceHandle,
                                  usb_host_configuration_handle configurationHandle,
                                  uint32_t eventCode)
{
#if ((defined USB_HOST_CONFIG_COMPLIANCE_TEST) && (USB_HOST_CONFIG_COMPLIANCE_TEST))
    usb_host_configuration_t *configuration;
    usb_status_t status1;
    usb_status_t status2;
    uint8_t interfaceIndex = 0;
#endif
    usb_status_t status = kStatus_USB_Success;
    switch (eventCode)
    {
        case kUSB_HostEventAttach:
#if ((defined USB_HOST_CONFIG_COMPLIANCE_TEST) && (USB_HOST_CONFIG_COMPLIANCE_TEST))
            status1 = USB_HostTestEvent(deviceHandle, configurationHandle, eventCode);
            status2 = USB_HostMsdEvent(deviceHandle, configurationHandle, eventCode);
            if ((status1 == kStatus_USB_NotSupported) && (status2 == kStatus_USB_NotSupported))
            {
                status = kStatus_USB_NotSupported;
            }
#else
            status = USB_HostMsdEvent(deviceHandle, configurationHandle, eventCode);
#endif
            break;

        case kUSB_HostEventNotSupported:
#if ((defined USB_HOST_CONFIG_COMPLIANCE_TEST) && (USB_HOST_CONFIG_COMPLIANCE_TEST))
            configuration = (usb_host_configuration_t *)configurationHandle;
            for (interfaceIndex = 0; interfaceIndex < configuration->interfaceCount; ++interfaceIndex)
            {
                if (((usb_descriptor_interface_t *)configuration->interfaceList[interfaceIndex].interfaceDesc)
                        ->bInterfaceClass == 9U) /* 9U is hub class code */
                {
                    break;
                }
            }

            if (interfaceIndex < configuration->interfaceCount)
            {
                usb_echo("unsupported hub\r\n");
            }
            else
            {
                usb_echo("Unsupported Device\r\n");
            }
#else
            usb_echo("Unsupported Device\r\n");
#endif
            break;

        case kUSB_HostEventEnumerationDone:
#if ((defined USB_HOST_CONFIG_COMPLIANCE_TEST) && (USB_HOST_CONFIG_COMPLIANCE_TEST))
            status1 = USB_HostTestEvent(deviceHandle, configurationHandle, eventCode);
            status2 = USB_HostMsdEvent(deviceHandle, configurationHandle, eventCode);
            if ((status1 != kStatus_USB_Success) && (status2 != kStatus_USB_Success))
            {
                status = kStatus_USB_Error;
            }
#else
            status = USB_HostMsdEvent(deviceHandle, configurationHandle, eventCode);
#endif
            break;

        case kUSB_HostEventDetach:
#if ((defined USB_HOST_CONFIG_COMPLIANCE_TEST) && (USB_HOST_CONFIG_COMPLIANCE_TEST))
            status1 = USB_HostTestEvent(deviceHandle, configurationHandle, eventCode);
            status2 = USB_HostMsdEvent(deviceHandle, configurationHandle, eventCode);
            if ((status1 != kStatus_USB_Success) && (status2 != kStatus_USB_Success))
            {
                status = kStatus_USB_Error;
            }
#else
            status = USB_HostMsdEvent(deviceHandle, configurationHandle, eventCode);
#endif
            break;

        default:
            break;
    }
    return status;
}

static void USB_HostApplicationInit(void)
{
    usb_status_t status = kStatus_USB_Success;

    USB_HostClockInit();

#if ((defined FSL_FEATURE_SOC_SYSMPU_COUNT) && (FSL_FEATURE_SOC_SYSMPU_COUNT))
    SYSMPU_Enable(SYSMPU, 0);
#endif /* FSL_FEATURE_SOC_SYSMPU_COUNT */

    status = USB_HostInit(CONTROLLER_ID, &g_HostHandle, USB_HostEvent);
    if (status != kStatus_USB_Success)
    {
        usb_echo("host init error\r\n");
        return;
    }
    USB_HostIsrEnable();

    usb_echo("host init done\r\n");
}



void DEMO_UART_IRQHandler(void)
{
    uint8_t data;

    /* If new data arrived. */
    if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(DEMO_UART))
    {
//    	Dati_Presenti = 0;
        data = UART_ReadByte(DEMO_UART);

        /* If ring buffer is not full, add data to ring buffer. */

        if ((data != 0xD) && (rxIndex < 500))
        {
			demoRingBuffer[rxIndex] = data;
			rxIndex++;
        }
        else
        {
        	demoRingBuffer[rxIndex] = data;
        	Dati_Presenti = 1;

        	DisableIRQ(DEMO_UART_IRQn);		// test gianluca
//        	GPIO_PinWrite(GPIOB, 18U, 1);	// Metto in Tx // Test Gianluca

        	rxIndex = 0;

        }
    }
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

// Interrupt PIT Timer
void PIT_LED_HANDLER(void)
{
    /* Clear interrupt flag.*/
    PIT_ClearStatusFlags(DEMO_PIT_BASEADDR, DEMO_PIT_CHANNEL, kPIT_TimerFlag);
    pitIsrFlag = true;
    /* Added for, and affects, all PIT handlers. For CPU clock which is much larger than the IP bus clock,
     * CPU can run out of the interrupt handler before the interrupt flag being cleared, resulting in the
     * CPU's entering the handler again and again. Adding DSB can prevent the issue from happening.
     */
    __DSB();
}

void delay(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < 500; ++i)
    {
        __asm("NOP"); /* delay */
    }
}

uint32_t Address_Index_Log = 0x1000F800;
uint32_t Address_Log[1];

uint32_t Address_Index_Identifier = 0x1000F000;
uint32_t Address_Identifier[1];
uint32_t Identifier = 0;

uint32_t Sector_Start_Address[31];
uint8_t Enable_Sector_Erase  [31];
uint8_t IndexAddress = 0;
uint8_t	IndexErase   = 0;
uint8_t US00[15];

volatile uint32_t x, y;

#define BUFFER_SIZE_TEST 200
uint8_t demoRingBuffer_test[BUFFER_SIZE_TEST];

void error_trap(void);
void app_finalize(void);

 void error_trap(void)
 {
     PRINTF("\r\n\r\n\r\n\t---- HALTED DUE TO FLASH ERROR! ----");

     PRINTF("\r\n Perform a system reset error_trap \r\n");
     NVIC_SystemReset();
 }

 void app_finalize(void)
 {
     /* Print finished message. */
     PRINTF("\r\n End of FlexNVM EEprom Example \r\n");

     PRINTF("\r\n Perform a system reset app_finalize \r\n");
     NVIC_SystemReset();
 }

 uint32_t failAddr, failDat;
 bool Controllo_Inizio_Stringa = false;
 uint32_t Controllo_Fine_Stringa = 0;


/*
 * @brief   Application entry point.
 */
int main(void) {
  	/* Init board hardware. */

//    bool Controllo_Inizio_Stringa = false;
//    uint32_t Controllo_Fine_Stringa = 0;

    uint32_t dflashBlockBase  = 0;
    uint32_t dflashTotalSize  = 0;
    uint32_t dflashSectorSize = 0;

    US00[0]= 85; // U
    US00[1]= 83; // S
    US00[2]= 48; // 0
    US00[3]= 48; // 0
    US00[4]= 32; // Spazio
    US00[5]= 35; // #

    US00[12]= 35;   // #
    US00[13]= 0x0D; // New line
    US00[14]= 0x0C; // Invio

    /*--------------------------------------------------------------------------------------------------------------------------------------*/

	gpio_pin_config_t pinConfig;	// USB
	uart_config_t config;			// UART

    BOARD_InitBootPins();
    //BOARD_InitBootClocks();
    BOARD_BootClockRUN();
    //BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
	BOARD_InitDebugConsole();

	/* Enable Vbus for USB */
	pinConfig.outputLogic = 1U;
	pinConfig.pinDirection = kGPIO_DigitalOutput;
	GPIO_PinInit(GPIOC, 9U, &pinConfig);

	USB_HostApplicationInit();

    /*--------------------------------------------------------------------------------------------------------------------------------------*/
	/* Inzio Gestione FlexNVM */

    /*! @brief Flash driver Structure */
    static flash_config_t s_flashDriver;

	/* Return protection status */
	flash_security_state_t securityStatus = kFLASH_SecurityStateNotSecure;

	/* Return code from each flash driver function */
	status_t result;


	/* Clean up Flash driver Structure*/
	memset(&s_flashDriver, 0, sizeof(flash_config_t));


	PRINTF("\r\n FlexNVM DFlash Example Start \r\n");

	#if defined(SIM_FCFG2_PFLSH_MASK)
		if (SIM->FCFG2 & SIM_FCFG2_PFLSH_MASK)
		{
			PRINTF("\r\n Current device doesn't support FlexNVM feature \r\n");
			app_finalize();
		}
	#endif


	result = FLASH_Init(&s_flashDriver);
	if (kStatus_FLASH_Success != result)
	{
		error_trap();
	}


	result = FLASH_GetSecurityState(&s_flashDriver, &securityStatus);
	if (kStatus_FLASH_Success != result)
	{
		error_trap();
	}


	switch (securityStatus)
	{
		case kFLASH_SecurityStateNotSecure:
			PRINTF("\r\n Flash is UNSECURE!");
			break;
		case kFLASH_SecurityStateBackdoorEnabled:
			PRINTF("\r\n Flash is SECURE, BACKDOOR is ENABLED!");
			break;
		case kFLASH_SecurityStateBackdoorDisabled:
			PRINTF("\r\n Flash is SECURE, BACKDOOR is DISABLED!");
			break;
		default:
			break;
	}
	PRINTF("\r\n");

	if (kFLASH_SecurityStateNotSecure != securityStatus)
	{
		PRINTF("\r\n Data Flash opeation will not be executed, as Flash is SECURE!");
		app_finalize();
	}
	else
	{



		FLASH_GetProperty(&s_flashDriver, kFLASH_PropertyDflashBlockBaseAddr, &dflashBlockBase);
		FLASH_GetProperty(&s_flashDriver, kFLASH_PropertyDflashTotalSize, &dflashTotalSize);
		FLASH_GetProperty(&s_flashDriver, kFLASH_PropertyDflashSectorSize, &dflashSectorSize);


		PRINTF("\r\n DFlash Information: ");
		if (dflashTotalSize)
		{
			PRINTF("\r\n Data Flash Base Address: (0x%x) ", dflashBlockBase);
			PRINTF("\r\n Data Flash Total Size:\t%d KB, Hex: (0x%x)", (dflashTotalSize / 1024), dflashTotalSize);
			PRINTF("\r\n Data Flash Sector Size:\t%d KB, Hex: (0x%x) ", (dflashSectorSize / 1024), dflashSectorSize);
		}
		else
		{
			PRINTF("\r\n There is no D-Flash (FlexNVM) on this Device.");
			app_finalize();
		}

	}

	/* Fine Gestione FlexNVM */
	/*--------------------------------------------------------------------------------------------------------------------------------------*/

	/* Structure of initialize PIT */
	pit_config_t pitConfig;

	/*
	 * pitConfig.enableRunInDebug = false;
	 */

	PIT_GetDefaultConfig(&pitConfig);
	PIT_Init(DEMO_PIT_BASEADDR, &pitConfig);
	PIT_SetTimerPeriod(DEMO_PIT_BASEADDR, DEMO_PIT_CHANNEL, USEC_TO_COUNT(10000000U, PIT_SOURCE_CLOCK));
	PIT_EnableInterrupts(DEMO_PIT_BASEADDR, DEMO_PIT_CHANNEL, kPIT_TimerInterruptEnable);
	EnableIRQ(PIT_IRQ_ID);
	PRINTF("\r\nStarting channel No.0 ...");
	PIT_StartTimer(DEMO_PIT_BASEADDR, DEMO_PIT_CHANNEL);

	//-------------------------------------------------------------------------------------

    /* UART
     * config.baudRate_Bps = 115200U;
     * config.parityMode = kUART_ParityDisabled;
     * config.stopBitCount = kUART_OneStopBit;
     * config.txFifoWatermark = 0;
     * config.rxFifoWatermark = 1;
     * config.enableTx = false;
     * config.enableRx = false;
     */

    UART_GetDefaultConfig(&config);
    config.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
    config.enableTx = true;
    config.enableRx = true;

    UART_Init(DEMO_UART, &config, DEMO_UART_CLK_FREQ);

    /*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
    /* Inizializzazione Direzione Tx Rx 485 */

    gpio_pin_config_t pinConfig_DIR_485;

    pinConfig_DIR_485.outputLogic = 0U;
    pinConfig_DIR_485.pinDirection = kGPIO_DigitalOutput;

    PORT_SetPinMux(PORTB, 18U, kPORT_MuxAsGpio);
    GPIO_PinInit(GPIOB, 18U, &pinConfig_DIR_485);

    GPIO_PinWrite(GPIOB, 18U, 0); //  Lo metto in Rx

    //-----------------------------------------------------------------------------
    /* Enable RX interrupt. */
    UART_EnableInterrupts(DEMO_UART, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
    EnableIRQ(DEMO_UART_IRQn);

    //-------------------------------------------------------------------------------------


    while (1)
    {
//		USB_HostTaskFn(g_HostHandle);
//		USB_HostMsdTask(&g_MsdFatfsInstance, demoRingBuffer_test);


		while ((kUART_TxDataRegEmptyFlag & UART_GetStatusFlags(DEMO_UART))  && (Dati_Presenti == 1))
		{

			Controllo_Inizio_Stringa = false;
			Controllo_Fine_Stringa = 0;

			/*---------------------------------------------------------------------------------------------------------------------------------------------------------------*/

			/* Cerco il #35#NEWCODE:,000000, e salvo il nuovo identificativo in eeprom all'indirizzo 0x1000F100 */

			if (
					(demoRingBuffer[0] == 35) && (demoRingBuffer[1] == 51) && (demoRingBuffer[2] == 53) && (demoRingBuffer[3] == 35) &&
					(demoRingBuffer[4] == 78) && (demoRingBuffer[5] == 69) && (demoRingBuffer[6] == 87) && (demoRingBuffer[7] == 67) &&
					(demoRingBuffer[8] == 79) && (demoRingBuffer[9] == 68) && (demoRingBuffer[10] == 69)
			   )
			{

				Address_Identifier[0] = (demoRingBuffer[13] - 48) * 100000 + (demoRingBuffer[14] - 48) * 10000 + (demoRingBuffer[15] - 48) * 1000 + (demoRingBuffer[16] - 48) * 100 + (demoRingBuffer[17] - 48) * 10 + (demoRingBuffer[18] - 48);


				result = FLASH_Erase(&s_flashDriver, Address_Index_Identifier, dflashSectorSize, kFLASH_ApiEraseKey);
				if (kStatus_FLASH_Success != result)
				{
					error_trap();
				}

				result = FLASH_VerifyErase(&s_flashDriver, Address_Index_Identifier, dflashSectorSize, kFLASH_MarginValueUser);
				if (kStatus_FLASH_Success != result)
				{
					error_trap();
				}

				result    = FLASH_Program(&s_flashDriver, Address_Index_Identifier, Address_Identifier, sizeof(Address_Identifier));
				if (kStatus_FLASH_Success != result)
				{
					error_trap();
				}

				result = FLASH_VerifyProgram(&s_flashDriver, Address_Index_Identifier, sizeof(Address_Identifier), Address_Identifier, kFLASH_MarginValueUser, &failAddr, &failDat);
				if (kStatus_FLASH_Success != result)
				{
					error_trap();
				}

			}

			/*---------------------------------------------------------------------------------------------------------------------------------------------------------------*/


			if ((demoRingBuffer[0] == 35) && (demoRingBuffer[1] == 51) && (demoRingBuffer[2] == 53) && (demoRingBuffer[3] == 35)) Controllo_Inizio_Stringa = true;
			//while ((demoRingBuffer[Controllo_Fine_Stringa]!= '?') && ((demoRingBuffer[Controllo_Fine_Stringa+1]!= '?'))) Controllo_Fine_Stringa++;
			while ((demoRingBuffer[Controllo_Fine_Stringa]!= '?') && ((demoRingBuffer[Controllo_Fine_Stringa+1]!= '?')) && (Controllo_Fine_Stringa  < 500)) Controllo_Fine_Stringa++;


			//if ((Controllo_Inizio_Stringa == true) && (Controllo_Fine_Stringa > 0))
			if ((Controllo_Inizio_Stringa == true) && (Controllo_Fine_Stringa > 310) && (Controllo_Fine_Stringa < 340))
			{

				demoRingBuffer_test[0] = demoRingBuffer[0]; //#
				demoRingBuffer_test[1] = (demoRingBuffer[1] - 48) * 10 + (demoRingBuffer[2] - 48); //35
				demoRingBuffer_test[2] = demoRingBuffer[3]; //#
				demoRingBuffer_test[3] = 0;// Spazio


				demoRingBuffer_test[4] = demoRingBuffer[5]; //#
				demoRingBuffer_test[5] = (demoRingBuffer[6] - 48) * 10 + (demoRingBuffer[7] - 48); //35
				demoRingBuffer_test[6] = demoRingBuffer[8]; //#
				demoRingBuffer_test[7] = 0;// Spazio


				demoRingBuffer_test[8] = demoRingBuffer[10]; //#
				demoRingBuffer_test[9] = (demoRingBuffer[11] - 48) * 10 + (demoRingBuffer[12] - 48); //35
				demoRingBuffer_test[10] = demoRingBuffer[13]; //#
				demoRingBuffer_test[11] = 0;// Spazio

				demoRingBuffer_test[12] = 35; // #

				i = 16;
				x = 13;


				for (i = 16; i <= 63; i=i+3)
				{
					demoRingBuffer_test[x] = (demoRingBuffer[i] - 48) * 10 + (demoRingBuffer[i+1] - 48);
					x++;
				}


				demoRingBuffer_test[29] = '#';
				demoRingBuffer_test[30] = '#';


				i = 66;
				x = 31;

				for (i = 66; i <= 83; i=i+3)
				{
					demoRingBuffer_test[x] = (demoRingBuffer[i] - 48) * 10 + (demoRingBuffer[i+1] - 48);
					x++;
				}

				demoRingBuffer_test[37] = '#';

				i = 85;
				x = 38;

				while( (demoRingBuffer[i]!= '?') && ((demoRingBuffer[i+1]!= '?')))
				{

					demoRingBuffer_test[x] = (demoRingBuffer[i] - 48) * 10 + (demoRingBuffer[i+1] - 48);
					i=i+3;
					x++;
				} // End While

				demoRingBuffer_test [x] = '?';
				demoRingBuffer_test [x+1] = '?';

				/* Mi attiva il salvataggio su chiavetta del Log arrivato */
/*
				g_MsdFatfsInstance.runState = kRunMassStorageTest;

				rxIndex = 0;
				for (i = 0; i < DEMO_RING_BUFFER_SIZE; ++i)
				{
					demoRingBuffer[i] = 0;
				}


				while (g_MsdFatfsInstance.runState != kRunIdle)
				{

				}
*/
				g_MsdFatfsInstance.runState = kRunMassStorageTest;
				USB_HostMsdTask(&g_MsdFatfsInstance, demoRingBuffer_test);

				rxIndex = 0;
				x = 0;

				for (i = 0; i < DEMO_RING_BUFFER_SIZE; ++i)
				{
					demoRingBuffer[i] = 0;
				}


				for (i = 0; i < BUFFER_SIZE_TEST; ++i)
				{
					demoRingBuffer_test[i] = 0;
				}

				Dati_Presenti = 0;
				EnableIRQ(DEMO_UART_IRQn);		// test Gianluca
				GPIO_PinWrite(GPIOB, 18U, 0);	// test Gianluca
//				Dati_Presenti = 0;				// test Gianluca


			} //End if ((Controllo_Inizio_Stringa == true) && (Controllo_Fine_Stringa < 340))
			else
			{

				Controllo_Inizio_Stringa = false;
				Controllo_Fine_Stringa = 0;

				rxIndex = 0;
				x = 0;
				for (i = 0; i < DEMO_RING_BUFFER_SIZE; ++i)
				{
					demoRingBuffer[i] = 0;
				}
				for (i = 0; i < BUFFER_SIZE_TEST; ++i)
				{
					demoRingBuffer_test[i] = 0;
				}

				Dati_Presenti = 0;
				EnableIRQ(DEMO_UART_IRQn);		//test Gianluca
				GPIO_PinWrite(GPIOB, 18U, 0);	//test Gianluca
//				Dati_Presenti = 0;				//test Gianluca


			}

		} // End while ((kUART_TxDataRegEmptyFlag & UART_GetStatusFlags(DEMO_UART))  && (Dati_Presenti == 1))


		USB_HostTaskFn(g_HostHandle);
//		USB_HostMsdTask(&g_MsdFatfsInstance, demoRingBuffer_test);


		//-------------------------------------------------------------------------------------//
		// Trasmissione su UART

		// Timer a 10 secondi per mandare sulla 485 il segnale US00 #123456#

		if ((true == pitIsrFlag) /*&& (Dati_Presenti == 0)*/) // Flag Interrupt Timer
		{

/*
			Identifier = *(volatile uint32_t *)(0x1000F000);

			US00[6] = (Identifier / 100000) + 48;
			US00[7] = ((Identifier % 100000) / 10000) + 48;
			US00[8] = ((Identifier % 10000) / 1000) + 48;
			US00[9] = ((Identifier % 1000) / 100) + 48;
			US00[10] = ((Identifier % 100) / 10) + 48;
			US00[11] = ((Identifier % 10)) + 48;

			DisableIRQ(DEMO_UART_IRQn);		// Disabilito Interrupt ricezione della 485 	// 2 Ottobre 2020
			GPIO_PinWrite(GPIOB, 18U, 1);	// Metto in Tx

			txIndex = 0;
			while (US00[txIndex] != 0xC)
			{
				UART_WriteByte(DEMO_UART, US00[txIndex]);
				delay();
				txIndex++;
			}

			rxIndex = 0;
			x = 0;
			for (i = 0; i < DEMO_RING_BUFFER_SIZE; ++i)
			{
				demoRingBuffer[i] = 0;
			}
			for (i = 0; i < BUFFER_SIZE_TEST; ++i)
			{
				demoRingBuffer_test[i] = 0;
			}


			Dati_Presenti = 0;
			EnableIRQ(DEMO_UART_IRQn);		// Abilito Interrupt ricezione della 485		// 2 Ottobre 2020
			GPIO_PinWrite(GPIOB, 18U, 0);	// Metto in Rx
			PRINTF(" US00 #XXXXXX#  \n");
*/
			pitIsrFlag = false;

		}



    }


    return 0 ;
}
