#include "ConyHomeAutomation.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <EEPROM.h>

// Need ArduinoJson library by Benoit Blanchon Version 6.21.3

#include <ArduinoJson.h>


String jsonWebToken;
unsigned long lastTime = 0;

/* 
  return the token from JsonWebToken. Function are not allowed
  use SSID and Password of WiFi user dynamically. Set SSID and Password 
  as parameter. ex: startConnectionAndGetToken("my_wifi_ssid", "my_wifi_password");
*/
String ConyHomeAutomation::startConnectionAndGetToken(String ssid, String password){
    if (WiFi.status() != WL_CONNECTED){
      Serial.printf("SSID : %s\n", ssid);
      Serial.printf("Password : %s\n", password);
      Serial.println("Connecting");
      WiFi.begin(ssid, password);
      while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("");
      Serial.print("Connected to WiFi network with IP Address: ");
      Serial.println(WiFi.localIP());

      for(uint8_t t = 3; t > 0; t--) {
          Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
          delay(1000);
      }
    }
    

    // Initialization, request the token
    WiFiClient client;
    HTTPClient http;
    // Your Domain name with URL path or IP address with path
    http.begin(client, "http://192.168.100.2:8080/api/esp8266/auth");
    http.addHeader("Content-Type", "application/json");

    // JSON Declare
    DynamicJsonDocument doc(2048);
    
    doc["chip_id"] = String(ESP.getChipId());
    doc["mac"] = WiFi.macAddress();
    doc["api_key"] = "$argon2id$v=19$m=65536,t=3,p=4$gX6VFv3fKlvvqkvHBbpxWw$XVqT4tzrOjNj/oSoHx2qb5IG6TYB8mNM/SYdWQXHS+g";

    // Serialize JSON document
    String json;
    serializeJson(doc, json);
      
    // Send HTTP POST request
    int httpResponseCode = http.POST(json);
    if (httpResponseCode == 200) {                                     
      String payload = http.getString();
      StaticJsonDocument<512> jsonBuffer;
      deserializeJson(doc, payload);
      String newToken = doc["token"];
      jsonWebToken = newToken;
    } 
    http.end();
    return jsonWebToken; // return the jwt token value only
}

String ConyHomeAutomation::fetch(String token){
  String obj;
  if(WiFi.status() == WL_CONNECTED && (millis() - lastTime) > 3000 ){   
    WiFiClient client;
    HTTPClient http;

    // Your Domain name with URL path or IP address with path
    http.begin(client, "http://192.168.100.2:8080/api/fetch");

    http.addHeader("Content-Type", "application/json");

    // JSON Declare
    DynamicJsonDocument doc(2048);
    doc["api_key"] = "$argon2id$v=19$m=65536,t=3,p=4$gX6VFv3fKlvvqkvHBbpxWw$XVqT4tzrOjNj/oSoHx2qb5IG6TYB8mNM/SYdWQXHS+g";;
    doc["token"] = token;
    doc["chip_id"] = String(ESP.getChipId());
    doc["mac"] = WiFi.macAddress();

        // Serialize JSON document
    String json;
    serializeJson(doc, json);
      
    // Send HTTP POST request
    int httpResponseCode = http.POST(json);
    
    if (httpResponseCode == 200) {
      String payload = http.getString();

      StaticJsonDocument<512> jsonBuffer;

      obj = payload;
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      Serial.println("Fetching error");
    }
    
    lastTime = millis();
  }else{
    if((millis() - lastTime) > 3000){
      lastTime = millis();
    }
    return {};
  }
  return obj;
}

/*
 This function allow to write a value to EEPROM memory
 first parameter is EEPROM address to store the value
 second parameter is value tp be stored.
*/
void ConyHomeAutomation::writeStringIntoEEPROM(int address, String str){
  byte len = str.length();
  EEPROM.write(address, len);

  for (int i = 0; i < len; i++){
    EEPROM.write(address + 1 + i, str[i]);
  }
  EEPROM.commit();  
}

/*
 This function allow to get a value from EEPROM memory
 first parameter is EEPROM address of previously stored value
*/
String ConyHomeAutomation::readStringFromEEPROM(int address){
  int len = EEPROM.read(address);
  char data[len + 1];

  for (int i = 0; i < len; i++){
  data[i] = EEPROM.read(address + 1 + i);
  }
  data[len] = '\0';

  return String(data);
}
