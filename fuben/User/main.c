/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-09-06
 * @brief       ���ᴫ����-��ȡԭʼ���� ʵ��
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ������ H743������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/SDRAM/sdram.h"
#include "./USMART/usmart.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/MPU/mpu.h"
#include "./BSP/SH3001/sh3001.h"
#include <math.h>
#include "ai_platform.h"
#include "network.h"
#include "network_data.h"

ai_handle network;
float aiInData[AI_NETWORK_IN_1_SIZE];
float aiOutData[AI_NETWORK_OUT_1_SIZE];
ai_u8 activations[AI_NETWORK_DATA_ACTIVATIONS_SIZE];
const char* activities[AI_NETWORK_OUT_1_SIZE] = {
  "stationary", "left_right", "up_down"
};
ai_buffer * ai_input;
ai_buffer * ai_output;

static void AI_Init(void);
static void AI_Run(float *pIn, float *pOut);
static uint32_t argmax(const float * values, uint32_t len);

static void AI_Init(void)
{
  ai_error err;

  /* Create a local array with the addresses of the activations buffers */
  const ai_handle act_addr[] = { activations };
  /* Create an instance of the model */
  err = ai_network_create_and_init(&network, act_addr, NULL);
  if (err.type != AI_ERROR_NONE) {
    printf("ai_network_create error - type=%d code=%d\r\n", err.type, err.code);
    //Error_Handler();
  }
  ai_input = ai_network_inputs_get(network, NULL);
  ai_output = ai_network_outputs_get(network, NULL);
}
static void AI_Run(float *pIn, float *pOut)
{
  ai_i32 batch;
  ai_error err;

  /* Update IO handlers with the data payload */
  ai_input[0].data = AI_HANDLE_PTR(pIn);
  ai_output[0].data = AI_HANDLE_PTR(pOut);

  batch = ai_network_run(network, ai_input, ai_output);
  if (batch != 1) {
    err = ai_network_get_error(network);
    printf("AI ai_network_run error - type=%d code=%d\r\n", err.type, err.code);
    //Error_Handler();
  }
}

static uint32_t argmax(const float * values, uint32_t len)
{
  float max_value = values[0];
  uint32_t max_index = 0;
  for (uint32_t i = 1; i < len; i++) {
    if (values[i] >= max_value) {
      max_value = values[i];
      max_index = i;
    }
  }
  return max_index;
}
/**
 * @brief       ��ʾԭʼ����
 * @param       x, y : ����
 * @param       title: ����
 * @param       val  : ֵ
 * @retval      ��
 */
void user_show_mag(uint16_t x, uint16_t y, char *title, int16_t val)
{
    char buf[20];

    sprintf(buf,"%s%d", title, val);                /* ��ʽ����� */
    lcd_fill(x, y, x + 160, y + 16, WHITE);         /* ����ϴ�����(�����ʾ20���ַ�,20*8=160) */
    lcd_show_string(x, y, 160, 16, 16, buf, BLUE);  /* ��ʾ�ַ��� */
}

/**
 * @brief       ��ʾ�¶�
 * @param       x, y : ����
 * @param       temp : �¶�
 * @retval      ��
 */
void user_show_temprate(uint16_t x, uint16_t y, float temp)
{
    char buf[20];

    sprintf(buf,"Temp:%2.1fC", temp);               /* ��ʽ����� */
    lcd_fill(x, y, x + 160, y + 16, WHITE);         /* ����ϴ�����(�����ʾ20���ַ�,20*8=160) */
    lcd_show_string(x, y, 160, 16, 16, buf, BLUE);  /* ��ʾ�ַ��� */
}

int main(void)
{
    uint8_t t = 0;
    float temperature;                              /* �¶�ֵ */
    short acc_data[3];                              /* ���ٶȴ�����ԭʼ���� */
    short gyro_data[3];                             /* ������ԭʼ���� */

    sys_cache_enable();                             /* ��L1-Cache */
    HAL_Init();                                     /* ��ʼ��HAL�� */
    sys_stm32_clock_init(160, 5, 2, 4);             /* ����ʱ��, 400Mhz */
    delay_init(400);                                /* ��ʱ��ʼ�� */
    usart_init(500000);                             /* ���ڳ�ʼ�� */
    usmart_init(200);                               /* ��ʼ��USMART */
    mpu_memory_protection();                        /* ������ش洢���� */
    led_init();                                     /* ��ʼ��LED */
    sdram_init();                                   /* ��ʼ��SDRAM */
    lcd_init();                                     /* ��ʼ��LCD */
		__HAL_RCC_CRC_CLK_ENABLE();		
		AI_Init();

  /* USER CODE END 2 */
    while (sh3001_init())                           /* ��ⲻ��SH3001 */
    {
        lcd_show_string(30, 130, 200, 16, 16, "SH3001 Check Failed!", RED);
        delay_ms(500);
        lcd_show_string(30, 130, 200, 16, 16, "Please Check!       ", RED);
        delay_ms(500);
        LED0_TOGGLE();                              /* �����˸ */
    }

    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "SH3001 TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 130, 200, 16, 16, " Temp :", RED);
    lcd_show_string(30, 150, 200, 16, 16, " ACC_X :", RED);
    lcd_show_string(30, 170, 200, 16, 16, " ACC_Y :", RED);
    lcd_show_string(30, 190, 200, 16, 16, " ACC_Z :", RED);
    lcd_show_string(30, 210, 200, 16, 16, " GYRO_X :", RED);
    lcd_show_string(30, 230, 200, 16, 16, " GYRO_Y :", RED);
    lcd_show_string(30, 250, 200, 16, 16, " GYRO_Z :", RED);
    uint32_t write_index = 0;
    while (1)
    {
        delay_ms(10);
        t++;

        if (t == 20)     /* 0.5�����Ҹ���һ���¶�/����ԭʼֵ */
        {
            temperature = sh3001_get_temperature();    /* ��ȡ�¶�ֵ */
            user_show_temprate(30, 130, temperature);
            
            sh3001_get_imu_compdata(acc_data, gyro_data);
            aiInData[write_index + 0] = (float) acc_data[0];
						aiInData[write_index + 1] = (float) acc_data[1];
						aiInData[write_index + 2] = (float) acc_data[2];
						write_index += 3;
						
						user_show_mag(30, 150, "ACC_X :", acc_data[0]);
            user_show_mag(30, 170, "ACC_Y :", acc_data[1]);
            user_show_mag(30, 190, "ACC_Z :", acc_data[2]);
            
            user_show_mag(30, 210, "GYRO_X:", gyro_data[0]);
            user_show_mag(30, 230, "GYRO_Y:", gyro_data[1]);
            user_show_mag(30, 250, "GYRO_Z:", gyro_data[2]);
						if (write_index == AI_NETWORK_IN_1_SIZE) {
							write_index = 0;

							printf("Running inference\r\n");
							AI_Run(aiInData, aiOutData);

        /* Output results */
							for (uint32_t i = 0; i < AI_NETWORK_OUT_1_SIZE; i++) {
								printf("%8.6f ", aiOutData[i]);
							}
							uint32_t class = argmax(aiOutData, AI_NETWORK_OUT_1_SIZE);
							printf(": %d - %s\r\n", (int) class, activities[class]);
						}
						
            
            t = 0;
            LED0_TOGGLE();
        }
    }
}


