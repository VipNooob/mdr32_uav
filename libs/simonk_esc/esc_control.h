#ifndef _ESC_
#define _ESC_

#include "MDR32FxQI_port.h"             // Milandr::Drivers:PORT
#include "MDR32FxQI_rst_clk.h"          // Milandr::Drivers:RST_CLK
#include "MDR32FxQI_timer.h"            // Milandr::Drivers:TIMER
#include <stdarg.h> 										//  handle variable argument list
#include "clock.h"             


void SimonkInit();
void SimonkCalibrate();

#endif