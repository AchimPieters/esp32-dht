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

 #include "dht.h"
 #include "freertos/FreeRTOS.h"
 #include "freertos/task.h"
 #include "esp_log.h"

 #define DHT_GPIO CONFIG_DHT_GPIO
 #define DHT_TYPE CONFIG_DHT_TYPE

 void dht_task(void *pvParameter) {
     float temperature = 0.0f, humidity = 0.0f;

     // Initialize the DHT sensor
     if (dht_init(DHT_GPIO, DHT_TYPE) == ESP_OK) {
         ESP_LOGI(INFORMATION, "DHT sensor initialized successfully.");
     } else {
         ESP_LOGE(ERROR, "Failed to initialize DHT sensor.");
         vTaskDelete(NULL);
         return;
     }

     while (1) {
         // Read temperature and humidity
         if (dht_read(&temperature, &humidity) == ESP_OK) {
             ESP_LOGI(INFORMATION, "Temperature: %.1f°C, Humidity: %.1f%%", temperature, humidity);
         } else {
             ESP_LOGE(ERROR, "Failed to read data from DHT sensor.");
         }
         vTaskDelay(pdMS_TO_TICKS(2000));  // Delay for 2 seconds
     }
 }

 void app_main(void) {
     xTaskCreate(dht_task, "DHT_Task", 2048, NULL, 5, NULL);
 }
