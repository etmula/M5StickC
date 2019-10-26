#include <stdlib.h>
#include <AXP192.h>
#include <IMU.h>
#include <M5Display.h>
#include <M5StickC.h>
#include <RTC.h>
#include <math.h>
#include "settings.h"

void modeTime();
void modeSetting();
void modeRandom();
void modeStamp();
void modeStopwatch();
void modeCounter();
void print_dif(RTC_DateTypeDef d_st_b, RTC_TimeTypeDef t_st_b, RTC_DateTypeDef d_st_a, RTC_TimeTypeDef t_st_a);
void print_dice(int number1, int number2);


void setup() {
  M5.begin();
  setupSettings();
 
}

void loop() {
  modeTime();
  modeRandom();
  modeStamp();
  modeCounter();
  modeStopwatch();
  modeSetting();
}

void modeTime() {
    M5.update();
    Serial.println("mode -> Time");
    int recentTime = -1;
    M5.Lcd.setRotation(1);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0, 4);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setTextSize(1);
    while(true) {
        M5.update();
        M5.Rtc.GetTime(&TimeStruct);
        M5.Rtc.GetData(&DateStruct);
        if(TimeStruct.Seconds != recentTime){
          switch(modeTime_mode){
            case 0:
            Serial.println(DateStruct.WeekDay);
              M5.Lcd.setCursor(0, 10, 4);
              M5.Lcd.printf("'%02d/%02d/%02d-%s", DateStruct.Year%100, DateStruct.Month, DateStruct.Date, weekday[DateStruct.WeekDay]);
              M5.Lcd.setCursor(32, 46, 4);
              M5.Lcd.printf("%02d:%02d:%02d", TimeStruct.Hours, TimeStruct.Minutes, TimeStruct.Seconds);
              break;
            case 1:
              M5.Lcd.setCursor(18, 0, 4);
              M5.Lcd.printf("%04d/%02d/%02d", DateStruct.Year, DateStruct.Month, DateStruct.Date);
              M5.Lcd.setCursor(9, 26, 7);
              M5.Lcd.printf("%02d:%02d", TimeStruct.Hours, TimeStruct.Minutes);
              break;
            default:
              break;
          }
        }
        if(M5.BtnA.wasPressed()){
            modeTime_mode = ++modeTime_mode % 2;
            preferences.begin(preferenceName);
            preferences.putInt("modeTime_mode", modeTime_mode);
            preferences.end();
            Serial.print("changed to mode:");
            Serial.println(modeTime_mode);
            M5.Lcd.fillScreen(BLACK);
        }
        if(M5.BtnB.wasPressed()){
            break;
        }
        recentTime = TimeStruct.Seconds;
    }
}

void modeRandom(){
  M5.update();
  M5.Lcd.fillScreen(BLACK);
  Serial.println("mode -> Random");
  int dice_rand1 = 0;
  int dice_rand2 = 0;
  while(M5.BtnB.wasPressed() == false){
    while(M5.BtnA.wasPressed() == false && M5.BtnB.wasPressed() == false){
      M5.update();
      dice_rand1 = rand() % 6 + 1;
      dice_rand2 = rand() % 6 + 1;
      print_dice(dice_rand1, dice_rand2);
    }
    dice_rand1 = rand() % 6 + 1;
    dice_rand2 = rand() % 6 + 1;
    print_dice(dice_rand1, dice_rand2);
    if(M5.BtnB.wasPressed() == true){
      break;
    }
    M5.update();
    while(M5.BtnA.wasPressed() == false && M5.BtnB.wasPressed() == false){
      M5.update();
    }
    if(M5.BtnB.wasPressed() == true){
      break;
    }
    m5.update();
  }
}


