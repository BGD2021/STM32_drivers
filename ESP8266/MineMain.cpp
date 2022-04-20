#include "esp8266.hpp"
using namespace ESP8266_Space;

extern "C" void MineMain(void)
{
	char buf[128];

	wifi.init();

	while(wifi.sendCmd("AT", "OK", 100) == Timeout){
		wifi.quitTrans(); //退出透传
	}

	wifi.sendCmd("ATE0"    , "OK" , 100);  //关闭回显
	//wifi.sendCmd("AT+CWLAP", "OK" , 5000); //显示可加入的AP

	/* 加入wifi */
	sprintf(buf,"AT+CWJAP=\"%s\",\"%s\"","ChinaNet-916FD0","fdsd5222");
	wifi.sendCmd(buf, "WIFI GOT IP", 15000); //加入AP
	wifi.send((char*)"", "OK"         , 2000); //加入AP

	/* 与PC机建立连接 */
	sprintf(buf,"AT+CIPSTART=\"%s\",\"%s\",%s", "TCP", "192.168.2.5", "8080");
	wifi.sendCmd(buf, "OK", 15000);

	/* 进入透传模式 */
	//wifi.sendCmd("AT+CIPMODE=1", "OK", 200); //开启透传模式
	//wifi.sendCmd("AT+CIPSEND"  , ">", 100); //进入透传

	for(;;){
		static int num =0;
		
		sprintf(buf,"%4d\r\n",num++);
		HAL_Delay(500);
		//非透传
		wifi.sendCmd("AT+CIPSEND=6",">",100); //将要发送4个数据
		wifi.send(buf, "SEND OK", 100);
		
		//透传模式
		//wifi.sendNoAck((uint8_t *)buf);
	}
}
