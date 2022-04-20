#include "esp8266.hpp"
#include "string.h"

extern DMA_HandleTypeDef hdma_usart6_rx;

CPP_UART test_uart = CPP_UART(&huart3);
ESP8266 wifi = ESP8266(&huart6, &hdma_usart6_rx, &htim6);

/**
 * @brief  ESP8266 �� ���캯��
 * @param  huart  : ����ESP8266���õ��Ĵ���
 * @param  dma_rx : ���ڽ��ն�Ӧ��DMA
 * @param  htim   : ���ڼ����Ƿ�ʱ�Ķ�ʱ��
 * @note  huart ��Ӧ�Ĵ�������Ҫ����ȷ����������Ĭ�ϵ�115200(Ҳ���Լ����ģ�
 * @            ��Ҫע��ESP8266������մ��ڵ�����)��������Ҫ���ж�
 * @     dma_rx ��������dma�Ľ��գ�һ��һ���ֽڣ�ѭ��ģʽ(circle)
 * @       htim ���ÿ�����ʱ���жϣ����ù���Ԥ��Ƶϵ�����Զ���װ��ֵ��OnePulseģʽ
 * @retval None
 **/
ESP8266::ESP8266(UART_HandleTypeDef* huart, DMA_HandleTypeDef *dma_rx, TIM_HandleTypeDef *htim)
	:uart(huart)
{
	this->dma_rx     = dma_rx;
	this->tim        = htim;
	this->uart_state = Idle;
	this->mode       = NOINIT;
}

/**
 * @brief  ESP8266 ��ʼ������
 * @retval None
 **/
void ESP8266::RxCallback(){
	UART_HandleTypeDef*  huart = uart.huart; //ESP8266�õ��Ĵ���

	/* �ж��Ƿ��������ж� */
	if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET) {
		__HAL_UART_CLEAR_IDLEFLAG(huart); //���IDLE�жϱ�־λ
		HAL_UART_DMAStop(huart);          // ֹͣDMA����

		//���²����ǽ����յ������ݴ��� AckBuffer ��
		int recv_end    = RxBufferSize - __HAL_DMA_GET_COUNTER(dma_rx); // �õ����յ����ַ�����
		int recv_start  = recv_end % RxBufferSize;                      // ���յ������ݵ���ʼ����
		while(RxBuffer[recv_start] == 0) { //�ҳ����յ���ʼ
			recv_start = (recv_start + 1) % RxBufferSize;
		}
		int index = recv_start;
		int i;
		for(i=0; index != recv_end; i++)
		{
			AckBuffer[i] = RxBuffer[index];
			RxBuffer[index] = 0;
			index = (index+1)%RxBufferSize;
		}
		AckBuffer[i] = 0; //�����ַ����ָ���

		//�����ã������յ�����Ӧͨ���������
		test_uart << "\r\n����:\r\n";
		test_uart << AckBuffer;
		//ʹ�ö���������
		//if(uart_state == Sent) {//������ڷ�����ϣ��ȴ�����
		//uart_state = Idle; //��Ϊ����ģʽ
		//}
		//if(uart_state == Idle) { //��һ���Ǳ����ؽ��յ���Ϣ��
		//for(;;); //TODO:
		//}

		/* ׼����һ��DMA���� */
		__HAL_DMA_SET_COUNTER(dma_rx, 0);
		HAL_UART_Receive_DMA(huart, (uint8_t*)RxBuffer, RxBufferSize);
	}
}

/**
 * @brief  ESP8266 ���÷���ָ������ݺ����ĵȴ���ʱʱ��
 * @note ms���ܳ���43,690
 * @retval None
 **/
void ESP8266::setTimeout(uint16_t ms){
	__HAL_TIM_SET_AUTORELOAD(tim, ms*3/2);  //Ҫ�����ӳ�10ms���жϣ�����Ҫ���� ARR Ϊ 10*3/2;
	__HAL_TIM_SET_COUNTER(tim, 1);          //��ΪҪ����CNT�Ƿ�Ϊ0���ж��Ƿ�ʱ��������������Ϊ1
	__HAL_TIM_ENABLE(tim);                  //������ʱ������ʱ����������Զ���0����ֹͣ����
}

/**
 * @brief  ESP8266 �ж��Ƿ�ʱ
 * @note   ������setTimeout֮�����ʹ��
 * @retval None
 **/
bool ESP8266::isTimeout(void){
	if(__HAL_TIM_GET_COUNTER(tim) == 0){
		return true;
	}
	else {
		return false;
	}
}

/*
 *
 * */
void ESP8266::sendNoAck(uint8_t *str) {
	uart << str;
	//TODO: ������
	if(!(str[0] == '\r' && str[1] == '\n')) {
		test_uart << "\r\n����:\r\n";
		test_uart << str;
	}
	/////////////////
}

