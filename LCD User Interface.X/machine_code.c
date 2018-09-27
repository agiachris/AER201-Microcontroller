/* 
 * File:   machine_code.c
 * Author: Christopher Agia
 *
 * Created on Feb 23, 2018, 5:10 PM
 */

#include "configBits.h"
#include "lcd.h"
#include "add_functions.h"
#include "I2C.h"
#include "RTC.h"
#include "machine_code.h"
#include "EEPROM_storage.h"
#include "UART_PIC.h"

/****************************** Psuedo Code ***********************************/
/*
     * MACHINE PSEUDO CODE: Includes the machine pseudo code. Description of each function is included, as well as a process which
     *                      describes how the commands will be carried out. The full code for each function is provided in the next section.
     * 
     * Process:
     * Iterate over double while loop (i, j). The first while (i) loop will involve 
     * adjusting the cabinet height and opening/closing of the drawers (vertical motors), the second while (j) loop
     * will iterate over the columns (horizontal motors, loading motors, color sensor, pressure sensor).
     * All motors stop operating if external emergency button is pressed.  
     * At the end, remaining pills are fed to respective reservoirs, and the initial positions are reset (height, servo slide). 
     * 
     * Functions: 
     * 
     * void package_food(int **mat){  
     * Description: logical function calling structure, no motors directly commanded through this function
     * Process: Will be the main machine code function, all other machine code
     *          functions will be called from this main function. Thus, all calls to
     *          functions that control mechanisms must be called within a double loop.
     * }
     * 
     * void height_mech(int y){
     * Description: Will lift the cabinet up by the number of rows specified by y
     * 
     * Process: involves retracting/protracting of the drawer opening/closing mechanism to allow for
     *          adjustment of the drawer height. Thus, height adjustment will be a four step process:
     *          1) Slightly drop height cabinet
     *          2) Retract the drawer mechanism
     *          3) Elevate to height above drawer mechanism
     *          4) Protract drawer mechanism
     *          5) Elevate to height at row n+1
     * }
     * 
     * short int sensor_row_check(int **mat, short int row_ind, short int count){
     * Description: return 1, to proceed with packaging row (a drawer in row n will be packed)
     *              return 0, to skip to the next cabinet row (no drawers are to be packed)
     *   
     * Process: if called, check IR sensors and identify which drawer in row n is marked.
     *          iterate over four drawers, checking if any drawer is empty, neglecting
     *          if drawers is identified as marked. 
     * }
     * 
     * void hook_mech(int z){
     * Description: if z = 1, close drawers
     *              if z = 0, open drawers
     * }
     * 
     * void servo_slide(int x){
     * Description: if x = 0: move slide to column 1
     *              if x = 1: move slide to column 2
     *              if x = 2: move slide to column 3
     *              if x = 3: move slide to column 4
     *              if x = 4: move slide to R reservoir
     *              if x = 5: move slide to F reservoir
     *              if x = 6: move slide to L reservoir
     * }
     * 
     * void load_mech(short int row, short int j){
     * Description: Proceed if row_sum (for given drawer) == to count of pressure sensor (pills correctly loaded)
     *              Call handler function (shaker(); )if row_sum (for given drawer) != to count of pressure sensor (pills incorrectly loaded)
     * Process: iterate over the pill types of given drawer changed_mat, commanding 
     *          the servo loaders of each reservoir move back and forth according
     *          to the number of of pills specified for that pill type. Check the count
     *          of the pressure sensor to ensure the correct amount of pills were loaded. 
     * }
     * 
     * void unload_reservoir(unsigned char pill){
     * Description: This function will take in a character 'pill', referring to pills
     *              R, F, or L. The function then proceeds to command the servo motors to
     *              unload the remaining pills into their respective reservoirs, keeping track 
     *              how many have been placed in the reservoir. In the case of a funnel block, 
     *              the function will call the shaker motor twice before concluding that the funnel is
     *              empty. 
     * 
     * Process: 1) command servo motor to drop pill
     *          2) poll pressure sensor to ensure pill has dropped
     *          3) if poll drop, increment reservoir count matrix, repeat steps 1-2
     *          4) if no pill dropped, command shaker motor, repeat steps 1-2 
     *          5) if no pill dropped, conclude the funnel is empty, finish
     * } 
     * 
     * void initialize(int **mat){
     * Description: Will also initialize changed_mat matrix and other global matrices
     *              that will be used to store data in EEPROM memory, for example marked drawers.
     *              This function will also configure PIC pins.
     * }
     * 
     * void finalize(short int curr_height, int **mat){
     * Description: Unload the remaining pills to their respective reservoirs, 
     *              adjust height back to starting position for next run, 
     *              reset servo slide to initial position. Also clears matrix changed_mat for next run.
     * }
     */


