#include "uart.h"


/**
	* @brief  Инициализирует UART1 для получения данных с приемника FlySky 
  * @param  None
  * @retval None
  */
void iBusUARTInit(void){
	// Обьявление переменной для последующей инициализации портов ввода вывода
	PORT_InitTypeDef PortInit;
	// Обьявление переменной для последующей инициализации UART	
	UART_InitTypeDef UART_InitStructure; 
	// Разрешение тактирования порта А
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA, ENABLE); 
	
	// Заполнение общих полей переменной PortInit для обеспечения работы UART
	PortInit.PORT_PULL_UP = PORT_PULL_UP_OFF;
	PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	PortInit.PORT_PD_SHM = PORT_PD_SHM_OFF;
	PortInit.PORT_PD = PORT_PD_DRIVER;
	PortInit.PORT_GFEN = PORT_GFEN_OFF;
	PortInit.PORT_FUNC = PORT_FUNC_OVERRID;
	PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;
	PortInit.PORT_MODE = PORT_MODE_DIGITAL;
	
	// Конфигурация 7 пина PORTA как выхода (UART1_TX)
	PortInit.PORT_OE = PORT_OE_OUT;
	PortInit.PORT_Pin = PORT_Pin_7;
	PORT_Init(MDR_PORTA, &PortInit);
	
	// Конфигурация 6 ножки порта PORTF как входа (UART1_RX)
	PortInit.PORT_OE = PORT_OE_IN;
	PortInit.PORT_Pin = PORT_Pin_6;
	PORT_Init(MDR_PORTA, &PortInit);
	
	//Разрешение тактирования блока UART1
	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, ENABLE);

	// Инициализация делителя тактовой частоты для UART1
	UART_BRGInit(MDR_UART1, UART_HCLKdiv1);
	
	// Разрешение прерывания для UART1
	NVIC_EnableIRQ(UART1_IRQn);
	
	// Заполнение полей для переменной UART_InitStructure
	UART_InitStructure.UART_BaudRate = 115200; //тактовая частота передачи данных
	UART_InitStructure.UART_WordLength = UART_WordLength8b; //длина символов 8 бит
	UART_InitStructure.UART_StopBits = UART_StopBits1; //1 стоп бит
	UART_InitStructure.UART_Parity = UART_Parity_No; // нет контроля четности
	UART_InitStructure.UART_FIFOMode = UART_FIFO_OFF; // выключение FIFO буфера
	UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE; // Аппаратный контроль за передачей и приемом
	
	//Инициализация UART1
	UART_Init(MDR_UART1, &UART_InitStructure);
	
	//Разрешение прерывания по приему и окончании передачи UART1
	UART_ITConfig (MDR_UART1, UART_IT_RX, ENABLE);
	UART_ITConfig (MDR_UART1, UART_IT_TX, ENABLE);
	//Разрешение работы UART1
	UART_Cmd(MDR_UART1, ENABLE);
	
}

/**
	* @brief  Инициализирует UART2 для вывода Debug информации в COM порт, посредством функций printf, scanf
  * @param  None
  * @retval None
  */
void DebugUARTInit()
{
  UART_InitTypeDef UART_InitStructure;
  PORT_InitTypeDef PORT_InitStructure;
  uint32_t BaudRateStatus;

  RST_CLK_PCLKcmd((RST_CLK_PCLK_PORTF | RST_CLK_PCLK_UART2), ENABLE);

  PORT_InitStructure.PORT_Pin = DEBUG_UART_PINS;
  PORT_InitStructure.PORT_FUNC = DEBUG_UART_PINS_FUNCTION;
  PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
  PORT_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;
  PORT_Init(DEBUG_UART_PORT, &PORT_InitStructure);

  UART_DeInit(DEBUG_UART); /* Инициализация структуры параметров UART */
  UART_InitStructure.UART_BaudRate = DEBUG_BAUD_RATE;
  UART_InitStructure.UART_WordLength = UART_WordLength8b;
  UART_InitStructure.UART_StopBits = UART_StopBits1;
  UART_InitStructure.UART_Parity = UART_Parity_No;
  UART_InitStructure.UART_FIFOMode = UART_FIFO_ON;
  UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;

  /* ----- Инициализация UART ----- */
  UART_BRGInit(DEBUG_UART, UART_HCLKdiv1);
  BaudRateStatus = UART_Init(DEBUG_UART, &UART_InitStructure);
  if(BaudRateStatus == SUCCESS)
  {
    UART_Cmd(DEBUG_UART, ENABLE);
  }
  else
  {
     while(1);
  }
}

// Базовые функции для получения/ отправки 1 символа
int sendchar(int c)
{
	  // Ожидать, если буфер TX заполнен
    while (UART_GetFlagStatus(DEBUG_UART, UART_FLAG_TXFF) == SET);
    UART_SendData(DEBUG_UART, (uint8_t) c);
  
    return (c);
}

int getkey ()
{
	// Ожидать, пока не начнётся передача
	while (UART_GetFlagStatus(DEBUG_UART, UART_FLAG_RXFE) == SET);
	return ( UART_ReceiveData(DEBUG_UART) );
}

