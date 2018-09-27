/* 
 * File:   add_functions.h
 * Author: Christopher Agia
 *
 * Created on February 27, 2018, 2:36 AM
 */


#ifndef ADD_FUNCTIONS_H
#define	ADD_FUNCTIONS_H

/********************************** Includes **********************************/
#include <xc.h>
#include <stdio.h>

/****************************** Global Variables ******************************/
extern int EEPROM_mat[8][4] = {0};
extern long int total_init_time = 0; // initial time in seconds
extern long int total_final_time = 0; // final time in seconds
extern long int total_difference = 0; // difference in seconds
extern short int run_time_minutes = 0; // to be stored in EEPROM
extern short int run_time_seconds = 0; // to be stored in EEPROM

/****************************** Helper Functions ******************************/
short int compare_arrays(int *array1, int *array2);
void array_transition(int mat[16][3]);
void receive_data_RX(void);
void operation_summary(void);
void set_total_init_time(void);
void comp_time_difference(void);
void comp_EEPROM_time(void);

#endif	/* ADD_FUNCTIONS_H */

