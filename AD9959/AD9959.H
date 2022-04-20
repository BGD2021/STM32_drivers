#ifndef _AD9959_H_
#define _AD9959_H_
#include "base.h"
#include "stdint.h"
//AD9959锟侥达拷锟斤拷锟斤拷址锟斤拷锟斤拷
#define CSR_ADD   0x00   //CSR 通锟斤拷选锟斤拷拇锟斤拷锟�
#define FR1_ADD   0x01   //FR1 锟斤拷锟杰寄达拷锟斤拷1
#define FR2_ADD   0x02   //FR2 锟斤拷锟杰寄达拷锟斤拷2
#define CFR_ADD   0x03   //CFR 通锟斤拷锟斤拷锟杰寄达拷锟斤拷
#define CFTW0_ADD 0x04   //CTW0 通锟斤拷频锟斤拷转锟斤拷锟街寄达拷锟斤拷
#define CPOW0_ADD 0x05   //CPW0 通锟斤拷锟斤拷位转锟斤拷锟街寄达拷锟斤拷
#define ACR_ADD   0x06   //ACR 锟斤拷锟饺匡拷锟狡寄达拷锟斤拷
#define LSRR_ADD  0x07   //LSR 通锟斤拷锟斤拷锟斤拷扫锟斤拷拇锟斤拷锟�
#define RDW_ADD   0x08   //RDW 通锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷扫锟斤拷拇锟斤拷锟�
#define FDW_ADD   0x09   //FDW 通锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷扫锟斤拷拇锟斤拷锟�
//AD9959锟杰脚宏定锟斤拷
#define CS			PEout(11)
#define SCLK		PEout(9)
#define UPDATE		PEout(13)
#define PS0			PBout(12)
#define PS1			PBout(10)
#define PS2			PEout(14)
#define PS3			PEout(12)
#define SDIO0		PEout(10)
#define SDIO1		PEout(8)
#define SDIO2		PGout(1)
#define SDIO3		PEout(7)
#define AD9959_PWR	PBout(11)
#define Reset		PEout(15)

void delay1 (u32 length);
void IntReset(void);	  //AD9959锟斤拷位
void IO_Update(void);   //AD9959锟斤拷锟斤拷锟斤拷锟斤拷
void Intserve(void);		   //IO锟节筹拷始锟斤拷
void WriteData_AD9959(u8 RegisterAddress, u8 NumberofRegisters, u8 *RegisterData,u8 temp);
void Init_AD9959(void);
void Write_frequence(u8 Channel,u32 Freq);
void Write_Amplitude(u8 Channel, u16 Ampli);
void Write_Phase(u8 Channel,u16 Phase);

void Sweep_Freq(void);
void Out_mV(u8 ch, float nmV);
void Out_freq(u8 ch, u32 freq);

typedef struct _sweep_freq {
	u32 flag;
	u32 start;
	u32 step;
	u32 time;
	u32 end;
}sweep_freq;

typedef struct _sinwave {
	u32 freq;
	u16 phase;
	float amp;
}sinwave;
















#endif
