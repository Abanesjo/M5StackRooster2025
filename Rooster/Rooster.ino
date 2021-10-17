#include <M5Core2.h>

double screenHeight, screenWidth;
int selectMode;
int xCursor, yCursor;
String placeHolderTime;

void setup() 
{
  M5.begin();
  M5.Lcd.begin();
  M5.Lcd.fillScreen(BLACK);
  screenHeight = M5.Lcd.height();
  screenWidth = M5.Lcd.width();
  xCursor = M5.Lcd.getCursorX();
  yCursor = M5.Lcd.getCursorY();
  M5.Lcd.setTextSize(7);
  M5.Lcd.setTextColor(BLACK, LIGHTGREY);
}


void loop() 
{
  homeScreenLayout();
  delay(200);
}


void homeScreenLayout()
{                 //Starts        Initial      step         height
  M5.Lcd.fillRect(0.1*screenWidth, 0.125*screenHeight, 0.8*screenWidth, 0.35 * screenHeight, LIGHTGREY);
  
  while (true) {
    
    M5.Lcd.setCursor(0.175 * screenWidth, 0.19 * screenHeight);
    placeHolderTime = "10:49";

    //Printing the Time
    M5.Lcd.print(placeHolderTime);
    //break function if touch screen detected
  }
}
