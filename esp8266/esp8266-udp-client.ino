#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Servo.h> 

#define servoPin 14

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

Servo myservo;
WiFiUDP Udp;
UDP_PACKET packet;

char replyPacket[10] = "1";
IPAddress serverIp(192, 168, 4, 1);
IPAddress clientIp(192, 168, 4, 2);
IPAddress serverGateway(192, 168, 4, 1);
IPAddress serverSubnet(255, 255, 255, 0);
unsigned int udpPort = 4210;

void setup()
{
  pinMode(2, OUTPUT);
  pinMode(16, OUTPUT);
  digitalWrite(2, HIGH);
  digitalWrite(16, HIGH);

  myservo.attach(servoPin);
  
  Serial.begin(115200);
  Serial.println();
  Serial.println("Initializing");
  
  WiFi.setOutputPower(20.5);  
  WiFi.setPhyMode(WIFI_PHY_MODE_11B);
  WiFi.setAutoReconnect(true);
  WiFi.config(clientIp, serverGateway, serverSubnet);
  
  WiFi.begin(ssid, password);
  while (!WiFi.isConnected())
  {
     delay (10);
  }
  digitalWrite(16, LOW);

   Udp.begin(udpPort);
   Serial.println("WIFI connected!");
   Serial.print("Connected, IP address: ");
   Serial.println(WiFi.localIP());
   
  Serial.println("Initialized!");
}

unsigned long ledEnabledLastTime = 0;
unsigned long rssiDisaplyLastTime = 0;
void loop()
{
  if(!WiFi.isConnected()) {
    digitalWrite(16, HIGH);
    return;
  }
  digitalWrite(16, LOW);

  digitalWrite(2, HIGH);
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    Udp.read((byte *)&packet, sizeof(UDP_PACKET));
    Serial.printf("x1:%d y1:%d x2:%d y2:%d r1:%d r2:%d\n", packet.x1, packet.y1, packet.x2, packet.y2, packet.r1, packet.r2);   
    myservo.write(map(packet.y1, 0, 4095, 0, 180));
    Udp.flush();
    ledEnabledLastTime = millis();   
  }

  if(millis() - ledEnabledLastTime < 200) {
    digitalWrite(2, LOW);
  } else {
    digitalWrite(2, HIGH);
  }

  if(millis() - rssiDisaplyLastTime > 1000) {
    //Serial.println(WiFi.RSSI()); 
    rssiDisaplyLastTime = millis();  
  }
}
