/*
 * can.h
 *
 *  Created on: 12. mar. 2018
 *      Author: Emilie1
 */


#ifndef SRC_CAN_H_
#define SRC_CAN_H_

#include "board.h"

#define CAN_CTRL_NO         0
#define LPC_CAN             (LPC_CAN1)



void can_init(void);
void PrintCANErrorInfo(uint32_t Status);
void PrintCANMsg(CAN_MSG_T *pMsg);
void ReplyNormalMessage(CAN_MSG_T *pRcvMsg);

#endif /* SRC_CAN_H_ */
