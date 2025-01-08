/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-09-06
 * @brief       六轴传感器-读取原始数据 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 阿波罗 H743开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
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


/**
 * @brief       显示原始数据
 * @param       x, y : 坐标
 * @param       title: 标题
 * @param       val  : 值
 * @retval      无
 */
void user_show_mag(uint16_t x, uint16_t y, char *title, int16_t val)
{
    char buf[20];

    sprintf(buf,"%s%d", title, val);                /* 格式化输出 */
    lcd_fill(x, y, x + 160, y + 16, WHITE);         /* 清除上次数据(最多显示20个字符,20*8=160) */
    lcd_show_string(x, y, 160, 16, 16, buf, BLUE);  /* 显示字符串 */
}

/**
 * @brief       显示温度
 * @param       x, y : 坐标
 * @param       temp : 温度
 * @retval      无
 */
void user_show_temprate(uint16_t x, uint16_t y, float temp)
{
    char buf[20];

    sprintf(buf,"Temp:%2.1fC", temp);               /* 格式化输出 */
    lcd_fill(x, y, x + 160, y + 16, WHITE);         /* 清除上次数据(最多显示20个字符,20*8=160) */
    lcd_show_string(x, y, 160, 16, 16, buf, BLUE);  /* 显示字符串 */
}

int main(void)
{
    uint8_t t = 0;
		uint8_t count=0;
    float temperature;                              /* 温度值 */
    short acc_data[3];                              /* 加速度传感器原始数据 */
    short gyro_data[3];                             /* 陀螺仪原始数据 */

    sys_cache_enable();                             /* 打开L1-Cache */
    HAL_Init();                                     /* 初始化HAL库 */
    sys_stm32_clock_init(160, 5, 2, 4);             /* 设置时钟, 400Mhz */
    delay_init(400);                                /* 延时初始化 */
    usart_init(500000);                             /* 串口初始化 */
    usmart_init(200);                               /* 初始化USMART */
    mpu_memory_protection();                        /* 保护相关存储区域 */
    led_init();                                     /* 初始化LED */
    sdram_init();                                   /* 初始化SDRAM */
    lcd_init();                                     /* 初始化LCD */

    while (sh3001_init())                           /* 检测不到SH3001 */
    {
        lcd_show_string(30, 130, 200, 16, 16, "SH3001 Check Failed!", RED);
        delay_ms(500);
        lcd_show_string(30, 130, 200, 16, 16, "Please Check!       ", RED);
        delay_ms(500);
        LED0_TOGGLE();                              /* 红灯闪烁 */
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
		count=0;
    while (1)
    {
        delay_ms(10);
        t++;
				

        if (t == 50)     /* 0.2秒左右更新一次温度/六轴原始值 */
        {
            temperature = sh3001_get_temperature();    /* 读取温度值 */
            user_show_temprate(30, 130, temperature);
						
            
            sh3001_get_imu_compdata(acc_data, gyro_data);
            count=count+1;
					  if(count%3==0)
						{
								printf("\n");
						}
            user_show_mag(30, 150, "ACC_X :", acc_data[0]);
            user_show_mag(30, 170, "ACC_Y :", acc_data[1]);
            user_show_mag(30, 190, "ACC_Z :", acc_data[2]);
            
            user_show_mag(30, 210, "GYRO_X:", gyro_data[0]);
            user_show_mag(30, 230, "GYRO_Y:", gyro_data[1]);
            user_show_mag(30, 250, "GYRO_Z:", gyro_data[2]);
            
            t = 0;
            LED0_TOGGLE();
        }
    }
}


