#include <M5Core2.h>

#define THRESHOLD 2

double screenHeight,screenWidth;
int selectMode;
int xCursor, yCursor;

float posX = 0.0f;
float posY = 0.0f;
float posZ = 0.0f;

RTC_TimeTypeDef RTCtime;

String timeStrbuff;

void setup(){
  M5.begin();
  M5.IMU.Init();
  M5.Lcd.begin();
  M5.Lcd.fillScreen(BLACK);
  screenHeight = M5.Lcd.height();
  screenWidth = M5.Lcd.width();
  xCursor = M5.Lcd.getCursorX();
  yCursor = M5.Lcd.getCursorY();
  M5.Lcd.setTextSize(6);
  M5.Lcd.setTextColor(BLACK,LIGHTGREY);
  setTime();  
}

void loop(){
  homeScreenLayout();
  delay(150);  
}

void setTime(){
  RTCtime.Hours = 0;
  RTCtime.Minutes = 0;
  RTCtime.Seconds = 0;
  M5.Rtc.SetTime(&RTCtime);
}

bool detectMovement(){
  bool result;
  float oldX = posX;
  float oldY = posY;
  float oldZ = posZ;
  float newX, newY, newZ;
  M5.IMU.getAhrsData(&newX, &newY, &newZ);

  posX = newX;
  posY = newY;
  posZ = newZ;

  if (abs(newX-oldX) > THRESHOLD || abs(newY-oldY) > THRESHOLD || abs(newZ - oldZ) > THRESHOLD) {
    return true;
  }
  else {
    return false;
  }
}

bool maxTimeReached(int hours, int minutes, int seconds){
  M5.Rtc.GetTime(&RTCtime);
  if (hours == RTCtime.Hours && minutes == RTCtime.Minutes && seconds == RTCtime.Seconds){
    return true;
  }
  else{
    return false;
  }
}

bool alarm(bool condition1, bool condition2){
  if (condition1 == 1 || condition2 == 1){
    return true;
  }
  else{
    return false;
  }
}

void homeScreenLayout(){
  M5.Lcd.fillRect(10, 0.125*screenHeight, screenWidth-20, 0.35 * screenHeight, LIGHTGREY);
  //TEST ALARM INDICATOR
  if (detectMovement()){
    M5.Lcd.fillCircle(screenWidth/2, screenHeight/2+50, 10, RED);
  }
  else{
    M5.Lcd.fillCircle(screenWidth/2, screenHeight/2+50, 10, GREEN);
  }
  /*M5.Lcd.fillCircle(screenWidth/2, screenHeight/2+50, 10, GREEN);
  if (){
    M5.Lcd.fillCircle(screenWidth/2, screenHeight/2+50, 10, RED);*/

   M5.Lcd.setCursor(0.13 * screenWidth, 0.2 * screenHeight);

   //Printing the Time
   M5.Rtc.GetTime(&RTCtime);
//     sprintf(timeStrbuff,"%02d:%02d:%02d",RTCtime.Hours,RTCtime.Minutes,RTCtime.Seconds);
//     M5.Lcd.println(timeStrbuff);
   int alarmTime = toSeconds(8, 0, 0);
   int currentTime = toSeconds(RTCtime.Hours, RTCtime.Minutes, RTCtime.Seconds);
   timeStrbuff = formatTime(alarmTime - currentTime);

   M5.Lcd.println(timeStrbuff);
   //break function if touch screen detected
}

int toSeconds(int hours, int minutes, int seconds) {
  return hours * 3600 + minutes * 60 + seconds;
}

String formatTime(int sec) {
  int orig = sec;
  int hours = sec/3600;
  sec -= 3600*hours;
  int minutes = sec/60;
  sec -= 60 * minutes;
  int seconds = sec;
  String output;
  output = String(hours) + ":" + String(minutes) + ":" + String(seconds);
  return output;
}

