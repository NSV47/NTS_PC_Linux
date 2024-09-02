#include <Arduino.h>

#define E22_33

#include "LoRa_E22.h"

// With FIXED RECEIVER configuration
// #define DESTINATION_ADDL 2
#define DESTINATION_ADDL 3

// HardwareSerial SerialPort(2);

LoRa_E22 e22ttl(18, 17, &Serial1, 8, 46, 3, UART_BPS_RATE_9600, SERIAL_8N1); //  esp32 RX <-- e22 TX, esp32 TX --> e22 RX AUX M0 M1 // UART_BPS_RATE_9600

// put function declarations here:
void printParameters(struct Configuration configuration);
void printModuleInformation(struct ModuleInformation moduleInformation);
void somethingAvailableLORA();
void initEbyteE22();
int myFunction(int, int);
void serialEvent();

bool stringComplete = false;  // whether the string is complete
String inputString = "";         // a String to hold incoming data

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // SerialPort.begin(115200, SERIAL_8N1, 38, 2);
  initEbyteE22();
  //------------------------------------------
  Serial1.end();
  Serial1.begin(115200, SERIAL_8N1, 18, 17);
  
  // SerialPort.println(str);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  somethingAvailableLORA();

  while(Serial.available()>0){
    char inChar = (char)Serial.read();
    inputString += inChar;
    stringComplete = true;
  }

  if(stringComplete){
    stringComplete = false;
    // SerialPort.print(inputString);
    ResponseStatus rs = e22ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, inputString); // 20
    inputString = "";
  }

  // if(SerialPort.available()>0){
    // Serial.write(SerialPort.read());
  // }
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

void somethingAvailableLORA(){
    // If something available
    if (e22ttl.available()>1) {
        // read the String message
    #ifdef ENABLE_RSSI
        //ResponseContainer rc = e22ttl.receiveMessageRSSI();
        ResponseStructContainer rsc = e22ttl.receiveMessageRSSI(sizeof(Message));
    #else
        ResponseContainer rc = e22ttl.receiveMessage();
        // ResponseStructContainer rsc = e22ttl.receiveMessage(sizeof(Message));
    #endif
        // Is something goes wrong print error
        if (rc.status.code!=1){
            // Serial.println(rc.status.getResponseDescription());
            // Serial.println(rsc.status.getResponseDescription());
        }else{
            // Print the data received
            // Serial.println(rc.status.getResponseDescription());
            
            Serial.print(rc.data);
            
    #ifdef ENABLE_RSSI
            Serial.print(F("RSSI: ")); Serial.println(rc.rssi, DEC);
    #endif
        
        }
    }
}

void initEbyteE22(){
    e22ttl.begin();

  	ResponseStructContainer c;

  	c = e22ttl.getConfiguration();

    // It's important get configuration pointer before all other operation
    Configuration configuration = *(Configuration*) c.data;
    Serial.println(c.status.getResponseDescription());
    Serial.println(c.status.code);

    printParameters(configuration);

    //---------------------------------------------------------------------
    configuration.ADDL = 0x02;
    // configuration.ADDL = 0x03;
    configuration.ADDH = 0x00;
    configuration.NETID = 0x00;

    configuration.CHAN = 23;

    configuration.SPED.uartBaudRate = UART_BPS_115200; // UART_BPS_115200 // UART_BPS_9600
    configuration.SPED.airDataRate = AIR_DATA_RATE_101_192; // AIR_DATA_RATE_001_12  // AIR_DATA_RATE_011_48 // AIR_DATA_RATE_100_96 // AIR_DATA_RATE_101_192 // AIR_DATA_RATE_110_384 // AIR_DATA_RATE_111_625 
    configuration.SPED.uartParity = MODE_00_8N1;

    configuration.OPTION.subPacketSetting = SPS_240_00;
    configuration.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_DISABLED;
    configuration.OPTION.transmissionPower = POWER_24; //POWER_30 // POWER_21

    configuration.TRANSMISSION_MODE.enableRSSI = RSSI_DISABLED;                // ENABLE_RSSI
    //configuration.TRANSMISSION_MODE.enableRSSI = RSSI_ENABLED;                // ENABLE_RSSI
    configuration.TRANSMISSION_MODE.fixedTransmission = FT_FIXED_TRANSMISSION;
    configuration.TRANSMISSION_MODE.enableRepeater = REPEATER_DISABLED;
    configuration.TRANSMISSION_MODE.enableLBT = LBT_DISABLED;
    //configuration.TRANSMISSION_MODE.WORTransceiverControl = WOR_RECEIVER;
    configuration.TRANSMISSION_MODE.WORTransceiverControl = WOR_TRANSMITTER;
    configuration.TRANSMISSION_MODE.WORPeriod = WOR_2000_011;

    //ResponseStatus rs = e22ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_LOSE);
    e22ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_LOSE);

    printParameters(configuration);
    //---------------------------------------------------------------------

    ResponseStructContainer cMi;
    cMi = e22ttl.getModuleInformation();
    // It's important get information pointer before all other operation
    ModuleInformation mi = *(ModuleInformation*)cMi.data;

    Serial.println(cMi.status.getResponseDescription());
    Serial.println(cMi.status.code);

    c.close();
    cMi.close();

    printModuleInformation(mi);
}

