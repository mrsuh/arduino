#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiAP.h>

const char* ssid = "esp32-udp";
const char* password = "password";

WiFiUDP Udp;
unsigned int udpPort = 4210;
uint8_t requestPacket[10] = "1";
IPAddress serverIp(192, 168, 4, 1);
IPAddress clientIp(192, 168, 4, 2);
IPAddress serverGateway(192, 168, 4, 1);
IPAddress serverSubnet(255, 255, 255, 0);

#define INIT_LED 19
#define PING_LED 18

void setup()
{
  pinMode(INIT_LED, OUTPUT);
  digitalWrite(INIT_LED, LOW);
  pinMode(PING_LED, OUTPUT);
  digitalWrite(PING_LED, LOW);

  Serial.begin(115200);
  Serial.println();
  Serial.println("Initializing");

  if(!WiFi.mode(WIFI_AP)) {
    Serial.println("WiFi.mode(WIFI_AP) failed");
  }

  if(!WiFi.softAPConfig(serverIp, serverGateway, serverSubnet)) {
    Serial.println("WiFi.softAPConfig failed");
  }

  if(!WiFi.softAP(ssid, password)) {
    Serial.println("WiFi.softAP(ssid, password, 12) failed");
  }

  Serial.println("WIFI AP Initialized!");

  Udp.begin(udpPort);
  Serial.printf("UDP Initialized!");

  digitalWrite(INIT_LED, HIGH);
}


void loop()
{
  if(Udp.beginPacket(clientIp, udpPort) == 0) {
    Serial.println("Udp.beginPacket error");
    return;
  }

  Udp.write(requestPacket, 1);

  if(Udp.endPacket() == 0) {
    Serial.println("Udp.endPacket() error");
    return;
  }

  digitalWrite(PING_LED, HIGH);
  delay(200);
  digitalWrite(PING_LED, LOW);
  delay(200);
}
