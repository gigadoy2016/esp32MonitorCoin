#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include "connection.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>


TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
// TFT Display 135x240
unsigned long period = 5000; //ระยะเวลาที่ต้องการรอ
unsigned long last_time = 0; //ประกาศตัวแปรเป็น global เพื่อเก็บค่าไว้ไม่ให้ reset จากการวนloop
int _count = 0; 
float KUB_COIN =0;
float BTC_COIN =0;

const String bitkubAPI = "https://api.bitkub.com/api/market/ticker";

void setup(void) {
  Serial.begin(115200);
  tft.init();       // Start TFT
  loadTemplate();   // Load Template
  connectionWIFI(WLAN_SSID,WLAN_PASS);
}
void loop(){

  if( millis() - last_time >= period) {
    last_time = millis(); //เซฟเวลาปัจจุบันไว้เพื่อรอจนกว่า millis() จะมากกว่าตัวมันเท่า period 
    Serial.println("Start process Get Coin...");
    
    HTTPClient http;
    http.begin(bitkubAPI);              //Specify the URL
    int httpCode = http.GET();            //Make the request
    if (httpCode > 0) {                   //Check for the returning code
      String payload = http.getString();
      displayCoin(payload);
      tft.setTextColor(TFT_GREEN, TFT_WHITE);
      tft.drawString("        Connected  ",85,125,1);
    }else{
      Serial.println("Error! Can not get datas from API..!");
      tft.setTextColor(TFT_RED, TFT_WHITE);
      tft.drawString("ERROR!! Connection Fail",70,125,1);
      connectionWIFI(WLAN_SSID,WLAN_PASS);
    }
  }
}

//===============================================
//               Display Layout.
//===============================================
void loadTemplate(){
  // TFT Display 240x135
  tft.setRotation(3);
  tft.fillScreen(TFT_WHITE);
  //tft.drawPixel(128,210,TFT_WHITE);       //drawPixel(x, y, coloer)
  tft.fillRect(0, 0, 55, 135,TFT_BLACK);    //(xStart, yStart, width, height);
  tft.fillRect(210, 0, 50, 135,TFT_BLACK);
  tft.fillRect(220, 0, 40, 135,TFT_GREEN);
  
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawString("Mon 27 December 2021",57,2,1);
  //tft.drawString("08:59AM",57,10,2);
}
//===============================================
//               Display Coin.
//===============================================
void displayCoin(String txt){
  DynamicJsonDocument doc(8192);
  deserializeJson(doc, txt);
      
  // get the JsonObject in the JsonDocument
  JsonObject KUB = doc["THB_KUB"].as<JsonObject>();
  JsonObject BTC = doc["THB_BTC"].as<JsonObject>();
  
  Serial.println("=======================================");
  const float kub = KUB["last"];
  const float btc = BTC["last"];
  Serial.println(kub);
  Serial.println(btc);
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("KUB",1,40,4);
  tft.drawString("BTC",1,80,4);

  Serial.println(String(kub)+" = "+KUB_COIN);
  if(kub < KUB_COIN){
    tft.setTextColor(TFT_BLACK, TFT_RED);
    Serial.println("RED");
  }else if(kub > KUB_COIN){
    tft.setTextColor(TFT_BLACK, TFT_GREEN);
    Serial.println("GREEN");
  }else{
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
  }
  tft.drawString(String(kub, 2),57,40,4);


  if(btc < BTC_COIN){
    tft.setTextColor(TFT_BLACK, TFT_RED);
  }else if(btc > BTC_COIN){
    tft.setTextColor(TFT_BLACK, TFT_GREEN);
  }else{
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
  }
  tft.drawString(String(btc, 2),57,80,4);
 
  KUB_COIN = kub;
  BTC_COIN = btc;
}
