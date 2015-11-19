#include <SPI.h>

#include <RFM98W_library.h>
RFMLib radio =RFMLib(20,7,16,21);
#define nss 20
void setup(){
  SPI.begin();
  Serial.begin(38400);
  byte my_config[6] = {0x44,0x84,0x88,0xAC,0xCD, 0x08};
  radio.configure(my_config);
}

void loop(){
  if(radio.rfm_status ==0){
    RFMLib::Packet p;
    p.data[0]=255;
    p.data[1]=243;
    p.len = 2;
    radio.beginTX(p); 
    attachInterrupt(7,RFMISR,RISING);
  }

  if(radio.rfm_done){
        Serial.println("Ending");   
    radio.endTX();
  }
  
}

void RFMISR(){
  Serial.println("interrupt");
 radio.rfm_done = true; 
}


