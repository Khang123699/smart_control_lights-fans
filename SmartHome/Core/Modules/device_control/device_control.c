/*
 * device_control.c
 *
 *  Created on: May 19, 2026
 *      Author: Admin
 */

#include "device_control.h"
#include "sensor_manager.h"

// Định nghĩa chân phần cứng kết nối các Relay điều khiển thiết bị (Active Low - Kích mức thấp)
#define RELAY1_FAN_PORT     GPIOB
#define RELAY1_FAN_PIN      GPIO_PIN_0

#define RELAY2_LED1_PORT    GPIOB
#define RELAY2_LED1_PIN     GPIO_PIN_1

#define RELAY3_LED2_PORT    GPIOB
#define RELAY3_LED2_PIN     GPIO_PIN_10

// Cấu hình các ngưỡng cảm biến ánh sáng LDR mặc định
uint16_t dark_Threshold = 2500;   // Ngưỡng giá trị LDR để coi là trời tối (Bật đèn)
uint16_t bright_Threshold = 1500; // Ngưỡng giá trị LDR để coi là trời sáng (Tắt đèn)

// Cấu hình ngưỡng nhiệt độ bật tắt quạt thông gió mặc định (Tránh hiện tượng dao động bật tắt liên tục)
uint16_t tempOnFan = 30;  // Nhiệt độ bật quạt (>= 30°C)
uint16_t tempOffFan = 28; // Nhiệt độ tắt quạt (<= 28°C)

// Các biến toàn cục phản ánh trạng thái bật/tắt thiết bị vật lý (1: Bật, 0: Tắt)
uint8_t led1State = 0;
uint8_t led2State = 0;
uint8_t fanState = 0;

// Các biến toàn cục phản ánh chế độ điều khiển thiết bị (1: AUTO, 0: MANUAL)
uint8_t led1Mode = 1;
uint8_t led2Mode = 1;
uint8_t fanMode = 1;

/**
  * @brief  Cập nhật trạng thái điều khiển rơ-le thiết bị ngoại vi dựa trên cảm biến và chế độ
  * @param  None
  * @retval None
  */
void deviceControlUpdate(void)
{
	// ----------------------------------------------------
	// LED1: Đèn sân vườn (Chỉ dựa vào cảm biến ánh sáng LDR)
	// ----------------------------------------------------
    if(led1Mode) // Chỉ xử lý khi thiết bị đang ở chế độ tự động (AUTO)
    {
        if(ldrValue >= dark_Threshold) // Cường độ sáng giảm, điện trở quang tăng -> trời tối
        {
            led1State = 1; // Bật đèn sân vườn
        }
        else if(ldrValue <= bright_Threshold) // Trời sáng
        {
            led1State = 0; // Tắt đèn sân vườn
        }
    }

    // Ghi trạng thái vật lý ra rơ-le LED1 (Relay kích mức thấp: RESET = ON, SET = OFF)
    HAL_GPIO_WritePin(RELAY2_LED1_PORT, RELAY2_LED1_PIN,
                      led1State ? GPIO_PIN_RESET : GPIO_PIN_SET);

	// ----------------------------------------------------
	// LED2: Đèn trong nhà (Kết hợp cảm biến ánh sáng LDR và Radar hiện diện)
	// ----------------------------------------------------
    if(led2Mode) // Chỉ xử lý khi thiết bị đang ở chế độ tự động (AUTO)
    {
        // Điều kiện bật: Trời tối VÀ có người hiện diện trong phòng
        if(ldrValue >= dark_Threshold && presenceState)
        {
            led2State = 1; // Bật đèn trong nhà
        }
        // Điều kiện tắt: Trời sáng HOẶC không có người hiện diện
        else if(ldrValue <= bright_Threshold || !presenceState)
        {
            led2State = 0; // Tắt đèn trong nhà
        }
    }

    // Ghi trạng thái vật lý ra rơ-le LED2
    HAL_GPIO_WritePin(RELAY3_LED2_PORT, RELAY3_LED2_PIN,
                      led2State ? GPIO_PIN_RESET : GPIO_PIN_SET);

	// ----------------------------------------------------
	// FAN: Quạt thông gió (Kết hợp nhiệt độ DHT11 và Radar hiện diện)
	// ----------------------------------------------------
    if(fanMode) // Chỉ xử lý khi thiết bị đang ở chế độ tự động (AUTO)
    {
        // Điều kiện bật: Nhiệt độ vượt ngưỡng bật VÀ có người hiện diện
        if(temp >= tempOnFan && presenceState)
        {
            fanState = 1; // Bật quạt thông gió
        }
        // Điều kiện tắt: Nhiệt độ hạ dưới ngưỡng tắt HOẶC không có người hiện diện
        else if(temp <= tempOffFan || !presenceState)
        {
            fanState = 0; // Tắt quạt thông gió
        }
    }

    // Ghi trạng thái vật lý ra rơ-le Quạt thông gió
    HAL_GPIO_WritePin(RELAY1_FAN_PORT, RELAY1_FAN_PIN,
                      fanState ? GPIO_PIN_RESET : GPIO_PIN_SET);
}
