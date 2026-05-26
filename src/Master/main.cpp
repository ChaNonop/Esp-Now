#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include "ShareData.h"

// ---------------------------------------------------------
// Master Node Class (OOP) + Non-blocking Task

class MasterNode {
private:
    static MasterNode* instance;
    sensor_data_t latestData;
    bool newDataReceived = false;

    static void onDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
        if (instance != nullptr) {
            instance->handleReceive(mac, incomingData, len);
        }
    }

    void handleReceive(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
        memcpy(&latestData, incomingData, sizeof(latestData));
        newDataReceived = true; // Set Flag ว่ามีข้อมูลใหม่

        Serial.print("\n[RX] Msg from MAC: ");
        for (int i = 0; i < 6; i++) {
            Serial.printf("%02X", mac[i]);
            if (i < 5) Serial.print(":");
        }
        Serial.printf(" | Node ID: %d\n", latestData.node_id);
    }

public:
    MasterNode() {
        instance = this;
    }

    void begin() {
        WiFi.mode(WIFI_STA);
        Serial.print("[MASTER] MAC Address: ");
        Serial.println(WiFi.macAddress());

        if (esp_now_init() != 0) {
            Serial.println("[ERROR] ESP-NOW Init Failed");
            return;
        }
        esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
        esp_now_register_recv_cb(onDataRecv);
    }

    // ฟังก์ชันสำหรับดึงข้อมูล (Non-blocking check)
    bool getNewData(sensor_data_t &dataOut) {
        if (newDataReceived) {
            dataOut = latestData;
            newDataReceived = false; // Reset flag
            return true;
        }
        return false;
    }
};

MasterNode* MasterNode::instance = nullptr;

MasterNode myMaster;

// ตัวแปรสำหรับ Non-blocking Task (Heartbeat LED / System Check)
unsigned long previousMillis = 0;
const long interval = 1000; // ทำงานทุกๆ 1 วินาที

void setup() {
    Serial.begin(115200);
    while (!Serial) {}
    Serial.println("\n--- Master Node Waking Up ---");

    pinMode(LED_BUILTIN, OUTPUT);
    myMaster.begin();
}

void loop() {
    sensor_data_t rxData;
    if (myMaster.getNewData(rxData)) {
        Serial.println("--- New Data Processed ---");
        Serial.printf("  > Temp: %.2f C\n", rxData.temperature);
        Serial.printf("  > Hum : %.2f %%\n", rxData.humidity);
        Serial.printf("  > Analog : %d\n", rxData.analog_val);
    }

    // ---------------------------------------------------------
    // Task 2: Background Task อื่นๆ (เช่น Blink LED บอกสถานะการทำงาน)
    // ---------------------------------------------------------
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        
        // สลับสถานะ LED (Active LOW บนบอร์ด ESP8266 ส่วนใหญ่)
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        
        // ตรงนี้สามารถเพิ่มโค้ด Non-blocking อื่นๆ ได้ 
    }
}