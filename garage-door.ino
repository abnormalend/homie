#include <Homie.h>

#define FW_NAME "garage-door"
#define FW_VERSION "0.1.1"

#define PIN_RELAY         D1

#define OPENER_EVENT_MS   1500
#define OPEN_TIME         15000
#define DEBOUNCER_MS      250
#define PUB_INTERVAL      60

unsigned long openerEvent = 0;
unsigned long doorEvent = 0;
unsigned long pubEvent = 0;

int lastSensorValue = -1;

HomieNode openerNode("opener", "relay");
HomieNode doorNode("door","sensor");

bool openerHandler(String value) {
  if (value == "true") {
    digitalWrite(PIN_RELAY, HIGH);
    Homie.setNodeProperty(openerNode, "on", "true"); // Update the state of the light
    Serial.println("button is pressed");
    openerEvent = millis();
/*  } else if (value == "false") {
    digitalWrite(PIN_RELAY, LOW);
    Homie.setNodeProperty(lightNode, "on", "false");
    Serial.println("Light is off"); */
  } else {
    return false;
  }

  return true;
}

void loopHandler() {

}

bool resetFunction () {
  return true; // If true is returned, the device will reset, if false, it won't
}

void setup() {
//  Homie.setResetFunction(resetFunction); 
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, LOW);

  Homie.setFirmware(FW_NAME, FW_VERSION);
  openerNode.subscribe("on", openerHandler);
  Homie.registerNode(openerNode);
  Homie.registerNode(doorNode);
  pubEvent = millis();
  Homie.setup();
}

void loop() {
  if (openerEvent && (millis() - openerEvent >= OPENER_EVENT_MS)) {
    digitalWrite(PIN_RELAY, LOW);
    Serial.println("releasing button");
    openerEvent = 0;
  }  

    int sensorValue = 0; //debouncer.read();

  if (sensorValue != lastSensorValue) {
     Serial.print("Door is now: ");
     Serial.println(sensorValue ? "open" : "close");

     Homie.setNodeProperty(doorNode, "open", sensorValue ? "true" : "false");
     lastSensorValue = sensorValue;
  } 

  if (pubEvent + (PUB_INTERVAL * 1000) <= millis()) {
    Serial.print("Door is now: ");
     Serial.println(lastSensorValue ? "open" : "close");

     Homie.setNodeProperty(doorNode, "open", lastSensorValue ? "true" : "false");
    pubEvent = millis();
  }
  
  Homie.loop();
}