/****************************** Machine Code **********************************/
void package_food(int mat[16][3]){
    short int i = 0, j = 0, row_ind = 0, count = 0, row_used = 0;
    initialize(mat); // changed_mat initialize outside of function (global variable)
    
    for (i = 0; i < 4; i++){ // Iterating over rows
        count = 0;
        row_ind = 4*i;
      
        row_used = sensor_row_check(row_ind, count); // Checking if nothing in row n needs to be packed, if so go to next row
        if (row_used == 0){ // check if anything is inputted in a given row
            continue;
        } 
        
        else{ // else, adjust the height, and set the current height
            height_mech((i+1) - curr_height, curr_height);
            curr_height = (i+1);
            count++; // increment count, enabling the IR sensor functionality of sensor_row_check
            row_used = sensor_row_check(row_ind, count);
            if (row_used == 0){ // checking if inputted values are erased by marked drawer
                continue;
            }
        } 

        hook_mech(0); // open drawers
        __delay_ms(2500);
        for (j = 0; j < 4; j++){ // iterating over columns, loading pills and moving servo slide appropriately 
            if ((changed_mat[row_ind + j][0] > 0)||(changed_mat[row_ind + j][1] > 0)||(changed_mat[row_ind + j][2] > 0)){ // check is drawer is used
                servo_slide(j); // adjust servo slide to appropriate column
                load_mech(row_ind, j); // load pills into drawer
            }
        }
        __delay_ms(1000);
        hook_mech(1); // close drawers   
        
    } // end of row for loop   
    
    finalize(curr_height, mat);
} // Food packaging complete


