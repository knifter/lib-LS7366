#ifndef __LS7366__H
#define __LS7366_H

#include <stdint.h>

#include <Arduino.h>
#include <SPI.h>

class LS7366
{
    public:
        LS7366(SPIClass& spi, const int8_t ss) : _spi(spi), _ss(ss) {}; 
        ~LS7366() {};

        void begin(); 

        // Read counter value via OTR, preferred way
        uint32_t read_otr(); 

        // Read counter value directly from counter. Faster but might interfere with counting!
        uint32_t read_cntr(); 

        // Clear CNTR register via 1 instruction
        void clear_cntr(); 

        // Clear CNTR register by loading, obsolete?
        void load_cntr(uint32_t cntr);

    private:
        SPIClass& _spi;
        uint8_t _ss;
};

#endif
