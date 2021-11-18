SRCS += $(NO-OS)/util/util.c \
	$(NO-OS)/util/list.c \
	$(PLATFORM_DRIVERS)/delay.c \
	$(PLATFORM_DRIVERS)/timer.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_i2c.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c \
	$(PLATFORM_DRIVERS)/uart.c \
	$(PLATFORM_DRIVERS)/rtc.c \
	$(PLATFORM_DRIVERS)/platform_init.c \
	$(DRIVERS)/cdc/ad7746/ad7746.c \
	$(DRIVERS)/cdc/ad7746/iio_ad7746.c \
	$(DRIVERS)/irq/irq.c \
	$(DRIVERS)/i2c/i2c.c	\
	$(PROJECT)/src/app/headless.c

INCS +=	$(INCLUDE)/uart.h \
	$(INCLUDE)/util.h \
	$(INCLUDE)/list.h \
	$(INCLUDE)/delay.h \
	$(INCLUDE)/timer.h \
	$(INCLUDE)/error.h \
	$(INCLUDE)/irq.h \
	$(INCLUDE)/gpio.h \
	$(INCLUDE)/rtc.h \
	$(INCLUDE)/i2c.h \
	$(INCLUDE)/print_log.h \
	$(PLATFORM_DRIVERS)/irq_extra.h \
	$(PLATFORM_DRIVERS)/timer_extra.h \
	$(PLATFORM_DRIVERS)/uart_extra.h \
	$(PLATFORM_DRIVERS)/rtc_extra.h \
	$(PLATFORM_DRIVERS)/platform_init.h \
	$(PLATFORM_DRIVERS)/i2c_extra.h \
	$(DRIVERS)/cdc/ad7746/ad7746.h \
	$(DRIVERS)/cdc/ad7746/iio_ad7746.h \
	$(PROJECT)/src/app/parameters.h

SRC_DIRS += $(NO-OS)/iio/iio_app

TINYIIOD=y