short int sensor_row_check(short int row_ind, short int count){
    /*
     * Description: return 1, to proceed with packaging row (a drawer in row n will be packed)
     *              return 0, to skip to the next cabinet row (no drawers are to be packed)
     *   
     * Process: if called, check IR sensors and identify which drawer in row n is marked.
     *          iterate over four drawers, checking if any drawer is empty, neglecting
     *          if drawers is identified as marked. 
     */ 
   
    short int i = 0, j = 0, drawer_bool = 0, pieces = 0;
    int marked_row[4] = {0};

    while (i < 4){
        marked_row[i] = 0;
        i++;}
    i = 0;
    
    if (count == 0){ // regular user input row check required
        for (i = 0; i < 4; i++){
            pieces = 0;
            for (j = 0; j < 3; j++){
                pieces += changed_mat[i + row_ind][j];}
            if (pieces != 0){
                drawer_bool = 1;
                break;}
        }
        return drawer_bool;
        
    }
    
    else{ // Sensor row check required, modify user inputs if drawer is marked
        // checking sensor values, record which drawer in row is marked
        
        // Checking IR sensor 1
        data[0] = 'L';
        receive_data_RX();
        marked_row[0] = data_RX;
        // Checking IR sensor 2
        data[0] = 'M';
        receive_data_RX();
        marked_row[1] = data_RX;
        // Checking IR sensor 3
        data[0] = 'N';
        receive_data_RX();
        marked_row[2] = data_RX;
        // Checking IR sensor 4
        data[0] = 'O';
        receive_data_RX();
        marked_row[3] = data_RX;
        
        // checking to see if a drawer in row is used considering the markings on the drawer
        for (i = 0; i < 4; i++){
            pieces = 0;
            for (j = 0; j < 3; j++){
                pieces += changed_mat[i + row_ind][j];}
            if ((pieces != 0)&&(marked_row[i] != 1)){
                drawer_bool = 1;
                continue;}
            else if ((pieces != 0)&&(marked_row[i] == 1)){
                for (j = 0; j < 3; j++){
                    changed_mat[i + row_ind][j] = 0;}}   
        }
        
        for (i = 0; i < 4; i++){ // tracks the drawer number of marked drawers to be stored in EEPROM
            if (marked_drawers[i] == 0){
                for (j = 0; j < 4; j++){
                    if (marked_row[j] == 1){
                        marked_drawers[i] = (row_ind + j + 1);
                        marked_row[j] = 0;
                        break;
                    }}}}
        
        return drawer_bool; // if a drawer is used and unmarked, 1 will be returned, else 0 returns (skip row)
    }   
}

 void servo_slide(short int x){
     
    if (x == 0){
        data[0] = '4';
        uartTransmitBlocking(data, 1); // move slide to column 1
    }
    else if (x == 1){
        data[0] = '5';
        uartTransmitBlocking(data, 1); // move slide to column 2
    }
    else if (x == 2){
        data[0] = '6';
        uartTransmitBlocking(data, 1); // move slide to column 3
    }
    else if (x == 3){
        data[0] = '7';
        uartTransmitBlocking(data, 1); // move slide to column 4
    }
    else if (x == 4){
        data[0] = '8';
        uartTransmitBlocking(data, 1); // move slide to column to reservoir R
    }
    else if (x == 5){
        data[0] = '9';
        uartTransmitBlocking(data, 1); // move slide to column to reservoir F
    }
    else if (x == 6){
        data[0] = 'A';
        uartTransmitBlocking(data, 1); // move slide to column to reservoir L
    }
    __delay_ms(1500); // time needed to turn servo slide 
}
 
 void height_mech(short int dy, short int curr_height){
    /* Description: Will lift the cabinet up by the number of rows specified by dy
     * 
     * Process: involves retracting/protracting of the drawer opening/closing mechanism to allow for
     *          adjustment of the drawer height. Thus, height adjustment will be a four step process:
     *          1) Slightly drop height cabinet
     *          2) Retract the drawer mechanism
     *          3) Elevate to height above drawer mechanism
     *          4) Protract drawer mechanism
     *          5) Elevate to height at row n+1
     */
    short int dy_cpy = dy;
    short int lower_row_count = 0;
    /*
     * Cases:
     * 1) starting position, moving to next row
     * 2) not starting position, moving to next row
     * 3) moving back to starting position
     */
    
    if (dy > 0){ // a raise of some sort is to be executed
        if ((curr_height == 0)&&(dy == 1)){ // from starting position to first row
            hook_mech(1); // protract hooks
            data[0] = 'F';
            uartTransmitBlocking(data, 1); // small raise to attach to hooks 
            __delay_ms(2400);
            __delay_ms(2400);
            __delay_ms(1000);
        }
        
        else if ((curr_height == 0)&&(dy > 1)){ // from starting position to rows 2, 3, or 4
            while ((dy_cpy-1) != 0){
                data[0] = 'D';
                uartTransmitBlocking(data, 1); // large row size lift
                dy_cpy--; // decrement step
                __delay_ms(2400); // necessary time to reach next row 
                __delay_ms(2400);
                __delay_ms(2400);
            }
            hook_mech(1); // protract hooks
            data[0] = 'F';
            uartTransmitBlocking(data, 1); // small raise to attach to hooks
            __delay_ms(2400);
            __delay_ms(2400)
            __delay_ms(1000);
        }
        
        else if ((dy > 0)&&(curr_height != 0)){ // from attached position to rows 2, 3, or 4
            data[0] = 'G';
            uartTransmitBlocking(data, 1); // small height decrease, detaching from hooks
            __delay_ms(2400);
            __delay_ms(2400);
            __delay_ms(1000);
            hook_mech(0); // retract hooks, allowing for height change
            while (dy_cpy != 0){
                data[0] = 'D';
                uartTransmitBlocking(data, 1); // large row size lift
                dy_cpy--; // decrement counter
                __delay_ms(2400); // necessary time to raise to next row
                __delay_ms(2400);
                __delay_ms(2400);
            }
            hook_mech(1); // protract hooks
            data[0] = 'F';
            uartTransmitBlocking(data, 1); // small raise to attach to hooks
            __delay_ms(2400);
            __delay_ms(2400);
            __delay_ms(1000);

        }
    }
    
    else if (dy < 0){ // called by finalize, go back to starting position
        if (curr_height != 0){
            
            while (dy_cpy + 1 != 0){
                lower_row_count += 1;
                dy_cpy++;
            }
            
            if (lower_row_count == 1){ data[0] = 'E';} // lower by one row 
            else if (lower_row_count == 2){ data[0] = 'Q';} // lower by two rows
            else if (lower_row_count == 3){ data[0] = 'R';} // lower by three rows
            
            uartTransmitBlocking(data, 1); // decreasing in the height of rows
        }}    
 }
 
