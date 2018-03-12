/*
 * @brief FreeRTOS Blinky example
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "io.h"

xSemaphoreHandle btn_semaphore;

void GPIO_IRQ_HANDLER(void) {
	Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, GPIO_INTERRUPT_PORT,
			1 << GPIO_INTERRUPT_PIN);
	xSemaphoreGiveFromISR(btn_semaphore, NULL);;
}

/* TX thread */
static void vTXTask(void *pvParameters) {
	while (1) {

		if (xSemaphoreTake(btn_semaphore, portMAX_DELAY)) {
			puts("button pressed!");
		}

		vTaskDelay(configTICK_RATE_HZ);
	}
}

/* RX toggle thread */
static void vRXTask2(void *pvParameters) {
	while (1) {

		vTaskDelay(configTICK_RATE_HZ);
	}
}

int main(void) {
	io_init();
	prvSetupHardware();

	/* TX toggle thread */
	xTaskCreate(vTXTask2, (signed char * ) "vTXTask", configMINIMAL_STACK_SIZE,
			NULL, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

	/* RX output thread, simply counts seconds */
	xTaskCreate(vRXTask, (signed char * ) "vRXTask", configMINIMAL_STACK_SIZE,
			NULL, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}
