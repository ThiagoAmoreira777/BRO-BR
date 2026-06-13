#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "ssd1306.h"
#include "driver/gpio.h"
#include "servo.h"
#include "pot.h"

//========PINOS========
#define LED_INDICADOR 2
#define SERVO_PINO1 25
#define SERVO_PINO2 26
#define SERVO_PINO3 27
#define SERVO_PINO4 4
#define SERVO_PINO5 33
#define POT1 ADC1_CHANNEL_0  // GPIO 36 (Pino Físico 5)
#define POT2 ADC1_CHANNEL_3  // GPIO 39 (Pino Físico 8)
#define POT3 ADC1_CHANNEL_6  // GPIO 34 (Pino Físico 10)
#define POT4 ADC1_CHANNEL_7  // GPIO 35 (Pino Físico 11)
#define POT5 ADC1_CHANNEL_4  // GPIO 32 (Pino Físico 12)
#define I2C_MASTER_NUM I2C_NUM_0
#define BTN_CIMA    18
#define BTN_BAIXO   19
#define BTN_OK      23
//======================

//=====================INDICAÇÃO DE INICIALIZAÇÃO==================
void pisca_led(int tempo, int vezes, int pino){
  gpio_set_direction(pino,GPIO_MODE_OUTPUT);
    for (int i = 0; i < vezes; i++)
    {
        gpio_set_level(pino,1);
     vTaskDelay(tempo / portTICK_PERIOD_MS);
         gpio_set_level(pino,0);                 
         vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
//=================================================================

SSD1306_t display;
int opcao_selecionada = 0; // 0 = Manual, 1 = Gravação, 2 = Reprodução
int posicoes_gravadas[10][5];
int total_posicoes = 5;
int posicao_atual  = 0;
char texto[32]; // espaço para guardar o texto

//=========================DISPLAY OLED MENU=======================
void mostrar_menu(void) {
    ssd1306_clear_screen(&display, false);

    // Cada linha: se for a selecionada, coloca ">" na frente
    if (opcao_selecionada == 0) ssd1306_display_text(&display, 0, "> Modo Manual",   13, false);
    else                        ssd1306_display_text(&display, 0, "  Modo Manual",   13, false);

    if (opcao_selecionada == 1) ssd1306_display_text(&display, 2, "> Modo Gravacao", 15, false);
    else                        ssd1306_display_text(&display, 2, "  Modo Gravacao", 15, false);

    if (opcao_selecionada == 2) ssd1306_display_text(&display, 4, "> Modo Reproduzir", 17, false);
    else                        ssd1306_display_text(&display, 4, "  Modo Reproduzir", 17, false);
}
//=================================================================

//==================
typedef enum {
    MODO_MANUAL,
    MODO_MENU,
    MODO_GRAVACAO,
    MODO_REPRODUCAO
} ModoAtual;

ModoAtual modo_atual = MODO_MENU;

//==================

//================CONFIRMAÇÃO DO BOTÃO=============================
int estado_anterior_cima  = 0;
int estado_anterior_baixo = 0;
int estado_anterior_ok    = 0;

int botao_pressionado(int pino, int *estado_anterior) {
    int estado_atual = gpio_get_level(pino);

    if (estado_atual == 1 && *estado_anterior == 0) {
        vTaskDelay(pdMS_TO_TICKS(80));
        if (gpio_get_level(pino) == 1) {
            *estado_anterior = 1;
            return 1;
        }
    }
    if (estado_atual == 0) {
        *estado_anterior = 0;
    }
    return 0;
}

//=================================================================
void app_main(void){
    //======================CONFIGURAÇÃO=================================
    
    servo_init_timer();
    servo_configurar(SERVO_PINO1, LEDC_CHANNEL_0);
    servo_configurar(SERVO_PINO2, LEDC_CHANNEL_1);
    servo_configurar(SERVO_PINO3, LEDC_CHANNEL_2);
    servo_configurar(SERVO_PINO4, LEDC_CHANNEL_3);
    servo_configurar(SERVO_PINO5, LEDC_CHANNEL_4);

    pot_init();
    pot_configurar(POT1);
    pot_configurar(POT2);
    pot_configurar(POT3);
    pot_configurar(POT4);
    pot_configurar(POT5);
    configuracao_botao(BTN_CIMA);
    configuracao_botao(BTN_BAIXO);
    configuracao_botao(BTN_OK);
    i2c_master_init(&display, 21, 22, -1);
    ssd1306_init(&display, 128, 64);
    ssd1306_clear_screen(&display, false);
    //===================================================================

    // Move todos os servos para a posição inicial de 90 graus.
    servo_mover(90, LEDC_CHANNEL_0);
    servo_mover(90, LEDC_CHANNEL_1);
    servo_mover(90, LEDC_CHANNEL_2);
    servo_mover(90, LEDC_CHANNEL_3);
    servo_mover(90, LEDC_CHANNEL_4);

    adc1_channel_t pots[5] = {POT1, POT2, POT3, POT4, POT5};
    ledc_channel_t servos[5] = {LEDC_CHANNEL_0, LEDC_CHANNEL_1, LEDC_CHANNEL_2, LEDC_CHANNEL_3,  LEDC_CHANNEL_4};

    pisca_led(500, 2, LED_INDICADOR);
    int menu_atualizar = 1;

    while (1) {

if (modo_atual == MODO_MANUAL) {

    ssd1306_clear_screen(&display, false);
    ssd1306_display_text(&display, 2, "  Modo Manual", 13, false);
    ssd1306_display_text(&display, 4, "  OK p/ voltar", 14, false);

    for (int i = 0; i < 5; i++) {
        int graus = graus_pot(pots[i]);
        servo_mover(graus, servos[i]);
    }

    if (botao_pressionado(BTN_OK, &estado_anterior_ok)) {
        modo_atual = MODO_MENU;
        menu_atualizar = 1;
    }



} else if (modo_atual == MODO_MENU) {

    if (menu_atualizar) {
        mostrar_menu();
        menu_atualizar = 0;
    }

    if (botao_pressionado(BTN_CIMA, &estado_anterior_cima)) {
        opcao_selecionada--;
        if (opcao_selecionada < 0) opcao_selecionada = 2;
        menu_atualizar = 1;
    }

    if (botao_pressionado(BTN_BAIXO, &estado_anterior_baixo)) {
        opcao_selecionada++;
        if (opcao_selecionada > 2) opcao_selecionada = 0;
        menu_atualizar = 1;
    }

    if (botao_pressionado(BTN_OK, &estado_anterior_ok)) {
        if (opcao_selecionada == 0) modo_atual = MODO_MANUAL;
        if (opcao_selecionada == 1) modo_atual = MODO_GRAVACAO;
        if (opcao_selecionada == 2) modo_atual = MODO_REPRODUCAO;
        menu_atualizar = 1;
    }

} else if (modo_atual == MODO_GRAVACAO) {
    sprintf(texto, "Gravando: %d/%d", posicao_atual + 1, total_posicoes);

    ssd1306_clear_screen(&display, false);
    ssd1306_display_text(&display, 2, texto, strlen(texto), false);

    for (int i = 0; i < 5; i++) {
        int graus = graus_pot(pots[i]);
        servo_mover(graus, servos[i]);
    }

    vTaskDelay(pdMS_TO_TICKS(500));
        if (botao_pressionado(BTN_OK, &estado_anterior_ok)){
            posicoes_gravadas[posicao_atual][0] = graus_pot(POT1);
            posicoes_gravadas[posicao_atual][1] = graus_pot(POT2);
            posicoes_gravadas[posicao_atual][2] = graus_pot(POT3);
            posicoes_gravadas[posicao_atual][3] = graus_pot(POT4);
            posicoes_gravadas[posicao_atual][4] = graus_pot(POT5);

            ssd1306_clear_screen(&display, false);
            ssd1306_display_text(&display, 3, "Posicao salva!", 13, false);
            vTaskDelay(pdMS_TO_TICKS(800));

            posicao_atual++;

            if (posicao_atual == total_posicoes) {
                posicao_atual = 0;
                modo_atual = MODO_MENU;
                menu_atualizar = 1;
            }
        }

} else if (modo_atual == MODO_REPRODUCAO) {

    sprintf(texto, "Reprod.: %d/%d", posicao_atual + 1, total_posicoes);
    ssd1306_clear_screen(&display, false);
    ssd1306_display_text(&display, 2, texto, strlen(texto), false);

    // Move os 5 servos para a posição salva
    for (int i = 0; i < 5; i++) {
        servo_mover(posicoes_gravadas[posicao_atual][i], servos[i]);
    }

    vTaskDelay(pdMS_TO_TICKS(800)); // espera antes de ir para próxima posição

    posicao_atual++;
    if (posicao_atual == total_posicoes) {
        posicao_atual = 0; // volta para o início — looping
    }

    // OK volta para o menu
    if (botao_pressionado(BTN_OK, &estado_anterior_ok)) {
        posicao_atual = 0;
        modo_atual = MODO_MENU;
        menu_atualizar = 1;
    }

}

}
    vTaskDelay(pdMS_TO_TICKS(20));
}