################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_flash.c \
../drivers/fsl_gpio.c \
../drivers/fsl_pit.c \
../drivers/fsl_sim.c \
../drivers/fsl_smc.c \
../drivers/fsl_uart.c 

OBJS += \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_flash.o \
./drivers/fsl_gpio.o \
./drivers/fsl_pit.o \
./drivers/fsl_sim.o \
./drivers/fsl_smc.o \
./drivers/fsl_uart.o 

C_DEPS += \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_flash.d \
./drivers/fsl_gpio.d \
./drivers/fsl_pit.d \
./drivers/fsl_sim.d \
./drivers/fsl_smc.d \
./drivers/fsl_uart.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MK22DX256VLF5 -DCPU_MK22DX256VLF5_cm4 -DSDK_OS_BAREMETAL -DFSL_RTOS_BM -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DSDK_DEBUGCONSOLE=0 -I"C:\Users\gianluca.pasquali\git\repository\USB_Log_Insert\CMSIS" -I"C:\Users\gianluca.pasquali\git\repository\USB_Log_Insert\drivers" -I"C:\Users\gianluca.pasquali\git\repository\USB_Log_Insert\utilities" -I"C:\Users\gianluca.pasquali\git\repository\USB_Log_Insert\board" -I"C:\Users\gianluca.pasquali\git\repository\USB_Log_Insert\source" -I"C:\Users\gianluca.pasquali\git\repository\USB_Log_Insert" -I"C:\Users\gianluca.pasquali\git\repository\USB_Log_Insert\drivers" -I"C:\Users\gianluca.pasquali\git\repository\USB_Log_Insert\CMSIS" -I"C:\Users\gianluca.pasquali\git\repository\USB_Log_Insert\utilities" -I"C:\Users\gianluca.pasquali\git\repository\USB_Log_Insert\startup" -I"C:\Users\gianluca.pasquali\git\repository\USB_Log_Insert\sources" -I"C:\Users\gianluca.pasquali\git\repository\USB_Log_Insert\usb\host" -I"C:\Users\gianluca.pasquali\git\repository\USB_Log_Insert\usb\include" -I"C:\Users\gianluca.pasquali\git\repository\USB_Log_Insert\usb\host\class" -I"C:\Users\gianluca.pasquali\git\repository\USB_Log_Insert\fatfs\fatfs_include" -I"C:\Users\gianluca.pasquali\git\repository\USB_Log_Insert\osa" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


