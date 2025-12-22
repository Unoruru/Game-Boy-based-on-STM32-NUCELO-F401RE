################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/SEGGER/Rec/HIF_UART.c 

OBJS += \
./Middlewares/Third_Party/SEGGER/Rec/HIF_UART.o 

C_DEPS += \
./Middlewares/Third_Party/SEGGER/Rec/HIF_UART.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/SEGGER/Rec/%.o Middlewares/Third_Party/SEGGER/Rec/%.su Middlewares/Third_Party/SEGGER/Rec/%.cyclo: ../Middlewares/Third_Party/SEGGER/Rec/%.c Middlewares/Third_Party/SEGGER/Rec/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Core/Inc -I"C:/Users/zjzac/STM32CubeIDE/workspace_1.19.0/Gameboy/Middlewares/Third_Party/SEGGER/OS" -I"C:/Users/zjzac/STM32CubeIDE/workspace_1.19.0/Gameboy/Middlewares/Third_Party/SEGGER/Rec" -I"C:/Users/zjzac/STM32CubeIDE/workspace_1.19.0/Gameboy/Middlewares/Third_Party/SEGGER/SEGGER" -I"C:/Users/zjzac/STM32CubeIDE/workspace_1.19.0/Gameboy/Middlewares/Third_Party/SEGGER/Config" -I"C:/Users/zjzac/STM32CubeIDE/workspace_1.19.0/Gameboy/Drivers/OLED" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-SEGGER-2f-Rec

clean-Middlewares-2f-Third_Party-2f-SEGGER-2f-Rec:
	-$(RM) ./Middlewares/Third_Party/SEGGER/Rec/HIF_UART.cyclo ./Middlewares/Third_Party/SEGGER/Rec/HIF_UART.d ./Middlewares/Third_Party/SEGGER/Rec/HIF_UART.o ./Middlewares/Third_Party/SEGGER/Rec/HIF_UART.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-SEGGER-2f-Rec

