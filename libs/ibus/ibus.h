#ifndef _IBUS_
#define _IBUS_
#include <stdint.h>
#include <stdlib.h>


// Параметры протокола iBus
#define	PROTOCOL_LENGTH 0x20 // 32 in decimal -> 32 - 4 = 28 bytes for 14 channels
#define	PROTOCOL_OVERHEAD 3 // excess bytes: MessageID, chkSumHigh, chkSumLow
#define	PROTOCOL_CHANNELS 14
#define	MessageID  0x40 // Command is always 0x40


// Параметры отслеживания состояния iBus потока
enum {
    GET_LENGTH,
    GET_DATA,
    GET_CHKSUML,
    GET_CHKSUMH,
    DISCARD
};

// Структура для хранения данных об iBus сообщении
typedef struct {
	uint16_t channels[PROTOCOL_CHANNELS];
} iBusInfo;


void iBus_handler(iBusInfo* current_iBus, char received_byte);
int16_t read_iBusChannel(iBusInfo* current_iBus, int8_t channel_index);


#endif