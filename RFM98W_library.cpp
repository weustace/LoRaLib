#include "RFM98W_library.h"

RFMLib::RFMLib(byte a, byte b, byte c, byte d){
    nss = (byte)a;
    dio0 = (byte)b;
    dio5 = (byte)c;
    rfm_rst = (byte)d;
    rfm_done = false;
    rfm_status =0;
}

void RFMLib::configure(byte config[6]){
    pinMode(nss,OUTPUT);
    digitalWrite(nss,HIGH);
    
    if(rfm_rst!=255){
        pinMode(rfm_rst,OUTPUT);
        digitalWrite(rfm_rst,HIGH);
    }
    
    pinMode(dio0,INPUT);
    if(dio5!=255)
    pinMode(dio5,INPUT);
    radioMode(0);
    wRFM(0x1D,config[0]);
    wRFM(0x1E,config[1]);//modem config registers
    wRFM(0x09,config[2]);//use PA_BOOST - even at the same power setting
    //it seems to give a stronger RSSI.
    wRFM(0x07,config[3]);//freq to 434.7MHz - mid SB
    wRFM(0x08,config[4]);//freq -LSB
	wRFM(0x26, config[5]);
}

void RFMLib::beginRX(){
    rfm_status = 2;
    rfm_done = false;
    radioMode(1);
    wRFM(0x12,255);//reset IRQ
    wRFM(0x0D,rRFM(0x0F));//set RX base address
    wRFM(0x40,0);//set up DIO0 interrupt
    radioMode(2);
    //You need to attach an interrupt function which sets this object's "rfm_done" bool to TRUE on a RISING interrupt on DIO0
}

void RFMLib::endRX(Packet& received){//function to be called on, or soon after, reception of RX_DONE interrupt
    rfm_done = false;
    rfm_status = 0;
    radioMode(1);//stby
    byte len = rRFM(0x13);//length of packet
    received.len = len;
    byte packet[(int)len];
	
    if(bitRead(rRFM(0x12),5)){
        received.crc = false;
    }
    else{
	    received.crc = true;
        wRFM(0x0D,0);
        brRFM(0x00,received.data,len);
    }
    received.rssi = (int)(rRFM(0x1B)-137);
    byte rawSNR = rRFM(0x19);
    if(bitRead(rawSNR,7)){
        received.snr = 0-(255-rawSNR);
    }
    else{
        received.snr = rawSNR;
    }
    received.snr /= 4;
    radioMode(0);//now sleeps
    wRFM(0x12,255);//clear IRQ again.
}

void RFMLib::endTX(){//function to be called at the end of transmission; cleans up.
    rfm_status = 0;
    rfm_done = false;
    radioMode(1);//stby
    wRFM(0x12,255);//clear IRQ
    radioMode(0);//sleep
}

void RFMLib::beginTX(Packet transmit_pkt){
    rfm_status = 1;
    rfm_done = false;
    radioMode(1);//stby
    wRFM(0x12,255);//clear IRQ
    wRFM(0x22,transmit_pkt.len);//set payload length;
    byte base_addr = rRFM(0x0E);
    wRFM(0x0D,base_addr);//Put transmit base FIFO addr in FIFO pointer
    byte new_data[transmit_pkt.len];
    for(int i = 0;i<transmit_pkt.len;i++){
        new_data[i] = transmit_pkt.data[i];
    }
    bwRFM(0x00,new_data,transmit_pkt.len);
    wRFM(0x0D,base_addr);//reset FIFO pointer
    wRFM(0x0D,base_addr);//and again...
    wRFM(0x40,0x40);//arm DIO0 interrupt
    radioMode(4);//begin transmit
    //you need to attach a rising interrupt on DIO0.
}

void RFMLib::radioMode(byte m){//set specified mode
    switch(m){
        case 0://sleep
            wRFM(0x01,0x80);
            break;
        case 1://stby
            wRFM(0x01,0x81);
            break;
        case 2://rx cont
            wRFM(0x01,0x85);
            break;
        case 3://rx single
            wRFM(0x01,0x86);
            break;
        case 4://tx
            wRFM(0x01,0x83);
            break;
    }
    
}

//Low-level IO functions beyond this point. ============================================
void RFMLib::wRFM(byte ad, byte val){//single byte write
    digitalWrite(nss,LOW);
    SPI.transfer(ad | 128);//set wrn bit - WRITE = 1
    SPI.transfer(val);
    digitalWrite(nss,HIGH);
}

void RFMLib::bwRFM(byte ad, byte vals[], int n){//burst write - less efficient but faster
    //for multiple bits
    //(less efficient for singles due to array overhead etc)
    digitalWrite(nss,LOW);
    SPI.transfer(ad | 128);//set wrn bit - WRITE = 1
    for(int i = 0;i<n;i++)
        SPI.transfer(vals[i]);
    
    digitalWrite(nss,HIGH);
}

byte RFMLib::rRFM(byte ad){//single byte read
    digitalWrite(nss,LOW);
    SPI.transfer(ad & B01111111);//wrn bit low
    byte val = SPI.transfer(0);//read, but we still have to spec a value?
    digitalWrite(nss,HIGH);
    return val;
}

void RFMLib::brRFM(byte ad, byte vals[], byte len){//burst read - slower for singles due to
    digitalWrite(nss,LOW);   //overhead of working with arrays
    SPI.transfer(ad & 0x7F);//wrn bit low
    for(int i = 0;i<len;i++){
        vals[i] = SPI.transfer(0);
    }  
	
   digitalWrite(nss,HIGH);
}
