#include "function.h"
#include <EEPROM.h>

void setup() 
{
    Serial.begin(115200);
    EEPROM.begin(512); 
    setupProvisioning();
}

void loop() {
  // put your main code here, to run repeatedly:
}
