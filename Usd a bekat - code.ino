// Hozzáadjuk az arduino és a projekt működéséhez szükséges könyvtárakat
#include <MD_Parola.h> //Parola könyvtár importálása, kijelző manipuláláshoz
#include <MD_MAX72xx.h> //Kijelzőhöz tartozó könyvtár importálása
#include <SPI.h> // szinkron soros kommunikációs könyvtár importálása


// Definiáljuk a hardver típust és a kijelzőhöz tartozó pineket 
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN 11     
#define DATA_PIN 12
#define CLK_PIN 10
 

// Egy új parola objektumot hozunk létre a hardver (kijelző) és SPI kapcsolatokkal
 MD_Parola kijelzo = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);


int ido_bank = 0; // jaték sebesség
int megnyomott_gomb = 1; //megnyomott gomb bemeneti PIN számát fogja tárolni
boolean jatekban = 1; // Ezt a bool értéket arra használjuk hogy meghatározzuk hogy 
                      // még mindig játékban vagyunk e
int jatek_pont = 0; //Ez a játékos által elért pontokat mutatja
int nehezseg = 0;   //0-3ig vehet fel majd értéket

 
void setup(){
  // soros kapcsolat létrehozása 9600 bit/s sebességgel 
  // Az Arduino eszközök alapértelmezett átviteli sebessége
  Serial.begin(9600);

  // inicializáljuk a kijelzőt:
  kijelzo.begin();
  // beállítjuk a kijelző intenzitását:
  kijelzo.setIntensity(10);
  // töröljük a kijelző tartalmát indulás, inicializálás után:
  kijelzo.displayClear();
  
  //beállítjuk a LED kimeneteit és a nyomógomb bemeneteit
  pinMode(A1,INPUT_PULLUP);   //1.led nyomógomb bemenetének állítása, megfelelő PIN-hez
  pinMode(A2,INPUT_PULLUP);   //2.led nyomógomb bemenetének állítása, megfelelő PIN-hez
  pinMode(A3,INPUT_PULLUP);   //3.led nyomógomb bemenetének állítása, megfelelő PIN-hez
  pinMode(A4,INPUT_PULLUP);   //4.led nyomógomb bemenetének állítása, megfelelő PIN-hez
  pinMode(A5,INPUT_PULLUP);   //5.led nyomógomb bemenetének állítása, megfelelő PIN-hez
  pinMode(8,INPUT_PULLUP);    //6.led nyomógomb bemenetének állítása, megfelelő PIN-hez
  
  pinMode(2,HIGH);  //1.led kimeneteinek állítása, a megfelelő PIN-hez
  pinMode(3,HIGH);  //2.led kimeneteinek állítása, a megfelelő PIN-hez
  pinMode(4,HIGH);  //3.led kimeneteinek állítása, a megfelelő PIN-hez
  pinMode(5,HIGH);  //4.led kimeneteinek állítása, a megfelelő PIN-hez
  pinMode(6,HIGH);  //5.led kimeneteinek állítása, a megfelelő PIN-hez
  pinMode(7,HIGH);  //6.led kimeneteinek állítása, a megfelelő PIN-hez
}


void loop(){

  while(inditoGomb()==0){ //Gomb lenyomására várunk, hogy kezdődhessen a játék
    
  } 
  nehezseg_beallitas(nehezseg);
  jatekban = 1; //Visszaállítjuk a játék vége után alapértelmezett értékre (játékban)
  programFutas(); //Játék futtatása
  if (jatekban == 0) {  // ha a játéknak vége akkor kiírni a végső pontszámot 
                        //a VEGE játékot jelző felirattal együtt
    vegsoPont_mutato();
  }
}


