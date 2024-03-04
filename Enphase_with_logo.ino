
#include <SPI.h>
#include <Wire.h>
#include <ArduinoJson.h>

#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// SDA GPIO4
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

// Remplacez ces informations par vos propres paramètres WiFi et MQTT
const char* ssid = "XXX"; // Remplacez par le nom de votre réseau WiFi
const char* passwordW = "XXXX"; // Remplacez par le mot de passe de votre réseau WiFi

//NTPClient timeClient(ntpUDP);
WiFiUDP ntpUDP;
// ete 
//NTPClient timeClient(ntpUDP, "fr.pool.ntp.org", 7200, 60000);
NTPClient timeClient(ntpUDP, "fr.pool.ntp.org", 3600, 60000);


//PASS ENVOY
const char *username = "installer";
const char *password = "XXX";
const char *server = "http://XXXXXX";  // ADRESSE IP ENVOY
const char *uri = "/stream/meter";

const int hourstart =6;
const int hourstop =20;
float Compteur =0 ;
float Power =0 ;
float AReal_power = 0;
float AReactive_power = 0;
float AApparent_power = 0;
float Avoltage = 0;
float Acurrent =0 ;
float pf = 0;
float freq = 0;

float tcAReal_power = 0;
float tcAReactive_power = 0;
float tcAApparent_power = 0;
float tcAvoltage = 0;
float tcAcurrent =0 ;

float Consumption = 0; 
float NetPower = 0;

WiFiClient client;
HTTPClient http;

const uint8_t prise [] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xe0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf0, 
0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xe0, 0x18, 
0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xe0, 0x18, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xf8, 
0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xf0, 0x00, 0x0e, 0x00, 0x1c, 0x7f, 0xff, 0xe0, 0x00, 
0x00, 0x3f, 0x80, 0x7f, 0x7f, 0xff, 0xe0, 0x00, 0x00, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xe0, 0x00, 
0x01, 0xf3, 0xe3, 0xe3, 0xff, 0xff, 0xe0, 0x00, 0x03, 0xc0, 0xff, 0x81, 0xff, 0xff, 0xe0, 0x00, 
0x0f, 0x80, 0x7e, 0x00, 0x7f, 0xff, 0xe0, 0x00, 0x1f, 0x00, 0x1c, 0x00, 0x7f, 0xff, 0xff, 0xf0, 
0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xe0, 0x18, 
0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xe0, 0x18, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xf8, 
0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t solaire [] PROGMEM = {
0x3f, 0xff, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf0, 0x3f, 0xff, 0x00, 0x00, 0xc0, 0x07, 0xff, 0xf0, 
0x3f, 0xff, 0x00, 0x03, 0x80, 0x07, 0xff, 0xf0, 0x00, 0x1f, 0x00, 0x07, 0x00, 0x07, 0xc0, 0x00, 
0x00, 0x1f, 0x80, 0x0e, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x1f, 0x80, 0x1f, 0xf0, 0x0f, 0xc0, 0x00, 
0x00, 0x0f, 0xc0, 0x1f, 0xf0, 0x0f, 0x80, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x70, 0x1f, 0x80, 0x00, 
0x00, 0x1f, 0xe0, 0x00, 0xe0, 0x3f, 0xc0, 0x00, 0x00, 0x3f, 0xf0, 0x01, 0xc0, 0x7f, 0xe0, 0x00, 
0x00, 0xff, 0xfc, 0x03, 0x80, 0xff, 0xf8, 0x00, 0x01, 0xf1, 0xff, 0x03, 0x03, 0xfc, 0x3e, 0x00, 
0x01, 0xe0, 0xff, 0xe0, 0x1f, 0xf8, 0x0f, 0x00, 0x01, 0xc0, 0x3f, 0xff, 0xff, 0xf0, 0x07, 0x00, 
0x00, 0x00, 0x1f, 0xff, 0xff, 0xe0, 0x03, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xe0, 0x00, 0x00, 
0x00, 0x00, 0x3c, 0x7f, 0xf8, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x78, 0x0f, 0x80, 0x70, 0x00, 0x00, 
0x00, 0x00, 0x70, 0x07, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x38, 0x00, 0x00, 
0x00, 0x01, 0xe0, 0x07, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x07, 0x00, 0x0c, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void setup() {
  Serial.begin(9600);
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  // AFFICHAGE IMAGE DEFAUT
  display.display();
  delay(2000);

  // NETTOYAGE BUFFER.
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  WiFiClient espClient;
  PubSubClient client(espClient);
  display.println("BOOT");
  display.display();

  // Connexion WiFi
     WiFi.begin(ssid, passwordW);
    display.print("WIFI WAIT");
    while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    display.print(".");
    display.display();
    }
    display.clearDisplay();
     display.display();
     display.setCursor(0,0);
    display.println("WIFI OK");
    display.display();
      delay(1000);

  //NTP START
      timeClient.begin();

}

