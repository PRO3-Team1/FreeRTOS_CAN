/*
 * io.h
 *
 *  Created on: 12. mar. 2018
 *      Author: Emilie1
 */

#ifndef SRC_IO_H_
#define SRC_IO_H_

#define GPIO_INTERRUPT_PIN     10 				/* GPIO pin number mapped to interrupt */
#define GPIO_INTERRUPT_PORT    2 				/* GPIO port number mapped to interrupt */
#define GPIO_IRQ_HANDLER     GPIO_IRQHandler	/* GPIO interrupt IRQ function name */
#define GPIO_INTERRUPT_NVIC_NAME    GPIO_IRQn 	/* GPIO interrupt NVIC interrupt name */

// pins 5-8
#define GPIO_PORT1 1
#define GPIO_PIN1 24
#define GPIO_PORT2 1
#define GPIO_PIN2 23
#define GPIO_PORT3 1
#define GPIO_PIN3 20
#define GPIO_PORT4 0
#define GPIO_PIN4 21

void io_init(void);
void prvSetupHardware(void);

#endif /* SRC_IO_H_ */
