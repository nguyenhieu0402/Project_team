#include "function.h"
#include <EEPROM.h>

void setup() 
{
    Serial.begin(115200);
    EEPROM.begin(512); 
    init();
}

void loop() {
  // put your main code here, to run repeatedly:
}
