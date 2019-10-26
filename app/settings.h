#include <M5Display.h>
#include <M5StickC.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include "WebServer.h"
#include <Preferences.h>
#include "time.h"

const IPAddress apIP(192, 168, 4, 1);
const char *apIPADDRESS = "http://192.168.4.1";
const char *apSSID = "SETTING";
const char *apPASSWD = "password";
const char *preferenceName = "config";
const char *preferenceKeys[] = {"WIFI_SSIF", "WIFI_PASSWD"};
const uint8_t FONT = 1;//8px ASCII
const char *ntpServer = "";
const char weekday[7][4] = {{"Sun"}, {"Mon"}, {"Tue"}, {"Wed"}, {"Thu"}, {"Fri"}, {"Sat"}};

//settings
String wifi_ssid;
String wifi_password;


//local variables
String ssidList;
bool settingmode;
int modeTime_mode;
int counter[2];
//timestamps
RTC_TimeTypeDef stampTimeStruct[5];
RTC_DateTypeDef stampDateStruct[5];


// DNSServer dnsServer;
WebServer webServer(80);
// wifi config store
Preferences preferences;

//m5StickC RTC
RTC_TimeTypeDef TimeStruct;
RTC_DateTypeDef DateStruct;

void startSetting();
void setupSettings();
void restoreConfig();
boolean tryConnection();
void startWebServer();
String makePage(String title, String contents);
String urlDecode(String input);

void startSetting() {
  M5.update();
  Serial.println("startSetting");
  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  delay(100);
  for (int i = 0; i < n; ++i) {
    ssidList += "<option value=\"";
    ssidList += WiFi.SSID(i);
    ssidList += "\">";
    ssidList += WiFi.SSID(i);
    ssidList += "</option>";
  }
  delay(100);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(apSSID, apPASSWD);
  WiFi.mode(WIFI_MODE_AP);
  startWebServer();
  // WiFi.softAPConfig(IPAddress local_ip, IPAddress gateway, IPAddress subnet);
  // WiFi.softAP(const char* ssid, const char* passphrase = NULL, int channel = 1, int ssid_hidden = 0);
  // dnsServer.start(53, "*", apIP);
  Serial.print("Starting Access Point at \"");
  Serial.print(apSSID);
  Serial.println("\"");
  M5.Lcd.setRotation(0);
  M5.Lcd.fillScreen(WHITE);
  M5.Lcd.setCursor(0, 0, FONT);
  M5.Lcd.setTextColor(BLACK, WHITE);
  M5.Lcd.println("1. Take out your iPhone or Android.");
  M5.lcd.println("2. Open Setting App.");
  M5.Lcd.println("3. Open WiFi Setting.");
  M5.Lcd.println("4. Connect WiFi\n");
  M5.Lcd.printf("SSID\n\"%s\"\n", apSSID);
  M5.Lcd.printf("PASSWORD\n\"%s\"\n\n", apPASSWD);
  M5.Lcd.println("5. Press Decide");
  M5.update();
  while(M5.BtnA.wasPressed() == false){
    M5.update();
    webServer.handleClient();
  }
  M5.Lcd.fillScreen(WHITE);
  M5.Lcd.setCursor(0, 0, FONT);
  M5.Lcd.println("6. Scan QR or open URL");
  M5.Lcd.printf("\"%s\"\n", apIPADDRESS);
  M5.Lcd.println("7. Setting in your device");
  M5.Lcd.println("8. Restart automaticaly or yourself.");
  M5.Lcd.qrcode(apIPADDRESS, 0, 80, 80, 2);

  while(true){
    webServer.handleClient();
  }
}

void setupSettings() {
  //setupにおいて、設定を反映する
  restoreConfig();
   if(tryConnection()) {
     Serial.println("connection successed!");
   }else{
     Serial.println("Connection failed!");
   }
}

