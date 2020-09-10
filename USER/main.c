#include	"config.h"
#include	"USART1.h"
#include	"timer.h"
#include	"delay.h"
#include "eeprom.h"

#include "motor.h"

int count = 0;
char servoID = 0;
extern MOTOR Motor;
/*************	����˵��	**************

˫����ȫ˫���жϷ�ʽ�շ�ͨѶ����

ͨ��PC��MCU��������, MCU�յ���ͨ�����ڰ��յ�������ԭ������.

******************************************/

/*************	���س�������	**************/


/*************	���ر�������	**************/


/*************	���غ�������	**************/



/*************  �ⲿ�����ͱ������� *****************/


/*************  ����1��ʼ������ *****************/
void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//�ṹ����
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//ģʽ,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//ʹ�ò�����,   BRT_Timer1, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//������, һ�� 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//��������,   ENABLE��DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//�����ʼӱ�, ENABLE��DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//�ж�����,   ENABLE��DISABLE
	COMx_InitStructure.UART_Polity    = PolityHigh;			//�ж����ȼ�, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//�л��˿�,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(����ʹ���ڲ�ʱ��)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//�ڲ���·RXD��TXD, ���м�, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//��ʼ������1 USART1,USART2
}

void	Timer_config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;					//�ṹ����
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//ָ������ģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			//ָ���ж����ȼ�, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//�ж��Ƿ�����,   ENABLE��DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;			//ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = ENABLE;				//�Ƿ������������, ENABLE��DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 8000);		//��ֵ,
	TIM_InitStructure.TIM_Run       = ENABLE;				//�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//��ʼ��Timer0	  Timer0,Timer1,Timer2
}

void	ADC_config(void)
{
	ADC_InitTypeDef		ADC_InitStructure;				//�ṹ����
	ADC_InitStructure.ADC_Px        = ADC_P10;	//����Ҫ��ADC��IO,	ADC_P10 ~ ADC_P17(�����),ADC_P1_All
	ADC_InitStructure.ADC_Speed     = ADC_540T;			//ADC�ٶ�			ADC_90T,ADC_180T,ADC_360T,ADC_540T
	ADC_InitStructure.ADC_Power     = ENABLE;			//ADC��������/�ر�	ENABLE,DISABLE
	ADC_InitStructure.ADC_AdjResult = ADC_RES_H8L2;		//ADC�������,	ADC_RES_H2L8,ADC_RES_H8L2
	ADC_InitStructure.ADC_Polity    = PolityLow;		//���ȼ�����	PolityHigh,PolityLow
	ADC_InitStructure.ADC_Interrupt = DISABLE;			//�ж�����		ENABLE,DISABLE
	ADC_Inilize(&ADC_InitStructure);					//��ʼ��
	ADC_PowerControl(ENABLE);							//������ADC��Դ��������, ENABLE��DISABLE
}

/**********************************************/
void main(void)
{
	AUXR &= ~2;
//	InternalRAM_enable();	
	
	UART_config();
	Timer_config();
	ADC_config();
	
	Motor.state=0;
	
	EA = 1;
	EEPROM_read_n(0x00, &servoID, 1);
	PrintString1("ID:");	//SUART1����һ���ַ���
	TX1_write2buff(servoID%100/10 + '0');
	TX1_write2buff(servoID%10 + '0');
	PrintString1("\r\n");
	
	while (1)
	{
		delay_ms(1);
		if(COM1.RX_TimeOut > 0)		//��ʱ����
		{
			if(--COM1.RX_TimeOut == 0)
			{
				if(COM1.RX_Cnt > 0)
				{
					
					if(((RX1_Buffer[0] & 0xC0))==0xC0)
					{
  				  servoID = RX1_Buffer[0] & 0x3F;
						EEPROM_SectorErase(0x00);
						EEPROM_write_n(0x00, &servoID, 1);
						
						PrintString1("���ID����Ϊ");
						TX1_write2buff(servoID%100/10 + '0');
						TX1_write2buff(servoID%10 + '0');
						PrintString1("\r\n");
//						servoID=7;
//						EEPROM_read_n(0x01, &servoID, 1);
//						TX1_write2buff(servoID%100/10 + '0');
//						TX1_write2buff(servoID%10 + '0');
//						TX1_write2buff('\n');						
						
						
						
					}
					else
					{
						//TX1_write2buff(RX1_Buffer[0]&224);
						//TX1_write2buff(servostate&224);
						if((RX1_Buffer[0]&0x3C)>>2 == servoID)//11100000B
						{
							//TX1_write2buff(motorID);
							Motor.state = (RX1_Buffer[0]&0xC0)>>6;
							Motor.target = ((u16)RX1_Buffer[1]<<2);
							Motor.speed = RX1_Buffer[1];
							//EEPROM_write_n(u16 EE_address,u8 *DataAddress,u16 number);

							
								TX1_write2buff(Motor.angle/1000+ '0');
								TX1_write2buff(Motor.angle%1000/100 + '0');
								TX1_write2buff(Motor.angle%100/10 + '0');
								TX1_write2buff(Motor.angle%10 + '0');
								TX1_write2buff(' ');

						}
					}
				}
//					for(i=0; i<COM1.RX_Cnt; i++)	TX1_write2buff(RX1_Buffer[i]);	//�յ�������ԭ������
				COM1.RX_Cnt = 0;
			}
		}
	}
}

/********************* Timer0�жϺ���************************/
void timer0_int (void) interrupt TIMER0_VECTOR
{
//	int j=0;
	Set_Motor();
/*	
	if(count!=2500)
		count++;
//		TX1_write2buff(count);
	else
	{
		count=0;
		
		j = Motor.angle;
		TX1_write2buff(j/1000 + '0');
		TX1_write2buff(j%1000/100 + '0');
		TX1_write2buff(j%100/10 + '0');
		TX1_write2buff(j%10 + '0');
		TX1_write2buff(' ');
	}
*/
	

}
