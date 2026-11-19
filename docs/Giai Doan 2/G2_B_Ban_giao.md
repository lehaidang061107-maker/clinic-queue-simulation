# Bàn giao Giai đoạn 2 — Thành viên B

**Phụ trách:** `src/PriorityHeap.cpp`, `src/Statistics.cpp`, `tests/test_b.cpp`.

## 1. Đã cài gì

- **PriorityHeap:**
  - `PriorityHeap` / `~PriorityHeap`: cấp phát mảng con trỏ động `Patient**` (dung lượng khởi tạo `16`) và thu hồi mảng ở hàm hủy, không `delete` đối tượng `Patient`.
  - `comesBefore`: so sánh tăng dần theo bộ khóa `(priority, arrivalTime, ticketNumber)`.
  - `push`: kiểm tra `patient == nullptr` (ném `std::invalid_argument`); khi đầy mảng gọi `grow()` có kiểm tra tràn số nguyên `capacity_ > INT_MAX / 2` (ném `std::overflow_error`) trước khi nhân đôi dung lượng; vun đống lên bằng `siftUp`.
  - `pop` / `peek` / `isEmpty` / `size`: trả về `nullptr` hoặc `0` khi rỗng; `pop` đưa phần tử cuối lên gốc và vun đống xuống bằng `siftDown`.
  - `clear`: đặt `size_ = 0`, giữ nguyên dung lượng mảng đã cấp phát để tái sử dụng.
- **Statistics:**
  - `reset`: kiểm tra `roomCount < 0` (ném `std::invalid_argument`), xóa sạch số liệu cũ của 3 mức ưu tiên và cấp phát lại mảng `RoomStats[roomCount]` với `roomId = index + 1`.
  - `recordStart` / `recordCompletion`: bỏ qua ca chưa bắt đầu khám (`startTime < 0`); cộng dồn `startedCount`, `totalWait` và cập nhật `maxWait` theo từng mức ưu tiên (giữ `startedCount = 0` khi chưa có dữ liệu để tránh chia cho 0).
  - `observeQueues`: cập nhật kỷ lục hàng chờ lớn nhất cho từng phòng và toàn hệ thống (`maxSystemWaiting`).
  - `byPriority` / `byRoom`: kiểm tra chỉ số hợp lệ trước khi truy xuất mảng (ném `std::out_of_range` nếu ngoài phạm vi).
- **Merge Sort (`makeSortedWaitReport`):** duyệt danh sách liên kết `PatientStore` qua `store.head()` và `node->next`, lọc bệnh nhân có `startTime >= 0`, tự cài đặt thuật toán Merge Sort sắp xếp `waitingTime` giảm dần, hòa thì `ticketNumber` tăng dần.
- **test_b:** bộ kiểm thử độc lập cho `PriorityHeap`, `Statistics` và `makeSortedWaitReport`; đã xóa các comment `TODO` cũ trong các tệp của B.

## 2. Cách chạy

Môi trường: Windows (MinGW-w64, GCC hỗ trợ C++17). Ngày chạy: 2026-11-19.

```bash
mkdir -p build
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude tests/test_b.cpp src/PriorityHeap.cpp src/Statistics.cpp src/PatientStore.cpp src/Patient.cpp -o build/test_b.exe
./build/test_b.exe

## 3. Kết quả kiểm tra

Môi trường: Windows, g++ (MinGW-w64, C++17). Ngày chạy: 2026-11-19.

| Ca | Mong đợi | Thực tế (`./build/test_b.exe`) |
|---|---|---|
| Heap rỗng: `isEmpty`/`peek`/`pop`; `push(nullptr)` | `nullptr`, size 0; `push(nullptr)` ném `std::invalid_argument` | Đạt |
| Thứ tự ưu tiên & hòa khóa: `BN01` (1,1,1), `BN02` (0,5,2), `BN03` (1,1,3) | Thứ tự lấy `BN02 → BN01 → BN03` | Đạt |
| Vượt sức chứa 16 (`grow`), `clear` và dùng lại | Tự nhân đôi lên 32 giữ đúng heap; `clear` về size 0, thêm lại vẫn đúng | Đạt |
| Thống kê ưu tiên: mức rỗng; chờ 4 và 8 phút; bỏ ca `startTime < 0` | Mức rỗng `startedCount = 0`; tổng chờ 12, đếm 2, max 8 | Đạt |
| Đỉnh hàng chờ, biên `byRoom(-1)`/`byRoom(roomCount)` và `reset` | Max hệ thống = 4; ngoài biên ném `std::out_of_range`; `reset` về 0 | Đạt |
| Báo cáo `Merge Sort` (`makeSortedWaitReport`): kho rỗng; hòa thời gian chờ | Kho rỗng `count = 0`, `nullptr`; xếp `BN04 → BN02 → BN03 → BN01` | Đạt |
| Biên dịch (`-Wall -Wextra -Wpedantic`) & `git diff --check` | 0 lỗi, 0 cảnh báo, không lỗi whitespace | Đạt, không cảnh báo |


4. Kiểm tra tích hợp với A, C và Leader
Statistics.cpp đã liên kết và chạy kiểm thử trực tiếp cùng PatientStore (thành viên C) và Patient (Leader) qua store.head() và waitingTime().

Sau khi merge PR vào main, Leader và thành viên A có thể liên kết ngay PriorityHeap.cpp và Statistics.cpp thật để chạy make test-leader, make test-a và FileIO::saveRunResults (thay thế các bản giả lập trước đó).

5. Lỗi còn lại và việc chờ
Đã khắc phục toàn bộ góp ý từ review PR #7: sửa lỗi cú pháp ở Statistics.cpp, đổi sang duyệt store.head(), thêm kiểm tra tràn dung lượng trong grow(), báo lỗi nullptr trong push(), kiểm tra giới hạn chỉ số trong byRoom() và xóa comment TODO.

Chưa chạy make test-leader và make test-a toàn cục trên nhánh riêng của B; chờ Leader merge vào main để kiểm tra tích hợp cả 4 thành viên.

## 6. Thời gian/bộ nhớ

| Thao tác | Thời gian | Ghi chú |
|---|---|---|
| `PriorityHeap::push` | O(log n) | Khi đầy mảng gọi `grow()` tốn O(n) thời gian, O(n) bộ nhớ tạm |
| `PriorityHeap::pop` | O(log n) | Vun đống từ gốc xuống (`siftDown`) |
| `PriorityHeap::peek`, `isEmpty`, `size`, `clear` | O(1) | `clear()` giữ dung lượng mảng, không xóa `Patient` |
| `PriorityHeap::~PriorityHeap` | O(1) | Chỉ `delete[] items_`, tổng bộ nhớ mảng O(n) |
| `Statistics::recordStart`, `recordCompletion`, `observeQueues` | O(1) | Cập nhật trực tiếp trên mảng |
| `Statistics::byPriority`, `byRoom`, getter | O(1) | Có kiểm tra giới hạn chỉ số |
| `Statistics::reset` | O(R) | Cấp phát mảng `RoomStats[R]` |
| `makeSortedWaitReport` | O(N + m log m) | Duyệt kho `N` nút; Merge Sort `m` ca đã khám tốn O(m) bộ nhớ phụ |
