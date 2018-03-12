
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "io.h"

xSemaphoreHandle btn_semaphore;
long task_woken;

void GPIO_IRQ_HANDLER(void) {
	Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, GPIO_INTERRUPT_PORT,
			1 << GPIO_INTERRUPT_PIN);
	xSemaphoreGiveFromISR(btn_semaphore, &task_woken);;
}

/* TX thread */
static void vTXTask(void *pvParameters) {
	while (1) {
		if (xSemaphoreTake(btn_semaphore, portMAX_DELAY)) {
			puts("button pressed!");
		}
	}
}

/* RX toggle thread */
static void vRXTask(void *pvParameters) {
	while (1) {
		vTaskDelay(200);
	}
}

int main(void) {
	prvSetupHardware();

	//Setup the semaphore
	 vSemaphoreCreateBinary(btn_semaphore);

	io_init();



	/* TX toggle thread */
	xTaskCreate(vTXTask, (signed char * ) "vTXTask", configMINIMAL_STACK_SIZE * 5,
			NULL, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

	/* RX output thread, simply counts seconds */
	xTaskCreate(vRXTask, (signed char * ) "vRXTask", configMINIMAL_STACK_SIZE * 5,
			NULL, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}
