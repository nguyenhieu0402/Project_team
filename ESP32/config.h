#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "WiFiProv.h"
#include "WiFi.h"
#include <EEPROM.h>

#define SSID_LENGTH 32
#define PASSWORD_LENGTH 32
#define MAX_SSID_COUNT 5

const char* pop = "abcd1234";
const char* service_name = "PROV_123";
const char* service_key = NULL;
bool reset_provisioned = true;

#endif 