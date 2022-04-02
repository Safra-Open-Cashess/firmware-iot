#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

//Display
LiquidCrystal_I2C lcd(0x27, 2,1,0,4,5,6,7,3, POSITIVE);

//Constants
const int buzzer = 17;
const int rfidPin = 14;

//Keypad
const byte LINHAS = 4;
const byte COLUNAS = 3;
byte PinosLinhas[LINHAS] = {0, 1, 2, 3};
byte PinosColunas[COLUNAS] = {4, 5, 6};
char matriz_teclas[LINHAS][COLUNAS] = {
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'*','0','#'}
};

Keypad key_board = Keypad(makeKeymap(matriz_teclas), PinosLinhas, PinosColunas, LINHAS, COLUNAS);

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

    //Keyboard
    String valor = "";
    while(true){
        char tecla_pressionada = key_board.getKey();
        if (tecla_pressionada){
            buzzerTone(1, 1000, 100);
            Serial.print("Tecla pressionada : ");
            Serial.println(tecla_pressionada);

            if(tecla_pressionada == '*'){
                tecla_pressionada = '.';
            } else if(tecla_pressionada == '#'){
                break;
            }
            
            valor = valor + String(tecla_pressionada);
            Serial.println(valor);
            
            lcd.setCursor(1, 1);
            lcd.print(valor + "               ");
        }
    }

    buzzerTone(3, 2000, 100);

    lcd.setCursor(0, 1);
    lcd.print("Aprox. pulseira");
    delay(400);

    //Read RFID
    
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