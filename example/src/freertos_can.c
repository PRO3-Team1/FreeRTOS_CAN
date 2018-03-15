#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "io.h"
#include "can.h"

#define QUEUESIZE 10

xQueueHandle tx_queue_handle = NULL;
xQueueHandle rx_queue_handle = NULL;
xSemaphoreHandle btn_semaphore;


void GPIO_IRQ_HANDLER(void) {
	long task_woken;
	Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, GPIO_INTERRUPT_PORT,
			1 << GPIO_INTERRUPT_PIN);
	xSemaphoreGiveFromISR(btn_semaphore, &task_woken);;
}

void CAN_IRQHandler(void) {
	long task_woken;
	uint32_t IntStatus;
	CAN_MSG_T RcvMsgBuf;
	IntStatus = Chip_CAN_GetIntStatus(LPC_CAN);
	PrintCANErrorInfo(IntStatus);

	if (IntStatus & CAN_ICR_RI) {
		Chip_CAN_Receive(LPC_CAN, &RcvMsgBuf);
		xQueueSendFromISR(rx_queue_handle, &RcvMsgBuf, &task_woken);
	}
}

/* TX thread */
static void vTXTask(void *pvParameters) {
	CAN_MSG_T telegram;

	/* fill data in TX buffer */
	telegram.ID = 420;
	telegram.DLC = 5;
	telegram.Data[0] = 'a';
	telegram.Data[1] = 'b';
	telegram.Data[2] = 'c';
	telegram.Data[3] = 'd';
	telegram.Data[4] = 'e';

	while (1) {
		if (xSemaphoreTake(btn_semaphore, portMAX_DELAY)) {
			puts("button pressed!");
			xQueueSend(tx_queue_handle, &telegram, portMAX_DELAY);
		}
	}
}

/* RX thread */
static void vRXTask(void *pvParameters) {
	CAN_MSG_T telegram;
	while (1) {
		if (xQueueReceive(rx_queue_handle, &telegram, portMAX_DELAY)) {
			puts("Received CAN telegram");
			PrintCANMsg(&telegram);
		}
	}
}

/* CAN thread */
static void vCANTask(void *pvParameters) {
	CAN_MSG_T telegram;
	CAN_BUFFER_ID_T TxBuf;
	while (1) {
		if (xQueueReceive(tx_queue_handle, &telegram, portMAX_DELAY)) {
			puts("Sending CAN telegram");
			PrintCANMsg(&telegram);
			TxBuf = Chip_CAN_GetFreeTxBuf(LPC_CAN);
			Chip_CAN_Send(LPC_CAN, TxBuf, &telegram);
		}
	}
}

int main(void) {
	prvSetupHardware();

//Setup the semaphore
	vSemaphoreCreateBinary(btn_semaphore);

	io_init();
	can_init();

	tx_queue_handle = xQueueCreate(QUEUESIZE, sizeof(CAN_MSG_T));
	rx_queue_handle = xQueueCreate(QUEUESIZE, sizeof(CAN_MSG_T));

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
