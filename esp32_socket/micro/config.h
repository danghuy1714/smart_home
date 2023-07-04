// #ifndef _CONFIG_H_
// #define _CONFIG_H_
/*
  Cấu hình mạng Wifi
  Nhận dữ liệu từ broadcast tại cổng 5005, sử dụng giao thức udp
  Sử dụng giao thức TCP/IP để kết nối tới server
*/
// extern char *ssid = "VAN";
// extern char *password = "0912549900";
// const char* ssid = "Galaxy A23";
// const char* password = "123456789";
// const char* ssid = "SET407-2.4G";
// const char* password = "2135586831314306498S@t";
extern char *ssid = "KIEN2";
extern char *password = "0947091818";

// port server
extern uint16_t port = 1234;

// port broadcast
extern const int udp_port = 5005;
// #endif