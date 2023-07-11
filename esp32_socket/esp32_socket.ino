#include <WiFi.h>
#include <WiFiClient.h>
#include <AsyncUDP.h>
#include "config.h"


// IP server
char* host = 0;

// Chân đầu ra sử dụng là GPIO2
const int IN_PIN = GPIO_NUM_2;

/*
  Khởi tạo một biến client từ Class WiFiClient để truyền nhận dữ liệu dùng giao thức TCP/IP
  Sử dụng thư viện AsyncUDP để nhận dữ liệu từ broadcast tại mạng cục bộ
*/
WiFiClient client;
AsyncUDP udp;

// Trạng thái đầu ra của chân điều khiển
int status = -1;

void connecting() {
  /*
    - Kết nối tới server
  */
  Serial.print("Connecting to server...");
  while (!client.connect(host, port)) {
    Serial.println("Connection to host failed");
    delay(500);
  }
  Serial.println("Connected to server successful");
  client.print("đèn 1");
}

void setIp() {
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
      });
    }
    if (host != 0) {
      break;
    }
  }
}

void setup() {
  // Thiết lập thông số của ESP32
  // Tốc độ truyền
  Serial.begin(115200);
  delay(2000);

  // Chọn chân điều khiển đầu ra
  pinMode(IN_PIN, OUTPUT);


  /* 
    Thiết lập kết nối tới wifi với tên(ssid) và mật khẩu (password)
  */
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  /*
    Kiểm tra trạng thái kết nối
    Nếu WiFi.status() = WL_CONNECTED thì đã có kết nối tới WiFi
    WL_CONNECTED là hằng số có giá trị là 1
    Nếu WiFi.status() = 0 thì chưa có kết nối tới wifi
  */
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Hiển thị các thông số IP của ESP32 tại mạng cục bộ
  Serial.println();
  Serial.print("WiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  setIp();

  connecting();
  // Kiểm tra đã có kết nối tới server hay chưa
  while (client.connected()) {

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
        digitalWrite(IN_PIN, HIGH);
      } else {
        status = 0;
        digitalWrite(IN_PIN, LOW);
      }
      Serial.println(status);
    }

  }
  // Ngắt kết nối tới server
  Serial.println("Disconnecting....");
  client.stop();
  delay(3000);
}
