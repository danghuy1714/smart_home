/*
  Module micro: thiết lập các kết nối và truyền dữ liêu từ INMP441 tới ESP32 và tới server
*/

#include <driver/i2s.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <AsyncUDP.h>
#include "config.h"


// Cấu hình cách chân của imnp441
#define I2S_WS GPIO_NUM_25
#define I2S_SD GPIO_NUM_33
#define I2S_SCK GPIO_NUM_32


// Cấu hình giao thức I2S
#define I2S_SAMPLE_RATE 16000
#define I2S_SAMPLE_BITS 16

// thiết lập cổng i2s
#define I2S_PORT I2S_NUM_0

// IP server
char *host = 0;

/*
  Khởi tạo một biến client từ Class WiFiClient để truyền nhận dữ liệu dùng giao thức TCP/IP
  Sử dụng thư viện AsyncUDP để nhận dữ liệu từ broadcast tại mạng cục bộ
*/
WiFiClient client;
AsyncUDP udp;

// Tạo mảng để ghi dữ liệu bit(âm thanh) thừ INMP441
#define bufferLen 16 * 1024
byte sBuffer[bufferLen];

void i2s_install();
void i2s_setpin();


void setup() {

   /* 
    Thiết lập kết nối tới wifi với tên(ssid) và mật khẩu (password)
  */
  // Set up Serial Monitor
  Serial.begin(115200);


  // Kết nối tới wifi
  WiFi.begin(ssid, password);

  /*
    Kiểm tra trạng thái kết nối
    Nếu WiFi.status() = WL_CONNECTED thì đã có kết nối tới WiFi
    WL_CONNECTED là hằng số có giá trị là 1
    Nếu WiFi.status() = 0 thì chưa có kết nối tới wifi
  */
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // Hiển thị các thông số IP của ESP32 tại mạng cục bộ
  Serial.println();
  Serial.print("WiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

   /* 
    - Sử dụng AsyncUDP với udp_port là port mà bản tin broadcast đã gửi lên để nhận bản tin
    - Sử dụng phương thức onPacket để đăng ký một hàm xử lý sự kiện
    [](AsyncUDPPacket packet): khởi tạo hàm lambda với tham số là packet dùng để xử lý các bản tin broadcast
    - Nhận IP từ server đã gửi broadcast để dùng làm host
  */
  while (true) {
    if (udp.listen(udp_port)) {
      // Serial.println("UDP Listening on IP: " + WiFi.localIP().toString() + " Port: " + udp_port);
      udp.onPacket([](AsyncUDPPacket packet) {
        char *c = (char*)packet.data();
        host = &packet.remoteIP().toString()[0];
        // port = (uint16_t)toInt(c);
      });
    }
    if (host != 0) {
      break;
    }
  }
  
  /*
    - Kết nối tới server
  */
  Serial.print("Connecting to server...");
  while (!client.connect(host, port)) {
    Serial.println("Connection to host failed");
    delay(1000);
  }
  Serial.println("Connected to server successful");
  // Xác định tên thiết bị
  client.print("micro");

  delay(1000);

  // Thiết lập I2S
  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);


  delay(500);
}

void loop() {

  size_t bytesIn = 0;
  esp_err_t result = i2s_read(I2S_PORT, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);

  if (result == ESP_OK && bytesIn > 0) {
    // Send data over the socket
    client.write((const byte*)sBuffer, bytesIn);
    client.flush();
    Serial.println("Complete send");
  }
}

void i2s_install() {
  // Thiết lập các xử lý của I2S
  const i2s_config_t i2s_config = {
    /*
      .mode chế độ hoạt động của i2s. MASTER là chế độ điều khiển đồng bộ, RX là chế độ nhận dữ liệu
    */
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    /*
      .sample_rate tốc độ lấy mẫu âm tham 16000 Hz hoặc 44100 Hz
    */
    .sample_rate = I2S_SAMPLE_RATE,
    /*
      .bits_per_sample: Số bit mô tả mỗi mẫu âm thanh trong giao thức I2S.
      Đoạn code đang sử dụng một kiểu dữ liệu được chuyển đổi từ I2S_SAMPLE_BITS.
    */
    .bits_per_sample = i2s_bits_per_sample_t(I2S_SAMPLE_BITS),
    /*
      .bits_per_sample: Số bit mô tả mỗi mẫu âm thanh trong giao thức I2S. 
      Đoạn code đang sử dụng một kiểu dữ liệu được chuyển đổi từ I2S_SAMPLE_BITS.
    */
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    /*
      .communication_format: Định dạng giao tiếp của dữ liệu âm thanh trong giao thức I2S. 
      Đoạn code đang sử dụng I2S_COMM_FORMAT_I2S và I2S_COMM_FORMAT_I2S_MSB, 
      cho biết định dạng I2S và dữ liệu được truyền dưới dạng MSB (Most Significant Bit) trước.
    */
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    /*
      .intr_alloc_flags: Cờ chỉ định cách phân bổ ngắt (interrupt) cho giao thức I2S. 
      Trong đoạn code, nó được đặt thành 0, có nghĩa là không sử dụng phân bổ ngắt đặc biệt.
    */
    .intr_alloc_flags = 0,
    /*
      .dma_buf_count và .dma_buf_len: Số lượng và độ dài của các bộ đệm DMA (Direct Memory Access) 
      được sử dụng cho việc truyền và nhận dữ liệu âm thanh. 
      Trong đoạn code, có 64 bộ đệm với độ dài 1024 bytes cho mỗi bộ đệm.
    */
    .dma_buf_count = 64,
    .dma_buf_len = 1024,
    /*
      .use_apll: Cờ chỉ định việc sử dụng APLL (Audio PLL) cho đồng bộ hóa âm thanh. 
      Trong đoạn code, nó được đặt thành 1 để sử dụng APLL.
    */
    .use_apll = 1
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

void i2s_setpin() {
  // Thiết lập các chân kết nối giao thức i2s
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);
}

