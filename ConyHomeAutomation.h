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

// int cobasatu(int a, int b);
// int cobadulu(int a, int b);

// void cobadua(String a, String b);

#endif