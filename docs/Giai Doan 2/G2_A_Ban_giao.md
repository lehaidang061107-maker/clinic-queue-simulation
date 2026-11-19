# Bàn giao Giai đoạn 2 - Thành viên A
**Phụ trách:** `src/LinkedQueue.cpp`, `src/ClinicRoom.cpp`, `tests/test_a.cpp`

---

## 1. Chức năng đã cài đặt
- **`LinkedQueue` (`src/LinkedQueue.cpp`):**
  - Cài đặt đầy đủ `LinkedQueue()`, `~LinkedQueue()`, `enqueue()`, `dequeue()`, `peek()`, `isEmpty()`, `size()`, `clear()`.
  - Đảm bảo khi lấy phần tử cuối cùng thì cả `front_` và `rear_` đều được đặt về `nullptr`.
  - Hàm `clear()` chỉ giải phóng các `Node`, không `delete` con trỏ `Patient*`.
- **`ClinicRoom` (`src/ClinicRoom.cpp`):**
  - Cài đặt cấu hình phòng `configure()`, hàng chờ thường `normal_` (`LinkedQueue`), hàng chờ ưu tiên `priority_` (`PriorityHeap`) và con trỏ `current_`.
  - `enqueue()`: khi bật ưu tiên (`usePriority == true`), chỉ đưa bệnh nhân mức `0` (`Emergency`) và mức `1` (`Preferential`) vào `priority_`; mức `2` (`Normal`) và chế độ tắt ưu tiên đưa vào `normal_`.
  - `startNext()`: bỏ qua nếu phòng đang bận; khi bật ưu tiên lấy từ `priority_` trước `normal_`, khi tắt ưu tiên lấy từ `normal_` trước; cập nhật `status = InService`, `assignedRoomId`, `startTime` và `finishTime`.
  - `finishIfDue()`: khi `minute >= finishTime`, chuyển `status = Completed`, trả về bệnh nhân vừa xong và đưa `current_ = nullptr`.
  - `reset()`: dọn sạch cả 2 hàng chờ và đặt `current_ = nullptr`, giữ nguyên dữ liệu `Patient`.

## 2. Cách biên dịch và chạy kiểm thử
Chạy lệnh biên dịch bằng trình biên dịch C++17 (sau khi có đủ `Patient.cpp` và `PriorityHeap.cpp`):

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude tests/test_a.cpp src/LinkedQueue.cpp src/ClinicRoom.cpp src/Patient.cpp src/PriorityHeap.cpp -o test_a
./test_a