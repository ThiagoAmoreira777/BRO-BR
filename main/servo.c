#include <stdio.h>
#include "driver/ledc.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "servo.h"

void servo_init_timer(void) {
    ledc_timer_config_t timer = {
        .speed_mode      = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_14_BIT,
        .timer_num       = LEDC_TIMER_0,
        .freq_hz         = 50,
        .clk_cfg         = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer);
}

void servo_configurar(int pino, ledc_channel_t canal) {
    ledc_channel_config_t ledc_canal = {
        .gpio_num   = pino,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = canal,
        .timer_sel  = LEDC_TIMER_0,
        .duty       = 0,
        .hpoint     = 0
    };
    ledc_channel_config(&ledc_canal);
}

void servo_mover(int graus, ledc_channel_t canal) {
    if (graus > 180) graus = 180;
    if (graus < 0) graus = 0;
    
    uint32_t duty = (500 + (graus * 2000) / 180) * 16383 / 20000;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, canal, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, canal);
}

void configuracao_botao(int botao) {
    gpio_config_t io_conf = {
        .mode         = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << botao),
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
}


void servo_reset_posicao(int botao, ledc_channel_t canal) {
    if (gpio_get_level(botao) == 1) {
        servo_mover(90, canal);
        printf("Servo no canal %d resetado para 90°\n", canal);
    }
}