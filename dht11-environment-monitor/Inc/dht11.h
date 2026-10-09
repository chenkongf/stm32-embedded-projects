#ifndef __DHT11_H
#define __DHT11_H

#include "main.h"

typedef struct
{
    uint8_t 	temperature_high;
    uint8_t 	temperature_low;
    uint8_t 	humidity_high;
    uint8_t 	humidity_low;
    uint8_t 	checkout;
    uint8_t		status;
} dht11_t;

void moshi_in(void);
void moshi_out(uint8_t i);
uint8_t receive_data(void);
void dht11_init(void);
dht11_t rd_dht11(void);

#endif
