/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint8_t receive_str[100];     //接收缓存数组,最大USART_REC_LEN个字节
uint16_t uart_byte_count=0;
uint8_t aRxBuffer[1]; //HAL库使用的串口接收缓冲
/* USER CODE END PV */


/**
  * @brief This function handles USART6 global interrupt.
  */
void USART6_IRQHandler(void)
{
  /* USER CODE BEGIN USART6_IRQn 0 */
	uint32_t timeout=0;
  /* USER CODE END USART6_IRQn 0 */
  HAL_UART_IRQHandler(&huart6);
  /* USER CODE BEGIN USART6_IRQn 1 */
  	timeout=0;
    while (HAL_UART_GetState(&huart6) != HAL_UART_STATE_READY)//等待就绪
    {
    	timeout++;////超时处理
       if(timeout>HAL_MAX_DELAY) break;
  	}

  	timeout=0;
  	while(HAL_UART_Receive_IT(&huart6, (uint8_t *)aRxBuffer, 1) != HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
  	{

  	 timeout++; //超时处理
  	 if(timeout>HAL_MAX_DELAY) break;
    }
  /* USER CODE END USART6_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if(huart->Instance==USART6){
		if((uart_byte_count&0x8000)==0)//接收未完成
		{
			if(uart_byte_count&0x4000)//接收到了0x0d
			{
				if(aRxBuffer[0]!=0x0a)uart_byte_count=0;//接收错误,重新开始
				else uart_byte_count|=0x8000;	//接收完成了
			}
			else //还没收到0X0D
			{
				if(aRxBuffer[0]==0x0d)uart_byte_count|=0x4000;
				else
				{
					receive_str[uart_byte_count&0X3FFF]=aRxBuffer[0] ;
					uart_byte_count++;
					if(uart_byte_count>(100-1))uart_byte_count=0;//接收数据错误,重新开始接收
				}
			}
		}

	}
}
/* USER CODE END 1 */

//主函数
//串口
uint16_t len;
extern uint16_t uart_byte_count;
extern uint8_t aRxBuffer[1];
extern uint8_t receive_str[100];


if(uart_byte_count&0x8000)
{
	len=uart_byte_count&0x3fff;//得到此次接收到的数据长度
	printf("\r\n您发送的消息为:\r\n");
	HAL_UART_Transmit(&huart6,(uint8_t*)receive_str,len,1000);	//发送接收到的数据
	printf("\r\n\r\n");//插入换行
	uart_byte_count=0;
}


//重定向printf
#include "stdio.h"
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
	HAL_UART_Transmit(&huart6, (uint8_t*)&ch,1,HAL_MAX_DELAY);
    return ch;
}