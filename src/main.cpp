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
  while(Serial.available()>0){
    char inChar = (char)Serial.read();
    inputString += inChar;
    stringComplete = true;
  }

  if(stringComplete){
    stringComplete = false;
    SerialPort.print(inputString);
    inputString = "";
  }

  if(SerialPort.available()>0){
    Serial.write(SerialPort.read());
  }
//--------------------------------------------------
  if (stringComplete) {
    // SerialPort.print(inputString);
  #if 0
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
#if 0
void serialEvent() {
  if (Serial.available()) { // while
    char inChar = (char)Serial.read();
    
    if(inChar == 0xC0){
      inputString += 0xC0;
      stringComplete = true;
    }else{
      inputString += inChar;
    }
  }
}
#endif