#include <Adafruit_Soundboard.h>
#include <ArduinoOTA.h>
#include <TelnetStream.h>

#define DOOR_RELAY1_PIN       12
#define DOOR_RELAY2_PIN       13
#define LIGHTS_RELAY_PIN      27
#define PIR_SENSOR_PIN        A0
#define BUTTON_PIN            4 // TODO: Update me

#define SFX_RESET_PIN 21

#define DOOR_MOVE_TIME_MS 16000

#define DEVICE_NAME "toilet"
#define WIFI_CONNECT_TIMEOUT_MS 15000

WiFiClient client;

Adafruit_Soundboard sfx = Adafruit_Soundboard(&Serial1, NULL, SFX_RESET_PIN);

unsigned long lastTriggerTime;

int pirValue;

void setup() {  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  pinMode(DOOR_RELAY1_PIN, OUTPUT);
  digitalWrite(DOOR_RELAY1_PIN, LOW);

  pinMode(DOOR_RELAY2_PIN, OUTPUT);
  digitalWrite(DOOR_RELAY2_PIN, LOW);

  pinMode(LIGHTS_RELAY_PIN, OUTPUT);
  digitalWrite(LIGHTS_RELAY_PIN, LOW);

  pinMode(PIR_SENSOR_PIN, INPUT);
  
  Serial.begin(115200);
  while (!Serial);

  initWiFi();
  
  resetDoorPosition();
  initSound();
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

void initSound() {
  Serial1.begin(9600);

  if (!sfx.reset()) {
    Serial.println("Waiting for SFX board");
    while (1);
  }
  Serial.println("SFX board found");

  for (int i = 0; i < 10; i++) {
    sfx.volUp();
  }

  sfx.playTrack("T10     WAV");
}

void waitForSound() {
  uint32_t current, total;
  while (sfx.trackTime(&current, &total)) {
    delay(1000);
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
  checkIfButtonPressed();
  
  pirValue = digitalRead(PIR_SENSOR_PIN);
  if (pirValue == HIGH) {
    Serial.println("Motion detected.");
    TelnetStream.println("Motion detected.");
    lastTriggerTime = millis();

    Serial.println("Playing sounds");
    sfx.playTrack("T08     WAV");
    delay(1000);
    openDoor();
    delay(500);
    sfx.playTrack("T09     WAV");
    lightsOn();
    delay(2000);
    sfx.playTrack("T10     WAV");
    delay(3000);
    stopDoor();
    delay(1000);
    sfx.playTrack("T01     WAV");


    delay(7000);

    lightsOff();
    closeDoor();
    delay(DOOR_MOVE_TIME_MS);
    stopDoor();

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

void checkIfButtonPressed() {
  int buttonVal = digitalRead(BUTTON_PIN);

  if (buttonVal == LOW) {
    Serial.println("Button pressed! Opening door");
    openDoor();
    delay(DOOR_MOVE_TIME_MS);
    stopDoor();

    do {
      buttonVal = digitalRead(BUTTON_PIN);
      Serial.println("Waiting for button to be pressed again");
      delay(1000);
    }
    while (buttonVal == HIGH);

    Serial.println("Closing door");
    closeDoor();
    delay(DOOR_MOVE_TIME_MS);
    stopDoor();
  }
}