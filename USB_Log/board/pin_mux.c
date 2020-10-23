/*
 * Copyright 2017-2020 NXP
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
 
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v4.0
* BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********/

/**
 * @file    pin_mux.c
 * @brief   Board pins file.
 */
 
/* This is a template for board specific configuration created by MCUXpresso IDE Project Wizard.*/


#include "fsl_gpio.h"

#include "fsl_common.h"
#include "fsl_port.h"
#include "pin_mux.h"

#define SOPT5_UART1TXSRC_UART_TX 0x00u


/**
 * @brief Set up and initialize all required blocks and functions related to the board hardware.
 */
void BOARD_InitBootPins(void) {
	/* The user initialization should be placed here */

    /* Port C Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortC);
    /* Port E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);

    /* PORTC9 (pin D6) is configured as PTC9 */
    PORT_SetPinMux(PORTC, 9U, kPORT_MuxAsGpio);

    //-------------------------------------------------
    /* PORTE16 (pin K3) is configured as UART2_TX */
    //PORT_SetPinMux(PORTE, 16U, kPORT_MuxAlt3);

    /* PORTE17 (pin H4) is configured as UART2_RX */
    //PORT_SetPinMux(PORTE, 17U, kPORT_MuxAlt3);
    //-------------------------------------------------

    /* PORTE0 (pin 1) is configured as UART1_TX */
        PORT_SetPinMux(PORTE, 0U, kPORT_MuxAlt3);

	/* PORTE1 (pin 2) is configured as UART1_RX */
	//PORT_SetPinMux(BOARD_INITPINS_DEBUG_UART_RX_PORT, BOARD_INITPINS_DEBUG_UART_RX_PIN, kPORT_MuxAlt3);
	PORT_SetPinMux(PORTE, 1U, kPORT_MuxAlt3);


	SIM->SOPT5 = ((SIM->SOPT5 &
				   /* Mask bits to zero which are setting */
				   (~(SIM_SOPT5_UART1TXSRC_MASK)))

				  /* UART 1 transmit data source select: UART1_TX pin. */
				  | SIM_SOPT5_UART1TXSRC(SOPT5_UART1TXSRC_UART_TX));





    /* PORTB18 (pin H4) is configured as DIR 485 */
    //PORT_SetPinMux(PORTB, 18U, kPORT_MuxAlt3);


    /* Port B Clock Gate Control: Clock enabled */

        CLOCK_EnableClock(kCLOCK_PortB);

 /*       gpio_pin_config_t DIR_485_config = {
            .pinDirection = kGPIO_DigitalOutput,
            .outputLogic = 1U
        };
  */
        /* Initialize GPIO functionality on pin PTB18 (pin 23)  */
        //GPIO_PinInit(DIR_485_GPIO, DIR_485_PIN, &DIR_485_config);

        //GPIO_PinWrite(DIR_485_GPIO, DIR_485_PIN, 1);



}
