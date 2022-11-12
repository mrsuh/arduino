#include <WiFi.h>
#include <WiFiUdp.h>


typedef struct _udp_packet {
  int x1;
  int y1;
  int x2;
  int y2;
  int r1;
  int r2;
} UDP_PACKET;

const char* ssid = "esp32-udp";
const char* password = "password";

WiFiUDP Udp;
UDP_PACKET packet;

IPAddress serverIp(192, 168, 4, 1);
IPAddress clientIp(192, 168, 4, 2);
IPAddress serverGateway(192, 168, 4, 1);
IPAddress serverSubnet(255, 255, 255, 0);
unsigned int udpPort = 4210;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Initializing");


  WiFi.begin(ssid, password);
  while(!WiFi.isConnected())
  {
     delay(10);
  }

   Udp.begin(udpPort);
   Serial.println("WIFI connected!");
   Serial.print("Connected, IP address: ");
   Serial.println(WiFi.localIP());

  Serial.println("Initialized!");
}

unsigned long ledEnabledLastTime = 0;
unsigned long rssiDisaplyLastTime = 0;
unsigned long driverLastTime = 0;
void loop()
{
  if(!WiFi.isConnected()) {
    Serial.println("NOT connected");
    return;
  }

  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    Udp.read((byte *)&packet, sizeof(UDP_PACKET));
    //Serial.printf("x1:%d y1:%d x2:%d y2:%d r1:%d r2:%d\n", packet.x1, packet.y1, packet.x2, packet.y2, packet.r1, packet.r2);
    Serial.print(".");
    Udp.flush();
    ledEnabledLastTime = millis();
  }

  if(millis() - rssiDisaplyLastTime > 1000) {
    Serial.print("RSSI: ");
    Serial.println(WiFi.RSSI());
    rssiDisaplyLastTime = millis();
  }
}
