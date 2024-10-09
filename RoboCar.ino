#include <WiFi.h> 
#include <ESPAsyncWebServer.h> 

// SSID and password of wifi to which to connect
/*const char* ssid = "Samsung Galaxy A30"; // CHANGE IT 
const char* password = "prathmesh3623"; // CHANGE IT 
*/
const char* ssid = "Boondals"; // CHANGE IT 
const char* password = "Haveaboondalshaveafun"; // CHANGE IT 

#define MOTOR_1_PIN_1 12
#define MOTOR_1_PIN_2 13
#define MOTOR_2_PIN_1 14
#define MOTOR_2_PIN_2 15

int MOTORSTATE = 0;
int PREVMOTORSTATE = 0;
String messagetosend = "";
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Welcome to shishi car</title>
</head>

<style>
    *
    {
        padding: 0;
        margin: 0;
    }
    body
    {
        position: absolute;
        top: 0%;
        left: 0%;
        width: 100%;
        height: 100%;
        background-color: pink;
    }
    .titlle
    {
        position: relative;
        left: 35%;
        padding-top: 10px;        
    }
    .butcontainer
    {
        position: relative;
        top: 20%;
        left: 40%;
        background-color: rgba(14, 0, 0, 0);
        width: 20%;
        height: 50%;
        display: flex;
        flex-direction: column;
        justify-content: space-between;
    }
    .dirbutt
    {
        width: 100%;
        display: flex;
        justify-content: space-evenly;
    }
</style>

<body>
    <h1 class="titlle">Welcome to SHISHI Car server</h1>
        <button onclick='sendFORWARD()'>FORWARD</button>
        <div class="dirbutt">
            <button onclick='sendLEFT()'>LEFT</button>
            <button onclick='sendRIGHT()'>RIGHT</button>
        </div>
        <button onclick='sendBAKCWARD()'>BACKWARD</button>
        <button onclick='sendSTOP()'>STOP</button>
</body>
<script>
    document.addEventListener('keydown' , HandleKeyBoardInput);
    var websocket = new WebSocket('ws://' + '192.168.237.230' + '/ws');
    websocket.onmessage = function (event) { console.log('Message from ESP32:', event.data); };
    websocket.onopen = onOpen;
    websocket.onclose = onClose;

    function onOpen(event) {
        console.log('Connection opened');
    }
    function onClose(event) {
        console.log('Connection closed');
    }

    function sendLEFT() {
        websocket.send('LEFT');
    }

    function sendFORWARD() {
        websocket.send('FORWARD');
    }

    function sendBAKCWARD() {
        websocket.send('BACKWARD');
    }

    function sendRIGHT() {
        websocket.send('RIGHT');
    }

    function sendSTOP() {
        websocket.send('STOP');
    }


    function HandleKeyBoardInput(e)
    {
        switch(e.key)
        {
            case 'w':
                sendFORWARD();
                break;
            case 's':
                sendBAKCWARD();
                break;
            case 'a':
                sendLEFT();
                break;
            case 'd':
                sendRIGHT();
                break;
            case 'spacebar':
            case ' ':
                sendSTOP();
                break;
        }
    }
</script>
</htmlc>
)rawliteral";

// Webserver at port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws"); 

