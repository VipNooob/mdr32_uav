#include "esc_control.h"

// Частота импульсов, подаваемых на таймер (CLK), Гц
#define PWM_TIMER_CLK 250000


void SimonkInit(){
	
	// Cтруктура для инициализации ШИМ пинов
	PORT_InitTypeDef PortInitStructure;
	// Структура для инициализации таймера
	TIMER_CntInitTypeDef TimerInitStructure;
	// Cтруктура для инициализации ШИМ
	TIMER_ChnInitTypeDef TimerChnInitStructure;
	// Структура для инициализации выхода ШИМ
	TIMER_ChnOutInitTypeDef TimerChnOutInitStructure;
	/*
	Перед началом работы с таймером в первую очередь должны быть включены 
	тактовые сигналы. Параметры задаются в блоке «Сигналы тактовой частоты MDR_RST_CLK».	
	*/
	// Подаем тактирование на регистры таймеров и на порты А, B
	RST_CLK_PCLKcmd (RST_CLK_PCLK_PORTA | RST_CLK_PCLK_TIMER1 | RST_CLK_PCLK_PORTB | RST_CLK_PCLK_TIMER3, ENABLE);
	// Инициализируем порт A для Timer1
	PORT_StructInit(&PortInitStructure);
	PortInitStructure.PORT_FUNC = PORT_FUNC_ALTER;
	PortInitStructure.PORT_OE = PORT_OE_OUT;
	PortInitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PortInitStructure.PORT_Pin = (PORT_Pin_1 | PORT_Pin_3 | PORT_Pin_5);
	PortInitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;
	PORT_Init (MDR_PORTA, &PortInitStructure);
	
	// Инициализируем порт B для Timer3
	PortInitStructure.PORT_Pin = PORT_Pin_5;
	PORT_Init (MDR_PORTA, &PortInitStructure);
	
	// Сбросим настройки Timer1, Timer3
	TIMER_DeInit (MDR_TIMER1);
	TIMER_DeInit (MDR_TIMER3);
	
	TIMER_BRGInit (MDR_TIMER1, TIMER_HCLKdiv64); // TIM_CLK = 80 000 000 / 64 = 1 250 000
	TIMER_BRGInit (MDR_TIMER3, TIMER_HCLKdiv64);
	
	// Коэффициент деления системной тактовой частоты
	uint16_t PrescalerValue =  (uint16_t) (1250000 / PWM_TIMER_CLK) - 1; // 1 250 000 / 250 000 = 5. TIM_CLK = 250 000 Гц.
	
	// Конфигурируем таймеры
	TimerInitStructure.TIMER_Prescaler = PrescalerValue;
	TimerInitStructure.TIMER_Period = 5000 - 1; // Задаем значение основанию счета ARR (5000 тиков от 250 000 это 50 Гц.)
	TimerInitStructure.TIMER_CounterDirection = TIMER_CntDir_Up; // Направление счета
	TimerInitStructure.TIMER_CounterMode = TIMER_CntMode_ClkFixedDir; // Фиксированное направление счета
	TimerInitStructure.TIMER_EventSource = TIMER_EvSrc_TIM_CLK; // Инкремент таймера на каждом входящем фронте TIM_CLK
	TimerInitStructure.TIMER_FilterSampling = TIMER_FDTS_TIMER_CLK_div_1;
	TimerInitStructure.TIMER_ARR_UpdateMode = TIMER_ARR_Update_Immediately;
	TimerInitStructure.TIMER_ETR_FilterConf = TIMER_Filter_1FF_at_TIMER_CLK;
	TimerInitStructure.TIMER_ETR_Prescaler = TIMER_ETR_Prescaler_None;
	TimerInitStructure.TIMER_ETR_Polarity = TIMER_ETRPolarity_NonInverted;
	TimerInitStructure.TIMER_BRK_Polarity = TIMER_BRKPolarity_NonInverted;
	TIMER_CntInit (MDR_TIMER1,&TimerInitStructure);
	TIMER_CntInit (MDR_TIMER3,&TimerInitStructure);
	
	// Конфигурируем ШИМ каналы таймеров
	// Настраиваем 1 канал Timer1
	TIMER_ChnStructInit (&TimerChnInitStructure);
	TimerChnInitStructure.TIMER_CH_Number = TIMER_CHANNEL1;
	TimerChnInitStructure.TIMER_CH_Mode = TIMER_CH_MODE_PWM;
	TimerChnInitStructure.TIMER_CH_CCR1_Ena =	DISABLE; // CCR1 не используем.
	TimerChnInitStructure.TIMER_CH_CCR_UpdateMode = TIMER_CH_CCR_Update_Immediately; // Регистр CCR можно обновлять в любое время (CCR не используем).
	TimerChnInitStructure.TIMER_CH_FilterConf = TIMER_Filter_1FF_at_TIMER_CLK; // Входной сигнал от TIMER_CLK фиксируется одним триггером.
	TimerChnInitStructure.TIMER_CH_Prescaler = TIMER_CH_Prescaler_None; // В канале частота не делится.
	TimerChnInitStructure.TIMER_CH_ETR_Ena = DISABLE; // ETR не используется.
	TimerChnInitStructure.TIMER_CH_ETR_Reset = TIMER_CH_ETR_RESET_Disable; // Сброс ETR не производится.

	
	/*
	В простейшем случае можно считать, что когда REF = 1, на выходе 
	микроконтроллера, используемого под ШИМ, тоже будет логическая 
	единица; когда же REF = 0, то и на выходе микроконтроллера будет 
	логический ноль. Форматов формирования сигнала REF может быть целых 
	восемь, но чаще всего применяется один единственный формат –
	TIMER_CH_REF_Format6, соответствующий привычной логике работы ШИМ. 
	Его и выберем
	*/
	TimerChnInitStructure.TIMER_CH_REF_Format = TIMER_CH_REF_Format6;
	TIMER_ChnInit (MDR_TIMER1, &TimerChnInitStructure);
	
	// Настраиваем 2 канал Timer1
	TimerChnInitStructure.TIMER_CH_Number = TIMER_CHANNEL2;
	TIMER_ChnInit (MDR_TIMER1, &TimerChnInitStructure);
	
	// Настраиваем 3 канал Timer1, 3 канал Timer3
	TimerChnInitStructure.TIMER_CH_Number = TIMER_CHANNEL3;
	TIMER_ChnInit (MDR_TIMER1, &TimerChnInitStructure);
	TIMER_ChnInit (MDR_TIMER3, &TimerChnInitStructure);
	
	
	
	// Конфигурируем выход ШИМ канала
	// 1 канал Timer1
	TIMER_ChnOutStructInit (&TimerChnOutInitStructure);
	TimerChnOutInitStructure.TIMER_CH_Number = TIMER_CHANNEL1;
	TimerChnOutInitStructure.TIMER_CH_DirOut_Polarity = TIMER_CHOPolarity_NonInverted; // прямой или инвертированный
	TimerChnOutInitStructure.TIMER_CH_DirOut_Source = TIMER_CH_OutSrc_REF; // источник управления выходом
	TimerChnOutInitStructure.TIMER_CH_DirOut_Mode = TIMER_CH_OutMode_Output; // выход
	TIMER_ChnOutInit (MDR_TIMER1, &TimerChnOutInitStructure);
	
	// 2 канал Timer1
	TimerChnOutInitStructure.TIMER_CH_Number = TIMER_CHANNEL2;
	TIMER_ChnOutInit (MDR_TIMER1, &TimerChnOutInitStructure);
	
	// 3 канал Timer1, 3 канал Timer3
	TimerChnOutInitStructure.TIMER_CH_Number = TIMER_CHANNEL3;
	TIMER_ChnOutInit (MDR_TIMER1, &TimerChnOutInitStructure);
	TIMER_ChnOutInit (MDR_TIMER3, &TimerChnOutInitStructure);
	
	// Разрешаем работу Timer1, Timer3
	TIMER_Cmd (MDR_TIMER1, ENABLE);
	TIMER_Cmd (MDR_TIMER3, ENABLE);
	
	TIMER_SetChnCompare (MDR_TIMER1, TIMER_CHANNEL1, 0);
	TIMER_SetChnCompare (MDR_TIMER1, TIMER_CHANNEL2, 0);
	TIMER_SetChnCompare (MDR_TIMER1, TIMER_CHANNEL3, 0);
	TIMER_SetChnCompare (MDR_TIMER3, TIMER_CHANNEL3, 0);
	
}


