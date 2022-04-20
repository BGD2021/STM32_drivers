#include "AD9959.H"

u8 CSR_DATA0[1] = {0x10};     // ���ゆ�� CH0
u8 CSR_DATA1[1] = {0x20};      // ���ゆ�� CH1
u8 CSR_DATA2[1] = {0x40};      // ���ゆ�� CH2
u8 CSR_DATA3[1] = {0x80};      // ���ゆ�� CH3
																	
 
u8 FR2_DATA[2] = {0x00,0x00};//default Value = 0x0000
u8 CFR_DATA[3] = {0x00,0x03,0x02};//default Value = 0x000302	   
																	
u8 CPOW0_DATA[2] = {0x00,0x00};//default Value = 0x0000   @ = POW/2^14*360
																	


u8 LSRR_DATA[2] = {0x00,0x00};//default Value = 0x----
																	
u8 RDW_DATA[4] = {0x00,0x00,0x00,0x00};//default Value = 0x--------
																	
u8 FDW_DATA[4] = {0x00,0x00,0x00,0x00};//default Value = 0x--------


sinwave sin_wave;
sweep_freq sweepfreq;
//AD9959���ゆ�峰����ゆ��
void Init_AD9959(void)  
{ 
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	u8 FR1_DATA[3] = {0xD0,0x00,0x00};
	
	GPIO_Initure.Pin=GPIO_PIN_15|GPIO_PIN_14|GPIO_PIN_13|GPIO_PIN_12|GPIO_PIN_11|GPIO_PIN_10|GPIO_PIN_9|GPIO_PIN_8|GPIO_PIN_7;
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull=GPIO_NOPULL;
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOE, &GPIO_Initure);

	GPIO_Initure.Pin=GPIO_PIN_12|GPIO_PIN_11|GPIO_PIN_10;
	HAL_GPIO_Init(GPIOB, &GPIO_Initure);
	GPIO_Initure.Pin=GPIO_PIN_1;
	HAL_GPIO_Init(GPIOG,&GPIO_Initure);
				 //���ゆ�烽���ゆ�烽����瀹����ゆ�烽���ゆ�烽���ゆ�峰����ゆ��GPIOC
	
		
	Intserve();  //IO����绛规�峰����ゆ��
	IntReset();  //AD9959���ゆ�蜂�
	
  WriteData_AD9959(FR1_ADD,3,FR1_DATA,1);//�����ゆ�烽���板��杈炬�烽���ゆ��1
//  WriteData_AD9959(FR2_ADD,2,FR2_DATA,0);
//  WriteData_AD9959(CFR_ADD,3,CFR_DATA,1);
//  WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,0);
//  WriteData_AD9959(ACR_ADD,3,ACR_DATA,0);
//  WriteData_AD9959(LSRR_ADD,2,LSRR_DATA,0);
//  WriteData_AD9959(RDW_ADD,2,RDW_DATA,0);
//  WriteData_AD9959(FDW_ADD,4,FDW_DATA,1);
   //�����ゆ�烽��缁�璁＄��烽��锟�
	sweepfreq.flag=0;
	sweepfreq.start=1000;
	sweepfreq.end=40000;
	sweepfreq.step=1000;
	sweepfreq.time=500;
	
	sin_wave.freq=1000;
	sin_wave.amp=1000;
	sin_wave.phase=0;
	
//	Out_freq(2,sin_wave.freq);
//	Out_mV(2,sin_wave.amp);
//	Write_Phase(2,sin_wave.phase);
////	Write_frequence(3,50);
////	Write_frequence(0,50);
////	Write_frequence(1,50);
////	Write_frequence(2,50);

	
} 

