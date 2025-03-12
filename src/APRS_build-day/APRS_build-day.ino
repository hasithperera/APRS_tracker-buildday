

#include <stdio.h>
#include <SoftwareSerial.h>

#include <LibAPRS_Tracker.h>

#define ADC_REFERENCE REF_5V
#define OPEN_SQUELCH false

/* Used Pins */

#define radio_ppt 2  //not needed



#define sim_packet 11


// old 150
#define timeout 600



//APRS specification
#define symbol '>'
#define SSID 9

//APRS symbol
  // S - shuttle
  // < - Bike
  //O - Balloon
  //> - car

// station SSID
  // 9 - Mobile
  // 11 - Spacecraft

//#define simulate 1

String packetBuffer;
SoftwareSerial gps(10, 11);  // RX, TX


// W8CUL location
char Lat[] = "xxxx.xxxxxx";
char Lon[] = "xxxxx.xxxxxxxx";
char alt[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

char final_msg[100];

int time_share = 0;
int msg_id = 0;
int msg_valid = 0;
char myCALL[] = "KE8TJE";
float freq_tx;

int packet_id = 0;

void setup() {

  Serial.begin(9600);  // for logging

  Serial.println("[info] KE8TJE - APRS Buildday tracker v1.4");
  Serial.println("[info] IO init");

  //start GPS
  gps.begin(9600);

}

void loop() {


  gps.stopListening();

  if (msg_id > 0 & msg_valid == 1) {
    location_update();
    Serial.println("SEND APRS");
  } else {
    Serial.println("[info] ------------------------ No location data");
  }
  gps.listen();

  while (time_share < timeout) {
    while (gps.available() > 0) {
      time_share += 1;
      String gps_raw = gps.readStringUntil('\n');
      Serial.println(gps_raw);

      // Valid data: $GPGLL,3938.28486,N,07957.13511,W,191757.00,A,A*7D
      if (gps_raw.substring(0, 6) == "$GPGLL") {

        Serial.println(gps_raw);

        //simulated loc
        //gps_raw = "$GPGLL,3938.28486,N,07957.13511,W,191757.00,A,A*7D";


        if (gps_raw.length() > 30) {
          msg_id++;
          update_GPS_v2(gps_raw);

        }
      }

      //long packet - code
      // v2 - "$GPGGA"
      // v3 - "$GNGGA"
      //Serial.println(gps_raw);
      if (gps_raw.substring(0, 6) == "$GNGGA"){
        
        // debug packet
        //gps_raw = "$GNGGA,165006.000,2241.9107,N,12017.2383,E,1,14,0.79,22.6,M,18.5,M,,*42";
        
        Serial.println(gps_raw);
        update_GPS_alt(gps_raw);
      }
      
    }
  }
  time_share = 0;
}

void update_GPS(String gps_data) {
  char test_data[100];
  gps_data.toCharArray(test_data, 100);

  // V2 - Packet
  // $GPGLL,3927.83254,N,0808.25462,W,130448.00,A,A*71

  // V3 - Packet
  // $GNRMC,134055.000,A,3509.7572,N,09010.4938,W,1.51,338.00,121023,,,A*6C

  char *p = strtok(test_data, ",");  //code
  p = strtok(NULL,","); //time
  p = strtok(NULL,","); //validifty

  //Serial.print("Data validity:");
  //Serial.println(p);

  if (p[0]=='A') {
    Serial.println("[i] GPS valid");
  } else {
    Serial.println("[!] Invalid data");
    return;
  }
  //Process longitude
  // bug fix for v2 (location packet)

  p = strtok(NULL, ",");  //lat
  sprintf(Lat, "%s", p);

  // bug fix in v2
  if (Lat[4] == '.') {
    Lat[7] = '\0';
  } else {
    Lat[8] = '\0';
  }
  p = strtok(NULL, ",");  // lat_char
  sprintf(Lat, "%s%s\0", Lat, p);

  // Process latitude
  // bug fix for v2 (location packet)
  p = strtok(NULL, ",");  //lng
  sprintf(Lon, "%s", p);

  if (Lon[4] == '.') {
    Lon[7] = '\0';
  } else {
    Lon[8] = '\0';
  }

  p = strtok(NULL, ",");  //dir
  sprintf(Lon, "%s%s\0", Lon, p);

  p = strtok(NULL, ",");  //state
  //sprintf(alt, "NEBP-WV ");
  //Serial.println("Short packet:");
  Serial.println(Lat);
  Serial.println(Lon);
  
  
  msg_valid = 1;
}

void update_GPS_v2(String gps_data) {
  //process GPGLL packets
  char test_data[100];
  gps_data.toCharArray(test_data, 100);

  // V2 - Packet
  // $GPGLL,3927.83254,N,0808.25462,W,130448.00,A,A*71

  // V3 - Packet
  // $GNRMC,134055.000,A,3509.7572,N,09010.4938,W,1.51,338.00,121023,,,A*6C

  char *p = strtok(test_data, ",");  //code
  //p = strtok(NULL,","); //time
  //p = strtok(NULL,","); //validifty

  //Serial.print("Data validity:");
  //Serial.println(p);

  /*if (p[0]=='A') {
    Serial.println("[i] GPS valid");
  } else {
    Serial.println("[!] Invalid data");
    return;
  }*/
  //Process longitude
  // bug fix for v2 (location packet)

  p = strtok(NULL, ",");  //lat
  sprintf(Lat, "%s", p);

  // bug fix in v2
  if (Lat[4] == '.') {
    Lat[7] = '\0';
  } else {
    Lat[8] = '\0';
  }
  p = strtok(NULL, ",");  // lat_char
  sprintf(Lat, "%s%s\0", Lat, p);

  // Process latitude
  // bug fix for v2 (location packet)
  p = strtok(NULL, ",");  //lng
  sprintf(Lon, "%s", p);

  if (Lon[4] == '.') {
    Lon[7] = '\0';
  } else {
    Lon[8] = '\0';
  }

  p = strtok(NULL, ",");  //dir
  sprintf(Lon, "%s%s\0", Lon, p);

  p = strtok(NULL, ",");  //state


  //sprintf(alt, "NEBP-WV ");
  //Serial.println("Short packet:");
  Serial.println(Lat);
  Serial.println(Lon);
  
  
  msg_valid = 1;
}

void update_GPS_alt(String gps_data) {

  //data is sent with 2 decimal places
  // reformat the data to be used by the APRS library
  //"$GPGGA,191757.00,3938.28486,N,07957.13511,W,1,03,2.71,274.5,M,-33.9,M,,*6F";
  // v3
  // $GNGGA,165006.000,2241.9107,N,12017.2383,E,1,14,0.79,22.6,M,18.5,M,,*42

  char test_data[100];
  gps_data.toCharArray(test_data, 100);

  char *p = strtok(test_data, ",");  //code
  p = strtok(NULL, ",");             //time


  p = strtok(NULL, ",");             //lat
  sprintf(Lat, "%s", p);

  // bug fix in v2
  if (Lat[4] == '.') {
    Lat[7] = '\0';
  } else {
    Lat[8] = '\0';
  }

  
  p = strtok(NULL, ",");  // lat_char
  sprintf(Lat, "%s%s\0", Lat, p);

  //p = strtok(NULL, ",");             //lng
  //p = strtok(NULL, ",");             //dir

  p = strtok(NULL, ",");  //lng
  sprintf(Lon, "%s", p);

  if (Lon[4] == '.') {
    Lon[7] = '\0';
  } else {
    Lon[8] = '\0';
  }

  p = strtok(NULL, ",");  //dir
  sprintf(Lon, "%s%s\0", Lon, p);

  p = strtok(NULL, ",");  //state
  sprintf(alt,"ARC Roadtripping,");
  
  p = strtok(NULL, ",");    //sta-no
  strcat(alt,p);
  strcat(alt,",");
  p = strtok(NULL, ",");    //horizontal
  strcat(alt,p);
  strcat(alt,",alt=");
  p = strtok(NULL, ",");    //alti
  strcat(alt,p);
  p = strtok(NULL, ",");    //alti-unit
  strcat(alt,p);
  

  //Serial.print("alt len:");
  //strcpy(alt,final_msg);
  msg_valid = 1;
}

int location_update() {
  int wait = 400;


  //radio_TX();
  Serial.println("[info] APRS:start");
  char comment[30];

  sprintf(comment, "NEBP-WV msg_id:%d", msg_id);
  Serial.println(comment);
  time_share = 0;
  APRS_init();

  APRS_setPreamble(300);
  APRS_setCallsign(myCALL, SSID);  

  APRS_setLat(Lat);
  APRS_setLon(Lon);

  // APRS icon - setting
  APRS_setSymbol(symbol);  

  
  //delay(100);
  
  APRS_sendLoc(comment, strlen(comment), ' ');

  //sprintf(alt,"%s,%d",alt,packet_id++);
  //APRS_sendLoc(alt, strlen(alt), ' ');
  

  delay(1200);

  Serial.println("[info] APRS:end");
  //Serial.println(alt);
  msg_valid = 0;
  //gps.flush();
}