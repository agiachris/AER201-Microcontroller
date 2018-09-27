/* 
 * File:   RTC.c
 * Author: Christopher Agia
 *
 * Created on February 26, 2018, 2:23 PM
 */

#include "configBits.h"
#include "lcd.h"
#include "add_functions.h"
#include "I2C.h"
#include "RTC.h"
#include "machine_code.h"
#include "EEPROM_storage.h"
#include "UART_PIC.h"

//sec = 42, min = 09, hour = 07, weekday = Sunday, weekday(#) = the 5th, month = February, year = 2017
void initTime(unsigned char s, unsigned char m, unsigned char h,unsigned char w,
              unsigned char DD, unsigned char MM, unsigned char YY){
    time[0] = s;
    time[1] = m;
    time[2] = h;
    time[3] = w;
    time[4] = DD;
    time[5] = MM;
    time[6] = YY;
}

void loadRTC(){
    // Load the RTC with the date and time
    I2C_Master_Init(10000); //Initialize I2C Master with 100KHz clock
    
    di(); // Disable all interrupts
    
    I2C_Master_Start(); //Start condition
    I2C_Master_Write(0b11010000); //7 bit RTC address + Write (0 LSB)
    I2C_Master_Write(0x00); //Set memory pointer to seconds
    for(int i = 0; i<7; i++){
        I2C_Master_Write(time[i]);
    }
    I2C_Master_Stop(); //Stop condition
    ei();
}

void getRTC(void){
    // Sets members in time array by reading from RTC module
    
    //Reset RTC memory pointer 
    I2C_Master_Init(100000); 
    I2C_Master_Start(); //Start condition
    I2C_Master_Write(0b11010000); //7 bit RTC slave address + Write (0 LSB)
    I2C_Master_Write(0x00); //Set memory pointer to seconds
    I2C_Master_Stop(); //Stop condition

    //Read Current Time
    I2C_Master_Start();
    I2C_Master_Write(0b11010001); //7 bit RTC address + Read (1 LSB)
    for(int i = 0; i < 6; i++){
        time[i] = I2C_Master_Read(1);
    }
    time[6] = I2C_Master_Read(0);       //Final Read without ack
    I2C_Master_Stop();
}

void printRTC(void){
    getRTC();
    char month[] = "MMM";
    int INT_time[7];
    for(int i = 0; i < 7; i++){
        INT_time[i] = __bcd_to_num(time[i]);
    }
    int RTC_month = INT_time[5];
    
    switch(RTC_month){
        case 1 :
            strcpy(month, "JAN");
            break;
        case 2:
            strcpy(month, "FEB");
            break;
        case 3:
            strcpy(month, "MAR");
            break;
        case 4:
            strcpy(month, "APR");
            break;
        case 5:
            strcpy(month, "MAY");
            break;
        case 6:
            strcpy(month, "JUN");
            break;    
        case 7:
            strcpy(month, "JUL");
            break;  
        case 8:
            strcpy(month, "AUG");
            break;    
        case 9:
            strcpy(month, "SEP");
            break;    
        case 10:
            strcpy(month, "OCT");
            break;    
        case 11:
            strcpy(month, "NOV");
            break;    
        case 12:
            strcpy(month, "DEC");
            break;
    }
    __lcd_home();
    printf("%02d%s | %02d:%02d:%02d", INT_time[4],month,INT_time[2],INT_time[1],INT_time[0]);
}
