#include "ConyHomeAutomation.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
// #include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiClient.h>
#include <EEPROM.h>

// Need ArduinoJson library by Benoit Blanchon Version 6.21.3

#include <ArduinoJson.h>

/*
  Writen by @hai.cony (instagram). 15 Oct 2023
  Support me on https://ko-fi.com/haicony or https://paypal.me/haicony
  This library created for hobbies and fun, if you want to contribute
  you can find code at github https://github.com/hai-cony/cony-home-automation-web
*/

String jsonWebToken;

/*
  it will be reset to 0 after 50 days. If you get an error after 50 days,
  try to restart microcontroller.
*/
unsigned long lastTime = 0;

/*
  fingerprint get from web browser but changed to memory address.
  fingerprint needed for https request.
*/
// const uint8_t fingerprint[20] = {0xca, 0xbd, 0x2a, 0x79, 0x1a, 0x07, 0x6a, 0x31, 0xf2, 0x1d, 0x25, 0x36, 0x35, 0xcb, 0x03, 0x9d, 0x43, 0x29, 0xa5, 0xe8};

/*
  return the token from JsonWebToken. Function are not allowed
  use SSID and Password of WiFi user dynamically. Set SSID and Password
  as parameter. ex: startConnectionAndGetToken("my_wifi_ssid", "my_wifi_password");
*/
String ConyHomeAutomation::startConnectionAndGetToken(String ssid, String password)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.printf("SSID : %s\n", ssid);
    Serial.printf("Password : %s\n", password);
    Serial.println("Connecting");

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.println(WiFi.macAddress());
    Serial.println(String(ESP.getChipId()));
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());

    for (uint8_t t = 3; t > 0; t--)
    {
      Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
      delay(1000);
    }
  }

  // std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
  std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

  // client->setFingerprint(fingerprint);
  client->setInsecure();

  HTTPClient https;

  // Your Domain name with URL path or IP address with path
  https.begin(*client, "https://cony-home-automation-web.vercel.app/api/esp8266/auth");
  https.addHeader("Content-Type", "application/json");

  // JSON Declare
  DynamicJsonDocument doc(2048);

  doc["chip_id"] = String(ESP.getChipId());
  doc["mac"] = WiFi.macAddress();
  doc["api_key"] = "$argon2id$v=19$m=65536,t=3,p=4$gX6VFv3fKlvvqkvHBbpxWw$XVqT4tzrOjNj/oSoHx2qb5IG6TYB8mNM/SYdWQXHS+g";

  // Serialize JSON document
  String json;
  serializeJson(doc, json);

  // Send HTTP POST request
  int httpResponseCode = https.POST(json);
  if (httpResponseCode == 200)
  {
    String payload = https.getString();
    StaticJsonDocument<512> jsonBuffer;
    deserializeJson(doc, payload);
    String newToken = doc["token"];
    jsonWebToken = newToken;
  }
  https.end();
  return jsonWebToken; // return the jwt token value only
}

/*
  This function return JSON format but string type.
  JSON transformed to string and store to obj variable.
*/
String ConyHomeAutomation::fetch(String token)
{
  String obj;
  if (WiFi.status() == WL_CONNECTED && (millis() - lastTime) > 5000)
  {
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

    // client->setFingerprint(fingerprint);
    client->setInsecure();

    HTTPClient https;

    // Your Domain name with URL path or IP address with path
    https.begin(*client, "https://cony-home-automation-web.vercel.app/api/fetch");

    https.addHeader("Content-Type", "application/json");

    // JSON Declare
    DynamicJsonDocument doc(2048);
    doc["api_key"] = "$argon2id$v=19$m=65536,t=3,p=4$gX6VFv3fKlvvqkvHBbpxWw$XVqT4tzrOjNj/oSoHx2qb5IG6TYB8mNM/SYdWQXHS+g";
    // token get from parameter
    doc["token"] = token;
    doc["chip_id"] = String(ESP.getChipId());
    doc["mac"] = WiFi.macAddress();

    // Serialize JSON document
    String json;
    serializeJson(doc, json);

    // Send HTTP POST request
    int httpResponseCode = https.POST(json);

    if (httpResponseCode == 200)
    {
      String payload = https.getString();
      obj = payload;
    }
    else
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      Serial.println("Fetching error");
    }

    lastTime = millis();
  }
  else
  {
    if ((millis() - lastTime) > 5000)
    {
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
void ConyHomeAutomation::writeStringIntoEEPROM(int address, String str)
{
  byte len = str.length();
  EEPROM.write(address, len);

  for (int i = 0; i < len; i++)
  {
    EEPROM.write(address + 1 + i, str[i]);
  }
  EEPROM.commit();
}

/*
 This function allow to get a value from EEPROM memory
 first parameter is EEPROM address of previously stored value
*/
String ConyHomeAutomation::readStringFromEEPROM(int address)
{
  int len = EEPROM.read(address);
  char data[len + 1];

  for (int i = 0; i < len; i++)
  {
    data[i] = EEPROM.read(address + 1 + i);
  }
  data[len] = '\0';

  return String(data);
}
