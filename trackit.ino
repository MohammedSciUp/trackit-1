//ThatsEngineering
//Sending Data from Arduino to NodeMCU Via Serial Communication
//NodeMCU code

//Include Lib for Arduino to Nodemcu
#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wiegand.h>
#include <DHT.h>
WIEGAND wg;
unsigned long int pinD0 = D5; 
unsigned long int pinD1 = D6; 
 int x1;

TinyGPSPlus gps;
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
SoftwareSerial ss(RXPin, TXPin);

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 30000;  //the value is a number of milliseconds

unsigned long startMillis2;  //some global variables available anywhere in the program
unsigned long currentMillis2;
const unsigned long period2 = 9000;  //the value is a number of milliseconds


//Initialisation of DHT11 Sensor
#define DHTPIN D4
DHT dht(DHTPIN, DHT11);
float temp;
float hum;


 
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
int a ; 
// Insert your network credentials
#define WIFI_SSID "trackit"
#define WIFI_PASSWORD "00000000"

// Insert Firebase project API Key
#define API_KEY "AIzaSyBJWTrMZJ86yfP8LyLhq2d4KIQOMpSv7Jk"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://testing-d7987-default-rtdb.firebaseio.com"


FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;


//D6 = Rx & D5 = Tx








void setup() {
  // Initialize Serial port
  Serial.begin(115200);
  dht.begin();
  wg.begin(pinD0,pinD1);
  ss.begin(GPSBaud);
  
  while (!Serial) continue;
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }

   /// ----------------------------- /// 
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
 a = 0; 
startMillis= millis();  //initial start time
startMillis2= millis();
}

void loop() {


           if (wg.available()) {
     for (int i=a ; i < 1000; i++){
      x1 = wg.getCode();
      Serial.println(x1);
      Firebase.RTDB.setInt(&fbdo, "TestStation/test/int"+String (i) ,x1);
      
      if (wg.getCode() == x1) {
        a = i+1;
       break; 
       }
       else {continue;
       }}}



//      while (ss.available() > 0)
//      if (gps.encode(ss.read())){
      currentMillis2 = millis();
      if (currentMillis2 - startMillis2 >= period2){
      Serial.println("..................................");
        Serial.println("gps data: "); 
        double LATITUDE  = gps.location.lat();
        Serial.println("latitude : ");
        Serial.print(LATITUDE);
        Serial.println("....");
        Firebase.RTDB.setDouble(&fbdo, "TestStation/gpsinfo/LONGITUDE" ,LATITUDE);
        double LONGITUDE  = gps.location.lng();
        Serial.println("longitude : ");
        Serial.print(LONGITUDE);
        Serial.println("....");

        Firebase.RTDB.setDouble(&fbdo, "TestStation/gpsinfo/LATITUDE" ,LATITUDE);
        double SPEED      = gps.speed.kmph();   
        Serial.println("speed : ");
        Serial.print(SPEED);
        Serial.println("....");

        Firebase.RTDB.setDouble(&fbdo, "TestStation/gpsinfo/SPEED" ,SPEED);
        unsigned int TIME = gps.time.value(); // Raw time in HHMMSSCC format (u32)
        Serial.println("time : ");
        Serial.print(TIME);
        Serial.println("....");

        Firebase.RTDB.setDouble(&fbdo, "TestStation/gpsinfo/TIME" ,TIME);
        unsigned int DATE = gps.date.value(); // Raw date in DDMMYY format (u32)
        Serial.println("date : ");
        Serial.print(DATE);
        Serial.println("....");

        Firebase.RTDB.setDouble(&fbdo, "TestStation/gpsinfo/DATE" ,DATE);
        startMillis2 = currentMillis2 ; 
                Serial.println("..................................");
      }
//      }}

      currentMillis = millis();
      if (currentMillis - startMillis >= period){
     dht11_func();
     Serial.println("humadity : ");
     Serial.print(hum);
     Serial.println("....");
     Serial.println("temprature : ");
     Serial.print(temp);
     Serial.println("....");

     Firebase.RTDB.setInt(&fbdo, "TestStation/hdt/temp" ,temp);
     Firebase.RTDB.setInt(&fbdo, "TestStation/hdt/hum" ,hum);
     startMillis = currentMillis ; 
      }


     
}
void dht11_func(){
 hum = dht.readHumidity();
 temp = dht.readTemperature();}
