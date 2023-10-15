#include <ConyHomeAutomation.h>
#include <ArduinoJson.h>
#include <Servo.h>

ConyHomeAutomation cony;

String token;
Servo servo1;

void setup() {
  Serial.begin(115200);
  //Serial.setDebugOutput(true);
  servo1.attach(D4);

  token = cony.startConnectionAndGetToken("HOME", "linuxuser");
  Serial.print(token);
}


void loop() {
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
    if (!locked || locked == "null"){
      Serial.println("Failed to get response from the server.");
      return;
    }
    
    // response from server is boolean string
    if (locked == "true"){
      servo1.write(180);
    }else{
      servo1.write(0);
    }
    
    Serial.println(locked);
  }
}