void loop() {
  display.clearDisplay();
  display.display();
  display.setCursor(0,0);
  display.println("HTTP Start");
  display.display();  
  http.begin(client, String(server) + String(uri));   // configure traged server and url
  const char *keys[] = {"WWW-Authenticate"};
  http.collectHeaders(keys, 1);
  display.println("HTTP GET"); 
  display.display();                   // Starting first GET - authentication only

  // start connection and send HTTP header
    int httpCode = http.GET();
    if (httpCode > 0) {
      String authReq = http.header("WWW-Authenticate");
      String authorization = getDigestAuth(authReq, String(username), String(password), "GET", String(uri), 1);
      http.end();
      http.begin(client, String(server) + String(uri));
      http.addHeader("Authorization", authorization);
      int httpCode = http.GET();           // Second GET - Actually getting the payload here
      if (httpCode == 200) {
         WiFiClient *cl = http.getStreamPtr();
        int error = 0;
        do {
          cl->find("data: ");
          String payload = cl->readStringUntil('\n');
          error = processingJsondata(payload);
          client.flush();
        } while (error);
      } else { 
         display.clearDisplay();
         display.display();
            display.print("HTTP GET failed, error: ");
            display.println(http.errorToString(httpCode).c_str());
            display.display();
        }
    } else {  display.clearDisplay();
          display.display();
          display.print("HTTP GET failed, error: ");
          display.println(http.errorToString(httpCode).c_str());
          display.display();
      }
  http.end();
  delay(3000);                  

}


// Functions for Digest authorisation

String exractParam(String& authReq, const String& param, const char delimit) {
  int _begin = authReq.indexOf(param);
  if (_begin == -1) {
    return "";
  }
  return authReq.substring(_begin + param.length(), authReq.indexOf(delimit, _begin + param.length()));
}

