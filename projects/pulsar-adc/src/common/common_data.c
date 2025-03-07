/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by pulsar-adc examples.
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "common_data.h"
#include "no_os_pwm.h"
#include "no_os_spi.h"
#include "no_os_uart.h"
#include "pulsar_adc.h"

struct no_os_uart_init_param pulsar_adc_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS,
};

struct no_os_spi_init_param pulsar_adc_spi_init = {
	.device_id = SPI_DEVICE_ID,
	.chip_select = SPI_CS,
	.max_speed_hz = SPI_BAUDRATE,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = SPI_OPS,
	.extra = (void*)SPI_EXTRA,
};

#ifndef USE_STANDARD_SPI
struct spi_engine_offload_init_param spi_engine_offload_init_param = {
	.offload_config = OFFLOAD_RX_EN,
	.rx_dma_baseaddr = DMA_BASEADDR,
};

struct axi_clkgen_init clkgen_init = {
	.name = "rx_clkgen",
	.base = RX_CLKGEN_BASEADDR,
	.parent_rate = 100000000,
};

struct no_os_pwm_init_param trigger_pwm_init = {
	.period_ns = 0, /* set by driver max sample rate */
	.duty_cycle_ns = 0,
	.polarity = NO_OS_PWM_POLARITY_HIGH,
	.platform_ops = PWM_OPS,
	.extra = PWM_EXTRA,
};
#else
struct no_os_gpio_init_param gpio_cnv = {
	.port = GPIO_CNV_PORT,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_CNV,
	.platform_ops = GPIO_OPS,
};
#endif

struct pulsar_adc_init_param pulsar_adc_init_param = {
	.dev_id = ID_AD4020,
	.spi_init = &pulsar_adc_spi_init,
#ifndef USE_STANDARD_SPI
	.trigger_pwm_init = &trigger_pwm_init,
	.clkgen_init = &clkgen_init,
	.axi_clkgen_rate = REFCLK_RATE,
	.reg_access_speed = 1000000,
	.offload_init_param = &spi_engine_offload_init_param,
	.dcache_invalidate_range = DCACHE_INVALIDATE,
#else
	.gpio_cnv = &gpio_cnv,
#endif

#ifdef SPI_ENGINE_OFFLOAD_ENABLE
	.offload_enable = 1,
#else
	.offload_enable = 0,
#endif
	.turbo_mode = 1,
	.high_z_mode = 0,
	.span_compression = 0,
	.en_status_bits = 0,
};
