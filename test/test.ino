#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <SocketIoClient.h>
#include <ArduinoJson.h>

#define USE_SERIAL Serial

WiFiMulti WiFiMulti;
SocketIoClient webSocket;

void event(const char * payload, size_t length) {
  USE_SERIAL.printf("got message: %s\n", payload);
}

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

    WiFiMulti.addAP("dlink", "Pavuk2018");

    while(WiFiMulti.run() != WL_CONNECTED) {
        
        delay(100);
    }

    webSocket.begin("192.168.0.114", 5485, "/socket.io/?transport=websocket");
    webSocket.emit("sendData","\"Test data\"");

    const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4);
    DynamicJsonBuffer jsonBuffer(capacity);
    
    JsonObject& root = jsonBuffer.createObject();
    
    JsonObject& identification = root.createNestedObject("identification");
    identification["id"] = "sdf256s5df";
    identification["plantname"] = "MyPlant1";
    
    JsonObject& info = root.createNestedObject("info");
    info["temperature"] = "24";
    info["humidityAir"] = "25";
    info["humiditySoil"] = "32%";
    info["watersurface"] = "10";
    JsonObject& date = root.createNestedObject("date");
    date["date"] = "26.1.2019";
    
    root.printTo(Serial);
}

void loop() {
    webSocket.loop();
}
