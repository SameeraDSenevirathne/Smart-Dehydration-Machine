////////////////////////////////////////////
#include "DHT.h"
#define DHTPIN 10  
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
////////////////////////////////////////////

#include <Servo.h>
Servo servo;

#define relay1 11
#define relay2 4
#define relay3 2
#define relay4 3

unsigned long prev, interval = 1000;
unsigned long previousMillis = 0;  

//const unsigned long interval2 = ;  

   long timeL = millis();
   long timeI = millis();


byte address;
byte remainingPOS  ;

boolean Status = false;


#include <Keypad.h>
int tot, i1, i2, i3, i4;
int val10;
char c1, c2, c3, c4;
char keypressed;
char password[4];
char pass[4], pass1[4];
char number [9];
byte i = 0;
char customKey = 0;
char keys [4] [4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins [4] = {33, 35, 37, 39};
byte colPins [4] = {41, 43, 45, 47};
Keypad Mahela = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4);

////////////////////////////////////////////////////////////////////////
#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif
//pins:
const int HX711_dout = 6; //mcu > HX711 dout pin
const int HX711_sck = 7; //mcu > HX711 sck pin
//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);
const int calVal_calVal_eepromAdress = 0;
unsigned long t = 0;
///////////////////////////////////////////////////////////////////////

//#include <LCD.h>
#include <Wire.h> //I2C COM..
#include <LiquidCrystal_I2C.h>
#define I2C_ADDR 0x27
//LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
LiquidCrystal_I2C lcd(0x27, 16,2);


byte  a[8] = {
  B00100,
  B01010,
  B10001,
  B10001,
  B11111,
  B10001,
  B10001,
  B10001
};

byte b[8] = {
   B10001,
  B10001,
  B11011,
  B10101,
  B10101,
  B10001,
  B10001,
};

/////////////////////////////////////
#define  ledr 8
#define  ledg 24
#define  ledb 25
//////////////////////////////////////
#define  buzzer 13

void setup() {///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Serial.begin(9600);

    Wire.begin();
   
    lcd.begin(16, 2);
    lcd.home();
    lcd.backlight();
    lcd.init(); 
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Welcome"); 
    delay(2000);
    lcd.clear();

     lcd.createChar(0,a);
     lcd.createChar(1,b);

    float calibrationValue; // calibration value
    calibrationValue = 696.0; 
    LoadCell.begin();
    unsigned long stabilizingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilizing time
    boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
    LoadCell.start(stabilizingtime, _tare);
    
    dht.begin();
    
   LoadCell.begin(); // start connection to HX711
  LoadCell.start(2000); // load cells gets 2000ms of time to stabilize
  LoadCell.setCalFactor(1000.0);
  
  pinMode(ledr, OUTPUT);
  pinMode(ledg, OUTPUT);
  pinMode(ledb, OUTPUT);
  
  pinMode(relay1, OUTPUT);
   pinMode(relay2, OUTPUT);
    pinMode(relay3, OUTPUT);
     pinMode(relay4, OUTPUT);

  pinMode(buzzer, OUTPUT);

  servo.attach (9);
  
////////////////////////////////////////////
  digitalWrite (relay1,HIGH);
   digitalWrite (relay2,HIGH);
   digitalWrite (relay3,HIGH);
   digitalWrite (relay4,HIGH);
////////////////////////////////////////////
EEPROM.write(200,0);
   EEPROM.write(201,0);
    EEPROM.write(202,0);
    delay(100);

}//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  
  //if (Status==true){
  DTH_read();
 // }
 
if (Status==true){
 DTH_logic();
}

