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
```

Trên macOS/Linux có thể chạy qua Makefile (đầu ra `build/test_b`):

```bash
make test-b
```

## 3. Kết quả kiểm tra

| Ca | Mong đợi | Thực tế (`test_b`) |
|---|---|---|
| Heap rỗng: `isEmpty`/`peek`/`pop`; `push(nullptr)` | `nullptr`, size 0; `push(nullptr)` ném `std::invalid_argument` | Đạt |
| Ca bắt buộc: `BN01` (1,1,1), `BN02` (0,2,2), `BN03` (1,1,3) | Thứ tự lấy `BN02 → BN01 → BN03` | Đạt |
| Hòa mức/giờ đến: `BN01` (1,1,1), `BN02` (0,5,2), `BN03` (1,1,3) | `BN02 → BN01 → BN03` (hòa xét ticket) | Đạt |
| Một phần tử | `peek`/`pop` trả đúng phần tử, sau đó rỗng | Đạt |
| Vượt sức chứa 16 (`grow`), `clear` và dùng lại | 25 phần tử vẫn đúng heap; `clear` về size 0, thêm lại vẫn đúng | Đạt |
| Thống kê ưu tiên: mức rỗng; chờ 4 và 8 phút; bỏ ca `startTime < 0` | Mức rỗng `startedCount = 0`; tổng 12, đếm 2, trung bình 6, max 8 | Đạt |
| Hai phòng snapshot (3,0) rồi (0,4) | Max phòng 1 = 3, phòng 2 = 4, max hệ thống = 4 (không phải 7) | Đạt |
| Biên `byRoom(-1)`/`byRoom(roomCount)`, `reset(-1)` | Ném `std::out_of_range` / `std::invalid_argument` | Đạt |
| `reset` đổi số phòng 2 → 1 | Mọi số liệu về 0, không giữ số liệu cũ | Đạt |
| Báo cáo `makeSortedWaitReport`: kho rỗng | `count = 0`, trả `nullptr` | Đạt |
| Báo cáo: hòa thời gian chờ, người đang khám, người chưa khám | `BN04 (đang khám) → BN02 → BN03 → BN01`; `BN05` chưa khám không có; kho giữ 5 người | Đạt |
| Biên dịch `-Wall -Wextra -Wpedantic` & `git diff --check` | 0 lỗi, 0 cảnh báo, không lỗi whitespace | Đạt |

Môi trường đã chạy:

| Hệ điều hành | Trình biên dịch | Lệnh | Kết quả |
|---|---|---|---|
| Windows | g++ MinGW-w64 (C++17) | lệnh `g++` ở mục 2 | Đạt (B tự chạy, 2026-11-19) |
| macOS 26.5 | Apple clang 16.0.0 | `make test-b`; thêm `-fsanitize=undefined` | Đạt, UBSan không báo lỗi (Leader chạy, 2026-11-19) |

## 4. Kiểm tra tích hợp với A, C và Leader

`Statistics.cpp` liên kết và chạy kiểm thử trực tiếp với `PatientStore` thật (C) và `Patient` (Leader) qua `store.head()` và `waitingTime()`.

Sau khi merge vào `develop`, Leader đã chạy trên macOS với mô-đun thật (không dùng test double):

- `make` build `build/clinic_simulator`: 0 cảnh báo.
- `make test-a`: Đạt, gồm ví dụ phòng khám 0/9/3 (bật ưu tiên) và 0/4/8 (tắt ưu tiên) dùng `PriorityHeap` thật.
- `make test-c`, `make test-leader`: Đạt, gồm 4 cấu hình 1/2 phòng × bật/tắt ưu tiên.

**Kiểm tra chéo (A chạy test B):** chờ A chạy `make test-b` và ghi kết quả vào đây.

## 5. Lỗi còn lại và việc chờ

Đã khắc phục góp ý review PR #7: lỗi cú pháp ở `Statistics.cpp`, duyệt kho qua `store.head()`, kiểm tra tràn dung lượng trong `grow()`, báo lỗi `nullptr` trong `push()`, kiểm tra chỉ số trong `byRoom()`, xóa comment TODO.

Leader chỉnh thêm khi ghép:

- Chuyển test từ `tests/support/test_b.cpp` về `tests/test_b.cpp` đúng phân công (`tests/support/` chỉ chứa test double); thêm target `make test-b`.
- Bổ sung đúng các ca bắt buộc trong phân công: `BN02 (0,2,2)`, một phần tử, trung bình 6, snapshot (3,0)/(0,4), người đang khám có trong báo cáo.
- Rút gọn `extractPatient` thành hàm thường vì `PatientStore::Node` chứa trực tiếp `Patient`.

Quy ước lỗi: `push(nullptr)`, `reset(<0)`, `byRoom`/`byPriority` ngoài phạm vi và `grow()` tràn đều ném ngoại lệ chuẩn. `ClinicRoom` đã tự bỏ qua `nullptr` trước khi gọi heap nên luồng mô phỏng không gặp ngoại lệ này.

Còn chờ: A kiểm tra chéo `test_b`.

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
