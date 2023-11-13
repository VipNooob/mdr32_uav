#include "clock.h"



/**
	* @brief  Настраивает внутренний стабилизатор в зависимости от частоты ядра
  * @param  extraI - @ref SelectRI - указывает значение частоты ядра
  * @retval None
  */
void SetSelectRI(SelectRI extraI)
 {
  uint32_t temp;

  RST_CLK_PCLKcmd(RST_CLK_PCLK_BKP, ENABLE);

  temp = MDR_BKP->REG_0E & 0xFFFFFFC0;
  temp |= (extraI << 3) | extraI;
  MDR_BKP->REG_0E = temp;
}


/**
	* @brief  Настраивает тактирование CPU_CLK и соответственно HCLK
  * @param  None
  * @retval None
  */
void ClockInit(void){
	// Включение HSE осциллятора (внешнего кварцевого резонатора) 
	RST_CLK_HSEconfig(RST_CLK_HSE_ON); 
		                             
	if (RST_CLK_HSEstatus() == SUCCESS){
			// Выбор HSE осциллятора в качестве источника тактовых импульсов для CPU_PLL
			// Установка умножителя тактовой частоты CPU_PLL (16Mhz * 5 = 80Mhz)
			RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, RST_CLK_CPU_PLLmul5);
			// Включение схемы PLL	
			RST_CLK_CPU_PLLcmd(ENABLE); 
			// Выбор PLL в мультиплексоре CPU_C2
			RST_CLK_CPU_PLLuse(ENABLE);
		
			if (RST_CLK_CPU_PLLstatus() == SUCCESS){ 
				
				// Выбор делителя тактовой частоты процессора CPU_C3_prescaler (80MHz / 1 = 80Mhz)
				RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1); 
				// Выбор CPU_C3 в качестве тактовой частоты ядра
				RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
			
			}
			else while(1); // блок CPU_PLL не включился
			
			// Подать тактирование на блок EEPROM
			RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);
			// Задержка доступа к EEPROM при работе ядра на больших частотах
			EEPROM_SetLatency(EEPROM_Latency_3);
			
			// Регулировка внутреннего стабилизатора напряжения при работе на больших частотах 
			SetSelectRI(RI_over_80MHz);
	}
	else while(1); // кварцевый резонатор HSE не включился
}


































