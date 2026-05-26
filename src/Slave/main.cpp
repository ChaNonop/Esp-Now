#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <DHT.h>
#include "ShareData.h"

#define NODE_ID 90
#define DHTTYPE 11
#define DHTPIN D2
#define SLEEP_TIME_SEC 10 // 10 วินาที

uint16_t masterMac[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

DHT dht(DHTPIN, DHTTYPE);

// 1. กำหนดโครงสร้างข้อมูล (Data Structure)
sensor_data_t sensor_data;
Config_Pin_t Config_pin = { {D1, D2, D3, D4}, A0 };;

// state tracking Value
bool isMessageSent = false;

void callback(uint8_t *mac_addr, uint8_t *data, uint8_t len){
    Serial.print("[Slave] Sens status : ");
    Serial.println(data == 0 ? "Success" : "fail");
    isMessageSent = true;  // อัปเดต Flag เพื่อให้รู้ว่าส่งจบแล้ว 
} 

void setup() {
    Serial.begin(115200);
    Serial.println("\n ---Wake Up---");

    dht.begin();
    sensor_data.node_id = NODE_ID;
    sensor_data.temperature = dht.readTemperature();
    sensor_data.humidity = dht.readHumidity();
    sensor_data.analog_val = analogRead(Config_pin.Pin_Analog);

    // ตรวจสอบ Error ของเซ็นเซอร์
    if (isnan(sensor_data.temperature) || isnan(sensor_data.humidity)) {
        Serial.println("[ERROR] Failed to read from DHT sensor!");
        sensor_data.temperature = 0.0;
        sensor_data.humidity = 0.0;
    }
    Serial.printf("Temp: %.2f C, Hum: %.2f %%, Analog: %d\n", sensor_data.temperature, sensor_data.humidity, sensor_data.analog_val);

    WiFi.mode(WIFI_STA);
    if(esp_now_init() != 0){
        Serial.println("[ERROR] Error initializing ESP-NOW");
        ESP.deepSleep(SLEEP_TIME_SEC * 1000000);
        return;
    }
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    esp_now_register_send_cb(callback);
    esp_now_add_peer((uint8_t *)masterMac, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
    
    // ส่งข้อมูล
    esp_now_send((uint8_t *)masterMac, (uint8_t *) &sensor_data, sizeof(sensor_data));
}

void loop() {
    if(isMessageSent){
        Serial.println("[SLAVE] Going to Deep Sleep...");
        ESP.deepSleep(SLEEP_TIME_SEC * 1000000);
    }  
}