void hook_mech(int z){
    // Description: if z = 0, open drawers
    //              if z = 1, close drawers            
 
    if (z == 0){
        data[0] = 'B';
        uartTransmitBlocking(data, 1); // retract hook mechanisms
    }
    else if (z == 1){
        data[0] = 'C';
        uartTransmitBlocking(data, 1); // protract hook mechanism
    }
}

void load_mech(short int row, short int j){
    /*
     * Description: Proceed if pieces (for given drawer) == to count of pressure sensor (pills correctly loaded)
     *              Call handler function if pieces (for given drawer) != to count of pressure sensor (pills incorrectly loaded)
     *
     * Process: iterate over the pill types of given drawer changed_mat, commanding 
     *          the servo loaders of each reservoir move back and forth according
     *          to the number of of pills specified for that pill type. Check the count
     *          of the pressure sensor to ensure the correct amount of pills were loaded. 
     */
    
    short int i = 0, pieces = 0;
    
    while(i < 3){ // for three pill types
        pieces = 0; p_count = 0;

        if (i == 0){ // for round pill 
            pieces = changed_mat[row + j][i];
            if (pieces != 0){
                while(p_count < pieces){
                    data[0] = '1';
                    uartTransmitBlocking(data, 1); // load long pill
                    data[0] = 'P'; // prepare for polling
                    receive_data_RX(); // poll for response from Arduino
                    if (data_RX == 1){p_count++;} // pill must have dropped, increment p_count
                    __delay_ms(100);
                }}}

        else if (i == 1){ // for flat pill
            pieces = changed_mat[row + j][i];
            if (pieces != 0){
                while(p_count < pieces){ 
                    data[0] = '2';
                    uartTransmitBlocking(data, 1); // load long pill
                    data[0] = 'P'; // prepare for polling
                    receive_data_RX(); // poll for response from Arduino
                    if (data_RX == 1){p_count++;} // pill must have dropped, increment p_count
                    __delay_ms(100);
                }}}

        else if (i == 2){ // for long pill
            pieces = changed_mat[row + j][i];
            if (pieces != 0){
                while(p_count < pieces){ 
                    data[0] = '3';
                    uartTransmitBlocking(data, 1); // load long pill
                    data[0] = 'P'; // prepare for polling
                    receive_data_RX(); // poll for response from Arduino
                    if (data_RX == 1){p_count++;} // pill must have dropped, increment p_count
                    __delay_ms(100);
                }}}
    i++;    
    }
    p_count = 0;
    __delay_ms(500);
}

