#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "gps_state.h"
#include "web_page.h"

// Same separate-wire connection as the first GPS test: GPS TX -> D7.
constexpr int GPS_RX_PIN=44; // XIAO ESP32-S3 D7
constexpr int GPS_TX_PIN=43; // D6, may remain disconnected
constexpr uint32_t GPS_BAUD=9600;
const char* AP_SSID="ESP32-GPS";
const char* AP_PASSWORD="GPStracker24"; // Change before sharing; at least 8 characters.
WebServer server(80);
GPSState state;
String rawLog, line;
bool droppingLine=false;
bool webReady=false;
uint32_t lastSerialStatus=0;
// Enable to also print every complete received NMEA sentence.
constexpr bool PRINT_RAW_NMEA=false;

void printSerialStatus() {
  if(!Serial) return; // Keep GPS/web service running when USB monitor is closed.
  const uint32_t now=millis();
  auto &gps=state.gps;
  Serial.println("\n--- ESP32 GPS tracker ---");
  if(webReady) {
    Serial.print("Wi-Fi: ");Serial.print(AP_SSID);
    Serial.print(" | Open http://");Serial.println(WiFi.softAPIP());
    Serial.print("Phones connected: ");Serial.println(WiFi.softAPgetStationNum());
  } else Serial.println("Wi-Fi web server did not start. Reset the board to retry.");
  Serial.print("GPS: ");
  Serial.println(state.receiving(now)?"NMEA received":"No recent valid NMEA; check GPS TX -> D7, 3V3 and GND");
  Serial.print("UART characters: ");Serial.print(gps.charsProcessed());
  Serial.print(" | Checksum failures: ");Serial.println(gps.failedChecksum());
  Serial.print("Satellites used: ");
  if(state.receiving(now) && gps.satellites.isValid() && gps.satellites.age()<5000)
    Serial.println(gps.satellites.value());
  else Serial.println("unknown");
  if(state.live(now)) {
    Serial.print("FIX | Latitude: ");Serial.print(gps.location.lat(),6);
    Serial.print(" | Longitude: ");Serial.println(gps.location.lng(),6);
  } else Serial.println("NO FIX | Take the antenna outdoors, ceramic face toward the sky.");
}

String jsonEscape(const String& input) {
  String out;out.reserve(input.length()+32);
  for(size_t i=0;i<input.length();++i) {
    unsigned char c=input[i];
    if(c=='"') out+="\\\"";
    else if(c=='\\') out+="\\\\";
    else if(c=='\n') out+="\\n";
    else if(c=='\r') out+="\\r";
    else if(c>=32 && c<127) out+=char(c);
  }
  return out;
}
String numeric(bool valid,double value,int digits=1) {
  return valid && isfinite(value)?String(value,digits):String("null");
}
void sendGPS() {
  const uint32_t now=millis();
  const bool receiving=state.receiving(now), fix=state.live(now);
  auto &gps=state.gps;
  char utc[20]="";
  if(receiving && state.timePresent && gps.time.isValid() && gps.time.age()<5000)
    snprintf(utc,sizeof(utc),"%02u:%02u:%02u UTC",gps.time.hour(),gps.time.minute(),gps.time.second());
  String data;data.reserve(1800);
  data="{\"fix\":";data+=fix?"true":"false";
  data+=",\"receiving\":";data+=receiving?"true":"false";
  data+=",\"latitude\":";data+=numeric(fix,gps.location.lat(),6);
  data+=",\"longitude\":";data+=numeric(fix,gps.location.lng(),6);
  data+=",\"satellites\":";data+=numeric(receiving&&gps.satellites.isValid()&&gps.satellites.age()<5000,gps.satellites.value(),0);
  data+=",\"hdop\":";data+=numeric(receiving&&gps.hdop.isValid()&&gps.hdop.age()<5000,gps.hdop.hdop());
  data+=",\"speed_kmh\":";data+=numeric(fix&&gps.speed.isValid()&&gps.speed.age()<5000,gps.speed.kmph());
  data+=",\"altitude_m\":";data+=numeric(fix&&gps.altitude.isValid()&&gps.altitude.age()<5000,gps.altitude.meters());
  data+=",\"fix_age_ms\":";data+=fix?String(gps.location.age()):String("null");
  data+=",\"utc\":\"";data+=utc;
  data+="\",\"raw\":\"";data+=jsonEscape(rawLog);data+="\"}";
  server.sendHeader("Cache-Control","no-store");
  server.send(200,"application/json",data);
}
void setup() {
  Serial.begin(115200);
  Serial1.setRxBufferSize(2048);
  Serial1.begin(GPS_BAUD,SERIAL_8N1,GPS_RX_PIN,GPS_TX_PIN);
  rawLog.reserve(1200);line.reserve(180);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IPAddress(192,168,4,1),IPAddress(192,168,4,1),IPAddress(255,255,255,0));
  if(!WiFi.softAP(AP_SSID,AP_PASSWORD)) {
    Serial.println("Could not start GPS Wi-Fi access point.");return;
  }
  server.on("/",HTTP_GET,[]{
    server.sendHeader("Cache-Control","no-store");
    server.send_P(200,"text/html; charset=utf-8",GPS_PAGE);
  });
  server.on("/api/gps",HTTP_GET,sendGPS);
  server.onNotFound([]{server.send(404,"text/plain","Not found");});
  server.begin();
  webReady=true;
  Serial.println("GPS web reader ready. Join ESP32-GPS and open http://192.168.4.1");
}
void loop() {
  while(Serial1.available()) {
    const char c=char(Serial1.read());state.feed(c,millis());
    if(c=='\n') {
      if(!droppingLine && line.length()) {
        if(PRINT_RAW_NMEA && Serial) Serial.println(line);
        rawLog+=line;rawLog+='\n';
        while(rawLog.length()>1100) {
          int end=rawLog.indexOf('\n');
          if(end<0){rawLog="";break;}rawLog.remove(0,end+1);
        }
      }
      line="";droppingLine=false;
    } else if(c!='\r' && !droppingLine) {
      if(line.length()<180)line+=c;else{line="";droppingLine=true;}
    }
  }
  if(webReady) server.handleClient();
  if(uint32_t(millis()-lastSerialStatus)>=2000) {
    lastSerialStatus=millis();
    printSerialStatus();
  }
  delay(1);
}
