#include <Arduino.h>
#include <FS.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h> 
#include <buttonpush.h>
#include <turnout.h>
#include <iostream>
#include <AsyncElegantOTA.h>
//  #include <ArduinoOTA.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// ----------------------------------------------------------------------------
// Definition of macros
// ----------------------------------------------------------------------------

#define HTTP_PORT 80
#define NUM_TURNOUTS 8
#define STANDARD_DELAY 50
#define NUM_BUTTON 6

// ----------------------------------------------------------------------------
// Definition of global constants
// ----------------------------------------------------------------------------

// WiFi credentials

const char *WIFI_SSID = "Valkyria_2.4";
const char *WIFI_PASS = "Ev4.P3r0n.1778";
const char* hostname = "ESP8266maq";
// ----------------------------------------------------------------------------
// Definition of global variables
// ----------------------------------------------------------------------------

TURNOUT_PARAMS tdef[NUM_TURNOUTS] = {
 {0, 200, 215, ALIGN_MAIN, STANDARD_DELAY},
 {1, 200, 215, ALIGN_MAIN, STANDARD_DELAY},
 {2, 180, 215, ALIGN_MAIN, STANDARD_DELAY},
 {3, 200, 228, ALIGN_MAIN, STANDARD_DELAY},
 {4, 200, 228, ALIGN_MAIN, STANDARD_DELAY},
 {5, 200, 228, ALIGN_MAIN, STANDARD_DELAY},
 {6, 200, 228, ALIGN_MAIN, STANDARD_DELAY},
 {7, 200, 228, ALIGN_MAIN, STANDARD_DELAY},



 };

BUTTON_PARAMS bdef[NUM_BUTTON] = {
 {16, false, false, 0, 0, HIGH, 0, 0},
 {13, false, true,  1, 2, HIGH, 0, 0},
 {14, false, true,  3, 4, HIGH, 0, 0},
 {12, false, false, 5, 0, HIGH, 0, 0},
 {2,  false, false, 6, 0, HIGH, 0, 0},
 {9,  false, false, 7, 0, HIGH, 0, 0},
 };


turnout *turnouts[NUM_TURNOUTS];
Button *buTTon[NUM_BUTTON];

AsyncWebServer server(HTTP_PORT);
AsyncWebSocket ws("/ws");

// ----------------------------------------------------------------------------
// SPIFFS initialization
// ----------------------------------------------------------------------------

void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("Cannot mount SPIFFS volume...");
  }
}

// ----------------------------------------------------------------------------
// Connecting to the WiFi network
// ----------------------------------------------------------------------------

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("Trying to connect [%s] ", WiFi.macAddress().c_str());
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }
  Serial.printf(" %s\n", WiFi.localIP().toString().c_str());
}

// ----------------------------------------------------------------------------
// Web server initialization
// ----------------------------------------------------------------------------

void onRootRequest(AsyncWebServerRequest *request) {
  request->send(SPIFFS, "/index.html", "text/html");
}

void initWebServer() {
    server.on("/", onRootRequest);
    server.serveStatic("/", SPIFFS, "/");
    server.begin();
}

void notifyClients(int buttWeb) {
    const uint8_t size = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<size> json;
    char buttonJson = char(buttWeb);
    json["status"] = "moving";
    json["button"] = buttWeb;
    char buffer[38];
    size_t len = serializeJson(json, buffer);
    ws.textAll(buffer, len);
}


void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {

        const uint8_t size = JSON_OBJECT_SIZE(2);
        StaticJsonDocument<size> json;
        DeserializationError err = deserializeJson(json, data);
        if (err) {
            Serial.print(F("deserializeJson() failed with code "));
            Serial.println(err.c_str());
            return;
        }
        const char *button = json["button"];
        const char *pushed = json["pushed"];
        int buttonNum = int(*button) - 48;
        if (strcmp(pushed, "true") == 0) {
            buTTon[buttonNum]->webPush = true; 
        }

    }
}

void onEvent(AsyncWebSocket       *server,
             AsyncWebSocketClient *client,
             AwsEventType          type,
             void                 *arg,
             uint8_t              *data,
             size_t                len) {

    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}

void initWebSocket() {
    ws.onEvent(onEvent);
    server.addHandler(&ws);
}

void InitMDNS()
{
   if (!MDNS.begin(hostname)) 
   {             
     Serial.println("Error iniciando mDNS");
   }
   Serial.println("mDNS iniciado");
}


void setup() {
//  ArduinoOTA.begin();

// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------

  for (int i = 0; i < NUM_TURNOUTS; i++){ 
    turnouts[i] = new turnout(&tdef[i]); 
  }

  for (int i = 0; i < NUM_TURNOUTS; i++){ 
    turnouts[i]->initialize(); 
  }

  for (int x = 0; x < NUM_BUTTON; x++){ 
    buTTon[x] = new Button(&bdef[x]); 
  }

  for (int x = 0; x < NUM_BUTTON; x++){ 
    pinMode(buTTon[x]->pin, INPUT_PULLUP); 
  }

  pwm.begin();
  pwm.setPWMFreq(50); // Analog servos run at ~60 Hz updates
  Serial.begin(115200); delay(500);
  Serial.println("Comienzo a funcionar el sistema de Servos x ESP8266 y PCA9685");
  

  initSPIFFS();
  initWiFi();
  initWebSocket();
  initWebServer();
  InitMDNS();
  AsyncElegantOTA.begin(&server);  
}

void loop() {
ws.cleanupClients();
MDNS.update();
// ArduinoOTA.handle(); 

unsigned long currentMilis = millis();


// turnouts[0]->update(currentMilis);
// turnouts[1]->update(currentMilis);
for (int i = 0; i < NUM_TURNOUTS; i++){ 
    turnouts[i]->update(currentMilis); 
    }

for (int w = 0; w < NUM_BUTTON; w++) {
      buTTon[w]->read();
      if (buTTon[w]->pressed() || buTTon[w]->webPush == true ){
        notifyClients(w);
        Serial.println("Detecte un boton");
        Serial.println(w, DEC);
        buTTon[w]->webPush = false;
        if (buTTon[w]->mgdServo() == true) {
            Serial.println("Manejo mas de un servo");
            int l = buTTon[w]->servoManaged1;
            int v = buTTon[w]->servoManaged2;
            Serial.println("Muevo los servos");
            Serial.println(l, DEC);
            Serial.println(v, DEC);
            turnouts[l]->toggle();
            turnouts[v]->toggle();
        } else {
          int n = buTTon[w]->servoManaged1;
          Serial.println("Muevo el servo");
          Serial.println(n, DEC);
          turnouts[n]->toggle();
        }    
      }

  }




}