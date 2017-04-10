#include <ESP8266WiFi.h>
#include "WeatherClient.h"
#include <Ticker.h>
#include "SSD1306.h"
#include "icons.h"
#include <PubSubClient.h>
#include "SetupGBot.h"

SetupGBot GBot;

// Initialize the oled display for address 0x3c
// sda-pin=GPIO0 and scl-pin=GPIO2
SSD1306  display(0x3c, 4, 5);  // SDA:D2,SCL:D1
WeatherClient weather;
Ticker ticker;

const char *mqttServ = "Server";
const int   mqttPort = 0000;//port
const char *mqttUser = "user";
const char *mqttPass = "pass";

const char *subTopic = "/Location";  // subscribe 

// flag changed in the ticker function every 10 minutes
bool readyForWeatherUpdate = true;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  client.setServer(mqttServ, mqttPort);
  client.setCallback(callback);
  display.init();
  GBot.Setup();
  ticker.attach(60 * 10, setReadyForWeatherUpdate);
  setReadyForWeatherUpdate();
  display.clear();
  display.display();
}

void callback(char* topic, byte* payload, unsigned int length) {
  String readString="";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    char msgIN=((char)payload[i]);
    Serial.print(msgIN);
    readString += msgIN;
  }
  String msg1="";
  String msg2="";
  String msg3="";
  int lastIndex=0;
  int i=0;
  int size=readString.length();
  for (i = 0; i < readString.length(); i++) {
    if (readString.substring(i, i+1) == ",") {
      msg1 = readString.substring(0, i);
      lastIndex=i+1;
       for (i = lastIndex; i < readString.length(); i++) {
        if (readString.substring(i, i+1) == ","){
          msg2 = readString.substring(lastIndex, i);
          msg3 = readString.substring(i+1,size);
        }
       }
       break;
    }
  }
 GBot.ChangeLocationsFromCallBack(msg1,msg2,msg3);
 setReadyForWeatherUpdate();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(subTopic, mqttUser, mqttPass)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe(subTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if(WiFi.status() != WL_CONNECTED){
    DisplayLostConnect();
  }
  if(WiFi.status() == WL_CONNECTED){
     if (!client.connected()) {
      reconnect();
     }
    client.loop();
    display.clear();
    if (readyForWeatherUpdate==true)// update the weather information every 10 mintues becaue forecast.io only allows 1000 calls per day
    {
      readyForWeatherUpdate = false;
      weather.updateWeatherData(GBot.forecastApiKey, GBot.latitude, GBot.longitude);
    }
    State1();
    delay(2000);
    State2();
    delay(1500);
    State3();
    delay(2000);
    State4();
    delay(1500);
  }
  if(Serial.available()){
    display.clear();
    display.display();
    GBot.Recieve();
    setReadyForWeatherUpdate();
  }
}

void setReadyForWeatherUpdate() {
  readyForWeatherUpdate = true;  
}

void State1() {
   display.drawString(65 , 0 , "Today");
   display.drawXbm(0,0, 50, 50, getIconFromString(weather.getCurrentIcon()));
   display.setFont(ArialMT_Plain_16);
   display.drawString(64 , 14 , String(weather.getCurrentTemp()) + "C");
   display.setFont(ArialMT_Plain_16);
   display.drawString(64 , 30 , String(weather.getMinTempToday()) + "C/" + String(weather.getMaxTempToday()) + "C");
   display.setFont(ArialMT_Plain_10);
   display.drawString(15 , 48 , GBot.location);
   display.display();
}

const char* getIconFromString(String icon) {
  if (icon == "clear-day") {
    return clear_day_bits;
  } 
  else if (icon == "clear-night") {
    return clear_night_bits;  
  } 
  else if (icon == "rain") {
    return rain_bits;  
  } 
  else if (icon == "snow") {
    return snow_bits;  
  } 
  else if (icon == "sleet") {
    return sleet_bits;  
  } 
  else if (icon == "wind") {
    return wind_bits;  
  } 
  else if (icon == "fog") {
    return fog_bits;  
  } 
  else if (icon == "cloudy") {
    return cloudy_bits;  
  } 
  else if (icon == "partly-cloudy-day") {
    return partly_cloudy_day_bits;  
  } 
  else if (icon == "partly-cloudy-night") {
    return partly_cloudy_night_bits;  
  } 
  return cloudy_bits;
}

