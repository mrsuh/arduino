#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include <ArduinoJson.h>

const char* ssid = "esp8266-udp";
const char* password = "password";

WiFiUDP Udp;
unsigned int localUdpPort = 4210;

void setup()
{
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);

  Serial.begin(115200);
  Serial.println();
  Serial.println("Initializing");

  IPAddress ip(192, 168, 4, 1);
  IPAddress gateway(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.setOutputPower(20.5);
  WiFi.setPhyMode(WIFI_PHY_MODE_11B);

  WiFi.softAPConfig(ip, gateway, subnet);
  WiFi.softAP(ssid, password, 12, true);
  Serial.println("WIFI AP Initialized!");

  Udp.begin(localUdpPort);
  Serial.printf("UDP Initialized!");

  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);
}

unsigned long ledEnabledLastTime = 0;
void loop()
{
  digitalWrite(2, HIGH);
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    Udp.flush();
    ledEnabledLastTime = millis();
  }

  if(millis() - ledEnabledLastTime < 200) {
    digitalWrite(2, LOW);
  } else {
    digitalWrite(2, HIGH);
  }
}
