/***************************************************************************//**
 *   @file   iio_trigger_example.c
 *   @brief  Implementation of IIO trigger example for eval-adis1655x project.
 *   @author RBolboac (ramona.gradinariu@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

#include "iio_trigger_example.h"
#include "no_os_print_log.h"
#include "common_data.h"

#define DATA_BUFFER_SIZE 400

uint8_t iio_data_buffer[DATA_BUFFER_SIZE * 13 * sizeof(int)];

/**
 * @brief IIO trigger example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run_with_trigs and will
 * 				 not return.
 */
int iio_trigger_example_main()
{
	int ret;
	struct adis_iio_dev *adis1655x_iio_desc;
	struct iio_data_buffer data_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * 13 * sizeof(int)
	};

	struct iio_hw_trig *adis1655x_trig_desc;
	struct no_os_irq_ctrl_desc *adis1655x_irq_desc;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = adis1655x_iio_init(&adis1655x_iio_desc, &adis1655x_ip);
	if (ret)
		goto exit;

	/* Initialize interrupt controller */
	ret = no_os_irq_ctrl_init(&adis1655x_irq_desc, &adis1655x_gpio_irq_ip);
	if (ret)
		goto exit;

	ret = no_os_irq_set_priority(adis1655x_irq_desc, adis1655x_gpio_trig_ip.irq_id,
				     1);
	if (ret)
		goto exit;

	adis1655x_gpio_trig_ip.irq_ctrl = adis1655x_irq_desc;

	/* Initialize hardware trigger */
	ret = iio_hw_trig_init(&adis1655x_trig_desc, &adis1655x_gpio_trig_ip);
	if (ret)
		goto exit;

	/* List of devices */
	struct iio_app_device iio_devices[] = {
		{
			.name = "adis16550",
			.dev = adis1655x_iio_desc,
			.dev_descriptor = adis1655x_iio_desc->iio_dev,
			.read_buff = &data_buff,
		}
	};

	/* List of triggers */
	struct iio_trigger_init trigs[] = {
		IIO_APP_TRIGGER(ADIS1655X_GPIO_TRIG_NAME, adis1655x_trig_desc,
				&adis_iio_trig_desc)
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adis1655x_uart_ip;
	app_init_param.trigs = trigs;
	app_init_param.nb_trigs = NO_OS_ARRAY_SIZE(trigs);
	app_init_param.irq_desc = adis1655x_irq_desc;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto exit;

	/* Update the reference to iio_desc */
	adis1655x_trig_desc->iio_desc = app->iio_desc;

	ret = iio_app_run(app);

	iio_app_remove(app);

exit:
	iio_hw_trig_remove(adis1655x_trig_desc);
	no_os_irq_ctrl_remove(adis1655x_irq_desc);
	adis1655x_iio_remove(adis1655x_iio_desc);
	if (ret)
		pr_info("Error!\n");
	return ret;
}
