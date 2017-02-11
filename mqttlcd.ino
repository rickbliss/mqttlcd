/**
 * \file
 *       ESP8266 MQTT Bridge example
 * \author
 *       Tuan PM <tuanpm@live.com>
 */
#include <SoftwareSerial.h>
#include <espduino.h>
#include <mqtt.h>
#include <LiquidCrystal.h>
//Changed 2,3 to 7,8
//Changed 4 to 9

SoftwareSerial debugPort(7, 8); // RX, TX
LiquidCrystal lcd(12, 11, 10, 5, 4, 3, 2);

ESP esp(&Serial, &debugPort, 9);
MQTT mqtt(&esp);
boolean wifiConnected = false;
boolean l12 = false;
void wifiCb(void* response)
{
  uint32_t status;
  RESPONSE res(response);
  boolean l12 = false;
  if(res.getArgc() == 1) {
    res.popArgs((uint8_t*)&status, 9);
    if(status == STATION_GOT_IP) {
      debugPort.println("WIFI CONNECTED");
      mqtt.connect("rickbliss.strangled.net", 1883);
      wifiConnected = true;
      //or mqtt.connect("host", 1883); /*without security ssl*/
    } else {
      wifiConnected = false;
      mqtt.disconnect();
    }
    
  }
}

void mqttConnected(void* response)
{
  debugPort.println("Connected");
  mqtt.subscribe("#"); //or mqtt.subscribe("topic"); /*with qos = 0*/

  mqtt.publish("/ESP8266Thing", "Connected.");

}
void mqttDisconnected(void* response)
{

}
void mqttData(void* response)
{
  RESPONSE res(response);
  
  //debugPort.print("Received: topic=");
  String topic = res.popString();
  //debugPort.println(topic);
  lcd.clear();
  lcd.print(topic); 
  //debugPort.print("data=");
  lcd.setCursor(0,1);
   
  
  
  String data = res.popString();
  lcd.println(data+"L"+data.length());
  pinMode(12,OUTPUT);
  if (data == "hi"){
    if (l12 == false){
;
      mqtt.publish("/ESP8266Thing", "HIGH");
    
    digitalWrite(12,HIGH);
    digitalWrite(13,LOW);
    l12 = true;
  } else {
    
      digitalWrite(12,LOW);
    digitalWrite(13,LOW);
    mqtt.publish("/ESP8266Thing", "LOW");
    l12 = false;
  }
 
  }
  debugPort.println(data);

}
void mqttPublished(void* response)
{

}
void setup() {
   lcd.begin(16, 2);
  debugPort.begin(19200);
  debugPort.println("Starting SS");
 Serial.begin(19200);
 //   Serial.println("HW Serial");
  esp.enable();
  delay(500);
  esp.reset();
  delay(500);
  while(!esp.ready());

  debugPort.println("ARDUINO: setup mqtt client");
  if(!mqtt.begin("DVES_duino", "admin", "Isb_C4OGD4c3", 20, 1)) {
    debugPort.println("ARDUINO: fail to setup mqtt");
    while(1);
  }


  debugPort.println("ARDUINO: setup mqtt lwt");
  mqtt.lwt("/lwt", "offline", 0, 0); //or mqtt.lwt("/lwt", "offline");

/*setup mqtt events */
  mqtt.connectedCb.attach(&mqttConnected);
  mqtt.disconnectedCb.attach(&mqttDisconnected);
  mqtt.publishedCb.attach(&mqttPublished);
  mqtt.dataCb.attach(&mqttData);

  /*setup wifi*/
  debugPort.println("ARDUINO: setup wifi");
  lcd.println("ARDUINO: setup wifi");
  esp.wifiCb.attach(&wifiCb);

  esp.wifiConnect("bliss","sexmonkey");


  debugPort.println("ARDUINO: system started");
  lcd.print("ARDUINO: system started");
}

void loop() {
  esp.process();
  if(wifiConnected) {

  }
}
