/* 
 * File:   ACD.c
 * Author: Christopher Agia
 *
 * Created on February 26, 2018, 2:24 PM
 */

#include "configBits.h"
#include "lcd.h"
#include "ADC.h"
#include "add_functions.h"
#include "I2C.h"
#include "RTC.h"
#include "machine_code.h"
#include "EEPROM_storage.h"
#include "UART_PIC.h"

unsigned short readADC(char channel){
    /* Reads the analog input from the specified analog channel.
     *
     * Arguments: channel, the byte corresponding to the channel to read
     *
     * Returns: the 10-bit value corresponding to the voltage read from
     *          the specified channel
     */
    
    ADCON0 = (channel & 0x0F) << 2; // Select ADC channel (i.e. pin)
    ADON = 1; // Enable module
    ADCON0bits.GO = 1; // Initiate sampling
    while(ADCON0bits.GO_NOT_DONE){  continue;   } // Poll for acquisition completion
    return (ADRESH << 8) | ADRESL; // Return result as a 16-bit value
}
