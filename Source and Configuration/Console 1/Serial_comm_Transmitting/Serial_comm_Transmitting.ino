#include <SoftwareSerial.h>
#include <dht11.h>
dht11 DHT11;
#define DHT11PIN 8

//RF24 radio(5, 10); // CE, CSN
//const byte address[6] = "00001";
const int gledPin =  6;      // the number of the LED pin
const int rledPin =  7;
const int bledPin =  9;
String sbh;
char mystr[100];//String data
char recstr[10]; //Initialized variable to store recieved data
int temp;
SoftwareSerial mySerialT(0, 1);//(Rx,Tx)

void setup() {
  Serial.begin(9600);
  mySerialT.begin(9600);
// Begin the Serial at 9600 Baud

DHT11.read(DHT11PIN); 
pinMode(rledPin, OUTPUT);
pinMode(gledPin, OUTPUT); 
pinMode(bledPin, OUTPUT); 
}

void loop() {
      digitalWrite(rledPin, LOW);
      digitalWrite(gledPin, LOW);      
      digitalWrite(bledPin, LOW); 
temp=DHT11.temperature;
itoa(temp, mystr,10); 
Serial.write(mystr,2); 
Serial.print('\n');
delay(1000);
 int i=0;
 mySerialT.listen();
 if (mySerialT.available()) {
  delay(1000);
  while(mySerialT.available() && i<7) {
    recstr[i++] = mySerialT.read(); 
  }
  recstr[i++]='\0';
  
 }
 if(i>0){
  sbh=String(recstr);
  //Serial.print(sbh);
  if(sbh.equals("STABLE")){
      digitalWrite(rledPin, LOW);
      digitalWrite(gledPin, HIGH);
      digitalWrite(bledPin, LOW); 
      delay(500);
  }
    if(sbh.equals("NTSTBL")){
      digitalWrite(rledPin, HIGH);
      digitalWrite(gledPin, LOW); 
      digitalWrite(bledPin, LOW);
      delay(500);    
  }
    if((!(sbh.equals("STABLE")))||((!(sbh.equals("NTSTBL"))))){
      digitalWrite(rledPin, LOW);
      digitalWrite(gledPin, LOW); 
      digitalWrite(bledPin, HIGH);
      delay(500);    
  }

 }
}
