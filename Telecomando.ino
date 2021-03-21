#include <SPI.h>
#include <Wire.h> //port exp 1=0x4A 2=0x48 3=0x4C
#include "nRF24L01.h"
#include "RF24.h"


//cosa mandiamo? 4 switch, 4 encoder, 2 Joystick, 
//Switch ---> 4 bool
//Encoder 4 int + 4 bool
//2 Joystick ---> 4 byte 2 bool




//-------------------------------------------------------
//DEFINIZIONE DATI DA MANDARE
struct RadioData{
  //partono da sinistra
  bool switch1; //accensione auto
  bool switch2; //abilita esp
  bool switch3; //abilita abs
  bool switch4; 
  bool switch5;
  bool switch6;

  short xsx; 
  short ysx;
  short xdx;
  short ydx;
  bool buttonsx;
  bool buttondx;

  short encoder1;
  short encoder2;
  short encoder3;
  short encoder4;
  bool encoderButton1;
  bool encoderButton2;
  bool encoderButton3;
  bool encoderButton4;

  
}; RadioData data;
//---------------------------------------------
//---------------------------------------------

//---------------------------------------------
//VARIABILI DEI JOYSTICK
#define J_SX_BUTTON A0
#define J_DX_BUTTON A7
#define J_SX_X A2
#define J_SX_Y A1
#define J_DX_X A3
#define J_DX_Y A6
void read_joystick(){
  data.ysx = analogRead(J_SX_Y);  
  data.ydx = analogRead(J_DX_Y); 
  data.xsx = analogRead(J_SX_X); 
  data.xsx = analogRead(J_SX_X);
  if(analogRead(J_SX_BUTTON)>= 512){
    data.buttonsx = true;  
  }else{
    data.buttonsx = false;  
  }
  if(analogRead(J_DX_BUTTON)>= 512){
    data.buttondx = true;  
  }else{
    data.buttondx = false;  
  }
  
}
//---------------------------------------------
//---------------------------------------------

//---------------------------------------------
//VARIABILI NRF
#define MOSI 11
#define MISO 12
#define SCK 13
#define CE 10
#define CSN 6
RF24 radio(CE, CSN); 
const byte address[6] = "00001";
void setup_nrf(){
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_2MBPS);
  radio.stopListening();  
}
//-------------------------------------------- 


//--------------------------------------------
//VARIABILI DEI PORT EXPANDER
#define PEXP1 0x4A
#define PEXP2 0x48
#define PEXP3 0x4C
int stato = 1;
byte requestedPayload;
void read_pexp(){
  switch(stato){
    case 1:
      if(!Wire.available()) Wire.requestFrom(PEXP1, 1); //port expander 1, 1 byte
      if(Wire.available()){
        stato = 2;
        requestedPayload = Wire.read(); 
        data.switch6 = (bool)bitRead(requestedPayload, 0);  
      }
      break;
    case 2:
      if(!Wire.available()) Wire.requestFrom(PEXP2, 1); //port expander 2, 1 byte
      if(Wire.available()){
        stato = 3;
        requestedPayload = Wire.read(); 
        data.switch1 = (bool)bitRead(requestedPayload, 0);
        data.switch2 = (bool)bitRead(requestedPayload, 1); 
        data.switch3 = (bool)bitRead(requestedPayload, 2); 
        data.switch4 = (bool)bitRead(requestedPayload, 3); 
        data.switch5 = (bool)bitRead(requestedPayload, 4); 
      }
      break;
    case 3:
      if(!Wire.available()) Wire.requestFrom(PEXP3, 1); //port expander 3, 1 byte
      if(Wire.available()){
        stato = 1;
        requestedPayload = Wire.read();  
      }
      break;
  }  
}


//---------------------------------------------
//COMUNICAZIONE SERIALE
#define SERIAL_BAUDRATE 115200
unsigned long Elapsed = 0;
#define SERIAL_WAIT 300
String serial_payload = "";

void MarkTime(){
  Elapsed = millis();  
}

void send_data_to_display(){
  if((millis - Elapsed) >= SERIAL_WAIT){
    //TO-DO: Comunicazione dei dati via seriale all'arduino dei display
    serial_payload = serial_payload + '#';
    serial_payload = serial_payload + (char)data.switch1; 
    serial_payload = serial_payload + (char)data.switch2;
    serial_payload = serial_payload + (char)data.switch3;
    serial_payload = serial_payload + (char)data.switch4;
    serial_payload = serial_payload + (char)data.switch5;
    serial_payload = serial_payload + (char)data.switch6;
    serial_payload = serial_payload + '*';
    Serial.print(serial_payload);
    serial_payload = "";
    MarkTime();
  }  
}
//---------------------------------------------
//---------------------------------------------
void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  Wire.begin();
  setup_nrf();
}

void loop() {
  read_joystick(); //fatto e apposto 
  read_pexp(); //da rivedere
  radio.write(&data, sizeof(data));
  send_data_to_display(); //da fare urgentemente 
}
