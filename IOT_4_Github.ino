#include <ThingSpeak.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
const char* apiKey = "*****************";  //  Enter your Write API key here
long unsigned myChannelNumber =  ******* ;
const char* server = "api.thingspeak.com";
const char *ssid =  "**********"; / Enter your WiFi Name
const char *pass =  "*********";      // Enter your WiFi Password 
#define DHTPIN D4                  // GPIO Pin where the dht11 is connected
DHT dht(DHTPIN, DHT11); 
WiFiClient client;

const int moisturePin = A0;             // moisture sensor pin
const int motorPin = D1;
unsigned long interval = 10000;
unsigned long previousMillis = 0;
unsigned long interval1 = 1000;
unsigned long previousMillis1 = 0;
float moisturePercentage;              //moisture reading
float h;                  // humidity reading
float t;                  //temperature reading
//String myStatus = "";

void setup()
{
  Serial.begin(115200);
  delay(10);
  pinMode(D0, OUTPUT);
  digitalWrite(motorPin, LOW); // keep motor off initally
  digitalWrite(D0,HIGH);
  delay(1000);
  digitalWrite(D0,LOW);
  dht.begin();
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  ThingSpeak.begin(client);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(5000);
    Serial.print(".");              // print ... till not connected
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop()
{
  unsigned long currentMillis = millis(); // grab current time

  h = dht.readHumidity();     // read humidity
  t = dht.readTemperature();     // read temperature

  if ((h==0)||(t==0))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  } 

  moisturePercentage = ( 100.00 - ( (analogRead(moisturePin) / 1023.00) * 100.00 ) );
  

  if ((unsigned long)(currentMillis - previousMillis1) >= interval1) {
    Serial.print("Soil Moisture is  = ");
    Serial.print(moisturePercentage);
    Serial.println("%");
    if(moisturePercentage > 55) {
      digitalWrite(motorPin,LOW);
    }
    previousMillis1 = millis();
  }

if (moisturePercentage < 50) {
  digitalWrite(D0, HIGH);         // turn on motor pump
}
if (moisturePercentage > 55) {
  digitalWrite(D0, LOW);        //turn on motor pump
}

if ((unsigned long)(currentMillis - previousMillis) >= interval) {

  sendThingspeak();           //send data to thingspeak
  previousMillis = millis();
  client.stop();
}

}

void sendThingspeak() {
  if (client.connect(server, 80))
  { 
    ThingSpeak.writeField(myChannelNumber,1,t,apiKey);
    delay(500);
    ThingSpeak.writeField(myChannelNumber,2,h,apiKey);
    ThingSpeak.writeField(myChannelNumber,3,moisturePercentage,apiKey);
     
    Serial.print("Moisture Percentage: ");
    Serial.print(moisturePercentage);
    Serial.print("%. Temperature: ");
    Serial.print(t);
    Serial.print(" C, Humidity: ");
    Serial.print(h); 
    Serial.println("%. Sent to Thingspeak.");
    Serial.println("Channel updated successfully.");  
   }
  else{
    Serial.println("Problem updatingchannel. HTTP error code" + String(x));
  }
}

