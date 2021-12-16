#include <M5Core2.h>
#include <driver/i2s.h>

//accelerometer threshold
#define THRESHOLD 8
//speaker pins and other necessary setup
#define CONFIG_I2S_BCK_PIN 12 
#define CONFIG_I2S_LRCK_PIN 0
#define CONFIG_I2S_DATA_PIN 2
#define CONFIG_I2S_DATA_IN_PIN 34

#define Speak_I2S_NUMBER I2S_NUM_0  

#define MODE_MIC 0 
#define MODE_SPK 1
#define DATA_SIZE 1024

//array for sound file in data.c 
extern const unsigned char alarmTone[38528];

//point structure that can store x and y coordinates
struct customPoint {
  double x;
  double y;
};

//class for interactive game
class orderedCircles {
private:
  int ID;
  customPoint center;
  double radius;
  bool stat;

public:
  orderedCircles(int ID = 0, customPoint center = {-20,-20}, double radius = 0.0)
  {
    this->ID = ID;
    this->center = center;
    this->radius = radius;
    stat = true;

    updateImage();
  }

  int getID()
  {
    return ID;
  }

  customPoint getCenter()
  {
    return center;
  }

  double getRadius()
  {
    return radius;
  }

  bool getStatus()
  {
    return stat;
  }

  void updateImage()
  {
    //if stat is true, displays default image with blue circle and ID
    if (stat)
    {
      M5.Lcd.fillCircle(center.x, center.y, radius, BLUE);
      M5.Lcd.setCursor(center.x - 6, center.y - 8);
      M5.Lcd.setTextSize(3);
      M5.Lcd.setTextColor(WHITE, BLUE);
      M5.Lcd.print(ID);
    }
    //if stat is false, overrwites same region with a black circle
    else
    {
      M5.Lcd.fillCircle(center.x, center.y, radius, BLACK);
    }
  }

  bool trigger(int goal)
  {
    //if the ID equals the goal, stat is set to false
    if (goal == ID)
    {
      stat = false;
      updateImage();
      return true;
    }
    else
    {
      return false;
    }
  }
};

//Global Variables
double screenHeight,screenWidth;
int xCursor, yCursor;

//Gyro Position Variables
float pitch = 0.0f;
float roll = 0.0f;
float yaw = 0.0f;

RTC_TimeTypeDef RTCtime;

bool onButton = false;
int newProgress = 0;

//Alarm Time
int alarmHour = 0;
int alarmMinute = 0;

//Uncertainty
int bufferMinute = 0;

String timeStrbuff;

//for ringTone();
int triggerVibrate = 0;
bool toVibrate = false;

//for alarmVibrate();
int triggerRing = 0;
bool toRing = false;

void setup(){
  M5.begin(true, true, true, true); 
  M5.IMU.Init();
  M5.Lcd.begin();
  M5.Lcd.fillScreen(BLACK);
  screenHeight = M5.Lcd.height();
  screenWidth = M5.Lcd.width();
  xCursor = M5.Lcd.getCursorX();
  yCursor = M5.Lcd.getCursorY();  
  SpeakInit();
}

//void loop uses control system detailed in final report
void loop(){  
  M5.Lcd.clear();
  stopVibrate();
  while (newProgress == 0)
  {
    welcomeScreen();
    if (M5.Touch.ispressed())
    {
        newProgress = 1;
      vibrate();
    }
  }
  
  if (newProgress == 1)
  {
    M5.Lcd.clear();
    secondScreenLayoutStatic();
  }
  while(newProgress == 1)
  {
    secondScreenLayoutDynamic();
    secondScreenButtonSystem();
  }

  if (newProgress == 2)
  {
    M5.Lcd.clear();
    thirdScreenLayoutStatic();
  }
  while (newProgress == 2)
  {
    thirdScreenLayoutDynamic();
    thirdScreenButtonSystem();  
  }

  if (newProgress == 3)
  {
    M5.Lcd.clear();
    setTime();
    countdownLayout1Static();
  }
  while(newProgress == 3)
  {
    countdownLayoutDynamic();
    countdownLayout1ButtonSystem();
    delay(150);
  };

  if (newProgress == 4)
  {
    M5.Lcd.clear();
    countdownLayout2Static();
  }
  while (newProgress == 4)
  { 
    countdownLayoutDynamic();
    countdownLayout2ButtonSystem();
    delay(150);
  }

  if (newProgress == 5)
  {
    setTime();
    M5.Lcd.clear();
    stopAlarmScreenStatic();
    triggerRing = 0;
    toRing = false;
    triggerVibrate = 0; 
    toVibrate = false;
  }
  while (newProgress == 5)
  {
    stopAlarmScreenDynamic();
    if (isPressed({0.5 * screenWidth, 0.5 * screenHeight}, 80))
    {
      newProgress = 6;
      vibrate();
    }
  }

  if (newProgress == 6)
  {
    setTime();
    M5.Lcd.clear();
    triggerRing = 0;
    toRing = false;
  }
  while (newProgress == 6)
  {
    gameSystem();
  }
}

