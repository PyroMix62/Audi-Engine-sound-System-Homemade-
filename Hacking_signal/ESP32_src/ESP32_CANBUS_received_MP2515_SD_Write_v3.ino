// demo: CAN-BUS Shield, receive data
#include <mcp_can.h>
#include "FS.h"
#include "SD.h"
#include <SPI.h>

#define NB_VALUE 5400
#define LAPS_TIME 50


class record
{
private:
  
public:
   uint16_t T PROGMEM;
   uint16_t V PROGMEM;
   long unsigned int rxI PROGMEM;
   unsigned char le = 0 PROGMEM;
   unsigned char rxBf[8] PROGMEM;
};

const int broche_CS = 17;  // broche CS du module SD branché à GPIO 17
const int broche_lecture = 36; // un potentiomètre est branché à GPIO 36

unsigned char len = 0;
unsigned char rxBuf[8];
unsigned long tempsPrecedent;
unsigned long cpt;
long unsigned int rxId;


record tab_record [NB_VALUE] ;
  


MCP_CAN CAN0(5);                               // Set CS to pin 5
File file;

void setup()
{
  Serial.begin(115200);
  
  CAN0.begin(CAN_500KBPS,MCP_8MHz);                       // init can bus : baudrate = 500k 
  pinMode(2, INPUT);                            // Setting pin 2 for /INT input
  Serial.println("MCP2515 Connected");
  
  if (!SD.begin(broche_CS)) {
    Serial.println("Carte SD introuvable");
    return;
  }
  else{
    Serial.println("Carte SD détectée");
  }
   file = SD.open("/Valeurs.txt", FILE_WRITE);
   if (!file) {
    Serial.println("Echec d'ouverture du fichier");
    return;
   } else {
    Serial.println("Ouverture du fichier OK");
   }
  tempsPrecedent = micros();
  cpt = 0;
}

void save_sd ()
{
 char Str[50];
 for(int j = 0; j<=NB_VALUE-2; j++) {   
   if (tab_record[j].rxI!=0) {
      sprintf(Str,"%d;%d \n",  tab_record[j].T, tab_record[j].V, tab_record[j].rxI, tab_record[j].le );
   } else {
     sprintf(Str,"%d;%d;%d%d;%d;%d;%d;%d;%d;%d;%d;%d;%d \n",  tab_record[j].T, tab_record[j].V, (tab_record[j].rxI >> 8) & 0xff,(tab_record[j].rxI)  & 0xff, tab_record[j].le ,
                                                             tab_record[j].rxBf[0], tab_record[j].rxBf[1], tab_record[j].rxBf[2], tab_record[j].rxBf[3], tab_record[j].rxBf[4],
                                                             tab_record[j].rxBf[5], tab_record[j].rxBf[6], tab_record[j].rxBf[7] );
   }
 //   Serial.print(Str);
 
    file.print(Str);   
    }  
    file.flush();    // vider la memoire tampon sur la carte SD  
}

void loop()
{ char Str[50];
  int rxId = 0;
  if ((micros() - tempsPrecedent) >= LAPS_TIME) {
 
    cpt++;
    tab_record[cpt].T = int (micros()-tempsPrecedent);
    tempsPrecedent = micros();

    int mesure = analogRead(broche_lecture);

    tab_record[cpt].V = mesure;
    tab_record[cpt].rxI = 0;
    

    if(!digitalRead(2))                         // If pin 2 is low, read receive buffer
      {
        CAN0.readMsgBuf(&len, rxBuf);              // Read data: len = data length, buf = data byte(s)
        rxId = CAN0.getCanId();  // Get message ID
        tab_record[cpt].rxI = rxId;  // Get message ID
        tab_record[cpt].le = len;  

        for(int i = 0; i<len; i++)                  // Print each byte of the data
        { 
          if (i<len){
               tab_record[cpt].rxBf[i] = rxBuf[i];
            } else {
               tab_record[cpt].rxBf[i] = 0;
            }
          }        
        } else {
           tab_record[cpt].rxI = 0;  // Get message ID
         /*  tab_record[cpt].le = 0;  
            for(int i = 0; i<len; i++)                  // Print each byte of the data
            { 
              tab_record[cpt].rxBf[i] = 0;
            }   */
        }

       
/*
// ************************************************************
// DEBUT DEBUG

        char Str[50];
        sprintf(Str,"%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d \n",  tab_record[cpt].T, tab_record[cpt].V, tab_record[cpt].rxI, tab_record[cpt].le ,
  //      sprintf(Str,"%d;%d;%X;%d;%X;%X;%X;%X;%X;%X;%X;%X \n",  tab_record[cpt].T, tab_record[cpt].V, tab_record[cpt].rxI, tab_record[cpt].le ,
                                                     tab_record[cpt].rxBf[0], tab_record[cpt].rxBf[1], tab_record[cpt].rxBf[2], tab_record[cpt].rxBf[3], tab_record[cpt].rxBf[4],
                                                     tab_record[cpt].rxBf[5], tab_record[cpt].rxBf[6], tab_record[cpt].rxBf[7] );

         Serial.print(Str);

//  FIN DEBUG  
//    Serial.println(tab_record[cpt].V);

// ************************************************************
         */
    if (cpt == NB_VALUE-2){
     save_sd ();
     cpt = 0;
    }      
  }     
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
