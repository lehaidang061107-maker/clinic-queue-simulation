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
Từ thư mục gốc:

```bash
make test-a-queue   # chỉ LinkedQueue, chạy được ngay
make test-a         # LinkedQueue + ClinicRoom, cần PriorityHeap thật của B
```

Lệnh tương đương:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude -DTEST_A_QUEUE_ONLY tests/test_a.cpp src/LinkedQueue.cpp src/Patient.cpp -o build/test_a_queue
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude tests/test_a.cpp src/LinkedQueue.cpp src/ClinicRoom.cpp src/PriorityHeap.cpp src/Patient.cpp -o build/test_a
```

## 3. Kết quả kiểm tra

Môi trường: macOS 26.5, Apple clang 16.0.0 (arm64), `make`. Ngày chạy: 2026-11-19.

| Ca | Mong đợi | Thực tế (`make test-a-queue`) |
|---|---|---|
| FIFO rỗng: `dequeue`/`peek`; `enqueue(nullptr)`; `clear` hàng rỗng | `nullptr`, size 0, không lỗi | Đạt |
| Một người: thêm → lấy → rỗng → thêm lại | Lấy đúng người; sau khi rỗng thêm lại vẫn đúng (rear về `nullptr`) | Đạt |
| BN01 → BN02 → BN03, lấy BN01 rồi thêm BN04 | Thứ tự lấy BN01, BN02, BN03, BN04 | Đạt |
| `clear`/destructor rồi dùng lại | Chỉ xóa node, Patient còn nguyên | Đạt |
| Rò rỉ bộ nhớ (`leaks --atExit`) | 0 leak | 0 leaks for 0 total leaked bytes |

Phần ClinicRoom trong `test_a.cpp` gồm: phòng rỗng/`configure`; bật ưu tiên mức 0/1 vào heap, mức 2 vào FIFO; tắt ưu tiên tất cả vào FIFO; `startNext` ghi InService/phòng/giờ bắt đầu/giờ xong; phòng bận không gọi thêm và không đổi cấu hình; `finishIfDue` trước giờ, đúng giờ, gọi lần hai; `reset` rồi dùng lại; ví dụ 3 người của Leader (chờ 0/9/3 khi bật, 0/4/8 khi tắt).

- `make test-a` **chưa liên kết được** vì `src/PriorityHeap.cpp` của B còn TODO (thiếu 7 symbol `PriorityHeap::*`).
- Để kiểm tra logic của test, đã chạy tạm với một heap viết riêng ngoài repo (không commit): cả 9 nhóm ca đều PASS, UBSan không báo lỗi, `leaks` báo 0 leak. Khi B bàn giao heap thật, chạy lại `make test-a` để có kết quả chính thức.

## 4. Lỗi và điểm cần ghép

- Chờ `PriorityHeap` của B để chạy `make test-a` đầy đủ.
- AddressSanitizer của Apple clang 16 lỗi ngay khi khởi tạo trên macOS 26 (`CHECK failed: sanitizer_malloc_mac.inc`), không liên quan code; dùng `leaks` thay thế trên macOS, cần chạy ASan trên Linux/Windows nếu có.
- Chưa chạy trên Windows.

## 5. Thời gian/bộ nhớ

| Thao tác | Thời gian | Ghi chú |
|---|---|---|
| `enqueue`, `dequeue`, `peek`, `size`, `isEmpty` | O(1) | Có con trỏ `rear_` và biến `size_` |
| `clear`, destructor | O(n) | Chỉ giải phóng node |
| `ClinicRoom::enqueue`/`startNext` | O(1) FIFO, O(log n) heap | Heap tăng mảng O(n) |
| `finishIfDue`, getter | O(1) | |
