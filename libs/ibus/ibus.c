#include "ibus.h"


/**
	* @brief  Расшифровывает и сохраняет iBus сообщение
  * @param  current_iBus - @ref iBusInfo* - указывает структуру для хранения iBus данных
	* @param  received_byte - указывает на полученный символ
  * @retval None
  */
void iBus_handler(iBusInfo* current_iBus, char received_byte)
{
	static uint8_t ibus_state = GET_LENGTH;
	static uint8_t current_index = 0; 
	static uint8_t buffer[0x20];
	static uint8_t message_length;
	
	static uint16_t chksum = 0;
	static uint16_t chksuml = 0;
	
    switch (ibus_state)
    {
    case GET_LENGTH:
        // Первый символ должен быть равен длине отправляемого сообщения
        if (received_byte <= PROTOCOL_LENGTH)
        {
            current_index = 0;
            message_length = received_byte - PROTOCOL_OVERHEAD;
            chksum = 0xFFFF - received_byte;
            ibus_state = GET_DATA;
        }
        else
        {
            ibus_state = GET_LENGTH;
        }
        break;

    case GET_DATA:

        buffer[current_index++] = received_byte;
        chksum -= received_byte;

        if (current_index == message_length)
        {
            ibus_state = GET_CHKSUML;
        }
        break;

    case GET_CHKSUML:
        chksuml = received_byte;
        ibus_state = GET_CHKSUMH;
        break;

    case GET_CHKSUMH:
        // Проверка контрольной суммы
        if (chksum == (received_byte << 8) + chksuml)
        {
            switch (buffer[0])
            {
            // Проверка на наличие 0x40 
            case MessageID:

                for (uint8_t i = 1; i < PROTOCOL_CHANNELS * 2 + 1; i += 2)
                {
                    current_iBus->channels[i / 2] = buffer[i] | (buffer[i + 1] << 8);
                }
                break;
            default:
                break;
            }
        }
        ibus_state = GET_LENGTH;
        break;
    }
}
