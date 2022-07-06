#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "Redmi";
const char* password = "12345678";
bool ledState = 0;
const int ledPin = 2;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncWebSocketClient* adminClient;
uint8_t clientIndex = 0;

long int loopIndex = 0;

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
    String dataString = String((const char*)data);
    if (strcmp((char*)data, "toggle") == 0) {
      ledState = !ledState;
    }

    if (strcmp((char*)data, "A") == 0) {
      // Serial.print('A');
      notifyClientsStr("UART : A");
    }

    if (strcmp((char*)data, "R") == 0) {

    }
    else {}
  }
}

int clientNo = 0;

void onEvent(AsyncWebSocket* socket, AsyncWebSocketClient* client, AwsEventType type,
  void* arg, uint8_t* data, size_t len) {
  switch (type) {
  case WS_EVT_CONNECT:
    clientNo++;
    notifyClientsStr("con");
    break;
  case WS_EVT_DISCONNECT:
    // Serial.printf("WebSocket client #%u disconnected\n", client->id());
    clientNo--;
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len, client);
    break;
  case WS_EVT_PONG:
    break;
  case WS_EVT_ERROR:
    break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var) {
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

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  initWebSocket();
  server.begin();
}

int secs = 0;
int oldSecs = 0;

bool lock = true;

bool recvInProgress = false;
bool doneTransmitting = true;
bool doneReading = true;

bool readyToQuery = true;
bool readyToReadData = false;
bool readyToSendToSocket = false;
bool sendingToSocket = false;
bool bufferHasNewData = false;


char receivedBytes[20];
char charsForSocket[20];
u_int8_t recvIndex = 0;

void flushUARTbuffer() {
  while (Serial.available() > 0) {
    Serial.read();
  }
}

void sendData() {
  if (readyToQuery == true && clientNo > 0) {
    flushUARTbuffer();
    Serial.print('A');
    readyToQuery = false;
    readyToReadData = true;
    ws.textAll("sent UART data");
  }
}

void receiveByte() {
  if (Serial.available() > 0 && readyToReadData) {
    if (recvIndex < 13) {
      char received;
      char chr[1];
      received = Serial.read();
      receivedBytes[recvIndex] = received;
      chr[0] = received;
      // ws.textAll(receivedBytes);
      ws.textAll(chr);

      recvIndex++;
      bufferHasNewData = true;
    }
    else {
      ws.textAll(receivedBytes);
      flushUARTbuffer();
      readyToReadData = false;
      readyToSendToSocket = true;
      recvIndex = 0;
    }
  }
  else {
    flushUARTbuffer();
    readyToReadData = false;
    readyToSendToSocket = true;
    strcpy(charsForSocket, receivedBytes);
    recvIndex = 0;
  }
}

void sendDataToSocket() {
  if (readyToSendToSocket && bufferHasNewData && lock == false) {
    // ws.textAll("test send");
    ws.textAll(receivedBytes);

    readyToSendToSocket = false;
    readyToQuery = true;
    bufferHasNewData = false;
    lock = true;
  }
}

void loop() {
  loopIndex++;
  ws.cleanupClients();
  digitalWrite(ledPin, ledState);

  sendData();
  receiveByte();
  sendDataToSocket();
  secs = millis() / 1000;

  if (secs != oldSecs) {
    oldSecs = secs;
    lock = false;
    if (bufferHasNewData == false) {
      readyToQuery = true;
    }
  }
}