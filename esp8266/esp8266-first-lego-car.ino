#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

#define greenLedpin 5
#define blueLedpin 4

#define motorLeftIn1Pin 14
#define motorLeftIn2Pin 12

#define motorRightIn1Pin 13
#define motorRightIn2Pin 15


AsyncWebServer server(80);

const char* ssid = "ssid";
const char* password = "password";

const char* PARAM_MESSAGE = "action";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

const char indexhtml[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">
    <meta name="viewport" content="width=device-width, initial-scale=1 maximum-scale=1 user-scalable=no">
    <title>LED</title>
</head>
<body>
<div>
    <button id="upButton">UP</button>
</div>
<br>
<div>
    <button id="downButton">DOWN</button>
</div>
<br>
<div>
    <button id="leftButton">LEFT</button>
</div>
<br>
<div>
    <button id="rightButton">RIGHT</button>
</div>
<br>
<div>
    <button id="stopButton">STOP</button>
</div>
<br>
</body>
<script>
    var query = function (url) {
        var xhr = new XMLHttpRequest();
        xhr.open('GET', url, false);
        xhr.send();

        if (xhr.status !== 200) {
            alert(xhr.status + ': ' + xhr.statusText); // пример вывода: 404: Not Found
        }
    };
    document.getElementById('upButton').addEventListener('click', function () {
        query('/move?action=up');
    });
    document.getElementById('downButton').addEventListener('click', function () {
        query('/move?action=down');
    });
    document.getElementById('leftButton').addEventListener('click', function () {
        query('/move?action=left');
    });
    document.getElementById('rightButton').addEventListener('click', function () {
        query('/move?action=right');
    });
     document.getElementById('stopButton').addEventListener('click', function () {
        query('/move?action=stop');
    });
</script>
)rawliteral";

String ledColor;
bool changeColor = false;
IPAddress IP;

void setup() {

    Serial.begin(115200);
    pinMode(greenLedpin, OUTPUT);
    pinMode(blueLedpin, OUTPUT);

    digitalWrite(blueLedpin, HIGH);

    pinMode(motorLeftIn1Pin, OUTPUT);
    pinMode(motorLeftIn2Pin, OUTPUT);

    pinMode(motorRightIn1Pin, OUTPUT);
    pinMode(motorRightIn2Pin, OUTPUT);

    digitalWrite(motorLeftIn1Pin, LOW);
    digitalWrite(motorLeftIn2Pin, LOW);

    digitalWrite(motorRightIn1Pin, LOW);
    digitalWrite(motorRightIn2Pin, LOW);

     IPAddress ip(192,168,4,1);
    IPAddress gateway(192,168,4,254);
    IPAddress subnet(255,255,255,0);

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(ip, gateway, subnet);

    WiFi.softAP(ssid, password);

    IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    // Print ESP8266 Local IP Address
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", indexhtml);
    });

    // Send a GET request to <IP>/get?message=<message>
    server.on("/move", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String color;
        if (request->hasParam(PARAM_MESSAGE)) {
            ledColor = request->getParam(PARAM_MESSAGE)->value();
            changeColor = true;
        } else {
            ledColor = "No action sent";
        }

        request->send(200, "text/plain", "Action: " + ledColor);
    });

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });

    server.onNotFound(notFound);

    server.begin();
}

bool signalLedEnabled = false;

void loop() {

  if(signalLedEnabled) {
    signalLedEnabled = false;
  digitalWrite(blueLedpin, HIGH);
    digitalWrite(greenLedpin, LOW);
  }

  if(ledColor == "up" && changeColor) {
    changeColor = false;
    signalLedEnabled = true;
    digitalWrite(blueLedpin, LOW);
    digitalWrite(greenLedpin, HIGH);

    digitalWrite(motorLeftIn1Pin, HIGH);
    digitalWrite(motorLeftIn2Pin, LOW);
    digitalWrite(motorRightIn1Pin, HIGH);
    digitalWrite(motorRightIn2Pin, LOW);
  }

    if(ledColor == "down" && changeColor) {
    changeColor = false;
    signalLedEnabled = true;
    digitalWrite(blueLedpin, LOW);
    digitalWrite(greenLedpin, HIGH);

    digitalWrite(motorLeftIn1Pin, LOW);
    digitalWrite(motorLeftIn2Pin, HIGH);
    digitalWrite(motorRightIn1Pin, LOW);
    digitalWrite(motorRightIn2Pin, HIGH);
  }

   if(ledColor == "left" && changeColor) {
    changeColor = false;
    signalLedEnabled = true;
    digitalWrite(blueLedpin, LOW);
    digitalWrite(greenLedpin, HIGH);

    digitalWrite(motorLeftIn1Pin, LOW);
    digitalWrite(motorLeftIn2Pin, HIGH);
    digitalWrite(motorRightIn1Pin, HIGH);
    digitalWrite(motorRightIn2Pin, LOW);
  }

  if(ledColor == "right" && changeColor) {
    changeColor = false;
    signalLedEnabled = true;
   digitalWrite(blueLedpin, LOW);
    digitalWrite(greenLedpin, HIGH);

    digitalWrite(motorLeftIn1Pin, HIGH);
    digitalWrite(motorLeftIn2Pin, LOW);
    digitalWrite(motorRightIn1Pin, LOW);
    digitalWrite(motorRightIn2Pin, HIGH);
  }

   if(ledColor == "stop" && changeColor) {
    changeColor = false;
    signalLedEnabled = true;
    digitalWrite(blueLedpin, LOW);
    digitalWrite(greenLedpin, HIGH);

    digitalWrite(motorLeftIn1Pin, LOW);
    digitalWrite(motorLeftIn2Pin, LOW);
    digitalWrite(motorRightIn1Pin, LOW);
    digitalWrite(motorRightIn2Pin, LOW);
   }

  delay(50);
}
