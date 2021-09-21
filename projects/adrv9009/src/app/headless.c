/**
 * \file adrv9009/src/app/headless.c
 *
 * \brief Contains example code for user integration with their application
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 *
 */

/****< Insert User Includes Here >***/

#include <stdio.h>
#include "adi_hal.h"
#include "spi.h"
#include "spi_extra.h"
#include "gpio_extra.h"
#include "error.h"
#include "delay.h"
#include "parameters.h"
#include "util.h"
#include "axi_dac_core.h"
#include "axi_adc_core.h"
#include "axi_dmac.h"
#ifndef ALTERA_PLATFORM
#include "xil_cache.h"
#endif
#include "talise.h"
#include "talise_config.h"
#include "app_config.h"
#include "app_clocking.h"
#include "app_jesd.h"
#include "app_transceiver.h"
#include "app_talise.h"
#include "ad9528.h"

#ifdef IIO_SUPPORT

#include "iio.h"
#include "iio_app.h"
#include "iio_axi_adc.h"
#include "iio_axi_dac.h"

int32_t start_iiod(struct axi_dmac *rx_dmac, struct axi_dmac *tx_dmac,
		   struct axi_adc *rx_adc, struct axi_dac *tx_dac)
{
	struct iio_axi_adc_init_param	iio_axi_adc_init_par;
	struct iio_axi_dac_init_param	iio_axi_dac_init_par;
	struct iio_axi_adc_desc		*iio_axi_adc_desc;
	struct iio_axi_dac_desc		*iio_axi_dac_desc;
	struct iio_device		*adc_dev_desc;
	struct iio_device		*dac_dev_desc;
	int32_t				status;

#ifndef ADRV9008_2
	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = rx_adc,
		.rx_dmac = rx_dmac,
#ifndef PLATFORM_MB
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange
#endif
	};
#endif
#ifndef ADRV9008_1
	iio_axi_dac_init_par = (struct iio_axi_dac_init_param) {
		.tx_dac = tx_dac,
		.tx_dmac = tx_dmac,
#ifndef PLATFORM_MB
		.dcache_flush_range = (void (*)(uint32_t, uint32_t))Xil_DCacheFlushRange,
#endif
	};
#endif

#ifndef ADRV9008_2
	status = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (status < 0)
		return status;
#endif

#ifndef ADRV9008_1
	status = iio_axi_dac_init(&iio_axi_dac_desc, &iio_axi_dac_init_par);
	if(status < 0)
		return status;
#endif

#ifndef ADRV9008_2
	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &adc_dev_desc);
#endif
#ifndef ADRV9008_1
	iio_axi_dac_get_dev_descriptor(iio_axi_dac_desc, &dac_dev_desc);
#endif

#ifndef ADRV9008_2
	struct iio_data_buffer read_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = 0xFFFFFFFF,
	};
#endif
#ifndef ADRV9008_1
	struct iio_data_buffer write_buff = {
		.buff = (void *)DAC_DDR_BASEADDR,
		.size = 0xFFFFFFFF,
	};
#endif

	struct iio_app_device devices[] = {
#ifndef ADRV9008_2
		IIO_APP_DEVICE("axi_adc", iio_axi_adc_desc, adc_dev_desc,
			       &read_buff, NULL),
#endif
#ifndef ADRV9008_1
		IIO_APP_DEVICE("axi_dac", iio_axi_dac_desc, dac_dev_desc,
			       NULL, &write_buff)
#endif
	};

	return iio_app_run(devices, ARRAY_SIZE(devices));
}

#endif // IIO_SUPPORT

/**********************************************************/
/**********************************************************/
/********** Talise Data Structure Initializations ********/
/**********************************************************/
/**********************************************************/

