#pragma once

#include "pin.h"
#include "interrupt.h"

class CAN_
{

	bool set{false};
	bool reset{false};
public:

  CAN_TxHeaderTypeDef TxHeader;
  CAN_RxHeaderTypeDef RxHeader;

  uint8_t TxData[8];
  uint8_t RxData[8];

  uint32_t TxMailBox;

  uint32_t ID{0x001};

  CAN_(){}

  bool is_set(){return set;}
  bool is_reset(){return reset;}

  void transmit_set_limit(){
	  TxHeader.DLC = 8;
	  TxHeader.ExtId = 0;
	  TxHeader.IDE = CAN_ID_STD;
	  TxHeader.RTR = CAN_RTR_DATA;
	  TxHeader.StdId = 0x1FF;
	  TxHeader.TransmitGlobalTime = DISABLE;
	  TxData[0] = 0x73;
	  TxData[1] = 0x70;
	  TxData[2] = 0x65;
	  TxData[3] = 0x65;
	  TxData[4] = 0x64;
	  TxData[5] = 0x00;
	  TxData[6] = 0x00;
	  TxData[7] = 0x50;
	  HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailBox);
  }

	void transmit_reset_limit() {
		TxHeader.DLC = 8;
		TxHeader.ExtId = 0;
		TxHeader.IDE = CAN_ID_STD;
		TxHeader.RTR = CAN_RTR_DATA;
		TxHeader.StdId = 0x1FF;
		TxHeader.TransmitGlobalTime = DISABLE;
		TxData[0] = 0x73;
		TxData[1] = 0x70;
		TxData[2] = 0x65;
		TxData[3] = 0x65;
		TxData[4] = 0x64;
		TxData[5] = 0x00;
		TxData[6] = 0x00;
		TxData[7] = 0x3D;
		HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailBox);
	}

  void receive(){
	  HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &RxHeader, RxData);
		if (RxHeader.StdId == 0x1FE) {
			if (RxData[0] == 0x73
			and	RxData[1] == 0x70
			and	RxData[2] == 0x65
			and	RxData[3] == 0x65
			and	RxData[4] == 0x64
			and	RxData[5] == 0x00
			and	RxData[6] == 0x00) {
				if (RxData[7] == 0x3D) {
					set = true;
					reset = false;
				} else if (RxData[7] == 0x50) {
					reset = true;
					set = false;
				}
			}
		}
  }

};


