#ifndef CONYHOMEAUTOMATION_H
#define CONYHOMEAUTOMATION_H

#include <Arduino.h>

class ConyHomeAutomation {
  public: 
    String startConnectionAndGetToken(String ssid, String password);
    String fetch(String token);
    void writeStringIntoEEPROM(int address, String str);
    String readStringFromEEPROM(int address);
};

#endif