#include <Arduino.h>
#include <ArduinoJson.h>
#include <Servo.h> 


#define driverPin 2

#define MIN_PULSE_LENGTH 1000 // Minimum pulse length in µs
#define MIN_PULSE_LENGTH_START 1080 
#define MAX_PULSE_LENGTH 2000 // Maximum pulse length in µs

Servo driver;

void setup() {

    Serial.begin(115200);

    driver.attach(driverPin, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
    driver.writeMicroseconds(MAX_PULSE_LENGTH);
}

int data;

void init2()
{
    for (int i = MIN_PULSE_LENGTH; i <= MIN_PULSE_LENGTH_START; i += 5) {
        Serial.print("Pulse length = ");
        Serial.println(i);
        
        driver.writeMicroseconds(i);         
        delay(100);
    }
}

void toHigh()
{  
  int max = 1200;
  int min = MIN_PULSE_LENGTH_START;
        
    for (int i = min; i <= max; i += 5) {
        Serial.print("Pulse length = ");
        Serial.println(i);
        
        driver.writeMicroseconds(i);         
        delay(50);
    }
}

void toLow()
{
  int max = 1200;
  int min = MIN_PULSE_LENGTH_START;
  
      for (int i = max; i >= min; i -= 5) {
        Serial.print("Pulse length = ");
        Serial.println(i);
        
        driver.writeMicroseconds(i);         
        delay(50);
    }
}

void loop() {
  if (Serial.available()) {
        data = Serial.parseInt();

        if(data == 0) {
          return;
        }
        
        driver.writeMicroseconds(data);
        Serial.println(data);
        return;

        switch (data) {
            case 1 : Serial.println("Sending minimum throttle");
                      driver.writeMicroseconds(MIN_PULSE_LENGTH);
            break;

            case 2 : Serial.println("Sending maximum throttle");
                      driver.writeMicroseconds(MAX_PULSE_LENGTH);
            break;
            
            case 3 : Serial.println("toLow()");
                      toLow();
            break;
                      
            case 4 : Serial.println("toHigh()");
                      toHigh();                      
            break;
                        
            case 5 : Serial.println("init()");
                      init2();                                
            break;
        }
    }
}
