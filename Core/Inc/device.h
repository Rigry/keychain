#pragma once

#include "interrupt.h"
#include "pin.h"
#include "can.h"
#include "uart.h"

class Device {

	CAN_& can;
	Interrupt& rx_interrupt;

	UART_ &uart;
	Interrupt &interrupt_DMA;
	Interrupt& interrupt_usart;

	Pin& led_tx;
	Pin& led_rx;

	bool flag{false};

	uint8_t ToChar(uint8_t c) {
		if (0 <= c && c <= 9) {
			return c + '0';
		} else if ('a' <= c && c <= 'f') {
			return c + 'a' - 10;
		} else if (10 <= c && c <= 15) {
			return c + 'A' - 10;
		}
		//return c + '0';
	}

	uint8_t FromChar(uint8_t c) {

		if ('0' <= c && c <= '9') {
			return c - '0';
		} else if ('a' <= c && c <= 'f') {
			return c - 'a' + 10;
		} else if ('A' <= c && c <= 'F') {
			return c - 'A' + 10;
		}

	}

public:

	Device  ( CAN_& can
			, Interrupt& rx_interrupt
			, UART_& uart
			, Interrupt& interrupt_DMA
			, Interrupt& interrupt_usart
			, Pin& led_tx, Pin& led_rx
	)
	: can {can}
	, rx_interrupt{rx_interrupt}
	, uart{uart}
	, interrupt_DMA    {interrupt_DMA}
    , interrupt_usart  {interrupt_usart}
	, led_tx {led_tx}, led_rx{led_rx}
	{uart.receive();}

	void uartInterrupt() {
 		flag = true;
	}

	void dmaInterrupt() {
		uart.receive();
		led_rx = false;
	}

	using Parent = Device;

	struct uart_interrupt: Interrupting {
		Parent &parent;
		uart_interrupt(Parent &parent) :
				parent(parent) {
			parent.interrupt_usart.subscribe(this);
		}
		void interrupt() override {
			parent.uartInterrupt();
		}
	} uart_ { *this };

	struct dma_interrupt: Interrupting {
		Parent &parent;
		dma_interrupt(Parent &parent) :
				parent(parent) {
			parent.interrupt_DMA.subscribe(this);
		}
		void interrupt() override {
			parent.dmaInterrupt();
		}
	} dma_ { *this };

	struct can_rx_interrupt: Interrupting {
		Parent &parent;
		can_rx_interrupt(Parent &parent) :
				parent(parent) {
			parent.rx_interrupt.subscribe(this);
		}
		void interrupt() {
			parent.can_receive();
		}
	} can_rx_ { *this };

	void can_receive(){}

	void operator() (){
		if(flag) {
			led_tx = true;
			can.TxHeader.DLC = FromChar((uint8_t) (uart.buffer[4]));
			can.TxHeader.ExtId = 0;
			can.TxHeader.IDE = CAN_ID_STD;
			can.TxHeader.RTR = CAN_RTR_DATA;
			can.TxHeader.StdId = (uint32_t) (FromChar(uart.buffer[1]) << 8)
					| (uint32_t) (FromChar(uart.buffer[2]) << 4)
					| (uint32_t) (FromChar(uart.buffer[3]));
			can.TxHeader.TransmitGlobalTime = DISABLE;
			int j = 0;
			for (uint8_t i = 0; i < can.TxHeader.DLC; i++) {
				can.TxData[i] = (FromChar(uart.buffer[i + 5 + j++]) << 4)
						| FromChar(uart.buffer[i + 5 + j]);
			}
			can.transmit();
			uart.receive();
			led_tx = false;
			flag = false;
		}
	}

};

Interrupt interrupt_dma;
Interrupt interrupt_uart;

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART1) {
		interrupt_dma.interrupt();
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
	if (huart->Instance == USART1) {
		interrupt_uart.interrupt();
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
	if (huart->Instance == USART1) {
		interrupt_dma.interrupt();
	}
}

Interrupt interrupt_can_rx;

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  interrupt_can_rx.interrupt();
}


