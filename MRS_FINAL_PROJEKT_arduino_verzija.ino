//https://create.arduino.cc/projecthub/akshayjoseph666/interface-i2c-16x2-lcd-with-arduino-uno-just-4-wires-273b24

//sketch created by Luka Krisla
#include <Wire.h>               //biblijoteka za I2C (display u ovom slucaju)
#include <LiquidCrystal_I2C.h>  //bibilojeteka za Displej
#include <dht.h>                //bibilojeteka za DHT11 (temp i vlaga)

dht DHT;
#define DHT11_PIN 2                   //Poziv za DHT11 i postavljanje PIN 2 za komunikaciju

LiquidCrystal_I2C lcd(0x27, 16, 2);   //Poziv za LDC - njegov ID i koliko segmenata ima

int ocitanje;        //varijabla u koju spremamo vrijednost sa senzora  (Osuncanost)
int analogPin=A0;    //pin na koji je spojen analogni izlaz sa senzora  (Osuncanost)


 ////////// ALGORITAM ZA ZALJEVANJE - DEKLARIRANJE ///////////
bool mirovanje = false;    // za mirovanje 20h NAKON zaljevanje 
bool nocJe = false;        // za provjeru dali je noc
bool zalito = false;       // za provjeru dali je vec zalito
int vodaTimer = 0;         // timer za pustanje vode (koliko dugo pusta vodu)
int delayTimer = 0;        // timer koji blokira pustanje vode sljedecih 20 sati
int nightTimer = 0;        // timer koji provjerava dali je tama bar 1 sat vremena

void setup()
{
  Serial.begin(9600); //Inicijalizacija serijske komunikacije( brzina 9600 bauda) (Osuncanost)

  pinMode(analogPin,INPUT); //postavljamo analogni pin kao ulaz (Osuncanost)
  
  lcd.begin();        //postavljanje LCDa,
  lcd.backlight();    //pozadinsko svjetlo
  lcd.clear();        //ocisti display od starih vrijednosti

   pinMode(11,OUTPUT); // RELEJ

   
}



void loop()
{
  /////////// TESTING: temp i vlaga za serial monitor ///////////
  int chk = DHT.read11(DHT11_PIN);
 // Serial.print("Temp=");
 // Serial.println(DHT.temperature);
 // Serial.print("Hum=");
 // Serial.println(DHT.humidity);
  

  //////////// Temp i vlaga i svjetlina ispis na display ////////////////
  int temp = DHT.temperature;
  int hum = DHT.humidity;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp=");
  lcd.print(temp);
  lcd.print("C");
  lcd.setCursor(9,0);
  lcd.print("Hum=");
  lcd.print(hum);
  lcd.print("%");

  ocitanje=analogRead(analogPin);//spremamo vrijednost s analognog pina u varijablu očitanje (Osuncanost)
  lcd.setCursor(3,1);
  lcd.print("Light=");
  lcd.print(ocitanje);
  lcd.print("xXx");



  ////////// ALGORITAM ZA ZALJEVANJE ///////////

  ///// Senzor svjetlosti provjerava dali je noc ///// 

  
  // za prikaz timera na ekran
  Serial.print("--------------\n");
  Serial.print("vodaTimer=");
  Serial.println(vodaTimer);
  Serial.print("nightTimer=");
  Serial.println(nightTimer);
  Serial.print("delayTimer=");
  Serial.println(delayTimer);
  

  if(mirovanje == false){
    if(nocJe == true){
        digitalWrite(11,HIGH);      //otvori ventil
        vodaTimer = vodaTimer + 1;
        
        if (vodaTimer >= 3){        //pusta vodu 5 sekundi
            digitalWrite(11,LOW);   //zatvori ventil
            vodaTimer = 0;
            mirovanje = true;       //nakon sto je zaliveno upali mirovanje
            nocJe = false;          //dokazano je daj noc - resetiraj
            zalito = true;          //dokaz da je zalito sada dozvoli cekanje 20h
        }
    }
  }  ///// KRAJ

  
  ///// Timer koji miruje 20 sati ///// 

  if(zalito == true){
    delayTimer = delayTimer + 1;
  
    if(delayTimer >= 20){  // 60 sec * 60 min * 20 sati = 72000 sekundi
      mirovanje = false;
      delayTimer = 0;
    }
  } ///// KRAJ

  ///// BONUS: Timer od 1 sat koji provjerava dali je stvarno noć a ne ruka na senzoru /////
  if(ocitanje >= 500){
      nightTimer = nightTimer + 1;
      
      if(nightTimer >= 5){    // 60 sec * 60 min = 3600 sekundi
          nocJe = true;
          nightTimer = 0;
      }
  } ///// KRAJ
  
  delay(1000);   // Jedna sekunda loop cijelog algoritma
}
