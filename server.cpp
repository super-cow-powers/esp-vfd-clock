#include "server.h"
#include "screen.h"
ESP8266WebServer server(80);
String ap_name;
String ap_password;
int done = 0;

int start_server(){
  server.on("/", HTTP_GET, handleRoot);        // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/configure", HTTP_GET, handleCfg);
  server.on("/control", HTTP_POST, handleControl); // Call the 'handleLogin' function when a POST request is made to URI "/login"
  server.on("/os", HTTP_POST, handleOffset);
  server.on("/time", HTTP_POST, handleTime);
  server.onNotFound(handleNotFound);           // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"
  server.begin();                            // Actually start the server
}
int handle_server(){
server.handleClient();
}

void handleRoot() {
  server.send(200, "text/html", avail_aps);
}
void handleCfg() {
  if (offline == 0){                    
    server.send(200, "text/html",   "<body bgcolor=\"#00ccff\"><center><h1><u>ESPClock</div></u></h1><br><form action=\"/os\" method=\"POST\"><input type=\"text\" name=\"os\" placeholder=\"UTC Offset\"><input type=\"text\" name=\"mreig\" placeholder=\"MetOffice Region\"><br>(Default 509, Yorkshire) Find from http://datapoint.metoffice.gov.uk/public/data/txt/wxfcs/regionalforecast/json/sitelist?key=KEY-HERE</br><input type=\"submit\" value=\"Set\"></form></center>");
  } else {
    server.send(200, "text/html",   "<body bgcolor=\"#00ccff\"><center><h1><u>ESPClock</div></u></h1><br><form action=\"/time\" method=\"POST\"><input type=\"text\" name=\"hours\" placeholder=\"HH\"><input type=\"text\" name=\"mins\" placeholder=\"MM\"><input type=\"text\" name=\"day\" placeholder=\"Day number (0-6)\"><br><input type=\"submit\" value=\"Set Time\"></form></center>");
  }
}

void handleOffset(){
  if (server.arg("os").toInt() >= -12 && server.arg("hours").toInt() <= 12){
    curr_time -> UTC_os = server.arg("os").toInt();
  } else {
    curr_time -> UTC_os = 0; //Default GMT
  }
  if (server.arg("mreig").toInt() >= 500 && server.arg("hours").toInt() <= 516){
    curr_time -> Location = server.arg("mreig").toInt();
  } else {
    curr_time -> Location = 509; //Default Yorkshire
  }
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ap_name,ap_password);
}
void handleTime(){
  if (server.arg("hours").toInt() >= 0 && server.arg("hours").toInt() <= 23){
    curr_time -> Hours = server.arg("hours").toInt();
  } else {
    curr_time -> Hours = 0;
  }
  if (server.arg("mins").toInt() >= 0 && server.arg("mins").toInt() <= 59){
    curr_time -> Minutes = server.arg("mins").toInt();
  } else {
    curr_time -> Minutes = 0;
  }
  if (server.arg("day").toInt() >= 0 && server.arg("day").toInt() <= 6){
    curr_time -> Day = server.arg("day").toInt();
  } else {
    curr_time -> Day = 0;
  }
  curr_time -> Seconds = 0;
  curr_time -> UTC_os = 0;
  curr_time -> Location = 509;
  done = 1;
  WiFi.mode(WIFI_OFF); //Turn Off Wifi
}

void handleControl() {
  int found = 0;
  if (server.arg("mode") != String("Offline")){
    offline = 0;
    char * ap_str_buff = (char *) (malloc(34*sizeof(char)));
    char * Scan_str_buff = (char *) (malloc(34*sizeof(char)));
    for (int i = 0; i <= ap_numbers; i++){
      WiFi.SSID(i).toCharArray(Scan_str_buff, 34);
      server.arg("name").toCharArray(ap_str_buff, 34);
      if (!strcmp(ap_str_buff,Scan_str_buff)){
        found = 1;
       }
    }
    
    free(ap_str_buff);
    free(Scan_str_buff);
   if (found == 1){
    clear();
    Serial.print("Connecting...");
   } else {
    server.send(200, "text/html","Not Found. Try Again.");
   }
   server.sendHeader("Location", "/configure",true); //Redirect
   server.send(302, "text/plane","");
   ap_name += server.arg("name");
   ap_password += server.arg("password");
  } else {
    clear();
    Serial.print("Offline Mode.");
    offline = 1;
    server.sendHeader("Location", "/configure",true); //Redirect
    server.send(302, "text/plane","");
  }
}



void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