//resets system time
void setTime(){
  RTCtime.Hours = 0;
  RTCtime.Minutes = 0;
  RTCtime.Seconds = 0;
  M5.Rtc.SetTime(&RTCtime);
}

//measures if position deviation is greater than threshold
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

  float deltaPitch = abs(newPitch - oldPitch);
  float deltaRoll = abs(newRoll - oldRoll);
  float deltaYaw = abs(newYaw - oldYaw);
  float delta = deltaPitch + deltaRoll + deltaYaw;

  
//  Unresolved bug: for some reason, removing the next 4 lines crashes the program
  M5.Lcd.setTextSize(0);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setCursor(-20, -20);
  M5.Lcd.printf("Error: %3.02f", delta);
//  Serial.println(delta);
  
  if (delta > THRESHOLD) {
    return true;
  }
  else {
    return false;
  }
}

//if not declaring a customPoint object, you can use distance({x1, y1}, {x2, y2})
double distance(customPoint p1, customPoint p2)
{
  return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

// touch detection function
bool isPressed(customPoint center, double radius) {
  TouchPoint_t coordinate = M5.Touch.getPressPoint();
//  M5.Lcd.setCursor(30, 30);
//  M5.Lcd.printf("x:%d, y:%d \n", coordinate.x, coordinate.y);

  customPoint pressPoint = {coordinate.x, coordinate.y};

  if (coordinate.x == -1 || coordinate.y == -1)
  {
    return false;
  }
  else
  {
    if (distance(pressPoint, center) > radius)
    {
      return false;
    }
    else {
      return true;
    }
  }  
}

// converts time to seconds
int toSeconds(int hours, int minutes, int seconds) {
  return hours * 3600 + minutes * 60 + seconds;
}

// formats time as XX:XX:XX
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

//start screen
void welcomeScreen() {
  M5.Lcd.setTextSize(5);
  M5.Lcd.setTextColor(WHITE, BLACK);
  
  M5.Lcd.setCursor(0.125*screenWidth, 0.125*screenHeight);
  M5.Lcd.print("M5 Stack");
  M5.Lcd.setCursor(0.18*screenWidth, 0.3*screenHeight);
  M5.Lcd.print("Rooster");
  M5.Lcd.setCursor(0.28*screenWidth, 0.475*screenHeight);
  M5.Lcd.print("2025");

  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(LIGHTGREY, BLACK);
  M5.Lcd.setCursor(0.1*screenWidth, 0.8*screenHeight);
  M5.Lcd.print("Touch Screen to Begin");
}

void countdownLayout1Static(){
//  Time Display
  M5.Lcd.fillRect(10, 0.125*screenHeight, screenWidth-20, 0.35 * screenHeight, NAVY);
  
//  cancel box
  M5.Lcd.fillRect(0.07 * screenWidth, 0.7 * screenHeight, 0.30 * screenWidth, 0.20 * screenHeight, RED);
  
//  cancel text
  M5.Lcd.setCursor(0.1 * screenWidth, 0.78 * screenHeight);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE, RED);
  M5.Lcd.print("Cancel");
}

void countdownLayout2Static(){
//  Time Display
  M5.Lcd.fillRect(10, 0.125*screenHeight, screenWidth-20, 0.35 * screenHeight, NAVY);
//  Cancel box
  M5.Lcd.fillRect(0.07 * screenWidth, 0.7 * screenHeight, 0.30 * screenWidth, 0.20 * screenHeight, BLUE);
//  Cancel text
  M5.Lcd.setCursor(0.1 * screenWidth, 0.78 * screenHeight);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE, BLUE);
  M5.Lcd.print("Skip");
}

