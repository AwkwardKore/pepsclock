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

#define  BLACK   0x0000 //WHITE
#define BLUE    0x001F //YELLOW
#define RED     0xF800 //CYAN
#define GREEN   0x07E0 //MAGENTA
#define CYAN    0x07FF //RED
#define MAGENTA 0xF81F //GREEN
#define YELLOW  0xFFE0 //BLUE
#define WHITE   0xFFFF //BLACK

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define BOXSIZE 40
#define PENRADIUS 3

//Variables
int currentHour, currentMinute, currentSecond, currentDay, currentMonth, currentYear, pastSecond;
String dayTime, alarmTime;
char digit, isThereAlarm;
int interval = 1, minutesAlarm, hourAlarm;

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
  isThereAlarm = 'n';
  pinMode(19, OUTPUT);
  digitalWrite(19, LOW);
}

void loop () {
  tft.setTextColor(RED);
  checkForAlarm();
  alarmSound();
  displayDate();
  displayHour();
}

//Checks if there is any alarm available. Received from the ruby script
void checkForAlarm () {
  tft.setTextColor(RED,WHITE);
  tft.setTextSize(3);
  if (Serial.available() > 0) {
    isThereAlarm = Serial.read();
    if (isThereAlarm != 'n') {
      alarmTime.concat(isThereAlarm);
      for (int x = 0 ; x < 3 ; x++) {
        if (Serial.available() > 0) {
          isThereAlarm = Serial.read();
          alarmTime.concat(isThereAlarm);
        }
      }
    }
  }
  if (isThereAlarm == 'n') {
    tft.fillRect(10,150,219,25, WHITE);
    tft.fillRect(30, 200, 150, 40, WHITE);
    tft.fillRect(45, 250, 150, 40, WHITE);
    alarmTime = "";
    digitalWrite(19, LOW);
  }
  if (isThereAlarm != 'n') {
      hourAlarm = alarmTime.substring(0,2).toInt();
      minutesAlarm = alarmTime.substring(2,4).toInt();
      tft.setCursor(20,150);
      tft.print("Alarm:");
      if (hourAlarm < 10) {
        tft.print("0");
      }
      tft.print(hourAlarm);
      tft.print(":");
      if (minutesAlarm < 10) {
        tft.print("0");
      }
      tft.print(minutesAlarm);
      tft.setTextColor(WHITE, RED);
      tft.setCursor(60,260);
      tft.print("SILENCE");
      digitalWrite(13, HIGH);
      TSPoint p = ts.getPoint();
      digitalWrite(13, LOW);
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
      if (p.z > 10 && p.z < 1000) {
        if (p.x > 305 && p.x < 775) {
          if (p.y > 715  && p.y < 820) {
            isThereAlarm = 'n';
            Serial.write("r");
            digitalWrite(19, LOW);
          }
        }
      }
  }
}

void alarmSound () {
  tft.setTextSize(3);
  tft.setTextColor(RED);
  if (isThereAlarm != 'n') {
    if (minutesAlarm == minute() && hourAlarm == hour()) {
      digitalWrite(19, HIGH);
    }
  }
  else {
    digitalWrite(19, LOW);
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
  tft.setTextColor(RED,WHITE);
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
}

//Display Date
void displayDate () {
  tft.setTextColor(RED);
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

