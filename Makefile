TARGET = project
MCU = cortex-m3

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

CFLAGS = -mcpu=$(MCU) -mthumb \
         -std=gnu99 -Os \
         -ffunction-sections -fdata-sections \
         -DSTM32F103x8 -DUSE_STDPERIPH_DRIVER \
         -I. -Istm32/include

LDFLAGS = -T stm32/linker/STM32F103C8Tx_FLASH.ld \
          -specs=nosys.specs -specs=nano.specs \
          -Wl,--gc-sections,-Map=$(TARGET).map

C_SOURCES = main.c \
            stm32/system_stm32f10x.c \
            stm32/startup/startup_stm32f10x_md.c

OBJS = $(C_SOURCES:.c=.o)

all: $(TARGET).bin

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)
	$(SIZE) $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).bin $(TARGET).map

flash: $(TARGET).bin
	stm32flash -w $(TARGET).bin -v -g 0x0 /dev/cu.usbserial-0001

.PHONY: all clean flash
