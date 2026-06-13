#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mp.h"

static int pino_in1 = 0;
static int pino_in2 = 0;
static int pino_in3 = 0;
static int pino_in4 = 0;

void mp_config(int in1, int in2, int in3, int in4){
    pino_in1 = in1;
    pino_in2 = in2;
    pino_in3 = in3;
    pino_in4 = in4;

    // Configura os 4 pinos usando uma máscara de bits simples
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = ((1ULL << in1) | (1ULL << in2) | (1ULL << in3) | (1ULL << in4)),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    // Começa com todas as bobinas desligadas (0)
    gpio_set_level(pino_in1, 0);
    gpio_set_level(pino_in2, 0);
    gpio_set_level(pino_in3, 0);
    gpio_set_level(pino_in4, 0);
}

// Função para ligar/desligar as bobinas
static void atualizar_bobinas(int b1, int b2, int b3, int b4) {
    gpio_set_level(pino_in1, b1);
    gpio_set_level(pino_in2, b2);
    gpio_set_level(pino_in3, b3);
    gpio_set_level(pino_in4, b4);
}

void mp_passo_completo(int passos, int delay) {
    int direcao = (passos > 0) ? 1 : -1;
    if (passos < 0) passos = -passos; // Transforma passos negativos em positivos

    static int passo_atual = 0;

    for (int i = 0; i < passos; i++) {
        passo_atual += direcao;

        // Limita o contador entre 0 e 3 
        if (passo_atual > 3) passo_atual = 0;
        if (passo_atual < 0) passo_atual = 3;

        if (passo_atual == 0) atualizar_bobinas(1, 1, 0, 0); // IN1 e IN2 ligadas
        if (passo_atual == 1) atualizar_bobinas(0, 1, 1, 0); // IN2 e IN3 ligadas
        if (passo_atual == 2) atualizar_bobinas(0, 0, 1, 1); // IN3 e IN4 ligadas
        if (passo_atual == 3) atualizar_bobinas(1, 0, 0, 1); // IN4 e IN1 ligadas

        vTaskDelay(pdMS_TO_TICKS(delay));
    }
    atualizar_bobinas(0, 0, 0, 0);
}

void mp_meio_passo(int passos, int delay) {
    int direcao = (passos > 0) ? 1 : -1;
    if (passos < 0) passos = -passos;

    static int passo_atual = 0;

    for (int i = 0; i < passos; i++) {
        passo_atual += direcao;

        // Limita o contador entre 0 e 7
        if (passo_atual > 7) passo_atual = 0;
        if (passo_atual < 0) passo_atual = 7;

        if (passo_atual == 0) atualizar_bobinas(1, 0, 0, 0);
        if (passo_atual == 1) atualizar_bobinas(1, 1, 0, 0);
        if (passo_atual == 2) atualizar_bobinas(0, 1, 0, 0);
        if (passo_atual == 3) atualizar_bobinas(0, 1, 1, 0);
        if (passo_atual == 4) atualizar_bobinas(0, 0, 1, 0);
        if (passo_atual == 5) atualizar_bobinas(0, 0, 1, 1);
        if (passo_atual == 6) atualizar_bobinas(0, 0, 0, 1);
        if (passo_atual == 7) atualizar_bobinas(1, 0, 0, 1);

        vTaskDelay(pdMS_TO_TICKS(delay));
    }

    atualizar_bobinas(0, 0, 0, 0);
}