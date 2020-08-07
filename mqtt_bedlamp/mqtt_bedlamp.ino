#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <ESP8266WiFi.h>
#include "config.h"

//Set up MQTT and WiFi clients
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

//Set up the feed you're subscribing to
Adafruit_MQTT_Subscribe onoff = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/lights.bedlamp1");
//Set up the feed to publish
Adafruit_MQTT_Publish lights = Adafruit_MQTT_Publish(&mqtt, MQTT_NAME "/f/lights.bedlamp1");

const int Rele2 = D1;
bool ligado = false;

void setup()
{
  Serial.begin(9600);

  //Connect to WiFi
  Serial.print("\n\nConnecting Wifi... ");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Connecting...");
    delay(1000);
  }

  Serial.println("Wifi Connected!");

  //Subscribe to the onoff feed
  mqtt.subscribe(&onoff);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(Rele2, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(Rele2, HIGH);
  lights.publish("testando 123");
}

void loop()
{

  
  MQTT_connect();
  
  //Read from our subscription queue until we run out, or
  //wait up to 5 seconds for subscription to update
  Adafruit_MQTT_Subscribe * subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    //If we're in here, a subscription updated...
    if (subscription == &onoff)
    {
      //Print the new value to the serial monitor
      Serial.print("message: ");
      Serial.println((char*) onoff.lastread);
      
      if (!strcmp((char*) onoff.lastread, "bedlamp1 on"))
      {
        liga();
      }
      else if(!strcmp((char*) onoff.lastread, "bedlamp1 toggle"))
      {
          if(!ligado){
          lights.publish("bedlamp1 on");
          delay(1000);
          }
          else{
          lights.publish("bedlamp1 off");
          delay(1000);
       }
      }
      else if(!strcmp((char*) onoff.lastread, "bedlamp1 off"))
      {
          desliga();
      }
      else if(!strcmp((char*) onoff.lastread, "all off"))
      {
          desliga();
      }
      else if(!strcmp((char*) onoff.lastread, "all on"))
      {
          liga();
      }
    }
  }

  // ping the server to keep the mqtt connection alive
  if (!mqtt.ping())
  {
    mqtt.disconnect();
  }
}



void MQTT_connect() 
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) 
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
  { 
       Serial.println("Checking the Wifi connection...");
       if(WiFi.status() != WL_CONNECTED){
        Serial.println("Wifi disconnected");
        wifiReconnect();
       }
       else
       {
        Serial.println("Wifi is Connected");
       }
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       
       
  }
  Serial.println("MQTT Connected!");
}

void wifiReconnect(){


while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Reconnecting...");
    delay(1000);
  }
  Serial.println("Wifi Connected!");
}

void liga(){
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(Rele2, LOW);
  Serial.println("liga()");
  ligado = true; 
}

void desliga(){
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(Rele2, HIGH);
  Serial.println("desliga()");
  ligado = false;
}