void printParameters(struct Configuration configuration) {
    Serial.println("----------------------------------------");

    Serial.print(F("HEAD : "));  Serial.print(configuration.COMMAND, HEX);Serial.print(" ");Serial.print(configuration.STARTING_ADDRESS, HEX);Serial.print(" ");Serial.println(configuration.LENGHT, HEX);
    Serial.println(F(" "));
    Serial.print(F("AddH : "));  Serial.println(configuration.ADDH, HEX);
    Serial.print(F("AddL : "));  Serial.println(configuration.ADDL, HEX);
    Serial.print(F("NetID : "));  Serial.println(configuration.NETID, HEX);
    Serial.println(F(" "));
    Serial.print(F("Chan : "));  Serial.print(configuration.CHAN, DEC); Serial.print(" -> "); Serial.println(configuration.getChannelDescription());
    Serial.println(F(" "));
    Serial.print(F("SpeedParityBit     : "));  Serial.print(configuration.SPED.uartParity, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getUARTParityDescription());
    Serial.print(F("SpeedUARTDatte     : "));  Serial.print(configuration.SPED.uartBaudRate, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getUARTBaudRateDescription());
    Serial.print(F("SpeedAirDataRate   : "));  Serial.print(configuration.SPED.airDataRate, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getAirDataRateDescription());
    Serial.println(F(" "));
    Serial.print(F("OptionSubPacketSett: "));  Serial.print(configuration.OPTION.subPacketSetting, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getSubPacketSetting());
    Serial.print(F("OptionTranPower    : "));  Serial.print(configuration.OPTION.transmissionPower, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getTransmissionPowerDescription());
    Serial.print(F("OptionRSSIAmbientNo: "));  Serial.print(configuration.OPTION.RSSIAmbientNoise, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getRSSIAmbientNoiseEnable());
    Serial.println(F(" "));
    Serial.print(F("TransModeWORPeriod : "));  Serial.print(configuration.TRANSMISSION_MODE.WORPeriod, BIN);Serial.print(" -> "); Serial.println(configuration.TRANSMISSION_MODE.getWORPeriodByParamsDescription());
    Serial.print(F("TransModeTransContr: "));  Serial.print(configuration.TRANSMISSION_MODE.WORTransceiverControl, BIN);Serial.print(" -> "); Serial.println(configuration.TRANSMISSION_MODE.getWORTransceiverControlDescription());
    Serial.print(F("TransModeEnableLBT : "));  Serial.print(configuration.TRANSMISSION_MODE.enableLBT, BIN);Serial.print(" -> "); Serial.println(configuration.TRANSMISSION_MODE.getLBTEnableByteDescription());
    Serial.print(F("TransModeEnableRSSI: "));  Serial.print(configuration.TRANSMISSION_MODE.enableRSSI, BIN);Serial.print(" -> "); Serial.println(configuration.TRANSMISSION_MODE.getRSSIEnableByteDescription());
    Serial.print(F("TransModeEnabRepeat: "));  Serial.print(configuration.TRANSMISSION_MODE.enableRepeater, BIN);Serial.print(" -> "); Serial.println(configuration.TRANSMISSION_MODE.getRepeaterModeEnableByteDescription());
    Serial.print(F("TransModeFixedTrans: "));  Serial.print(configuration.TRANSMISSION_MODE.fixedTransmission, BIN);Serial.print(" -> "); Serial.println(configuration.TRANSMISSION_MODE.getFixedTransmissionDescription());

    Serial.println("----------------------------------------");
}

void printModuleInformation(struct ModuleInformation moduleInformation) {
    Serial.println("----------------------------------------");
    Serial.print(F("HEAD: "));  Serial.print(moduleInformation.COMMAND, HEX);Serial.print(" ");Serial.print(moduleInformation.STARTING_ADDRESS, HEX);Serial.print(" ");Serial.println(moduleInformation.LENGHT, DEC);

    Serial.print(F("Model no.: "));  Serial.println(moduleInformation.model, DEC);
    Serial.print(F("Version  : "));  Serial.println(moduleInformation.version, DEC);
    Serial.print(F("Features : "));  Serial.println(moduleInformation.features, BIN);
    Serial.println("----------------------------------------");

}