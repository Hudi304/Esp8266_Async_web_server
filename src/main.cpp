#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SoftwareSerial.h>


// Replace with your network credentials
const char* ssid = "Redmi";
const char* password = "12345678";

// const char* ssid = "TechQuarter";
// const char* password = "!techP455";

// AsyncWebSocketClient gGlient =  new AsyncWebSocketClient();


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

void handleWebSocketMessage(void* arg, uint8_t* data, size_t len) {
  AwsFrameInfo* info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggle") == 0) {
      ledState = !ledState;
      notifyClients();
    }

    if (strcmp((char*)data, "dash") == 0) {
      ws.textAll(String(rpm[rpm_index]));
      if (rpm_index < 11) {
        rpm_index++;
      }
      else {
        rpm_index = 0;
      }
      // notifyClients();
    }
  }
}

void onEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type,
  void* arg, uint8_t* data, size_t len) {
  Serial.printf("got data id:%u \n", client->id());

  switch (type) {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    client->ping();
    ws.textAll("connected");
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    Serial.printf("message\n");
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
    Serial.printf("pong\n");
    break;
  case WS_EVT_ERROR:
    Serial.printf("error\n");
    break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var) {
  Serial.println(var);
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

SoftwareSerial SpeeduinoSerial(D5, D6); //Rx. Tx


void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  SpeeduinoSerial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  //   Serial.println("Connecting to WiFi..");
  // }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Route for root / web page
  // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    // request->send(200, "text/plain");
  // });

  // Start server

  server.begin();

  delay(10000);
}




int secs = 0;
String chr;


void getSimpleDataSetA() {
  SpeeduinoSerial.print("A");
  //Check if there is anything inside the buffer

  // Serial.println();
  // Serial.println("getSimpleDataSet");

  char buffer[78];
  int index = 0;

  // SpeeduinoSerial.read();

  while (SpeeduinoSerial.available() > 0) {
    // buffer[index] = SpeeduinoSerial.read();
    // index++;v

    Serial.print(SpeeduinoSerial.read(),HEX);
    Serial.print("|");
  }


  Serial.println();
  // Serial.print(buffer);
  // Serial.print("|");


  // Serial.printf("buffer : %s\n", buffer[index]);

  // buffer[index] = '\n';

  // return buffer;
}


char AcmdResp[78];


void loop() {
  if (millis() / 50 > secs) {
    // ws.pingAll();
    ws.binaryAll("a");
    // ws.textAll("p");
    // chr = softSerial.read();
    // Serial.println(chr);
    getSimpleDataSetA();

    // Serial.println();

    // AcmdResp = getSimpleDataSetA()

      // SpeeduinoSerial.println("ESP8266");

    secs = millis() / 50;
  }

  ws.cleanupClients();
  digitalWrite(ledPin, ledState);
}