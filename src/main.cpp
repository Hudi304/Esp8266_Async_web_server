#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "Redmi";
const char* password = "12345678";

const char* SoftAPssid = "ESP8266";
const char* SoftAPpassword = "ESP8266";

bool ledState = 0;
const int ledPin = 2;

uint32_t rpm[12] = { 1000, 1000, 1500, 2000, 2500, 3000, 3000, 2500, 2000,1500,1000,1000 };
uint8_t rpm_index = 0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void notifyClients() {
  ws.textAll(String(ledState));
}

void notifyClientsStr(String msg) {
  ws.textAll(msg);
}

void handleWebSocketMessage(void* arg, uint8_t* data, size_t len, AsyncWebSocketClient* client) {
  AwsFrameInfo* info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggle") == 0) {
      ledState = !ledState;

      String msg = "";
      msg.concat("Client[");
      msg.concat(String(client->id()));
      msg.concat("] turned the LED : ");
      if (ledState) {
        msg.concat("OFF");
      }
      else {
        msg.concat("ON");
      }
      notifyClientsStr(msg);
    }
    else {
      String msg = "";
      msg.concat("Client[");
      msg.concat(String(client->id()));
      msg.concat("]: ");


      for (int i = 0; i < len;i++) {
        msg.concat((char)data[i]);
      }
      notifyClientsStr(msg);

    }

    // if (strcmp((char*)data, "dash") == 0) {
    //   ws.textAll(String(rpm[rpm_index]));
    //   if (rpm_index < 11) {
    //     rpm_index++;
    //   }
    //   else {
    //     rpm_index = 0;
    //   }
    //   // notifyClients();
    // }
  }
}

void onEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type,
  void* arg, uint8_t* data, size_t len) {
  // Serial.printf("Msg Client [%u]  \n", client->id());

  switch (type) {
  case WS_EVT_CONNECT:
    // Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    // client->ping();
    // ws.textAll("connected");
    break;
  case WS_EVT_DISCONNECT:
    // Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    // Serial.printf("message\n");
    handleWebSocketMessage(arg, data, len, client);
    break;
  case WS_EVT_PONG:
    // Serial.printf("pong\n");
    break;
  case WS_EVT_ERROR:
    // Serial.printf("error\n");
    break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var) {
  // Serial.println(var);
  if (var == "STATE") {
    if (ledState) {
      return "ON";
    }
    else {
      return "OFF";
    }
  }
  return String();
}



const char* ap_ssid = "ESP8266"; //Access Point SSID
const char* ap_password = "embedded-robotics"; //Access Point Password
uint8_t max_connections = 8;//Maximum Connection Limit for AP
int current_stations = 0, new_stations = 0;

void setUpStation() {
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    // Serial.println("Connecting to WiFi..");
  }
}

void setup() {
  Serial.begin(115200);

  delay(500);

  Serial.swap();

  delay(500);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  setUpStation();
  delay(2000);
  // Serial.println(WiFi.localIP());
  initWebSocket();
  server.begin();
}

void loop() {
  ws.cleanupClients();
  digitalWrite(ledPin, ledState);

  
}