void Out_mV(u8 ch, float nmV) 
{  
	nmV=nmV*200/110;  
	if(nmV>1023)nmV=1023;//���ゆ�峰�撮���ゆ�烽���ゆ��
	Write_Amplitude(ch,nmV); 
}
void Out_freq(u8 ch, u32 freq) 
{ 
//	if(freq>FreqMax)freq=FreqMax; 
//	if(freq<FreqMin)freq=FreqMin;//���ゆ�峰�撮���ゆ�烽���ゆ��
	Write_frequence(ch,freq); 
}
u16 rmstime;
void Sweep_Freq(void)
{ 
	u16 times; 
	times=(sweepfreq.end-sweepfreq.start)/sweepfreq.step+1; 
	for(rmstime=0;rmstime<times;rmstime++) 
	{ 
		Write_frequence(2,sweepfreq.start+sweepfreq.step*rmstime); 
		delay_ms(sweepfreq.time); 
	} 
}
//���ゆ�锋��
void delay1 (u32 length)
{
	length = length*12;
   while(length--);
}
//IO����绛规�峰����ゆ��
void Intserve(void)		   
{   
		AD9959_PWR=0;
    CS = 1;
    SCLK = 0;
    UPDATE = 0;
    PS0 = 0;
    PS1 = 0;
    PS2 = 0;
    PS3 = 0;
    SDIO0 = 0;
    SDIO1 = 0;
    SDIO2 = 0;
    SDIO3 = 0;
}
//AD9959���ゆ�蜂�
void IntReset(void)	  
{
  Reset = 0;
	delay1(1);
	Reset = 1;
	delay1(30);
	Reset = 0;
}
 //AD9959���ゆ�烽���ゆ�烽���ゆ�烽���ゆ��
