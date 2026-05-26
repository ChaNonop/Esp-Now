#include <Arduino.h>
#include <ShareData.h>

uint16_t Pin[4] = {D1,D2,D3,D4};
uint8_t Pin_Analog = A0;

// 1. กำหนดโครงสร้างข้อมูล (Data Structure)
// ---------------------------------------------------------
// ข้อควรระวัง: ขนาดของ struct ห้ามเกิน 250 bytes
typedef struct struct_message {
    int id;
    float temp;
    bool state;
} struct_message;

void setup() {
  Serial.begin(112500);
  if(uint8_t i = 0; i < 3; i++;){
    pinMode(Pin[i],INPUT_PULLUP);
  } 
}

void loop() {
  }