void modeStamp() {
  int measure = 0;
  int recentTime = -1;

  M5.update();
  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setCursor(0, M5.Lcd.fontHeight(2), 2);
  for(int i = 0; i < 4; i++){
    int ye = stampDateStruct[i].Year;
    int mo = stampDateStruct[i].Month;
    int da = stampDateStruct[i].Date;
    int we = stampDateStruct[i].WeekDay;
    int ho = stampTimeStruct[i].Hours;
    int mi = stampTimeStruct[i].Minutes;
    int se = stampTimeStruct[i].Seconds;
    Serial.println("print");
    M5.Lcd.printf("%02d/%02d/%02d %02d:%02d:%02d +",ye%100, mo, da, ho, mi, se);
    print_dif(stampDateStruct[i+1], stampTimeStruct[i+1], stampDateStruct[i], stampTimeStruct[i]);
    M5.Lcd.println();
  }

  M5.Lcd.setTextColor(GREEN, BLACK);
  while(M5.BtnB.wasPressed() == false){
    M5.update();
    M5.Lcd.setCursor(0, 0, 2);
    M5.Rtc.GetTime(&TimeStruct);
    M5.Rtc.GetData(&DateStruct);
    if(M5.BtnA.isPressed() == true){
      measure += 1;
      delay(2);
      M5.Lcd.fillRect(0, 0, measure, M5.Lcd.fontHeight(2), DARKGREEN);
      M5.Lcd.setTextColor(GREEN);
    }else if(M5.BtnA.wasReleased() == true){
      measure = 0;
      M5.Lcd.fillRect(0, 0, M5.Lcd.width(), M5.Lcd.fontHeight(2), BLACK);
      M5.Lcd.setTextColor(GREEN, BLACK);
    }
    if(recentTime != TimeStruct.Seconds || M5.BtnA.isPressed() == true || M5.BtnA.wasReleased() == true){
      M5.Lcd.printf("%02d/%02d/%02d %02d:%02d:%02d\n",DateStruct.Year%100, DateStruct.Month, DateStruct.Date, TimeStruct.Hours, TimeStruct.Minutes, TimeStruct.Seconds);
    }
    recentTime = TimeStruct.Seconds;
    if(measure > 200){
      for(int i = 4; i > 0; i--){
        stampDateStruct[i] = stampDateStruct[i - 1];
        stampTimeStruct[i] = stampTimeStruct[i - 1];
        store_timestamp(i);
      }
      stampDateStruct[0].Year = DateStruct.Year;
      stampDateStruct[0].Month = DateStruct.Month;
      stampDateStruct[0].Date = DateStruct.Date;
      stampDateStruct[0].WeekDay = DateStruct.WeekDay;
      stampTimeStruct[0].Hours = TimeStruct.Hours;
      stampTimeStruct[0].Minutes = TimeStruct.Minutes;
      stampTimeStruct[0].Seconds = TimeStruct.Seconds;
      store_timestamp(0);
      M5.Lcd.fillRect(0, 0, measure, M5.Lcd.fontHeight(2), BLACK);
      delay(300);
      M5.Lcd.setTextColor(GREEN);
      M5.Lcd.setCursor(int(M5.Lcd.width()/2 - (M5.Lcd.textWidth("stamp!", 2) / 2)), 0, 2);
      M5.Lcd.print("stamp!");

      M5.Lcd.setTextColor(WHITE, BLACK);
      M5.Lcd.setCursor(0, M5.Lcd.fontHeight(2), 2);
      for(int i = 0; i < 4; i++){
        M5.Lcd.fillRect(0, (i+1) * M5.Lcd.fontHeight(2), measure, M5.Lcd.fontHeight(2), BLACK);
        delay(300);
        int ye = stampDateStruct[i].Year;
        int mo = stampDateStruct[i].Month;
        int da = stampDateStruct[i].Date;
        int we = stampDateStruct[i].WeekDay;
        int ho = stampTimeStruct[i].Hours;
        int mi = stampTimeStruct[i].Minutes;
        int se = stampTimeStruct[i].Seconds;
        M5.Lcd.printf("%02d/%02d/%02d %02d:%02d:%02d +",ye%100, mo, da, ho, mi, se);
        print_dif(stampDateStruct[i+1], stampTimeStruct[i+1], stampDateStruct[i], stampTimeStruct[i]);
        M5.Lcd.println();
      }
      measure = 0;
    }
  }
}

void modeStopwatch() {
  unsigned long timer = 0;
  unsigned long lap = 0;
  unsigned long split = 0;
  unsigned long last_timer = 0;
  unsigned long started_time;
  bool is_start = false;
  M5.update();
  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setCursor(0, 0, 4);

  while(!M5.BtnB.wasPressed()){
    M5.update();
    M5.Lcd.setCursor(0, 0, 4);
    M5.Lcd.printf("L %02d:%02d:%02d.%02d\n", lap%86400000/3600000, lap%3600000/60000, lap%60000/1000, lap%1000/10);
    M5.Lcd.printf("S %02d:%02d:%02d.%02d\n", split%86400000/3600000, split%3600000/60000, split%60000/1000, split%1000/10);
    M5.Lcd.printf("T %02d:%02d:%02d.%02d\n", timer%86400000/3600000, timer%3600000/60000, timer%60000/1000, timer%1000/10);
    
    if(M5.BtnA.wasPressed()){
      if(is_start){
        is_start = false;
      }else{
        started_time = millis() - timer;
        is_start = true;
      }
    }else if(M5.Axp.GetBtnPress()){
      if(is_start){
        lap = timer - last_timer;
        split = timer;
        last_timer = timer;
      }else{
        timer = 0;
        lap = 0;
        split = 0;
        last_timer = 0;
      }
    }else{
      if(is_start){
        timer = millis() - started_time;
      }
    }
  }
}

