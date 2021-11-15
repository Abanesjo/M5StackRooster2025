#include <M5Core2.h>

#define THRESHOLD 2

struct customPoint {
  double x;
  double y;
};

double screenHeight,screenWidth;
int xCursor, yCursor;

float pitch = 0.0f;
float roll = 0.0f;
float yaw = 0.0f;

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
  float oldPitch = pitch;
  float oldRoll = roll;
  float oldYaw = yaw;
  float newPitch, newRoll, newYaw;
  M5.IMU.getAhrsData(&newPitch, &newRoll, &newYaw);

  pitch = newPitch;
  roll = newRoll;
  yaw = newYaw;

  if (abs(newPitch - oldPitch) > THRESHOLD || abs(newRoll - oldRoll) > THRESHOLD || abs(newYaw - oldYaw) > THRESHOLD) {
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

void homeScreenLayout(){
  M5.Lcd.fillRect(10, 0.125*screenHeight, screenWidth-20, 0.35 * screenHeight, LIGHTGREY);
  //TEST ALARM INDICATOR
  if (detectMovement()){
    M5.Lcd.fillCircle(screenWidth/2, screenHeight/2+50, 10, RED);
  }
  else{
    M5.Lcd.fillCircle(screenWidth/2, screenHeight/2+50, 10, GREEN);
  }

   M5.Lcd.setCursor(0.13 * screenWidth, 0.2 * screenHeight);

   //Printing the Time
   M5.Rtc.GetTime(&RTCtime);
   int alarmTime = toSeconds(8, 0, 0);
   int currentTime = toSeconds(RTCtime.Hours, RTCtime.Minutes, RTCtime.Seconds);
   timeStrbuff = formatTime(alarmTime - currentTime);

   M5.Lcd.println(timeStrbuff);
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
  String hourVal = String(hours);
  String minVal = String(minutes);
  String secVal = String(seconds);
  if(hours<10){
    hourVal = "0" + hourVal;
  }
  if(minutes<10){
    minVal = "0" + minVal;
  }
  if(seconds<10){
    secVal = "0" + secVal;
  }
  String output = hourVal + ":" + minVal + ":" + secVal;
  return output;
}

// if not declaring a customPoint object, you can use distance({x1, y1}, {x2, y2})
double distance (customPoint p1, customPoint p2)
{
  return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

bool isPressed(customPoint center, double radius) {
  TouchPoint_t coordinate = M5.Touch.getPressPoint();
  M5.Lcd.setCursor(30, 30);
  M5.Lcd.printf("x:%d, y:%d \r\n", coordinate.x, coordinate.y);

  customPoint pressPoint = {coordinate.x, coordinate.y};

  if (distance(pressPoint, center) > radius)
  {
    return false;
  }
  else {
    return true;
  }
}
