#define TS_ENABLE_SSL

#include <WiFiClientSecure.h>
#include <ThingSpeak.h>
#include <DHT.h>
#include "mqtt_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
WiFiClientSecure  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char* myWriteAPIKey = SECRET_WRITE_APIKEY;
const char* myReadAPIKey = SECRET_READ_APIKEY;
const char* certificate = SECRET_TS_ROOT_CA;

unsigned long lastTime = 0;
unsigned long timerDelay = 20000;

float t, h;
float field[8] = {1,2,3,4,5,6,7,8};


#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup(){
  Serial.begin(115200);
  
  dht.begin();
  
  WiFi.mode(WIFI_STA);

  client.setCACert(certificate);

  ThingSpeak.begin(client); 
}

void connectWiFi() {
  if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, pass); 
        Serial.print(".");
        delay(5000);     
      } 
      Serial.println("\nConnected.");
    }
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    
    // Connect or reconnect to WiFi
    connectWiFi();

    Serial.println("==========================Publish==========================");

    // Get a new temperature reading
    t = dht.readTemperature();
    Serial.print("Temperature (ºC): ");
    Serial.println(t);

    h = dht.readHumidity();
    Serial.print("Humidity (%): ");
    Serial.println(h);

    ThingSpeak.setField(1, t);
    ThingSpeak.setField(2, h);
     
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if(x == 200){
      Serial.println("Channel update successful.");
    } else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    Serial.println();
    Serial.println("===========================================================");
    Serial.println("==========================Subcribe=========================");

    int statusCode = ThingSpeak.readMultipleFields(myChannelNumber, myReadAPIKey);
    
    if(statusCode == 200)
    {
      float temp = ThingSpeak.getFieldAsFloat(field[0]); // Field 1
      float humid = ThingSpeak.getFieldAsFloat(field[1]); // Field 2  
      String createdAt = ThingSpeak.getCreatedAt();
      Serial.println("Subcribed Temperature (ºC): " + String(temp));
      Serial.println("Subcribed Humidity (%): " + String(humid));
      Serial.println("Created at, if any (YYYY-MM-DD hh:mm:ss): " + createdAt);
    } else{
      Serial.println("Problem reading channel. HTTP error code " + String(statusCode)); 
    }
    Serial.println("\n");
    lastTime = millis();
  }
}
