#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <Servo.h>

#define greenLedpin 5
#define blueLedpin 4
#define redLedpin 2

#define servoPin 14

#define motorSpeedPin 12
#define motorIn1Pin 13
#define motorIn2Pin 15

AsyncWebServer server(80);
Servo myservo;

const char* ssid = "ssid";
const char* password = "password";

const char* PARAM_MESSAGE = "color";

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
    <button id="redButton">RED</button>
</div>
<br>
<div>
    <button id="blueButton">BLUE</button>
</div>
<br>
<div>
    <button id="greenButton">GREEN</button>
</div>
<br>
<div>
    <button id="whiteButton">WHITE</button>
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
    document.getElementById('redButton').addEventListener('click', function () {
        query('/led?color=red');
    });
    document.getElementById('blueButton').addEventListener('click', function () {
        query('/led?color=blue');
    });
    document.getElementById('greenButton').addEventListener('click', function () {
        query('/led?color=green');
    });
    document.getElementById('whiteButton').addEventListener('click', function () {
        query('/led?color=white');
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
    pinMode(redLedpin, OUTPUT);

    pinMode(motorSpeedPin, OUTPUT);
    pinMode(motorIn1Pin, OUTPUT);
    pinMode(motorIn2Pin, OUTPUT);

    digitalWrite(motorIn1Pin, HIGH);
    digitalWrite(motorIn2Pin, LOW);

    myservo.attach(servoPin);

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
    server.on("/led", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String color;
        if (request->hasParam(PARAM_MESSAGE)) {
            ledColor = request->getParam(PARAM_MESSAGE)->value();
            changeColor = true;
        } else {
            ledColor = "No color sent";
        }

        request->send(200, "text/plain", "Color, GET: " + ledColor);
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

void loop() {

  if(ledColor == "red" && changeColor) {
    changeColor = false;
    digitalWrite(greenLedpin, LOW);
    digitalWrite(blueLedpin, LOW);
    digitalWrite(redLedpin, HIGH);
    myservo.write(45);
    digitalWrite(motorIn1Pin, HIGH);
    digitalWrite(motorIn2Pin, LOW);
    digitalWrite(motorSpeedPin, HIGH);
  }

  if(ledColor == "orange" && changeColor) {
    changeColor = false;
    digitalWrite(greenLedpin, LOW);
    digitalWrite(blueLedpin, HIGH);
    digitalWrite(redLedpin, HIGH);
  }

    if(ledColor == "green" && changeColor) {
      changeColor = false;
    digitalWrite(greenLedpin, HIGH);
    digitalWrite(blueLedpin, LOW);
    digitalWrite(redLedpin, LOW);
    myservo.write(90);
    digitalWrite(motorIn1Pin, HIGH);
    digitalWrite(motorIn2Pin, LOW);
    digitalWrite(motorSpeedPin, HIGH);
  }

   if(ledColor == "blue" && changeColor) {
    changeColor = false;
    digitalWrite(greenLedpin, LOW);
    digitalWrite(blueLedpin, HIGH);
    digitalWrite(redLedpin, LOW);
    myservo.write(180);
    digitalWrite(motorIn1Pin, HIGH);
    digitalWrite(motorIn2Pin, LOW);
    digitalWrite(motorSpeedPin, HIGH);
  }

  if(ledColor == "white" && changeColor) {
    changeColor = false;
    digitalWrite(greenLedpin, LOW);
    digitalWrite(blueLedpin, LOW);
    digitalWrite(redLedpin, LOW);
    digitalWrite(motorIn1Pin, LOW);
    digitalWrite(motorIn2Pin, LOW);
    digitalWrite(motorSpeedPin, LOW);
    myservo.write(0);
    analogWrite(motorSpeedPin, 0);
  }

  delay(50);
}
