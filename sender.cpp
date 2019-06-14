#include <Arduino.h>
#include "heltec.h"
#define BAND    433E6  //or [868E6 , 915E6]

//static
int moiSens=34;         //moisture Sensor port
int sReadQtty=5;        //ammount of sensor reads
bool debug = true;    //activates 

//functions
void sendLora(int);
int moistRead(int);
int moistReadD(int);

//data
int avg= 0;




void setup() 
{
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
}

void loop() {
  
  avg = moistReadD(moiSens);
  sendLora(avg);

  avg=0;
  
}

int moistReadD(int port, int sReadQtty, bool debug)
{
  /* 
  Reads Sensor Data on given port X ammount of times then, returns the average reading
  */ 
  int i=0;
  int average = 0;
  int value = 0;

  do
    {
    value = analogRead(port); 

      if (debug)
      {
      Serial.print("\t Value read: ");
      Serial.println(value);
      }
  
    average+=value;

   delay(1000);
    i++;
  } while (i < sReadQtty);

  average = average/sReadQtty;

  if (debug)
  {
    Serial.println("\t\tAverage:");
    Serial.println(average);
  }

  return(average);
}

int moistRead(int port)
{
    int i=0;
    int average = 0;
    int value = 0;
  do
    {
    value = analogRead(port); 
    average+=value;
    delay(1000);
    i++;
    } while (i<5);
    return(average/5);
}

void sendLora(int val)
{
  LoRa.beginPacket();
  LoRa.print(val);
  LoRa.endPacket();
}