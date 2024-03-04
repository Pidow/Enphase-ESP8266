#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// SDA GPIO4
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

#define serialNumber "XXXXXXXX"
//Installer est le username par defaut
#define userName "installer"

#define realm "enphaseenergy.com"

// Fonction calcul Hash MD5
String emupwGetPasswdForSn(String serial, const String user, String srealm) {

 MD5Builder md5;
 String hash1;
  md5.begin();
  md5.add("[e]" + user + "@" + srealm + "#" + serial + " EnPhAsE eNeRgY ");
  md5.calculate();
  hash1 = md5.toString();
  return hash1;
  }
  
//Fin de fonction
//---------------------------------------------------------------

// fonction de calcul du mot de passe
String emupwGetMobilePasswd(String serial){

String digest;
int countZero = 0;
int countOne =  0;
digest = emupwGetPasswdForSn(serial,userName,realm);
int len = digest.length();
for (int i=0 ; i<len; i++) {
  if (digest.charAt(i) == char(48)) countZero++;
  if (digest.charAt(i) == char(49)) countOne++;
  }
  String password ="";
 for (int i=1; i < 9; ++i) {
  if (countZero == 3 || countZero == 6 || countZero == 9) countZero -= 1;
  if (countZero > 20) countZero = 20; 
  if (countZero < 0) countZero = 0;

  if (countOne == 9 ||countOne == 15) countOne--;
  if (countOne > 26) countOne = 26;
  if (countOne < 0) countOne = 0;
  char cc = digest.charAt(len-i);
  if (cc == char(48)) {
    password += char(102 + countZero);  // "f" =102
            countZero --;
    }
  else if (cc == char(49)) {
     password += char(64 + countOne);  // "@" = 64
            countOne--; 
      }
   else password += cc;
  }

    return password;
}


void setup() {
  Serial.begin(9600);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  // init done

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.clearDisplay();
  display.display();
  

display.print("Pass:");
display.println(emupwGetPasswdForSn(serialNumber, userName, realm));
display.print("Password:");
display.println(emupwGetMobilePasswd(serialNumber));
display.display();
delay(20000);
}

void loop() {
  // put your main code here, to run repeatedly:

}