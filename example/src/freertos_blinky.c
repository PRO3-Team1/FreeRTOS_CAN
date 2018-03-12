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

// pins 5-8
#define GPIO_PORT1 1
#define GPIO_PIN1 24
#define GPIO_PORT2 1
#define GPIO_PIN2 23
#define GPIO_PORT3 1
#define GPIO_PIN3 20
#define GPIO_PORT4 0
#define GPIO_PIN4 21

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/
void pin_init(void) {

	Chip_IOCON_PinMuxSet(LPC_IOCON, GPIO_PORT1, GPIO_PIN1, IOCON_FUNC0);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, GPIO_PORT1, GPIO_PIN1);

	Chip_IOCON_PinMuxSet(LPC_IOCON, GPIO_PORT2, GPIO_PIN2, IOCON_FUNC0);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, GPIO_PORT2, GPIO_PIN2);

	Chip_IOCON_PinMuxSet(LPC_IOCON, GPIO_PORT3, GPIO_PIN3, IOCON_FUNC0);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, GPIO_PORT3, GPIO_PIN3);

	Chip_IOCON_PinMuxSet(LPC_IOCON, GPIO_PORT4, GPIO_PIN4, IOCON_FUNC0);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, GPIO_PORT4, GPIO_PIN4);

}

void running(int PORT, int PIN) {
	Chip_GPIO_SetPinState(LPC_GPIO, PORT, PIN, 1);
}

void suspended(int PORT, int PIN) {
	Chip_GPIO_SetPinState(LPC_GPIO, PORT, PIN, 0);
}


/* Sets up system hardware */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();

	/* Initial LED0 state is off */
	Board_LED_Set(0, false);
}

/* LED1 toggle thread */
static void vLEDTask1(void *pvParameters) {
	bool LedState = false;

	while (1) {
		running(GPIO_PORT1, GPIO_PIN1);
		Board_LED_Set(0, LedState);
		LedState = (bool) !LedState;
		suspended(GPIO_PORT1, GPIO_PIN1);
		/* About a 3Hz on/off toggle rate */
		vTaskDelay(configTICK_RATE_HZ / 6);
	}
}

/* LED2 toggle thread */
static void vLEDTask2(void *pvParameters) {
	bool LedState = false;

	while (1) {
		running(GPIO_PORT2, GPIO_PIN2);
		Board_LED_Set(1, LedState);
		LedState = (bool) !LedState;
		suspended(GPIO_PORT2, GPIO_PIN2);
		/* About a 7Hz on/off toggle rate */
		vTaskDelay(configTICK_RATE_HZ / 14);
	}
}

/* UART (or output) thread */
static void vUARTTask(void *pvParameters) {
	int tickCnt = 0;

	while (1) {
		running(GPIO_PORT3, GPIO_PIN3);
		DEBUGOUT("Tick: %d\r\n", tickCnt);
		tickCnt++;
		suspended(GPIO_PORT3, GPIO_PIN3);
		/* About a 1s delay here */
		vTaskDelay(configTICK_RATE_HZ);
	}
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	main routine for FreeRTOS blinky example
 * @return	Nothing, function should not exit
 */

int main(void)
{
	pin_init();
	prvSetupHardware();

	/* LED1 toggle thread */
	xTaskCreate(vLEDTask1, (signed char *) "vTaskLed1",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	/* LED2 toggle thread */
	xTaskCreate(vLEDTask2, (signed char *) "vTaskLed2",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	/* UART output thread, simply counts seconds */
	xTaskCreate(vUARTTask, (signed char *) "vTaskUart",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}

/**
 * @}
 */
