// demo: CAN-BUS Shield, receive data
#include <mcp_can.h>
#include "FS.h"
#include "SD.h"
#include <SPI.h>

const int broche_CS = 17;  // broche CS du module SD branché à GPIO 17
const int broche_lecture = 36; // un potentiomètre est branché à GPIO 36

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
unsigned char rxBuf0, rxBuf1, rxBuf2, rxBuf3, rxBuf4, rxBuf5, rxBuf6, rxBuf7, rxBuf_tmp; 
unsigned long tempsPrecedent;

MCP_CAN CAN0(5);                               // Set CS to pin 5
File file;
  int compte = 0; 

void setup()
{
  Serial.begin(115200);
  
  CAN0.begin(CAN_500KBPS);                       // init can bus : baudrate = 500k 
  pinMode(2, INPUT);                            // Setting pin 2 for /INT input
  Serial.println("MCP2515 Connected");
  
  if (!SD.begin(broche_CS)) {
    Serial.println("Carte SD introuvable");
    return;
  }
  else{
    Serial.println("Carte SD détectée");
  }
   file = SD.open("/Valeurs.txt", FILE_APPEND);
   if (!file) {
    Serial.println("Echec d'ouverture du fichier");
    return;
   } else {
    Serial.println("Ouverture du fichier OK");
   }
  tempsPrecedent = millis();
}

void loop()
{
 
  compte++;  
  char Str[50];

  unsigned long times = micros();
  int mesure = analogRead(broche_lecture);
 


    if(!digitalRead(2))                         // If pin 2 is low, read receive buffer
    {
      CAN0.readMsgBuf(&len, rxBuf);              // Read data: len = data length, buf = data byte(s)
      rxId = CAN0.getCanId();                    // Get message ID
 //     Serial.print("ID: ");
 //     Serial.print(rxId, HEX);
 //     Serial.print("  Data: ");
      for(int i = 0; i<8; i++)                  // Print each byte of the data
      { 
        if (i<len) {
           if(rxBuf[i] < 0x10)                     // If data byte is less than 0x10, add a leading zero
           {
  //            Serial.print("0");
              rxBuf_tmp = 0;
           }
  //         Serial.print(rxBuf[i], HEX);
           rxBuf_tmp = rxBuf[i];
  //         Serial.print(" ");
        } else {
            rxBuf_tmp =0;
        }
        switch (i) {
        case 0 :
          rxBuf0 = rxBuf_tmp;
        break;
        case 1 :
          rxBuf1 = rxBuf_tmp;
        break;
        case 2 :
          rxBuf2 = rxBuf_tmp;
        break;
         case 3 :
          rxBuf3 = rxBuf_tmp;
        break;
         case 4 :
          rxBuf4 = rxBuf_tmp;
        break;
         case 5 :
          rxBuf5 = rxBuf_tmp;
        break;
         case 6 :
          rxBuf6 = rxBuf_tmp;
        break;
        case 7 :
          rxBuf7 = rxBuf_tmp;
        break;
        }
      }  
    
       sprintf(Str,"%d;%d;%X;%d;%X;%X;%X;%X;%X;%X;%X;%X \n",  micros(), mesure, rxId, len ,rxBuf0, rxBuf1, rxBuf2, rxBuf3, rxBuf4, rxBuf5, rxBuf6, rxBuf7 );

    } else {
       sprintf(Str,"%d;%d \n",  micros(),mesure);   // pas de message CAN détecté
    }  

 //   Serial.print(Str);
    file.print(Str);
 //   file.flush();
    
    if (compte > 10000) {
       file.flush();    // vider la memoire tampon sur la carte SD tous les 10000 enregistrements afin d'eviter de ralentir le programme
       compte =0;
    
    }   
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
