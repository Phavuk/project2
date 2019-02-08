#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <SocketIoClient.h>
#include <ArduinoJson.h>

#define USE_SERIAL Serial

WiFiMulti WiFiMulti;
SocketIoClient webSocket;

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

    WiFiMulti.addAP("SOS-1N1", "globallogic54");

    while(WiFiMulti.run() != WL_CONNECTED) {
        
        delay(100);
    }

    webSocket.begin("192.168.2.105", 5485, "/socket.io/?transport=websocket");
    

    const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4);
    DynamicJsonBuffer jsonBuffer(capacity);
    
    JsonObject& root = jsonBuffer.createObject();
    
    JsonObject& identification = root.createNestedObject("identification");
    identification["id"] = "sdf256s5df";
    identification["plantname"] = "MyPlant1";
    
    JsonObject& info = root.createNestedObject("info");
    info["temperature"] = "24";
    info["humidityAir"] = "25";
    info["humiditySoil"] = "32";
    info["watersurface"] = "10";
    JsonObject& date = root.createNestedObject("date");
    date["date"] = "2019.12.25 14:17:54";
    
    root.printTo(Serial);

    char output[capacity];
    root.printTo(output);
    
    webSocket.emit("arduinoData", output);
}

void loop() {
    webSocket.loop();
}
