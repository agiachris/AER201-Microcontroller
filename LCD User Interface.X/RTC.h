/* 
 * File:   RTC.h
 * Author: Christopher Agia
 *
 * Created on February 26, 2018, 2:22 PM
 */

#ifndef RTC_H
#define	RTC_H

/********************************** Includes **********************************/
#include <xc.h>
#include <stdio.h>
#include <string.h>
#include "configBits.h"

/****************************** RTC Functions *********************************/
extern unsigned char time[7] = {0}; //sec, min, hour, weekday, weekday(#), month, year

void initTime(unsigned char s, unsigned char m, unsigned char h,unsigned char w,
              unsigned char DD, unsigned char MM, unsigned char YY);
void loadRTC();
void getRTC(void);
void printRTC(void);

#endif	/* RTC_H */