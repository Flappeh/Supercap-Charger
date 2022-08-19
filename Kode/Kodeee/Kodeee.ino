#include <Servo.h>
#include <LiquidCrystal_I2C.h>

#define scr1_gate   10
#define scr2_gate   15
#define scr3_gate   16
#define Vpin A1
#define Apin A0


//
LiquidCrystal_I2C lcd(0x27,16,2);
Servo servo;  // create servo object to control a servo

long previousMillis = 0; 
long interval = 600;  


int Vread = 0;
float Vcharge,Vbatre,AcsValueF= 0.0;
float R1 = 30000.0;
float R2 = 7500.0; 
float ref_voltage = 5.0;
float vFact = 5.176;

volatile byte state1 = LOW;
volatile byte state2 = LOW;
volatile byte state3 = LOW;
byte ZC1 = 0;
byte ZC2 = 0;
byte ZC3 = 0;

void setup(void){
  servo.write(75);
  lcd.init();                      // initialize the lcd 
  lcd.init();
  lcd.backlight();  servo.attach(9);  // attaches the servo on pin 9 to the servo object
  lcd.setCursor(0,0);
  lcd.print("SuperCAP Charger");
  lcd.setCursor(0,1);
  delay(1000);
  lcd.print("M. Fikri Alfandi");
  delay(1000);
  lcd.clear();
  pinMode(scr1_gate, OUTPUT);
  digitalWrite(scr1_gate, LOW);
  pinMode(scr2_gate, OUTPUT);
  digitalWrite(scr2_gate, LOW);
  pinMode(scr3_gate, OUTPUT);
  digitalWrite(scr3_gate, LOW);
  pinMode(Apin, INPUT);
  pinMode(Vpin, INPUT);
  pinMode(1, INPUT);
  pinMode(0, INPUT);
  Vread = analogRead(Vpin);
  Vbatre  = (Vread * ref_voltage) / 1024.0; 
  Vcharge = Vbatre * vFact ;  
  if(Vcharge <= 10.0){
    lcd.setCursor(1,0);
    lcd.print("Perlu Pengisian");
    lcd.setCursor(1,1);
    delay(1000);
    lcd.print("Mulai Charging");
//    lcd.print(Vcharge);
    delay(2000);
    servo.write(0);
    delay(2000);
    lcd.clear();
  }
  else if(Vcharge >= 10.0){
    lcd.setCursor(2,0);
    lcd.print("Baterai Cukup");
    lcd.setCursor(1,1);
    delay(1000);
    lcd.print("Tidak Charge");
//    lcd.print(Vcharge);
    delay(2000);
    servo.write(75);
    delay(2000);
    lcd.clear();
  }
  
  attachInterrupt(INT3, ZC_detect1, CHANGE);
  attachInterrupt(INT2 , ZC_detect2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(7) , ZC_detect3, CHANGE);
}
void ZC_detect1() {
  state1 = !state1;
    if(state1){
        ZC1=1;
      }
      else{
        ZC1=2;
      }
}
void ZC_detect2() {
  state2 = !state2;
  if(state2){
    ZC2=1;
  }
  else{
    ZC2=2;
  }
}
void ZC_detect3() {
  state3 = !state3;
  if(state3){
    ZC3=1;
  }
  else{
    ZC3=2;
  }
}
void loop() {
  CekV();
  if(Vcharge>=10.5){
    servo.write(75);
    lcd.setCursor(1,0);
    lcd.print("Status Charging");
    lcd.setCursor(0,1);
    lcd.print("Selesai!");   
    delay(5000);
  }
  while(Vcharge<10.9){ 
    CekV();
    if(millis() - previousMillis > interval){
      CekA();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Status Charging");
      lcd.setCursor(0,1);
      lcd.print("V :" + String(Vcharge)+ " A ="+ String(AcsValueF));   
      previousMillis = millis(); 
    }
    if(ZC1 == 1){
      delayMicroseconds(3500);
      digitalWrite(scr1_gate, HIGH);
      delayMicroseconds(2000);
      digitalWrite(scr1_gate, LOW);
      ZC1 = 0;
    }
    if(ZC1 == 2){
      delayMicroseconds(3500);
      digitalWrite(scr2_gate, HIGH);
      delayMicroseconds(2000);
      digitalWrite(scr2_gate, LOW);
      ZC1 = 0;
    }
    if(ZC2 == 1){
      delayMicroseconds(5500);
      digitalWrite(scr2_gate, HIGH);
      delayMicroseconds(3500);
      digitalWrite(scr2_gate, LOW);
      ZC2 = 0;
    }
    if(ZC2 == 2){
      delayMicroseconds(5500);
      digitalWrite(scr3_gate, HIGH);
      delayMicroseconds(3500);
      digitalWrite(scr3_gate, LOW);
      ZC2 = 0;
    }
    if(ZC3 == 1){
      delayMicroseconds(3500);
      digitalWrite(scr3_gate, HIGH);
      delayMicroseconds(2000);
      digitalWrite(scr3_gate, LOW);
      ZC3 = 0;
    }
    if(ZC3 == 2){
      delayMicroseconds(3500);
      digitalWrite(scr1_gate, HIGH);
      delayMicroseconds(2000);
      digitalWrite(scr1_gate, LOW);
      ZC3 = 0;
    }
  }
}

void CekV(){
//  Vread = 0;
//  for(int i = 0; i<10; i++){
//    Vread += analogRead(Vpin);  
//  }
//  Vcharge = Vread*0.5/1024.0;
//  Vbatre = Vcharge/0.2;
   Vread = analogRead(Vpin);
   Vbatre  = (Vread * ref_voltage) / 1024.0; 
   Vcharge = Vbatre *vFact ; 
}
void CekA(){
  unsigned int x=0;
  float AcsValue=0.0,Samples=0.0,AvgAcs=0.0;
  for (int x = 0; x < 100; x++){ //Get 150 samples
    AcsValue = analogRead(Apin);     //Read current sensor values   
    Samples = Samples + AcsValue;  //Add samples together
    delayMicroseconds(10); // let ADC settle before next sample 3ms
  }
  AvgAcs=Samples/100.0;//Taking Average of Samples 
  AcsValueF = (2.5 - (AvgAcs * (5.0 / 1024.0)) )/0.066;
}
