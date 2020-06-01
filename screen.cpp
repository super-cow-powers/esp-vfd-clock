#include "screen.h"

void clear(){
  int clr0=0x1b,clr1=0x49;
  Serial.write(clr0);
  delay(10);
  Serial.write(clr1);
  delay(10);
  Serial.write(0x16);
  delay(1);
  set_brightnes();
}

void overwrite(int on_){
  if (on_ == 1){
    Serial.write(0x12);
  } else {
    Serial.write(0x13);
  }
  delay(10);
  Serial.write(0x16);
  delay(1);
  set_brightnes();
}

void left(){
 Serial.write(0xD);
 delay(1);
 set_brightnes();
}

void set_brightnes(){ //1 high, 0 low
  if (bright_level == 0){ //Low
    Serial.write(0x1b);
    delay(1);
    Serial.write(0x4c);
    delay(1);
    Serial.write(0x3f);
    delay(1);
  } else { //High
    Serial.write(0x1b);
    delay(1);
    Serial.write(0x4c);
    delay(1);
    Serial.write(0xff);
    delay(1);
  }
  
}
