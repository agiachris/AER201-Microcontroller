/* 
 * File:   EEPROM_storage.c
 * Author: Christopher Agia
 *
 * Created on February 27, 2018, 1:24 PM
 */

#include "configBits.h"
#include "lcd.h"
#include "add_functions.h"
#include "I2C.h"
#include "RTC.h"
#include "machine_code.h"
#include "EEPROM_storage.h"
#include "UART_PIC.h"


void write_EEPROM(unsigned char address, unsigned char data){
    while( EECON1bits.WR  ){continue;} //checking if not busy with an earlier write.

    EECON1bits.WREN=1; // Enable writing to EEPROM 
    EEADR=address; // load address 
    EEDATA=data; // load data
    EECON1bits.EEPGD=0; // access EEPROM memory
    EECON1bits.CFGS=0; // avoid access configuration registers
    INTCONbits.GIE=0; // disable interrupts for critical EEPROM write sequence
    // required sequence start
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    // required sequence end
    INTCONbits.GIE = 1; // enable interrupts, critical sequence complete
    while (EECON1bits.WR==1); // wait for write to complete
    EECON1bits.WREN=0;  // do not allow EEPROM writes
}

unsigned char read_EEPROM(unsigned char address){
    while( EECON1bits.WR  ){continue;} //checking if busy
    
    EEADR = address; // load address 
    EECON1bits.EEPGD = 0; // access EEPROM memory
    EECON1bits.CFGS  = 0; // avoid access configuration registers
    EECON1bits.RD    = 1; // read 
    return( EEDATA );
}

void shift_EEPROM(void){
    short int i;
    short int val = 0;
    
    for (i = 204; i >= 0; i--){ // shifting all values by 41 addresses in EEPROM 
        val = read_EEPROM(i);
        write_EEPROM(i+41, val);
    }
}

void store_EEPROM(void){
    short int i, j;
    
    for (i = 0; i < 8; i++){ // Entering EEPROM_mat, input summary 
        for (j = 0; j < 4; j++){
            write_EEPROM((4*i + j), EEPROM_mat[i][j]);}}
    
    for (i = 0; i < 4; i++){ // marked drawers
        write_EEPROM(32 + i, marked_drawers[i]);}
    
    write_EEPROM(36, run_time_seconds);
    write_EEPROM(37, run_time_minutes);
    
    for (i = 0; i < 3; i++){ // remaining pills in reservoir
        write_EEPROM(38 + i, reservoir_count[i]);
    }
}