/*
 * The Clear BSD License
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
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

#include "usb_host_config.h"
#include "usb_host.h"
#include "usb_host_msd.h"
#include "host_msd_fatfs.h"
#include "ff.h"
#include "diskio.h"
#include "stdio.h"
#include "fsl_device_registers.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#if MSD_FATFS_THROUGHPUT_TEST_ENABLE
#include "fsl_device_registers.h"
#define THROUGHPUT_BUFFER_SIZE (64 * 1024) /* throughput test buffer */
#define MCU_CORE_CLOCK (120000000)         /* mcu core clock, user need to configure it. */
#endif                                     /* MSD_FATFS_THROUGHPUT_TEST_ENABLE */


#define DEMO_UART_IRQn UART1_RX_TX_IRQn

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief host msd control transfer callback.
 *
 * This function is used as callback function for control transfer .
 *
 * @param param      the host msd fatfs instance pointer.
 * @param data       data buffer pointer.
 * @param dataLength data length.
 * @status           transfer result status.
 */
void USB_HostMsdControlCallback(void *param, uint8_t *data, uint32_t dataLength, usb_status_t status);

/*!
 * @brief msd fatfs test code execute done.
 */
static void USB_HostMsdFatfsTestDone(void);

#if ((defined MSD_FATFS_THROUGHPUT_TEST_ENABLE) && (MSD_FATFS_THROUGHPUT_TEST_ENABLE))
/*!
 * @brief host msd fatfs throughput test.
 *
 * @param msdFatfsInstance   the host fatfs instance pointer.
 */
static void USB_HostMsdFatfsThroughputTest(usb_host_msd_fatfs_instance_t *msdFatfsInstance);

#else

/*!
 * @brief display file information.
 */
static void USB_HostMsdFatfsDisplayFileInfo(FILINFO *fileInfo);

/*!
 * @brief list files and sub-directory in one directory, the function don't check all sub-directories recursively.
 */
static FRESULT USB_HostMsdFatfsListDirectory(const TCHAR *path);

/*!
 * @brief forward function pointer for fatfs f_forward function.
 *
 * @param data_ptr   forward data pointer.
 * @param dataLength data length.
 */
#if _USE_FORWARD && _FS_TINY
static uint32_t USB_HostMsdFatfsForward(const uint8_t *data_ptr, uint32_t dataLength);
#endif

/*!
 * @brief host msd fatfs test.
 *
 * This function implements msd fatfs test.
 *
 * @param msdFatfsInstance   the host fatfs instance pointer.
 */
static void USB_HostMsdFatfsTest(usb_host_msd_fatfs_instance_t *msdFatfsInstance);

#endif /* MSD_FATFS_THROUGHPUT_TEST_ENABLE */

#if ((defined USB_HOST_CONFIG_COMPLIANCE_TEST) && (USB_HOST_CONFIG_COMPLIANCE_TEST))
extern usb_status_t USB_HostTestModeInit(usb_device_handle deviceHandle);
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief msd class handle array for fatfs */
extern usb_host_class_handle g_UsbFatfsClassHandle;

usb_host_msd_fatfs_instance_t g_MsdFatfsInstance; /* global msd fatfs instance */
static FATFS fatfs;
/* control transfer on-going state. It should set to 1 when start control transfer, it is set to 0 in the callback */
volatile uint8_t controlIng;
/* control transfer callback status */
volatile usb_status_t controlStatus;

#if MSD_FATFS_THROUGHPUT_TEST_ENABLE
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint32_t testThroughputBuffer[THROUGHPUT_BUFFER_SIZE / 4]; /* the buffer for throughput test */
uint32_t testSizeArray[] = {20 * 1024, 20 * 1024}; /* test time and test size (uint: K)*/
#else
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t testBuffer[(_MAX_SS > 256) ? _MAX_SS : 256]; /* normal test buffer */
#endif /* MSD_FATFS_THROUGHPUT_TEST_ENABLE */

/*******************************************************************************
 * Code
 ******************************************************************************/

void USB_HostMsdControlCallback(void *param, uint8_t *data, uint32_t dataLength, usb_status_t status)
{
    usb_host_msd_fatfs_instance_t *msdFatfsInstance = (usb_host_msd_fatfs_instance_t *)param;

    if (msdFatfsInstance->runWaitState == kRunWaitSetInterface) /* set interface finish */
    {
        msdFatfsInstance->runWaitState = kRunIdle;
        msdFatfsInstance->runState = kRunMassStorageTest;
    }
    controlIng = 0;
    controlStatus = status;
}

