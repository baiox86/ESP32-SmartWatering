#include "heltec.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>

#define BAND    433E6  //or [868E6 , 915E6]

#define HUM_TOPIC    "humidity/1"
#define MOTOR_TOPIC    "estadoMotor/1"


const char* ssid = "ADRCV";
const char* password = "ADRCV123";

const char* mqtt_server = "94.62.164.97";

const char led = 2;

WiFiClient espClient;
PubSubClient client(espClient);

bool stateChange(int, bool, bool, PubSubClient);
char getLoraPacket();


bool loraData = false;
bool previousState;
String st;
char payload[5];

int waterThreshold  = 2000;

void receivedCallback(char* topic, byte* payload, unsigned int length) {
  
  String value ="";
  int data;
  Serial.print("Message received: ");
  Serial.println(topic);


  Serial.print("payload: ");
  
  for (int i = 0; i < length; i++)
  { 
    value +=  (char) payload[i];
  }
  value+="\0";
  Serial.println(value);
  data = value.toInt();
  waterThreshold = data;
}

void mqttconnect() {
  while (!client.connected()) {
    Serial.print("MQTT connecting ...");
    String clientId = "ESP32Client";
    if (client.connect(clientId.c_str())) 
    {
      Serial.println("connected");
      client.subscribe("threshold");

    } else
    {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}

void setup() 
{
  pinMode(led, OUTPUT);
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, 1883);
  client.setCallback(receivedCallback);

}

void loop() 
{
  // Serial.print("Water: ");
  // Serial.println(waterThreshold);
 
   if (!client.connected()) { mqttconnect(); }
   
  client.loop(); 
  getLoraPacket();


  if (loraData)
  { 
    Serial.print("Humidity: ");
    Serial.println(st);
    loraData=false;
    st.toCharArray(payload,5);
    client.publish(HUM_TOPIC, payload);
    if (st.toInt()>= waterThreshold)
    {
      previousState = stateChange(led, true, previousState, client);
    }else
      { 
        previousState =  stateChange(led, false, previousState, client);
      }
  }
}

bool stateChange(int led, bool state, bool previousState, PubSubClient client){
  if(state){
    if (state != previousState)
    {
        digitalWrite(led,HIGH);
        Serial.println("state changed");
        client.publish(MOTOR_TOPIC, "MotorLigado");
    }    
    }else
    {
      if (state != previousState)
      {
        Serial.println("state changed");
        digitalWrite(led,LOW);
        client.publish(MOTOR_TOPIC, "MotorDesligado");
      }
  }
  return state;
 }
char getLoraPacket()
{
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    while (LoRa.available()) 
    {
      st = LoRa.readString();
    }
    
    loraData = true;
  }
}