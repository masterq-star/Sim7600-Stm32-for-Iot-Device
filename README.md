# 🚀 Thiết bị Giám sát hệ đồng hồ điện 3 phase (1 phase) bẳng giao thức mqtt và modbus rtu
> Mô tả ngắn gọn về dự án (1-2 câu)

![GitHub stars](https://img.shields.io/github/stars/yourusername/projectname?style=flat-square)
![GitHub forks](https://img.shields.io/github/forks/yourusername/projectname?style=flat-square)
![GitHub license](https://img.shields.io/github/license/yourusername/projectname?style=flat-square)

---

## 📖 Mô tả dự án
Dự án này phát triển một thiết bị giám sát đồng hồ điện 3 phase (hoặc 1 phase), sử dụng giao thức Modbus RTU để lấy dữ liệu từ công tơ điện và MQTT để truyền dữ liệu lên server. Hệ thống sử dụng vi điều khiển STM32 kết hợp với module SIM7600 để gửi dữ liệu qua 4G LTE đến một nền tảng IoT (MQTT broker hoặc cloud server).

Thiết bị có thể được ứng dụng trong giám sát tiêu thụ điện từ xa, cảnh báo khi có bất thường và tối ưu hóa quản lý điện năng.

### 🎯 **Tính năng chính**
✅ Đọc dữ liệu từ đồng hồ điện qua Modbus RTU (dòng điện, điện áp, công suất, hệ số công suất, tần số, v.v.)
✅ Gửi dữ liệu lên server qua MQTT sử dụng SIM7600 (4G LTE)
✅ Lưu trữ dữ liệu tạm thời khi mất kết nối và tự động gửi lại khi có mạng
✅ Cảnh báo tức thời khi có bất thường (quá tải, mất phase, điện áp thấp/cao)
✅ Giao diện giám sát real-time trên dashboard IoT
✅ Nâng cấp firmware từ xa (OTA) qua MQTT

## 🏗 Cấu trúc dự án
🔧 Công nghệ sử dụng
Công nghệ	Ứng dụng
STM32F4	Vi điều khiển chính
Modbus RTU	Giao tiếp với đồng hồ điện
SIM7600 4G LTE	Kết nối Internet và gửi dữ liệu qua MQTT
MQTT	Giao tiếp giữa thiết bị và server
Node-RED	Xây dựng dashboard hiển thị dữ liệu
Altium Designer	Thiết kế mạch phần cứng