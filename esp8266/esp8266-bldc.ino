#include <Arduino.h>
#include <Servo.h> 

#define driverPin 5

#define MIN_PULSE_LENGTH 1000 // Minimum pulse length in µs 
#define MAX_PULSE_LENGTH 2000 // Maximum pulse length in µs

Servo driver;

void setup() {
    Serial.begin(115200);
    driver.attach(driverPin, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
    driver.writeMicroseconds(MIN_PULSE_LENGTH);     
    Serial.printf("Initialized\n");    
}

int currentDriverValue = MIN_PULSE_LENGTH;

void runDriver(int value)
{
  if(value > MAX_PULSE_LENGTH) {
    return;
  }

  if(value < MIN_PULSE_LENGTH) {
    return;
  }
  Serial.printf("driver.writeMicroseconds %d\n", value);
  driver.writeMicroseconds(value);  
  delay(200);
}

long int  value = 0;

void loop() {
 
if (Serial.available()) {
     value = Serial.parseInt();

     if(value == 0) {
      return;
     }
     
     Serial.printf("Sending %d\n", value);
     runDriver((int)value);
    }
}
