#ifndef __ESP8266_HPP
#define __ESP8266_HPP

#include "cpp_uart.hpp"
#include "tim.h"
#define RxBufferSize 2048  //���ջ����С

/* ESP8266�����ռ� */
namespace ESP8266_Space
{
	/* ESP8266��ǰ����ģʽ */
	enum mode_set {
		NOINIT,      //δ��ʼ��
		STA,         //�ӻ�ģʽ
		AP,          //����ģʽ
		APSTA,       //�ӻ�+����ģʽ
		DISCONNECT   //�Ͽ�����
	};
	/* ����ִ�е�״̬ */
	enum uart_state_set {
		Idle,         //����
		Busy,         //æ
		Sent,         //�������,�ȴ�����
	};
	/* ����ִ�еĽ�� */
	enum result {
		Fail,        //ʧ��
		Success,     //�ɹ�
		Timeout      //��ʱ
	};
}
/* ʹ�������ռ� */
using namespace ESP8266_Space;


class ESP8266{
public:
	CPP_UART uart;                 //ESP8266���õĴ���
	DMA_HandleTypeDef *dma_rx;     //���ڴ��ڽ��յ�DMA
	TIM_HandleTypeDef *tim;        //���ڼ�¼�Ƿ�ʱ�Ķ�ʱ��
	mode_set mode;                 //ESP8266�Ĺ���ģʽ
	uart_state_set uart_state;     //��¼����ͨ�ŵĴ��ڵ�״̬
	char RxBuffer[RxBufferSize];   //���ջ���
	char AckBuffer[RxBufferSize];  //�����Ӧ���ݵĻ���
	//char TxBuffer[TxBufferSize]; //���ջ���

	/* ���캯�� */
	ESP8266(UART_HandleTypeDef* huart, DMA_HandleTypeDef *dma_rx, TIM_HandleTypeDef *htim);
	/* �������� */
	result send(char *str, const char* response, uint16_t timeout);
	/* ��������(��Ҫ����Ӧ) */
	void sendNoAck(uint8_t *str);
	/* ����ָ�� */
	result sendCmd(const char *str, const char* response, uint16_t timeout);
	/* �˳�͸��ģʽ */
	void quitTrans(void);
	/* ���ջص����������Լ���ӣ� */
	void RxCallback();
	/* ���ó�ʱʱ�� */
	void setTimeout(uint16_t ms);
	/* �ж��Ƿ�ʱ */
	bool isTimeout(void);
	/* �ӳٺ��� */
	void delay(uint16_t ms);
	/* �ж��Ƿ���Ӧ */
	bool isResponded(const char* response);
	/* ��ʼ�� */
	result init();
};


extern ESP8266 wifi;

#endif
