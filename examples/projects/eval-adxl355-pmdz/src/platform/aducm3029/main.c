/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for aducm3029 platform of eval-adxl355-pmdz project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "parameters.h"
#include "common_data.h"

extern int example_main();

/***************************************************************************//**
 * @brief Main function execution for ADUCM3029 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret;
	struct no_os_gpio_desc *adxl355_gpio_desc;

	adxl355_ip.comm_init.spi_init = adxl355_spi_ip;

	platform_init();

	ret = no_os_gpio_get_optional(&adxl355_gpio_desc, &adxl355_gpio_drdy_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_direction_input(adxl355_gpio_desc);
	if (ret)
		return ret;

	return example_main();
}
