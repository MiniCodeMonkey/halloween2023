#include <Adafruit_Soundboard.h>

#define DOOR_RELAY1_PIN       12
#define DOOR_RELAY2_PIN       13
#define LIGHTS_RELAY_PIN      27
#define PIR_SENSOR_PIN        A0
#define BUTTON_PIN            4 // TODO: Update me

#define SFX_RESET_PIN 5

#define DOOR_MOVE_TIME_MS 18000

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
  
  resetDoorPosition();
}

void initSound() {
  Serial1.begin(9600);

  if (!sfx.reset()) {
    Serial.println("Waiting for SFX board");
    while (1);
  }
  Serial.println("SFX board found");
}

void resetDoorPosition() {
  Serial.println("Resetting door position");  
  closeDoor();
  delay(DOOR_MOVE_TIME_MS);
  stopDoor();
  Serial.println("Door is closed");
}

void loop() {
  checkIfButtonPressed();
  
  pirValue = digitalRead(PIR_SENSOR_PIN);
  if (pirValue == HIGH) {
    Serial.println("Motion detected.");
    lastTriggerTime = millis();

    Serial.println("Playing sounds");
    sfx.playTrack(1);
    openDoor();
    delay(2000);
    lightsOn();
    sfx.playTrack(2);
    delay(5000);

    lightsOff();
    closeDoor();
    delay(DOOR_MOVE_TIME_MS);
    stopDoor();
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