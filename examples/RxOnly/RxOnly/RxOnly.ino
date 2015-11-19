#include <SPI.h>
#include <RFM98W_library.h>
RFMLib radio = RFMLib(20,7,16,21);
void setup(){
 SPI.begin();
 Serial.begin(38400);
  radio.configure();
  
}
void loop(){
 radio.transmit_pkt.len = 10;
 for(int i = 0;i<10;i++){
  radio.transmit_pkt.data[i] = 3;
 } 
 radio.beginTX();
}