void unload_reservoir(unsigned char pill){
    /*
     * Description: This function will take in a character 'pill', referring to pills
     *              R, F, or L. The function then proceeds to command the servo motors to
     *              unload the remaining pills into their respective reservoirs, keeping track 
     *              how many have been placed in the reservoir. In the case of a funnel block, 
     *              the function will call the shaker motor twice before concluding that the funnel is
     *              empty. 
     * 
     * Process: 1) command servo motor to drop pill
     *          2) poll pressure sensor to ensure pill has dropped
     *          3) if poll drop, increment reservoir count matrix, repeat steps 1-2
     *          4) if no pill dropped, command shaker motor, repeat steps 1-2 
     *          5) if no pill dropped, conclude the funnel is empty, finish
     */
    
    short int res_index, unload_count; 
    unsigned char pill_unload[1]; // randomly used array
    
    pill_unload[0] = pill; // used to transmit information to arduino
    
    if (pill == '1'){ // move servo slide to R reservoirs
        unload_count = 5;
        servo_slide(4);
        res_index = 0;}
    else if (pill == '2'){ // move servo slide to F reservoirs
        unload_count = 6;
        servo_slide(5);
        res_index = 1;}
    else if (pill == '3'){ // move servo slide to L reservoirs
        unload_count = 7;
        servo_slide(6);
        res_index = 2;} 
 
    while(unload_count > 0){ // continue to unload so long as there are still pills in funnels (p_count > 0)
        comp_time_difference(); // computing the current run time
        if (total_difference >= 175){ return;} // if the current run time is close to three minutes, end run
       
        uartTransmitBlocking(pill_unload, 1); // unload pill type according to pill_unload variable (R, F, L)          
        data[0] = 'P'; // prepare for polling
        receive_data_RX(); // poll for response from Arduino
        if (data_RX == 1){
            reservoir_count[res_index] += 1;
            unload_count = 5;}
        else{ unload_count --;}
        __delay_ms(100);
    }  
}

void initialize(int mat[16][3]){
    // Description: initialize changed_mat matrix, as well as configure PIC pins settings
    
    short int i = 0, j = 0;
    
    p_count = 0; // initializing p_count
    data_RX = 0; // initializing data_RX
    curr_height = 0; // initializing current height
    data[0] = '0'; // initializing data transmit array
    
    for (i = 0; i < 16; i++){ // uploading input values food diet pieces into changed_mat
        for (j = 0; j < 3; j++){
            changed_mat[i][j] = mat[i][j];}}
    for (i = 0; i < 4; i++){
        marked_drawers[i] = 0;}
    for (i = 0; i < 3; i++){
        reservoir_count[i] = 0;}
}

void finalize(short int curr_height, int mat[16][3]){
    /*
     * Description: Unload the remaining pills to their respective reservoirs, 
     *              adjust height back to starting position for next run, 
     *              reset servo slide to initial position.
     *            
     */ 
    
    short int i = 0, j = 0;
    for (i = 0; i < 16; i++){ // reset changed_mat
        for (j = 0; j < 3; j++){
            changed_mat[i][j] = 0;}} 
    
    if (curr_height > 0){
        data[0] = 'G';
        uartTransmitBlocking(data, 1); // Small lower to allow for the retracting of hook mechanism
        __delay_ms(2400);
        __delay_ms(2400);
        __delay_ms(1000);
        hook_mech(0); // retract hooks
    }
    
    unload_reservoir('1'); // unload remaining R pills to respective reservoir
    if (total_difference >= 175){ // if the current run time is close to three minutes, end run
        servo_slide(0);
        return;} 
    
    unload_reservoir('2'); // unload remaining F pills to respective reservoir
    if (total_difference >= 175){ // if the current run time is close to three minutes, end run
        servo_slide(0);
        return;}
   
    unload_reservoir('3'); // unload remaining L pills to respective reservoir 
    if (total_difference >= 175){ // if the current run time is close to three minutes, end run
         servo_slide(0);
         return;}
    
    servo_slide(0); // reset servo slide to first position 
}
