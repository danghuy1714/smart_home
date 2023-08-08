#include <driver/i2s.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <AsyncUDP.h>
#include "D:\Source\Python\vxl\esp32_socket\config.h"

// Cấu hình cách chân của imnp441
#define I2S_WS GPIO_NUM_25
#define I2S_SD GPIO_NUM_33
#define I2S_SCK GPIO_NUM_32


// Cấu hình giao thức I2S
#define I2S_SAMPLE_RATE 16000
#define I2S_SAMPLE_BITS 16
#define buffer_count 64
#define buffer_len 1024

// thiết lập cổng i2s
#define I2S_PORT I2S_NUM_0


// Tạo mảng để ghi dữ liệu bit(âm thanh) thừ INMP441
#define bufferLen 16 * 1024
byte sBuffer[bufferLen];

// IP server
char *host = 0;

// Chân đầu ra sử dụng là GPIO2
const int IN_PIN = GPIO_NUM_2;

hw_timer_t * timer = NULL;

int status = 0;

WiFiClient client;
AsyncUDP udp;


void i2s_install();
void i2s_setpin();
void connectWifi();
void connectPort();
void setIp();
void checkStatus();
// void checkStatus();
void sendAudio();
void updateStatus();


void setup() {
  // Set up Serial Monitor
  Serial.begin(115200);

  // Chọn chân điều khiển đầu ra
  pinMode(IN_PIN, OUTPUT);
  digitalWrite(IN_PIN, LOW);
  connectWifi();
  setIp();
  connectPort();

  // Thiết lập I2S
  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);

}
void loop() {
  int i = 0;
  for (i = 0; i < 15; ++i) {
    sendAudio();
  }
  checkStatus();
}

void connectWifi() {
    Serial.print("Connecting to Wi-Fi...");
    WiFi.begin(ssid, password);

    /*
        Kiểm tra trạng thái kết nối
        Nếu WiFi.status() = WL_CONNECTED thì đã có kết nối tới WiFi
        WL_CONNECTED là hằng số có giá trị là 1
        Nếu WiFi.status() = 0 thì chưa có kết nối tới wifi
    */
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
    }

    // Hiển thị các thông số IP của ESP32 tại mạng cục bộ
    Serial.println();
    Serial.print("WiFi connected. IP address: ");
    Serial.println(WiFi.localIP());
}

void connectPort() {
    /*
    - Kết nối tới server
    */
    Serial.print("Connecting to server...");
    
    while (!client.connect(host, port)) {
        Serial.println(host);
        Serial.println(port);
        Serial.println("Connection to host failed");
    }
    Serial.println("Connected to server successful");
    client.print("đèn-micro1");
}

void setIp() {
    while (true) {
    if (udp.listen(udp_port)) {
      // Serial.println("UDP Listening on IP: " + WiFi.localIP().toString() + " Port: " + udp_port);
      udp.onPacket([](AsyncUDPPacket packet) {
        char *c = (char*)packet.data();
        host = &packet.remoteIP().toString()[0];
      });
    }
    if (host != 0) {
      break;
    }
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
    .dma_buf_count = buffer_count,
    .dma_buf_len = buffer_len,
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

void checkStatus() {
  int i = 0;
  while (client.connected() && i < 300000 ) {
    // Kiểm ra có tin mới gửi từ server không
    if (client.available()) {
      // Đọc dữ liệu từ server
      String data = client.readStringUntil('\n');
      Serial.print("Received data: ");
      Serial.println(data);

      /*
        Thay đổi trạng thái của chân đầu ra
      */
      if (data[0] == '1') {
        status = 1;
      } else {
        // digitalWrite(IN_PIN, LOW);
        status = 0;
      }
      break;
    }
    ++i;
  }
  updateStatus();
}

void sendAudio() {
  // while (true) {
    size_t bytesIn = 0;
    esp_err_t result = i2s_read(I2S_PORT, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);

    if (result == ESP_OK && bytesIn > 0) {
      // Send data over the socket
      client.write((const byte*)sBuffer, bytesIn);
      client.flush();
      Serial.println("Complete send");
    }
  // }
}

void updateStatus() {
  if (status == 0) {
    digitalWrite(IN_PIN, LOW);
  } else {
    digitalWrite(IN_PIN, HIGH);
  }
}