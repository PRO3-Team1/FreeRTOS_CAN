/*
 * io.c
 *
 *  Created on: 12. mar. 2018
 *      Author: Emilie1
 */
#include "io.h"
#include "board.h"
void io_init(void) {

	Chip_IOCON_PinMuxSet(LPC_IOCON, GPIO_PORT1, GPIO_PIN1, IOCON_FUNC0);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, GPIO_PORT1, GPIO_PIN1);

	Chip_IOCON_PinMuxSet(LPC_IOCON, GPIO_PORT2, GPIO_PIN2, IOCON_FUNC0);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, GPIO_PORT2, GPIO_PIN2);

	Chip_IOCON_PinMuxSet(LPC_IOCON, GPIO_PORT3, GPIO_PIN3, IOCON_FUNC0);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, GPIO_PORT3, GPIO_PIN3);

	Chip_IOCON_PinMuxSet(LPC_IOCON, GPIO_PORT4, GPIO_PIN4, IOCON_FUNC0);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, GPIO_PORT4, GPIO_PIN4);

	/* Configure GPIO interrupt pin as input */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, GPIO_INTERRUPT_PORT, GPIO_INTERRUPT_PIN);

	/* Configure the GPIO interrupt */
	Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, GPIO_INTERRUPT_PORT,
			1 << GPIO_INTERRUPT_PIN);

	/* Enable interrupt in the NVIC */
	NVIC_ClearPendingIRQ(GPIO_INTERRUPT_NVIC_NAME);
	NVIC_EnableIRQ(GPIO_INTERRUPT_NVIC_NAME);
}

/* Sets up system hardware */
void prvSetupHardware(void) {
	SystemCoreClockUpdate();
	Board_Init();
}
