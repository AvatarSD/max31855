#ifndef MAX31855_STM32_HAL_H
#define MAX31855_STM32_HAL_H

//note: include required hal version for yout mcu series(f0/f1/f3 etc)
#include "stm32f4xx.h"
#include "stdbool.h"
#include "inttypes.h"

typedef struct {
    //required to init manually:
    SPI_HandleTypeDef * hspi;
    GPIO_TypeDef * CS_port;
    uint16_t CS_pin;
    //private:
    volatile uint32_t data;
    volatile uint8_t err;
    volatile float tc_temp;
    volatile float self_temp;
} max31855_h;

typedef enum {
    TC_NO_CONNECTED = 0x01,
    TC_SHORT_GND = 0x02,
    TC_SHORT_VCC = 0x04,
    MAX_FAULT = 0x08
} max31855_err;


int8_t max31855_init(max31855_h * handler);
void max31855_recvd_handler(max31855_h *
                            handler); // must be called from spi_rx_cplt hal event
void max31855_temp_recvd(max31855_h * handler);
float max31855_getTemp(max31855_h * handler);
float max31855_getSelfTemp(max31855_h * handler);

inline bool MAX31855_isTcShortVccErr(max31855_h * handler)
{
    return ((handler->err)&TC_SHORT_VCC);
}

inline bool MAX31855_isTcShortGndErr(max31855_h * handler)
{
    return ((handler->err)&TC_SHORT_GND);
}
inline bool MAX31855_isTcNoConnectedErr(max31855_h * handler)
{
    return ((handler->err)&TC_NO_CONNECTED);
}
inline bool MAX31855_isMaxFaultErr(max31855_h * handler)
{
    return ((handler->err)&MAX_FAULT);
}


#endif
