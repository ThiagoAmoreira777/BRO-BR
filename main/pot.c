#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "pot.h"

void pot_init(void){
    adc1_config_width(ADC_WIDTH_BIT_12);
}

void pot_configurar(adc1_channel_t canal){
    adc1_config_channel_atten(canal, ADC_ATTEN_DB_11);
}

int graus_pot(adc1_channel_t canal) {
    int soma_leituras = 0;
    
    for (int i = 0; i < 10; i++) {
        soma_leituras += adc1_get_raw(canal);
        vTaskDelay(pdMS_TO_TICKS(2));
    }
    
    int media = soma_leituras / 10;
    
    int graus = (media * 180) / 4095;
    return graus;
}
//========================================