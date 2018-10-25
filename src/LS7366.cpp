#include "LS7366.h"

#define REG_MDR0                0b00001000
    #define MDR0_QUAD_NONE      0x00
    #define MDR0_QUAD_X1        0x01
    #define MDR0_QUAD_X2        0x02
    #define MDR0_QUAD_X4        0x03
    #define MDR0_MODE_FREE      0x00
    #define MDR0_MODE_SINGLE    0x04
    #define MDR0_MODE_RANGELIM  0x08
    #define MDR0_MODE_MODULO    0x0C
    #define MDR0_INDEX_DISABLE  0x00
    #define MDR0_INDEX_LOAD_C   0x10
    #define MDR0_INDEX_RESET_C  0x20
    #define MDR0_INDEX_LOAD_O   0x30
    #define MDR0_POSITIVE_INDEX 0x40
    #define MDR0_CLOCKDIV       0x80
#define REG_MDR0_CFG            (MDR0_QUAD_X1 | MDR0_MODE_FREE | MDR0_INDEX_DISABLE)
#define REG_MDR1                0b00010000
    #define MDR1_4BYTE          0b00000000
    #define MDR1_3BYTE          0b00000001
    #define MDR1_2BYTE          0b00000010
    #define MDR1_1BYTE          0b00000011
    #define MDR1_COUNTING       0b00000100
    #define MDR1_FLAG_IDX       0b00010000
    #define MDR1_FLAG_CMP       0b00100000
    #define MDR1_FLAG_BW        0b01000000
    #define MDR1_FLAG_CY        0b00010000
#define REG_MDR1_CFG            (MDR1_4BYTE)
#define REG_DTR                 0b00011000
#define REG_CNTR                0b00100000
#define REG_OTR                 0b00101000
#define REG_STR                 0b00110000
#define OP_CLR                  0b00000000
#define OP_RD                   0b01000000
#define OP_WR                   0b10000000
#define OP_LOAD                 0b11000000

#define SPI_DELAY               100

// ****************************************************
// Initializes our encoders
// RETURNS: long
// ****************************************************
void LS7366::begin() 
{
    // Initialize encoder
    //    Clock division factor: 0
    //    Negative index input
    //    free-running count mode
    //    x4 quatrature count mode (four counts per quadrature cycle)
    // NOTE: For more information on commands, see datasheet
    // was:
    // wr(0x88)     OP_WR | MDR0
    // wr(0x03)     MDR0_QUAD_X4
    digitalWrite(_ss, LOW);
    _spi.transfer(OP_WR | REG_MDR0);
    _spi.transfer(REG_MDR0_CFG);
    digitalWrite(_ss, HIGH);

    digitalWrite(_ss, LOW);
    _spi.transfer(OP_WR | REG_MDR1);
    _spi.transfer(REG_MDR1_CFG);
    digitalWrite(_ss, HIGH);
}


// ****************************************************
// Reads the CNTR to retreive the current value
// Datasheets warns that this might interfere with the counter and suggests
// using the LOAD|OTR, RD|OTR method which is implemented in read_otr()
// ****************************************************
uint32_t LS7366::read_cntr() 
{
    digitalWrite(_ss, LOW);     // Begin SPI conversation

    // TvR declare variables where I use them
    uint32_t count;
    // was:
    // wr(0x60 0x00 0x00 0x00 0x00)
    _spi.transfer(OP_RD | REG_CNTR);
    count = _spi.transfer(0x00);
    count <<= 8;
    count |= _spi.transfer(0x00);
    count <<= 8;
    count |= _spi.transfer(0x00);
    count <<= 8;
    count |= _spi.transfer(0x00);
    digitalWrite(_ss, HIGH);    // Terminate SPI conversation

    // Serial.print("count: 0x");
    // Serial.println(readEncoder(), HEX);

    return count;
}

// ****************************************************
// Reads the CNTR register by first (parallel) copying it to OTR and then reading OTR
// ****************************************************
uint32_t LS7366::read_otr() 
{
    // Transfer CNTR -> OTR
    digitalWrite(_ss, LOW);     // Begin SPI conversation
    _spi.transfer(OP_LOAD | REG_OTR);
    digitalWrite(_ss, HIGH);     // Begin SPI conversation

    delayMicroseconds(SPI_DELAY);

    // Read OTR
    uint32_t count;
    _spi.transfer(OP_RD | REG_OTR);
    count = _spi.transfer(0x00);
    count <<= 8;
    count |= _spi.transfer(0x00);
    count <<= 8;
    count |= _spi.transfer(0x00);
    count <<= 8;
    count |= _spi.transfer(0x00);
    digitalWrite(_ss, HIGH);    // Terminate SPI conversation
}

// ****************************************************
// Resets Encoders to 0x00 0x00 0x00 0x00 (0 0 0 0)
// RETURNS: N/A
// ****************************************************
void LS7366::clear_cntr() 
{  
    digitalWrite(_ss, LOW);     // Begin SPI conversation
    _spi.transfer(OP_CLR | REG_CNTR);
    digitalWrite(_ss, HIGH);    // Terminate SPI conversation
};

void LS7366::load_cntr(uint32_t cntr) 
{
    // Write to DTR and load 0-data
    digitalWrite(_ss, LOW);     // Begin SPI conversation
    // 0x98 = 0b10 011 000 = OP_WR | REG_DTR
    _spi.transfer(OP_WR | REG_DTR);
    _spi.transfer(cntr & 0x000000FF);
    cntr >> 8;
    _spi.transfer(cntr & 0x000000FF);
    cntr >> 8;
    _spi.transfer(cntr & 0x000000FF);
    cntr >> 8;
    _spi.transfer(cntr & 0x000000FF);
    digitalWrite(_ss, HIGH);    // Terminate SPI conversation

    delayMicroseconds(SPI_DELAY);  // provides some breathing room between SPI converrsations

    // Set encoder1's current data register to center
    digitalWrite(_ss, LOW);     // Begin SPI conversation
    // 0xE0 = 0b11 100 000 = OP_LOAD | REG_CNTR = Load CNTR from DTR
    _spi.transfer(OP_LOAD | REG_CNTR);
    digitalWrite(_ss, HIGH);    // Terminate SPI conversation
}