void countdownLayout1ButtonSystem()
{
//  M5.Lcd.fillCircle(0.07*screenWidth + 0.15*screenWidth, 0.7*screenHeight + 0.1*screenHeight, 0.15*screenWidth, GREEN);
  if (isPressed({0.07*screenWidth + 0.15*screenWidth, 0.7*screenHeight + 0.1*screenHeight}, 0.15*screenWidth))
  {
    newProgress = 1;
    vibrate();
  }
}

void countdownLayout2ButtonSystem()
{
//  M5.Lcd.fillCircle(0.07*screenWidth + 0.15*screenWidth, 0.7*screenHeight + 0.1*screenHeight, 0.15*screenWidth, GREEN);
  if (isPressed({0.07*screenWidth + 0.15*screenWidth, 0.7*screenHeight + 0.1*screenHeight}, 0.15*screenWidth))
  {
    newProgress = 5;
    vibrate();
  }
}

void countdownLayoutDynamic()
{
//  Printing the time left
  M5.Lcd.setTextSize(5);
  M5.Lcd.setTextColor(LIGHTGREY,NAVY);
  M5.Lcd.setCursor(0.13 * screenWidth, 0.2 * screenHeight);
  M5.Rtc.GetTime(&RTCtime);
  int alarmTime = toSeconds(alarmHour, alarmMinute, 0);
  int currentTime = toSeconds(RTCtime.Hours, RTCtime.Minutes, RTCtime.Seconds);
  int timeDiff = alarmTime - currentTime;
  
  timeStrbuff = formatTime(timeDiff);
  M5.Lcd.println(timeStrbuff);

//  TEST Movement Indicator
  if (detectMovement()){
    M5.Lcd.fillCircle(screenWidth/2, screenHeight/2+50, 10, RED);
  }
  else{
    M5.Lcd.fillCircle(screenWidth/2, screenHeight/2+50, 10, GREEN);
  }
  if (timeDiff < bufferMinute*60)
  {
    newProgress = 4;
  }
  if (triggerAlarm())
  {
      newProgress = 5;
  }
}

void secondScreenButtonSystem()
{
//  M5.Lcd.setCursor(20,20);
//  M5.Lcd.printf("  %d   ", alarmHour);
//  M5.Lcd.setCursor(20, 40);
//  M5.Lcd.printf("  %d   ", alarmMinute);
  
  if (!M5.Touch.ispressed())
  {
    onButton = false;
  }

//  Confirmation Button: (0.1*screenWidth, 0.5*screenHeight), R = 0.1*screenWidth
  
  if (isPressed({0.1*screenWidth, 0.5*screenHeight}, 0.1*screenWidth))
  {
    newProgress = 2;
    vibrate();
  }
  else if (isPressed({0.130 * screenWidth, 0.134 * screenHeight}, 30 ))
  {
    newProgress = 0;
    vibrate();
  }
  else
  {
//    Button 1: (0.65*screenWidth, 0.275*screenHeight), R = 0.1*screenWidth
//    M5.Lcd.fillCircle(0.65*screenWidth, 0.275*screenHeight, 0.1*screenWidth, GREEN);
    if (isPressed({0.65*screenWidth, 0.275*screenHeight}, 0.1*screenWidth))
    {
      if (!onButton && alarmHour > 0)
      {
        vibrate();
        alarmHour--;
        onButton = true;
      }
    }
  
//    Button 2: (0.9*screenWidth, 0.275*screenHeight), R = 0.1*screenWidth
//    M5.Lcd.fillCircle(0.9*screenWidth, 0.275*screenHeight, 0.1*screenWidth, GREEN);
    if (isPressed({0.9*screenWidth, 0.275*screenHeight}, 0.1*screenWidth))
    {
      if (!onButton && alarmHour < 24)
      {
        vibrate();
        alarmHour++;
        onButton = true;
      }
    }

//    Button 3: (0.65*screenWidth, 0.75*screenHeight), R = 0.1*screenWidth
//    M5.Lcd.fillCircle(0.65*screenWidth, 0.75*screenHeight, 0.1*screenWidth, GREEN);
    if (isPressed({0.65*screenWidth, 0.75*screenHeight}, 0.1*screenWidth))
    {
      if (!onButton && alarmMinute > 0)
      {
        vibrate();
        alarmMinute -= 1;
        onButton = true;
      }
    }
  
//     Button 4: (0.9*screenWidth, 0.75*screenHeight), R = 0.1*screenWidth
//     M5.Lcd.fillCircle(0.9*screenWidth, 0.75*screenHeight, 0.1*screenWidth, GREEN);
     if (isPressed({0.9*screenWidth, 0.75*screenHeight}, 0.1*screenWidth))
     {
        if (!onButton && alarmMinute < 60)
        {
          vibrate();
          alarmMinute += 1;
          onButton = true;
        }
     }
  }
}

