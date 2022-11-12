#include <Arduino.h>
#include <ArduinoJson.h>
#include <Servo.h> 

#define driverPin 2

Servo driver;

void setup() {
    Serial.begin(115200);
    driver.attach(driverPin);
    driver.write(0);
}

long int  value = 0;

void loop() {
 
if (Serial.available()) {
     value = Serial.parseInt();

     if(value == 0) {
        return;
     }
     
     Serial.printf("Sending %d\n", value);
     driver.write((int)value);
    }
}
