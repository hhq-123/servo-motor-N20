#include	"config.h"
#include	"USART1.h"
#include	"timer.h"
#include	"delay.h"
#include "eeprom.h"

#include "motor.h"

int count = 0;
char servoID = 0;
extern MOTOR Motor;
/*************	功能说明	**************

双串口全双工中断方式收发通讯程序。

通过PC向MCU发送数据, MCU收到后通过串口把收到的数据原样返回.

******************************************/

/*************	本地常量声明	**************/


/*************	本地变量声明	**************/


/*************	本地函数声明	**************/



/*************  外部函数和变量声明 *****************/


/*************  串口1初始化函数 *****************/
void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//结构定义
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//使用波特率,   BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率, 一般 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Polity    = PolityHigh;			//中断优先级, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//切换端口,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(必须使用内部时钟)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//内部短路RXD与TXD, 做中继, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//初始化串口1 USART1,USART2
}

void	Timer_config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;					//结构定义
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			//指定中断优先级, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;			//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = ENABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 8000);		//初值,
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//初始化Timer0	  Timer0,Timer1,Timer2
}

void	ADC_config(void)
{
	ADC_InitTypeDef		ADC_InitStructure;				//结构定义
	ADC_InitStructure.ADC_Px        = ADC_P10;	//设置要做ADC的IO,	ADC_P10 ~ ADC_P17(或操作),ADC_P1_All
	ADC_InitStructure.ADC_Speed     = ADC_540T;			//ADC速度			ADC_90T,ADC_180T,ADC_360T,ADC_540T
	ADC_InitStructure.ADC_Power     = ENABLE;			//ADC功率允许/关闭	ENABLE,DISABLE
	ADC_InitStructure.ADC_AdjResult = ADC_RES_H8L2;		//ADC结果调整,	ADC_RES_H2L8,ADC_RES_H8L2
	ADC_InitStructure.ADC_Polity    = PolityLow;		//优先级设置	PolityHigh,PolityLow
	ADC_InitStructure.ADC_Interrupt = DISABLE;			//中断允许		ENABLE,DISABLE
	ADC_Inilize(&ADC_InitStructure);					//初始化
	ADC_PowerControl(ENABLE);							//单独的ADC电源操作函数, ENABLE或DISABLE
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
	PrintString1("ID:");	//SUART1发送一个字符串
	TX1_write2buff(servoID%100/10 + '0');
	TX1_write2buff(servoID%10 + '0');
	PrintString1("\r\n");
	
	while (1)
	{
		delay_ms(1);
		if(COM1.RX_TimeOut > 0)		//超时计数
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
						
						PrintString1("舵机ID更新为");
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
//					for(i=0; i<COM1.RX_Cnt; i++)	TX1_write2buff(RX1_Buffer[i]);	//收到的数据原样返回
				COM1.RX_Cnt = 0;
			}
		}
	}
}

/********************* Timer0中断函数************************/
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