if (Status==false){
 EEPROM_READ ();
 //Timer();
}
  
  loadcell ();
   
   ///lcd_backlight_off();
   keypadevent();
   
  state();
  print_icon();

 Timer();
  
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DTH_read() {
    // Read humidity and temperature values from the DHT sensor
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    
    // Compute heat index
    float f = dht.readTemperature(true);
    float hif = dht.computeHeatIndex(f, h);
    float hic = dht.computeHeatIndex(t, h, false);
    
    // Get current time
    unsigned long currentMillis = millis();

    // Display values on LCD every 'interval' milliseconds
    if (currentMillis - previousMillis >= interval && Status == true) {
        previousMillis = currentMillis;

        lcd.setCursor(0, 0);
        lcd.print("H=");
        lcd.print(h);
        lcd.print("%");

        lcd.setCursor(0, 1);
        lcd.print("T=");
        lcd.print(t);
        lcd.print("C");

        // Transmit humidity and temperature via Wire
        Wire.beginTransmission(8);
        //Wire.write("Humidity: ");
        Wire.print(",");
        Wire.print(h);
        Wire.write("%");
        Wire.print(",");
        Wire.endTransmission();
        
         Wire.beginTransmission(8);
        //Wire.write("Temperature: ");
        Wire.print(t);
        Wire.println("C");
        Wire.endTransmission();
    }

    // Receive data from slave device #8
    Wire.requestFrom(8, 8);
    while (Wire.available()) {
        char c = Wire.read(); // Read the received byte
        Serial.print(c);      // Print the received character
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DTH_logic(){

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);
    float hif = dht.computeHeatIndex(f, h);
    float hic = dht.computeHeatIndex(t, h, false);


     digitalWrite (relay1,LOW);

 if (t<36){
    digitalWrite(relay2, LOW);
    digitalWrite(relay3, HIGH);
    digitalWrite(relay4, HIGH);
    servo.write(0);
}
  
 if (t>37 && t<60){
    digitalWrite(relay3, LOW);
    digitalWrite(relay2, HIGH);
    digitalWrite(relay4, HIGH);
    servo.write(0);
   }
  
 if (t>61){
    digitalWrite(relay4, LOW);
    digitalWrite(relay2, HIGH);
    digitalWrite(relay3, HIGH);
    servo.write(0);
    }
if (h>70){
    digitalWrite(relay4, LOW);
    digitalWrite(relay2, HIGH);
    digitalWrite(relay3, HIGH);
    servo.write(180);
    }
  
}

void loadcell (){/////////////////////////////////////////////////////////////////////////////////////
  LoadCell.update(); // retrieves data from the load cell
  float i = LoadCell.getData(); // get output value
  lcd.setCursor(10, 1); // set cursor to first row
  lcd.print("W="); // print out to LCD
  lcd.setCursor(12, 1); // set cursor to second row
  lcd.print(i); // print out the retrieved value to the second row

}
///////////////////////////////////////////////////////////////////////////////////////////////


void relay_states(){///////////////     
}


void lcd_backlight_off(){//////////////////////////////////////////////////////////
   
   Serial.print(customKey);
   if (customKey)  {
    beep100();
    lcd.backlight();
   }
    
   if (millis()-timeL>20000 &&  Status==true){
   timeL = millis();
        lcd.noBacklight();
   
   }
}
   void beep100()/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
   digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
 
}

void beep_beep()/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    delay(50);
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
}

void keypadevent() { /////////////////////////////////////////////////////////////////////////////////////////////////////


  customKey = Mahela.getKey();


 if (customKey == '*') {
  lcd.clear();
   lcd.backlight();
    digitalWrite(ledg,LOW);
    digitalWrite(ledb,LOW);
    beep_beep();
    
    update_eprom ();
   // heat_store ();
    
 
  }///////////////////////////////////////////////////

  if (customKey == '#') {
     lcd.backlight();
     digitalWrite(ledg,LOW);
    digitalWrite(ledb,LOW);
    beep_beep();
    time_set();
    }//////////////////////////////////////////////////////
    

}//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


void state(){/////////////////////////////////////////
 
  int address = 10;
  EEPROM.get(address, remainingPOS );

  if (remainingPOS == 0 ) {
   Status = false;
  }
 
  if (remainingPOS == 1 ) {
   Status = true;
  }

}//////////////////////////////////////////////////////////


void print_icon(){
  if (Status==true){
    lcd.setCursor(15,0);
  lcd.write(0);

  }
  if (Status==false){
    lcd.setCursor(15,0);
  lcd.write(1);
  
  }
  
}

