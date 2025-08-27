################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/Third_Party/LittleFS/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/subdir.mk
-include Drivers/STM32L4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/trace/subdir.mk
-include Core/Src/threads/subdir.mk
-include Core/Src/stm/subdir.mk
-include Core/Src/spif/subdir.mk
-include Core/Src/littlefs/subdir.mk
-include Core/Src/func/subdir.mk
-include Core/Src/console/subdir.mk
-include Core/Src/cmds/subdir.mk
-include Core/Src/cJson/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := SMB_Ariel_VER_1_0_0
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
SMB_Ariel_VER_1_0_0.elf \

MAP_FILES += \
SMB_Ariel_VER_1_0_0.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
SMB_Ariel_VER_1_0_0.list \


# All Target
all: main-build

# Main-build Target
main-build: SMB_Ariel_VER_1_0_0.elf secondary-outputs

# Tool invocations
SMB_Ariel_VER_1_0_0.elf SMB_Ariel_VER_1_0_0.map: $(OBJS) $(USER_OBJS) D:\work\WorkTree\Scratch\SMB_Ariel\STM32L496ZGTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "SMB_Ariel_VER_1_0_0.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"D:\work\WorkTree\Scratch\SMB_Ariel\STM32L496ZGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="SMB_Ariel_VER_1_0_0.map" -Wl,--gc-sections -static -L"D:\work\WorkTree\Scratch\MagicSwitch\Debug" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

SMB_Ariel_VER_1_0_0.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "SMB_Ariel_VER_1_0_0.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) SMB_Ariel_VER_1_0_0.elf SMB_Ariel_VER_1_0_0.list SMB_Ariel_VER_1_0_0.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
