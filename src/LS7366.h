#ifndef __LS7366__H
#define __LS7366_H

#include <stdint.h>

#include <Arduino.h>
#include <SPI.h>
#include <SPIDevice.h>

class LS7366 : SPIDevice
{
    public:
        SPISettings settie;

        LS7366(SPIClass& spi, const int8_t cs) : SPIDevice(spi, cs, 1000000, SPI_MSBFIRST, SPI_MODE0) {}; 
        // ~LS7366() {};

        void begin(); 

        // Read counter value via OTR, preferred way
        uint32_t read_otr(); 

        // Read counter value directly from counter. Faster but might interfere with counting!
        uint32_t read_cntr(); 

        // Clear CNTR register via 1 instruction
        void clear_cntr(); 

        // Clear CNTR register by loading, obsolete?
        void load_cntr(uint32_t cntr);
        
};

#endif
