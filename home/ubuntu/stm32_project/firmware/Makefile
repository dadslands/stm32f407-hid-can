# Project name
PROJECT = stm32f407_hid_to_can

# Directories
SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj
BIN_DIR = bin

# Toolchain
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# MCU flags
MCU = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

# Compiler flags
CFLAGS = $(MCU) -Wall -Wextra -O2 -g
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -I$(INC_DIR)
CFLAGS += -DSTM32F407xx -DUSE_HAL_DRIVER

# Linker flags
LDFLAGS = $(MCU) -specs=nano.specs -T stm32f407vg_flash.ld
LDFLAGS += -Wl,--gc-sections

# Libraries
LIBS = -lc -lm -lnosys

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.s)

# Object files
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.s=$(OBJ_DIR)/%.o)

# Include STM32 HAL
CFLAGS += -I$(PWD)/lib/STM32CubeF4/STM32F4xx_HAL_Driver/Inc
CFLAGS += -I$(PWD)/lib/STM32CubeF4/CMSIS/Device/ST/STM32F4xx/Include
CFLAGS += -I$(PWD)/lib/STM32CubeF4/CMSIS/Include

# Add HAL source files
HAL_SRC = $(wildcard lib/STM32CubeF4/STM32F4xx_HAL_Driver/Src/*.c)
OBJ_FILES += $(HAL_SRC:lib/%.c=$(OBJ_DIR)/%.o)

# Targets
.PHONY: all clean flash

all: $(BIN_DIR)/$(PROJECT).bin $(BIN_DIR)/$(PROJECT).hex

$(BIN_DIR)/$(PROJECT).bin: $(BIN_DIR)/$(PROJECT).elf
	$(OBJCOPY) -O binary $< $@

$(BIN_DIR)/$(PROJECT).hex: $(BIN_DIR)/$(PROJECT).elf
	$(OBJCOPY) -O ihex $< $@

$(BIN_DIR)/$(PROJECT).elf: $(OBJ_FILES) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ -o $@ $(LIBS)
	$(SIZE) $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s | $(OBJ_DIR)
	$(AS) $(ASFLAGS) $< -o $@

$(OBJ_DIR)/lib/%.o: lib/%.c | $(OBJ_DIR)/lib
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN_DIR) $(OBJ_DIR) $(OBJ_DIR)/lib:
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

flash: $(BIN_DIR)/$(PROJECT).bin
	st-flash write $(BIN_DIR)/$(PROJECT).bin 0x8000000