void modeCounter() {
  int current = 0;
  bool is_print = true;
  M5.update();
  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(BLACK);
  preferences.begin(preferenceName);
  while(!M5.BtnB.wasPressed()){
    M5.update();
    if(M5.BtnA.wasPressed()){
      counter[current]++;
      if(counter[current] > 99){
        counter[current] = 0;
      }
      is_print = true;
      preferences.putInt("counter0", counter[0]);
      preferences.putInt("counter1", counter[1]);
    }else if(M5.Axp.GetBtnPress() == 2){
      current = ++current % 2;
      is_print = true;
    }else if(M5.BtnA.pressedFor(1000) && counter[current] != 0){
      counter[current] = 0;
      is_print = true;
      preferences.putInt("counter0", counter[0]);
      preferences.putInt("counter1", counter[1]);
    }else if(M5.BtnA.pressedFor(2000) && counter[(current+1)%2] != 0){
      counter[(current+1) % 2] = 0;
      is_print = true;
      preferences.putInt("counter0", counter[0]);
      preferences.putInt("counter1", counter[1]);
    }
    if(is_print){
      if(current == 0){
        M5.Lcd.setTextColor(WHITE, LIGHTGREY);
      }else{
        M5.Lcd.setTextColor(WHITE, BLACK);
      }
      M5.Lcd.setCursor(int(M5.Lcd.width()/4 - (M5.Lcd.textWidth("00", 6) / 2)), int(M5.Lcd.height()/2 - (M5.Lcd.fontHeight(6) / 2)), 6);
      M5.Lcd.printf("%02d", counter[0]);

      M5.Lcd.setCursor(int(M5.Lcd.width()/2 - (M5.Lcd.textWidth(":", 6) / 2)), int(M5.Lcd.height()/2 - (M5.Lcd.fontHeight(6) / 2)), 6);
      M5.Lcd.setTextColor(WHITE, BLACK);
      M5.Lcd.print(":");

      if(current == 1){
        M5.Lcd.setTextColor(WHITE, LIGHTGREY);
      }else{
        M5.Lcd.setTextColor(WHITE, BLACK);
      }
      M5.Lcd.setCursor(int(M5.Lcd.width()*3/4 - (M5.Lcd.textWidth("00", 6) / 2)), int(M5.Lcd.height()/2 - (M5.Lcd.fontHeight(6) / 2)), 6);
      M5.Lcd.printf("%02d", counter[1]);
      is_print = false;
    }
  }
  preferences.end();
}


void modeSetting() {
    M5.update();
    Serial.println("mode -> Setting");
    M5.Lcd.setRotation(1);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0, 4);
    M5.Lcd.setTextColor(WHITE, BLACK);
    //M5.Lcd.setCursor(int(M5.Lcd.width()/2 - (M5.Lcd.textWidth("SETTING", 4) / 2)), int(M5.Lcd.height()/2 - (M5.Lcd.fontHeight(4) / 2)), 4);
    M5.Lcd.drawCentreString("SETTING", M5.Lcd.width()/2, 0, 4);
    //M5.Lcd.setCursor(int(M5.Lcd.width()/2 - (M5.Lcd.textWidth("START ->", 4) / 2)), int(M5.Lcd.height()/2 - (M5.Lcd.fontHeight(4) / 2)), 4);
    M5.Lcd.drawCentreString("START ->", M5.Lcd.width()/2, M5.Lcd.fontHeight(4), 4);

    while(true) {
        M5.update();
        if(M5.BtnA.wasPressed()){
            startSetting();
        }
        if(M5.BtnB.wasPressed()){
            break;
        }
    }
}

void print_dif(RTC_DateTypeDef d_st_b, RTC_TimeTypeDef t_st_b, RTC_DateTypeDef d_st_a, RTC_TimeTypeDef t_st_a) {
  int ye_dif = d_st_a.Year - d_st_b.Year;
  int mo_dif = d_st_a.Month - d_st_b.Month;
  int da_dif = d_st_a.Date - d_st_b.Date;
  int we_dif = d_st_a.WeekDay - d_st_b.WeekDay;
  int ho_dif = t_st_a.Hours - t_st_b.Hours;
  int mi_dif = t_st_a.Minutes - t_st_b.Minutes;
  int se_dif = t_st_a.Seconds - t_st_b.Seconds;
  Serial.println("dif_str");
  char dif_str[4] = "?";
  if(ye_dif){
    sprintf(dif_str, "%02dY", ye_dif % 100);
  }else if(mo_dif){
    sprintf(dif_str, "%02dM", mo_dif);
  }else if(da_dif){
    sprintf(dif_str, "%02dD", da_dif);
  }else if(ho_dif){
    sprintf(dif_str, "%02dh", ho_dif);
  }else if(mi_dif){
    sprintf(dif_str, "%02dm", mi_dif);
  }else if(se_dif){
    sprintf(dif_str, "%02ds", se_dif);
  }
  Serial.println(dif_str);
  M5.Lcd.printf("%s", dif_str);
}