void secondScreenLayoutStatic()
{ 
//  4 boxes for setting the alarm 
//                    xCur           yCur                 width         height
  M5.Lcd.fillRect(0.55*screenWidth, 0.125*screenHeight, 0.2*screenWidth, 0.30 * screenHeight, OLIVE); //1st Rect
  M5.Lcd.fillRect(0.80*screenWidth, 0.125*screenHeight, 0.2*screenWidth, 0.30 * screenHeight, OLIVE); //2nd Rect
  M5.Lcd.fillRect(0.55*screenWidth, 0.6*screenHeight, 0.2*screenWidth, 0.30 * screenHeight, OLIVE); //3rd Rect
  M5.Lcd.fillRect(0.80*screenWidth, 0.6*screenHeight, 0.2*screenWidth, 0.30 * screenHeight, OLIVE); //4th rect

//   + and - symbols in those boxes

//  1st Box
  M5.Lcd.fillRect(0.605*screenWidth, 0.274*screenHeight, 0.09*screenWidth, 0.025*screenHeight, BLACK);

//  2nd Box
  M5.Lcd.fillRect(0.89*screenWidth, 0.220*screenHeight, 0.02*screenWidth, 0.12*screenHeight, BLACK);
  M5.Lcd.fillRect(0.85*screenWidth, 0.264*screenHeight, 0.09*screenWidth, 0.025*screenHeight, BLACK);

//  3rd Box
  M5.Lcd.fillRect(0.605*screenWidth, 0.749*screenHeight, 0.09*screenWidth, 0.025*screenHeight, BLACK);

//  4th Box
  M5.Lcd.fillRect(0.89*screenWidth, 0.695*screenHeight, 0.02*screenWidth, 0.12*screenHeight, BLACK);
  M5.Lcd.fillRect(0.85*screenWidth, 0.739*screenHeight, 0.09*screenWidth, 0.025*screenHeight, BLACK);

//  LEFT 1 Box for hours
  M5.Lcd.fillRect(0.285*screenWidth, 0.125*screenHeight, 0.2*screenWidth, 0.30 * screenHeight, LIGHTGREY);
//  LEFT 2 Box for minutes
  M5.Lcd.fillRect(0.285*screenWidth, 0.6*screenHeight, 0.2*screenWidth, 0.30 * screenHeight, LIGHTGREY);

//  "BUFFER TIME" text

   M5.Lcd.setTextSize(2.4);
   M5.Lcd.setTextColor(WHITE, BLACK);

//  Set Cursor
  M5.Lcd.setCursor(0.23 * screenWidth, 0.03 * screenHeight);
  M5.Lcd.print("Set Alarm Time");

//  Hours and mins text
  M5.Lcd.setCursor(0.03 * screenWidth, 0.25 * screenHeight);
  M5.Lcd.print("Hours");

  M5.Lcd.setCursor(0.03 * screenWidth, 0.70 * screenHeight);
  M5.Lcd.print("Mins");

//  Back button 
  M5.Lcd.fillCircle(0.130 * screenWidth, 0.134 * screenHeight, 19, CYAN);
  M5.Lcd.setCursor(0.096 * screenWidth, 0.118 * screenHeight);
  M5.Lcd.setTextSize(2.8);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.print("<-");

//  OK Button
  M5.Lcd.fillCircle(0.1*screenWidth, 0.5*screenHeight, 0.1*screenWidth, BLUE);
  M5.Lcd.setCursor(0.03*screenWidth, 0.45*screenHeight);
  M5.Lcd.setTextSize(4.2);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.print("OK");
}

void secondScreenLayoutDynamic()
{
//  Hours and Mins time 
  M5.Lcd.setTextColor(BLACK, LIGHTGREY);
  M5.Lcd.setTextSize(4.8);
  
  M5.Lcd.setCursor(0.32 * screenWidth  , 0.25* screenHeight);
  M5.Lcd.printf("%0.2d",alarmHour);

  M5.Lcd.setCursor(0.32 * screenWidth , 0.70* screenHeight);
  M5.Lcd.printf("%0.2d",alarmMinute);
}

