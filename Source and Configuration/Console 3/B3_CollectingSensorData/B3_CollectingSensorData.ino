#include <ESP8266WiFi.h>
 #include <Adafruit_Sensor.h>
 #include<SoftwareSerial.h> 
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h> 
String temp[2];
String hum[2];
  int valueRed = LOW; 
  int valueGreen = LOW;
SoftwareSerial wifiSerial(7,0);      // RX, TX for Receiving msg from ESP-01 of Console 2

String API = "MGWRT5S8QFWTGMSS";// For Tweet
String apiKey = "9WYW3LQR4PAG6AYC"; // For data log

///* For Lakehead
const char* ssid = "Cisco08028";
const char* password = "mobile13WIFI";
WiFiServer server(80);
IPAddress ip(192, 168, 1, 210); // where xx is the desired IP Address
IPAddress gateway(192, 168, 1, 1); // set gateway to match your network

// */
int redled =D5;    // initialize digital pin.   
int greenled =D6; 
uint32_t delayMS;
// your message
String tweet = "Temp_Not_Stable_Console3_Report"; 
 
void setup() {
  Serial.begin(115200);
  delay(10); 
  pinMode(LED_BUILTIN, OUTPUT); 

  pinMode(redled, OUTPUT);   
pinMode(greenled, OUTPUT); 
  digitalWrite(redled, LOW); 
  digitalWrite(greenled, LOW);
  
 
  Serial.print(F("Setting static ip to : "));
  Serial.println(ip);
 
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(" Your Wi-Fi ");
  IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network
  WiFi.config(ip, gateway, subnet); 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/"); 
 
}
 String  readWifiSerialMessage(){
  char value[100]; 
  int index_count =0;
  while(wifiSerial.available()>0){
    value[index_count]=wifiSerial.read();
    index_count++;
    value[index_count] = '\0'; // Null terminate the string
  }
  String str(value);
  str.trim();
  return str;
}
void loop() {
  int t=0;
  int h=0;
  String th;
   digitalWrite(LED_BUILTIN, LOW);
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush(); 
   th=readWifiSerialMessage();
   //temp=th.charAt(0);
   temp[2]=th.substring(0);
   //hum = th.charAt(3);
   hum[2] = th.substring(3);
   String temp1=temp[2];
   String hum1=hum[2];
    t=temp1.toInt();
    h=hum1.toInt();
   if (request.indexOf("/LEDR=ON") != -1) { 
   digitalWrite(redled, HIGH);
   digitalWrite(greenled, LOW);
    valueRed = HIGH;
    
  }   
     if (request.indexOf("/LEDR=OFF") != -1){
     digitalWrite(redled, LOW);
   digitalWrite(greenled, HIGH);
    valueRed = LOW;
     }
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<body bgcolor=\"/#00FF00\"/>");
  client.print("<center><h1><u>Temperature and Humidity Data From Console-1 Via Console-2</u></h1></center>");
    if((t>=18)&&(t<=25)) {
      digitalWrite(redled, LOW);
      digitalWrite(greenled, HIGH);      
    }
     else  if((t<18)||(t>25)){
      digitalWrite(redled, HIGH);
      digitalWrite(greenled, LOW);
      if (client.connect("184.106.153.149", 80)){
      tweet = "Temperature_Not_stable_You_Need_To_Visit_The_Place_To_Fix_This_Issue."; 
  client.print("GET /apps/thingtweet/1/statuses/update?key=" + API + "&status=" + tweet + " HTTP/1.1\r\n");
  client.print("Host: api.thingspeak.com\r\n"); 
  client.print("Accept: */*\r\n"); 
  client.print("User-Agent: Mozilla/4.0 (compatible; esp8266 Lua; Windows 7 Ultimate)\r\n");
  client.print("\r\n");  
      }
     }
  client.println("<br><br>");
  client.println("The Temperature of the room is :<br>");
  client.println(t);
  client.print("degrees<br>");
  client.println("<br><br>");
   client.println("The Humidity level of the room is : <br>");
  client.println(h);
  client.print("%<br>");
  client.println("<br><br>"); 
  client.println("Click <a href=\"/LEDR=ON\">here</a> to Manually start the temperature control Mechanism.<br>");
  client.println("Click <a href=\"/LEDR=OFF\">here</a> to turn the temperature control Mechanism off.<br>");
  client.println("<br><br>");

  client.println("</body>");
  client.println("</html>");
 //****************For Things Speak Data Log************//


