################################################################################
#									       #
#     Shared variables:							       #
#	- PROJECT							       #
#	- DRIVERS							       #
#	- INCLUDE							       #
#	- PLATFORM_DRIVERS						       #
#	- NO-OS								       #
#									       #
################################################################################

SRC_DIRS += $(PROJECT)/src \
		$(DRIVERS)/adc/ad9208 \
		$(DRIVERS)/axi_core/axi_adc_core \
		$(DRIVERS)/axi_core/axi_dmac \
		$(DRIVERS)/axi_core/clk_axi_clkgen

ifeq (y,$(strip $(TINYIIOD)))
SRC_DIRS += $(NO-OS)/iio/iio_axi_adc			\
		$(NO-OS)/iio/iio_app

SRCS	+= $(PLATFORM_DRIVERS)/uart.c			\
		$(PLATFORM_DRIVERS)/irq.c		\
		$(NO-OS)/util/list.c 
INCS	+= $(INCLUDE)/uart.h				\
		$(INCLUDE)/list.h			\
		$(INCLUDE)/irq.h			\
		$(PLATFORM_DRIVERS)/irq_extra.h		\
		$(PLATFORM_DRIVERS)/uart_extra.h
		
endif

SRCS += $(DRIVERS)/spi/spi.c						\
	$(DRIVERS)/gpio/gpio.c						\
	$(DRIVERS)/frequency/hmc7044/hmc7044.c
SRCS += $(DRIVERS)/axi_core/jesd204/axi_adxcvr.c			\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.c			\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.c			\
	$(DRIVERS)/axi_core/jesd204/xilinx_transceiver.c		\
	$(NO-OS)/util/util.c
SRCS +=	$(PLATFORM_DRIVERS)/axi_io.c					\
	$(PLATFORM_DRIVERS)/xilinx_spi.c				\
	$(PLATFORM_DRIVERS)/xilinx_gpio.c				\
	$(PLATFORM_DRIVERS)/delay.c

INCS += $(DRIVERS)/frequency/hmc7044/hmc7044.h				\
	$(DRIVERS)/axi_core/jesd204/axi_adxcvr.h			\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.h			\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.h			\
	$(DRIVERS)/axi_core/jesd204/xilinx_transceiver.h
INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h					\
	$(PLATFORM_DRIVERS)/gpio_extra.h
INCS +=	$(INCLUDE)/axi_io.h						\
	$(INCLUDE)/spi.h						\
	$(INCLUDE)/gpio.h						\
	$(INCLUDE)/error.h						\
	$(INCLUDE)/delay.h						\
	$(INCLUDE)/util.h
