#include <Wire.h>
#include "RTClib.h"
#include <EEPROM.h>
#include <Servo.h>

Servo myservo;  // create servo object to control a servo 

RTC_DS1307 RTC;

char receivedChar;
boolean newData = false;
int pos_standby = 180;    // variable to store the servo position 
int pos_active = 100;
int led = 13;
int mode = 1;             //1 for PROD, 2 for DEV (Serial Input
int bulb_status = 0;
int starttime = 0;
int endtime = 0;
int loopcount = 0;
int address = 12;
byte value;

void setup() { 
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
  }
  
  read_eeprom();              // Gets current position from EEPROM
  myservo.attach(3);          // Attaches the servo on pin 3 to the servo object 
  myservo.write(pos_standby); // Puts servo in default position
  Serial.println("Setup Complete");

  //#### Uncomment the below to set the RTC to the date & time this sketch was compiled ###
  //#### Then comment it out again and reupload sketch to Arduino ###  
  //RTC.adjust(DateTime(__DATE__, __TIME__));
}

void loop() {
  print_time();
  delay(500); 
  while (true){
   
      if (mode == 1) {
        bulb_sequence();
        }      
  
      starttime = millis();
      endtime = starttime;
      while (((endtime - starttime) <=10000) || (loopcount < 10000)) // do this loop for up to 10000mS
        {
        loopcount = loopcount+1;
        endtime = millis();
        }  
        
   recvOneChar();
   showNewData();

    if (receivedChar=='a') {
    Serial.println("A Selected");
    servo_move(1); //Bulb ON
    }  
    else if (receivedChar=='b') {
    Serial.println("B Selected");
    servo_move(2); //Bulb BLUE
    }     
    else if (receivedChar=='c') {
    Serial.println("C Selected");
    servo_move(3); //Bulb OFF
    }    
   receivedChar='d';
        
  }
}

void recvOneChar() {
 if (Serial.available() > 0) {
 receivedChar = Serial.read();
 newData = true;
 }
}

void showNewData() {
 if (newData == true) {
 //Serial.print("This just in ... ");
 //Serial.println(receivedChar);
 newData = false;
 }
}

void bulb_sequence() {
      DateTime now = RTC.now();
      if (now.hour() > 10 && now.hour() < 20 && bulb_status!=1) {
          servo_move(1); //Bulb ON
          }
      else if (now.hour() > 19 && now.hour() < 23 && bulb_status!=2) {
          servo_move(2); //Bulb BLUE
          }
      else if (now.hour()>=23 && bulb_status!=3) {
          servo_move(3); //Bulb OFF
          }   
          //Serial.println(bulb_status); 
          //Serial.println(now.hour);     
}

void servo_move(int x) 
{ 
  read_eeprom();
  while (bulb_status != x) {
    servo(); 
  }
  write_eeprom();   
} 

void servo() 
{ 
  myservo.write(pos_active);              // tell servo to go to position in variable 'pos' 
  delay(1000);                     // waits 1s for the servo to reach the position                       
  myservo.write(pos_standby);              // tell servo to go to position in variable 'pos' 
  delay(1000);                     // waits 1s for the servo to reach the position  
  bulb_status += 1;
  if (bulb_status == 4) {
    bulb_status = 1;
    digitalWrite(led, HIGH);  
    }
  digitalWrite(led, LOW);  
} 

void print_time() {
    DateTime now = RTC.now(); 
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
}

void read_eeprom()
{
  // read a byte from the current address of the EEPROM
  value = EEPROM.read(address);
  Serial.print("EERPROM Stored Value at Address: "); 
  Serial.print(address);
  Serial.print("\t");
  Serial.print(value, DEC);
  Serial.println();
  bulb_status = value;
}

void write_eeprom()
{
  EEPROM.write(address, bulb_status);
  Serial.print("New EEPROM: ");
  Serial.println(bulb_status);
}
