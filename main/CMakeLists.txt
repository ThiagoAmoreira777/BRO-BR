idf_component_register(SRCS "pot.c" "mp.c" "main.c" "servo.c"
                    INCLUDE_DIRS "."
                    REQUIRES ssd1306 driver esp_adc esp_driver_ledc esp_driver_gpio)