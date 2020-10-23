/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_spi.h"
#include "fsl_gpio.h"
#include "board.h"
#include "fsl_debug_console.h"

#include "pin_mux.h"
#include <stdbool.h>

#include "display\bussim.h"
#include "display\gdisp.h"
#include "display\gi_fonts.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_SPI_MASTER SPI7
#define EXAMPLE_SPI_MASTER_IRQ FLEXCOMM7_IRQn
#define EXAMPLE_SPI_MASTER_CLK_SRC kCLOCK_Flexcomm7
#define EXAMPLE_SPI_MASTER_CLK_FREQ CLOCK_GetFlexCommClkFreq(7U)
#define EXAMPLE_SPI_SSEL 1
#define EXAMPLE_SPI_SPOL kSPI_SpolActiveAllLow

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void BOARD_InitSPI(void) {
	spi_master_config_t userConfig;
	SPI_MasterGetDefaultConfig(&userConfig);
	userConfig.baudRate_Bps 	= 500000U;
	userConfig.polarity 		= kSPI_ClockPolarityActiveLow;
	userConfig.phase 			= kSPI_ClockPhaseFirstEdge;
	userConfig.sselNum 			= (spi_ssel_t)EXAMPLE_SPI_SSEL;
	userConfig.sselPol 			= (spi_spol_t)EXAMPLE_SPI_SPOL;
	SPI_MasterInit(EXAMPLE_SPI_MASTER, &userConfig, EXAMPLE_SPI_MASTER_CLK_FREQ);
}



int main(void) {

    /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);
    /* attach 12 MHz clock to SPI7 */
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM7);

    BOARD_InitPins();
    BOARD_BootClockPLL150M();
    BOARD_InitDebugConsole();

    BOARD_InitSPI();

	ginit();
	ghw_cont_set(70);
	gsetpos(5,10);
	//gputs("Test OK");
	gputs("Gianluca");

    while(1);

}
