#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "config.h"
#include "DHTesp.h"

//Set up MQTT and WiFi clients
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

//Set up the feed to publish
Adafruit_MQTT_Publish temp = Adafruit_MQTT_Publish(&mqtt, MQTT_NAME "/f/temperature");

Adafruit_MQTT_Publish hum = Adafruit_MQTT_Publish(&mqtt, MQTT_NAME "/f/humidity");

DHTesp dht;

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


Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)\tHeatIndex (C)\t(F)");
  String thisBoard= ARDUINO_BOARD;
  Serial.println(thisBoard);

  // Autodetect is not working reliable, don't use the following line
  // dht.setup(17);
  // use this instead: 
  dht.setup(4, DHTesp::DHT22); 

}

void loop()
{

  
  MQTT_connect();
  
  

  // ping the server to keep the mqtt connection alive
  if (!mqtt.ping())
  {
    mqtt.disconnect();
  }

  le_temperatura();
  publish_mqtt();
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

void le_temperatura(){
  delay(dht.getMinimumSamplingPeriod());

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  
  temp.publish(temperature);
  delay(2000);
  hum.publish(humidity);
  delay(2000);
  
  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.print(temperature, 1);
  Serial.print("\t\t");
  Serial.print(dht.toFahrenheit(temperature), 1);
  Serial.print("\t\t");
  Serial.print(dht.computeHeatIndex(temperature, humidity, false), 1);
  Serial.print("\t\t");
  Serial.println(dht.computeHeatIndex(dht.toFahrenheit(temperature), humidity, true), 1);
 
}

void publish_mqtt(){
  
}
