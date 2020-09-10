
#ifndef __MOTOR_H
#define __MOTOR_H

#include	"config.h"

#define error0 4
#define error1 20

typedef struct
{
	char state;
	int angle;
	int target;
	char speed;
}MOTOR;

void Set_Motor();

#endif