void thirdScreenLayoutStatic()
{
//  2 boxes for setting the alarm 
//                    xCur           yCur                 width         height
  M5.Lcd.fillRect(0.55*screenWidth, 0.125*screenHeight, 0.2*screenWidth, 0.30 * screenHeight, OLIVE); //1st Rect
  M5.Lcd.fillRect(0.80*screenWidth, 0.125*screenHeight, 0.2*screenWidth, 0.30 * screenHeight, OLIVE); //2nd Rect

//   + and - symbols in those boxes

//  1st Box
  M5.Lcd.fillRect(0.605*screenWidth, 0.274*screenHeight, 0.09*screenWidth, 0.025*screenHeight, BLACK);

//  2nd Box
  M5.Lcd.fillRect(0.89*screenWidth, 0.220*screenHeight, 0.02*screenWidth, 0.12*screenHeight, BLACK);
  M5.Lcd.fillRect(0.85*screenWidth, 0.264*screenHeight, 0.09*screenWidth, 0.025*screenHeight, BLACK);

//  LEFT 1 Box for minutes
  M5.Lcd.fillRect(0.285*screenWidth, 0.125*screenHeight, 0.2*screenWidth, 0.30 * screenHeight, LIGHTGREY);

//  "BUFFER TIME" text

   M5.Lcd.setTextSize(2.4);
   M5.Lcd.setTextColor(WHITE, BLACK);

//  Set Cursor
  M5.Lcd.setCursor(0.23 * screenWidth, 0.03 * screenHeight);
  M5.Lcd.print("Set Uncertainty");

//  Hours and mins text
  M5.Lcd.setCursor(0.03 * screenWidth, 0.25 * screenHeight);
  M5.Lcd.print("Mins");
  
//  Back button 
  M5.Lcd.fillCircle(0.130 * screenWidth, 0.134 * screenHeight, 19, CYAN);
  M5.Lcd.setCursor(0.096 * screenWidth, 0.118 * screenHeight);
  M5.Lcd.setTextSize(2.8);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.print("<-");

//  OK button
  M5.Lcd.fillCircle(0.1*screenWidth, 0.5*screenHeight, 0.1*screenWidth, BLUE);
  M5.Lcd.setCursor(0.03*screenWidth, 0.45*screenHeight);
  M5.Lcd.setTextSize(4.2);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.print("OK");
}

void thirdScreenLayoutDynamic()
{
//  Mins time
  M5.Lcd.setTextColor(BLACK, LIGHTGREY);
  M5.Lcd.setTextSize(4.8);
  
  M5.Lcd.setCursor(0.32 * screenWidth  , 0.25* screenHeight);
  M5.Lcd.printf("%0.2d",bufferMinute);
}

void thirdScreenButtonSystem()
{
  if (bufferMinute > alarmHour*60 + alarmMinute)
  {
    bufferMinute = alarmHour*60 + alarmMinute;
  }
  
  if (!M5.Touch.ispressed())
  {
    onButton = false;
  }

//  Confirmation Button: (0.1*screenWidth, 0.5*screenHeight), R = 0.1*screenWidth
  if (isPressed({0.1*screenWidth, 0.5*screenHeight}, 0.1*screenWidth))
  {
    newProgress = 3;
    vibrate();
  }
  else if (isPressed({0.130 * screenWidth, 0.134 * screenHeight}, 30 ))
  {
    newProgress = 1;
    vibrate();
  }
  else
  {
//    Button 1: (0.65*screenWidth, 0.275*screenHeight), R = 0.1*screenWidth
//    M5.Lcd.fillCircle(0.65*screenWidth, 0.275*screenHeight, 0.1*screenWidth, GREEN);
    if (isPressed({0.65*screenWidth, 0.275*screenHeight}, 0.1*screenWidth))
    {
      if (!onButton && bufferMinute > 0)
      {
        vibrate();
        bufferMinute-=1;
        onButton = true;
      }
    }
  
//    Button 2: (0.9*screenWidth, 0.275*screenHeight), R = 0.1*screenWidth
//    M5.Lcd.fillCircle(0.9*screenWidth, 0.275*screenHeight, 0.1*screenWidth, GREEN);
    if (isPressed({0.9*screenWidth, 0.275*screenHeight}, 0.1*screenWidth))
    {
      if (!onButton && (bufferMinute + 1) <= min(90, alarmHour*60 + alarmMinute))
      {
        vibrate();
        bufferMinute+=1;
        onButton = true;
      }
    }
  }
}
// Game. Detailed explanation in the final report. 
void gameSystem()
{
  bool pass = false;
  while(!pass)
  {
    int* randomList = randomize(9);
    orderedCircles circles[9];
    
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        customPoint center;
        center.x = ((double) j * 0.33 + 0.167)*screenWidth;
        center.y = ((double) i * 0.33 + 0.167)*screenHeight;
        double radius = 0.166*screenHeight;
        circles[3*i + j] = orderedCircles(randomList[3*i + j], center, radius);
      }
    }

    int goal = 1;
    while(true)
    {
      if (goal > 9)
      {
        pass = true;
        newProgress = 0;
        break;
      }
      if (M5.Touch.ispressed())
      {
        TouchPoint_t coordinate = M5.Touch.getPressPoint();
        int region = floor(coordinate.x / (screenWidth/3.0)) + 3 * floor(coordinate.y / (screenHeight/3.0));
        if (!circles[region].getStatus())
        {
          continue;
        }
        else
        {
          if (circles[region].trigger(goal))
          {
            goal++;
            vibrate();
          }
        }
      }
    }
    delete[] randomList;
  }
}