void restoreConfig() {
//WiFi設定をpreferencesから読み込み、WiFi設定をする
//return　WiFi設定がpreferencesに存在したらtrue
  Serial.println("restore config");
  preferences.begin(preferenceName);
  wifi_ssid = preferences.getString("WIFI_SSID");
  wifi_password = preferences.getString("WIFI_PASSWD");
  modeTime_mode = preferences.getInt("modeTime_mode");
  counter[0] = preferences.getInt("counter0");
  counter[1] = preferences.getInt("counter1");
  stampDateStruct[0].Year    = preferences.getInt("stamp_year_0");
  stampDateStruct[0].Month   = preferences.getInt("stamp_month_0");
  stampDateStruct[0].Date    = preferences.getInt("stamp_day_0");
  stampDateStruct[0].WeekDay = preferences.getInt("stamp_week_0");
  stampTimeStruct[0].Hours   = preferences.getInt("stamp_hour_0");
  stampTimeStruct[0].Minutes = preferences.getInt("stamp_minute_0");
  stampTimeStruct[0].Seconds = preferences.getInt("stamp_second_0");
  stampDateStruct[1].Year    = preferences.getInt("stamp_year_1");
  stampDateStruct[1].Month   = preferences.getInt("stamp_month_1");
  stampDateStruct[1].Date    = preferences.getInt("stamp_day_1");
  stampDateStruct[1].WeekDay = preferences.getInt("stamp_week_1");
  stampTimeStruct[1].Hours   = preferences.getInt("stamp_hour_1");
  stampTimeStruct[1].Minutes = preferences.getInt("stamp_minute_1");
  stampTimeStruct[1].Seconds = preferences.getInt("stamp_second_1");
  stampDateStruct[2].Year    = preferences.getInt("stamp_year_2");
  stampDateStruct[2].Month   = preferences.getInt("stamp_month_2");
  stampDateStruct[2].Date    = preferences.getInt("stamp_day_2");
  stampDateStruct[2].WeekDay = preferences.getInt("stamp_week_2");
  stampTimeStruct[2].Hours   = preferences.getInt("stamp_hour_2");
  stampTimeStruct[2].Minutes = preferences.getInt("stamp_minute_2");
  stampTimeStruct[2].Seconds = preferences.getInt("stamp_second_2");
  stampDateStruct[3].Year    = preferences.getInt("stamp_year_3");
  stampDateStruct[3].Month   = preferences.getInt("stamp_month_3");
  stampDateStruct[3].Date    = preferences.getInt("stamp_day_3");
  stampDateStruct[3].WeekDay = preferences.getInt("stamp_week_3");
  stampTimeStruct[3].Hours   = preferences.getInt("stamp_hour_3");
  stampTimeStruct[3].Minutes = preferences.getInt("stamp_minute_3");
  stampTimeStruct[3].Seconds = preferences.getInt("stamp_second_3");
  stampDateStruct[4].Year    = preferences.getInt("stamp_year_4");
  stampDateStruct[4].Month   = preferences.getInt("stamp_month_4");
  stampDateStruct[4].Date    = preferences.getInt("stamp_day_4");
  stampDateStruct[4].WeekDay = preferences.getInt("stamp_week_4");
  stampTimeStruct[4].Hours   = preferences.getInt("stamp_hour_4");
  stampTimeStruct[4].Minutes = preferences.getInt("stamp_minute_4");
  stampTimeStruct[4].Seconds = preferences.getInt("stamp_second_4");
  preferences.end();
}

boolean tryConnection() {
//WiFi設定が完了したらtrueを返す
//15秒間応答なしなら、falseを返す
  Serial.println("try to connect WiFi");
  Serial.print("WIFI-SSID: ");
  Serial.println(wifi_ssid);
  Serial.print("WIFI-PASSWD: ");
  Serial.println(wifi_password);
  WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
  int count = 0;
  Serial.print("Waiting for Wi-Fi connection");
  while ( count < 30 ) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.println("Connected!");
      return (true);
    }
    delay(500);
    Serial.print(".");
    count++;
  }
  Serial.println("Timed out.");
  return false;
}

