#include <TimeLib.h>
#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SPFD5408_TouchScreen.h>
// *** SPFD5408 change -- End

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6

#define TS_MINX 125
#define TS_MINY 85
#define TS_MAXX 965
#define TS_MAXY 905

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4

#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define BOXSIZE 40
#define PENRADIUS 3

//Variables
int currentHour, currentMinute, currentSecond, currentDay, currentMonth, currentYear, pastSecond;
String dayTime, booleanAlarm;
char digit;
int interval = 1;

void setup () {
  Serial.begin(9600);
  tft.reset();

  tft.begin(0x9341);
  tft.setRotation(1);
  drawBorder();
  for (int x = 0 ; x < 19 ; x++) {
    getCurrentTime();
  }
  setSistemTime();
}

void loop () {
  tft.setTextColor(RED);
  checkForAlarm();
  displayDate();
  displayHour();
}

//Checks if there is any alarm available. Received from the ruby script
void checkForAlarm () {
  if (Serial.available() > 0) {
    booleanAlarm = Serial.read();
  }
  if (booleanAlarm == "48") {
    tft.fillRect(10,150,219,25, WHITE);
  }
  if (booleanAlarm == "49") {
    tft.setCursor(45,150);
    tft.print("Alarm set");
  }
}

// Draw a border
void drawBorder () {
  uint16_t width = tft.width() - 1;
  uint16_t height = tft.height() - 1;
  uint8_t border = 10;

  tft.fillScreen(RED);
  tft.fillRect(border, border, (width - border * 2), (height - border * 2), WHITE);
}

//Set the current time of the global system
void setSistemTime() {
  currentHour = dayTime.substring(0,2).toInt();
  currentMinute = dayTime.substring(3,5).toInt();
  currentSecond = dayTime.substring(6,8).toInt();
  currentDay = dayTime.substring(9,11).toInt();
  currentMonth = dayTime.substring(12,14).toInt();
  currentYear = dayTime.substring(15,19).toInt();
  setTime(currentHour,currentMinute,currentSecond,currentDay,currentMonth,currentYear);
}

//Get the actual time from serial input. Written by ruby script
void getCurrentTime () {
  if (Serial.available() > 0) {
    //Read the most recent byte
    digit = Serial.read();
    dayTime.concat(digit);
  }
}

//Display hour
void displayHour () {
  tft.setTextSize (2);
  tft.setCursor(50,50);
  tft.print("Current time");
  tft.setCursor(50,80);
  tft.setTextSize (3);
  if (hour() < 10) {
    tft.print(0);
  }
  tft.print(hour());
  tft.print(":");
  if (minute() < 10) {
    tft.print(0);
  }
  tft.print(minute());
  tft.print(":");
  if (second() < 10) {
    tft.print(0);
  }
  tft.print(second());
  delay(1000);
  tft.fillRect(10,80,219,25, WHITE);
}

//Display Date
void displayDate () {
  tft.setCursor(15,15);
  tft.setTextSize(1);
  tft.print("Today is ");
  displayMonth(month());
  tft.print(" ");
  tft.print(day());
  tft.print(" ");
  tft.print(year());
}

//Print the current month
void displayMonth (int m) {
  switch (m) {
    case 1:
      tft.print("January");
      break;
    case 2:
      tft.print("February");
      break;
    case 3:
      tft.print("March");
      break;
    case 4:
      tft.print("April");
      break;
    case 5:
      tft.print("May");
      break;
    case 6:
      tft.print("June");
      break;
    case 7:
      tft.print("July");
      break;
    case 8:
      tft.print("August");
      break;
    case 9:
      tft.print("September");
      break;
    case 10:
      tft.print("November");
      break;
    case 11:
      tft.print("December");
      break;
    case 12:
      tft.print("January");
      break;
  }
}
