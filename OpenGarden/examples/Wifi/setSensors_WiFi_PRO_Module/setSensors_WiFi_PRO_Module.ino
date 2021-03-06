/*  
 *  OpenGarden sensor platform for Arduino from Cooking-hacks.
 *  
 *  Copyright (C) Libelium Comunicaciones Distribuidas S.L. 
 *  http://www.libelium.com 
 *  
 *  This program is free software: you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation, either version 3 of the License, or 
 *  (at your option) any later version. 
 *  
 *  This program is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License 
 *  along with this program.  If not, see http://www.gnu.org/licenses/. 
 *  
 *  Version:           2.2
 *  Design:            David Gascón 
 *  Implementation:    Victor Boria & Jorge Casanova
 */

#include <OpenGarden.h>
#include <Wire.h> 

//Enter here your data
const char server[] = "http://YOUR_SERVER_IP";
const char server_port[] = "YOUR_SERVER_PORT";
const char wifi_ssid[] = "YOUR_WIFI_SSID";
const char wifi_password[] = "YOUR_WIFI_PASSWORD";
const char GET[] = "opengarden/set_sensors.php?data=";

String wifiString;
char buffer[20];
int flag = 0; // auxiliar variable
DateTime myTime;

char aux_str[90];
int8_t answer;
char response[300];

void setup() {
  Serial.begin(115200);
  OpenGarden.initSensors();    //Initialize sensors power 
  OpenGarden.sensorPowerON();  //Turns on the sensor power supply
  delay(100);
  OpenGarden.initRF();
  OpenGarden.initRTC();
  wifireset(); 
  wificonfig(); 
}


void loop() {

  OpenGarden.receiveFromNode(); //Receive data from node
  myTime = OpenGarden.getTime();
  OpenGarden.printTime(myTime);  
  Serial.println(" ");

  if ( myTime.minute() == 0 && flag == 0  ){ //Only enter 1 time each hour 

    //Get Gateway Sensors
    int soilMoisture0 = OpenGarden.readSoilMoisture();
    float airTemperature0 = OpenGarden.readAirTemperature();
    float airHumidity0 = OpenGarden.readAirHumidity();
    float soilTemperature0 = OpenGarden.readSoilTemperature();
    int luminosity0 = OpenGarden.readLuminosity();

    //Get Node Sensors
    Payload node1Packet = OpenGarden.getNodeData(node1); 
    Payload node2Packet = OpenGarden.getNodeData(node2);
    Payload node3Packet = OpenGarden.getNodeData(node3); 


    int soilMoisture1 = node1Packet.moisture;
    float airTemperature1 = node1Packet.temperature;
    float airHumidity1 = node1Packet.humidity;
    int luminosity1 = node1Packet.light;
    int battery1 = node1Packet.supplyV;

    int soilMoisture2 = node2Packet.moisture;
    float airTemperature2 = node2Packet.temperature;
    float airHumidity2 = node2Packet.humidity;
    int luminosity2 = node2Packet.light;
    int battery2 = node2Packet.supplyV;

    int soilMoisture3 = node3Packet.moisture;
    float airTemperature3 = node3Packet.temperature;
    float airHumidity3 = node3Packet.humidity;
    int luminosity3 = node3Packet.light;
    int battery3 = node3Packet.supplyV;


    //Create string of the floats to send it
    dtostrf(airTemperature0,2,1,buffer);
    String airTemperature0_wf = String (buffer);
    dtostrf(airHumidity0,2,1,buffer);
    String airHumidity0_wf = String (buffer);
    dtostrf(soilTemperature0,2,1,buffer);
    String soilTemperature0_wf = String (buffer);

    dtostrf(airTemperature1,2,1,buffer);
    String airTemperature1_wf = String (buffer);
    dtostrf(airHumidity1,2,1,buffer);
    String airHumidity1_wf = String (buffer);

    dtostrf(airTemperature2,2,1,buffer);
    String airTemperature2_wf = String (buffer);
    dtostrf(airHumidity2,2,1,buffer);
    String airHumidity2_wf = String (buffer);

    dtostrf(airTemperature3,2,1,buffer);
    String airTemperature3_wf = String (buffer);
    dtostrf(airHumidity3,2,1,buffer);
    String airHumidity3_wf = String (buffer);

    /*
   You must create strings with this structure: "node_id:sensor_type:value;node_id2:sensor_type2:value2;...." 
     Note the ";" between different structures
     
     Where node_id:
     0 - Gateway
     1 - Node 1
     2 - Node 2
     3 - Node 3
     
     And where sensor_type:
     0 - Soil moisture 
     1 - Soil temperature
     2 - Air Humidity
     3 - Air Temperature
     4 - Light level 
     5 - Battery level 
     6 - pH level 
     7 - Electrical conductivity
     
     For example: "0:0:56;0:1:17.54;0:2:56.45"
     This means that you send data of the gateway: Soil moisture=56, Soil temperature=17.54 and Air humidity=56.45
     
     */

    //Cut data in several string because the wifi module have problems with longer strings
    wifiString= "0:0:" + String(soilMoisture0) + ";0:1:"  + soilTemperature0_wf + ";0:2:" + airHumidity0_wf; 
    sendGET();


    wifiString= "0:3:"   +  airTemperature0_wf + ";0:4:" + String(luminosity0); 
    sendGET();


    wifiString = "1:0:" + String(soilMoisture1) + ";1:2:" + airHumidity1_wf + ";1:3:"  +  airTemperature1_wf + ";1:4:" + String(luminosity1);
    sendGET();

    
    wifiString = "2:0:" + String(soilMoisture2) + ";2:2:" + airHumidity2_wf + ";2:3:"  +  airTemperature2_wf + ";2:4:" + String(luminosity2);
    sendGET();


    wifiString = "3:0:" + String(soilMoisture3) + ";3:2:" + airHumidity3_wf + ";3:3:"  +  airTemperature3_wf + ";3:4:" + String(luminosity3);
    sendGET();
    
    
    wifiString = "1:5:" +  String(battery1) + ";2:5:" +  String(battery2) + ";3:5:" +  String(battery3); 
    sendGET();


    flag = 1;  

  }

  else if (myTime.minute() == 30 && flag == 1){
    flag = 0;
  } 
}


