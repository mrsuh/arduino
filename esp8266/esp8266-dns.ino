#include <Arduino.h>
#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>

#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

const byte DNS_PORT = 53;
AsyncWebServer server(80);
DNSServer dnsServer;

const char *ssid = "test-esp8266";
const char *password = "password";


IPAddress IP;

void setup() {

    Serial.begin(115200);
 

    IPAddress ip(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(ip, ip, subnet);

    WiFi.softAP(ssid, password);

    // modify TTL associated  with the domain name (in seconds)
  // default is 60 seconds
  dnsServer.setTTL(300);
  // set which return code will be used for all other domains (e.g. sending
  // ServerFailure instead of NonExistentDomain will reduce number of queries
  // sent by clients)
  // default is DNSReplyCode::NonExistentDomain
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);

  // start DNS server for a specific domain name
  dnsServer.start(DNS_PORT, "example.com", ip);
  

    server.on("/", HTTP_GET,[](AsyncWebServerRequest * request)
    {
      char data[100];
      sprintf(data, "hello %d", millis());
        request->send(200, "text/html", data);
    });

     server.onNotFound([](AsyncWebServerRequest * request)
    {
        request->send(404, "text/html", "Not found");
    });


    server.begin();
}


void loop() {

}
