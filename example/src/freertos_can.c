#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "io.h"
#include "can.h"

#define QUEUESIZE 10

xQueueHandle tx_queue_handle = NULL;
xSemaphoreHandle btn_semaphore;
long task_woken;

void GPIO_IRQ_HANDLER(void) {
	Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, GPIO_INTERRUPT_PORT,
			1 << GPIO_INTERRUPT_PIN);
	xSemaphoreGiveFromISR(btn_semaphore, &task_woken);;
}

/* TX thread */
static void vTXTask(void *pvParameters) {
	can_tlg_t telegram;

	/* fill data in TX buffer */
	telegram.ID = 420;
	telegram.DLC = 5;
	telegram.data[0] = 'b';
	telegram.data[1] = 'b';
	telegram.data[2] = 'b';
	telegram.data[3] = 'b';
	telegram.data[4] = 'b';

	while (1) {
		if (xSemaphoreTake(btn_semaphore, portMAX_DELAY)) {
			puts("button pressed!");
			xQueueSend(tx_queue_handle, &telegram, portMAX_DELAY);
		}
	}
}

/* RX thread */
static void vRXTask(void *pvParameters) {
	while (1) {
		vTaskDelay(200);
	}
}

/* CAN thread */
static void vCANTask(void *pvParameters) {
	can_tlg_t telegram;
	while (1) {
		if (xQueueReceive(tx_queue_handle, &telegram, portMAX_DELAY)) {
			printf(" RX Task got item ID = %d\n", telegram.ID);
			printf("data 0 = %c\n", telegram.data[0]);
			printf("data 1 = %c\n", telegram.data[1]);
			printf("data 2 = %c\n", telegram.data[2]);
			printf("data 3 = %c\n", telegram.data[3]);
			printf("data 3 = %c\n", telegram.data[4]);
		}
	}
}

int main(void) {
	prvSetupHardware();

//Setup the semaphore
	vSemaphoreCreateBinary(btn_semaphore);

	io_init();
	can_init();

	tx_queue_handle = xQueueCreate(QUEUESIZE, sizeof(can_tlg_t));

	/* TX toggle thread */
	xTaskCreate(vTXTask, (signed char * ) "vTXTask",
			configMINIMAL_STACK_SIZE * 5, NULL, (tskIDLE_PRIORITY + 1UL),
			(xTaskHandle *) NULL);

	/* RX output thread, simply counts seconds */
	xTaskCreate(vRXTask, (signed char * ) "vRXTask",
			configMINIMAL_STACK_SIZE * 5, NULL, (tskIDLE_PRIORITY + 1UL),
			(xTaskHandle *) NULL);

	/* CAN output thread, simply counts seconds */
	xTaskCreate(vCANTask, (signed char * ) "vCANTask",
			configMINIMAL_STACK_SIZE * 5, NULL, (tskIDLE_PRIORITY + 1UL),
			(xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}