int main(void)
{
	adiHalErr_t err;
	int status;

	// compute the lane rate from profile settings
	// lane_rate = input_rate * M * 20 / L
	// 		where L and M are explained in taliseJesd204bFramerConfig_t comments
	uint32_t rx_lane_rate_khz = talInit.rx.rxProfile.rxOutputRate_kHz *
				    talInit.jesd204Settings.framerA.M * (20 /
						    hweight8(talInit.jesd204Settings.framerA.serializerLanesEnabled));
	uint32_t rx_div40_rate_hz = rx_lane_rate_khz * (1000 / 40);
	uint32_t tx_lane_rate_khz = talInit.tx.txProfile.txInputRate_kHz *
				    talInit.jesd204Settings.deframerA.M * (20 /
						    hweight8(talInit.jesd204Settings.deframerA.deserializerLanesEnabled));
	uint32_t tx_div40_rate_hz = tx_lane_rate_khz * (1000 / 40);
	uint32_t rx_os_lane_rate_khz = talInit.obsRx.orxProfile.orxOutputRate_kHz *
				       talInit.jesd204Settings.framerB.M * (20 /
						       hweight8(talInit.jesd204Settings.framerB.serializerLanesEnabled));
	uint32_t rx_os_div40_rate_hz = rx_os_lane_rate_khz * (1000 / 40);

	// compute the local multiframe clock
	// serializer:   lmfc_rate = (lane_rate * 100) / (K * F)
	// deserializer: lmfc_rate = (lane_rate * 100) / (K * 2 * M / L)
	// 		where K, F, L, M are explained in taliseJesd204bFramerConfig_t comments
	uint32_t rx_lmfc_rate = (rx_lane_rate_khz * 100) /
				(talInit.jesd204Settings.framerA.K * talInit.jesd204Settings.framerA.F);
	uint32_t tx_lmfc_rate = (tx_lane_rate_khz * 100) /
				(talInit.jesd204Settings.deframerA.K * 2 * talInit.jesd204Settings.deframerA.M /
				 hweight8(talInit.jesd204Settings.deframerA.deserializerLanesEnabled));
	uint32_t rx_os_lmfc_rate = (rx_os_lane_rate_khz * 100) /
				   (talInit.jesd204Settings.framerB.K * talInit.jesd204Settings.framerB.F);

	uint32_t lmfc_rate = min(rx_lmfc_rate, rx_os_lmfc_rate);
	lmfc_rate = min(tx_lmfc_rate, lmfc_rate);

	struct axi_adc_init rx_adc_init = {
		"rx_adc",
		RX_CORE_BASEADDR,
		TALISE_NUM_CHANNELS
	};
	struct axi_adc *rx_adc;

	struct axi_dac_init tx_dac_init = {
		"tx_dac",
		TX_CORE_BASEADDR,
		TALISE_NUM_CHANNELS,
		NULL
	};
	struct axi_dac *tx_dac;

	struct axi_dmac_init rx_dmac_init = {
		"rx_dmac",
		RX_DMA_BASEADDR,
		DMA_DEV_TO_MEM,
		0
	};
	struct axi_dmac *rx_dmac;

	struct axi_dmac_init tx_dmac_init = {
		"tx_dmac",
		TX_DMA_BASEADDR,
		DMA_MEM_TO_DEV,
#ifdef IIO_SUPPORT
		DMA_CYCLIC,
#else
		0,
#endif
	};
	struct axi_dmac *tx_dmac;

#ifdef DAC_DMA_EXAMPLE
	struct gpio_desc *gpio_plddrbypass;
	struct gpio_init_param gpio_init_plddrbypass;
	extern const uint32_t sine_lut_iq[1024];
#endif
#ifndef ALTERA_PLATFORM
	xil_spi_init_param hal_spi_param = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
		.flags = SPI_CS_DECODE
	};
	struct xil_gpio_init_param hal_gpio_param = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_DEVICE_ID
	};
#else
	struct altera_spi_init_param hal_spi_param = {
		.type = NIOS_II_SPI,
		.base_address = SPI_BASEADDR
	};
	struct altera_gpio_init_param hal_gpio_param = {
		.type = NIOS_II_GPIO,
		.device_id = 0,
		.base_address = GPIO_BASEADDR
	};

	hal.extra_gpio = &hal_gpio_param;
#endif
	int t;
	struct adi_hal hal[TALISE_DEVICE_ID_MAX];
	taliseDevice_t tal[TALISE_DEVICE_ID_MAX];
	for (t = TALISE_A; t < TALISE_DEVICE_ID_MAX; t++) {
		hal[t].extra_gpio= &hal_gpio_param;
		hal[t].extra_spi = &hal_spi_param;
		tal[t].devHalInfo = (void *) &hal[t];
	}
	hal[TALISE_A].gpio_adrv_resetb_num = TRX_A_RESETB_GPIO;
	hal[TALISE_A].spi_adrv_csn = ADRV_CS;
#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
	hal[TALISE_B].gpio_adrv_resetb_num = TRX_B_RESETB_GPIO;
	hal[TALISE_B].spi_adrv_csn = ADRV_B_CS;