static void USB_HostMsdFatfsTestDone(void)
{
    usb_echo("............................test done......................\r\n");
}

#if ((defined MSD_FATFS_THROUGHPUT_TEST_ENABLE) && (MSD_FATFS_THROUGHPUT_TEST_ENABLE))

static void USB_HostMsdFatfsThroughputTest(usb_host_msd_fatfs_instance_t *msdFatfsInstance)
{
    uint64_t totalTime;
    FRESULT fatfsCode;
    FIL file;
    uint32_t resultSize;
    uint32_t testSize;
    uint8_t testIndex;
    char test_file_name[30];

    /* time delay (~100ms) */
    for (resultSize = 0; resultSize < 400000; ++resultSize)
    {
        __ASM("nop");
    }

    usb_echo("............................fatfs test.....................\r\n");
    CoreDebug->DEMCR |= (1 << CoreDebug_DEMCR_TRCENA_Pos);

    for (testSize = 0; testSize < (THROUGHPUT_BUFFER_SIZE / 4); ++testSize)
    {
        testThroughputBuffer[testSize] = testSize;
    }

    sprintf(test_file_name, "%c:", USBDISK + '0');
    fatfsCode = f_mount(&fatfs, test_file_name, 1);
    if (fatfsCode)
    {
        usb_echo("fatfs mount error\r\n");
        USB_HostMsdFatfsTestDone();
        return;
    }

    sprintf(test_file_name, "%c:/thput.dat", USBDISK + '0');
    usb_echo("throughput test:\r\n");
    for (testIndex = 0; testIndex < (sizeof(testSizeArray) / 4); ++testIndex)
    {
        fatfsCode = f_unlink(test_file_name); /* delete the file if it is existed */
        if ((fatfsCode != FR_OK) && (fatfsCode != FR_NO_FILE))
        {
            USB_HostMsdFatfsTestDone();
            return;
        }

        fatfsCode = f_open(&file, test_file_name, FA_WRITE | FA_READ | FA_CREATE_ALWAYS); /* create one new file */
        if (fatfsCode)
        {
            USB_HostMsdFatfsTestDone();
            return;
        }

        totalTime = 0;
        testSize = testSizeArray[testIndex] * 1024;
        while (testSize)
        {
            if (msdFatfsInstance->deviceState != kStatus_DEV_Attached)
            {
                USB_HostMsdFatfsTestDone();
                return;
            }
            DWT->CYCCNT = 0;
            DWT->CTRL |= (1 << DWT_CTRL_CYCCNTENA_Pos);
            fatfsCode = f_write(&file, testThroughputBuffer, THROUGHPUT_BUFFER_SIZE, &resultSize);
            if (fatfsCode)
            {
                usb_echo("write error\r\n");
                f_close(&file);
                USB_HostMsdFatfsTestDone();
                return;
            }
            totalTime += DWT->CYCCNT;
            DWT->CTRL &= ~(1 << DWT_CTRL_CYCCNTENA_Pos);
            testSize -= THROUGHPUT_BUFFER_SIZE;
        }
        testSize = testSizeArray[testIndex];
        usb_echo("    write %dKB data the speed is %d KB/s\r\n", testSize,
                 (uint32_t)((uint64_t)testSize * (uint64_t)MCU_CORE_CLOCK / (uint64_t)totalTime));

        fatfsCode = f_lseek(&file, 0);
        if (fatfsCode)
        {
            USB_HostMsdFatfsTestDone();
            return;
        }
        totalTime = 0;
        testSize = testSizeArray[testIndex] * 1024;
        while (testSize)
        {
            if (msdFatfsInstance->deviceState != kStatus_DEV_Attached)
            {
                USB_HostMsdFatfsTestDone();
                return;
            }
            DWT->CYCCNT = 0;
            DWT->CTRL |= (1 << DWT_CTRL_CYCCNTENA_Pos);
            fatfsCode = f_read(&file, testThroughputBuffer, THROUGHPUT_BUFFER_SIZE, &resultSize);
            if (fatfsCode)
            {
                usb_echo("read error\r\n");
                f_close(&file);
                USB_HostMsdFatfsTestDone();
                return;
            }
            totalTime += DWT->CYCCNT;
            DWT->CTRL &= ~(1 << DWT_CTRL_CYCCNTENA_Pos);
            testSize -= THROUGHPUT_BUFFER_SIZE;
        }
        testSize = testSizeArray[testIndex];
        usb_echo("    read %dKB data the speed is %d KB/s\r\n", testSize,
                 (uint32_t)((uint64_t)testSize * (uint64_t)MCU_CORE_CLOCK / (uint64_t)totalTime));

        fatfsCode = f_close(&file);
        if (fatfsCode)
        {
            USB_HostMsdFatfsTestDone();
            return;
        }
    }

    USB_HostMsdFatfsTestDone();
}