void startWebServer() {
  ///settingsに設定ページを表示
    Serial.print("Starting Web Server at ");
    Serial.println(WiFi.softAPIP());
    webServer.on("/settings", []() {
      String s = "<h1>Settings</h1><p>Please enter your password by selecting the SSID.</p>";
      s += "<form method=\"get\" action=\"setSettings\">";
      s += "SSID: <select name=\"ssid\">" + ssidList + "</select><br>";
      s += "Password: <input name=\"pass\" length=64 type=\"password\"><input type=\"submit\"><br>";
      s += "</form>";
      s += "<form method=\"get\" action=\"setTime\">";
      s += "<p>Set Time from NTP Server</p>";
      s += ntpServer;
      s += "<input type=\"submit\">";
      s += "</form>";
      webServer.send(200, "text/html", makePage("Wi-Fi Settings", s));
    });
    webServer.on("/setWiFi", []() {
      Serial.println("setWiFi");
      wifi_ssid = urlDecode(webServer.arg("ssid"));
      Serial.print("SSID: ");
      Serial.println(wifi_ssid);
      wifi_password = urlDecode(webServer.arg("pass"));
      Serial.print("Password: ");
      Serial.println(wifi_password);
      preferences.begin(preferenceName);
      Serial.println("Writing Password to nvr...");
      preferences.putString("WIFI_SSID", wifi_ssid);
      preferences.putString("WIFI_PASSWD", wifi_password);
      preferences.end();
      String s = "<h1>Setup complete.</h1>";
      s += "restart...";
      webServer.send(200, "text/html", makePage("Setting", s));
      delay(3000);
      ESP.restart();
    });
    webServer.on("/setTime", []() {
      Serial.println("setTime");
      // Set ntp time to local
      configTime(9 * 3600, 0, ntpServer);
      // Get local time
      struct tm timeInfo;
      if (getLocalTime(&timeInfo)) {
        // Set RTC time
        RTC_TimeTypeDef TimeStruct;
        TimeStruct.Hours   = timeInfo.tm_hour;
        TimeStruct.Minutes = timeInfo.tm_min;
        TimeStruct.Seconds = timeInfo.tm_sec;
        M5.Rtc.SetTime(&TimeStruct);

        RTC_DateTypeDef DateStruct;
        DateStruct.WeekDay = timeInfo.tm_wday;
        DateStruct.Month = timeInfo.tm_mon + 1;
        DateStruct.Date = timeInfo.tm_mday;
        DateStruct.Year = timeInfo.tm_year + 1900;
        M5.Rtc.SetData(&DateStruct);
      }else{
        Serial.println("setup failed!");
      }
      String s = "<h1>Setup complete.</h1>";
      s += "restart...";
      webServer.send(200, "text/html", makePage("Setting", s));
      delay(3000);
      ESP.restart();
    });
    webServer.onNotFound([]() {
      String s = "<h1>AP mode</h1><p><a href=\"/settings\">Wi-Fi Settings</a></p>";
      webServer.send(200, "text/html", makePage("AP mode", s));
    });

    webServer.begin();
}


String makePage(String title, String contents) {
  String s = "<!DOCTYPE html><html><head>";
  s += "<meta name=\"viewport\" content=\"width=device-width,user-scalable=0\">";
  s += "<title>";
  s += title;
  s += "</title></head><body>";
  s += contents;
  s += "</body></html>";
  return s;
}

