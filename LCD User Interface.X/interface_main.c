/*
 * File:   interface_main.c
 * Author: Christopher Agia
 *
 * Created on February 6, 2018
 */

/***** Includes *****/
#include <xc.h>
#include <stdio.h>
#include <string.h>
#include "configBits.h"
#include "lcd.h"
#include "add_functions.h"
#include "I2C.h"
#include "RTC.h"
#include "machine_code.h"
#include "EEPROM_storage.h"
#include "UART_PIC.h"


/***** Constants *****/
const char keys[] = "123A456B789C*0#D"; 

/***** Global Variables *****/
unsigned char man_cont_data[1]; // used for manual control of motors

/***** Main Interface Functions *****/
void main(void){
    
    // <editor-fold defaultstate="collapsed" desc="Machine Configuration">
    /********************************* PIN I/O ********************************/
    /* Write outputs to LATx, read inputs from PORTx. Here, all latches (LATx)
     * are being cleared (set low) to ensure a controlled start-up state. */  
    LATA = 0x00;
    LATB = 0x00; 
    LATC = 0x00;
    LATD = 0x00;
    LATE = 0x00;

    /* After the states of LATx are known, the data direction registers, TRISx
     * are configured. 0 --> output; 1 --> input. Default is  1. */
    TRISA = 0xFF; // All inputs (this is the default, but is explicated here for learning purposes)
    TRISB = 0xFF;
    TRISC = 0b10000000;
    TRISD = 0x00; // All output mode on port D for the LCD
    TRISE = 0x00;
    
    /************************** A/D Converter Module **************************/
    ADCON0 = 0x00;  // Disable ADC
    ADCON1 = 0b00001111; // Set all A/D ports to digital
    
    // Initialize character LCD.
    initLCD();
    // Initialize I2C
    I2C_Master_Init(100000); //Initialize I2C Master with 100 kHz clock
    // RTC Loading upon Re-upload
    initTime(0x00, 0x50, 0x2, 0x03, 0x11, 0x04, 0x18);
    loadRTC();
    
    // Initializing AURT Communication Protocol
    UART_Init(9600);
    
    di(); // Disable all masked interrupts (will be enabled when needed, ie. machine code)
    
    // </editor-fold>
    
    
    // Declaring all necessary variables
    short int i=0, j=0, k=0, row=0, temp_int, row_sum, amtR, amtF, amtL, tot_drawers, legit_diet, legit_pieces, drawer_used, mode_bool, timer_count, past_run_count, manual_unload_slide;
    unsigned char temp, keypress, letter;
    int mat[16][3] = {0};
    int diet[16][4] = {0};
    int row_check[4] = {0,0,0,0};
    // Dietary Cases (R=1, F=2, L=3)
    int a[4] = {1,0,0,0};
    int b[4] = {2,0,0,0};
    int c[4] = {3,0,0,0};
    int d[4] = {1,2,0,0};
    int e[4] = {1,3,0,0};
    int f[4] = {2,3,0,0};
    int g[4] = {1,1,2,0};
    int h[4] = {1,1,3,0};
    int x[4] = {1,2,2,0};
    int y[4] = {1,3,3,0};
    int z[4] = {1,2,3,0};
    int l[4] = {2,2,3,0};
    int m[4] = {2,3,3,0};
    int n[4] = {1,1,2,3};
    int o[4] = {1,2,2,3};
    int p[4] = {1,2,3,3};
    int q[4] = {1,3,3,3};
    int r[4] = {2,3,3,3};
    int s[4] = {0,0,0,0};
    unsigned char EEPROM_ind, copy_EEPROM_ind;
    unsigned char EEPROM_print[16] = {0};
  
    // Booting Screen
    __lcd_display_control(1, 0, 0);
    printf("Booting...");
    __delay_ms(2000);
    __lcd_newline();
    printf("Ready");
    __delay_ms(1000);
    __lcd_clear();
    __lcd_home();
    
    
    // Booting Complete
    // NEW SECTION
    // Introduction
    
    
    while(1){  
        __lcd_home();
        __lcd_clear();
        lcd_set_cursor(0, 0);
        printf("Hi! I am Cerberus v1. I am an autonomous");
        __delay_ms(1500);
        for (i = 0; i < 24; i++){
            lcd_shift_display(1, 0);
            __delay_ms(100);
        }       
        __lcd_clear();
        __lcd_home();
        printf("am an autonomous robot designed to");
        for (i = 0; i < 18; i++){
            lcd_shift_display(1, 0);
            __delay_ms(100);
        }
        __lcd_clear();
        __lcd_home();
        printf("obot designed to package daily dietary");
        for (i = 0; i < 22; i++){
            lcd_shift_display(1, 0);
            __delay_ms(100);
        }
        __lcd_clear();
        __lcd_home();
        printf("ge daily dietary foods for laboratory");
        for (i = 0; i <21 ; i++){
            lcd_shift_display(1, 0);
            __delay_ms(100);
        }
        __lcd_clear();
        __lcd_home();
        printf("s for laboratory rats!");
        for (i = 0; i <6 ; i++){
            lcd_shift_display(1, 0);
            __delay_ms(100);
        }
        __delay_ms(1500);
        __lcd_clear();
        __lcd_home();

        while(1){ // Polling for input, read again or continue to home screen    
            __lcd_clear();
            __lcd_home();
            printf("Home Screen: 1 ");
            __lcd_newline();
            printf("Read Again: 2 ");
            while(PORTBbits.RB1 == 0){  continue;   }
            keypress = (PORTB & 0xF0) >> 4;
            while(PORTBbits.RB1 == 1){  continue;   }
            Nop();  // Apply breakpoint here to prevent compiler optimizations
            temp = keys[keypress];
            temp_int = (temp - '0');
            if (temp_int == 1){ // Continue to home screen chosen
                lcd_set_cursor(15, 0);
                break;
            }
            else if (temp_int == 2){ // Read introduction again chosen
                lcd_set_cursor(15, 1);
                break;
            }
            else{ // neither 1 or 2 was pressed
                __lcd_clear();
                printf("Invalid Input");
                __delay_ms(1000);
                __lcd_clear();
                continue;
            }
        }
        printf("<"); // select shown for either 1 or 2
        __delay_ms(500);
        if (temp_int == 1){
            break;
        }
        if (temp_int == 2){
            continue;  
        }
    }
    
    __lcd_display_control(1, 0, 0);
    
    
    // Introduction Completed
    // NEW SECTION
    // Home Screen
    
    
    while(1){ // Encompasses the entire home screen
        __lcd_display_control(1,0,0);
        __lcd_clear();
        __lcd_home();
        
        // First function screen, user choice: Operate: 1, Standby: 2 
        while (1){ // Polling for input (1, 2, Invalid Other)
            __lcd_clear();
            __lcd_home();
            printf("Operate: 1");
            __lcd_newline();
            printf("Standby: 2");
          
            while(PORTBbits.RB1 == 0){  continue;   }
            keypress = (PORTB & 0xF0) >> 4;
            while(PORTBbits.RB1 == 1){  continue;   }
            Nop();  // Apply breakpoint here to prevent compiler optimizations
            temp = keys[keypress];
            mode_bool = (temp - '0');
            if (mode_bool == 1){
                lcd_set_cursor(15, 0);
                break;
            }
            else if (mode_bool == 2){
                lcd_set_cursor(15, 1);
                break;
            }
            else{
                __lcd_clear();
                printf("Invalid Input");
                __delay_ms(1000);
                __lcd_clear();
                continue;
            }
        }
        printf("<"); // show selection of either 1 or 2
        __delay_ms(300);
        __lcd_home();
        __lcd_clear();
        tot_drawers = 0; //ensure tot_drawers is 0
        for (i = 0; i < 16; i++){
            for (j = 0; j < 3; j++){
                mat[i][j] = 0;}} // ensure all indices of mat are 0
        
        printf("Press B for");
        __lcd_newline();
        printf("Mode Selection");
        __delay_ms(2000);
        __lcd_home();
        __lcd_clear();
        
        // Selection of mode is chosen
        if (mode_bool == 1){ //Operate (Input Diets, Clear Diets, Package Food, Drawer Count)
            
            while(1){ // Loop containing all Autonomous and Manual Control of machine
                
                __lcd_home();
                __lcd_clear();
                printf("Manual Control:");
                __lcd_newline();
                printf("Yes: 1   No: 2");
                while(PORTBbits.RB1 == 0){  continue;   }
                keypress = (PORTB & 0xF0) >> 4;
                while(PORTBbits.RB1 == 1){  continue;   }
                Nop(); 
                temp = keys[keypress];
                
                if (temp == '1'){ // Manual Control of Motors
                    
                    while (1){ // Encloses different manual functions
                        __lcd_home();
                        __lcd_clear();

                        printf("Height:1  Hook:2");
                        __lcd_newline();
                        printf("Unload Rest: 3");
                        
                        __delay_ms(250); // necessary delay to account for holding up button from height adjustment
                        while(PORTBbits.RB1 == 0){  continue;   }
                        keypress = (PORTB & 0xF0) >> 4;
                        while(PORTBbits.RB1 == 1){  continue;   }
                        Nop(); 
                        temp = keys[keypress];
                        
                        __lcd_home();
                        __lcd_clear();

                       
                        if ((temp - '0' > 0)&&(temp - '0' < 4)){
                              printf("Press D");
                            __lcd_newline();
                            printf("when done");
                            __delay_ms(1500);
                            __lcd_clear();
                            __lcd_home();
                        }
                        
                        if (temp == '1'){ // manual operation of height mechanism
                            printf("Raise: A");
                            __lcd_newline();
                            printf("Lower: B");
                            
                            man_cont_data[0] = 'J'; // wake stepper motors up from sleep mode 
                            uartTransmitBlocking(man_cont_data, 1);
                            while(1){
                                while(PORTBbits.RB1 == 0){  continue;   }
                                keypress = (PORTB & 0xF0) >> 4;
                                __delay_ms(20); // delay by 20 ms to ensure that data is sent to arduino 
                                Nop(); 
                                temp = keys[keypress];
                                
                                if (temp == 'A'){ // command arduino to raise motor
                                    man_cont_data[0] = 'H'; // manual stepper raise
                                    uartTransmitBlocking(man_cont_data, 1);
                                    continue;
                                }
                                
                                else if (temp == 'B'){ // command arduino to lower motor
                                    man_cont_data[0] = 'I'; // manual stepper lower
                                    uartTransmitBlocking(man_cont_data, 1);
                                    continue;
                                }
                                
                                else if (temp == 'D'){ // done adjusting height
                                    man_cont_data[0] = 'K'; // put stepper motors back to sleep
                                    uartTransmitBlocking(man_cont_data, 1);
                                    break;} 
                            }
                            continue;}
                        
                        else if (temp == '2'){ // manual operation of hook mechanism
                            printf("Retract: 1");
                            __lcd_newline();
                            printf("Protract: 2");
                            
                            while(1){
                                while(PORTBbits.RB1 == 0){  continue;   }
                                keypress = (PORTB & 0xF0) >> 4;
                                while(PORTBbits.RB1 == 1){  continue;   }
                                Nop(); 
                                temp = keys[keypress];
                                
                                if (temp == '1'){ // command arduino to retract hooks
                                    man_cont_data[0] = 'B'; // detach hooks
                                    uartTransmitBlocking(man_cont_data, 1);
                                    continue;
                                }
                                
                                else if (temp == '2'){ // command arduino to protract hooks
                                    man_cont_data[0] = 'C'; // attach hooks
                                    uartTransmitBlocking(man_cont_data, 1);
                                    continue;
                                }
                                
                                else if (temp == 'D'){ // done with hook mechanism
                                    break;} 
                            }
                            continue;}
                        
                        else if (temp == '3'){ // manual operation of servo slide
                            printf("Unload Food");
                            __lcd_newline();
                            printf("R: 1  F: 2  L: 3");
                            manual_unload_slide = 0;
                            
                            while(1){
                               
                                while(PORTBbits.RB1 == 0){  continue;   } // hold to unload pills
                                keypress = (PORTB & 0xF0) >> 4;
                                Nop(); 
                                temp = keys[keypress];
                                
                                if (temp == '1'){ // continue unloading R Pills in reservoir
                                    if (manual_unload_slide != 1){
                                         manual_unload_slide = 1;
                                        man_cont_data[0] = '8';
                                        uartTransmitBlocking(man_cont_data, 1);
                                        __delay_ms(300);
                                    }
                                    man_cont_data[0] = '1';
                                    uartTransmitBlocking(man_cont_data, 1);
                                    __delay_ms(190);
                                    continue;
                                }
                                
                                else if (temp == '2'){ // continue unloading F Pills in reservoir
                                    if (manual_unload_slide != 2){
                                         manual_unload_slide = 2;
                                        man_cont_data[0] = '9';
                                        uartTransmitBlocking(man_cont_data, 1);
                                        __delay_ms(300);
                                    }
                                    man_cont_data[0] = '2';
                                    uartTransmitBlocking(man_cont_data, 1);
                                    __delay_ms(200);
                                    continue;
                                }
                                
                                else if (temp == '3'){ // continue unloading L Pills in reservoir
                                    if (manual_unload_slide != 3){
                                         manual_unload_slide = 3;
                                        man_cont_data[0] = 'A';
                                        uartTransmitBlocking(man_cont_data, 1);
                                        __delay_ms(300);
                                    }
                                    man_cont_data[0] = '3';
                                    uartTransmitBlocking(man_cont_data, 1);  
                                    __delay_ms(170);
                                    continue;
                                }
                                
                                else if (temp == 'D'){ // done with slide mechanism
                                    break;} 
                            }
                            continue;}
                        
                        else if (temp == 'B'){ // break out of manual operation
                            break;
                        }
                        
                        else{ // invalid input 
                            __lcd_clear();
                            __lcd_home();
                            printf("Invalid Input");
                            __delay_ms(1500);
                            continue;
                        }
                                   
                    } // End of manual control while statements
                } // End of manual control case

                else if (temp == '2'){ // Autonomous Control of Motors

                    while(1){ // Operate Case While loop

                        __lcd_home();
                        __lcd_clear();
                        printf("Enter Row:      Drawers Used: %d", tot_drawers);
                        __lcd_newline();
                        printf("Pack Food: #    Clear Rows: C"); 
                         __delay_ms(500);
                        for (i = 0; i < 8; i++){ // shifts screen
                            __delay_ms(100);
                            lcd_shift_display(2, 0);
                         }
                        __delay_ms(1200);
                        for (i = 0; i < 8; i++){ // shifts screen
                            __delay_ms(100);
                            lcd_shift_display(2, 1);
                        }
                        lcd_set_cursor(11, 0);
                        __lcd_display_control(1, 1, 1);

                        // Polling for function choice in operate mode
                        while(PORTBbits.RB1 == 0){  continue;   }
                        keypress = (PORTB & 0xF0) >> 4;
                        while(PORTBbits.RB1 == 1){  continue;   }
                        Nop(); 
                        temp = keys[keypress];
                        temp_int = (temp - '0');
                        if ((temp_int <= 4)&&(temp_int >= 1)){ // only display row number chosen on op-mode function screen
                            putch(temp);}                      // don't display pressed letter or symbol
                        __delay_ms(300);
                        // User Has Chosen at this point, choice stored in temp or temp_int
                        __lcd_display_control(1, 0, 0);

                        // Machine code/packing begins if # is pressed
                        if (temp_int == -13){
                            __lcd_clear();
                            __lcd_home();
                            printf("Packaging");
                            __lcd_newline();
                            printf("in progress");
               
                            set_total_init_time(); // set initial time
                            package_food(mat); // Call to machine code
                            comp_time_difference(); // Compute the final time
                            comp_EEPROM_time(); // Compute absolute time to seconds and minutes
                            array_transition(mat); // create EEPROM_mat for efficient storage
                            shift_EEPROM(); // shift data in EEPROM, opening space for new data
                            store_EEPROM(); // run_time_minutes, run_time_seconds, EEPROM_mat[8][4], marked_drawers[4]
                            
                            __lcd_clear();
                            __lcd_home();
                            printf("Packaging");
                            __lcd_newline();
                            printf("Complete");
                            __delay_ms(3000);   
                            
                            __lcd_clear();
                            __lcd_home();
                            printf("View Summary:"); // User option to view run summary
                            __lcd_newline();
                            printf("Yes: 1   No: 2");
                            while(PORTBbits.RB1 == 0){  continue;   }
                            keypress = (PORTB & 0xF0) >> 4;
                            while(PORTBbits.RB1 == 1){  continue;   }
                            Nop(); 
                            temp = keys[keypress];
                            
                            // show run information upon user request
                            if (temp == '1'){ 
                                operation_summary(); // call to function that displays past run data (pulled from EEPROM)
                            }
                            
                            __lcd_clear();
                            __lcd_home();
                            printf("Reset:"); // User option to view run summary
                            __lcd_newline();
                            printf("Yes: 1   No: 2");
                            while(PORTBbits.RB1 == 0){  continue;   }
                            keypress = (PORTB & 0xF0) >> 4;
                            while(PORTBbits.RB1 == 1){  continue;   }
                            Nop(); 
                            temp = keys[keypress];
                            
                            // show run information upon user request
                            if (temp == '1'){ 
                                height_mech((-1)*curr_height, curr_height);  // adjust height back to start
                                curr_height = 0;
                            }
                            
                            // clearing all matrices containing current run data
                            for (i = 0; i < 16; i++){
                                for (j = 0; j < 4; j++){
                                    diet[i][j] = 0;}}
                                
                            for (i = 0; i < 16; i++){
                                for (j = 0; j < 3; j++){
                                    mat[i][j] = 0;}}
                            
                            tot_drawers = 0;
                            drawer_used = 0;
                            i = 0;
                            j = 0;
                            __lcd_clear();
                            __lcd_home();
                        } // Rows Packed

                        // Valid Row Chosen
                        else if ((temp_int >= 1)&&(temp_int <= 4)&&(tot_drawers < 8)){
                            __lcd_clear();
                            __lcd_home();

                            printf("Row %d...        Press *", temp_int);
                            __lcd_newline();
                            printf("                to skip drawer"); 
                            __delay_ms(700);
                            for (i = 0; i < 4 ; i++){
                            lcd_shift_display(4, 0);
                            __delay_ms(150);
                            }
                            __delay_ms(1500);
                            __lcd_clear();
                            __lcd_home();

                            // row variable used for mat matrix
                            row = (temp_int-1)*4;

                            for (i = 0; i < 4; i++){ //Large i for loop, for each drawer in row temp_int
                                legit_diet = 0; // reset variables 
                                legit_pieces = 0;
                                drawer_used = 0;

                                for (j = 0; j < 3; j++){ // if drawer has been previously used, avoid double-counting drawer entry
                                    if (mat[i + row][j] != 0){
                                        tot_drawers -= 1;
                                        break;}} // break out of loop if any drawer input is not zero

                                while (legit_diet == 0){ //polling for diet input, checking for correct/incorrect diet type
                                    j = 0;
                                    k = 0;
                                    __lcd_clear();
                                    __lcd_home();
                                    printf("D%d Diet:", i + 1 + row); //Drawer Number
                                    lcd_set_cursor(10, 0);
                                    __lcd_display_control(1, 1, 0);
                                    while (j < 4){ //large j while loop, polling for the diet combination of each drawer
                                        while (1){ //Poll for input, conditions for (0,1,2,3)=(Nothing,R,F,L)
                                            while(PORTBbits.RB1 == 0){  continue;   }
                                            keypress = (PORTB & 0xF0) >> 4;
                                            while(PORTBbits.RB1 == 1){  continue;   }
                                            Nop(); 
                                            temp = keys[keypress];
                                            if ((temp - '0') == 1){
                                                letter = 'R';
                                                break;}
                                            else if ((temp - '0') == 2){
                                                letter = 'F';
                                                break;}
                                            else if ((temp - '0') == 3){
                                                letter = 'L';
                                                break;}
                                            else if (temp == '*'){
                                                letter = '*';
                                                break;}   
                                        }   

                                        // If not skip drawer, put letter on screen, store value in matrix
                                        if (letter != '*'){
                                            putch(letter); //Put entered value onto R, F, or L  
                                            diet[row + i][j] = (temp - '0');}

                                        // Skip Drawer if * pressed
                                        if (letter == '*'){
                                            while (k < 4 - j){
                                                diet[row + i][k + j] = 0;
                                                putch(' ');  
                                                k++;}
                                            break;}
                                        j++;
                                    } //end of j while loop

                                    __lcd_display_control(1, 0, 0);

                                    // giving values to array row_check, used to compare to acceptable diet codes
                                    for (k = 0; k < 4; k++){
                                        row_check[k] = diet[i + row][k];
                                    }

                                    // Checking if diet type entered allowed
                                    // if valid diet has been entered
                                    legit_diet += compare_arrays(row_check, a);
                                    legit_diet += compare_arrays(row_check, b);
                                    legit_diet += compare_arrays(row_check, c);
                                    legit_diet += compare_arrays(row_check, d);
                                    legit_diet += compare_arrays(row_check, e);
                                    legit_diet += compare_arrays(row_check, f);
                                    legit_diet += compare_arrays(row_check, g);
                                    legit_diet += compare_arrays(row_check, h);
                                    legit_diet += compare_arrays(row_check, x);
                                    legit_diet += compare_arrays(row_check, y);
                                    legit_diet += compare_arrays(row_check, z);
                                    legit_diet += compare_arrays(row_check, l);
                                    legit_diet += compare_arrays(row_check, m);
                                    legit_diet += compare_arrays(row_check, n);
                                    legit_diet += compare_arrays(row_check, o);
                                    legit_diet += compare_arrays(row_check, p);
                                    legit_diet += compare_arrays(row_check, q);
                                    legit_diet += compare_arrays(row_check, r);
                                    legit_diet += compare_arrays(row_check, s);

                                    // if invalid diet has been entered
                                    if (legit_diet == 0){
                                        __lcd_home();
                                        __lcd_clear();
                                        printf("Invalid Diet");
                                        __delay_ms(1000);}
                                } // end of legit_diet while loop

                                while (legit_pieces == 0){ //polling for pieces input, checking for correct/incorrect pieces
                                    // IF incorrect, pull values from diet matrix to re-print of the screen
                                    j = 0;
                                    k = 0;
                                    row_sum = 0; 
                                    amtR = 0;
                                    amtF = 0;
                                    amtL = 0;
                                    __lcd_home();
                                    __lcd_clear();
                                    printf("D%d Diet:", i + 1 + row); //Drawer Number
                                    lcd_set_cursor(10, 0);
                                    for (j = 0; j < 4; j++){ // Case where invalid food piece combination is entered, previous screen must be reloaded
                                        if (diet[i+ row][j] == 0){
                                            putch(' ');}
                                        else if (diet[i+ row][j] == 1){
                                            putch('R');}

                                        else if (diet[i+ row][j] == 2){
                                            putch('F');}
                                        else if (diet[i + row][j] == 3){
                                            putch('L');}
                                    }
                                    __lcd_newline();
                                    printf("Pieces: ");
                                    lcd_set_cursor(10, 1);
                                    __lcd_display_control(1, 1, 0);
                                    for (j = 0; j < 4; j++){ // j for loop, polling for each number of piece in each diet code

                                        if (diet[i + row][j] == 0){ // if diet matrix has no letter for diet code, no need to input number of pieces
                                            while (k < 4 - j){
                                                putch(' ');
                                                k++;}
                                                __delay_ms(250);
                                            break;}
                                        
                                        while(1){
                                            while(PORTBbits.RB1 == 0){  continue;   }
                                            keypress = (PORTB & 0xF0) >> 4;
                                            while(PORTBbits.RB1 == 1){  continue;   }
                                            Nop(); 
                                            temp = keys[keypress];
                                            if ((temp - '0' >= 1)&&(temp - '0' <= 3)){ // pieces entered must be between 1-3
                                                break;}
                                        }
                                        putch(temp);
                                        __delay_ms(100);

                                        // adding inputted number of each piece per diet type
                                        if (diet[i + row][j] == 1){
                                            amtR += temp - '0';} 
                                        else if (diet[i + row][j] == 2){
                                            amtF += temp - '0';}
                                        else if (diet[i + row][j] == 3){
                                            amtL += temp - '0';}
                                        else {continue;}
                                    }
                                    __lcd_display_control(1, 0, 0);

                                    // Checking piece conditions
                                    row_sum = amtR + amtF + amtL; // total number of pieces inputted for drawer
                                    if ((row_sum > 4)||(amtR > 2)||(amtF > 2)||(amtL > 3)){
                                        __lcd_home();
                                        __lcd_clear();
                                        printf("Invalid Inputs");
                                        __delay_ms(1000);}
                                    else{ // if conditions are met, enter the number of each piece into matrix 
                                        mat[i + row][0] = amtR;
                                        mat[i + row][1] = amtF;
                                        mat[i + row][2] = amtL;
                                        legit_pieces = 1;}
                                } // end of legit_pieces for loop 

                                // Drawer is used if diet code, pieces for diet are valid, and drawer not empty
                                if ((legit_diet == 1)&&(legit_pieces == 1)&&(row_sum != 0)){
                                    drawer_used = 1;}

                                // Add to total drawers (will add 0 if not drawer not used)
                                tot_drawers += drawer_used;

                                // Check to see if max drawers have been used
                                // if max drawers used, break out of row entry
                                if (tot_drawers == 8){
                                    __lcd_clear();
                                    __lcd_home();
                                    printf("Maximum Drawers");
                                    __lcd_newline();
                                    printf("Used");
                                    __delay_ms(1500);
                                    break;}
                            } // end of row i while loop

                            i = 0;
                            row = 0;
                            continue;
                        } //end of row case statement 

                        // Clear all entries
                        else if (temp == 'C'){
                             for (i = 0; i < 16; i++){
                                for (j = 0; j < 4; j++){
                                    diet[i][j] = 0;}}
                             for (i = 0; i < 16; i++){
                                for (j = 0; j < 3; j++){
                                    mat[i][j] = 0;}}
                        
                            tot_drawers = 0;
                            drawer_used = 0;
                            i = 0;
                            j = 0;
                            __lcd_clear();
                            __lcd_home();
                            printf("Entries cleared!");
                            __delay_ms(1500);
                            continue;
                        } // all entries cleared

                        // Pressing B takes back to home page
                        else if (temp == 'B'){
                            break;}

                         // Checking to see if 8/16 drawers are in use
                        else if (tot_drawers == 8){
                            __lcd_clear();
                            __lcd_home();
                            printf("Maximum drawers");
                            __lcd_newline();
                            printf("used");
                            __delay_ms(1500);
                        }

                        // Invalid Input
                        else{ 
                            __lcd_clear();
                            __lcd_home();
                            printf("Invalid Input");
                            __delay_ms(1500);
                            continue;
                        }
                        
                    } // End of Autonomous Operate Case While 
                } // End of Autonomous Operation Case 
                
                else if (temp == 'B'){ // Break to Standby Mode
                    break;}
                
            } // End of Manual Operation Option While
        } // End of Operate Case
        
        
        if (mode_bool == 2){ //Standby (Date/Time, Past Run Data)   
        
            while(1){ // Standby Case While loop
                __lcd_clear();
                __lcd_home();
                
                printf("Past Run Logs: C");
                __lcd_newline();
                printf("Date/Time: D");
                
                // Polling for function choice in operate mode
                while(PORTBbits.RB1 == 0){  continue;   }
                keypress = (PORTB & 0xF0) >> 4;
                while(PORTBbits.RB1 == 1){  continue;   }
                Nop();
                temp = keys[keypress];
                
                __delay_ms(300);
                // User Has Chosen at this point, choice stored in temp or temp_int
                __lcd_display_control(1, 0, 0);
                
                // Interface that will allow the access of past 6 run data
                if (temp == 'C'){
                    
                    while(1){ // Past Run Data while Loop
                        EEPROM_ind = 0; // reset EEPROM index 
                        past_run_count = 0; 
                        
                        __lcd_clear();
                        __lcd_home();
                        printf("Press B for");
                        __lcd_newline();
                        printf("Standby Screen");
                        __delay_ms(2000);
                        
                        __lcd_clear();
                        __lcd_home();
                        printf("Run Selection"); 
                        __lcd_newline();
                        printf("Runs ago (1-6): "); // user can choose to look at any one of past six runs
                        lcd_set_cursor(15, 1);
                        __lcd_display_control(1, 1, 1);

                        while(PORTBbits.RB1 == 0){  continue;   } // poll for past run choice
                        keypress = (PORTB & 0xF0) >> 4;
                        while(PORTBbits.RB1 == 1){  continue;   }
                        Nop();
                        temp = keys[keypress];
                        temp_int = temp - '0';
                        if ((temp_int >= 1) && temp_int <= 6){
                            putch(temp);
                        }
                        __delay_ms(300);
                        __lcd_display_control(1, 0, 0);
                       
                        if (temp == 'B'){ // breaks back to standby mode screen
                            __delay_ms(250);
                            break;
                        }
                        
                        __lcd_clear();
                        __lcd_home();
                        printf("Press B for");
                        __lcd_newline();
                        printf("Run Selection");
                        __delay_ms(2000);
                        
                        EEPROM_ind = (41 * (temp_int-1)); // index to beginning of information for chosen run
                        copy_EEPROM_ind = EEPROM_ind; // used to store original location (will not be changed)
                        __lcd_clear();
                        __lcd_home();
                        printf("Press * to");
                        __lcd_newline();
                        printf("view run data");
                        
                        while(past_run_count < 12){ // EEPROM polling while, displays past run data
                            EEPROM_ind = copy_EEPROM_ind;
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
                                    printf("R %d F %d  L %d", EEPROM_print[0],EEPROM_print[1], EEPROM_print[2]);
                                }
                                past_run_count += 1; // increment data select variable
                            }
                  
                            else if (temp == 'B'){ // break out to run selection
                                break;
                            }
                            
                            else{ // invalid input
                                __lcd_clear();
                                __lcd_home();
                                printf("Invalid Input");
                                __delay_ms(1000);
                                continue;
                            }    
                        } // end of EEPROM polling while
                        
                    } // end of past run data while loop
                    continue;
                } // end of past run interface
                
                //If D is pressed, date/time will be displayed for 10 seconds
                else if (temp == 'D'){ 
                    __lcd_clear();
                    __lcd_home();
                    timer_count = 0;
                    while(timer_count <= 10){
                        /* Reset RTC memory pointer. */
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

                        /* Print received data to LCD. */
                        __lcd_home();
                        printf("Date %02x/%02x/%02x", time[6],time[5],time[4]); // Print date in YY/MM/DD
                        __lcd_newline();
                        printf("Time %02x:%02x:%02x", time[2],time[1],time[0]); // HH:MM:SS
                        __delay_ms(1000);
                        timer_count++;
                    }
                }

              // Pressing B takes back to home page
                else if (temp == 'B'){
                    break;}

                else{ 
                        __lcd_clear();
                        __lcd_home();
                        printf("Invalid Input");
                        __delay_ms(1500);
                        continue;
                }
                
            } // End of Standby Case While
        } // End of Standby Case 
        
    } // End of Home Screen
    
} // End of main

