/***************************************************************************//**
 *   @file   channel_output_example.c
 *   @brief  implementation of channel output configuration for eval-ad5460 project
 *   @author Akila MArimuthu (akila.marimuthu@analog.com)
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
#include "ad5460.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"

/***************************************************************************//**
 * @brief Channel output example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int example_main()
{
	struct ad5460_desc *ad5460_desc;
	union ad5460_live_status *live_status;
	int ret, status;
	uint16_t dac_code0, gpi_0, val;
	int32_t output_in_uamps_ch0 = 10000;
	int32_t output_in_mvolts_ch0 = 6000;

	ret = ad5460_init(&ad5460_desc, &ad5460_ip);
	if (ret)
		goto error;

	pr_info("ad5460 successfully initialized!\r\n");

	/* Set channel function */
	ret = ad5460_set_channel_function(ad5460_desc, 0, AD5460_VOLTAGE_OUT);
	if (ret)
		goto error_ad5460;

	/* set output range */
	ret = ad5460_set_channel_vout_range(ad5460_desc, 0, AD5460_VOUT_RANGE_0_12V);
	if (ret)
		goto error_ad5460;

	/* DAC slew configuration */
	ret = ad5460_dac_slew_enable(ad5460_desc, 0, AD5460_STEP_0_8_PERCENT,
				     AD5460_LIN_RATE_4KHZ8);
	if (ret)
		goto error_ad5460;

	/* Thermal RESET configuration */
	ret = ad5460_set_therm_rst(ad5460_desc, true);
	if (ret)
		goto error_ad5460;

	/* GPIO Configuration */
	ret = ad5460_set_gpio_config(ad5460_desc, 0, AD5460_GPIO_SEL_GPI);
	if (ret)
		goto error_ad5460;

	/* Get GPIO input data */
	ret = ad5460_gpio_get(ad5460_desc, 0, &gpi_0);
	if (ret)
		goto error_ad5460;

	/* Diagnostic mux configuration */
	ret = ad5460_set_diag(ad5460_desc, 0, AD5460_DIAG_NO_DIAG);
	if (ret)
		goto error_ad5460;

	/* Set channel 0 output */
	if (ad5460_desc->channel_configs[0].function == AD5460_VOLTAGE_OUT) {
		ret = ad5460_dac_voltage_to_code(ad5460_desc, output_in_mvolts_ch0, &dac_code0,
						 0);
		if (ret)
			goto error_ad5460;

		ret = ad5460_set_channel_dac_code(ad5460_desc, 0, dac_code0);
		if (ret)
			goto error_ad5460;

		pr_info("For channel 0, expected output = %d mV \n DAC code = %d \n",
			output_in_mvolts_ch0, dac_code0);
	} else if (ad5460_desc->channel_configs[0].function == (AD5460_CURRENT_OUT |
			AD5460_CURRENT_OUT_HART)) {
		ret = ad5460_dac_current_to_code(ad5460_desc, output_in_uamps_ch0, &dac_code0,
						 0);
		if (ret)
			goto error_ad5460;

		ret = ad5460_set_channel_dac_code(ad5460_desc, 0, dac_code0);
		if (ret)
			goto error_ad5460;

		pr_info("For channel 0, expected output = %d uA \n DAC code = %d \n",
			output_in_uamps_ch0, dac_code0);
	}
	ret = ad5460_reg_read(ad5460_desc, AD5460_DAC_ACTIVE(0), &val);
	if (ret)
		goto error;

	pr_info("DAC ACTIVE CODE of channel 0 = %d \n", val);

	/* Get live status */
	ret = ad5460_get_live(ad5460_desc, live_status->value);
	if (ret)
		goto error_ad5460;

	return 0;

error_ad5460:
	ad5460_remove(ad5460_desc);
error:
	pr_info("AD5460 Error!\r\n");
	return ret;
}