#else

static void USB_HostMsdFatfsDisplayFileInfo(FILINFO *fileInfo)
{
    char *fileName;
#if _USE_LFN
    fileName = (fileInfo->lfname[0] ? fileInfo->lfname : fileInfo->fname;
#else
    fileName = fileInfo->fname;
#endif /* _USE_LFN */
    /* note: if this file/directory don't have one attribute, '_' replace the attribute letter ('R' - readonly, 'H' - hide, 'S' - system) */
    usb_echo("    %s - %c%c%c - %s - %dBytes - %d-%d-%d %d:%d:%d\r\n", (fileInfo->fattrib & AM_DIR) ? "dir" : "fil",
             (fileInfo->fattrib & AM_RDO) ? 'R' : '_',
             (fileInfo->fattrib & AM_HID) ? 'H' : '_',
             (fileInfo->fattrib & AM_SYS) ? 'S' : '_',
             fileName,
             (fileInfo->fsize),
             (uint32_t)((fileInfo->fdate >> 9) + 1980) /* year */,
             (uint32_t)((fileInfo->fdate >> 5) & 0x000Fu) /* month */,
             (uint32_t)(fileInfo->fdate & 0x001Fu) /* day */,
             (uint32_t)((fileInfo->ftime >> 11) & 0x0000001Fu) /* hour */,
             (uint32_t)((fileInfo->ftime >> 5) & 0x0000003Fu) /* minute */,
             (uint32_t)(fileInfo->ftime & 0x0000001Fu) /* second */
             );
}

static FRESULT USB_HostMsdFatfsListDirectory(const TCHAR *path)
{
    FRESULT fatfsCode = FR_OK;
    FILINFO fileInfo;
    DIR dir;
    uint8_t outputLabel = 0;

#if _USE_LFN
    static uint8_t fileNameBuffer[_MAX_LFN];
    fileInfo.lfname = fileNameBuffer;
    fileInfo.lfsize = _MAX_LFN;
#endif /* _USE_LFN */

    fatfsCode = f_opendir(&dir, path);
    if (fatfsCode)
    {
        return fatfsCode;
    }
    while (1)
    {
        fatfsCode = f_readdir(&dir, &fileInfo);
        if ((fatfsCode) || (!fileInfo.fname[0]))
        {
            break;
        }
        outputLabel = 1;
        USB_HostMsdFatfsDisplayFileInfo(&fileInfo);
    }
    if (!outputLabel)
    {
        usb_echo("\r\n");
    }

    return fatfsCode;
}

#if _USE_FORWARD && _FS_TINY
static uint32_t USB_HostMsdFatfsForward(const uint8_t *data, uint32_t dataLength)
{
    uint32_t resultCount = dataLength;

    if (dataLength == 0)
    {
        return 1;
    }
    else
    {
        do
        {
            usb_echo("%c", *data);
            data++;
            resultCount--;
        } while (resultCount);
        return dataLength;
    }
}
#endif

static void USB_HostMsdFatfsTest(usb_host_msd_fatfs_instance_t *msdFatfsInstance)
{
    FRESULT fatfsCode;
    FATFS *fs;
    FIL file;
    FILINFO fileInfo;
    uint32_t freeClusterNumber;
    uint32_t index;
    uint32_t resultSize;
    char *testString;
    uint8_t driverNumberBuffer[3];
    uint32_t i = 0;
    uint32_t destAdrss = 0x10000000;
    uint32_t x = 0;

    typedef  union
      {
        uint32_t cal_value;
        struct {
     			   uint8_t dato_A;
     			   uint8_t dato_B;
     			   uint8_t dato_C;
     			   uint8_t dato_D;
     		   } BIT;
      } TEST_ ;


     TEST_   Valore_Test;

#define BUFFER_SIZE_TEST 200
uint8_t  demoRingBuffer_test[BUFFER_SIZE_TEST];

#define BUFFER_SIZE_MEMORY 30
uint32_t demoRingBuffer_memory[BUFFER_SIZE_MEMORY];

uint32_t s_buffer_rbc[BUFFER_SIZE_TEST];


#if _USE_LFN
    static uint8_t fileNameBuffer[_MAX_LFN];
    fileInfo.lfname = fileNameBuffer;
    fileInfo.lfsize = _MAX_LFN;
#endif /* _USE_LFN */

    /* time delay */
    for (freeClusterNumber = 0; freeClusterNumber < 10000; ++freeClusterNumber)
    {
        __ASM("nop");
    }

    usb_echo("............................fatfs test.....................\r\n");

    usb_echo("fatfs mount as logiacal driver %d......", USBDISK);
    sprintf((char *)&driverNumberBuffer[0], "%c:", USBDISK + '0');
    fatfsCode = f_mount(&fatfs, (char const *)&driverNumberBuffer[0], 0);
    if (fatfsCode)
    {
        usb_echo("error\r\n");
        USB_HostMsdFatfsTestDone();

        EnableIRQ(DEMO_UART_IRQn);
        usb_echo("Enabled UART 485 \r\n");

        return;
    }
    usb_echo("success\r\n");

#if (_FS_RPATH >= 2)
    fatfsCode = f_chdrive((char const *)&driverNumberBuffer[0]);
    if (fatfsCode)
    {
        usb_echo("error\r\n");
        USB_HostMsdFatfsTestDone();
        return;
    }
#endif

#if _USE_MKFS
/*
    usb_echo("test f_mkfs......");
    fatfsCode = f_mkfs((char const *)&driverNumberBuffer[0], FM_SFD | FM_ANY, 0U, testBuffer, _MAX_SS);
    if (fatfsCode)
    {
        usb_echo("error\r\n");
        USB_HostMsdFatfsTestDone();
        return;
    }
    usb_echo("success\r\n");
    */
#endif /* _USE_MKFS */

    usb_echo("test f_getfree:\r\n");
    fatfsCode = f_getfree((char const *)&driverNumberBuffer[0], (DWORD *)&freeClusterNumber, &fs);
    if (fatfsCode)
    {
        usb_echo("error\r\n");
        USB_HostMsdFatfsTestDone();

        EnableIRQ(DEMO_UART_IRQn);
        usb_echo("Enabled UART 485 \r\n");

        return;
    }
    if (fs->fs_type == FS_FAT12)
    {
        usb_echo("    FAT type = FAT12\r\n");
    }
    else if (fs->fs_type == FS_FAT16)
    {
        usb_echo("    FAT type = FAT16\r\n");
    }
    else
    {
        usb_echo("    FAT type = FAT32\r\n");
    }
    usb_echo("    bytes per cluster = %d; number of clusters=%lu \r\n", fs->csize * 512, fs->n_fatent - 2);
    usb_echo("    The free size: %dKB, the total size:%dKB\r\n", (freeClusterNumber * (fs->csize) / 2),
             ((fs->n_fatent - 2) * (fs->csize) / 2));



#if (_FS_RPATH >= 2)
    usb_echo("get current directory......");
    fatfsCode = f_getcwd((TCHAR *)&testBuffer[0], 256);
    if (fatfsCode)
    {
        usb_echo("error\r\n");
        USB_HostMsdFatfsTestDone();
        return;
    }
    usb_echo("%s\r\n", testBuffer);
    usb_echo("change current directory to \"dir_1\"......");
    fatfsCode = f_chdir(_T("dir_1"));
    if (fatfsCode)
    {
        usb_echo("error\r\n");
        USB_HostMsdFatfsTestDone();
        return;
    }
    usb_echo("success\r\n");
    usb_echo("list current directory:\r\n");
    fatfsCode = USB_HostMsdFatfsListDirectory(_T("."));
    if (fatfsCode)
    {
        USB_HostMsdFatfsTestDone();
        return;
    }
    usb_echo("get current directory......");
    fatfsCode = f_getcwd((TCHAR *)&testBuffer[0], 256);
    if (fatfsCode)
    {
        usb_echo("error\r\n");
        USB_HostMsdFatfsTestDone();
        return;
    }
    usb_echo("%s\r\n", testBuffer);
#endif




    usb_echo("file operation:\r\n");
    usb_echo("create file \"LOG_USB.txt\"......");
    //fatfsCode = f_open(&file, _T("1:/LOG_USB.txt"), FA_WRITE | FA_READ | FA_CREATE_ALWAYS);
    fatfsCode = f_open(&file, _T("1:/LOG_USB.txt"), FA_WRITE | FA_READ | FA_OPEN_APPEND); // Apro il file e aggiungo tutto alla fine.
    if (fatfsCode)
    {
        if (fatfsCode == FR_EXIST)
        {
            usb_echo("file exist\r\n");
        }
        else
        {
            usb_echo("error\r\n");
            USB_HostMsdFatfsTestDone();

            EnableIRQ(DEMO_UART_IRQn);
            usb_echo("Enabled UART 485 \r\n");

            return;
        }
    }
    else
    {
        usb_echo("success\r\n");
    }

    usb_echo("test f_write......");

    /*---------------------------------------------------------------------------------------------------*/


//  while( destAdrss < (0x1000F800 - 0x78))
    while( destAdrss < (0x1000F000 - 0x78))
	{


		for (i = 0; i < BUFFER_SIZE_TEST; ++i)
		{
			demoRingBuffer_test[i] = 0;
		}

		x = 0;

		for (i = 0; i < BUFFER_SIZE_MEMORY; i++)
		{
			s_buffer_rbc[i] = *(volatile uint32_t *)(destAdrss + i * 4);
			//PRINTF("\r\n  Address %x  Log %d \r\n", (destAdrss + i * 4), s_buffer_rbc[i]);


			Valore_Test.cal_value = s_buffer_rbc[i];
			demoRingBuffer_test[x]   = Valore_Test.BIT.dato_D;
			demoRingBuffer_test[x+1] = Valore_Test.BIT.dato_C;
			demoRingBuffer_test[x+2] = Valore_Test.BIT.dato_B;
			demoRingBuffer_test[x+3] = Valore_Test.BIT.dato_A;

			x = x + 4;
		}


		char test[400];


		sprintf (test, "%02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d",
						demoRingBuffer_test[33],demoRingBuffer_test[34],demoRingBuffer_test[35],demoRingBuffer_test[36],demoRingBuffer_test[37],
						demoRingBuffer_test[38],demoRingBuffer_test[39],demoRingBuffer_test[40],demoRingBuffer_test[41],demoRingBuffer_test[42],
						demoRingBuffer_test[43],demoRingBuffer_test[44],demoRingBuffer_test[45],demoRingBuffer_test[46],demoRingBuffer_test[47],
						demoRingBuffer_test[48],demoRingBuffer_test[49],demoRingBuffer_test[50],demoRingBuffer_test[51],demoRingBuffer_test[52],
						demoRingBuffer_test[53],demoRingBuffer_test[54],demoRingBuffer_test[55],demoRingBuffer_test[56],demoRingBuffer_test[57],
						demoRingBuffer_test[58],demoRingBuffer_test[59],demoRingBuffer_test[60],demoRingBuffer_test[61],demoRingBuffer_test[62],
						demoRingBuffer_test[63],demoRingBuffer_test[64],demoRingBuffer_test[65],demoRingBuffer_test[66],demoRingBuffer_test[67],
						demoRingBuffer_test[68],demoRingBuffer_test[69],demoRingBuffer_test[70],demoRingBuffer_test[71],demoRingBuffer_test[72],
						demoRingBuffer_test[73],demoRingBuffer_test[74],demoRingBuffer_test[75],demoRingBuffer_test[76],demoRingBuffer_test[77],
						demoRingBuffer_test[78],demoRingBuffer_test[79],demoRingBuffer_test[80],demoRingBuffer_test[81],demoRingBuffer_test[82],
						demoRingBuffer_test[83],demoRingBuffer_test[84],demoRingBuffer_test[85],demoRingBuffer_test[86],demoRingBuffer_test[87],
						demoRingBuffer_test[88],demoRingBuffer_test[89],demoRingBuffer_test[90],demoRingBuffer_test[91],demoRingBuffer_test[92],
						demoRingBuffer_test[93],demoRingBuffer_test[94],demoRingBuffer_test[95],demoRingBuffer_test[96],demoRingBuffer_test[97],
						demoRingBuffer_test[98],demoRingBuffer_test[99],demoRingBuffer_test[100],demoRingBuffer_test[101],demoRingBuffer_test[102],
						demoRingBuffer_test[103],demoRingBuffer_test[104],demoRingBuffer_test[105],demoRingBuffer_test[106],demoRingBuffer_test[107],
						demoRingBuffer_test[108],demoRingBuffer_test[109],demoRingBuffer_test[110],demoRingBuffer_test[111],demoRingBuffer_test[112],
						demoRingBuffer_test[113],demoRingBuffer_test[114]
				);

		/* Mi costruisco la stringa da inviare */

		/* #35# */

		testBuffer[0] = 35; // #
		testBuffer[1] = 51; // 3
		testBuffer[2] = 53; // 5
		testBuffer[3] = 35; // #
		testBuffer[4] = 0;  // Spazio

		char Number[2];
		testBuffer[5] = demoRingBuffer_test[0]; // #

		sprintf (Number,"%02d",demoRingBuffer_test[1]);
		testBuffer[6] = Number[0];
		testBuffer[7] = Number[1];

		testBuffer[8] = demoRingBuffer_test[2]; // #
		testBuffer[9] = 0;					// Spazio

		testBuffer[10] = demoRingBuffer_test[4]; // #

		sprintf (Number,"%02d",demoRingBuffer_test[5]);
		testBuffer[11] = Number[0];
		testBuffer[12] = Number[1];

		testBuffer[13] = demoRingBuffer_test[6]; // #
		testBuffer[14] = 0;					    // Spazio

		x = 15;
		for (i = 7; i <= 32; ++i)
		{
			testBuffer[x] = demoRingBuffer_test[i];
			x++;
		}


		testBuffer[41] = 0;

		x = 42;
		for (i = 0; i <= 244; ++i)
		{
			testBuffer[x] = test[i];
			x++;
		}

		testBuffer[287] = 0;
		testBuffer[288] = 63;
		testBuffer[289] = 63;
		testBuffer[290] = 10;


		fatfsCode = f_write(&file, testBuffer, 300, (UINT *)&resultSize);
		if ((fatfsCode) || (resultSize != 300))
		{
			usb_echo("error\r\n");
			f_close(&file);
			USB_HostMsdFatfsTestDone();

			EnableIRQ(DEMO_UART_IRQn);
			usb_echo("Enabled UART 485 \r\n");

			return;
		}

		destAdrss = destAdrss + 0x78;


	} // End While

    usb_echo("success\r\n");

   /*---------------------------------------------------------------------------------------------------------------------------*/

    /*

    usb_echo("EOF......");
    if (f_printf(&file, _T("%s\r\n"), "EOF") == EOF)
    {
        usb_echo("error\r\n");
        f_close(&file);
        USB_HostMsdFatfsTestDone();
        return;
    }


    fatfsCode = f_sync(&file);
    if (fatfsCode)
    {
        usb_echo("error\r\n");
        f_close(&file);
        USB_HostMsdFatfsTestDone();
        return;
    }
    usb_echo("success\r\n");
*/


#if _USE_FORWARD && _FS_TINY
    usb_echo("test f_forward......");
    fatfsCode = f_forward(&file, USB_HostMsdFatfsForward, 10, &resultSize);
    if (fatfsCode)
    {
        usb_echo("error\r\n");
        f_close(&file);
        USB_HostMsdFatfsTestDone();
        return;
    }
    usb_echo("\r\n");
#endif




    usb_echo("test f_close......");
    fatfsCode = f_close(&file);
    if (fatfsCode)
    {
        usb_echo("error\r\n");
        USB_HostMsdFatfsTestDone();

        EnableIRQ(DEMO_UART_IRQn);
        usb_echo("Enabled UART 485 \r\n");

        return;
    }
    usb_echo("success\r\n");

    USB_HostMsdFatfsTestDone();

    EnableIRQ(DEMO_UART_IRQn);
    usb_echo("Enabled UART 485 \r\n");

}

#endif /* MSD_FATFS_THROUGHPUT_TEST_ENABLE */

void USB_HostMsdTask(void *arg)
{
    usb_status_t status;
    usb_host_msd_fatfs_instance_t *msdFatfsInstance = (usb_host_msd_fatfs_instance_t *)arg;

    if (msdFatfsInstance->deviceState != msdFatfsInstance->prevDeviceState)
    {
        msdFatfsInstance->prevDeviceState = msdFatfsInstance->deviceState;
        switch (msdFatfsInstance->deviceState)
        {
            case kStatus_DEV_Idle:
                break;

            case kStatus_DEV_Attached: /* deivce is attached and numeration is done */
                status = USB_HostMsdInit(msdFatfsInstance->deviceHandle,
                                         &msdFatfsInstance->classHandle); /* msd class initialization */
                g_UsbFatfsClassHandle = msdFatfsInstance->classHandle;
                if (status != kStatus_USB_Success)
                {
                    usb_echo("usb host msd init fail\r\n");
                    return;
                }
                msdFatfsInstance->runState = kRunSetInterface;
                break;

            case kStatus_DEV_Detached: /* device is detached */
                msdFatfsInstance->deviceState = kStatus_DEV_Idle;
                msdFatfsInstance->runState = kRunIdle;
                USB_HostMsdDeinit(msdFatfsInstance->deviceHandle,
                                  msdFatfsInstance->classHandle); /* msd class de-initialization */
                msdFatfsInstance->classHandle = NULL;

                usb_echo("mass storage device detached\r\n");
                break;

            default:
                break;
        }
    }

    /* run state */
    switch (msdFatfsInstance->runState)
    {
        case kRunIdle:
            break;

        case kRunSetInterface: /* set msd interface */
            msdFatfsInstance->runState = kRunIdle;
            msdFatfsInstance->runWaitState = kRunWaitSetInterface;
            status = USB_HostMsdSetInterface(msdFatfsInstance->classHandle, msdFatfsInstance->interfaceHandle, 0,
                                             USB_HostMsdControlCallback, msdFatfsInstance);
            if (status != kStatus_USB_Success)
            {
                usb_echo("set interface fail\r\n");
            }
            break;

        case kRunMassStorageTest: /* set interface succeed */
#if ((defined MSD_FATFS_THROUGHPUT_TEST_ENABLE) && (MSD_FATFS_THROUGHPUT_TEST_ENABLE))
            USB_HostMsdFatfsThroughputTest(msdFatfsInstance); /* test throughput */
#else
            USB_HostMsdFatfsTest(msdFatfsInstance); /* test msd device */
#endif /* MSD_FATFS_THROUGHPUT_TEST_ENABLE */
            msdFatfsInstance->runState = kRunIdle;
            break;

        default:
            break;
    }
}

usb_status_t USB_HostMsdEvent(usb_device_handle deviceHandle,
                              usb_host_configuration_handle configurationHandle,
                              uint32_t eventCode)
{
    usb_status_t status = kStatus_USB_Success;
    usb_host_configuration_t *configuration;
    uint8_t interfaceIndex;
    usb_host_interface_t *interface;
    uint32_t infoValue;
    uint8_t id;

    switch (eventCode)
    {
        case kUSB_HostEventAttach:
            /* judge whether is configurationHandle supported */
            configuration = (usb_host_configuration_t *)configurationHandle;
            for (interfaceIndex = 0; interfaceIndex < configuration->interfaceCount; ++interfaceIndex)
            {
                interface = &configuration->interfaceList[interfaceIndex];
                id = interface->interfaceDesc->bInterfaceClass;
                if (id != USB_HOST_MSD_CLASS_CODE)
                {
                    continue;
                }
                id = interface->interfaceDesc->bInterfaceSubClass;
                if ((id != USB_HOST_MSD_SUBCLASS_CODE_UFI) && (id != USB_HOST_MSD_SUBCLASS_CODE_SCSI))
                {
                    continue;
                }
                id = interface->interfaceDesc->bInterfaceProtocol;
                if (id != USB_HOST_MSD_PROTOCOL_BULK)
                {
                    continue;
                }
                else
                {
                    if (g_MsdFatfsInstance.deviceState == kStatus_DEV_Idle)
                    {
                        /* the interface is supported by the application */
                        g_MsdFatfsInstance.deviceHandle = deviceHandle;
                        g_MsdFatfsInstance.interfaceHandle = interface;
                        g_MsdFatfsInstance.configHandle = configurationHandle;
                        return kStatus_USB_Success;
                    }
                    else
                    {
                        continue;
                    }
                }
            }
            status = kStatus_USB_NotSupported;
            break;

        case kUSB_HostEventNotSupported:
            break;

        case kUSB_HostEventEnumerationDone:
            if (g_MsdFatfsInstance.configHandle == configurationHandle)
            {
                if ((g_MsdFatfsInstance.deviceHandle != NULL) && (g_MsdFatfsInstance.interfaceHandle != NULL))
                {
                    /* the device enumeration is done */
                    if (g_MsdFatfsInstance.deviceState == kStatus_DEV_Idle)
                    {
                        g_MsdFatfsInstance.deviceState = kStatus_DEV_Attached;

                        DisableIRQ(DEMO_UART_IRQn);
                        usb_echo("Disable UART 485 \r\n");



                        USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDevicePID, &infoValue);
                        usb_echo("mass storage device attached:pid=0x%x", infoValue);
                        USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceVID, &infoValue);
                        usb_echo("vid=0x%x ", infoValue);
                        USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceAddress, &infoValue);
                        usb_echo("address=%d\r\n", infoValue);
                    }
                    else
                    {
                        usb_echo("not idle msd instance\r\n");
                        status = kStatus_USB_Error;
                    }
                }
            }
            break;

        case kUSB_HostEventDetach:
            if (g_MsdFatfsInstance.configHandle == configurationHandle)
            {
                /* the device is detached */
                g_UsbFatfsClassHandle = NULL;
                g_MsdFatfsInstance.configHandle = NULL;
                if (g_MsdFatfsInstance.deviceState != kStatus_DEV_Idle)
                {
                    g_MsdFatfsInstance.deviceState = kStatus_DEV_Detached;
                }
            }
            break;

        default:
            break;
    }
    return status;
}