void print_dice(int number1, int number2) {
    M5.Lcd.setRotation(1);
    M5.Lcd.fillRoundRect(10, 10, 60, 60, 2, WHITE);
    M5.Lcd.fillRoundRect(90, 10, 60, 60, 2, WHITE);
    int dice_rad = 7;
    int gap = 80;
    switch(number1){
      case 1:
        M5.Lcd.fillCircle(40, 40, 2 * dice_rad, RED);
        break;
      case 2:
        M5.Lcd.fillCircle(25, 25, dice_rad, BLACK);
        M5.Lcd.fillCircle(55, 55, dice_rad, BLACK);
        break;
      case 3:
        M5.Lcd.fillCircle(25, 25, dice_rad, BLACK);
        M5.Lcd.fillCircle(55, 55, dice_rad, BLACK);
        M5.Lcd.fillCircle(40, 40, dice_rad, BLACK);
        break;
      case 4:
        M5.Lcd.fillCircle(25, 25, dice_rad, BLACK);
        M5.Lcd.fillCircle(55, 25, dice_rad, BLACK);
        M5.Lcd.fillCircle(25, 55, dice_rad, BLACK);
        M5.Lcd.fillCircle(55, 55, dice_rad, BLACK);
        break;
      case 5:
        M5.Lcd.fillCircle(25, 25, dice_rad, BLACK);
        M5.Lcd.fillCircle(55, 25, dice_rad, BLACK);
        M5.Lcd.fillCircle(25, 55, dice_rad, BLACK);
        M5.Lcd.fillCircle(55, 55, dice_rad, BLACK);
        M5.Lcd.fillCircle(40, 40, dice_rad, BLACK);
        break;
      case 6:
        M5.Lcd.fillCircle(25, 25, dice_rad, BLACK);
        M5.Lcd.fillCircle(40, 25, dice_rad, BLACK);
        M5.Lcd.fillCircle(55, 25, dice_rad, BLACK);
        M5.Lcd.fillCircle(25, 55, dice_rad, BLACK);
        M5.Lcd.fillCircle(40, 55, dice_rad, BLACK);
        M5.Lcd.fillCircle(55, 55, dice_rad, BLACK);
        break;
      default:
        break;
    }
    switch(number2){
      case 1:
        M5.Lcd.fillCircle(40 + gap, 40, 2 * dice_rad, RED);
        break;
      case 2:
        M5.Lcd.fillCircle(25 + gap, 25, dice_rad, BLACK);
        M5.Lcd.fillCircle(55 + gap, 55, dice_rad, BLACK);
        break;
      case 3:
        M5.Lcd.fillCircle(25 + gap, 25, dice_rad, BLACK);
        M5.Lcd.fillCircle(55 + gap, 55, dice_rad, BLACK);
        M5.Lcd.fillCircle(40 + gap, 40, dice_rad, BLACK);
        break;
      case 4:
        M5.Lcd.fillCircle(25 + gap, 25, dice_rad, BLACK);
        M5.Lcd.fillCircle(55 + gap, 25, dice_rad, BLACK);
        M5.Lcd.fillCircle(25 + gap, 55, dice_rad, BLACK);
        M5.Lcd.fillCircle(55 + gap, 55, dice_rad, BLACK);
        break;
      case 5:
        M5.Lcd.fillCircle(25 + gap, 25, dice_rad, BLACK);
        M5.Lcd.fillCircle(55 + gap, 25, dice_rad, BLACK);
        M5.Lcd.fillCircle(25 + gap, 55, dice_rad, BLACK);
        M5.Lcd.fillCircle(55 + gap, 55, dice_rad, BLACK);
        M5.Lcd.fillCircle(40 + gap, 40, dice_rad, BLACK);
        break;
      case 6:
        M5.Lcd.fillCircle(25 + gap, 25, dice_rad, BLACK);
        M5.Lcd.fillCircle(40 + gap, 25, dice_rad, BLACK);
        M5.Lcd.fillCircle(55 + gap, 25, dice_rad, BLACK);
        M5.Lcd.fillCircle(25 + gap, 55, dice_rad, BLACK);
        M5.Lcd.fillCircle(40 + gap, 55, dice_rad, BLACK);
        M5.Lcd.fillCircle(55 + gap, 55, dice_rad, BLACK);
        break;
      default:
        break;
    }
  }