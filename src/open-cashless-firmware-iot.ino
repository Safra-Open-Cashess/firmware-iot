#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <MFRC522.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <Keypad.h>

//RFID
#define SS_PIN 8 //10 - PINO SDA
#define RST_PIN 9 //9 - PINO DE RESET
MFRC522 rfid(SS_PIN, RST_PIN);

//Display
LiquidCrystal_I2C lcd(0x27, 2,1,0,4,5,6,7,3, POSITIVE);

//Constants
const int buzzer = 17;
const int rfidPin = 14;

//Ethernet
const char kHostname[] = "opencashless.wilson.eng.br";
const char kPath[] = "/api";
const int kNetworkTimeout = 30*1000;
const int kNetworkDelay = 1000;
const byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 201);
IPAddress dns(192, 168, 0, 1);
EthernetClient client;

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

    //Start RFID
    SPI.begin(); //INICIALIZA O BARRAMENTO SPI
    rfid.PCD_Init(); //INICIALIZA MFRC522

    //Start Display
    lcd.begin(16,2);
    lcd.clear();

    //Ok
    buzzerTone(2, 1500, 200);

    //Start ethernet
    Serial.println("Conectando-se a rede...");
    while(Ethernet.begin(mac) != 1){
        Serial.println("Erro DHCP, tentando novamente...");
        delay(3000);
    }
    Serial.println("Rede conectada!");

    delay(1000);

    welcomeMsgLCD();

}

void loop() {

    //Keyboard
    String valor = "";
    while(true){
        char pressed_key = key_board.getKey();
        if (pressed_key){
            buzzerTone(1, 1000, 100);
            Serial.print("Tecla pressionada : ");
            Serial.println(pressed_key);

            if(pressed_key == '*'){
                pressed_key = '.';
            } else if(pressed_key == '#'){
                break;
            }
            
            valor = valor + String(pressed_key);
            Serial.println(valor);
            
            lcd.setCursor(1, 1);
            lcd.print(valor + "               ");
        }
    }

    buzzerTone(3, 2000, 100);

    lcd.setCursor(0, 1);
    lcd.print("Aprox. pulseira");
    delay(400);

    //Led RFID
    digitalWrite(rfidPin, HIGH);
    delay(100);
    digitalWrite(rfidPin, LOW);
    delay(50);
    digitalWrite(rfidPin, HIGH);
    delay(150);
    digitalWrite(rfidPin, LOW);

    //Read RDID
    String rfid = readRFID();

    delay(1500);
    digitalWrite(rfidPin, HIGH);
    buzzerTone(1, 2000, 1000);
    digitalWrite(rfidPin, LOW);

    //Send data
    delay(500);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enviando dados...");
    String resp = postSale("{\"partner_id\": 1855, \"client_rfid\": \"" + rfid + "\", \"amount\": " + valor + "}");
    Serial.println(resp);

    //End
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dados enviados!");

    delay(1500);
    lcd.setCursor(0, 0);
    lcd.print(" Digite o valor");
    lcd.setCursor(1, 1);
    lcd.print("Open Cashless");
    
}

String readRFID(){
    
    //Wait for RFID
    while(!rfid.PICC_ReadCardSerial());

    String strID = "";
    for (byte i = 0; i < 4; i++) {
        strID +=
        (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
        String(rfid.uid.uidByte[i], HEX) +
        (i!=3 ? ":" : "");
    }
    strID.toUpperCase();
    
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();

    return strID;
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

String postSale(String data){

    String line;
    String response;
    String statusCode;
    
    if(client.connect("opencashless.wilson.eng.br", 80)){
        client.println("POST /api HTTP/1.1");
        client.println("Host: opencashless.wilson.eng.br");
        client.println("Content-Type: application/json");
        client.println("Connection:close");
        client.print("Content-Length:");
        client.println(data.length());
        
        client.println();
        client.print(data);
        client.println();
        client.println();

        while(client.connected() && !client.available()){
            delay(1);
        }

        Serial.println("INICIO");
        while (client.connected() || client.available()) {
            char c = client.read();
            if(String(c) == "\n"){
                //Serial.println("| " + line);
                if(line.startsWith("{")){
                    response = line;
                } else if(line.startsWith("HTTP/")){
                    statusCode = String(line[9]) + String(line[10]) + String(line[11]);
                    Serial.println("statusCode: " + statusCode);
                }
                line = "";
            } else {
                line += c;
            }
        }
        Serial.println("FIM");
        
        client.flush();
        client.stop();
    }

    if(statusCode != "200"){
        return "ERRO " + statusCode;
    }
    
    return response;
}