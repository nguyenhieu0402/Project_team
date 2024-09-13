#include "function.h"
#include "config.h"
#include "Arduino.h"
#include "WiFiProv.h"


void setupProvisioning()  // Renamed from init() to avoid conflicts
{
    wifiProvConfig();
}

void wifiProvConfig()
{
    WiFi.onEvent(SysProvEvent);
    Serial.println("Begin Provisioning using BLE");
    // Sample UUID that user can pass during provisioning using BLE
    uint8_t uuid[16] = {0xb4, 0xdf, 0x5a, 0x1c, 0x3f, 0x6b, 0xf4, 0xbf,
                        0xea, 0x4a, 0x82, 0x03, 0x04, 0x90, 0x1a, 0x02 };
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name, service_key, uuid, reset_provisioned);
    log_d("BLE QR code generated");
    WiFiProv.printQR(service_name, pop, "ble");
}

void saveWiFiCredentials(const char* ssid, const char* password) 
{
    int ssidCount = EEPROM.read(0);
    char ssidList[MAX_SSID_COUNT][SSID_LENGTH];
    char passwordList[MAX_SSID_COUNT][PASSWORD_LENGTH];

    for (int i = 0; i < ssidCount; i++) {
        for (int j = 0; j < SSID_LENGTH; j++) {
            ssidList[i][j] = EEPROM.read(1 + i * SSID_LENGTH + j);
        }
        for (int j = 0; j < PASSWORD_LENGTH; j++) {
            passwordList[i][j] = EEPROM.read(1 + MAX_SSID_COUNT * SSID_LENGTH + i * PASSWORD_LENGTH + j);
        }
    }

    bool ssidExists = false;
    for (int i = 0; i < ssidCount; i++) {
        if (strcmp(ssidList[i], ssid) == 0) {
            ssidExists = true;
            strcpy(passwordList[i], password);
            break;
        }
    }

    if (!ssidExists) {
        if (ssidCount < MAX_SSID_COUNT) {
            strcpy(ssidList[ssidCount], ssid);
            strcpy(passwordList[ssidCount], password);
            ssidCount++;
        } else {
            for (int i = 1; i < MAX_SSID_COUNT; i++) {
                strcpy(ssidList[i - 1], ssidList[i]);
                strcpy(passwordList[i - 1], passwordList[i]);
            }
            strcpy(ssidList[MAX_SSID_COUNT - 1], ssid);
            strcpy(passwordList[MAX_SSID_COUNT - 1], password);
        }

        EEPROM.write(0, ssidCount);
        for (int i = 0; i < ssidCount; i++) {
            for (int j = 0; j < SSID_LENGTH; j++) {
                EEPROM.write(1 + i * SSID_LENGTH + j, ssidList[i][j]);
            }
            for (int j = 0; j < PASSWORD_LENGTH; j++) {
                EEPROM.write(1 + MAX_SSID_COUNT * SSID_LENGTH + i * PASSWORD_LENGTH + j, passwordList[i][j]);
            }
        }

        EEPROM.commit();
    }
}

// Đọc và hiển thị thông tin Wi-Fi đã lưu trong EEPROM
void printSavedWiFiCredentials() {
    int ssidCount = EEPROM.read(0);
    Serial.print("Number of saved Wi-Fi credentials: ");
    Serial.println(ssidCount);

    if (ssidCount > 0) {
        char ssid[SSID_LENGTH];
        char password[PASSWORD_LENGTH];

        for (int i = 0; i < ssidCount; i++) {
            for (int j = 0; j < SSID_LENGTH; j++) {
                ssid[j] = EEPROM.read(1 + i * SSID_LENGTH + j);
            }
            ssid[SSID_LENGTH - 1] = '\0';  // Đảm bảo SSID kết thúc bằng ký tự null

            for (int j = 0; j < PASSWORD_LENGTH; j++) {
                password[j] = EEPROM.read(1 + MAX_SSID_COUNT * SSID_LENGTH + i * PASSWORD_LENGTH + j);
            }
            password[PASSWORD_LENGTH - 1] = '\0';  // Đảm bảo mật khẩu kết thúc bằng ký tự null

            Serial.print("SSID ");
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.println(ssid);

            Serial.print("Password ");
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.println(password);
        }
    } else {
        Serial.println("No Wi-Fi credentials saved.");
    }
}

// Kết nối với Wi-Fi đã lưu trong EEPROM
void connectToSavedWiFi() {
    char ssid[SSID_LENGTH];
    char password[PASSWORD_LENGTH];
    int ssidCount = EEPROM.read(0);

    if (ssidCount > 0) {
        for (int i = 0; i < SSID_LENGTH; i++) {
            ssid[i] = EEPROM.read(1 + (ssidCount - 1) * SSID_LENGTH + i);
        }
        ssid[SSID_LENGTH - 1] = '\0';

        for (int i = 0; i < PASSWORD_LENGTH; i++) {
            password[i] = EEPROM.read(1 + MAX_SSID_COUNT * SSID_LENGTH + (ssidCount - 1) * PASSWORD_LENGTH + i);
        }
        password[PASSWORD_LENGTH - 1] = '\0';

        Serial.print("Connecting to saved WiFi: ");
        Serial.println(ssid);
        WiFi.begin(ssid, password);

        int retry_count = 0;
        while (WiFi.status() != WL_CONNECTED && retry_count < 20) {
            delay(500);
            Serial.print(".");
            retry_count++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nWiFi Connected!");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
        } else {
            Serial.println("\nFailed to connect to WiFi. Starting provisioning...");
            startProvisioning();
        }
    } else {
        Serial.println("No saved WiFi credentials found. Starting provisioning...");
        startProvisioning();
    }
}


void SysProvEvent(arduino_event_t *sys_event) 
{
    switch (sys_event->event_id) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        Serial.print("\nConnected IP address : ");
        Serial.println(IPAddress(sys_event->event_info.got_ip.ip_info.ip.addr));
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        Serial.println("\nDisconnected. Connecting to the AP again... ");
        break;
    case ARDUINO_EVENT_PROV_START:
        Serial.println("\nProvisioning started\nProvide credentials of your access point using smartphone app");
        break;
    case ARDUINO_EVENT_PROV_CRED_RECV: {
        Serial.println("\nReceived Wi-Fi credentials");
        Serial.print("\tSSID : ");
        Serial.println((const char *) sys_event->event_info.prov_cred_recv.ssid);
        Serial.print("\tPassword : ");
        Serial.println((char const *) sys_event->event_info.prov_cred_recv.password);
        saveWiFiCredentials((const char *) sys_event->event_info.prov_cred_recv.ssid, (const char *) sys_event->event_info.prov_cred_recv.password);
        break;
    }
    case ARDUINO_EVENT_PROV_CRED_FAIL: {
        Serial.println("\nProvisioning failed!\nPlease reset to factory and retry provisioning\n");
        if (sys_event->event_info.prov_fail_reason == WIFI_PROV_STA_AUTH_ERROR)
            Serial.println("\nWi-Fi AP password incorrect");
        else
            Serial.println("\nWi-Fi AP not found");
        break;
    }
    case ARDUINO_EVENT_PROV_CRED_SUCCESS:
        Serial.println("\nProvisioning Successful");
        break;
    case ARDUINO_EVENT_PROV_END:
        Serial.println("\nProvisioning Ends");
        break;
    default:
        break;
    }
}
