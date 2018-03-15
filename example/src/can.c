/*
 * can.c
 *
 *  Created on: 12. mar. 2018
 *      Author: Emilie1
 */

#include "can.h"

/**
 * Initialize the CAN bus
 */
void can_init(void) {
	/* LPC_CAN use pins 0.0 and 0.1 */
	Board_CAN_Init(LPC_CAN);
	// LPC_CANAF = accepcens filter
	Chip_CAN_Init(LPC_CAN, LPC_CANAF, LPC_CANAF_RAM);
	// Set baurate on CAN 1
	Chip_CAN_SetBitRate(LPC_CAN, 125000);
	Chip_CAN_EnableInt(LPC_CAN, CAN_IER_BITMASK);
	/* set acceptsance filters to bypass */
	Chip_CAN_SetAFMode(LPC_CANAF, CAN_AF_BYBASS_MODE);
	/* enable interrupts */
	NVIC_EnableIRQ(CAN_IRQn);
}

/**
 * Pretty print CAN bus error code
 */
void PrintCANErrorInfo(uint32_t Status) {
	if (Status & CAN_ICR_EI) {
		DEBUGOUT("Error Warning!\r\n");
	}
	if (Status & CAN_ICR_DOI) {
		DEBUGOUT("Data Overrun!\r\n");
	}
	if (Status & CAN_ICR_EPI) {
		DEBUGOUT("Error Passive!\r\n");
	}
	if (Status & CAN_ICR_ALI) {
		DEBUGOUT("Arbitration lost in the bit: %d(th)\r\n",
				CAN_ICR_ALCBIT_VAL(Status));
	}
	if (Status & CAN_ICR_BEI) {
		DEBUGOUT("Bus error !!!\r\n");
		if (Status & CAN_ICR_ERRDIR_RECEIVE) {
			DEBUGOUT("\t Error Direction: Transmiting\r\n");
		} else {
			DEBUGOUT("\t Error Direction: Receiving\r\n");
		}
		DEBUGOUT("\t Error Location: 0x%2x\r\n", CAN_ICR_ERRBIT_VAL(Status));
		DEBUGOUT("\t Error Type: 0x%1x\r\n", CAN_ICR_ERRC_VAL(Status));
	}
}

/* Print CAN Message */
void PrintCANMsg(CAN_MSG_T *pMsg) {
	uint8_t i;
	DEBUGOUT("\t**************************\r\n");
	DEBUGOUT("\tMessage Information: \r\n");
	DEBUGOUT("\tMessage Type: ");
	if (pMsg->ID & CAN_EXTEND_ID_USAGE) {
		DEBUGOUT(" Extend ID Message");
	} else {
		DEBUGOUT(" Standard ID Message");
	}
	if (pMsg->Type & CAN_REMOTE_MSG) {
		DEBUGOUT(", Remote Message");
	}
	DEBUGOUT("\r\n");
	DEBUGOUT("\tMessage ID :0x%x\r\n", (pMsg->ID & (~CAN_EXTEND_ID_USAGE)));
	DEBUGOUT("\tMessage Data :");
	for (i = 0; i < pMsg->DLC; i++)
		DEBUGOUT("%x ", pMsg->Data[i]);
	DEBUGOUT("\r\n\t**************************\r\n");
}

/* Reply message received */
void ReplyNormalMessage(CAN_MSG_T *pRcvMsg) {
	CAN_MSG_T SendMsgBuf = *pRcvMsg;
	CAN_BUFFER_ID_T TxBuf;
	// Increment id
	SendMsgBuf.ID++;
	// id is made to extended id (toggle bit 30)
	SendMsgBuf.ID |= CAN_EXTEND_ID_USAGE;
	TxBuf = Chip_CAN_GetFreeTxBuf(LPC_CAN);
	Chip_CAN_Send(LPC_CAN, TxBuf, &SendMsgBuf);
	DEBUGOUT("Message Replied!!!\r\n");
	PrintCANMsg(&SendMsgBuf);
}
