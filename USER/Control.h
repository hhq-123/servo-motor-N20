
#ifndef	__CONTROL_H
#define	__CONTROL_H

#include	"config.h"
#include  "motor.h"
#include	"adc.h"
			
#define PWMTIME 256			
#define Pdat 1
#define Idat 0
#define Ddat 0


void pid();
float Get_MxMi(float num,float max,float min);
	
#endif





			