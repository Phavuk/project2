#include <Arduino.h>
#include <SocketIoClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define USE_SERIAL Serial
#define interval 15000

#define waterSurface A0

unsigned long previousMillis = 0;

SocketIoClient webSocket;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void setup() {
    USE_SERIAL.begin(115200);

    USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

      for(uint8_t t = 4; t > 0; t--) {
          USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
          USE_SERIAL.flush();
          delay(1000);
      }

    //Connection to WIFI   
    const char* ssid     = "MI";
    const char* password = "heslo123456789";
    
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    //establish time client
    timeClient.begin();
    timeClient.setTimeOffset(3600);

    //establish socketIO connection
    webSocket.begin("192.168.1.109", 5485, "/socket.io/?transport=websocket");
    webSocket.on("pourFlower", pourFlower);
}

//function for WIFI reconection
void wifiConection() {
 const char* ssid     = "MI";
 const char* password = "heslo123456789";
 
 if (WiFi.status() != WL_CONNECTED) {
    
    WiFi.begin(ssid, password);
    delay(500);
    Serial.print(".");
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("");
      Serial.println("WiFi connected.");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());

      timeClient.begin();
      timeClient.setTimeOffset(3600);
    }
  }
}

////////////////////////////////////////// MEASURE DATA ////////////////////////////////////////////
void measureData() {
  String id = "kgj45as";
  float temperature = 50; 
  float humidityAir = 25.5;
  float humiditySoil = 65.5;
  float waterSurface = getWaterSurface();

  createJson(temperature, humidityAir, humiditySoil, waterSurface, id);
}

float getWaterSurface() {
  return analogRead(waterSurface);
}

String getDate() {
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  
  String formattedDate = timeClient.getFormattedDate();
  int splitT = formattedDate.indexOf("T");
  String dayStamp = formattedDate.substring(0, splitT);
  String timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  String date = dayStamp +" "+ timeStamp;

  return date;
}

//////////////////////////////////////// JSON Creation /////////////////////////////////////////////////

void createJson(float temperature, float humidityAir, float humiditySoil, float waterSurface, String id) {
    const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4);
    DynamicJsonBuffer jsonBuffer(capacity);
    
    JsonObject& root = jsonBuffer.createObject();
    
    JsonObject& identification = root.createNestedObject("identification");
    identification["id"] = id;
    
    JsonObject& info = root.createNestedObject("info");
    info["temperature"] = temperature;
    info["humidityAir"] = humidityAir;
    info["humiditySoil"] = humiditySoil;
    info["watersurface"] = waterSurface;
    
    JsonObject& date = root.createNestedObject("date");
    date["date"] = getDate();

    root.printTo(Serial);
    
    char output[capacity];
    root.printTo(output);
    sendData(output);
}

//////////////////////////////////////////// SOCKETIO EVENTS //////////////////////////////////

void sendData(char* output) {
    webSocket.emit("arduinoData", output);
    USE_SERIAL.println("Data sended.");
}

void pourFlower(const char * payload, size_t length) {
  USE_SERIAL.println("Data prijate");
}

/////////////////////////////////////////////// LOOP ///////////////////////////////////////////
void loop() {
    webSocket.loop();
    wifiConection();

    if (millis() - previousMillis >= interval) {
      previousMillis = millis();    
      measureData();
    }
}
