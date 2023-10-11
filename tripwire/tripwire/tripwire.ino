#include <ESP32HTTPUpdateServer.h>
#include <EspMQTTClient.h>

#define PIR_SENSOR_PIN   12

EspMQTTClient client(
  "Halloween",
  "veryspooky",
  "192.168.1.15",
  "",
  "",
  "TestClient",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

unsigned long lastTriggerTime;
int pirValue;

uint32_t chipId = 0;
bool connected = false;

void setup() {  
  pinMode(PIR_SENSOR_PIN, INPUT);
  
  Serial.begin(115200);
  while (!Serial);

  populateChipId();
  initMQTT();
}

void populateChipId() {
  for(int i=0; i<17; i=i+8) {
	  chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
	}
}

void initMQTT()
{
  // Optional functionalities of EspMQTTClient
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overridded with enableHTTPWebUpdater("user", "password").
  client.enableOTA(); // Enable OTA (Over The Air) updates. Password defaults to MQTTPassword. Port is the default OTA port. Can be overridden with enableOTA("password", port).
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true
}

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
  connected = true;
}

void loop() {
  if (connected) {
    pirValue = digitalRead(PIR_SENSOR_PIN);
    if (pirValue == HIGH) {
      Serial.println("Motion detected.");
      lastTriggerTime = millis();

      client.publish("tripwire/" + String(chipId), "trigger");
      delay(10000);
    }
  } else {
    Serial.println("Not connected yet...");
  }

  client.loop();
   
  delay(100);
}