#pragma once

#include "interrupt.h"
#include "pin.h"
#include "can.h"
//#include "uart.h"
#include "button.h"

class Device {

	CAN_& can;
	Button<false>& set;
	Button<false>& reset;
	Interrupt& rx_interrupt;

//	UART_ &uart;
//	Interrupt &interrupt_DMA;
//	Interrupt& interrupt_usart;

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
			, Button<false>& set
			, Button<false>& reset
			, Interrupt& rx_interrupt
//			, UART_& uart
//			, Interrupt& interrupt_DMA
//			, Interrupt& interrupt_usart
			, Pin& led_tx, Pin& led_rx
	)
	: can {can}
	, set {set}
	, reset {reset}
	, rx_interrupt{rx_interrupt}
//	, uart{uart}
//	, interrupt_DMA    {interrupt_DMA}
//    , interrupt_usart  {interrupt_usart}
	, led_tx {led_tx}, led_rx{led_rx}
	{/*uart.receive();*/
		set.set_down_callback([&]{can.transmit_set_limit(); led_tx = true; led_rx = true;});
		reset.set_down_callback([&]{can.transmit_reset_limit(); led_tx = true; led_rx = true;});
	}
/*
	void uartInterrupt() {

//		if(uart.buffer[0] == 3) {
//			uart.buffer[0] = 4;
//			uart.transmit(1);
//		}

		if (uart.buffer[0] == 'v') {
			uart.buffer[0] = 'v';
			uart.buffer[1] = '0';
			uart.buffer[2] = '1';
			uart.buffer[3] = '0';
			uart.buffer[4] = '0';
			uart.buffer[5] = '\r';
			uart.transmit(6);
			uart.receive();
			return;
		}

		if (uart.buffer[0] == 'O' || uart.buffer[0] == 'C' || uart.buffer[0] == 'Z') {
			uart.buffer[0] = '\r';
			uart.transmit(1);
			uart.receive();
			return;
		}

		if (uart.buffer[0] == 'S') {
//			if (uart.buffer[1] == '5') {
////				MX_CAN_Init_250();
//			} else if (uart.buffer[1] == '6') {
////				MX_CAN_Init_500();
//			}
			uart.buffer[0] = '\r';
			uart.transmit(1);
			uart.receive();
			return;
		}

		if((uart.buffer[0]) == 't') {
			flag = true;
			led_tx = true;
		} else {
			uart.receive();
		}
	}

	void dmaInterrupt() {
		uart.receive();
		led_rx = false;
	}
*/
	using Parent = Device;

//	struct uart_interrupt: Interrupting {
//		Parent &parent;
//		uart_interrupt(Parent &parent) :
//				parent(parent) {
//			parent.interrupt_usart.subscribe(this);
//		}
//		void interrupt() override {
//			parent.uartInterrupt();
//		}
//	} uart_ { *this };
//
//	struct dma_interrupt: Interrupting {
//		Parent &parent;
//		dma_interrupt(Parent &parent) :
//				parent(parent) {
//			parent.interrupt_DMA.subscribe(this);
//		}
//		void interrupt() override {
//			parent.dmaInterrupt();
//		}
//	} dma_ { *this };

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

	void can_receive(){
		can.receive();
		if(can.RxHeader.StdId == 0x1FE) {
			led_tx = false; led_rx = false;

			led_rx = can.is_reset();
			led_tx = can.is_set();
		}
//		uint16_t ts = (uwTick % 0xEA60);
//		uart.buffer[0] = 't';
//		uart.buffer[1] = ToChar((uint8_t) (can.RxHeader.StdId >> 8) & 0xF);
//		uart.buffer[2] = ToChar((uint8_t) (can.RxHeader.StdId >> 4) & 0xF);
//		uart.buffer[3] = ToChar((uint8_t) (can.RxHeader.StdId) & 0xF);
//		uart.buffer[4] = ToChar((uint8_t) (can.RxHeader.DLC));
//
//		int j = 0;
//		for (int i = 0; i < can.RxHeader.DLC; i++) {
//			uart.buffer[i + 5 + j++] = ToChar(can.RxData[i] >> 4);
//			uart.buffer[i + 5 + j] = ToChar(can.RxData[i] & 0b00001111);
//		}
//
//		uart.buffer[can.RxHeader.DLC * 2 + 5] = ToChar((uint8_t) (ts >> 12));
//		uart.buffer[can.RxHeader.DLC * 2 + 6] = ToChar((uint8_t) ((ts >> 8) & 0b00001111));
//		uart.buffer[can.RxHeader.DLC * 2 + 7] = ToChar((uint8_t) ((ts >> 4) & 0b00001111));
//		uart.buffer[can.RxHeader.DLC * 2 + 8] = ToChar((uint8_t) ((ts >> 0) & 0b00001111));
//
//		uart.buffer[can.RxHeader.DLC * 2 + 5 + 4] = '\r'; //+4
//
////		CDC_Transmit_FS(DataRx, RxHeader.DLC * 2 + 10); //+4
//
//		uart.transmit(can.RxHeader.DLC * 2 + 10);
	}

	void operator() (){
//		if(flag) {
//
//			can.TxHeader.DLC = FromChar((uint8_t) (uart.buffer[4]));
//			can.TxHeader.ExtId = 0;
//			can.TxHeader.IDE = CAN_ID_STD;
//			can.TxHeader.RTR = CAN_RTR_DATA;
//			can.TxHeader.StdId = (uint32_t) (FromChar(uart.buffer[1]) << 8)
//					| (uint32_t) (FromChar(uart.buffer[2]) << 4)
//					| (uint32_t) (FromChar(uart.buffer[3]));
//			can.TxHeader.TransmitGlobalTime = DISABLE;
//			if(can.TxHeader.StdId == 0x7D0) {
//				int j = 0;
//				for (uint8_t i = 0; i < can.TxHeader.DLC; i++) {
//					can.TxData[i] = (FromChar(uart.buffer[i + 5 + j++]) << 4)
//							| FromChar(uart.buffer[i + 5 + j]);
//				}
//				can.transmit();
//			}
//			uart.receive();
//			led_tx = false;
//			flag = false;
//		}
	}

};

//Interrupt interrupt_dma;
//Interrupt interrupt_uart;

//void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
//	if (huart->Instance == USART1) {
//		interrupt_dma.interrupt();
//	}
//}
//
//void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
//	if (huart->Instance == USART1) {
//		interrupt_uart.interrupt();
//	}
//}
//
//void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
//	if (huart->Instance == USART1) {
//		interrupt_dma.interrupt();
//	}
//}

Interrupt interrupt_can_rx;

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  interrupt_can_rx.interrupt();
}