void notifyClients() {
  ws.textAll(messagetosend);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "RIGHT") == 0) {
      Serial.println("Message from server : RIGHT");
      messagetosend = "ROTATING RIGHT";
      PREVMOTORSTATE = MOTORSTATE;
      MOTORSTATE = 3;
      notifyClients();
    }

    if (strcmp((char*)data, "FORWARD") == 0) {
      Serial.println("Message from server : FORWARD");
      messagetosend = "MOVING FORWARD";
      PREVMOTORSTATE = MOTORSTATE;
      MOTORSTATE = 1;
      notifyClients();
    }

    if (strcmp((char*)data, "BACKWARD") == 0) {
      Serial.println("Message from server : BACKWARD");
      messagetosend = "MOVING BACKWARD";
      PREVMOTORSTATE = MOTORSTATE;
      MOTORSTATE = 2;
      notifyClients();
    }

    if (strcmp((char*)data, "LEFT") == 0) {
      Serial.println("Message from server : LEFT");
      messagetosend = "ROTATING LEFT";
      PREVMOTORSTATE = MOTORSTATE;
      MOTORSTATE = 4;
      notifyClients();
    }

    if (strcmp((char*)data, "STOP") == 0) {
      Serial.println("Message from server : STOP");
      messagetosend = "BOT STOPED";
      PREVMOTORSTATE = MOTORSTATE;
      MOTORSTATE = 0;
      notifyClients();
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
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

void setup() { 
   Serial.begin(9600); 
   
   pinMode(MOTOR_1_PIN_1 , OUTPUT);
   pinMode(MOTOR_1_PIN_2 , OUTPUT);
   pinMode(MOTOR_2_PIN_1 , OUTPUT);
   pinMode(MOTOR_2_PIN_2 , OUTPUT);
   digitalWrite(MOTOR_1_PIN_1 , LOW);
   digitalWrite(MOTOR_1_PIN_2 , LOW);
   digitalWrite(MOTOR_2_PIN_1 , LOW);
   digitalWrite(MOTOR_2_PIN_2 , LOW);
  
  // add event handler to server 
  ws.onEvent(onEvent);
  server.addHandler(&ws);
   
   // Connect to Wi-Fi 
   WiFi.begin(ssid, password); 
   Serial.println("Connecting to wifi");
   while (WiFi.status() != WL_CONNECTED) { 
     delay(1000); 
     Serial.println("Connecting to WiFi..."); 
   } 
   Serial.println("Connected to WiFi"); 
   // Print the ESP32's IP address 
   Serial.print("ESP32 Web Server's IP address: "); 
   Serial.println(WiFi.localIP()); 
   
   // Define a default('/') route to serve the HTML page 
   server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) { 
     Serial.println("ESP32 Web Server: New request received:");  // for debugging 
     Serial.println("GET /");        // for debuggin; 
     request->send(200, "text/html", index_html); 
   }); 
   // Start the server 
   server.begin(); 
} 

void loop()
{
  ws.cleanupClients();
  switch(MOTORSTATE)
  {
    case 0: // STOP
      StopMotors();
      break;
    case 1: // FORWARD
      if((PREVMOTORSTATE - MOTORSTATE) != 0)
      {
        StopMotors();
        delay(10);
      }
      digitalWrite(MOTOR_1_PIN_1 , HIGH);
      digitalWrite(MOTOR_1_PIN_2 , LOW);
      digitalWrite(MOTOR_2_PIN_1 , HIGH);
      digitalWrite(MOTOR_2_PIN_2 , LOW);
      break;
    case 2:  // BACKWARD
      if((PREVMOTORSTATE - MOTORSTATE) != 0)
      {
        StopMotors();
        delay(10);
      }
      digitalWrite(MOTOR_1_PIN_1 , LOW);
      digitalWrite(MOTOR_1_PIN_2 , HIGH);
      digitalWrite(MOTOR_2_PIN_1 , LOW);
      digitalWrite(MOTOR_2_PIN_2 , HIGH);
      break;

    case 3:  // RIGHT
      if((PREVMOTORSTATE - MOTORSTATE) != 0)
      {
        StopMotors();
        delay(10);
      }
      digitalWrite(MOTOR_1_PIN_1 , LOW);
      digitalWrite(MOTOR_1_PIN_2 , HIGH);
      digitalWrite(MOTOR_2_PIN_1 , HIGH);
      digitalWrite(MOTOR_2_PIN_2 , LOW);
      break;
      
    case 4:  // LEFT
      if((PREVMOTORSTATE - MOTORSTATE) != 0)
      {
        StopMotors();
        delay(10);
      }
      digitalWrite(MOTOR_1_PIN_1 , HIGH);
      digitalWrite(MOTOR_1_PIN_2 , LOW);
      digitalWrite(MOTOR_2_PIN_1 , LOW);
      digitalWrite(MOTOR_2_PIN_2 , HIGH);
      break;
  }
} 

void StopMotors()
{
      digitalWrite(MOTOR_1_PIN_1 , LOW);
      digitalWrite(MOTOR_1_PIN_2 , LOW);
      digitalWrite(MOTOR_2_PIN_1 , LOW);
      digitalWrite(MOTOR_2_PIN_2 , LOW);
}
