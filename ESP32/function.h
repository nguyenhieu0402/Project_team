#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include <Arduino.h>
#include <WiFiProv.h>  // Ensure this is included to get the arduino_event_t type

void setupProvisioning();
void wifiProvConfig();

// Lưu tên Wi-Fi vào flash
void saveWiFiCredentials(const char* ssid, const char* password);

// Đọc Wi-Fi đã lưu trong EEPROM
void printSavedWiFiCredentials();

// Kết nối với Wi-Fi đã lưu trong EEPROM
void connectToSavedWiFi();

void startProvisioning();
void SysProvEvent(arduino_event_t *sys_event);

#endif