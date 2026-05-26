#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <Arduino.h>

// โครงสร้างข้อมูลนี้จะถูกเรียกใช้ทั้งฝั่ง Master และ Slave
// ขนาดไม่เกิน 250 bytes

typedef struct sensor_data_t {
    int node_id;
    float temperature;
    float humidity;
    int analog_val;
} sensor_data_t;

typedef struct Config_Pin_t{
    uint16_t Pin[4];
    uint8_t Pin_Analog;
} Config_Pin_t;

#endif