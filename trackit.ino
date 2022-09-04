//ThatsEngineering
//Sending Data from Arduino to NodeMCU Via Serial Communication
//NodeMCU code

//Include Lib for Arduino to Nodemcu
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wiegand.h>
#include <DHT.h>
WIEGAND wg;
unsigned long int pinD0 = D5; 
unsigned long int pinD1 = D6; 
 int x1;

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 30000;  //the value is a number of milliseconds

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
#define WIFI_SSID "GLORIA.JEANS"
#define WIFI_PASSWORD "yourname"

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
  Serial.begin(9600);
  dht.begin();
  wg.begin(pinD0,pinD1);
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
}

void loop() {

      currentMillis = millis();
      if (currentMillis - startMillis >= period){
     dht11_func();
     Firebase.RTDB.setInt(&fbdo, "hdt/temp" ,temp);
     Firebase.RTDB.setInt(&fbdo, "hdt/hum" ,hum);
      }


     
     if (wg.available()) {
     for (int i=a ; i < 1000; i++){
      x1 = wg.getCode();
      Firebase.RTDB.setInt(&fbdo, "test/int"+String (i) ,x1);
      
      if (wg.getCode() == x1) {
        a = i+1;
       break; 
       }
       else {continue;
       }}}}
void dht11_func(){
 hum = dht.readHumidity();
 temp = dht.readTemperature();}
