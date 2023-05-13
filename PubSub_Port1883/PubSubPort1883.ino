#include <WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>

const char* ssid = "SV";
const char* password = "44444444";

WiFiClient  client;

unsigned long myChannelNumber = 2145294;
const char* myWriteAPIKey = "7CYI9JM8QLYVMNKG";
const char* myReadAPIKey = "3FLL0BL3P5SYGDLG";

unsigned long lastTime = 0;
unsigned long timerDelay = 20000;

int statusCode = 0;
float t, h;
float field[2] = {1,2};

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup(){
  Serial.begin(115200);
  dht.begin();
  
  WiFi.mode(WIFI_STA);

  ThingSpeak.begin(client); 
}

void connectWiFi() {
  if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password); 
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
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    Serial.println();
    Serial.println("===========================================================");
    Serial.println("==========================Subcribe=========================");

    statusCode = ThingSpeak.readMultipleFields(myChannelNumber, myReadAPIKey);
    
    if(statusCode == 200)
    {
      float temp = ThingSpeak.getFieldAsInt(field[0]); // Field 1
      float humid = ThingSpeak.getFieldAsFloat(field[1]); // Field 2  
      Serial.println("Subcribed Temperature (ºC): " + String(temp));
      Serial.println("Subcribed Humidity (%): " + String(humid));
    } else{
      Serial.println("Problem reading channel. HTTP error code " + String(statusCode)); 
    }
    Serial.println("\n");
    lastTime = millis();
  }
}
