#pragma once

#include "net_buffer.h"
#include "pin.h"

class UART_
{

public:

	UART_() {}

	uint8_t buffer[31];

	void transmit(uint8_t size){
//		buffer.set_size(buffer_size - DMA1_Channel3->CNDTR);
		HAL_UART_Transmit_DMA(&huart1, buffer, size);
//		led_red = true;
	}

	void receive(){
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, buffer, 31);
//		led_red = false;
	}

};

//(255 - DMA1_Channel3->CNDTR)

