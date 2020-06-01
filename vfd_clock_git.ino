
/*
 * Sends data to serial Noritake VFD module
 * https://www.noritake-itron.com/site2017/images/Specs/VFD/BFC/CU406-KTW220A/CU406-KTW220A-F3-c8.pdf
*/
#include "screen.h"
#include "weather.h"
#include "server.h"
#include <WiFiClient.h>
#include <WiFiUdp.h> //handles sending and receiving of UDP packages
#include <NTPClient.h>

char* SVRapName = "espAP";
char* SVRapPSWD = "PASSWORD HERE";
char* avail_aps;
char* weather;
char* weather_host;
int ap_numbers = 0;
int offline = 0;
int old_millis;
time_S* curr_time;
int bright_level = 1; //brightnes. 1 high, 0 low

int c_off=0x16,h_scroll=0x13,international=0x18, count=160;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org");

void setup() {
  avail_aps = (char *)calloc(390, sizeof(char));
  weather_host = (char *)calloc(146, sizeof(char));
  curr_time = (time_S*)malloc(sizeof(time_S));
  strcpy(avail_aps, 
  "<body bgcolor=\"#00ccff\"><center><h1><u>ESPClock</div></u></h1><br><form action=\"/control\" method=\"POST\"><input type=\"text\" name=\"name\" placeholder=\"AP Name\"><input type=\"text\" name=\"password\" placeholder=\"WiFi Password\"><input type=\"submit\" name=\"mode\" value=\"Login\"></form><br><form action=\"/control\" method=\"POST\"><input type=\"submit\" name=\"mode\" value=\"Offline\"></form>"
  );
  
  Serial.begin(1200); //Display is 1200 baud
  overwrite(1);
  clear();
  Serial.write(international);
  delay(1000);
  ap_numbers = scan_networks();
  WiFi.softAP(SVRapName,SVRapPSWD);
  Serial.print("Connect to ");
  Serial.print(SVRapName);
  Serial.print(" at: ");
  Serial.print(WiFi.softAPIP());
  start_server(); 
  unsigned long counter = 0;
  while (WiFi.status() != WL_CONNECTED && done == 0){
      counter ++;
      ESP.wdtFeed();
     if (WiFi.status() == WL_CONNECT_FAILED){
      ap_numbers = scan_networks();
      WiFi.softAP(SVRapName,SVRapPSWD);
      clear();
      Serial.print("Bad Password. Reconnect");
     } 
     else if (counter == 2294967294){
      clear();
      Serial.print("Timeout.");
      break;
     }
     server.handleClient();
     ESP.wdtFeed();
  }
  Serial.print("Done");

  if (WiFi.status() == WL_CONNECTED){
    timeClient.begin();
    update_time_ntp();
    weather = get_weather(curr_time -> Location);
  }
  server.stop();
  free(avail_aps);
  old_millis = millis();
  clear();
}
int increment_second(){
  curr_time -> Seconds ++;
  if (curr_time -> Seconds > 59){
    curr_time -> Seconds = 0;
    curr_time -> Minutes ++;
  }
  if (curr_time -> Minutes > 59){
    curr_time -> Minutes = 0;
    curr_time -> Hours ++;
  }
  if (curr_time -> Hours > 23){
    curr_time -> Hours = 0;
    curr_time -> Day ++;
  }
  if (curr_time -> Day > 6){
    curr_time -> Day = 0;
  }
  if (curr_time -> Hours >= 6 && curr_time -> Hours < 22){
    bright_level = 1;
  } else {
    bright_level = 0;
  }
  if (curr_time -> Minutes == 0 && curr_time -> Seconds == 0 && WiFi.status() == WL_CONNECTED){
    return 1; // Do something each hour
  }
  return 0;
}

void update_time_ntp(){
  timeClient.update();
  curr_time -> Hours = timeClient.getHours()+curr_time->UTC_os; //Add local Timezone Offset
  curr_time -> Minutes = timeClient.getMinutes();
  curr_time -> Seconds = timeClient.getSeconds();
  curr_time -> Day = timeClient.getDay();
}
int scan_networks(){
  Serial.print("Scanning...");
  int number = 0;
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);
  number = WiFi.scanNetworks();
  if (number >255){
    clear();
    number = 255;
  }
  char * new_aps = (char *)realloc(avail_aps, (390*sizeof(char)) + ((number*36)*sizeof(char))+number);
  if (new_aps != NULL){
    avail_aps = new_aps;
  } else {
    clear();
    Serial.print("Memory Fault!");
    return 0; //Memory Problem!!!
  }
  char * Scan_str_buff = (char *) (malloc(34*sizeof(char)));
  for (int i = 0; i < number; i++){
    WiFi.SSID(i).toCharArray(Scan_str_buff, 34);
    strcat (avail_aps, Scan_str_buff);
    strcat (avail_aps, "<br>");
  }
  strcat (avail_aps, "</center>");
  free(Scan_str_buff);
  clear();
  return number;
}
int seconds = 0;
int target_seconds = 0;
void loop() { //Main Control Loop
  ESP.wdtFeed();
  if (millis() >= old_millis + 1000){
    int secs =(int)( millis()/(unsigned long)old_millis);
    old_millis = millis();
    for (int i=0; i<secs; i++){ //Keeps time fairly in sync
      seconds ++;
      if (increment_second() == 1 && offline == 0){ //Every hour
        free (weather);
        clear();
        update_time_ntp();
        weather = get_weather(curr_time -> Location);
      }
    }
    char* h_buff = (char*)malloc(2*sizeof(char));
    char* m_buff = (char*)malloc(2*sizeof(char));
    sprintf(h_buff, "%02d", curr_time -> Hours);
    sprintf(m_buff, "%02d", curr_time -> Minutes);
    if (seconds >= target_seconds && offline == 0 ){
      if (seconds == target_seconds+300){
        target_seconds += 1500;
        clear();
      }
      else if (!(seconds % 30)){
        Serial.write(0x13);
        delay(1);
        int i = 0;
        while (weather[i] != '\0'){
          Serial.write(weather[i]);
          delay(50);
          i++;
        }
        i = 0;
        while (h_buff[i] != '\0'){
          Serial.write(h_buff[i]);
          delay(50);
          i++;
        }
        i = 0;
        Serial.print(":");
        while (m_buff[i] != '\0'){
          Serial.write(m_buff[i]);
          delay(50);
          i++;
        }      
      }
    } else {
      left();
      Serial.write(0x16);
      delay(1);
      for (int i = 0; i<10; i++){
        Serial.print(" ");
      }
      switch (curr_time -> Day){
        case 0:
          Serial.print("Sunday: ");
          break;
        case 1:
          Serial.print("Monday: ");
          break;
        case 2:
          Serial.print("Tuesday: ");
          break;
        case 3:
          Serial.print("Wednesday: ");
          break;
        case 4:
          Serial.print("Thursday: ");
          break;
        case 5:
          Serial.print("Friday: ");
          break;
        case 6:
          Serial.print("Saturday: ");
          break;
      }
      Serial.print(h_buff);
      Serial.print(":");
      Serial.print(m_buff);
  }
  free (h_buff); free (m_buff);
  }
}
