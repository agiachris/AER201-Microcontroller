/* 
 * File:   EEPROM_storage.h
 * Author: Christopher Agia
 *
 * Created on February 27, 2018, 1:24 PM
 */

#ifndef EEPROM_STORAGE_H
#define	EEPROM_STORAGE_H

/********************************** Includes **********************************/
#include <xc.h>
#include <stdio.h>

/****************************** EEPROM Functions *******************************/
void write_EEPROM(unsigned char address, unsigned char data);
unsigned char read_EEPROM(unsigned char address);
void shift_EEPROM(void);
void store_EEPROM(void);

#endif	/* EEPROM_STORAGE_H */

