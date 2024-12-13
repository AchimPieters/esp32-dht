/**
   Copyright 2024 Achim Pieters | StudioPieters®

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   for more information visit https://www.studiopieters.nl
 **/

 #include <stdio.h>
 #include "freertos/FreeRTOS.h"
 #include "freertos/task.h"
 #include "esp_log.h"
 #include "driver/gpio.h"
 #include "dht.h"

 static const char *TAG = "example_main";

 void app_main(void)
 {
     // Determine sensor type based on Kconfig
     dht_sensor_type_t sensor_type;
 #if CONFIG_EXAMPLE_TYPE_DHT11
     sensor_type = DHT_TYPE_DHT11;
 #elif CONFIG_EXAMPLE_TYPE_AM2301
     sensor_type = DHT_TYPE_AM2301;
 #elif CONFIG_EXAMPLE_TYPE_SI7021
     sensor_type = DHT_TYPE_SI7021;
 #else
     #error "No sensor type selected in Kconfig"
 #endif

     gpio_num_t gpio_num = CONFIG_ESP_TEMP_SENSOR_GPIO;

     // Enable internal pull-up resistor if specified in Kconfig
     if (CONFIG_EXAMPLE_INTERNAL_PULLUP) {
         gpio_pullup_en(gpio_num);
     } else {
         gpio_pullup_dis(gpio_num);
     }

     while (true)
     {
         float humidity = 0, temperature = 0;

         esp_err_t result = dht_read_float_data(sensor_type, gpio_num, &humidity, &temperature);
         if (result == ESP_OK)
         {
             ESP_LOGI(TAG, "Humidity: %.1f%% Temperature: %.1f°C", humidity, temperature);
         }
         else
         {
             ESP_LOGE(TAG, "Failed to read sensor data: %s", esp_err_to_name(result));
         }

         vTaskDelay(pdMS_TO_TICKS(2000)); // Delay for 2 seconds
     }
 }
