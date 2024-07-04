#pragma once

#include "net_buffer.h"
#include "pin.h"

class UART_
{

public:

	UART_() {}

	uint8_t buffer[24];

	void transmit(){
//		buffer.set_size(buffer_size - DMA1_Channel3->CNDTR);
		HAL_UART_Transmit_DMA(&huart1, buffer, 24);
//		led_red = true;
	}

	void receive(){
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, buffer, 24);
//		led_red = false;
	}

};

//(255 - DMA1_Channel3->CNDTR)

