#include "esp8266.hpp"
using namespace ESP8266_Space;

extern "C" void MineMain(void)
{
	char buf[128];

	wifi.init();

	while(wifi.sendCmd("AT", "OK", 100) == Timeout){
		wifi.quitTrans(); //�˳�͸��
	}

	wifi.sendCmd("ATE0"    , "OK" , 100);  //�رջ���
	//wifi.sendCmd("AT+CWLAP", "OK" , 5000); //��ʾ�ɼ����AP

	/* ����wifi */
	sprintf(buf,"AT+CWJAP=\"%s\",\"%s\"","ChinaNet-916FD0","fdsd5222");
	wifi.sendCmd(buf, "WIFI GOT IP", 15000); //����AP
	wifi.send((char*)"", "OK"         , 2000); //����AP

	/* ��PC���������� */
	sprintf(buf,"AT+CIPSTART=\"%s\",\"%s\",%s", "TCP", "192.168.2.5", "8080");
	wifi.sendCmd(buf, "OK", 15000);

	/* ����͸��ģʽ */
	//wifi.sendCmd("AT+CIPMODE=1", "OK", 200); //����͸��ģʽ
	//wifi.sendCmd("AT+CIPSEND"  , ">", 100); //����͸��

	for(;;){
		static int num =0;
		
		sprintf(buf,"%4d\r\n",num++);
		HAL_Delay(500);
		//��͸��
		wifi.sendCmd("AT+CIPSEND=6",">",100); //��Ҫ����4������
		wifi.send(buf, "SEND OK", 100);
		
		//͸��ģʽ
		//wifi.sendNoAck((uint8_t *)buf);
	}
}
