#include <OneWire.h>
#include <DallasTemperature.h>


#define PID_TEMPERATURE_SENSOR 11
#define PID_RELAY 12
#define PID_LED 13
#define PID_POTENTIOMETR A0
 
OneWire oneWire(PID_TEMPERATURE_SENSOR);
DallasTemperature sensor(&oneWire);

void setup() {
  pinMode(PID_RELAY, OUTPUT);
  pinMode(PID_LED, OUTPUT);
  pinMode(PID_POTENTIOMETR, INPUT);
  digitalWrite(PID_LED, LOW);
  digitalWrite(PID_RELAY, HIGH);
  Serial.begin(9600);
  sensor.begin();
  
  // устанавливаем разрешение датчика от 9 до 12 бит
  sensor.setResolution(12);
  Serial.print("Initialized\n");
}

float targetTemperature;
float lowTemperature;
float realTemperature;

void loop() {
  sensor.requestTemperatures();
  realTemperature = sensor.getTempCByIndex(0); 

  targetTemperature = map(analogRead(PID_POTENTIOMETR), 0, 1024, 0, 11);
  lowTemperature = targetTemperature - 1.0;
    
  Serial.print("target: ");
  Serial.print(targetTemperature);
  Serial.print(" low target: ");
  Serial.print(lowTemperature);
  Serial.print(" real: ");
  Serial.print(realTemperature);
  Serial.print("\n");

  if(realTemperature < lowTemperature) {    
    digitalWrite(PID_LED, HIGH);
    digitalWrite(PID_RELAY, LOW);
  }

  if(realTemperature > targetTemperature) {
    digitalWrite(PID_LED, LOW);    
    digitalWrite(PID_RELAY, HIGH);      
  }
   
  delay(250);
  return;
}
