#pragma once

#include "pin.h"
#include "interrupt.h"

class CAN_
{

public:

  CAN_TxHeaderTypeDef TxHeader;
  CAN_RxHeaderTypeDef RxHeader;

  uint8_t TxData[8];
  uint8_t RxData[8];

  uint32_t TxMailBox;

  uint32_t ID{0x001};

  CAN_(){}

  void transmit(){
//		TxHeader.RTR = CAN_RTR_DATA;
//		TxHeader.TransmitGlobalTime = DISABLE;
		HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailBox);
  }

  void receive(){
	  HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &RxHeader, RxData);
  }

};