// big red circle for the stop alarm screen .
void stopAlarmScreenStatic()
{
    M5.Lcd.fillCircle(0.5 * screenWidth, 0.5 * screenHeight, 80,  RED);
    M5.Lcd.setCursor(0.275*screenWidth, 0.4 * screenHeight);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.print("  Stop");
    M5.Lcd.setCursor(0.25*screenWidth, 0.6 *screenHeight);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.print("  Alarm");
}

// for stop alarm screen vibrations and ringing
void stopAlarmScreenDynamic()
{
  alarmVibrate();
  ringTone();
}

// randomizer function
int* randomize(int n)
{
  int* list = new int(n+1);
  list[n] = -1;
  for (int i = 0; i < n; i++)
  {
    list[i] = i + 1;
  }

  for (int i = 0; i < random(5, 12); i++)
  {
    int a = random(1,9);
    int b = random(1,9);
    int temp = list[a];
    list[a] = list[b];
    list[b] = temp;
  }
  return list;
}

//  VIBRATION FEEDBACK
void vibrate(){
  M5.Axp.SetLDOEnable(3,true);
  delay(200);
  M5.Axp.SetLDOEnable(3,false);
}

//  ALARM VIBRATION
void alarmVibrate()
{
  M5.Rtc.GetTime(&RTCtime);
  int RTCinSecs = toSeconds(RTCtime.Hours, RTCtime.Minutes, RTCtime.Seconds);
  if (RTCinSecs > triggerVibrate)
  {
    
    if (toVibrate == false) 
    {
      toVibrate = true;
      triggerVibrate = RTCinSecs + 3;
    }
    else
    {
      toVibrate = false;
      triggerVibrate = RTCinSecs + 1;
    }
  }
  if (toVibrate == true)
  {
     M5.Axp.SetLDOEnable(3,true);
  }
  else
  {
    M5.Axp.SetLDOEnable(3,false);
  }
}

// RINGTONE
void ringTone()
{
  M5.Rtc.GetTime(&RTCtime);
  int RTCinSecs = toSeconds(RTCtime.Hours, RTCtime.Minutes, RTCtime.Seconds);

  if (RTCinSecs > triggerRing)
  {
    if (toRing == false)
    {
      toRing = true;
      triggerRing = RTCinSecs + 3;
    }
    else
    {
      toRing = false;
      triggerRing = RTCinSecs + 1;
    }
  }

  if (toRing)
  {
    RingAlarm();
  }
  
}

// STOP ALARM VIBRATION
void stopVibrate(){
  M5.Axp.SetLDOEnable(3,false);
}

// TRIGGER ALARM
bool triggerAlarm(){
  if (trigger1() || trigger2()){
    return true;
  }
  else{
    return false;
  }
}

// First Trigger Option: Triggers alarm when maximum time is reached.
bool trigger1(){
  int maximumTime = toSeconds(alarmHour, alarmMinute, 0);
  int RTCinSecs = toSeconds(RTCtime.Hours, RTCtime.Minutes, RTCtime.Seconds);
  if (maximumTime == RTCinSecs) {
    return true;
  }
  else {
    return false;
  }
}

