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

enum {
  READY_TO_WRITE = 0,
  READY_TO_READ = 2,
  READY_TO_SEND_SOCKET = 5,
  SENT_SOCKET = 6,
};

unsigned char state = READY_TO_WRITE;

#define  BUFF_LEN 76

char receivedBytes[BUFF_LEN] = {};
char charsForSocket[BUFF_LEN] = {};


int recvIndex = 0;

void flushUARTbuffer() {
  while (Serial.available() > 0) {
    Serial.read();
  }
}

void sendData() {
  if (state == READY_TO_WRITE) {
    if (clientNo > 0) {
      flushUARTbuffer();
      Serial.print('A');
      state = READY_TO_READ;
    }
  }
}

void receiveByte() {
  if (state == READY_TO_READ) {
    if (Serial.available() > 0) {
      // if (receivedBytes[BUFF_LEN - 2] == 0) {
      if (recvIndex < BUFF_LEN ) {

        char received;
        received = Serial.read();
        if (received == 0) {
          received = '0';
        }
        receivedBytes[recvIndex] = received;
        recvIndex++;
      }
      else {
        flushUARTbuffer();
        strcpy(charsForSocket, receivedBytes);
        for (int i = 0; i < BUFF_LEN ;i++) {
          receivedBytes[i] = 0;
        }
        recvIndex = 0;
        state = READY_TO_SEND_SOCKET;
      }
    }
  }
}

#define SO_MSG_LEN 20

void sendDataToSocket() {
  if (state == READY_TO_SEND_SOCKET) {
    ws.binaryAll(charsForSocket);
    state = SENT_SOCKET;
  }
}

void loop() {
  loopIndex++;
  ws.cleanupClients();
  digitalWrite(ledPin, ledState);

  sendData();
  receiveByte();
  sendDataToSocket();
  secs = millis() / 100;

  if (secs != oldSecs) {
    oldSecs = secs;
    state = READY_TO_WRITE;
  }
}