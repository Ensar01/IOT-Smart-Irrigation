
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>  // Include firebase library
#include <addons/TokenHelper.h>       
#include "DHT.h"          
 
#define DPIN 4        
#define DTYPE DHT11   // Definisi DHT11
#define vlaznost_tla_pin A0
#define LED D4

          
#define WIFI_SSID "YourWifiSSID"                                  
#define WIFI_PASSWORD "YourWifiPassword"

#define API_KEY "YourApiKey"
#define DATABASE_URL "YourDatabaseUrl" 



FirebaseData fbdo;       // define Data object
FirebaseAuth auth;       // define Firebase authentication object
FirebaseConfig config;   // define configuration object

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;                     //since we are doing an anonymous sign in 
const int relay = 5;
int statusPumpe = 0;

DHT dht(DPIN,DTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(relay, OUTPUT);
  pinMode(LED, OUTPUT);

 WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Print Firebase client version
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  // Assign the API key
  config.api_key = API_KEY;

   /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }


  // Assign the callback function for the long-running token generation task
  config.token_status_callback = tokenStatusCallback;  // see addons/TokenHelper.h

  // Begin Firebase with configuration and authentication
  Firebase.begin(&config, &auth);

  // Reconnect to Wi-Fi if necessary
  Firebase.reconnectWiFi(true);
}

 

void loop() {
  
  String documentPath = "EspData/DHT11"; 
  String documentPath2 = "EspData/SoilMoisture"; 
   FirebaseJson content;                   // Create a FirebaseJson object for storing data


  float tc = dht.readTemperature(false);  //Read temperature in C
    //Read Temperature in F
  float hu = dht.readHumidity();          //Read Humidity
  int vlaznostTlaUProcentima = (100.00 -( (analogRead(vlaznost_tla_pin)/1023.00)*100.00));

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)){
    //since we want the data to be updated every second
    sendDataPrevMillis = millis();
    // Enter Temperature in to the DHT_11 Table
    if (Firebase.RTDB.setInt(&fbdo, "DHT_11/Temperatura", tc)){
      // This command will be executed even if you dont serial print but we will make sure its working
      Serial.print("Temperatura: ");
      Serial.println(tc);
    }
    else {
      Serial.println("Failed to Read from the Sensor");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    
    // Enter Humidity in to the DHT_11 Table
    if (Firebase.RTDB.setFloat(&fbdo, "DHT_11/Vlaznost_zrala", hu)){
      Serial.print("Vlaznost zraka : ");
      Serial.print(hu);
    }
    else {
      Serial.println("Failed to Read from the Sensor");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setFloat(&fbdo, "Soil_moisture_sensor/Vlaznost tla", vlaznostTlaUProcentima)){
      Serial.print("Vlaznost tla : ");
      Serial.print(vlaznostTlaUProcentima);
    }
    else {
      Serial.println("Failed to Read from the Sensor");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.getInt(&fbdo, "Status_pumpe")){
      if(fbdo.dataType()=="int")
      {
        statusPumpe = fbdo.intData();
        if(statusPumpe==1)
        {
          digitalWrite(relay,HIGH);
        }
        else
        {
          digitalWrite(relay,LOW);
        }
      }
    }
    else {
      Serial.println("REASON: " + fbdo.errorReason());
    }
     
  }
}