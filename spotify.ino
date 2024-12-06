#include <Wire.h>
#include <hd44780.h>  
#include <hd44780ioClass/hd44780_I2Cexp.h>  
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>  

const char* ssid = "ssid";
const char* password = "pass";

// spotify token, refresh hourly
const String accessToken = "token";  // replace with token

hd44780_I2Cexp lcd; 

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  lcd.begin(16, 2);  // 16x2 LCD

  getCurrentSong();
}

void loop() {
  getCurrentSong();
  delay(5000); 
}

void getCurrentSong() {
  HTTPClient http;
  http.begin("https://api.spotify.com/v1/me/player/currently-playing");
  http.addHeader("Authorization", "Bearer " + accessToken);

  int httpCode = http.GET();  

  if (httpCode == 200) {
    String payload = http.getString();  

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    String songName = doc["item"]["name"].as<String>();
    String artist = doc["item"]["artists"][0]["name"].as<String>();
    int progress = doc["progress_ms"].as<int>() / 1000;  
    int duration = doc["item"]["duration_ms"].as<int>() / 1000;  
    displaySong(songName, artist, progress, duration);
  } else {
    Serial.print("Error getting song info. HTTP Code: ");
    Serial.println(httpCode);
    String payload = http.getString();  
    Serial.println(payload);  
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error fetching song");
  }

  http.end();  
}

void displaySong(String song, String artist, int progress, int duration) {
  lcd.clear(); 


  lcd.setCursor(0, 0);
  lcd.print(song.substring(0, 16)); 
  
  lcd.setCursor(0, 1);
  lcd.print(artist.substring(0, 16));  
  
  String time = String(progress / 60) + ":" + String(progress % 60) + "/" +
                String(duration / 60) + ":" + String(duration % 60);
  lcd.setCursor(0, 1);
  lcd.print(time.substring(0, 16));  
}

