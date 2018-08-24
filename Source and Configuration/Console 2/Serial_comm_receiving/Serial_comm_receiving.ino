#include <SoftwareSerial.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h> 
char mystr[]="STABLE";
char mystr2[]="NTSTBL";
const int gledPin =  9;      // the number of the LED pin
const int rledPin =  8; 
String ank;
int Lov;
char recstr[5]; //Initialized variable to store recieved data

SoftwareSerial mySerial(0, 1);//(Rx,Tx)

String data;
SoftwareSerial esp(2, 3);// RX & TX For ESP-01
 
//*/
///* For Lakehead
const char* ssid = "Cisco08028";
const char* password = "mobile13WIFI";
String server1 = "192.168.1.210";
String uri = "192.168.1.1";
WiFiServer server(80);
IPAddress ip(192, 168, 1, 210); // where xx is the desired IP Address
IPAddress gateway(192, 168, 1, 1); // set gateway to match your network

// */  
byte dat [5];

String temp ,hum;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  // set the digital pin as output:
  pinMode(rledPin, OUTPUT);
  pinMode(gledPin, OUTPUT); 
  IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network
//  WiFi.config(ip, gateway, subnet); 
  //WiFi.begin(ssid, password);
  esp.begin(115200);
  reset();

connectWifi();
}
//reset the esp8266 module

void reset() {

esp.println("AT+RST");

delay(1000);

if(esp.find("OK") ) esp.println("Module Reset");

}

//connect to your wifi network

void connectWifi() {

String cmd = "AT+CWJAP=\"" +ssid+"\",\"" + password + "\"";

esp.println(cmd);

delay(4000);

if(esp.find("OK")) {

esp.println("Connected!");

}

else {

connectWifi();

esp.println("Cannot connect to wifi"); }

} 

void loop() {
 // dat[5]=recstr[5];
      digitalWrite(rledPin, LOW);
      digitalWrite(gledPin, LOW); 
  int i=0;
  mySerial.listen();
  if (mySerial.available()) {
    delay(1000); //allows all serial sent to be received together
    while(mySerial.available() && i<3) {
      recstr[i++] = mySerial.read();
    }
    recstr[i++]='\0';
  }
   if(i>0){
    ank=String(recstr);
    Lov=ank.toInt(); 
    
hum = String(dat[0]);

temp= String(dat[2]);
data = "temperature=" + temp + "&humidity=" + hum;
httppost(); 
    //Serial.print(Lov);
    if((Lov>=18)&&(Lov<=25)) {  
      Serial.write(mystr,7); 
      Serial.write('\n');
      digitalWrite(rledPin, LOW);
      digitalWrite(gledPin, HIGH); 
      delay(500);
      
    }
   else if((Lov<18)||(Lov>25)){ 
      Serial.write(mystr2,7); 
      Serial.write('\n');
      digitalWrite(rledPin, HIGH);
      digitalWrite(gledPin, LOW); 
      delay(500);
    }
   }

}
void httppost () {

esp.println("AT+CIPSTART=\"TCP\",\"" + server1 + "\",80");//start a TCP connection.

if( esp.find("OK")) {

esp.println("TCP connection ready");

} delay(1000);

String postRequest =

"POST " + uri + " HTTP/1.0\r\n" +

"Host: " + server1 + "\r\n" +

"Accept: *" + "/" + "*\r\n" +

"Content-Length: " + data.length() + "\r\n" +

"Content-Type: application/x-www-form-urlencoded\r\n" +

"\r\n" + data;

String sendCmd = "AT+CIPSEND=";//determine the number of caracters to be sent.

esp.print(sendCmd);

esp.println(postRequest.length() );

delay(500);

if(esp.find(">")) {  esp.print(postRequest);

if( esp.find("SEND OK")) { 

while (esp.available()) {

String tmpResp = esp.readString();

esp.println(tmpResp);

}

// close the connection

esp.println("AT+CIPCLOSE");

}

}}
