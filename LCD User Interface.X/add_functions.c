/* 
 * File:   add_functions.c
 * Author: Christopher Agia
 *
 * Created on February 27, 2018, 2:36 AM
 */

#include "configBits.h"
#include "lcd.h"
#include "add_functions.h"
#include "I2C.h"
#include "RTC.h"
#include "machine_code.h"
#include "EEPROM_storage.h"
#include "UART_PIC.h"

/****************************** Helper Functions ******************************/

short int compare_arrays(int *array1, int *array2){ // Returns 1 if arrays are the same, 0 otherwise
    int i = 0;
    for (i = 0; i < 4; i ++){
        if (array1[i] != array2[i]){
            return 0;
        }
    }
    return 1;
}

void array_transition(int mat[16][3]){
    short int i, j, k;
    int d_numbers[8] = {0};
    
    for (i = 0; i < 8; i++){ // initializing EEPROM_mat and d_numbers
        d_numbers[i] = 0;
        for (j = 0; j < 4; j++){
            EEPROM_mat[i][j] = 0;}}
    
    for (i = 0; i < 16; i++){ // recording the drawer numbers for used drawers 
        if ((mat[i][0]!=0)||(mat[i][1]!=0)||(mat[i][2]!=0)){
            for (k = 0; k < 8; k++){ 
                if (d_numbers[k] == 0){
                    d_numbers[k] = (i + 1);
                    break;}}}}
    
    for (i = 0; i < 8; i++){ // transition from mat[16][3] -> EEPROM_mat[8][4]
        if (d_numbers[i] != 0){
            EEPROM_mat[i][0] = d_numbers[i];
            for (j = 0; j < 3; j++){
                EEPROM_mat[i][j+1] = mat[(d_numbers[i]-1)][j];}}}
    
}

void receive_data_RX(void){
    uartTransmitBlocking(data, 1); // transmit instruction to Arduino 
    uartReceiveBlocking(1); // waiting for response (either 1 or 0)
    data_RX = arr_dataRX[0]; // store response into external variable data_RX
}

void operation_summary(void){
    __lcd_clear();
    __lcd_home();
    printf("Press * to");
    __lcd_newline();
    printf("view run data");
    
    const char keys[] = "123A456B789C*0#D";
    unsigned char temp, keypress;
    short int EEPROM_ind = 0;
    short int past_run_count = 0;
    short int i = 0;
    unsigned char EEPROM_print[16] = {0};
    
    while(past_run_count < 12){ // EEPROM polling while, displays past run data
        EEPROM_ind = 0;
        
        while(PORTBbits.RB1 == 0){  continue;   } 
        keypress = (PORTB & 0xF0) >> 4;
        while(PORTBbits.RB1 == 1){  continue;   }
        Nop();
        temp = keys[keypress];

        if (temp == '*'){
            __lcd_clear();
            __lcd_home();
            
            if (past_run_count == 0){ // read input summary
                printf("Drawer: %d", read_EEPROM(EEPROM_ind));
                __lcd_newline();
                printf("R:%d  F:%d  L:%d", read_EEPROM(EEPROM_ind+1), read_EEPROM(EEPROM_ind+2), read_EEPROM(EEPROM_ind+3));
            }

            else if (past_run_count == 1){ // read input summary
                EEPROM_ind += 4;
                printf("Drawer: %d", read_EEPROM(EEPROM_ind));
                __lcd_newline();
                printf("R:%d  F:%d  L:%d", read_EEPROM(EEPROM_ind+1), read_EEPROM(EEPROM_ind+2), read_EEPROM(EEPROM_ind+3));
            }

            else if (past_run_count == 2){ // read input summary
                EEPROM_ind += 8;
                printf("Drawer: %d", read_EEPROM(EEPROM_ind));
                __lcd_newline();
                printf("R:%d  F:%d  L:%d", read_EEPROM(EEPROM_ind+1), read_EEPROM(EEPROM_ind+2), read_EEPROM(EEPROM_ind+3));
            }

            else if (past_run_count == 3){ // read input summary
                EEPROM_ind += 12;
                printf("Drawer: %d", read_EEPROM(EEPROM_ind));
                __lcd_newline();
                printf("R:%d  F:%d  L:%d", read_EEPROM(EEPROM_ind+1), read_EEPROM(EEPROM_ind+2), read_EEPROM(EEPROM_ind+3));
            }

            else if (past_run_count == 4){ // read input summary
                EEPROM_ind += 16;
                printf("Drawer: %d", read_EEPROM(EEPROM_ind));
                __lcd_newline();
                printf("R:%d  F:%d  L:%d", read_EEPROM(EEPROM_ind+1), read_EEPROM(EEPROM_ind+2), read_EEPROM(EEPROM_ind+3));
            }

            else if (past_run_count == 5){ // read input summary
                EEPROM_ind += 20;
                printf("Drawer: %d", read_EEPROM(EEPROM_ind));
                __lcd_newline();
                printf("R:%d  F:%d  L:%d", read_EEPROM(EEPROM_ind+1), read_EEPROM(EEPROM_ind+2), read_EEPROM(EEPROM_ind+3));
            }

            else if (past_run_count == 6){ // read input summary
                EEPROM_ind += 24;
                printf("Drawer: %d", read_EEPROM(EEPROM_ind));
                __lcd_newline();
                printf("R:%d  F:%d  L:%d", read_EEPROM(EEPROM_ind+1), read_EEPROM(EEPROM_ind+2), read_EEPROM(EEPROM_ind+3));
            }

            else if (past_run_count == 7){ // read input summary
                EEPROM_ind += 28;
                printf("Drawer: %d", read_EEPROM(EEPROM_ind));
                __lcd_newline();
                printf("R:%d  F:%d  L:%d", read_EEPROM(EEPROM_ind+1), read_EEPROM(EEPROM_ind+2), read_EEPROM(EEPROM_ind+3));
            }

            else if (past_run_count == 8){ // read marked drawers
                EEPROM_ind += 32;
                for(i = 0; i < 4; i++){
                    EEPROM_print[i] = read_EEPROM(i + EEPROM_ind);}
                printf("Marked Drawers:");
                __lcd_newline();
                printf("D%d, D%d, D%d, D%d", EEPROM_print[0], EEPROM_print[1], EEPROM_print[2], EEPROM_print[3]);
            }

            else if (past_run_count == 9){ // read run time
                EEPROM_ind += 36;
                for(i = 0; i < 2; i++){
                    EEPROM_print[i] = read_EEPROM(i + EEPROM_ind);}
                printf("Run Time");
                __lcd_newline();
                printf("Min:Sec = %d:%d", EEPROM_print[1], EEPROM_print[0]);
            }

            else if (past_run_count == 10){ // read reservoir count for R, F, L
                EEPROM_ind += 38;
                for(i = 0; i < 3; i++){
                    EEPROM_print[i] = read_EEPROM(i + EEPROM_ind);}
                printf("Reservoir Count");
                __lcd_newline();
                printf("R %d  F %d  L %d", EEPROM_print[0], EEPROM_print[1], EEPROM_print[2]);
            }
     
            past_run_count += 1; // increment data select variable
        }
    }    
}

