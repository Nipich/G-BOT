
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "SSD1306.h"
#include "icons.h"
#include "SetupGBot.h"

SSD1306  OLED(0x3c, 4, 5); 
SetupGBot::SetupGBot(){
}

boolean SetupGBot::eeprom_is_addr_ok(int addr) {
    return ((addr >= EEPROM_MIN_ADDR) && (addr <= EEPROM_MAX_ADDR));
}

boolean SetupGBot::eeprom_write_bytes(int startAddr, const byte* array, int numBytes) {
// counter
    int i;
// both first byte and last byte addresses must fall within
// the allowed range 
    if (!eeprom_is_addr_ok(startAddr) || !eeprom_is_addr_ok(startAddr + numBytes)) {
      return false;
    }
    for (i = 0; i < numBytes; i++) {
      EEPROM.write(startAddr + i, array[i]);
    }
    EEPROM.commit();
    return true;
}

boolean SetupGBot::eeprom_write_string(int addr, const char* string) {
    int numBytes; // actual number of bytes to be written
    //write the string contents plus the string terminator byte (0x00)
    numBytes = strlen(string) + 1;
    return eeprom_write_bytes(addr, (const byte*)string, numBytes);
}

boolean SetupGBot::eeprom_read_string(int addr, char* buffer, int bufSize) {
    byte ch; // byte read from eeprom
    int bytesRead; // number of bytes read so far
    if (!eeprom_is_addr_ok(addr)) { // check start address
      return false;
    }

    if (bufSize == 0) { // how can we store bytes in an empty buffer ?
     return false;
    }
    // is there is room for the string terminator only, no reason to go further
    if (bufSize == 1) {
      buffer[0] = 0;
      return true;
    }
    bytesRead = 0; // initialize byte counter
    ch = EEPROM.read(addr + bytesRead); // read next byte from eeprom
    buffer[bytesRead] = ch; // store it into the user buffer
    bytesRead++; // increment byte counter
    // stop conditions:
    // - the character just read is the string terminator one (0x00)
    // - we have filled the user buffer
    // - we have reached the last eeprom address
    while ( (ch != 0x00) && (bytesRead < bufSize) && ((addr + bytesRead) <= EEPROM_MAX_ADDR) ) {
      // if no stop condition is met, read the next byte from eeprom
      ch = EEPROM.read(addr + bytesRead);
      buffer[bytesRead] = ch; // store it into the user buffer
      bytesRead++; // increment byte counter
    }
    // make sure the user buffer has a string terminator, (0x00) as its last byte
    if ((ch != 0x00) && (bytesRead >= 1)) {
      buffer[bytesRead - 1] = 0;
    }
    return true;
}

// this setup include only function onConnected from netpie microgear
void SetupGBot::Setup() {
  EEPROM.begin(512);
  // initialize dispaly
  OLED.init();
  OLED.clear();
  OLED.display();

  Serial.println("Starting...");

  Serial.print("Reading string from address 5: ");
  eeprom_read_string(5, buf, 30); //read address 5 store to buf
    
  Serial.print("Reading string from address 36: ");
  eeprom_read_string(36, buf2, 30); //read address 36 store to buf2

  Serial.print("Reading string from address 80: ");
  eeprom_read_string(80, buf3, 15); //read address 80 store to buf3
    
  Serial.print("Reading string from address 90: ");
  eeprom_read_string(96, buf4, 15); //read address 90 store to buf4

  Serial.print("Reading string from address 100: ");
  eeprom_read_string(111, buf5, 30); //read address 100 store to bu5

  Serial.print("Reading string from address 140: ");
  eeprom_read_string(150, buf6, 40); //read address 140 store to buf6

  ssid=buf; //set ssid
  password=buf2; //set password
  latitude = atof(buf3); //set latitude
  longitude = atof(buf4); //set longitude
  location = buf5; //set location
  forecastApiKey=buf6; //set API
  
  
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    OLED.clear();
    OLED.drawXbm(34,25, 60, 36, WiFi_Logo_bits);
    OLED.setFont(ArialMT_Plain_10);
    OLED.setTextAlignment(TEXT_ALIGN_LEFT);
    OLED.drawString(0, 0,"Connecting to:");
    OLED.drawString(68, 0,ssid);
    OLED.display();
    i++;
    if (i==20){
      break;
    }
  }
  Serial.println("");
  
  if(WiFi.status() == WL_CONNECTED){
    OLED.clear();
    OLED.drawXbm(0, 0, wifi_connect_width, wifi_connect_height, wifi_connect_bits );
    OLED.setFont(ArialMT_Plain_16);
    OLED.setTextAlignment(TEXT_ALIGN_LEFT);
    OLED.drawString(20, 25,"Connected !!");
    OLED.display();
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    delay(1000);
  }
  OLED.clear();
  OLED.display();
}
            