// Second Trigger Option: Triggers alarm when movement is detected 5 times, within the time range.
int counter(0), timeGap, timer(0), baseTime;
bool condition(int gap){
   int RTCinSecs = toSeconds(RTCtime.Hours, RTCtime.Minutes, RTCtime.Seconds);
//   Serial.println(RTCinSecs);
   if(RTCinSecs>gap){
      return true;
   }
   else{
      return false;
   }
}

bool resetCounter(int startPoint){
   int RTCinSecs = toSeconds(RTCtime.Hours, RTCtime.Minutes, RTCtime.Seconds);
   int timeInterval = RTCinSecs - startPoint;
   if (timeInterval > 180){
      return true;
   }
   else{
      return false;
   }
}

bool trigger2(){
//  Serial.println(counter);
  int RTCinSecs = toSeconds(RTCtime.Hours, RTCtime.Minutes, RTCtime.Seconds);
  int maximumTime = toSeconds(alarmHour, alarmMinute, 0);
  int uncertainty = bufferMinute*60;
  int timeDifference = maximumTime-RTCinSecs;
  if(timeDifference<= uncertainty){
    if (counter == 0){
      if(detectMovement()){
          counter++;
          timeGap = RTCinSecs+1;
          baseTime = RTCinSecs;
        }
    }
    if(resetCounter(baseTime)){
      counter = 0;
    }
    if(condition(timeGap)){
      if (detectMovement()){
        counter++;
        timeGap = RTCinSecs+1;
      }
    }
  }
  if(counter>=5){
    return true;
  }
  else{
    return false;
  }
}

//speaker setup, from example speaker file. 
// see https://github.com/m5stack/M5Core2/tree/master/examples/Basics/speak 
bool InitI2SSpeakOrMic(int mode){  //Init I2S.  初始化I2S
    esp_err_t err = ESP_OK;

    i2s_driver_uninstall(Speak_I2S_NUMBER); // Uninstall the I2S driver.  卸载I2S驱动
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER),  // Set the I2S operating mode.  设置I2S工作模式
        .sample_rate = 44100, // Set the I2S sampling rate.  设置I2S采样率
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // Fixed 12-bit stereo MSB.  固定为12位立体声MSB
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT, // Set the channel format.  设置频道格式
        .communication_format = I2S_COMM_FORMAT_I2S,  // Set the format of the communication.  设置通讯格式
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // Set the interrupt flag.  设置中断的标志
        .dma_buf_count = 2, //DMA buffer count.  DMA缓冲区计数
        .dma_buf_len = 128, //DMA buffer length.  DMA缓冲区长度
    };
    if (mode == MODE_MIC){
        i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM);
    }else{
        i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
        i2s_config.use_apll = false;  //I2S clock setup.  I2S时钟设置
        i2s_config.tx_desc_auto_clear = true; // Enables auto-cleanup descriptors for understreams.  开启欠流自动清除描述符
    }
    // Install and drive I2S.  安装并驱动I2S
    err += i2s_driver_install(Speak_I2S_NUMBER, &i2s_config, 0, NULL);

    i2s_pin_config_t tx_pin_config;
    tx_pin_config.bck_io_num = CONFIG_I2S_BCK_PIN;  // Link the BCK to the CONFIG_I2S_BCK_PIN pin. 将BCK链接至CONFIG_I2S_BCK_PIN引脚
    tx_pin_config.ws_io_num = CONFIG_I2S_LRCK_PIN;  //          ...
    tx_pin_config.data_out_num = CONFIG_I2S_DATA_PIN;  //       ...
    tx_pin_config.data_in_num = CONFIG_I2S_DATA_IN_PIN; //      ...
    err += i2s_set_pin(Speak_I2S_NUMBER, &tx_pin_config); // Set the I2S pin number.  设置I2S引脚编号
    err += i2s_set_clk(Speak_I2S_NUMBER, 44100, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO); // Set the clock and bitwidth used by I2S Rx and Tx. 设置I2S RX、Tx使用的时钟和位宽
    return true;
}
// initializes speaker
void SpeakInit(void){
  M5.Axp.SetSpkEnable(true); 
  InitI2SSpeakOrMic(MODE_SPK);
}

//writes to speaker
void RingAlarm(void){
  size_t bytes_written = 0;
  i2s_write(Speak_I2S_NUMBER, alarmTone, 38528, &bytes_written, portMAX_DELAY);
}
