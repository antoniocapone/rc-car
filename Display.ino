#define SERIAL_BAUDRATE 115200
struct Switches{
  bool switch1; //partono da sinistra
  bool switch2;
  bool switch3;
  bool switch4;
  bool switch5;
  bool switch6;
};
Switches swiches;

void read_from_serial(){
  if(Serial.available()>0){
    delay(5);
    char c = Serial.read();
    if(c == "#"){
      swiches.switch1 = (bool)Serial.read(); 
      swiches.switch2 = (bool)Serial.read();
      swiches.switch3 = (bool)Serial.read();
      swiches.switch4 = (bool)Serial.read();
      swiches.switch5 = (bool)Serial.read();
      swiches.switch6 = (bool)Serial.read();
      if(Serial.read() == "*"){
        return;  
      }
    }
  }  
}



void setup(){
  Serial.begin(SERIAL_BAUDRATE);
}

void loop(){
  read_from_serial();
}