void IO_Update(void)  
{
	UPDATE = 0;
	delay1(2);
	UPDATE = 1;
	delay1(4);
	UPDATE = 0;
}
/*--------------------------------------------
���ゆ�烽���ゆ�烽���ゆ�烽���帮��烽���ゆ�烽���ゆ�烽���ゆ�烽�����ゆ��SPI���ゆ��AD9959�����ゆ�烽���ゆ��
RegisterAddress: ��渚ヨ揪�烽���ゆ�烽���ゆ�峰��
NumberofRegisters: ���ゆ�烽���ゆ�烽��琛��ゆ�烽���ゆ��
*RegisterData: ���ゆ�烽���ゆ�烽���ゆ�峰����ゆ�峰��
temp: ��瑙��ゆ�烽���ゆ�烽��锟�IO��渚ヨ揪�烽���ゆ��
----------------------------------------------*/
void WriteData_AD9959(u8 RegisterAddress, u8 NumberofRegisters, u8 *RegisterData,u8 temp)
{
	u8	ControlValue = 0;
	u8	ValueToWrite = 0;
	u8	RegisterIndex = 0;
	u8	i = 0;

	ControlValue = RegisterAddress;
//�����ゆ�烽��琛�锟�
	SCLK = 0;
	CS = 0;	 
	for(i=0; i<8; i++)
	{
		SCLK = 0;
		if(0x80 == (ControlValue & 0x80))
		SDIO0= 1;	  
		else
		SDIO0= 0;	  
		SCLK = 1;
		ControlValue <<= 1;
	}
	SCLK = 0;
//�����ゆ�烽���ゆ�烽���ゆ��
	for (RegisterIndex=0; RegisterIndex<NumberofRegisters; RegisterIndex++)
	{
		ValueToWrite = RegisterData[RegisterIndex];
		for (i=0; i<8; i++)
		{
			SCLK = 0;
			if(0x80 == (ValueToWrite & 0x80))
			SDIO0= 1;	  
			else
			SDIO0= 0;	  
			SCLK = 1;
			ValueToWrite <<= 1;
		}
		SCLK = 0;		
	}	
	if(temp==1)
	IO_Update();	
  CS = 1;
} 
/*---------------------------------------
���ゆ�烽���ゆ�烽���ゆ�烽���帮��烽���ゆ�烽���ゆ�烽�����ゆ�烽���ゆ�烽���＄��烽��锟�
Channel:  ���ゆ�烽���碉拷���ゆ��
Freq:     ���ゆ�烽���＄��烽��锟�
---------------------------------------*/
void Write_frequence(u8 Channel,u32 Freq)
{	 
		u8 CFTW0_DATA[4] ={0x00,0x00,0x00,0x00};	//����纭锋�烽���ゆ�烽��锟�
	  u32 Temp;            
	  Temp=(u32)Freq*8.589934592;	   //���ゆ�烽���ゆ�烽���ゆ�烽����ゆ�烽���ゆ�烽���ュ�ゆ�蜂负��渚ラ�╂�烽��琛��ゆ��  4.294967296=(2^32)/500000000
	  CFTW0_DATA[3]=(u8)Temp;
	  CFTW0_DATA[2]=(u8)(Temp>>8);
	  CFTW0_DATA[1]=(u8)(Temp>>16);
	  CFTW0_DATA[0]=(u8)(Temp>>24);
	  if(Channel==0)	  
	  {
			WriteData_AD9959(CSR_ADD,1,CSR_DATA0,1);//���ゆ�烽���″��杈炬�烽���ゆ�峰�����ゆ��CH0�����ゆ��
			WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.���ゆ�烽��锟�CH0����瀹�棰����ゆ��
		}
	  else if(Channel==1)	
	  {
			WriteData_AD9959(CSR_ADD,1,CSR_DATA1,1);//���ゆ�烽���″��杈炬�烽���ゆ�峰�����ゆ��CH1�����ゆ��
			WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.���ゆ�烽��锟�CH1����瀹�棰����ゆ��
	  }
	  else if(Channel==2)	
	  {
			WriteData_AD9959(CSR_ADD,1,CSR_DATA2,1);//���ゆ�烽���″��杈炬�烽���ゆ�峰�����ゆ��CH2�����ゆ��
			WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.���ゆ�烽��锟�CH2����瀹�棰����ゆ��
	  }
	  else if(Channel==3)	
	  {
			WriteData_AD9959(CSR_ADD,1,CSR_DATA3,1);//���ゆ�烽���″��杈炬�烽���ゆ�峰�����ゆ��CH3�����ゆ��
			WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,3);//CTW0 address 0x04.���ゆ�烽��锟�CH3����瀹�棰����ゆ��
	  }																																																																										 
	
} 
/*---------------------------------------
���ゆ�烽���ゆ�烽���ゆ�烽���帮��烽���ゆ�烽���ゆ�烽�����ゆ�烽���ゆ�烽���ゆ�烽���ゆ�烽��锟�
Channel:  ���ゆ�烽���碉拷���ゆ��
Ampli:    ���ゆ�烽���ゆ�烽���ゆ�烽��锟�
---------------------------------------*/
void Write_Amplitude(u8 Channel, u16 Ampli)
{ 
	u16 A_temp;//=0x23ff;
	u8 ACR_DATA[3] = {0x00,0x00,0x00};//default Value = 0x--0000 Rest = 18.91/Iout 
	
  A_temp=Ampli|0x1000;
  ACR_DATA[2] = (u8)A_temp;  //���ゆ�蜂����ゆ�烽���ゆ��
  ACR_DATA[1] = (u8)(A_temp>>8); //���ゆ�蜂����ゆ�烽���ゆ��
  if(Channel==0)
  {
		WriteData_AD9959(CSR_ADD,1,CSR_DATA0,1); 
    WriteData_AD9959(ACR_ADD,3,ACR_DATA,1); 
	}
  else if(Channel==1)
  {
		WriteData_AD9959(CSR_ADD,1,CSR_DATA1,1); 
    WriteData_AD9959(ACR_ADD,3,ACR_DATA,1);
	}
  else if(Channel==2)
  {
	  WriteData_AD9959(CSR_ADD,1,CSR_DATA2,1); 
    WriteData_AD9959(ACR_ADD,3,ACR_DATA,1); 
	}
  else if(Channel==3)
  {
		WriteData_AD9959(CSR_ADD,1,CSR_DATA3,1); 
    WriteData_AD9959(ACR_ADD,3,ACR_DATA,1); 
	}
}
/*---------------------------------------
���ゆ�烽���ゆ�烽���ゆ�烽���帮��烽���ゆ�烽���ゆ�烽�����ゆ�烽���ゆ�烽���ゆ�烽��杞匡拷
Channel:  ���ゆ�烽���碉拷���ゆ��
Phase:    ���ゆ�烽���ゆ�烽��杞匡拷,���ゆ�峰�撮���ゆ��0~16383(���ゆ�峰���瑙�搴�锝���0���ゆ��~360���ゆ��)
---------------------------------------*/
void Write_Phase(u8 Channel,u16 Phase)
{
	u16 P_temp=0;
  P_temp=(u16)Phase;
	CPOW0_DATA[1]=(u8)P_temp;
	CPOW0_DATA[0]=(u8)(P_temp>>8);
	if(Channel==0)
  {
		WriteData_AD9959(CSR_ADD,1,CSR_DATA0,0); 
    WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,0);
  }
  else if(Channel==1)
  {
		WriteData_AD9959(CSR_ADD,1,CSR_DATA1,0); 
		WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,0);
  }
  else if(Channel==2)
  {
		WriteData_AD9959(CSR_ADD,1,CSR_DATA2,0); 
    WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,0);
  }
  else if(Channel==3)
  {
		WriteData_AD9959(CSR_ADD,1,CSR_DATA3,0); 
    WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,0);
  }
}	 
