/*
 * File:   eeprom.c
 * Created on 31 January, 2025, 12:32 PM
 */

#include "eeprom.h"
#include "i2c.h"

unsigned char ext_eeprom_24C02_read(unsigned char addr) {
    unsigned char data;

    i2c_start();
    i2c_write(SLAVE_WRITE_EEPROM);
    i2c_write(addr);
    i2c_rep_start();
    i2c_write(SLAVE_READ_EEPROM);
    data = i2c_read(0);
    i2c_stop();

    return data;

}

void ext_eeprom_24C02_byte_write(unsigned char addr, char data) {
    i2c_start();
    i2c_write(SLAVE_WRITE_EEPROM);
    i2c_write(addr);
    i2c_write(data);
    i2c_stop();
}

void ext_eeprom_24C02_str_write(unsigned char addr, char *data) {
    
    while (*data != '\0') {
        ext_eeprom_24C02_byte_write(addr, *data);
        addr++;
        data++;
    }
}