if (client.connect("api.thingspeak.com",80)) {
String postStr = apiKey;
postStr +="&field1=";
postStr += String(t);
postStr +="&field2=";
postStr += String(h);
postStr += "\r\n\r\n";
 
client.print("POST /update HTTP/1.1\n");
client.print("Host: api.thingspeak.com\n");
client.print("Connection: close\n");
client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
client.print("Content-Type: application/x-www-form-urlencoded\n");
client.print("Content-Length: ");
client.print(postStr.length());
client.print("\n\n");
client.print(postStr);
 
}
client.stop();
 
Serial.println("Waiting 20 secs");
// thingspeak needs at least a 15 sec delay between updates
// 20 seconds to be safe
delay(20000);
 
  //****************For Sending Tweets************//
  if (client.connect("184.106.153.149", 80)){ 
     if(((t>=18)&&(t<=25))&& (request.indexOf("/LEDR=ON") != -1)) {
      tweet = "Stablizing_Temp_Measures_Are_Being_Taken."; 
  client.print("GET /apps/thingtweet/1/statuses/update?key=" + API + "&status=" + tweet + " HTTP/1.1\r\n");
  client.print("Host: api.thingspeak.com\r\n"); 
  client.print("Accept: */*\r\n"); 
  client.print("User-Agent: Mozilla/4.0 (compatible; esp8266 Lua; Windows 7 Ultimate)\r\n");
  client.print("\r\n");      
     }
     else  if(((t<18)||(t>25)) && (request.indexOf("/LEDR=ON") != -1)){
      tweet = "Temperature_Not_stable_You_Need_To_Visit_The_Place_To_Fix_This_Issue."; 
  client.print("GET /apps/thingtweet/1/statuses/update?key=" + API + "&status=" + tweet + " HTTP/1.1\r\n");
  client.print("Host: api.thingspeak.com\r\n"); 
  client.print("Accept: */*\r\n"); 
  client.print("User-Agent: Mozilla/4.0 (compatible; esp8266 Lua; Windows 7 Ultimate)\r\n");
  client.print("\r\n");   
     }   
     if(((t>=18)&&(t<=25))&& (request.indexOf("/LEDR=OFF") != -1)) {
      tweet = "Issue_resolved_Temperature_Is_Under_Control!"; 
  client.print("GET /apps/thingtweet/1/statuses/update?key=" + API + "&status=" + tweet + " HTTP/1.1\r\n");
  client.print("Host: api.thingspeak.com\r\n"); 
  client.print("Accept: */*\r\n"); 
  client.print("User-Agent: Mozilla/4.0 (compatible; esp8266 Lua; Windows 7 Ultimate)\r\n");
  client.print("\r\n");      
     }
     else  if(((t<18)||(t>25))&& (request.indexOf("/LEDR=OFF") != -1)){
      tweet = "Temperature_Not_stable_You_Need_To_Visit_The_Place_To_Fix_This_Issue."; 
  client.print("GET /apps/thingtweet/1/statuses/update?key=" + API + "&status=" + tweet + " HTTP/1.1\r\n");
  client.print("Host: api.thingspeak.com\r\n"); 
  client.print("Accept: */*\r\n"); 
  client.print("User-Agent: Mozilla/4.0 (compatible; esp8266 Lua; Windows 7 Ultimate)\r\n");
  client.print("\r\n");   
     }  
} 
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
}

