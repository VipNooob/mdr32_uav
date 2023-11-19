#ifndef _CLOCK_
#define _CLOCK_
#include "MDR32FxQI_rst_clk.h"          // Milandr::Drivers:RST_CLK
#include "MDR32FxQI_eeprom.h"           // Milandr::Drivers:EEPROM
#include <MDR32FxQI_config.h>
#include "MDR32FxQI_utils.h"            // Milandr::Drivers:UTILS


// Значения аргумента функции настройки внутреннего стабилизатора напряжения
typedef enum {
  RI_till_10KHz, RI_till_200KHz, RI_till_500KHz, RI_till_1MHz,
  RI_Gens_Off,
  RI_till_40MHz, RI_till_80MHz, RI_over_80MHz
}SelectRI;

void SetSelectRI(SelectRI extraI);
void ClockInit(void);
void SysTick_IntegerDelay(int8_t seconds);
#endif