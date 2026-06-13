#ifndef POT_H
#define POT_H

#include "driver/adc.h"

void pot_init(void);
void pot_configurar(adc1_channel_t canal);
int graus_pot(adc1_channel_t canal);

#endif