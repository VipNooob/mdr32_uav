#ifndef _UART_
#define _UART_
#include "MDR32FxQI_uart.h"             // Milandr::Drivers:UART
#include "MDR32FxQI_port.h"             // Milandr::Drivers:PORT
#include "MDR32FxQI_rst_clk.h"          // Milandr::Drivers:RST_CLK
#include <stdio.h>

void iBusUARTInit(void);
void DebugUARTInit(void);
int sendchar(int c);
int getkey(void);

#endif