result ESP8266::send(char *str,const char* response,uint16_t timeout){
	/* �������� */
	sendNoAck((uint8_t*)str);

	/* �ȴ���Ӧ */
	uart_state = Sent;
	setTimeout(timeout); //���ó�ʱʱ��
	while(!isTimeout() && !isResponded(response));
	uart_state = Idle;

	/* �ж�ִ�еĽ�� */
	if(isTimeout()) {
		return Timeout; //����ʧ�ܻ��߳�ʱ
	}
	else {
		return Success; //���ݳɹ�����
	}
}

result ESP8266::sendCmd(const char *str,const char* response,uint16_t timeout){
	/* �������� */
	sendNoAck((uint8_t*)str);
	sendNoAck((uint8_t*)"\r\n");

	/* �ȴ���Ӧ */
	uart_state = Sent;
	setTimeout(timeout); //���ó�ʱʱ��
	while(!isTimeout() && !isResponded(response));
	uart_state = Idle;

	/* �ж�ִ�еĽ�� */
	if(isResponded(response)) {
		return Success;
	}
	else {
		return Timeout; //����ʧ�ܻ��߳�ʱ
	}
}

/**
 * @brief  �˳�͸��ģʽ
 * @retval None
 **/
void ESP8266::quitTrans(void) {
	/* ���� "+++" �˳�͸��״̬ */
	sendNoAck((uint8_t*)"+"); delay(15);//���ڴ�����֡ʱ��(10ms)
	sendNoAck((uint8_t*)"+"); delay(15);
	sendNoAck((uint8_t*)"+"); delay(500);
	/* �ر�͸��ģʽ */
	sendCmd("AT+CIPMODE=0", "OK", 200);
}

/**
 * @brief  �ӳٺ���
 * @note   ͨ����ʱ��ʵ�ֵ��ӳ٣�ֻ��ʹ����quitTrans����
 * @retval None
 **/
void ESP8266::delay(uint16_t ms) {
	setTimeout(ms);
	while(!isTimeout());
}

/**
 * @brief  �ж��Ƿ��ESP8266�õ���������Ӧ��ĸ�����Ӵ���
 * @retval �Ƿ�õ���������Ӧ
 **/
bool ESP8266::isResponded(const char* response){
	int responseLen = strlen(response);
	int rxBufferLen = strlen(RxBuffer);
	for(int i=0,j=0; i<rxBufferLen; i++) {
		if(RxBuffer[i] == response[j])	{
			if(++j == responseLen) { //�ҵ��ִ� �����ҵ���Ӧ��
				return true;
			}
		}
		else {
			j = 0;
		}
	}
	return false; //δ�ҵ��Ӵ�
}

/**
 * @brief  ESP8266 ��ʼ������
 * @retval ��ʼ���Ƿ�ɹ�
 **/
result ESP8266::init(){
	UART_HandleTypeDef*  huart = uart.huart; //ESP8266�õ��Ĵ���
	/* ����DMA���� */
	HAL_UART_Receive_DMA(huart, (uint8_t*)RxBuffer, RxBufferSize);
	__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE); //���������ж�

	/* �������ڼ�¼�Ƿ�ʱ�Ķ�ʱ�� */
	HAL_TIM_Base_Start(tim);
	HAL_TIM_OnePulse_Init(tim, TIM_OPMODE_SINGLE);                 //����Ϊ OnePulse ģʽ
	__HAL_TIM_SET_PRESCALER(tim, HAL_RCC_GetHCLKFreq()/3/1000-1);  //���÷�Ƶ��Ϊ 56,000

	delay(100);
	
	
	
	char buf[128];
	quitTrans(); //�˳�͸��
	
//*---- ��������ֻ��Ҫִ��һ�� -----*/
//	sendCmd("AT+RESTORE"  , "OK" , 4000); //�ָ���������
//	delay(5000);
//	sendCmd("AT+CWMODE=1" , "OK" , 4000); //���ó�STAģʽ
//	sendCmd("AT+RST"      , "OK" , 1000); //������Ч
//	delay(5000);
//	sendCmd("AT+CIPSTAMAC=\"18:fe:36:98:d3:1b\"", "OK", 100); //���� MAC ��ַ
	wifi.sendCmd("ATE0"        , "OK" , 100);  //�رջ���
	
	
	/* ����wifi */
	sprintf(buf,"AT+CWJAP=\"%s\",\"%s\"","dxxy16-402-1","dxxy16402");
	sendCmd(buf, "WIFI GOT IP", 15000); //����AP
	send((char*)"", "OK"         , 2000); //����AP
	
	
	/* ����͸��ģʽ */
	sendCmd("AT+CIPMODE=1", "OK", 200); //����͸��ģʽ

	/* ��PC���������� */
	sprintf(buf,"AT+CIPSTART=\"%s\",\"%s\",%s", "TCP", "192.168.0.189", "8688");
	sendCmd(buf, "OK", 15000);

	sendCmd("AT+CIPSEND"  , ">", 100); //����͸��
	
	
	for(;;) {
		sendNoAck((uint8_t*)"hello world \r\n");
		delay(1000);
	}
	
	return Success;
}

