/*
 * sensor_manager.c
 *
 *  Created on: May 19, 2026
 *      Author: Admin
 */

#include "sensor_manager.h"
#include "DHT11.h"

// Ngoại vi phần cứng cấu hình bên main.c
extern ADC_HandleTypeDef hadc1; // Bộ chuyển đổi ADC1 cho quang trở LDR
extern TIM_HandleTypeDef htim1; // Bộ định thời Timer 1 dùng tạo trễ micro giây cho DHT11

// Đối tượng lưu trữ dữ liệu đọc từ cảm biến DHT11
DHT11_DataTypedef DHT11;

// Lưu trữ mốc thời gian đọc cảm biến cuối cùng (để lập lịch phi tuần tự)
uint32_t last_readLDR = 0;
uint32_t last_readDHT = 0;

// Các biến toàn cục chứa thông số môi trường của nhà thông minh
uint16_t temp = 0;       // Nhiệt độ phòng (°C)
uint16_t humi = 0;       // Độ ẩm tương đối (%)
uint16_t ldrValue = 0;   // Giá trị cường độ ánh sáng đo từ ADC (0 - 4095)

// Vùng đệm nhận dữ liệu từ cảm biến Radar qua UART
uint8_t radarBuffer[64];
uint8_t radarIndex = 0;

// Các biến trạng thái phát hiện con người từ cảm biến Radar LD2410
uint8_t presenceState = 0; // Trạng thái hiện diện chung (1: Có người, 0: Không người)
uint8_t movingTarget = 0;  // Có mục tiêu chuyển động (1: Có, 0: Không)
uint8_t staticTarget = 0;  // Có mục tiêu đứng yên (1: Có, 0: Không)

// Khai báo các hàm nội bộ (private helper functions)
static uint16_t Read_LDR(void);
static void Parse_LD2410_Frame(uint8_t *buf, uint8_t len);

/**
  * @brief  Khởi tạo mô-đun quản lý cảm biến
  * @param  None
  * @retval None
  */
void sensorManagerInit(void)
{
    // Khởi tạo cảm biến DHT11 tại chân GPIOA PIN 1, sử dụng Timer 1 để delay
    DHT11_Init(&DHT11, &htim1, GPIOA, GPIO_PIN_1);
}

/**
  * @brief  Cập nhật định kỳ thông số các cảm biến (gọi trong vòng lặp while)
  * @param  None
  * @retval None
  */
void sensorManagerUpdate(void)
{
	// Đọc cảm biến ánh sáng LDR mỗi 1 giây (1000ms)
    if(HAL_GetTick() - last_readLDR >= 1000)
    {
        last_readLDR = HAL_GetTick();

        // Lấy mẫu ADC 10 lần liên tục rồi tính trung bình cộng để giảm nhiễu nguồn
        uint32_t sum = 0;
        for(int i = 0; i < 10; i++)
        {
            sum += Read_LDR();
        }

        ldrValue = sum / 10; // Cập nhật giá trị trung bình chính thức
    }

    // Đọc cảm biến nhiệt độ & độ ẩm DHT11 mỗi 2 giây (2000ms)
    if(HAL_GetTick() - last_readDHT >= 2000)
    {
        last_readDHT = HAL_GetTick();

        // Đọc dữ liệu thô từ DHT11 qua chuẩn 1-wire
        if(DHT11_Read_Data(&DHT11))
        {
            // Cập nhật giá trị nhiệt độ và độ ẩm vào các biến toàn cục nếu đọc thành công
            temp = DHT11.Temperature;
            humi = DHT11.Humidity;
        }
    }
}

/**
  * @brief  Đọc một giá trị ADC thô từ quang trở LDR kết nối chân PA0
  * @param  None
  * @retval Giá trị ADC thô (12-bit, 0 đến 4095)
  */
static uint16_t Read_LDR(void)
{
    HAL_ADC_Start(&hadc1); // Kích hoạt bộ ADC
    HAL_ADC_PollForConversion(&hadc1, 100); // Chờ quá trình chuyển đổi hoàn tất (timeout 100ms)
    uint16_t value = HAL_ADC_GetValue(&hadc1); // Lấy giá trị chuyển đổi
    HAL_ADC_Stop(&hadc1); // Dừng bộ ADC để tiết kiệm điện

    return value;
}

/**
  * @brief  Hàm callback xử lý ký tự nhận được từ ngắt UART của cảm biến Radar
  * @param  byte: Ký tự nhận được từ USART1
  * @retval None
  */
void Sensor_Radar_UART_Callback(uint8_t byte)
{
    // Đọc byte nhận được vào vùng đệm
    radarBuffer[radarIndex++] = byte;

    // Tránh tràn vùng đệm nếu dữ liệu sai định dạng
    if(radarIndex >= sizeof(radarBuffer))
    {
        radarIndex = 0;
    }

    /* Khung truyền của HLK-LD2410C thường kết thúc bằng chuỗi byte F8 F7 F6 F5
       Khi nhận đủ 4 byte cuối trùng khớp → Coi như nhận đủ gói dữ liệu hoàn chỉnh */
    if(radarIndex >= 4)
    {
        if(radarBuffer[radarIndex - 4] == 0xF8 &&
           radarBuffer[radarIndex - 3] == 0xF7 &&
           radarBuffer[radarIndex - 2] == 0xF6 &&
           radarBuffer[radarIndex - 1] == 0xF5)
        {
            // Tiến hành phân tích gói dữ liệu radar
            Parse_LD2410_Frame(radarBuffer, radarIndex);
            radarIndex = 0; // Reset chỉ số vùng đệm để đón gói tin tiếp theo
        }
    }
}

/**
  * @brief  Phân tích gói tin UART nhận được từ HLK-LD2410C
  * @param  buf: Vùng đệm chứa gói tin
  * @param  len: Độ dài gói dữ liệu
  * @retval None
  */
static void Parse_LD2410_Frame(uint8_t *buf, uint8_t len)
{
	// Gói tin cơ bản hiển thị trạng thái của LD2410C có độ dài tối thiểu là 20 byte
    if(len < 20) return;

    // Kiểm tra Header gói tin xem có khớp chuỗi F4 F3 F2 F1 hay không
    if(buf[0] != 0xF4 || buf[1] != 0xF3 || buf[2] != 0xF2 || buf[3] != 0xF1)
    {
        return;
    }

    /*
     * Byte thứ 8 (buf[8]) lưu trữ trạng thái mục tiêu (Target State):
     * 0x00 = Không có người hiện diện
     * 0x01 = Có mục tiêu chuyển động (moving target)
     * 0x02 = Có mục tiêu đứng yên (static target)
     * 0x03 = Có cả mục tiêu động và mục tiêu tĩnh
     */
    uint8_t targetState = buf[8];	// TargetState ~ byte thứ 9 trong frame

    // Đánh giá trạng thái và gán vào các biến hiển thị toàn cục
    presenceState = (targetState != 0x00) ? 1 : 0;
    movingTarget = (targetState == 0x01 || targetState == 0x03) ? 1 : 0;
    staticTarget = (targetState == 0x02 || targetState == 0x03) ? 1 : 0;
}
