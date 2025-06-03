// Author:KE8TJE (ke8tje@gmail.com)
// March 2025

#include <stdio.h>
#include <SoftwareSerial.h>

#include <LibAPRS_Tracker.h>

#define ADC_REFERENCE REF_5V

#define radio_ppt 3  //not needed

//APRS specification
#define symbol '$'
#define SSID 9

#define APRS_TX_offset_utc_sec 14 // this selects what UTC second the packet will be sent out
#define APRS_interval_count 100  // 100 is 1 min

//APRS symbol
// S - shuttle
// < - Bike
// O - Balloon
// > - car

// station SSID
// 9 - Mobile
// 11 - Spacecraft


/*NOTE: Uncomment the following line to put the unit to simulation mode*/
#define sim 1

String packetBuffer;
SoftwareSerial gps(10, 11);  // RX, TX

char Lat[] = "xxxx.xxxxxx";
char Lon[] = "xxxxx.xxxxxxxx";


char test_data[100];

int msg_id = 0;
int msg_valid = 0;
int loc_valid = 0;

char myCALL[] = "KE8TJE";
float freq_tx;

int time_trigger = 0;
float lon, lat, utc;
int sec,min; 

int telemetry[6];
char ext[12];

void setup() {

  Serial.begin(9600);  // for logging and debugging
  Serial.println("[info]APRS Buildday tracker v1.43 - W8CUL/KE8TJE");

  #ifdef sim
    Serial.println("Unit is in Debug mode. GPS data will be simulated");
    Serial.println("For testing only");
  #endif

  pinMode(2,OUTPUT);
  digitalWrite(2,HIGH);
  delay(500);
  digitalWrite(2,LOW);
  delay(500);
  digitalWrite(2,HIGH);
  delay(500);
  digitalWrite(2,LOW);

  //start GPS
  gps.begin(9600);
}

void loop() {

  gps.listen();
  while (gps.available() > 0) {
    String gps_raw = gps.readStringUntil('\n');
    //Serial.println(gps_raw);
    // Valid data: $GPGLL,3938.28486,N,07957.13511,W,191757.00,A,A*7D
    if (gps_raw.substring(0, 6) == "$GPGLL") {
      
      #ifdef sim
        gps_raw = "$GPGLL,3938.28486,N,07957.13511,W,191757.00,A,A*7D";
      #endif
      Serial.println(gps_raw);
      gps_raw.toCharArray(test_data, 100);
      char *p = strtok(test_data, ",");
      update_GPS(p);
    }


  }
}

void base91_telemetry(int a_values){
  // 
  for(int i=0;i<(a_values+1)*2;i+=2){
    ext[i] = (int)(telemetry[i/2]/(91))+33;
    ext[i+1] = (int)(telemetry[i/2]%(91))+33;
  }
  Serial.println(ext);
}

void update_GPS(char *p) {

  p = strtok(NULL, ",");  //lat - <1>

  sprintf(Lat, "%s", p);

  // bug fix in v2
  if (Lat[4] == '.') {
    Lat[7] = '\0';
  } else {
    Lat[8] = '\0';
  }

  p = strtok(NULL, ",");  // lat_dir - <2>
  // handel south
  sprintf(Lat, "%s%s\0", Lat, p);


  p = strtok(NULL, ",");  //lng - <3>
  //formatted string for APRS update

  sprintf(Lon, "%s", p);
  if (Lon[4] == '.') {
    Lon[7] = '\0';
  } else {
    Lon[8] = '\0';
  }
  p = strtok(NULL, ",");           // lon_dir - <4>
  sprintf(Lon, "%s%s\0", Lon, p);  //line to process string for APRS


  p = strtok(NULL, ",");  //time - <5>
  utc = atof(p);

  p = strtok(NULL, ",");  //status- <6>



  // need GPS testing - TJE
  if (p[0] == 'A') {
    msg_valid = 1;
    //togrid(lat, lon);
  }else{
    return;
  }

  #ifdef sim
    sec = (sec+1);
    if(sec==60) sec =0;
  #endif
    
  #ifndef sim
    sec = long(utc) % 100;
    min = long(utc) % 10000 - sec;
  #endif

  Serial.print(min);
  Serial.print(",");
  Serial.println(sec);
  //else {
  // msg_valid = 0;
  //}

  if (sec == APRS_TX_offset_utc_sec & (min % APRS_interval_count) == 0 & msg_valid) {
    Serial.println("[i] Event trigger");
    gps.stopListening();

    //delay(5000);
    location_update(); //typical APRS location packet


    msg_valid = 0;
    gps.listen();
  }
}


int location_update() {

  Serial.println("[info] APRS:start");
  Serial.print(Lat);
  Serial.print(',');
  Serial.print(Lon);

  char comment[30];


  // telemetry functions unused

  telemetry[0] = msg_id;
  telemetry[1] = msg_id+50;
  base91_telemetry(1);    
  // variable name : ext

  // Users please do not chnage the version number

  sprintf(comment, "v1.43 | Testing TJE"); 
  // sprintf(comment, "v1.50|%s|",ext); 
 
  
  Serial.println(comment);

  APRS_init();

  APRS_setPreamble(700);
  APRS_setCallsign(myCALL, SSID);

  APRS_setLat(Lat);
  APRS_setLon(Lon);

  // APRS icon - setting

  APRS_setSymbol(symbol);
  APRS_sendLoc(comment, strlen(comment), ' ');
  while(bitRead(PORTB,5)); 
  delay(800);


  Serial.println("[info] APRS:end");
  msg_id++;
}
