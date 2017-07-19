#include "max31855_stm32_hal.h"

int8_t max31855_init(max31855_h *handler)
{
    handler->tc_temp = 0;
    handler->self_temp = 0;
    handler->err = 0;

    __HAL_SPI_ENABLE_IT(handler->hspi, SPI_IT_RXNE);

    HAL_GPIO_WritePin(handler->CS_port, handler->CS_pin, GPIO_PIN_RESET);
    HAL_SPI_Receive_IT(handler->hspi, (uint8_t*)&(handler->data), 4);

    return 0;
}

static void swapBytes(volatile uint32_t * data)
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

    handler->self_temp = ((handler->data)>>4) & 0b111111111111;
    handler->tc_temp  = ((handler->data)>>18) & 0b11111111111111;

    max31855_temp_recvd(handler);

    HAL_GPIO_WritePin(handler->CS_port, handler->CS_pin, GPIO_PIN_RESET);
    HAL_SPI_Receive_IT(handler->hspi, (uint8_t*)&(handler->data), 4);
}

float max31855_getTemp(max31855_h *handler)
{
    return ((float)handler->tc_temp)*0.25;
}

float max31855_getSelfTemp(max31855_h *handler)
{
    return ((float)handler->self_temp)*0.0625;
}

__weak void max31855_temp_recvd(max31855_h * handler)
{
    UNUSED(handler);
}