// Indítás előtt az összes gombot villogatatja amíg egyik nem lesz megnyomva a kezdéshez
boolean inditoGomb(){
    digitalWrite(2, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    delay(50);
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    delay(50);
    
  boolean gomb_megnyomva = 0;

  // ha bármelyik gomb lenyomásra kerül akkor visszaad egy igaz értéket az 
  // indító gomb megnyomására attól függ melyik gombot nyomtuk meg, az ahhoz tartozó
  // nehézségi értéket átadja a nehezseg változónak
  if(digitalRead(A1)==LOW || digitalRead(A2)==LOW) {
    nehezseg = 1;
    return gomb_megnyomva = 1;
    }
  else if (digitalRead(A3)==LOW || digitalRead(A4)==LOW){
    nehezseg = 2;
    return gomb_megnyomva = 1;
    }
  else if (digitalRead(A5)==LOW || digitalRead(8)==LOW){
    nehezseg = 3;
    return gomb_megnyomva = 1;
    }
  else {return gomb_megnyomva;} // ha nem történt gomb lenyomás 0 azaz hamis értéket ad vissza
}

// a nehézség értékétől függően állítja be a játék sebességét
void nehezseg_beallitas(int nehezseg_inputja){
  
  switch (nehezseg_inputja){
   case 1:
   ido_bank = 1000;
   break;
   
   case 2:
   ido_bank = 750;   
   break;
   
   case 3:
   ido_bank = 600;   
   break;
  }
}

void programFutas() {
  jatek_pont = 0; 
  int uj_villano_LED; // A következő LED amit megfogunk világítani

  randomSeed(analogRead(A0)); //Random Seed-et rendelünk az A0 analóg PIN-hez, 
  delay(1000);                //hogy TÉNYLEGES véletlenszerű számokat generáljuk minden egyes indításnál

  while(jatekban == 1){ 
       jatekban=0; 
        byte generalt_LED = random(2,8); // egy véletlenszerű LED-et kiválaszt
  while(uj_villano_LED == generalt_LED){ //ellenőrizzük, hogy ne az előző megnyomott gombot hozza fel
          generalt_LED = random(2,8);    //ha igen akkor újabb számot generálunk amíg
        }                                //különböző értéket nem kapunk
        uj_villano_LED = generalt_LED; 

  int ujGombPin = ledhez_InputPin(uj_villano_LED); // ujGombPin-hez rendeli a felvillano LED 
                                                   // gombjához tartozó bemeneti pin számát
  digitalWrite(uj_villano_LED, HIGH); //Megvilágítja a kiválasztott LED-et

  for(int t=0; t<ido_bank; t++){ //játék ciklus reagálási idő
   megnyomott_gomb = digitalRead(ujGombPin); //a gomb státuszát teszi bele a megnyomott_gomb változóba
    if(megnyomott_gomb==0){ // ha sikerült megnyomni a gombot 
      jatekban=1;           // akkor a jatekban változót 1-re azaz "igazra" állítja
      jatek_pont=jatek_pont+1; // és megnöveli a játékos pontszámát eggyel
      break; 
      } 
   delay(1);
  }

  digitalWrite(uj_villano_LED, LOW); //kikapcsolja a LED-et


  kijelzo.setTextAlignment(PA_CENTER); // a szöveget/pontszámot a kijelzőn középre igazítja
  kijelzo.print(jatek_pont); // kiírja a jatékos által elért pontszámot
  ido_bank=(ido_bank-15); //felgyorsítja a játékot minden egyes gomb lenyomásával 15ms-el
  } 
}


// Visszadja az adott LED pinjéhez tartozó bemeneti pin számát
int ledhez_InputPin(int uj_villano_LED){ 

int bemeneti_pin; 
  
 switch (uj_villano_LED) {
  
   case 2:
   bemeneti_pin = A1;
   break;
   
   case 3:
   bemeneti_pin = A2;   
   break;
   
   case 4:
   bemeneti_pin = A3;   
   break;
   
   case 5:
   bemeneti_pin = A4;   
   break;
   
   case 6:
   bemeneti_pin = A5;   
   break;

   case 7:
   bemeneti_pin = 8;   
   break;
   
 } 
  return bemeneti_pin; 
}

// Kiírja a játék végét jelző: VEGE feliratot majd egy kis késleltetéssel a végső pontszámot
void vegsoPont_mutato(){

  kijelzo.print("VEGE");
  delay(2000);
  kijelzo.print(jatek_pont);
  delay(50);
  
}
