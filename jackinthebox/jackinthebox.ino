#include <ArduinoOTA.h>
#include <TelnetStream.h>

#define DOOR_RELAY1_PIN       12
#define DOOR_RELAY2_PIN       13
#define LIGHTS_RELAY_PIN      27
#define PIR_SENSOR_PIN        A0
#define FOG_RELAY_PIN         14

#define DOOR_MOVE_TIME_MS 16000

#define DEVICE_NAME "jackinthebox"
#define WIFI_CONNECT_TIMEOUT_MS 15000

WiFiClient client;

unsigned long lastTriggerTime;

int pirValue;

void setup() {  
  pinMode(DOOR_RELAY1_PIN, OUTPUT);
  digitalWrite(DOOR_RELAY1_PIN, LOW);

  pinMode(DOOR_RELAY2_PIN, OUTPUT);
  digitalWrite(DOOR_RELAY2_PIN, LOW);

  pinMode(LIGHTS_RELAY_PIN, OUTPUT);
  digitalWrite(LIGHTS_RELAY_PIN, LOW);

  pinMode(FOG_RELAY_PIN, OUTPUT);
  digitalWrite(FOG_RELAY_PIN, LOW);

  pinMode(PIR_SENSOR_PIN, INPUT);
  
  Serial.begin(115200);
  while (!Serial);

  initWiFi();
  
  resetDoorPosition();
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin("Halloween", "veryspooky");
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFI Connection Failed");
    delay(1000);

    if (millis() > WIFI_CONNECT_TIMEOUT_MS) {
      Serial.println("Giving up. Continuing without WiFi");
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    ArduinoOTA.setHostname(DEVICE_NAME);
    ArduinoOTA.begin();
  
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    TelnetStream.begin();
  }
}

void resetDoorPosition() {
  Serial.println("Resetting door position");  
  closeDoor();
  delay(DOOR_MOVE_TIME_MS);
  stopDoor();
  Serial.println("Door is closed");
}

void loop() {
  ArduinoOTA.handle();
  
  pirValue = digitalRead(PIR_SENSOR_PIN);
  if (pirValue == HIGH) {
    Serial.println("Motion detected.");
    TelnetStream.println("Motion detected.");
    lastTriggerTime = millis();

    openDoor();
    lightsOn();
    fogOn();
    delay(DOOR_MOVE_TIME_MS);
    stopDoor();
    fogOff();
    delay(5000);
    lightsOff();
    closeDoor();

    delay(10000);
  }
   
  delay(100);
}

void openDoor() {
  Serial.println("Opening door");
  digitalWrite(DOOR_RELAY1_PIN, HIGH);  
  digitalWrite(DOOR_RELAY2_PIN, LOW);
}

void closeDoor() {
  Serial.println("Closing door");
  digitalWrite(DOOR_RELAY1_PIN, LOW);  
  digitalWrite(DOOR_RELAY2_PIN, HIGH);
}

void stopDoor() {
  Serial.println("Stopping door");
  digitalWrite(DOOR_RELAY1_PIN, LOW);  
  digitalWrite(DOOR_RELAY2_PIN, LOW);
}

void lightsOn() {
  Serial.println("Lights on");
  digitalWrite(LIGHTS_RELAY_PIN, HIGH);
}

void lightsOff() {
  Serial.println("Lights off");
  digitalWrite(LIGHTS_RELAY_PIN, LOW);
}

void fogOn() {
  Serial.println("Fog on");
  digitalWrite(FOG_RELAY_PIN, HIGH);
}

void fogOff() {
  Serial.println("Fog off");
  digitalWrite(FOG_RELAY_PIN, LOW);
}