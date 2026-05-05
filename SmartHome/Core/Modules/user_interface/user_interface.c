/*
 * user_interface.c
 *
 *  Created on: May 20, 2026
 *      Author: Admin
 */

#include "user_interface.h"
#include "main.h"
#include "sensor_manager.h"
#include "device_control.h"

#include <stdio.h>
#include "ssd1306.h"
#include "ssd1306_fonts.h"

// Thời điểm làm mới màn hình OLED cuối cùng
static uint32_t last_oled = 0;

void userInterfaceInit(void) {
	ssd1306_Init(); // Khởi tạo driver màn hình OLED SSD1306 qua I2C

	ssd1306_Fill(Black);	// Xóa sạch bộ đệm hiển thị màn hình (màu đen)

	// Vẽ chuỗi tiêu đề SMART HOME chữ cỡ lớn
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString("SMART HOME", Font_11x18, White); // Vẽ text vào RAM buffer

	// Vẽ thông tin chào mừng
	ssd1306_SetCursor(0, 24);
	ssd1306_WriteString("Welcome to", Font_7x10, White);

	ssd1306_SetCursor(0, 35);
	ssd1306_WriteString("BLACK HOME!", Font_7x10, White);

	ssd1306_UpdateScreen(); // Đẩy bộ đệm RAM lên màn hình OLED vật lý

	HAL_Delay(2000); // Đợi 2 giây màn hình chào trước khi vào màn hình chính
}

void userInterfaceUpdate(void) {
	// CẬP NHẬT MÀN HÌNH OLED ĐỊNH KỲ MỖI 500MS
	if (HAL_GetTick() - last_oled >= 500) {
		last_oled = HAL_GetTick();
		char buffer[64]; // Vùng nhớ đệm định dạng chuỗi

		ssd1306_Fill(Black);	// Xóa sạch màn hình

		// 1. Hiển thị Nhiệt độ & Độ ẩm (T: 28*C - H: 65%)
		sprintf(buffer, "T: %d*C - H: %d%%", temp, humi);
		ssd1306_SetCursor(0, 0);
		ssd1306_WriteString(buffer, Font_7x10, White);

		// 2. Hiển thị Giá trị cường độ ánh sáng (LDR: 1200)
		sprintf(buffer, "LDR: %d", ldrValue);
		ssd1306_SetCursor(0, 12);
		ssd1306_WriteString(buffer, Font_7x10, White);

		// 3. Hiển thị trạng thái phát hiện Radar (RADAR: DETECTED / NO DETECT)
		sprintf(buffer, "RADAR: %s", presenceState ? "DETECTED" : "NO DETECT");
		ssd1306_SetCursor(0, 24);
		ssd1306_WriteString(buffer, Font_7x10, White);

		// Đường kẻ nét ngang ngăn cách
		sprintf(buffer, "---------------------");
		ssd1306_SetCursor(0, 36);
		ssd1306_WriteString(buffer, Font_6x8, White);

		// 4. Trạng thái thiết bị và chế độ của LED1, LED2 (L1:ON(A) - L2:OFF(M))
		sprintf(buffer, "L1:%s(%s) - L2:%s(%s)", led1State ? "ON" : "OFF",
				led1Mode ? "A" : "M", led2State ? "ON" : "OFF",
				led2Mode ? "A" : "M");
		ssd1306_SetCursor(0, 46);
		ssd1306_WriteString(buffer, Font_6x8, White);

		// 5. Trạng thái thiết bị và chế độ của Quạt (FAN:ON(A))
		sprintf(buffer, "FAN:%s(%s)", fanState ? "ON" : "OFF",
				fanMode ? "A" : "M");
		ssd1306_SetCursor(0, 56);
		ssd1306_WriteString(buffer, Font_6x8, White);

		ssd1306_UpdateScreen(); // Cập nhật màn hình hiển thị
	}
}
