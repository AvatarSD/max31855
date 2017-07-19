#include "max31855_stm32_hal.h"

int8_t max31855_init(max31855_h *handler)
{
    handler->tc_temp = 0;
    handler->self_temp = 0;
    handler->err = 0;

    HAL_GPIO_WritePin(handler->CS_port, handler->CS_pin, GPIO_PIN_SET);
    HAL_Delay(2);

    __HAL_SPI_ENABLE_IT(handler->hspi, SPI_IT_RXNE);

    HAL_GPIO_WritePin(handler->CS_port, handler->CS_pin, GPIO_PIN_RESET);
    HAL_SPI_Receive_IT(handler->hspi, (uint8_t*)&(handler->data), 4);

    return 0;
}

static inline void swapBytes(volatile uint32_t * data)
{
    uint8_t tmp, *rep = (uint8_t*)data;
    tmp = rep[0];
    rep[0] = rep[3];
    rep[3] = tmp;
    tmp = rep[1];
    rep[1] = rep[2];
    rep[2] = tmp;
}

void max31855_recvd_handler(max31855_h *handler)
{
    HAL_GPIO_WritePin(handler->CS_port, handler->CS_pin, GPIO_PIN_SET);

    swapBytes(&handler->data);

    if(((handler->data)>>16)&0x0001) handler->err |= MAX_FAULT;
    else handler->err &=~ MAX_FAULT;
    if(handler->data&TC_NO_CONNECTED) handler->err |= TC_NO_CONNECTED;
    else handler->err &=~ TC_NO_CONNECTED;
    if(handler->data&TC_SHORT_GND) handler->err |= TC_SHORT_GND;
    else handler->err &=~ TC_SHORT_GND;
    if(handler->data&TC_SHORT_VCC) handler->err |= TC_SHORT_VCC;
    else handler->err &=~ TC_SHORT_VCC;

    /****/
    uint32_t v = (((handler->data) >> 18) & 0x3FFF);
    if ((v>>13) & 0x1) v|=(0xFFFF<<13); //todo: check
    handler->tc_temp = ((float)v)*0.25;
    /****/

    /****/
    v = (((handler->data)>>4) & 0xFFF);
    if ((v>>11) & 0x1) v|=(0xFFFF<<11);
    handler->self_temp = ((float)v)*0.0625;
    /****/

    max31855_temp_recvd(handler);

    HAL_GPIO_WritePin(handler->CS_port, handler->CS_pin, GPIO_PIN_RESET);
    HAL_SPI_Receive_IT(handler->hspi, (uint8_t*)&(handler->data), 4);
}

float max31855_getTemp(max31855_h *handler)
{
    return handler->tc_temp;
}

float max31855_getSelfTemp(max31855_h *handler)
{
    return handler->self_temp;
}

__weak void max31855_temp_recvd(max31855_h * handler)
{
    UNUSED(handler);
}
