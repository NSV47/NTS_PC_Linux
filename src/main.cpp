#include <Arduino.h>

HardwareSerial SerialPort(2);

// put function declarations here:
int myFunction(int, int);
void serialEvent();

bool stringComplete = false;  // whether the string is complete
String inputString = "";         // a String to hold incoming data

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SerialPort.begin(115200, SERIAL_8N1, 38, 2);
  String str="Ok";
  SerialPort.println(str);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (stringComplete) {
  #if 1
    uint16_t strLen = inputString.length();
    for(uint16_t i = 0; i < strLen; ++i){
      if(inputString[i] <= 0xF){
        SerialPort.print('0');
      }
      SerialPort.print(inputString[i], HEX);
      // SerialPort.print(' ');
    }
    SerialPort.println();
    SerialPort.println();

    inputString = "";
    stringComplete = false;
  #endif
  #if 0
    SerialPort.println(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  #endif
  }
  
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  if (Serial.available()) { // while
    char inChar = (char)Serial.read();
    
    if(inChar == 0xE0){
      stringComplete = true;
    }else{
      inputString += inChar;
    }
  }
}