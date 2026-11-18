#pragma once
#include <TinyGPS++.h>
#include <stdint.h>
#include <string.h>

// TinyGPS++ retains the last valid position even after a no-fix sentence.
// Track current GGA/RMC status separately so that position is never called live.
class GPSState {
 public:
  TinyGPSPlus gps;
  TinyGPSCustom gnQuality, gpQuality, gnStatus, gpStatus;
  TinyGPSCustom gnGgaTime, gpGgaTime, gnRmcTime, gpRmcTime;
  bool seenSentence=false, seenGga=false, seenRmc=false;
  bool ggaFix=false, rmcFix=false, timePresent=false;
  uint32_t lastSentence=0, lastGga=0, lastRmc=0;
  static constexpr uint32_t FRESH_MS=5000;
  GPSState(): gnQuality(gps,"GNGGA",6),gpQuality(gps,"GPGGA",6),
    gnStatus(gps,"GNRMC",2),gpStatus(gps,"GPRMC",2),
    gnGgaTime(gps,"GNGGA",1),gpGgaTime(gps,"GPGGA",1),
    gnRmcTime(gps,"GNRMC",1),gpRmcTime(gps,"GPRMC",1) {}
  void feed(char c, uint32_t now) {
    if(!gps.encode(c)) return; // checksum validated by TinyGPS++
    seenSentence=true; lastSentence=now;
    for(auto field:{&gnQuality,&gpQuality}) if(field->isUpdated()) {
      const char* quality=field->value();
      ggaFix=quality[0]>='1' && quality[0]<='5';
      seenGga=true;lastGga=now;
    }
    for(auto field:{&gnStatus,&gpStatus}) if(field->isUpdated()) {
      rmcFix=strcmp(field->value(),"A")==0;
      seenRmc=true;lastRmc=now;
    }
    for(auto field:{&gnGgaTime,&gpGgaTime,&gnRmcTime,&gpRmcTime})
      if(field->isUpdated()) timePresent=strlen(field->value())>=6;
  }
  bool receiving(uint32_t now) const {
    return seenSentence && uint32_t(now-lastSentence)<FRESH_MS;
  }
  bool live(uint32_t now) {
    bool ggaRecent=seenGga && uint32_t(now-lastGga)<FRESH_MS;
    bool rmcRecent=seenRmc && uint32_t(now-lastRmc)<FRESH_MS;
    return receiving(now) && gps.location.isValid() && gps.location.age()<FRESH_MS
      && (ggaRecent||rmcRecent) && (!ggaRecent||ggaFix) && (!rmcRecent||rmcFix);
  }
};