void update_eprom () {
  
  int address = 10;
  EEPROM.get(address, remainingPOS );

  if (remainingPOS == 1 ) {
    EEPROM.update(address, 0 );
    heat_store ();
  }
  else {
    EEPROM.update(address, 1 );
       }
}


void heat_store () { //////////////////////////////////////////////////////////////
  
     lcd.backlight();
  int  n = 100;

  lcd.clear();
   lcd.backlight();

  lcd.print("Enter");
  lcd.setCursor(0, 1);
  lcd.print("Temperature");
  lcd.setCursor(15, 1);
  lcd.print("C");
  lcd.setCursor(13, 1);
  while (n < 102)
  {
    char num = Mahela.getKey();
    if (num)
    {
      number[n] = num ;
      lcd.print(num);
      EEPROM.write(n, num - 48);
      n++;
      beep100();
    }
  }
delay(2500);
 lcd.clear();
  customKey = 0;


}


void time_set () { //////////////////////////////////////////////////////////////
  
     lcd.backlight();
  int  n = 200;

  lcd.clear();
   lcd.backlight();

  lcd.print("Enter time in");
  lcd.setCursor(0, 1);
  lcd.print("minuets ");
  lcd.setCursor(12, 1);
  lcd.print("_");
  
  lcd.setCursor(12, 1);
  while (n < 203)
  {
    char num = Mahela.getKey();
    if (num)
    {
      number[n] = num ;
      lcd.print(num);
      EEPROM.write(n, num - 48);
      n++;
      beep100();
    }
  }
delay(2500);
 lcd.clear();
  customKey = 0;


}



 

void EEPROM_READ (){
  

  int x1 = EEPROM.read (100);
  int x2 = EEPROM.read (101);
  
  String val1 = String(x1) + String(x2);
  int Mheat = val1.toInt();

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);
    float hif = dht.computeHeatIndex(f, h);
    float hic = dht.computeHeatIndex(t, h, false);

       lcd.setCursor(0, 0);
  lcd.print("ST=");
  lcd.setCursor(3, 0);
  lcd.print(Mheat);

  
   lcd.setCursor(0, 1);
  lcd.print("T=");
   lcd.setCursor(3, 1);
  lcd.print(t);

    if (t <=Mheat){
       servo.write(0);
       delay(100);
        digitalWrite(relay1,LOW );
      delay(1000);
      
    }
    else{
  digitalWrite(relay1,HIGH );
  servo.write(0);
      
}
  
}

void Timer() {///////////////////////////////////////////////////////
  
  unsigned long currentMillis = millis();

 // const unsigned long interval2  ;

  int x1 = EEPROM.read (200);
  int x2 = EEPROM.read (201);
  int x3 = EEPROM.read (202);
  
  String val2 = String(x1) + String(x2) + String(x3);
  int TIME = val2.toInt();
  
        lcd.setCursor(6, 0);
        lcd.print("Time-");
        lcd.setCursor(12, 0);
        lcd.print(TIME);

  const unsigned long interval2 = 60000; 
  
  if (currentMillis - previousMillis >= interval2) {
    
    previousMillis = currentMillis;
    static int minutes = 0;
    minutes++;

  if (minutes >= TIME ) {
   
  
  //lcd.clear();
  //lcd.backlight();
  lcd.print("Dehydration");
  lcd.setCursor(0, 1);
  lcd.print("Done >>>>");
  EEPROM.write(200,0);
   EEPROM.write(201,0);
    EEPROM.write(202,0);
    delay(100);
     digitalWrite(relay3, HIGH);
    digitalWrite(relay2, HIGH);
    digitalWrite(relay4, HIGH);
    servo.write(0);
     digitalWrite(buzzer, HIGH);
    delay(1000);
    digitalWrite(buzzer, LOW);
    delay(500);
    digitalWrite(buzzer, HIGH);
    delay(1000);
    digitalWrite(buzzer, LOW);
     
      while(1);
      
    }
  }

  
}
