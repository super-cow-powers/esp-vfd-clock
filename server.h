#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "structs.h"


extern ESP8266WebServer server;
extern char* avail_aps;
extern int ap_numbers;
extern int offline;
extern int done; //Flag to show offline config is done
extern time_S* curr_time;
extern String ap_name;
extern String ap_password;

void handleRoot();              // function prototypes for HTTP handlers
void handleControl();  
void handleNotFound();
void handleCfg();
void handleOffset();
void handleTime();

int start_server();
int handle_server();
