/*

*/
#include "MeshRC.h"

#include <ESP8266WiFi.h>
#define CHANNEL 9
//#define DEBUG_ALL false
#define FORSEDEEPSLEEP 25000
const int buttonPin = 2;     // the number of the pushbutton pin

ADC_MODE(ADC_VCC)  
uint16_t batterylevel;  

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton statu
int prevButtonState = 4711;
unsigned long lastUpdate = millis();
unsigned long currentMillis,delta1,delta2;
String mac;
void setup() {
  WiFi.persistent(false);//don't store and read Wifi settings, this savesabout 300ms
  Serial.begin(115200);

  batterylevel = ESP.getVcc(); 

  const char *ssid_not_used = "ESP001_sender_dummy";
  const char *pwd_not_used ="dummy_not_used";
  int ch = CHANNEL;
 
#ifdef DEBUG_ALL
  currentMillis = millis();
  Serial.printf("WiFi.begin\n");
#endif

  WiFi.begin(ssid_not_used,pwd_not_used,ch,NULL,false);//Trick to initiate channel
//without startinga connection when esp-now uses an other channel than 1(defaut for STA)

#ifdef DEBUG_ALL
  delta1 = millis()-currentMillis;
  Serial.print(" 3 WiFi.channel=");
  Serial.println(WiFi.channel());
  currentMillis = millis();
#endif

  WiFi.disconnect(); //saves power ifonly esp-now is used
  WiFi.softAPdisconnect(true);

#ifdef DEBUG_ALL
  delta2 = millis()-currentMillis; 
  Serial.printf("WiFi.begin time = %u \n",delta1);
  Serial.printf("WiFi.disconnect time = %u \n",delta2);
#endif
  mac = WiFi.macAddress();
  Serial.print("\n\nWiFi.macAddress=");
  Serial.println(mac);
  Serial.flush();
  
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);

//Channel must be the same for receiver and sender
//it should be the same as youre WiFi-network if reciver
//the receiver shall forvard to HA
  MeshRC::begin(0);
 
  MeshRC::send("ESP01 "+mac+" Started Vcc "+(String)( batterylevel / 1000.0)+" volts");
}    


void loop() {
   currentMillis = millis();
 // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (prevButtonState != buttonState) {
    prevButtonState = buttonState;
    if (buttonState == HIGH) {
      MeshRC::send("ESP01 "+mac+" Button activeted");
    } else {
      MeshRC::send("ESP01 "+mac+" Button deactiveted");
    }
      MeshRC::send("ESP01 "+mac+" WiFi.rssi "+ (String)(WiFi.RSSI()));   
  }
  
  if ( currentMillis > (lastUpdate + 5000) ){
    MeshRC::send("ESP01 "+mac+" still running " + String(currentMillis));
    lastUpdate = currentMillis;
    batterylevel = ESP.getVcc(); 
    MeshRC::send("ESP01 "+mac+" Voltage: "+ String(batterylevel/ 1000.0));
  }
  
  if ( currentMillis > FORSEDEEPSLEEP ){
    batterylevel = ESP.getVcc(); 
    String s = String(batterylevel/ 1000.0);
    MeshRC::send("ESP01 "+mac+" Voltage: "+ s);
    MeshRC::send("ESP01 "+mac+" Going to sleep ");
    ESP.deepSleep(0);
  }
}
