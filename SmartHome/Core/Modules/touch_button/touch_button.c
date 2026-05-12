/*
 * touch_button.c
 *
 *  Created on: May 19, 2026
 *      Author: Admin
 */

#include "touch_button.h"
#include "main.h"
#include "device_control.h"

// Định nghĩa các chân GPIO kết nối nút chạm cảm ứng TTP223 (Mức HIGH khi chạm)
#define TTP223_FAN_PORT     GPIOA
#define TTP223_FAN_PIN      GPIO_PIN_6

#define TTP223_LED1_PORT    GPIOA
#define TTP223_LED1_PIN     GPIO_PIN_4

#define TTP223_LED2_PORT    GPIOA
#define TTP223_LED2_PIN     GPIO_PIN_5

// Lưu trạng thái của nút chạm cảm ứng ở chu kỳ trước để nhận diện sườn xung thay đổi
uint8_t lastFanTouchState = 1;
uint8_t lastLed1TouchState = 1;
uint8_t lastLed2TouchState = 1;

// Lưu thời điểm chống rung (Debounce) của nút chạm
uint32_t lastFanDebounceTime = 0;
uint32_t lastLed1DebounceTime = 0;
uint32_t lastLed2DebounceTime = 0;

// Các biến lưu thời điểm chạm và bộ đếm số lần nhấn liên tiếp để nhận diện Double Tap (chạm đúp)
uint32_t lastFanTapTime = 0;
uint8_t fanTapCount = 0;

uint32_t lastLed1TapTime = 0;
uint8_t led1TapCount = 0;

uint32_t lastLed2TapTime = 0;
uint8_t led2TapCount = 0;

void touchButtonUpdate(void)
{
    // Đọc trạng thái số tức thời từ 3 chân nút nhấn cảm ứng
	uint8_t fanTouchState = HAL_GPIO_ReadPin(TTP223_FAN_PORT, TTP223_FAN_PIN);
	uint8_t led1TouchState = HAL_GPIO_ReadPin(TTP223_LED1_PORT, TTP223_LED1_PIN);
	uint8_t led2TouchState = HAL_GPIO_ReadPin(TTP223_LED2_PORT, TTP223_LED2_PIN);

	// ====================================================
	// XỬ LÝ NÚT NHẤN QUẠT (FAN BUTTON)
	// ====================================================
	if(fanTouchState != lastFanTouchState) // Trạng thái thay đổi (chạm vào hoặc thả ra)
	{
		if(HAL_GetTick() - lastFanDebounceTime >= 100) 	// Chống nhiễu rung tiếp điểm 100ms
		{
			lastFanDebounceTime = HAL_GetTick();
			
			// Phát hiện chạm đúp: khoảng cách giữa 2 lần chạm liên tiếp <= 400ms
			if(HAL_GetTick() - lastFanTapTime <= 400)
			{
				fanTapCount++;
			}
			else
			{
				fanTapCount = 1;	// Quá 400ms -> Reset tính chạm lần đầu
			}

			lastFanTapTime = HAL_GetTick();

			if(fanTapCount == 2 && fanTouchState == GPIO_PIN_SET)
			{
				fanMode = 1;	// Chạm kép: Kích hoạt chế độ tự động (AUTO Mode)
				fanTapCount = 0;
			}
			else if(fanTapCount == 1)
			{
				fanMode = 0;	// Chạm đơn: Chuyển sang chế độ bằng tay (MANUAL Mode)
				fanState = !fanState; // Đảo trạng thái bật tắt quạt
			}
			lastFanTouchState = fanTouchState; // Lưu trạng thái nút chạm
		}
	}

	// ====================================================
	// XỬ LÝ NÚT NHẤN ĐÈN SÂN VƯỜN (LED1 BUTTON)
	// ====================================================
	if(led1TouchState != lastLed1TouchState)
	{
		if(HAL_GetTick() - lastLed1DebounceTime >= 100) 	// Chống rung 100ms
		{
			lastLed1DebounceTime = HAL_GetTick();
			if(HAL_GetTick() - lastLed1TapTime <= 400)	// Nhấn đúp
			{
				led1TapCount++;
			}
			else
			{
				led1TapCount = 1;	// Reset
			}

			lastLed1TapTime = HAL_GetTick();

			if(led1TapCount == 2 && led1TouchState == GPIO_PIN_SET)
			{
				led1Mode = 1;	// Chạm kép: Chuyển sang AUTO
				led1TapCount = 0;
			}
			else if(led1TapCount == 1)
			{
				led1Mode = 0;	// Chạm đơn: Chuyển sang MANUAL
				led1State = !led1State; // Đảo trạng thái đèn LED1
			}
			lastLed1TouchState = led1TouchState;
		}
	}

	// ====================================================
	// XỬ LÝ NÚT NHẤN ĐÈN TRONG NHÀ (LED2 BUTTON)
	// ====================================================
	if(led2TouchState != lastLed2TouchState)
	{
		if(HAL_GetTick() - lastLed2DebounceTime >= 100) 	// Chống rung 100ms
		{
			lastLed2DebounceTime = HAL_GetTick();
			if(HAL_GetTick() - lastLed2TapTime <= 400)	// Nhấn đúp
			{
				led2TapCount++;
			}
			else
			{
				led2TapCount = 1;	// Reset
			}

			lastLed2TapTime = HAL_GetTick();

			if(led2TapCount == 2 && led2TouchState == GPIO_PIN_SET)
			{
				led2Mode = 1;	// Chạm kép: Chuyển sang AUTO
				led2TapCount = 0;
			}
			else if(led2TapCount == 1)
			{
				led2Mode = 0;	// Chạm đơn: Chuyển sang MANUAL
				led2State = !led2State; // Đảo trạng thái đèn LED2
			}
			lastLed2TouchState = led2TouchState;
		}
	}
}