//*********************************************************************
//*********************************************************************

void sendGET() {
  int str_len = wifiString.length() + 1; 
  char char_array[str_len];
  wifiString.toCharArray(char_array, str_len);

  snprintf(aux_str, sizeof(aux_str), "AT+iRLNK:\"%s:%s/%s%s\"\r", server, server_port, GET, char_array);
  sendCommand(aux_str,"I/OK",20000);
}


//**********************************************
void wifireset() {
  
  Serial.println(F("Setting Wifi parameters"));
  sendCommand("AT+iFD\r","I/OK",2000);
  delay(1000);  
  sendCommand("AT+iDOWN\r","I/OK",2000);
  delay(6000);  
}


//**********************************************
void wificonfig() {
  
  Serial.println(F("Setting Wifi parameters"));
  sendCommand("AT+iFD\r","I/OK",2000);
  delay(2000);  

  snprintf(aux_str, sizeof(aux_str), "AT+iWLSI=%s\r", wifi_ssid);
  answer = sendCommand(aux_str,"I/OK",10000);

  if (answer == 1){

    snprintf(aux_str, sizeof(aux_str), "Joined to \"%s\"", wifi_ssid);
    Serial.println(aux_str);
    delay(5000);
  }

  else {
    snprintf(aux_str, sizeof(aux_str), "Error joining to: \"%s\"", wifi_ssid);
    Serial.println(aux_str);
    delay(1000);
  }

  snprintf(aux_str, sizeof(aux_str), "AT+iWPP0=%s\r", wifi_password);
  sendCommand(aux_str,"I/OK",20000);
  delay(1000);

  if (answer == 1){

    snprintf(aux_str, sizeof(aux_str), "Connected to \"%s\"", wifi_ssid);
    Serial.println(aux_str);
    delay(5000);
  }

  else {
    snprintf(aux_str, sizeof(aux_str), "Error connecting to: \"%s\"", wifi_ssid);
    Serial.println(aux_str);
    delay(1000);
  }

  sendCommand("AT+iDOWN\r","I/OK",2000);
  delay(6000); 


}


//**********************************************
int8_t sendCommand(const char* Command, const char* expected_answer, unsigned int timeout){

  uint8_t x=0,  answer=0;
  unsigned long previous;

  memset(response, 0, 300);    // Initialize the string

  delay(100);

  while( Serial.available() > 0) Serial.read();    // Clean the input buffer

  Serial.println(Command);    // Send Command 

    x = 0;
  previous = millis();

  // this loop waits for the answer
  do{
    if(Serial.available() != 0){    
      // if there are data in the UART input buffer, reads it and checks for the asnwer
      response[x] = Serial.read();
      x++;
      // check if the desired answer  is in the response of the module
      if (strstr(response, expected_answer) != NULL)    
      {
        answer = 1;
      }
    }
  }
  // Waits for the asnwer with time out
  while((answer == 0) && ((millis() - previous) < timeout));    

  return answer;
}


//**********************************************