void State2() {
  display.clear();
  display.drawString(0 , 0 ,"Today");
  if (getIconFromString(weather.getCurrentIcon())==clear_day_bits) {
    display.drawXbm(0,0, 128, 64,happy_Logo_bits); 
  } 
  else if (getIconFromString(weather.getCurrentIcon()) == clear_night_bits) {
    display.drawXbm(0,0, 128, 64,love_bits);   
  } 
  else if (getIconFromString(weather.getCurrentIcon()) == rain_bits) {
    display.drawXbm(0,0, 128, 64,angry_bits);  
  } 
  else if (getIconFromString(weather.getCurrentIcon()) == snow_bits) {
    display.drawXbm(0,0, 128, 64,love_bits);   
  } 
  else if (getIconFromString(weather.getCurrentIcon()) == sleet_bits) {
    display.drawXbm(0,0, 128, 64,angry_bits);   
  } 
  else if (getIconFromString(weather.getCurrentIcon()) == wind_bits) {
    display.drawXbm(0,0, 128, 64,normal_Logo_bits);   
  } 
  else if (getIconFromString(weather.getCurrentIcon()) == fog_bits) {
    display.drawXbm(0,0, 128, 64,bored_bits);   
  } 
  else if (getIconFromString(weather.getCurrentIcon()) == cloudy_bits) {
    display.drawXbm(0,0, 128, 64,normal_Logo_bits);   
  } 
  else if (getIconFromString(weather.getCurrentIcon()) == partly_cloudy_day_bits) {
    display.drawXbm(0,0, 128, 64,what_bits);  
  } 
  else if (getIconFromString(weather.getCurrentIcon()) == partly_cloudy_night_bits) {
    display.drawXbm(0,0, 128, 64,what_bits);  
  } 
  display.display();  
}

void State3() {
   display.clear();
   display.drawString(60 , 0 ,"Tomorrow");
   display.drawXbm(0,0, 50, 50, getIconFromString(weather.getIconTomorrow()));
   display.setFont(ArialMT_Plain_16);
   display.drawString(59 , 14 , String(weather.getSummaryTomorrow()));
   display.setFont(ArialMT_Plain_16);
   display.drawString(59 , 30 , String(weather.getMinTempTomorrow()) + "C/" + String(weather.getMaxTempTomorrow()) + "C");
   display.setFont(ArialMT_Plain_10);
   display.drawString(15 , 48 , GBot.location);
   display.display();
}

void State4() {
  display.clear();
  display.drawString(0 , 0 ,"Tomorrow");
  if (getIconFromString(weather.getIconTomorrow())==clear_day_bits) {
    display.drawXbm(0,0, 128, 64,happy_Logo_bits); 
  } 
  else if (getIconFromString(weather.getIconTomorrow()) == clear_night_bits) {
    display.drawXbm(0,0, 128, 64,love_bits);   
  } 
  else if (getIconFromString(weather.getIconTomorrow()) == rain_bits) {
    display.drawXbm(0,0, 128, 64,angry_bits);  
  } 
  else if (getIconFromString(weather.getIconTomorrow()) == snow_bits) {
    display.drawXbm(0,0, 128, 64,love_bits);   
  } 
  else if (getIconFromString(weather.getIconTomorrow()) == sleet_bits) {
    display.drawXbm(0,0, 128, 64,angry_bits);   
  } 
  else if (getIconFromString(weather.getIconTomorrow()) == wind_bits) {
    display.drawXbm(0,0, 128, 64,normal_Logo_bits);   
  } 
  else if (getIconFromString(weather.getIconTomorrow()) == fog_bits) {
    display.drawXbm(0,0, 128, 64,bored_bits);   
  } 
  else if (getIconFromString(weather.getIconTomorrow()) == cloudy_bits) {
    display.drawXbm(0,0, 128, 64,normal_Logo_bits);   
  } 
  else if (getIconFromString(weather.getIconTomorrow()) == partly_cloudy_day_bits) {
    display.drawXbm(0,0, 128, 64,what_bits);  
  } 
  else if (getIconFromString(weather.getIconTomorrow()) == partly_cloudy_night_bits) {
    display.drawXbm(0,0, 128, 64,what_bits);  
  } 
  display.display();  
}

void DisplayLostConnect()
{
  display.clear();
  display.drawXbm(0, 0, wifi_disconnect_width, wifi_disconnect_height, wifi_disconnect_bits );
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(35, 10,"Failed to");
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(35, 40,"Connect Wifi");
  display.display();
  
}