void set_total_init_time(void){
    short int i = 0;
 
    I2C_Master_Init(100000);
    I2C_Master_Start(); // Start condition
    I2C_Master_Write(0b11010000); // 7 bit RTC address + Write
    I2C_Master_Write(0x00); // Set memory pointer to seconds
    I2C_Master_Stop(); // Stop condition

    /* Read current time. */
    I2C_Master_Start(); // Start condition
    I2C_Master_Write(0b11010001); // 7 bit RTC address + Read
    for(i = 0; i < 6; i++){
        time[i] = I2C_Master_Read(ACK); // Read with ACK to continue reading
    }
    time[6] = I2C_Master_Read(NACK); // Final Read with NACK
    I2C_Master_Stop(); // Stop condition
    
    total_init_time = (((time[2]&0x0F))*3600 + (((time[1] & 0xF0) >> 4)*10 + (time[1]&0x0F))*60 + ((time[0] & 0xF0) >> 4)*10 + (time[0]&0x0F));  
}

void comp_time_difference(void){
    short int i = 0;
  
    I2C_Master_Init(100000);
    I2C_Master_Start(); // Start condition
    I2C_Master_Write(0b11010000); // 7 bit RTC address + Write
    I2C_Master_Write(0x00); // Set memory pointer to seconds
    I2C_Master_Stop(); // Stop condition

    /* Read current time. */
    I2C_Master_Start(); // Start condition
    I2C_Master_Write(0b11010001); // 7 bit RTC address + Read
    for(i = 0; i < 6; i++){
        time[i] = I2C_Master_Read(ACK); // Read with ACK to continue reading
    }
    time[6] = I2C_Master_Read(NACK); // Final Read with NACK
    I2C_Master_Stop(); // Stop condition
    
    total_final_time = (((time[2]&0x0F))*3600 + (((time[1] & 0xF0) >> 4)*10 + (time[1]&0x0F))*60 + ((time[0] & 0xF0) >> 4)*10 + (time[0]&0x0F));
    total_difference = total_final_time - total_init_time;
    
}

void comp_EEPROM_time(void){
    short int i = 0;
    while (total_difference - i*60 >= 0){
        i++;
    }
    i--;
    run_time_minutes = i; // to be stored in EEPROM
    run_time_seconds = total_difference - i*60; // to be stored in EEPROM
}