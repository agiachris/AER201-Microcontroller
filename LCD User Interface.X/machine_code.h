/* 
 * File:   machine_code.h
 * Author: Christopher Agia
 *
 * Created on February 23, 2018, 5:14 PM
 */

#ifndef MACHINE_CODE_H
#define	MACHINE_CODE_H

/********************************** Includes **********************************/
#include <xc.h>
#include <stdio.h>
#include <string.h>

/***** Global Variables *****/
extern short int p_count = 0; // Used in machine_code.c
extern short int curr_height = 0; // Used in machine_code.c and interface_main.c to track height
int changed_mat[16][3] = {0}; // accounts for marked drawers
extern int marked_drawers[4] = {0}; // will store the drawer numbers that are marked
extern int reservoir_count[3] = {0};
extern unsigned char data[1] = {'0'}; // used to communication with arduino through UART
extern unsigned char data_RX = '0'; // used to receive data from Arduino

/****************************** Machine Functions *****************************/

// Machine Code Functions
void package_food(int mat[16][3]);
short int sensor_row_check(short int row_ind, short int count);
void servo_slide(short int x);
void height_mech(short int dy, short int curr_height);
void hook_mech(int z);
void load_mech(short int row, short int j);
void unload_reservoir(unsigned char pill);
void initialize(int mat[16][3]);
void finalize(short int curr_height, int mat[16][3]);

#endif	/* MACHINE_CODE_H */

