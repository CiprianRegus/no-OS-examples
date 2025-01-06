NO_OS_INC_DIRS += \
	$(INCLUDE) \
	$(PROJECT)/src/ \
	$(DRIVERS)/api/
	
INCS += $(DRIVERS)/power/lt3074/lt3074.h

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c \
	$(PROJECT)/src/common/common_data.c \
	$(PROJECT)/src/platform/$(PLATFORM)/parameters.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(DRIVERS)/api/no_os_i2c.c \
	$(DRIVERS)/api/no_os_dma.c \
	$(DRIVERS)/api/no_os_uart.c \
	$(DRIVERS)/api/no_os_irq.c \
	$(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/api/no_os_pwm.c \
	$(DRIVERS)/power/lt3074/lt3074.c \
	$(NO-OS)/util/no_os_util.c \
	$(NO-OS)/util/no_os_list.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c \
	$(NO-OS)/util/no_os_crc8.c
