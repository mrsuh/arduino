#include <OneWire.h>
#include <DallasTemperature.h>

#define PID_TEMPERATURE_SENSOR 33
#define PID_VIN 12

OneWire oneWire(PID_TEMPERATURE_SENSOR);
DallasTemperature sensor(&oneWire);

float temperature = 0.0;

void setup() {
  Serial.begin(115200);
  sensor.begin();

  pinMode(PID_VIN, OUTPUT);
  digitalWrite(PID_VIN, HIGH);

  sensor.setResolution(12);
  Serial.print("Initialized\n");
}

void loop() {
  sensor.requestTemperatures();
    temperature = sensor.getTempCByIndex(0);
    Serial.print("got temperature: ");
    Serial.println(temperature);
    delay(1000);
}