String getCNonce(const int len) {
  static const char alphanum[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";
  String s = "";
  for (int i = 0; i < len; ++i) {
    s += alphanum[rand() % (sizeof(alphanum) - 1)];
  }
  return s;
}

String getDigestAuth(String& authReq, const String& username, const String& password, const String& method, const String& uri, unsigned int counter) {
  // extracting required parameters for RFC 2069 simpler Digest
  String realm = exractParam(authReq, "realm=\"", '"');
  String nonce = exractParam(authReq, "nonce=\"", '"');
  String cNonce = getCNonce(8);
  char nc[9];
  snprintf(nc, sizeof(nc), "%08x", counter);
  // parameters for the RFC 2617 newer Digest
  MD5Builder md5;
  md5.begin();
  md5.add(username + ":" + realm + ":" + password);  // md5 of the user:realm:user
  md5.calculate();
  String h1 = md5.toString();
  md5.begin();
  md5.add(method + ":" + uri);
  md5.calculate();
  String h2 = md5.toString();
  md5.begin();
  md5.add(h1 + ":" + nonce + ":" + String(nc) + ":" + cNonce + ":" + "auth" + ":" + h2);
  md5.calculate();
  String response = md5.toString();
  String authorization = "Digest username=\"" + username + "\", realm=\"" + realm + "\", nonce=\"" + nonce +
                         "\", uri=\"" + uri + "\", algorithm=\"MD5\", qop=auth, nc=" + String(nc) + ", cnonce=\"" + cNonce + "\", response=\"" + response + "\"";
  //  display.println(authorization);
  return authorization;
}
// End of functions for digest authorisation





// Routine de gestion et impression des donnes JSON

int processingJsondata(String payload) {

DynamicJsonDocument doc(2000);          // Added from envoy exemple
     
// Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, payload);
      
  // Test if parsing does not succeeds.
  if (error!=DeserializationError::Ok) {
    display.print(F("deserializeJson() failed: "));
    display.println(error.f_str());
    display.display();
    delay(2000);
    return 0;
  }

   JsonObject obj = doc.as<JsonObject>();


      if (obj.containsKey("production"))
      {
          if (obj["production"].containsKey("ph-a"))
          {
              AReal_power = obj["production"]["ph-a"]["p"]; 
              AReactive_power = obj["production"]["ph-a"]["q"];  
              AApparent_power = obj["production"]["ph-a"]["s"];
              Avoltage = obj["production"]["ph-a"]["v"];
              Acurrent = obj["production"]["ph-a"]["i"];
              pf = obj["production"]["ph-a"]["pf"];
              freq = obj["production"]["ph-a"]["f"];
              
          }
          Power = AReal_power;
      } else {
            display.clearDisplay();
            display.println("Pas de production");
            display.display();
            return 0;
        }

      if (obj.containsKey("total-consumption"))
      {
          if (obj["total-consumption"].containsKey("ph-a"))
          {
              tcAReal_power = obj["total-consumption"]["ph-a"]["p"]; 
              tcAReactive_power = obj["total-consumption"]["ph-a"]["q"];  
              tcAApparent_power = obj["total-consumption"]["ph-a"]["s"];
              tcAvoltage = obj["total-consumption"]["ph-a"]["v"];
              tcAcurrent = obj["total-consumption"]["ph-a"]["i"];
              pf = obj["total-consumption"]["ph-a"]["pf"];
              freq = obj["total-consumption"]["ph-a"]["f"];
              


  Power = (int)(AReal_power-tcAReal_power);
  //Power = (int)(tcAReal_power);
  int16_t y = (display.height() - display.getCursorY()) / 2;
     timeClient.update();
    display.clearDisplay();

  //Coupe l'ecran la nuit.
  while ((timeClient.getHours() < hourstart)  ||  (timeClient.getHours() >= hourstop)) {
      timeClient.update();
      display.clearDisplay();
      display.setCursor(0,0);
      display.display();
      delay (300000);
    };

  if ((Power > 0) && (Power < 1000)) // PARLE EN WATT
      {// PRODUCTION entre 0 et 999w
    display.drawBitmap(0, 5, solaire, 64, 48, WHITE);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,35);
    display.print((int)(Power));
    display.setTextSize(2);
    display.print("W");
    display.display();
    delay(1000);
      }

  if (Power > 999) // PARLE EN KWATT
      {
    Power /= 1000;
    display.drawBitmap(0, 5, solaire, 64, 48, WHITE);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,35);
    //on affiche un chiffre apres la virgule
    display.print((Power),1);
    display.setTextSize(2);
    display.print("Kw");
    display.display();
    delay(1000);
      }

  if ((Power < 0) && (Power > -1000)) // PARLE EN WATT
      {
    Power *= -1;
    display.drawBitmap(0, 5, prise, 64, 48, WHITE);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,35);
    display.print((int)(Power));
    display.setTextSize(2);
    display.print("W");
    display.display();
    delay(1000);
      }

  if (Power < -999) // PARLE EN KWATT
      {
    Power /= -1000;
    display.drawBitmap(0, 5, prise, 64, 48, WHITE);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,35);
    //on affiche un chiffre apres la virgule
    display.print((Power),1);
    display.setTextSize(2);
    display.print("Kw");
    display.display();
    delay(1000);
      }

      //Compteur nb affichage avant screen off
      if (Compteur > 10)
      {
      display.clearDisplay();
      display.display();
      delay(30000);
      Compteur = 0;
      }
      Compteur +=1;
         
}
      }
      else {
        display.println("Pas de data");
        display.display();
        return 0;
        }
  return 1;
 }