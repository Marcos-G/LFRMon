#include <ESP8266WiFi.h>
#include <indexl.h>
#include <Arduino.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>   // Include the WebServer library

const char* ssid = "cosa";
const char* password = "aserejeja";
// Create an instance of the server
// specify the port to listen on as an argument
ESP8266WebServer server(80);
int ret=1000;
bool conns[10]={false,false,false,false,false,false,false,false,false,false};
WebSocketsServer webSocket = WebSocketsServer(81);    // create a websocket server on port 81
void webSocketEvent(uint8_t num, WStype_t type,unsigned char * payload, size_t lenght) { // When a WebSocket message is received
  switch (type) {
    case WStype_DISCONNECTED:             // if the websocket is disconnected
      conns[num]=false;
      break;
    case WStype_CONNECTED: {              // if a new websocket connection is established
        IPAddress ip = webSocket.remoteIP(num);
        conns[num]=true;
      }
      break;
    case WStype_TEXT:                     // if new text data is received
  String message=(char*)payload;
  if(message[0]=='S'){
    ret=message.substring(1).toInt();
  }
  else{
  Serial.print(message+"\n");
}
      break;
  }
}
void startWebSocket() { // Start a WebSocket server
  webSocket.begin();                          // start the websocket server
  webSocket.onEvent(webSocketEvent);          // if there's an incomming websocket message, go to function 'webSocketEvent'
  Serial.println("WebSocket server started.");
}
void handleRoot() {
  IPAddress ip =WiFi.localIP();
  server.send(200, "text/html", file1+String(ip[0])+"."+String(ip[1])+"."+String(ip[2])+"."+String(ip[3])+file2);   // Send HTTP status 200 (Ok) and send some text to the browser/client
}
void setup() {
  Serial.begin(115200);
  delay(10);
  randomSeed(analogRead(0));
  WiFi.begin(ssid, password);
  startWebSocket();            // Start a WebSocket server
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  server.on("/", handleRoot);               // Call the 'handleRoot' function when a client requests URI "/"
  // Start the server
  server.begin();
  // Print the IP address
  Serial.println(WiFi.localIP());
}
String output="string";
unsigned long time=0;
void loop() {
    server.handleClient();
    webSocket.loop();                           // constantly check for websocket events
    while(Serial.available()>0){
      output=Serial.readStringUntil('\n');
    }
if(time<(millis()-ret)){

for(int i=0;i<10;i++){
  if(conns[i]){
    webSocket.sendTXT(i,output);
  }
}
time=millis();
}
}
