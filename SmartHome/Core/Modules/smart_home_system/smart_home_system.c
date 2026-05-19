/*
 * smart_home_system.c
 *
 *  Created on: May 19, 2026
 *      Author: Admin
 */

#include "smart_home_system.h"
#include "sensor_manager.h"
#include "touch_button.h"
#include "device_control.h"
#include "user_interface.h"

/**
  * @brief  Khởi tạo tất cả các phân hệ của dự án Smart Home (gọi trước while(1) ở main.c)
  * @param  None
  * @retval None
  */
void smartHome_Init(void)
{
	sensorManagerInit(); // Khởi tạo bộ cảm biến DHT11 và Timer trễ
	userInterfaceInit(); // Khởi tạo màn hình OLED SSD1306 hiển thị Welcome
}

/**
  * @brief  Cập nhật định kỳ các phân hệ của Smart Home (gọi tuần tự trong while(1) ở main.c)
  * @param  None
  * @retval None
  */
void smartHome_Run(void)
{
	sensorManagerUpdate(); // Đọc và lọc nhiễu các cảm biến (LDR, DHT11, Radar)
	touchButtonUpdate();   // Quét trạng thái và xử lý chống rung các nút chạm cảm ứng
	deviceControlUpdate(); // Chạy logic máy trạng thái quyết định bật tắt rơ-le thiết bị
	userInterfaceUpdate(); // Hiển thị các thông số cảm biến và thiết bị lên màn hình OLED
}

