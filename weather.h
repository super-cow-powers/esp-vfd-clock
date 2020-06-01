#include <ArduinoJson.h> //Version 6.15
#include <Arduino.h>
#include <ESP8266HTTPClient.h>
extern char* weather_host;
char* get_weather(int site);