#endif

	printf("Hello\n");

	/**********************************************************/
	/**********************************************************/
	/************ Talise Initialization Sequence *************/
	/**********************************************************/
	/**********************************************************/

	err = clocking_init(rx_div40_rate_hz,
			    tx_div40_rate_hz,
			    rx_os_div40_rate_hz,
			    talInit.clocks.deviceClock_kHz,
			    lmfc_rate);
	if (err != ADIHAL_OK)
		goto error_0;

	err = jesd_init(rx_div40_rate_hz,
			tx_div40_rate_hz,
			rx_os_div40_rate_hz);
	if (err != ADIHAL_OK)
		goto error_1;

	err = fpga_xcvr_init(rx_lane_rate_khz,
			     tx_lane_rate_khz,
			     rx_os_lane_rate_khz,
			     talInit.clocks.deviceClock_kHz);
	if (err != ADIHAL_OK)
		goto error_2;

	for (t = TALISE_A; t < TALISE_DEVICE_ID_MAX; t++) {
		err = talise_setup(&tal[t], &talInit);
		if (err != ADIHAL_OK)
			goto error_3;
	}
#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
	printf("Performing multi-chip synchronization...\n");
	for(int i=0; i < 12; i++) {
		for (t = TALISE_A; t < TALISE_DEVICE_ID_MAX; t++) {
			err = talise_multi_chip_sync(&tal[t], i);
			if (err != ADIHAL_OK)
				goto error_3;
		}
	}
#endif
	ADIHAL_sysrefReq(tal[TALISE_A].devHalInfo, SYSREF_CONT_ON);

	jesd_rx_watchdog();

	/* Print JESD status */
	jesd_status();

	/* Initialize the DAC core */
#ifndef ADRV9008_1
	status = axi_dac_init(&tx_dac, &tx_dac_init);
	if (status) {
		printf("axi_dac_init() failed with status %d\n", status);
		goto error_3;
	}
#endif

	/* Initialize the ADC core */
#ifndef ADRV9008_2
	status = axi_adc_init(&rx_adc, &rx_adc_init);
	if (status) {
		printf("axi_adc_init() failed with status %d\n", status);
		goto error_3;
	}

#endif
#ifndef ADRV9008_1
	status = axi_dmac_init(&tx_dmac, &tx_dmac_init);
	if (status) {
		printf("axi_dmac_init() tx init error: %d\n", status);
		goto error_3;
	}

#endif
#ifndef ADRV9008_2
	status = axi_dmac_init(&rx_dmac, &rx_dmac_init);
	if (status) {
		printf("axi_dmac_init() rx init error: %d\n", status);
		goto error_3;
	}
#endif

#ifdef DAC_DMA_EXAMPLE
	gpio_init_plddrbypass.extra = &hal_gpio_param;
#ifndef ALTERA_PLATFORM
	gpio_init_plddrbypass.platform_ops = &xil_gpio_ops;
#else
	gpio_init_plddrbypass.platform_ops = &altera_gpio_platform_ops;
#endif
	gpio_init_plddrbypass.number = DAC_FIFO_BYPASS_GPIO;
	status = gpio_get(&gpio_plddrbypass, &gpio_init_plddrbypass);
	if (status) {
		printf("gpio_get() failed with status %d", status);
		goto error_3;
	}
	gpio_direction_output(gpio_plddrbypass, 0);

#ifndef ADRV9008_1
	axi_dac_load_custom_data(tx_dac, sine_lut_iq,
				 ARRAY_SIZE(sine_lut_iq),
				 DAC_DDR_BASEADDR);
#ifndef ALTERA_PLATFORM
	Xil_DCacheFlush();
#endif

	axi_dmac_transfer(tx_dmac, DAC_DDR_BASEADDR, sizeof(sine_lut_iq));

	mdelay(1000);
#endif

	/* Transfer 16384 samples from ADC to MEM */
#ifndef ADRV9008_2
	axi_dmac_transfer(rx_dmac,
			  DDR_MEM_BASEADDR + 0x800000,
			  16384 * TALISE_NUM_CHANNELS *
			  DIV_ROUND_UP(talInit.jesd204Settings.framerA.Np, 8));
#ifndef ALTERA_PLATFORM
	Xil_DCacheInvalidateRange(DDR_MEM_BASEADDR + 0x800000,
				  16384 * TALISE_NUM_CHANNELS *
				  DIV_ROUND_UP(talInit.jesd204Settings.framerA.Np, 8));
#endif
#endif
#endif

#ifdef IIO_SUPPORT
	status = start_iiod(rx_dmac, tx_dmac, rx_adc, tx_dac);
	if (status)
		printf("Tinyiiod error: %d\n", status);
#endif // IIO_SUPPORT

	for (t = TALISE_A; t < TALISE_DEVICE_ID_MAX; t++) {
		talise_shutdown(&tal[t]);
	}
error_3:
	fpga_xcvr_deinit();
error_2:
	jesd_deinit();
error_1:
	clocking_deinit();
error_0:
	printf("Bye\n");
	return SUCCESS;
}
