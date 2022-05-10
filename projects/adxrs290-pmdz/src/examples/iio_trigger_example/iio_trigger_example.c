/***************************************************************************//**
 *   @file   iio_trigger_example.c
 *   @brief  Implementation of IIO trigger example for eval-adxrs290-pmdz project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "iio_trigger_example.h"
#include "iio_adxrs290.h"
#include "iio_trigger.h"
#include "common_data.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define MAX_SIZE_BASE_ADDR		3000
static uint8_t in_buff[MAX_SIZE_BASE_ADDR];
#define GYRO_DDR_BASEADDR		((uint32_t)in_buff)

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief IIO trigger example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run_with_trigs and will
 * 				 not return.
*******************************************************************************/
int iio_trigger_example_main()
{
	int ret;
	struct adxrs290_dev *adxrs290_desc;
	struct iio_data_buffer rd_buf = {
		.buff = (void *)GYRO_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR
	};
	struct iio_hw_trig *adxrs290_trig_desc;
	struct no_os_irq_ctrl_desc *adxrs290_irq_desc;
	struct iio_desc *iio_desc;

	ret = adxrs290_init(&adxrs290_desc, &adxrs290_ip);
	if (ret)
		return ret;

	/* Initialize given IRQ controller*/
	ret = no_os_irq_ctrl_init(&adxrs290_irq_desc, &adxrs290_gpio_irq_ip);
	if (ret)
		return ret;
	adxrs290_gpio_trig_ip.irq_ctrl = adxrs290_irq_desc;

	/* Initialize hardware trigger */
	adxrs290_gpio_trig_ip.iio_desc = &iio_desc,
	ret = iio_hw_trig_init(&adxrs290_trig_desc, &adxrs290_gpio_trig_ip);
	if (ret)
		return ret;

	/* List of devices */
	struct iio_app_device iio_devices[] = {
		{
			.name = "adxrs290",
			.dev = adxrs290_desc,
			.dev_descriptor = &adxrs290_iio_descriptor,
			.read_buff = &rd_buf,
		}
	};

	/* List of triggers */
	struct iio_trigger_init trigs[] = {
		IIO_APP_TRIGGER(ADXRS290_GPIO_TRIG_NAME, adxrs290_trig_desc,
				&adxrs290_iio_trig_desc)
	};

	return iio_app_run_with_trigs(iio_devices, NO_OS_ARRAY_SIZE(iio_devices),
				      trigs, NO_OS_ARRAY_SIZE(trigs), adxrs290_irq_desc, &iio_desc);
}
