#include <cassert>
#include <cstdio>
#include <string>
#include "../GPSPhoneWeb/gps_state.h"
uint32_t testNow=1000;
uint32_t millis(){return testNow;}
void feed(GPSState& s,const std::string& body){
 unsigned char checksum=0;for(char c:body)checksum^=c;
 char end[8];snprintf(end,sizeof(end),"*%02X\r\n",checksum);
 for(char c:std::string("$")+body+end)s.feed(c,testNow);
}
int main(){
 GPSState s;assert(!s.receiving(testNow));assert(!s.live(testNow));
 feed(s,"GPTXT,01,01,01,ANTENNA OK");
 feed(s,"GNGGA,,,,,,0,00,25.5,,,,,,");
 feed(s,"GNRMC,,V,,,,,,,,,,N,V");
 assert(s.receiving(testNow));assert(!s.live(testNow));assert(!s.timePresent);
 assert(s.gps.satellites.value()==0);
 testNow=2000;
 feed(s,"GNRMC,082416.00,A,0121.1250,N,10349.1900,E,0.2,0.0,181126,,,A");
 assert(!s.live(testNow)); // Last GGA still explicitly says no fix.
 feed(s,"GNGGA,082416.00,0121.1250,N,10349.1900,E,1,09,0.9,22.8,M,0.0,M,,");
 assert(s.live(testNow));assert(s.timePresent);
 assert(fabs(s.gps.location.lat()-1.352083333)<0.000001);
 assert(fabs(s.gps.location.lng()-103.819833333)<0.000001);
 testNow=2100;
 feed(s,"GNRMC,082417.00,V,,,,,,,,,,N,V");
 assert(!s.live(testNow)); // Do not reuse retained last-known position.
 testNow=2200;
 feed(s,"GNRMC,082418.00,A,0121.1250,N,10349.1900,E,0.2,0.0,181126,,,A");
 assert(s.live(testNow));
 testNow=7200;assert(!s.live(testNow));assert(!s.receiving(testNow));
 GPSState gp;
 testNow=8000;
 feed(gp,"GPGGA,082416.00,0121.1250,S,10349.1900,W,1,09,0.9,22.8,M,0.0,M,,");
 assert(gp.live(testNow));assert(gp.gps.location.lat()<0);assert(gp.gps.location.lng()<0);
 for(char c:std::string("$GPGGA,,,,,,0,00,25.5,,,,,,*00\r\n"))gp.feed(c,testNow);
 assert(gp.live(testNow)); // Bad checksum cannot overwrite state.
 testNow=0xffffff00;GPSState wrap;
 feed(wrap,"GPGGA,082416.00,0121.1250,N,10349.1900,E,1,09,0.9,22.8,M,0.0,M,,");
 testNow=100;assert(wrap.live(testNow));
 puts("GPS tests passed: no-fix, fix, explicit loss, recovery, timeout, GN/GP, hemispheres, checksum, timer wrap.");
}
