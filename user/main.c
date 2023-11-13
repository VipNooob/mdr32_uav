#include "MDR32FxQI_config.h"           // Milandr::Device:Startup
#include "MDR32FxQI_utils.h"            // Milandr::Drivers:UTILS


#include "clock.h"
#include "uart.h"
#include "ibus.h"
#include "esc_control.h"




// Структура для хранения данных с iBus
volatile iBusInfo DataMotors;

// Обработчик прерывания UART1
void UART1_IRQHandler(void)
{		
		uint16_t received_byte=0x0000;
	
		//проверка установки флага прерывания после окончания приема данных
		if (UART_GetITStatusMasked(MDR_UART1, UART_IT_RX) == SET){
			//Очистка флага прерывания
			UART_ClearITPendingBit(MDR_UART1, UART_IT_RX);
			
			//Считываем принятый байт
			received_byte = UART_ReceiveData (MDR_UART1); 
			received_byte = (uint8_t)received_byte;
			
			iBus_handler(&DataMotors, received_byte);
		}
		
		if (UART_GetITStatusMasked(MDR_UART1, UART_IT_TX) == SET)
		//Проверка установки флага прерывания после окончания передачи данных
		{
			//Очистка флага прерывания
			UART_ClearITPendingBit(MDR_UART1, UART_IT_TX); 
			
		}
		
}



int main (void) {
 
	ClockInit();
	DebugUARTInit();
	iBusUARTInit();
	SimonkInit();
		
	while (1) 
	{
		printf("%d, %d, %d, %d\n", DataMotors.channels[1], DataMotors.channels[2], DataMotors.channels[3], DataMotors.channels[4]);
	}
}

