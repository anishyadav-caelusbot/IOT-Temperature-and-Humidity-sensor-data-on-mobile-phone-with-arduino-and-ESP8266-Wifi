#include <dht.h>UU



#include <SoftwareSerial.h> //include the software serial library

dht DHT; // initiate the DHT obeject
int a; // a variable to store the raw sensor value
String temp; // Variable to store the temperature
String humid; // Variable to store the humidity




SoftwareSerial esp8266(3, 4); //set the software serial pins RX pin = 3, TX pin = 4 
//definition of variables
#define DEBUG true //show messages between ESP8266 and Arduino in serial port, when set to true
#define SSID "JioFi3_03C4FF" //replace x with your wifi network name
#define PASS "p7yzzkwdk4" //replace x with your wifi network password


String sendAT(String command, const int timeout, boolean debug)
{
  String response = "";
  esp8266.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      char c = esp8266.read();
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
  return response;
}

void setup()
{

  Serial.begin(9600);// begin the serial communication with baud of 9600
  esp8266.begin(9600);// begin the software serial communication with baud rate 9600
  
  sendAT("AT+RST\r\n", 2000, DEBUG); // call sendAT function to send reset AT command
  sendAT("AT\r\n", 1000, DEBUG);
  sendAT("AT+CWMODE=1\r\n", 1000, DEBUG); //call sendAT function to set ESP8266 to station mode
  sendAT("AT+CWJAP=\""SSID"\",\""PASS"\"\r\n", 2000, DEBUG); //AT command to connect wit the wifi network

  while(!esp8266.find("OK")) { //wait for connection
  } 
  sendAT("AT+CIFSR\r\n", 1000, DEBUG); //AT command to print IP address on serial monitor
  sendAT("AT+CIPMUX=0\r\n", 1000, DEBUG); //AT command to set ESP8266 to multiple connections

  
}

void loop(){
  /// put your main code here, to run repeatedly:
  a=DHT.read11(5); // read the sensor data and store it in variable a
  temp=DHT.temperature; // store the temperature values in temp variable
  humid=DHT.humidity;// store the humidity values in humid variable
  Serial.print("Temperature :"); // print the temperature on serial monitor
  Serial.println(temp); // Print the temperature value
  Serial.print("Humidity : ");//Print Humidity on serial monitor
  Serial.print(humid); // Print the humidity value on serial monitor
  Serial.print(" \n "); 
  updateTS(temp,humid); // call the function to update Thingspeak channel
  delay(3000);
}



void updateTS(String T,String P){
  Serial.println("");
  sendAT("AT+CIPSTART=\"TCP\",\"https://api.thingspeak.com/apps/thinghttp/send_request?api_key=J9VC3S14SH7PMEUA\",80\r\n", 1000, DEBUG);    
  delay(2000);
  String cmdlen;
  String cmd="GET /update?key=V1MEHO8T1ZXEQZ7F&field1="+T+"&field2="+P+"\r\n"; // update the temprature and humidity values on thingspeak url,replace xxxxxxx with your write api key
   cmdlen = cmd.length();
  sendAT("AT+CIPSEND="+cmdlen+"\r\n", 2000, DEBUG);
   esp8266.print(cmd);
   Serial.println("");
   sendAT("AT+CIPCLOSE\r\n", 2000, DEBUG);
   Serial.println("");
   delay(15000);
   }