void SetupGBot::Recieve() {
        data1  = Serial.readStringUntil(',');
        Serial.read(); //next character is comma, so skip it using this
        data2  = Serial.readStringUntil(',');
        Serial.read();
        data3  = Serial.readStringUntil(',');
        Serial.read();
        data4  = Serial.readStringUntil('\0');
        if (data4=="wifi"){
          Reconnect();
        }
        else if(data4=="Locations"){
          ChangeLocations();
        }
        else if(data4=="api"){
          ChangeAPIKey();
        }
          
 }           

void SetupGBot::Reconnect(){
  WiFi.disconnect();
  const char* ssid=data1.c_str(); 
  const char* password=data2.c_str();
  for (int i = 5; i < 66; i++)
  {
    EEPROM.write(i, 0);   //clear eeprom 
  }
  strcpy(buf, data1.c_str()); //copy ssid to buf
  eeprom_write_string(5, buf);  //write buf to eeprom address 5
  strcpy(buf2, data2.c_str()); //copy password to buf2
  eeprom_write_string(36, buf2); // write buf to eeprom address 36
  WiFi.begin(ssid, password);
  i=0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    OLED.clear();
    OLED.drawXbm(34,30, 60, 36, WiFi_Logo_bits);
    OLED.setFont(ArialMT_Plain_10);
    OLED.setTextAlignment(TEXT_ALIGN_LEFT);
    OLED.drawString(0, 0,"Connecting to:");
    OLED.drawString(68, 0,ssid);
    OLED.display();
    i++;
    if (i==15){
      break;
    }
  }
  if(WiFi.status() == WL_CONNECTED){
    OLED.clear();
    OLED.drawXbm(0, 0, wifi_connect_width, wifi_connect_height, wifi_connect_bits );
    OLED.setFont(ArialMT_Plain_16);
    OLED.setTextAlignment(TEXT_ALIGN_LEFT);
    OLED.drawString(35, 10,"Connected !!");
    OLED.display();
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    delay(1000);
    OLED.clear();
    OLED.display();
  }
  OLED.clear();
  OLED.display();
}

void SetupGBot::ChangeLocations(){
  latitude = data1.toFloat();
  longitude = data2.toFloat();
  location = data3;
  for (int i = 80; i < 140; i++)
  {
    EEPROM.write(i, 0);   //clear eeprom 
  }
  strcpy(buf, data1.c_str()); //copy latitude to buf
  eeprom_write_string(80, buf);  //write buf to eeprom address 80
  strcpy(buf2, data2.c_str()); //copy longitude to buf2
  eeprom_write_string(96, buf2); // write buf to eeprom address 96
  strcpy(buf5, data3.c_str()); //copy location to buf5
  eeprom_write_string(111, buf5); // write buf to eeprom address 111
  
  OLED.clear();
  OLED.drawXbm(0,40, locations_width, locations_height, locations_bits );
  OLED.drawXbm(110,40, locations_width, locations_height, locations_bits );
  OLED.setFont(ArialMT_Plain_16);
  OLED.setTextAlignment(TEXT_ALIGN_LEFT);
  OLED.drawString(0,0,"Change Location");
  OLED.setFont(ArialMT_Plain_16);
  OLED.setTextAlignment(TEXT_ALIGN_LEFT);
  OLED.drawString(20, 30," Complete!!");
  OLED.display();
  delay(2000);
  OLED.clear();
  OLED.display();

  
}

void SetupGBot::ChangeAPIKey(){
  forecastApiKey = data1;
  for (int i = 150; i < 190; i++)
  {
    EEPROM.write(i, 0);   //clear eeprom 
  }
  strcpy(buf, data1.c_str()); //copy API to buf
  eeprom_write_string(150, buf);  //write buf to eeprom address 150
 
  OLED.clear();
  OLED.drawXbm(0,30, API_width, API_height, API_bits );
  //OLED.drawXbm(110,40, API_width, API_height, API_bits );
  OLED.setFont(ArialMT_Plain_16);
  OLED.setTextAlignment(TEXT_ALIGN_LEFT);
  OLED.drawString(0,0,"Setup API Key");
  OLED.setFont(ArialMT_Plain_16);
  OLED.setTextAlignment(TEXT_ALIGN_LEFT);
  OLED.drawString(30, 30," Complete!!");
  OLED.display();
  delay(2000);
  OLED.clear();
  OLED.display();
  
}

