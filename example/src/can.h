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

// make a struct to hold a can telegram
 typedef struct
 {
  unsigned int ID;  // holds 11/29 bit ID
  unsigned int ID_ext; // set to 1 if ID is extended -29 bit, else 0
  unsigned int DLC;  // how many byes of payload
  unsigned int data[8]; // the data payload
 } can_tlg_t;

void can_init(void);
void PrintCANErrorInfo(uint32_t Status);
void PrintCANMsg(CAN_MSG_T *pMsg);
void ReplyNormalMessage(CAN_MSG_T *pRcvMsg);

#endif /* SRC_CAN_H_ */
