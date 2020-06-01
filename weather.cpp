#include "weather.h"
const char * key = "Key Here";
char * get_weather(int site){
  sprintf(weather_host, "http://datapoint.metoffice.gov.uk/public/data/txt/wxfcs/regionalforecast/json/%d?&key=%s", 
      site, key);
  HTTPClient http;
  char * forecast = (char *)malloc(128*sizeof(char));
  http.begin(weather_host);     //Specify request destination
  int httpCode = http.GET();            //Send the request
  String payload = http.getString();    //Get the response payload from server
  
  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(3) + 2*JSON_OBJECT_SIZE(1) + 4*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + 1000;
  DynamicJsonDocument doc(capacity);
    
  deserializeJson(doc, payload);
  
  JsonObject RegionalFcst = doc["RegionalFcst"];
  const char* RegionalFcst_createdOn = RegionalFcst["createdOn"]; // "2020-05-31T14:19:22"
  const char* RegionalFcst_issuedAt = RegionalFcst["issuedAt"]; // "2020-05-31T16:00:00"
  const char* RegionalFcst_regionId = RegionalFcst["regionId"]; // "yh"
  
  const char* RegionalFcst_FcstPeriods_Period_0_id = RegionalFcst["FcstPeriods"]["Period"][0]["id"]; // "day1to2"
  
  JsonArray RegionalFcst_FcstPeriods_Period_0_Paragraph = RegionalFcst["FcstPeriods"]["Period"][0]["Paragraph"];
  
  const char* RegionalFcst_FcstPeriods_Period_0_Paragraph_0_title = RegionalFcst_FcstPeriods_Period_0_Paragraph[0]["title"]; // "Headline:"
  const char* RegionalFcst_FcstPeriods_Period_0_Paragraph_0__ = RegionalFcst_FcstPeriods_Period_0_Paragraph[0]["$"]; // "Cool night with some fog. Fine with sunshine on Monday."
  
  const char* RegionalFcst_FcstPeriods_Period_0_Paragraph_1_title = RegionalFcst_FcstPeriods_Period_0_Paragraph[1]["title"]; // "This Evening and Tonight:"
  const char* RegionalFcst_FcstPeriods_Period_0_Paragraph_1__ = RegionalFcst_FcstPeriods_Period_0_Paragraph[1]["$"]; // "A dry evening and night with light winds and long clear periods. Perhaps a few fog patches developing in the west during the early hours, with some low cloud too. Minimum Temperature 9C."
  
  const char* RegionalFcst_FcstPeriods_Period_0_Paragraph_2_title = RegionalFcst_FcstPeriods_Period_0_Paragraph[2]["title"]; // "Monday:"
  const char* RegionalFcst_FcstPeriods_Period_0_Paragraph_2__ = RegionalFcst_FcstPeriods_Period_0_Paragraph[2]["$"]; // "Any fog and low cloud  will quickly clear after sunrise. Plenty of sunshine to follow, though perhaps a bit cloudier than Sunday. Cooler, though still widely warm. Maximum Temperature 21C."
  strcpy(forecast, RegionalFcst_FcstPeriods_Period_0_Paragraph_0__);
  return forecast;
}
