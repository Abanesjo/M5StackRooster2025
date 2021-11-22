#include <M5Core2.h>

#define THRESHOLD 2

struct customPoint {
  double x;
  double y;
};

class orderedSquares {
private:
  int ID;
  customPoint center;
  double radius;
  bool stat;

public:
  orderedSquares(int ID = 0, customPoint center = {-20,-20}, double radius = 0.0)
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
    if (stat)
    {
      M5.Lcd.fillCircle(center.x, center.y, radius, BLUE);
      M5.Lcd.setCursor(center.x - 6, center.y - 8);
      M5.Lcd.setTextSize(3);
      M5.Lcd.setTextColor(WHITE, BLUE);
      M5.Lcd.print(ID);
    }
    else
    {
      M5.Lcd.fillCircle(center.x, center.y, radius, BLACK);
    }
  }

  bool trigger(int goal)
  {
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

double screenHeight,screenWidth;
int xCursor, yCursor;

float pitch = 0.0f;
float roll = 0.0f;
float yaw = 0.0f;

RTC_TimeTypeDef RTCtime;

bool onButton = false;
bool progress = false;

//Alarm Time
int alarmHour = 0;
int alarmMinute = 0;

//Uncertainty
int bufferMinute = 0;

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
}

void loop(){
  M5.Lcd.clear();
  progress = false;
  do
  {
    welcomeScreen();
    if (M5.Touch.ispressed())
    {
      progress = true;
    }
  } while(!progress);

  progress = false;
  M5.Lcd.clear();
  secondScreenLayoutStatic();
  do
  {
    secondScreenLayoutDynamic();
    secondScreenButtonSystem();
  } while(!progress);

  progress = false;
  M5.Lcd.clear();
  thirdScreenLayoutStatic();
  do
  {
    thirdScreenLayoutDynamic();
    thirdScreenButtonSystem();
  }while(!progress);

  progress = false;
  M5.Lcd.clear();
  setTime();
  countdownLayoutStatic();
  do
  {
    countdownLayoutDynamic();
    delay(150);
  } while(!progress);

  progress = false;
  M5.Lcd.clear();
  do
  {
    alarmSystem();
  }while(!progress);
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

  float deltaPitch = abs(newPitch - oldPitch);
  float deltaRoll = abs(newRoll - oldRoll);
  float deltaYaw = abs(newYaw - oldYaw);
  float delta = deltaPitch + deltaRoll + deltaYaw;

  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setCursor(0.2*screenWidth, 0.8*screenHeight);
  M5.Lcd.printf("Error: %3.02f", delta);
  
  if (delta > THRESHOLD) {
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

//if not declaring a customPoint object, you can use distance({x1, y1}, {x2, y2})
double distance(customPoint p1, customPoint p2)
{
  return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

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

void countdownLayoutStatic(){
  
  M5.Lcd.fillRect(10, 0.125*screenHeight, screenWidth-20, 0.35 * screenHeight, LIGHTGREY);
}

void countdownLayoutDynamic()
{
  //Printing the time left
  M5.Lcd.setTextSize(5);
  M5.Lcd.setTextColor(BLACK,LIGHTGREY);
  M5.Lcd.setCursor(0.13 * screenWidth, 0.2 * screenHeight);
  M5.Rtc.GetTime(&RTCtime);
  int alarmTime = toSeconds(alarmHour, alarmMinute, 0);
  int currentTime = toSeconds(RTCtime.Hours, RTCtime.Minutes, RTCtime.Seconds);
  int timeDiff = alarmTime - currentTime;

  if (timeDiff <= 0)
  {
    progress = true;
  }
  
  timeStrbuff = formatTime(timeDiff);
  M5.Lcd.println(timeStrbuff);

  
  //TEST Movement Indicator
  if (detectMovement()){
    M5.Lcd.fillCircle(screenWidth/2, screenHeight/2+50, 10, RED);
  }
  else{
    M5.Lcd.fillCircle(screenWidth/2, screenHeight/2+50, 10, GREEN);
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

  //Confirmation Button: (0.1*screenWidth, 0.5*screenHeight), R = 0.1*screenWidth
  if (isPressed({0.1*screenWidth, 0.5*screenHeight}, 0.1*screenWidth))
  {
    progress = true;
  }
  else
  {
    //Button 1: (0.65*screenWidth, 0.275*screenHeight), R = 0.1*screenWidth
//    M5.Lcd.fillCircle(0.65*screenWidth, 0.275*screenHeight, 0.1*screenWidth, GREEN);
    if (isPressed({0.65*screenWidth, 0.275*screenHeight}, 0.1*screenWidth))
    {
      if (!onButton && alarmHour > 0)
      {
        alarmHour--;
        onButton = true;
      }
    }
  
    //Button 2: (0.9*screenWidth, 0.275*screenHeight), R = 0.1*screenWidth
//    M5.Lcd.fillCircle(0.9*screenWidth, 0.275*screenHeight, 0.1*screenWidth, GREEN);
    if (isPressed({0.9*screenWidth, 0.275*screenHeight}, 0.1*screenWidth))
    {
      if (!onButton && alarmHour < 24)
      {
        alarmHour++;
        onButton = true;
      }
    }

    //Button 3: (0.65*screenWidth, 0.75*screenHeight), R = 0.1*screenWidth
//    M5.Lcd.fillCircle(0.65*screenWidth, 0.75*screenHeight, 0.1*screenWidth, GREEN);
    if (isPressed({0.65*screenWidth, 0.75*screenHeight}, 0.1*screenWidth))
    {
      if (!onButton && alarmMinute > 0)
      {
        alarmMinute -= 1;
        onButton = true;
      }
    }
  
     //Button 4: (0.9*screenWidth, 0.75*screenHeight), R = 0.1*screenWidth
//     M5.Lcd.fillCircle(0.9*screenWidth, 0.75*screenHeight, 0.1*screenWidth, GREEN);
     if (isPressed({0.9*screenWidth, 0.75*screenHeight}, 0.1*screenWidth))
     {
        if (!onButton && alarmMinute < 60)
        {
          alarmMinute += 1;
          onButton = true;
        }
     }
  }
}

void secondScreenLayoutStatic()
{ 
  //4 boxes for setting the alarm 
                        //Starts            Initial                 step         height
  M5.Lcd.fillRect(0.55*screenWidth, 0.125*screenHeight, 0.2*screenWidth, 0.30 * screenHeight, OLIVE); //1st Rect
  M5.Lcd.fillRect(0.80*screenWidth, 0.125*screenHeight, 0.2*screenWidth, 0.30 * screenHeight, OLIVE); //2nd Rect
  M5.Lcd.fillRect(0.55*screenWidth, 0.6*screenHeight, 0.2*screenWidth, 0.30 * screenHeight, OLIVE); //3rd Rect
  M5.Lcd.fillRect(0.80*screenWidth, 0.6*screenHeight, 0.2*screenWidth, 0.30 * screenHeight, OLIVE); //4th rect

  // + and - symbols in those boxes

  //1st Box
  M5.Lcd.fillRect(0.605*screenWidth, 0.274*screenHeight, 0.09*screenWidth, 0.025*screenHeight, BLACK);

  //2nd Box
  M5.Lcd.fillRect(0.89*screenWidth, 0.220*screenHeight, 0.02*screenWidth, 0.12*screenHeight, BLACK);
  M5.Lcd.fillRect(0.85*screenWidth, 0.264*screenHeight, 0.09*screenWidth, 0.025*screenHeight, BLACK);

  //3rd Box
  M5.Lcd.fillRect(0.605*screenWidth, 0.749*screenHeight, 0.09*screenWidth, 0.025*screenHeight, BLACK);

  //4th Box
  M5.Lcd.fillRect(0.89*screenWidth, 0.695*screenHeight, 0.02*screenWidth, 0.12*screenHeight, BLACK);
  M5.Lcd.fillRect(0.85*screenWidth, 0.739*screenHeight, 0.09*screenWidth, 0.025*screenHeight, BLACK);

  //LEFT 1 Box for hours
  M5.Lcd.fillRect(0.285*screenWidth, 0.125*screenHeight, 0.2*screenWidth, 0.30 * screenHeight, LIGHTGREY);
  //LEFT 2 Box for minutes
  M5.Lcd.fillRect(0.285*screenWidth, 0.6*screenHeight, 0.2*screenWidth, 0.30 * screenHeight, LIGHTGREY);

  //"BUFFER TIME" text

   M5.Lcd.setTextSize(2.4);
   M5.Lcd.setTextColor(WHITE, BLACK);

  //Set Cursor
  M5.Lcd.setCursor(0.23 * screenWidth, 0.03 * screenHeight);
  M5.Lcd.print("Set Alarm TIme");

  //Hours and mins text
  M5.Lcd.setCursor(0.03 * screenWidth, 0.25 * screenHeight);
  M5.Lcd.print("Hours");

  M5.Lcd.setCursor(0.03 * screenWidth, 0.70 * screenHeight);
  M5.Lcd.print("Mins");

  M5.Lcd.fillCircle(0.1*screenWidth, 0.5*screenHeight, 0.1*screenWidth, BLUE);
  M5.Lcd.setCursor(0.03*screenWidth, 0.45*screenHeight);
  M5.Lcd.setTextSize(4.2);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.print("OK");
}

void secondScreenLayoutDynamic()
{
  //Hours and Mins time 
  M5.Lcd.setTextColor(BLACK, LIGHTGREY);
  M5.Lcd.setTextSize(4.8);
  
  M5.Lcd.setCursor(0.32 * screenWidth  , 0.25* screenHeight);
  M5.Lcd.printf("%0.2d",alarmHour);

  M5.Lcd.setCursor(0.32 * screenWidth , 0.70* screenHeight);
  M5.Lcd.printf("%0.2d",alarmMinute);
}

void thirdScreenLayoutStatic()
{
  //2 boxes for setting the alarm 
                        //Starts            Initial                 step         height
  M5.Lcd.fillRect(0.55*screenWidth, 0.125*screenHeight, 0.2*screenWidth, 0.30 * screenHeight, OLIVE); //1st Rect
  M5.Lcd.fillRect(0.80*screenWidth, 0.125*screenHeight, 0.2*screenWidth, 0.30 * screenHeight, OLIVE); //2nd Rect

  // + and - symbols in those boxes

  //1st Box
  M5.Lcd.fillRect(0.605*screenWidth, 0.274*screenHeight, 0.09*screenWidth, 0.025*screenHeight, BLACK);

  //2nd Box
  M5.Lcd.fillRect(0.89*screenWidth, 0.220*screenHeight, 0.02*screenWidth, 0.12*screenHeight, BLACK);
  M5.Lcd.fillRect(0.85*screenWidth, 0.264*screenHeight, 0.09*screenWidth, 0.025*screenHeight, BLACK);

  //LEFT 1 Box for minutes
  M5.Lcd.fillRect(0.285*screenWidth, 0.125*screenHeight, 0.2*screenWidth, 0.30 * screenHeight, LIGHTGREY);

  //"BUFFER TIME" text

   M5.Lcd.setTextSize(2.4);
   M5.Lcd.setTextColor(WHITE, BLACK);

  //Set Cursor
  M5.Lcd.setCursor(0.23 * screenWidth, 0.03 * screenHeight);
  M5.Lcd.print("Set Alarm TIme");

  //Hours and mins text
  M5.Lcd.setCursor(0.03 * screenWidth, 0.25 * screenHeight);
  M5.Lcd.print("Mins");

  M5.Lcd.fillCircle(0.1*screenWidth, 0.5*screenHeight, 0.1*screenWidth, BLUE);
  M5.Lcd.setCursor(0.03*screenWidth, 0.45*screenHeight);
  M5.Lcd.setTextSize(4.2);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.print("OK");
}

void thirdScreenLayoutDynamic()
{
  //Mins time
  M5.Lcd.setTextColor(BLACK, LIGHTGREY);
  M5.Lcd.setTextSize(4.8);
  
  M5.Lcd.setCursor(0.32 * screenWidth  , 0.25* screenHeight);
  M5.Lcd.printf("%0.2d",bufferMinute);
}

void thirdScreenButtonSystem()
{
  if (!M5.Touch.ispressed())
  {
    onButton = false;
  }

  //Confirmation Button: (0.1*screenWidth, 0.5*screenHeight), R = 0.1*screenWidth
  if (isPressed({0.1*screenWidth, 0.5*screenHeight}, 0.1*screenWidth))
  {
    progress = true;
  }
  else
  {
    //Button 1: (0.65*screenWidth, 0.275*screenHeight), R = 0.1*screenWidth
//    M5.Lcd.fillCircle(0.65*screenWidth, 0.275*screenHeight, 0.1*screenWidth, GREEN);
    if (isPressed({0.65*screenWidth, 0.275*screenHeight}, 0.1*screenWidth))
    {
      if (!onButton && bufferMinute > 0)
      {
        bufferMinute-=5;
        onButton = true;
      }
    }
  
    //Button 2: (0.9*screenWidth, 0.275*screenHeight), R = 0.1*screenWidth
//    M5.Lcd.fillCircle(0.9*screenWidth, 0.275*screenHeight, 0.1*screenWidth, GREEN);
    if (isPressed({0.9*screenWidth, 0.275*screenHeight}, 0.1*screenWidth))
    {
      if (!onButton && (bufferMinute + 5) <= min(90, alarmHour*60 + alarmMinute))
      {
        bufferMinute+=5;
        onButton = true;
      }
    }
  }
}

void alarmSystem()
{
//  M5.Lcd.setCursor(20,20);
//  M5.Lcd.print("Alarm Triggered");

  bool pass = false;
  while(!pass)
  {
    int* randomList = randomize(9);
    orderedSquares squares[9];
    
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        customPoint center;
        center.x = ((double) j * 0.33 + 0.167)*screenWidth;
        center.y = ((double) i * 0.33 + 0.167)*screenHeight;
        double radius = 0.166*screenHeight;
        squares[3*i + j] = orderedSquares(randomList[3*i + j], center, radius);
      }
    }

    int goal = 1;
    while(true)
    {
      if (goal > 9)
      {
        pass = true;
        progress = true;
        break;
      }
      
//       M5.Lcd.setCursor(0.2*screenWidth, 0.5*screenHeight);
//       M5.Lcd.setTextColor(WHITE, BLACK);
//       M5.Lcd.setTextSize(4);
//       M5.Lcd.printf("Goal: %2d", goal);
      if (M5.Touch.ispressed())
      {
        TouchPoint_t coordinate = M5.Touch.getPressPoint();
        int region = floor(coordinate.x / (screenWidth/3.0)) + 3 * floor(coordinate.y / (screenHeight/3.0));
//        M5.Lcd.setCursor(0.2*screenWidth, 0.5*screenHeight);
//        M5.Lcd.setTextColor(WHITE, BLACK);
//        M5.Lcd.setTextSize(4);
//        M5.Lcd.printf("Region: %2d", region);

        if (!squares[region].getStatus())
        {
          continue;
        }
        else
        {
          if (squares[region].trigger(goal))
          {
            goal++;
          }
//          else
//          {
//            break;
//          }
        }
      }
    }
    delete[] randomList;
  }
}

int* randomize(int n)
{
  int* list = new int(n+1);
  list[n] = -1;
  for (int i = 0; i < n; i++)
  {
    list[i] = i + 1;
  }

  for (int i = 0; i < random(1, 9); i++)
  {
    int a = random(1,9);
    int b = random(1,9);
    int temp = list[a];
    list[a] = list[b];
    list[b] = temp;
  }
  return list;
}
