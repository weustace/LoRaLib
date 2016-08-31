/* (C) Team Impulse 2015
 * Made available under a modification of the MIT license - see repository root.
 * Comms library: RFM98W
 */

#ifndef RFMLib_h
#define RFMLib_h

#include <Arduino.h>
#include <SPI.h>

class RFMLib {
public:

    RFMLib(byte nss, byte dio0, byte dio5, byte rfm_rst);
    typedef struct Packet{//data structure for storing a packet
        byte len;
        byte data[256];
        int snr;
        int rssi;
        bool crc;//true for success, false for failure. Can read values if true.
    } Packet;
    void configure(byte config[6]);
    //Set the radio frequency to a given value in Hz
    void setFrequency(uint32_t frequency);
    void beginTX(Packet tx);
    void endTX();
    void beginRX();
    void endRX(Packet &received);

    volatile bool rfm_done;
    byte rfm_status;//0=idle,1=tx,2=rx

    //Set to true to enable high frequency mode
    bool high_frequency = false;
    //Return silicon version
    uint8_t getVersion();

private:
    void radioMode(byte m);//set the mode of the radio

    void wRFM(byte ad, byte val);//IO functions
    void bwRFM(byte ad, byte vals[], int n);
    byte rRFM(byte ad);
    void brRFM(byte ad, byte vals[], byte len);

    const uint32_t Fosc = 32000000;


    byte nss, dio0, dio5, rfm_rst;//pins - to be assigned at instantiation.

};

#endif
