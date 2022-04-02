#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//Display
LiquidCrystal_I2C lcd(0x27, 2,1,0,4,5,6,7,3, POSITIVE);

//Constants
const int buzzer = 17;
const int rfidPin = 14;

void setup() {

    //Led RFID
    pinMode(rfidPin, OUTPUT);
    digitalWrite(rfidPin, LOW);
    
    //Serial
    Serial.begin(9600);
    while(!Serial);

    //Start Display
    lcd.begin(16,2);
    lcd.clear();

    //Ok
    buzzerTone(2, 1500, 200);

    delay(1000);

    welcomeMsgLCD();

}

void loop() {
    
}

void buzzerTone(int x, int freq, int delayTime){
    for(int i=0;i<x; i++){
        pinMode(buzzer, freq);
        tone(buzzer,1500);
        delay(delayTime);
        noTone(buzzer);
    }
}

void welcomeMsgLCD(){
    lcd.clear();
    
    lcd.setCursor(5, 0);
    lcd.print("Safra");
    
    lcd.setCursor(1, 1);
    lcd.print("Open Cashless");
    
    delay(1000);
    
    lcd.clear();
    
    lcd.setCursor(0, 0);
    lcd.print("   Bem vindo");

    lcd.setCursor(1, 1);
    lcd.print("Open Cashless");
    
    delay(1000);

    lcd.setCursor(0, 0);
    lcd.print(" Digite o valor");
}