void store_timestamp(int number) {
  preferences.begin(preferenceName);
  switch(number) {
    case 0:
      preferences.putInt("stamp_year_0", int(stampDateStruct[0].Year));
      preferences.putInt("stamp_month_0", int(stampDateStruct[0].Month));
      preferences.putInt("stamp_day_0", int(stampDateStruct[0].Date));
      preferences.putInt("stamp_week_0", int(stampDateStruct[0].WeekDay));
      preferences.putInt("stamp_hour_0", int(stampTimeStruct[0].Hours));
      preferences.putInt("stamp_minute_0", int(stampTimeStruct[0].Minutes));
      preferences.putInt("stamp_second_0", int(stampTimeStruct[0].Seconds));
      break;
    case 1:
      preferences.putInt("stamp_year_1", stampDateStruct[1].Year);
      preferences.putInt("stamp_month_1", stampDateStruct[1].Month);
      preferences.putInt("stamp_day_1", stampDateStruct[1].Date);
      preferences.putInt("stamp_week_1", stampDateStruct[1].WeekDay);
      preferences.putInt("stamp_hour_1", stampTimeStruct[1].Hours);
      preferences.putInt("stamp_minute_1", stampTimeStruct[1].Minutes);
      preferences.putInt("stamp_second_1", stampTimeStruct[1].Seconds);
      break;
    case 2:
      preferences.putInt("stamp_year_2", stampDateStruct[2].Year);
      preferences.putInt("stamp_month_2", stampDateStruct[2].Month);
      preferences.putInt("stamp_day_2", stampDateStruct[2].Date);
      preferences.putInt("stamp_week_2", stampDateStruct[2].WeekDay);
      preferences.putInt("stamp_hour_2", stampTimeStruct[2].Hours);
      preferences.putInt("stamp_minute_2", stampTimeStruct[2].Minutes);
      preferences.putInt("stamp_second_2", stampTimeStruct[2].Seconds);
      break;
    case 3:
      preferences.putInt("stamp_year_3", stampDateStruct[3].Year);
      preferences.putInt("stamp_month_3", stampDateStruct[3].Month);
      preferences.putInt("stamp_day_3", stampDateStruct[3].Date);
      preferences.putInt("stamp_week_3", stampDateStruct[3].WeekDay);
      preferences.putInt("stamp_hour_3", stampTimeStruct[3].Hours);
      preferences.putInt("stamp_minute_3", stampTimeStruct[3].Minutes);
      preferences.putInt("stamp_second_3", stampTimeStruct[3].Seconds);
      break;
    case 5:
      preferences.putInt("stamp_year_4", stampDateStruct[4].Year);
      preferences.putInt("stamp_month_4", stampDateStruct[4].Month);
      preferences.putInt("stamp_day_4", stampDateStruct[4].Date);
      preferences.putInt("stamp_week_4", stampDateStruct[4].WeekDay);
      preferences.putInt("stamp_hour_4", stampTimeStruct[4].Hours);
      preferences.putInt("stamp_minute_4", stampTimeStruct[4].Minutes);
      preferences.putInt("stamp_second_4", stampTimeStruct[4].Seconds);
      break;
    default:
      break;
  }
  preferences.end();
}

String urlDecode(String input) {
  String s = input;
  s.replace("%20", " ");
  s.replace("+", " ");
  s.replace("%21", "!");
  s.replace("%22", "\"");
  s.replace("%23", "#");
  s.replace("%24", "$");
  s.replace("%25", "%");
  s.replace("%26", "&");
  s.replace("%27", "\'");
  s.replace("%28", "(");
  s.replace("%29", ")");
  s.replace("%30", "*");
  s.replace("%31", "+");
  s.replace("%2C", ",");
  s.replace("%2E", ".");
  s.replace("%2F", "/");
  s.replace("%2C", ",");
  s.replace("%3A", ":");
  s.replace("%3A", ";");
  s.replace("%3C", "<");
  s.replace("%3D", "=");
  s.replace("%3E", ">");
  s.replace("%3F", "?");
  s.replace("%40", "@");
  s.replace("%5B", "[");
  s.replace("%5C", "\\");
  s.replace("%5D", "]");
  s.replace("%5E", "^");
  s.replace("%5F", "-");
  s.replace("%60", "`");
  return s;
}