/**
	* @brief  Калибрует Simonk ESC для работы в диапозоне от 1 до 2 мс
  * @retval None
  */
void SimonkCalibrate(){
	 
	TIMER_SetChnCompare (MDR_TIMER1, TIMER_CHANNEL1, 500);
	TIMER_SetChnCompare (MDR_TIMER1, TIMER_CHANNEL2, 500);
	TIMER_SetChnCompare (MDR_TIMER1, TIMER_CHANNEL3, 500);
	TIMER_SetChnCompare (MDR_TIMER3, TIMER_CHANNEL3, 500);
	SysTick_IntegerDelay(4);
	TIMER_SetChnCompare (MDR_TIMER1, TIMER_CHANNEL1, 250);
	TIMER_SetChnCompare (MDR_TIMER1, TIMER_CHANNEL2, 250);
	TIMER_SetChnCompare (MDR_TIMER1, TIMER_CHANNEL3, 250);
	TIMER_SetChnCompare (MDR_TIMER3, TIMER_CHANNEL3, 250);
	SysTick_IntegerDelay(1);
	TIMER_SetChnCompare (MDR_TIMER1, TIMER_CHANNEL1, 0);
	TIMER_SetChnCompare (MDR_TIMER1, TIMER_CHANNEL2, 0);
	TIMER_SetChnCompare (MDR_TIMER1, TIMER_CHANNEL3, 0);
	TIMER_SetChnCompare (MDR_TIMER3, TIMER_CHANNEL3, 0);


}






