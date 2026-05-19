################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Modules/device_control/device_control.c 

OBJS += \
./Core/Modules/device_control/device_control.o 

C_DEPS += \
./Core/Modules/device_control/device_control.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Modules/device_control/%.o Core/Modules/device_control/%.su Core/Modules/device_control/%.cyclo: ../Core/Modules/device_control/%.c Core/Modules/device_control/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I"E:/New folder/Project_SmartHome_LTN/SmartHome/Core/Modules/device_control" -I"E:/New folder/Project_SmartHome_LTN/SmartHome/Core/lib/DHT11" -I"E:/New folder/Project_SmartHome_LTN/SmartHome/Core/lib/ssd1306" -I"E:/New folder/Project_SmartHome_LTN/SmartHome/Core/Modules/pc_serial_com" -I"E:/New folder/Project_SmartHome_LTN/SmartHome/Core/Modules/sensor_manager" -I"E:/New folder/Project_SmartHome_LTN/SmartHome/Core/Modules/smart_home_system" -I"E:/New folder/Project_SmartHome_LTN/SmartHome/Core/Modules/touch_button" -I"E:/New folder/Project_SmartHome_LTN/SmartHome/Core/Modules/user_interface" -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Modules-2f-device_control

clean-Core-2f-Modules-2f-device_control:
	-$(RM) ./Core/Modules/device_control/device_control.cyclo ./Core/Modules/device_control/device_control.d ./Core/Modules/device_control/device_control.o ./Core/Modules/device_control/device_control.su

.PHONY: clean-Core-2f-Modules-2f-device_control

