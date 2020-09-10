#include "Control.h"


unsigned char nmr=0;

extern PID PID_Motor[4];

void pid()
{
		for(nmr=0;nmr<4;nmr++)
		{
			PID_Motor[nmr].angle = Get_ADC10bitResult(nmr);	//����0~7,��ѯ��ʽ��һ��ADC, ����ֵ���ǽ��, == 1024 Ϊ����
			PID_Motor[nmr].error=PID_Motor[nmr].angle-PID_Motor[nmr].target;
						//�������
			PID_Motor[nmr].pout =Pdat * PID_Motor[nmr].error;
						//P
			PID_Motor[nmr].iout += Idat * PID_Motor[nmr].error;
			PID_Motor[nmr].iout = Get_MxMi(PID_Motor[nmr].iout,16,-16);//?D?��I��?��?3?3?��??��
						//I
			PID_Motor[nmr].dout = Ddat*(PID_Motor[nmr].error-PID_Motor[nmr].Last_error);	
			PID_Motor[nmr].Last_error=PID_Motor[nmr].error;	
						//D
			PID_Motor[nmr].OUT += PID_Motor[nmr].pout + PID_Motor[nmr].iout +PID_Motor[nmr].dout;////P,I,D�ں�
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



			