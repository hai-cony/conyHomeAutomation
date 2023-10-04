#include <ConyHomeAutomation.h>
#include <ArduinoJson.h>

ConyHomeAutomation cony;

String token;

void setup() {
  Serial.begin(115200);
  // Enter the WiFi SSID and Password as parameter.
  // store to JsonWebToken to token variable.  
  token = cony.startConnectionAndGetToken("HOME", "linuxuser");
}

void loop() {
  // return the object of json. Need to deserialize  
  String response = cony.fetch(token); 
  DynamicJsonDocument doc(2048);
  // Check value of json
  if(response.length() > 1){
    // Do whatever with response from web    
    Serial.println(response);
    StaticJsonDocument<512> jsonBuffer;
    deserializeJson(doc, response);

    // get data from buffer doc["name_id"];
    // the values ​​must be the same as those on the web name ID at controller's page
    String locked = doc["door_lock"];
    // Do whatever with response value       
    Serial.println(locked);
  }
}
