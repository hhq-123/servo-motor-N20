#include "Control.h"


unsigned char nmr=0;

extern PID PID_Motor[4];

void pid()
{
		for(nmr=0;nmr<4;nmr++)
		{
			PID_Motor[nmr].angle = Get_ADC10bitResult(nmr);	//参数0~7,查询方式做一次ADC, 返回值就是结果, == 1024 为错误
			PID_Motor[nmr].error=PID_Motor[nmr].angle-PID_Motor[nmr].target;
						//计算误差
			PID_Motor[nmr].pout =Pdat * PID_Motor[nmr].error;
						//P
			PID_Motor[nmr].iout += Idat * PID_Motor[nmr].error;
			PID_Motor[nmr].iout = Get_MxMi(PID_Motor[nmr].iout,16,-16);//?D?áIê?·?3?3?·??§
						//I
			PID_Motor[nmr].dout = Ddat*(PID_Motor[nmr].error-PID_Motor[nmr].Last_error);	
			PID_Motor[nmr].Last_error=PID_Motor[nmr].error;	
						//D
			PID_Motor[nmr].OUT += PID_Motor[nmr].pout + PID_Motor[nmr].iout +PID_Motor[nmr].dout;////P,I,D融合
			PID_Motor[nmr].OUT=Get_MxMi(PID_Motor[nmr].OUT,256,-256);
		}
}



	
		





float Get_MxMi(float num,float max,float min)
{
	if(num>max)
		return max;
	else if(num<min)
		return min;
	else
		return num;
}



			