#if ((defined USB_HOST_CONFIG_COMPLIANCE_TEST) && (USB_HOST_CONFIG_COMPLIANCE_TEST))
usb_status_t USB_HostTestEvent(usb_device_handle deviceHandle,
                               usb_host_configuration_handle configurationHandle,
                               uint32_t eventCode)
{
    /* process the same supported device that is identified by configurationHandle */
    static usb_host_configuration_handle s_ConfigHandle = NULL;
    static usb_device_handle s_DeviceHandle = NULL;
    static usb_host_interface_handle s_InterfaceHandle = NULL;
    usb_status_t status = kStatus_USB_Success;
    usb_host_configuration_t *configuration;
    uint8_t interfaceIndex;
    usb_host_interface_t *interface;
    uint32_t id;

    switch (eventCode)
    {
        case kUSB_HostEventAttach:
            /* judge whether is configurationHandle supported */
            configuration = (usb_host_configuration_t *)configurationHandle;
            for (interfaceIndex = 0; interfaceIndex < configuration->interfaceCount; ++interfaceIndex)
            {
                interface = &configuration->interfaceList[interfaceIndex];
                USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceVID, &id);
                if (id == 0x1a0a) /* certification Vendor ID */
                {
                    usb_echo("cetification test device VID match\r\n");
                    s_DeviceHandle = deviceHandle;
                    s_InterfaceHandle = interface;
                    s_ConfigHandle = configurationHandle;
                    return kStatus_USB_Success;
                }
            }
            status = kStatus_USB_NotSupported;
            break;

        case kUSB_HostEventNotSupported:
            usb_echo("Unsupported Device\r\n");
            break;

        case kUSB_HostEventEnumerationDone:
            if (s_ConfigHandle == configurationHandle)
            {
                USB_HostTestModeInit(s_DeviceHandle);
            }
            break;

        case kUSB_HostEventDetach:
            if (s_ConfigHandle == configurationHandle)
            {
                usb_echo("PET test device detach\r\n");
                USB_HostCloseDeviceInterface(s_DeviceHandle, s_InterfaceHandle);
                /* the device is detached */
                s_DeviceHandle = NULL;
                s_InterfaceHandle = NULL;
                s_ConfigHandle = NULL;
            }
            break;

        default:
            break;
    }
    return status;
}
#endif
