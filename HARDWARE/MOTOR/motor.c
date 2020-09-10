#include "motor.h"
#include	"adc.h"
#include "STC15Fxxxx.h"

sbit	Motor1 = P3^2;		//������������š�
sbit	Motor2 = P3^3;	

MOTOR Motor;
char pwm=0;
char pwm0=127;

void Set_Motor()
{
///////////////////////////////////////
	if(Motor.state==0)
	{
		Motor1=0,			Motor2=0;
	}
	else if(Motor.state==1)
	{
//		Motor.angle = (Get_ADC10bitResult(0)+Motor.angle*4)/5;	//����0~7,��ѯ��ʽ��һ��ADC, ����ֵ���ǽ��, == 1024 Ϊ����
		Motor.angle = Get_ADC10bitResult(0);
		if(Motor.angle-Motor.target> error1)
		{
			Motor1=1,			Motor2=0;
		}
		else if(Motor.angle-Motor.target< -error1)
		{
			Motor1=0,			Motor2=1;
		}
		else if(Motor.angle-Motor.target> error0)
		{
			Motor1=~Motor1,			Motor2=0;
		}	
		else if(Motor.angle-Motor.target< -error0)
		{
			Motor1=0,			Motor2=~Motor2;
		}
		else
		{
			Motor1=0,			Motor2=0;
		}
	}
	
	else if(Motor.state==2)	
	{
		
		if(pwm0==0)
		{
			if(Motor.speed>0)
				Motor2=0;
			if(Motor.speed<0)
				Motor2=1;
			
			pwm0=127;
			pwm= Motor.speed&0x7F;
			Motor1=1;
		}
		
		if(pwm!=0)
			pwm--;
		else
			Motor1=0;
		
		pwm0--;
		
		
	}
}
