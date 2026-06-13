#ifndef SERVO_H
#define SERVO_H

#include "driver/ledc.h"

void servo_init_timer(void);
void servo_configurar(int pino, ledc_channel_t canal);
void servo_mover(int graus, ledc_channel_t canal);
void configuracao_botao(int botao);
void servo_reset_posicao(int botao, ledc_channel_t canal);

#endif